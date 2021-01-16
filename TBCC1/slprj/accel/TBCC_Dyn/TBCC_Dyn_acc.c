#include "__cf_TBCC_Dyn.h"
#include <math.h>
#include "TBCC_Dyn_acc.h"
#include "TBCC_Dyn_acc_private.h"
#include <stdio.h>
#include "slexec_vm_simstruct_bridge.h"
#include "slexec_vm_zc_functions.h"
#include "slexec_vm_lookup_functions.h"
#include "slsv_diagnostic_codegen_c_api.h"
#include "simtarget/slSimTgtMdlrefSfcnBridge.h"
#include "simstruc.h"
#include "fixedpoint.h"
#define CodeFormat S-Function
#define AccDefine1 Accelerator_S-Function
#include "simtarget/slAccSfcnBridge.h"
extern void Ambient_TMATS_body ( double * y , const double * u , const
AmbientStruct * prm ) ; AmbientStruct ambientStruct ; extern void
Compressor_TMATS_body ( double * y , double * y1 , double * y2 , const double
* u , const double * Wcust , const double * FracWbld , const CompressorStruct
* prm ) ; CompressorStruct compressorStruct ; extern void Burner_TMATS_body (
double * , const double * , const BurnStruct * ) ; BurnStruct burnPrms ;
extern void Turbine_TMATS_body ( double * y , const double * u , const double
* CoolFlow , const TurbineStruct * prm ) ; TurbineStruct turbineStruct ;
extern void Nozzle_TMATS_body ( double * y , const double * u , const
NozzleStruct * prm ) ; NozzleStruct nozzleStruct ;
#ifndef __RTW_UTFREE__  
extern void * utMalloc ( size_t ) ; extern void utFree ( void * ) ;
#endif
boolean_T TBCC_Dyn_acc_rt_TDelayUpdateTailOrGrowBuf ( int_T * bufSzPtr ,
int_T * tailPtr , int_T * headPtr , int_T * lastPtr , real_T tMinusDelay ,
real_T * * tBufPtr , real_T * * uBufPtr , real_T * * xBufPtr , boolean_T
isfixedbuf , boolean_T istransportdelay , int_T * maxNewBufSzPtr ) { int_T
testIdx ; int_T tail = * tailPtr ; int_T bufSz = * bufSzPtr ; real_T * tBuf =
* tBufPtr ; real_T * xBuf = ( NULL ) ; int_T numBuffer = 2 ; if (
istransportdelay ) { numBuffer = 3 ; xBuf = * xBufPtr ; } testIdx = ( tail <
( bufSz - 1 ) ) ? ( tail + 1 ) : 0 ; if ( ( tMinusDelay <= tBuf [ testIdx ] )
&& ! isfixedbuf ) { int_T j ; real_T * tempT ; real_T * tempU ; real_T *
tempX = ( NULL ) ; real_T * uBuf = * uBufPtr ; int_T newBufSz = bufSz + 1024
; if ( newBufSz > * maxNewBufSzPtr ) { * maxNewBufSzPtr = newBufSz ; } tempU
= ( real_T * ) utMalloc ( numBuffer * newBufSz * sizeof ( real_T ) ) ; if (
tempU == ( NULL ) ) { return ( false ) ; } tempT = tempU + newBufSz ; if (
istransportdelay ) tempX = tempT + newBufSz ; for ( j = tail ; j < bufSz ; j
++ ) { tempT [ j - tail ] = tBuf [ j ] ; tempU [ j - tail ] = uBuf [ j ] ; if
( istransportdelay ) tempX [ j - tail ] = xBuf [ j ] ; } for ( j = 0 ; j <
tail ; j ++ ) { tempT [ j + bufSz - tail ] = tBuf [ j ] ; tempU [ j + bufSz -
tail ] = uBuf [ j ] ; if ( istransportdelay ) tempX [ j + bufSz - tail ] =
xBuf [ j ] ; } if ( * lastPtr > tail ) { * lastPtr -= tail ; } else { *
lastPtr += ( bufSz - tail ) ; } * tailPtr = 0 ; * headPtr = bufSz ; utFree (
uBuf ) ; * bufSzPtr = newBufSz ; * tBufPtr = tempT ; * uBufPtr = tempU ; if (
istransportdelay ) * xBufPtr = tempX ; } else { * tailPtr = testIdx ; }
return ( true ) ; } real_T TBCC_Dyn_acc_rt_TDelayInterpolate ( real_T
tMinusDelay , real_T tStart , real_T * tBuf , real_T * uBuf , int_T bufSz ,
int_T * lastIdx , int_T oldestIdx , int_T newIdx , real_T initOutput ,
boolean_T discrete , boolean_T minorStepAndTAtLastMajorOutput ) { int_T i ;
real_T yout , t1 , t2 , u1 , u2 ; if ( ( newIdx == 0 ) && ( oldestIdx == 0 )
&& ( tMinusDelay > tStart ) ) return initOutput ; if ( tMinusDelay <= tStart
) return initOutput ; if ( ( tMinusDelay <= tBuf [ oldestIdx ] ) ) { if (
discrete ) { return ( uBuf [ oldestIdx ] ) ; } else { int_T tempIdx =
oldestIdx + 1 ; if ( oldestIdx == bufSz - 1 ) tempIdx = 0 ; t1 = tBuf [
oldestIdx ] ; t2 = tBuf [ tempIdx ] ; u1 = uBuf [ oldestIdx ] ; u2 = uBuf [
tempIdx ] ; if ( t2 == t1 ) { if ( tMinusDelay >= t2 ) { yout = u2 ; } else {
yout = u1 ; } } else { real_T f1 = ( t2 - tMinusDelay ) / ( t2 - t1 ) ;
real_T f2 = 1.0 - f1 ; yout = f1 * u1 + f2 * u2 ; } return yout ; } } if (
minorStepAndTAtLastMajorOutput ) { if ( newIdx != 0 ) { if ( * lastIdx ==
newIdx ) { ( * lastIdx ) -- ; } newIdx -- ; } else { if ( * lastIdx == newIdx
) { * lastIdx = bufSz - 1 ; } newIdx = bufSz - 1 ; } } i = * lastIdx ; if (
tBuf [ i ] < tMinusDelay ) { while ( tBuf [ i ] < tMinusDelay ) { if ( i ==
newIdx ) break ; i = ( i < ( bufSz - 1 ) ) ? ( i + 1 ) : 0 ; } } else { while
( tBuf [ i ] >= tMinusDelay ) { i = ( i > 0 ) ? i - 1 : ( bufSz - 1 ) ; } i =
( i < ( bufSz - 1 ) ) ? ( i + 1 ) : 0 ; } * lastIdx = i ; if ( discrete ) {
double tempEps = ( DBL_EPSILON ) * 128.0 ; double localEps = tempEps *
muDoubleScalarAbs ( tBuf [ i ] ) ; if ( tempEps > localEps ) { localEps =
tempEps ; } localEps = localEps / 2.0 ; if ( tMinusDelay >= ( tBuf [ i ] -
localEps ) ) { yout = uBuf [ i ] ; } else { if ( i == 0 ) { yout = uBuf [
bufSz - 1 ] ; } else { yout = uBuf [ i - 1 ] ; } } } else { if ( i == 0 ) {
t1 = tBuf [ bufSz - 1 ] ; u1 = uBuf [ bufSz - 1 ] ; } else { t1 = tBuf [ i -
1 ] ; u1 = uBuf [ i - 1 ] ; } t2 = tBuf [ i ] ; u2 = uBuf [ i ] ; if ( t2 ==
t1 ) { if ( tMinusDelay >= t2 ) { yout = u2 ; } else { yout = u1 ; } } else {
real_T f1 = ( t2 - tMinusDelay ) / ( t2 - t1 ) ; real_T f2 = 1.0 - f1 ; yout
= f1 * u1 + f2 * u2 ; } } return ( yout ) ; } real_T look1_binlcapw ( real_T
u0 , const real_T bp0 [ ] , const real_T table [ ] , uint32_T maxIndex ) {
real_T y ; real_T frac ; uint32_T iRght ; uint32_T iLeft ; uint32_T bpIdx ;
if ( u0 <= bp0 [ 0U ] ) { iLeft = 0U ; frac = 0.0 ; } else if ( u0 < bp0 [
maxIndex ] ) { bpIdx = maxIndex >> 1U ; iLeft = 0U ; iRght = maxIndex ; while
( iRght - iLeft > 1U ) { if ( u0 < bp0 [ bpIdx ] ) { iRght = bpIdx ; } else {
iLeft = bpIdx ; } bpIdx = ( iRght + iLeft ) >> 1U ; } frac = ( u0 - bp0 [
iLeft ] ) / ( bp0 [ iLeft + 1U ] - bp0 [ iLeft ] ) ; } else { iLeft =
maxIndex ; frac = 0.0 ; } if ( iLeft == maxIndex ) { y = table [ iLeft ] ; }
else { y = ( table [ iLeft + 1U ] - table [ iLeft ] ) * frac + table [ iLeft
] ; } return y ; } void rt_ssGetBlockPath ( SimStruct * S , int_T sysIdx ,
int_T blkIdx , char_T * * path ) { _ssGetBlockPath ( S , sysIdx , blkIdx ,
path ) ; } void rt_ssSet_slErrMsg ( SimStruct * S , void * diag ) { if ( !
_ssIsErrorStatusAslErrMsg ( S ) ) { _ssSet_slErrMsg ( S , diag ) ; } else {
_ssDiscardDiagnostic ( S , diag ) ; } } void rt_ssReportDiagnosticAsWarning (
SimStruct * S , void * diag ) { _ssReportDiagnosticAsWarning ( S , diag ) ; }
real_T rt_urand_Upu32_Yd_f_pw_snf ( uint32_T * u ) { uint32_T lo ; uint32_T
hi ; lo = * u % 127773U * 16807U ; hi = * u / 127773U * 2836U ; if ( lo < hi
) { * u = 2147483647U - ( hi - lo ) ; } else { * u = lo - hi ; } return (
real_T ) * u * 4.6566128752457969E-10 ; } real_T rt_nrand_Upu32_Yd_f_pw_snf (
uint32_T * u ) { real_T y ; real_T sr ; real_T si ; do { sr = 2.0 *
rt_urand_Upu32_Yd_f_pw_snf ( u ) - 1.0 ; si = 2.0 *
rt_urand_Upu32_Yd_f_pw_snf ( u ) - 1.0 ; si = sr * sr + si * si ; } while (
si > 1.0 ) ; y = muDoubleScalarSqrt ( - 2.0 * muDoubleScalarLog ( si ) / si )
* sr ; return y ; } void TBCC_Dyn_NoiseGenerator_Init ( SimStruct * S ,
DW_NoiseGenerator_TBCC_Dyn_T * localDW , P_NoiseGenerator_TBCC_Dyn_T * localP
, X_NoiseGenerator_TBCC_Dyn_T * localX ) { uint32_T tseed ; int32_T r ;
int32_T t ; real_T tmp ; tmp = muDoubleScalarFloor ( localP -> P_4 ) ; if (
muDoubleScalarIsNaN ( tmp ) || muDoubleScalarIsInf ( tmp ) ) { tmp = 0.0 ; }
else { tmp = muDoubleScalarRem ( tmp , 4.294967296E+9 ) ; } tseed = tmp < 0.0
? ( uint32_T ) - ( int32_T ) ( uint32_T ) - tmp : ( uint32_T ) tmp ; r = (
int32_T ) ( tseed >> 16U ) ; t = ( int32_T ) ( tseed & 32768U ) ; tseed = ( (
( ( tseed - ( ( uint32_T ) r << 16U ) ) + t ) << 16U ) + t ) + r ; if ( tseed
< 1U ) { tseed = 1144108930U ; } else { if ( tseed > 2147483646U ) { tseed =
2147483646U ; } } localDW -> RandSeed = tseed ; localDW -> NextOutput =
rt_nrand_Upu32_Yd_f_pw_snf ( & localDW -> RandSeed ) * localP -> P_3 + localP
-> P_2 ; localX -> noisegeneratingfilter_CSTATE = localP -> P_9 ; } void
TBCC_Dyn_NoiseGenerator_Disable ( SimStruct * S ,
DW_NoiseGenerator_TBCC_Dyn_T * localDW ) { localDW -> NoiseGenerator_MODE =
false ; } void TBCC_Dyn_NoiseGenerator ( SimStruct * S , real_T rtu_Enable ,
real_T rtu_actual , real_T rtu_sensormaxrange , B_NoiseGenerator_TBCC_Dyn_T *
localB , DW_NoiseGenerator_TBCC_Dyn_T * localDW , P_NoiseGenerator_TBCC_Dyn_T
* localP , X_NoiseGenerator_TBCC_Dyn_T * localX ,
XDis_NoiseGenerator_TBCC_Dyn_T * localXdis ) { int32_T isHit ; real_T u0 ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && (
ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable > 0.0 ) { if ( ! localDW
-> NoiseGenerator_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S )
) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localXdis ->
noisegeneratingfilter_CSTATE = 0 ; localDW -> NoiseGenerator_MODE = true ; }
} else { if ( localDW -> NoiseGenerator_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; localXdis ->
noisegeneratingfilter_CSTATE = 1 ; TBCC_Dyn_NoiseGenerator_Disable ( S ,
localDW ) ; } } } if ( localDW -> NoiseGenerator_MODE ) { isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { u0 = rtu_actual /
rtu_sensormaxrange ; if ( u0 > localP -> P_0 ) { u0 = localP -> P_0 ; } else
{ if ( u0 < localP -> P_1 ) { u0 = localP -> P_1 ; } } localB -> B_0_3_0 = u0
* localDW -> NextOutput ; } localB -> B_0_4_0 = 0.0 ; localB -> B_0_4_0 +=
localP -> P_7 * localX -> noisegeneratingfilter_CSTATE ; localB -> B_0_4_0 +=
localP -> P_8 * localB -> B_0_3_0 ; localB -> B_0_5_0 = localP -> P_10 *
localB -> B_0_4_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC (
localDW -> NoiseGenerator_SubsysRanBC ) ; } } } void
TBCC_Dyn_NoiseGenerator_Update ( SimStruct * S , DW_NoiseGenerator_TBCC_Dyn_T
* localDW , P_NoiseGenerator_TBCC_Dyn_T * localP ) { int32_T isHit ; if (
localDW -> NoiseGenerator_MODE ) { isHit = ssIsSampleHit ( S , 2 , 0 ) ; if (
isHit != 0 ) { localDW -> NextOutput = rt_nrand_Upu32_Yd_f_pw_snf ( & localDW
-> RandSeed ) * localP -> P_3 + localP -> P_2 ; } } } void
TBCC_Dyn_NoiseGenerator_Deriv ( SimStruct * S , B_NoiseGenerator_TBCC_Dyn_T *
localB , DW_NoiseGenerator_TBCC_Dyn_T * localDW , P_NoiseGenerator_TBCC_Dyn_T
* localP , X_NoiseGenerator_TBCC_Dyn_T * localX ,
XDot_NoiseGenerator_TBCC_Dyn_T * localXdot ) { if ( localDW ->
NoiseGenerator_MODE ) { localXdot -> noisegeneratingfilter_CSTATE = 0.0 ;
localXdot -> noisegeneratingfilter_CSTATE += localP -> P_5 * localX ->
noisegeneratingfilter_CSTATE ; localXdot -> noisegeneratingfilter_CSTATE +=
localP -> P_6 * localB -> B_0_3_0 ; } else { localXdot ->
noisegeneratingfilter_CSTATE = 0.0 ; } } void TBCC_Dyn_NoiseGenerator_Term (
SimStruct * const S ) { } void TBCC_Dyn_IfActionSubsystem3_Enable ( SimStruct
* S ) { if ( ssGetTaskTime ( S , 0 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } } void
TBCC_Dyn_IfActionSubsystem3_Disable ( SimStruct * S ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } void
TBCC_Dyn_IfActionSubsystem3 ( SimStruct * S , real_T rtu_In1 , real_T *
rty_Out1 ) { * rty_Out1 = rtu_In1 ; } void TBCC_Dyn_IfActionSubsystem3_Term (
SimStruct * const S ) { } void TBCC_Dyn_IfActionSubsystem1_Init ( SimStruct *
S , P_IfActionSubsystem1_TBCC_Dyn_T * localP ,
X_IfActionSubsystem1_TBCC_Dyn_T * localX ) { localX -> StateSpace_CSTATE_k =
localP -> P_3 ; } void TBCC_Dyn_IfActionSubsystem1_Enable ( SimStruct * S ,
XDis_IfActionSubsystem1_TBCC_Dyn_T * localXdis ) { if ( ssGetTaskTime ( S , 0
) != ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
} localXdis -> StateSpace_CSTATE_k = 0 ; } void
TBCC_Dyn_IfActionSubsystem1_Disable ( SimStruct * S ,
XDis_IfActionSubsystem1_TBCC_Dyn_T * localXdis ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; localXdis ->
StateSpace_CSTATE_k = 1 ; } void TBCC_Dyn_IfActionSubsystem1 ( SimStruct * S
, real_T * rty_Out1 , P_IfActionSubsystem1_TBCC_Dyn_T * localP ,
X_IfActionSubsystem1_TBCC_Dyn_T * localX ) { * rty_Out1 = 0.0 ; * rty_Out1 +=
localP -> P_2 * localX -> StateSpace_CSTATE_k ; } void
TBCC_Dyn_IfActionSubsystem1_Deriv ( SimStruct * S , real_T rtu_In1 ,
P_IfActionSubsystem1_TBCC_Dyn_T * localP , X_IfActionSubsystem1_TBCC_Dyn_T *
localX , XDot_IfActionSubsystem1_TBCC_Dyn_T * localXdot ) { localXdot ->
StateSpace_CSTATE_k = 0.0 ; localXdot -> StateSpace_CSTATE_k += localP -> P_0
* localX -> StateSpace_CSTATE_k ; localXdot -> StateSpace_CSTATE_k += localP
-> P_1 * rtu_In1 ; } void TBCC_Dyn_IfActionSubsystem1_Term ( SimStruct *
const S ) { } static void mdlOutputs ( SimStruct * S , int_T tid ) { real_T
B_54_7_0 ; real_T B_54_49_0 ; real_T B_54_58_0 ; real_T B_54_59_0 ; real_T
B_54_60_0 ; real_T B_54_61_0 ; real_T B_54_62_0 ; real_T B_54_63_0 ; real_T
B_54_64_0 ; real_T B_54_65_0 ; real_T B_54_66_0 ; real_T B_54_67_0 ; real_T
B_54_68_0 ; real_T B_54_69_0 ; real_T B_54_70_0 ; real_T B_54_71_0 ; real_T
B_54_72_0 ; real_T B_54_73_0 ; real_T B_54_74_0 ; real_T B_54_75_0 ; real_T
B_54_76_0 ; real_T B_54_77_0 ; real_T B_54_78_0 ; real_T B_54_79_0 ; real_T
B_54_80_0 ; real_T B_54_81_0 ; real_T B_54_82_0 ; real_T B_54_83_0 ; real_T
B_54_84_0 ; real_T B_54_85_0 ; real_T B_54_86_0 ; real_T B_54_87_0 ; real_T
B_54_88_0 ; real_T B_54_89_0 ; real_T B_54_90_0 ; real_T B_54_91_0 ; real_T
B_54_92_0 ; real_T B_54_93_0 ; real_T B_54_94_0 ; real_T B_54_95_0 ; real_T
B_54_96_0 ; real_T B_54_97_0 ; real_T B_54_98_0 ; real_T B_54_99_0 ; real_T
B_54_100_0 ; real_T B_54_101_0 ; real_T B_54_102_0 ; real_T B_54_103_0 ;
real_T B_54_104_0 ; real_T B_54_105_0 ; real_T B_54_106_0 ; real_T B_54_107_0
; real_T B_54_108_0 ; real_T B_54_109_0 ; real_T B_54_110_0 [ 51 ] ; real_T
B_54_111_0 ; real_T B_54_112_0 ; real_T B_54_113_0 ; real_T B_54_114_0 ;
real_T B_54_115_0 ; real_T B_54_116_0 ; real_T B_54_117_0 ; real_T B_54_118_0
; real_T B_54_119_0 ; real_T B_54_120_0 ; real_T B_54_121_0 ; real_T
B_54_122_0 ; real_T B_54_123_0 ; real_T B_54_124_0 ; real_T B_54_125_0 ;
real_T B_54_126_0 ; real_T B_54_127_0 ; real_T B_54_128_0 ; real_T B_54_129_0
; real_T B_54_130_0 [ 48 ] ; real_T B_54_131_0 ; real_T B_54_132_0 ; real_T
B_54_133_0 ; real_T B_54_134_0 ; real_T B_54_135_0 ; real_T B_54_137_0 [ 3 ]
; real_T B_54_138_0 ; real_T B_54_220_0 ; real_T B_54_261_0 ; real_T
B_54_270_0 ; real_T B_54_271_0 ; real_T B_54_272_0 ; real_T B_54_273_0 ;
real_T B_54_274_0 ; real_T B_54_275_0 ; real_T B_54_276_0 ; real_T B_54_277_0
; real_T B_54_278_0 ; real_T B_54_279_0 ; real_T B_54_280_0 ; real_T
B_54_281_0 ; real_T B_54_282_0 ; real_T B_54_283_0 ; real_T B_54_284_0 ;
real_T B_54_285_0 ; real_T B_54_286_0 ; real_T B_54_287_0 ; real_T B_54_288_0
; real_T B_54_289_0 ; real_T B_54_290_0 ; real_T B_54_291_0 ; real_T
B_54_292_0 ; real_T B_54_293_0 ; real_T B_54_294_0 ; real_T B_54_295_0 ;
real_T B_54_296_0 ; real_T B_54_297_0 ; real_T B_54_298_0 ; real_T B_54_299_0
; real_T B_54_300_0 ; real_T B_54_301_0 ; real_T B_54_302_0 ; real_T
B_54_303_0 ; real_T B_54_304_0 ; real_T B_54_305_0 ; real_T B_54_306_0 ;
real_T B_54_307_0 ; real_T B_54_308_0 ; real_T B_54_309_0 ; real_T B_54_310_0
; real_T B_54_311_0 ; real_T B_54_312_0 ; real_T B_54_313_0 ; real_T
B_54_314_0 ; real_T B_54_315_0 ; real_T B_54_316_0 ; real_T B_54_317_0 ;
real_T B_54_318_0 ; real_T B_54_319_0 ; real_T B_54_320_0 ; real_T B_54_321_0
; real_T B_54_322_0 [ 54 ] ; real_T B_54_323_0 ; real_T B_54_324_0 ; real_T
B_54_325_0 ; real_T B_54_326_0 ; real_T B_54_327_0 ; real_T B_54_328_0 ;
real_T B_54_329_0 ; real_T B_54_330_0 ; real_T B_54_331_0 ; real_T B_54_332_0
; real_T B_54_333_0 ; real_T B_54_334_0 ; real_T B_54_335_0 ; real_T
B_54_336_0 ; real_T B_54_337_0 ; real_T B_54_338_0 ; real_T B_54_339_0 ;
real_T B_54_340_0 ; real_T B_54_341_0 ; real_T B_54_342_0 [ 51 ] ; real_T
B_54_343_0 ; real_T B_54_344_0 ; real_T B_54_345_0 ; real_T B_54_346_0 ;
real_T B_54_347_0 ; real_T B_54_349_0 [ 3 ] ; real_T B_54_350_0 ; real_T
B_52_30_0 ; real_T B_35_30_0 ; int32_T B_54_136_0 ; int32_T B_54_348_0 ;
boolean_T B_49_17_0 [ 3 ] ; boolean_T B_49_21_0 [ 3 ] ; boolean_T B_49_23_0 [
3 ] ; boolean_T B_32_17_0 [ 3 ] ; boolean_T B_32_21_0 [ 3 ] ; boolean_T
B_32_23_0 [ 3 ] ; real_T rtb_B_54_146_0 ; real_T rtb_B_54_149_0 ; real_T
rtb_B_54_154_0 ; real_T rtb_B_54_157_0 ; real_T rtb_B_54_159_0 ; real_T
rtb_B_54_161_0 ; real_T rtb_B_54_165_0 ; real_T rtb_B_54_166_0 ; real_T
rtb_B_54_169_0 ; real_T rtb_B_54_172_0 ; real_T rtb_B_54_175_0 ; real_T
rtb_B_54_177_0 ; real_T rtb_B_54_185_0 ; real_T rtb_B_54_195_0 ; real_T
rtb_B_54_200_0 ; real_T rtb_B_54_202_0 ; real_T rtb_B_54_203_0 ; int8_T
rtPrevAction ; int8_T rtAction ; real_T rtb_B_54_193_0 ; real_T
rtb_B_54_194_0 ; int32_T Iter ; real_T rtb_B_54_184_0 ; real_T rtb_B_54_198_0
; boolean_T rtb_B_51_5_0 ; boolean_T rtb_B_34_5_0 ; int32_T i ; real_T tmp [
3 ] ; real_T B_49_3_0_idx_0 ; real_T B_49_3_0_idx_1 ; real_T B_49_3_0_idx_2 ;
int32_T i_0 ; B_TBCC_Dyn_T * _rtB ; P_TBCC_Dyn_T * _rtP ; X_TBCC_Dyn_T * _rtX
; DW_TBCC_Dyn_T * _rtDW ; _rtDW = ( ( DW_TBCC_Dyn_T * ) ssGetRootDWork ( S )
) ; _rtX = ( ( X_TBCC_Dyn_T * ) ssGetContStates ( S ) ) ; _rtP = ( (
P_TBCC_Dyn_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( ( B_TBCC_Dyn_T * )
_ssGetModelBlockIO ( S ) ) ; _rtB -> B_54_0_0 = 0.0 ; _rtB -> B_54_0_0 +=
_rtP -> P_308 * _rtX -> TransferFcn_CSTATE ; i = ssIsSampleHit ( S , 3 , 0 )
; if ( i != 0 ) { _rtB -> B_54_1_0 = _rtP -> P_309 ; } rtPrevAction = _rtDW
-> If1_ActiveSubsystem ; if ( ssIsMajorTimeStep ( S ) != 0 ) { rtAction = (
int8_T ) ! ( _rtB -> B_54_0_0 > 0.0 ) ; _rtDW -> If1_ActiveSubsystem =
rtAction ; } else { rtAction = _rtDW -> If1_ActiveSubsystem ; } if (
rtPrevAction != rtAction ) { switch ( rtPrevAction ) { case 0 :
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; ( ( XDis_TBCC_Dyn_T * )
ssGetContStateDisabled ( S ) ) -> TransferFcn2_CSTATE = 1 ; break ; case 1 :
TBCC_Dyn_IfActionSubsystem3_Disable ( S ) ; break ; } } switch ( rtAction ) {
case 0 : if ( rtAction != rtPrevAction ) { if ( ssGetTaskTime ( S , 0 ) !=
ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } (
( XDis_TBCC_Dyn_T * ) ssGetContStateDisabled ( S ) ) -> TransferFcn2_CSTATE =
0 ; } _rtB -> B_54_5_0 = 0.0 ; _rtB -> B_54_5_0 += _rtP -> P_5 * _rtX ->
TransferFcn2_CSTATE ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC (
_rtDW -> IfActionSubsystem2_SubsysRanBC_a ) ; } break ; case 1 : if (
rtAction != rtPrevAction ) { TBCC_Dyn_IfActionSubsystem3_Enable ( S ) ; }
TBCC_Dyn_IfActionSubsystem3 ( S , _rtB -> B_54_1_0 , & _rtB -> B_54_5_0 ) ;
if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
IfActionSubsystem3 . IfActionSubsystem3_SubsysRanBC ) ; } break ; } B_54_7_0
= rt_Lookup ( _rtP -> P_310 , 7 , ssGetT ( S ) , _rtP -> P_311 ) ;
rtPrevAction = _rtDW -> If2_ActiveSubsystem ; if ( ssIsMajorTimeStep ( S ) !=
0 ) { rtAction = ( int8_T ) ! ( _rtB -> B_54_5_0 > 0.0 ) ; _rtDW ->
If2_ActiveSubsystem = rtAction ; } else { rtAction = _rtDW ->
If2_ActiveSubsystem ; } if ( rtPrevAction != rtAction ) { switch (
rtPrevAction ) { case 0 : ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
( void ) memset ( & ( ( ( XDis_TBCC_Dyn_T * ) ssGetContStateDisabled ( S ) )
-> StateSpace_CSTATE ) , 1 , 5 * sizeof ( boolean_T ) ) ; switch ( _rtDW ->
If3_ActiveSubsystem_i ) { case 0 : ssSetBlockStateForSolverChangedAtMajorStep
( S ) ; ( void ) memset ( & ( ( ( XDis_TBCC_Dyn_T * ) ssGetContStateDisabled
( S ) ) -> StateSpace_CSTATE_m ) , 1 , 3 * sizeof ( boolean_T ) ) ; switch (
_rtDW -> If3_ActiveSubsystem_l ) { case 0 :
TBCC_Dyn_IfActionSubsystem1_Disable ( S , & ( ( XDis_TBCC_Dyn_T * )
ssGetContStateDisabled ( S ) ) -> IfActionSubsystem1_l ) ; break ; case 1 :
TBCC_Dyn_IfActionSubsystem1_Disable ( S , & ( ( XDis_TBCC_Dyn_T * )
ssGetContStateDisabled ( S ) ) -> IfActionSubsystem2_or ) ; break ; } _rtDW
-> If3_ActiveSubsystem_l = - 1 ; break ; case 1 :
TBCC_Dyn_IfActionSubsystem1_Disable ( S , & ( ( XDis_TBCC_Dyn_T * )
ssGetContStateDisabled ( S ) ) -> IfActionSubsystem2_o ) ; break ; } _rtDW ->
If3_ActiveSubsystem_i = - 1 ; break ; case 1 :
TBCC_Dyn_IfActionSubsystem3_Disable ( S ) ; break ; } } switch ( rtAction ) {
case 0 : if ( rtAction != rtPrevAction ) { if ( ssGetTaskTime ( S , 0 ) !=
ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } (
void ) memset ( & ( ( ( XDis_TBCC_Dyn_T * ) ssGetContStateDisabled ( S ) ) ->
StateSpace_CSTATE ) , 0 , 5 * sizeof ( boolean_T ) ) ; } _rtB -> B_15_0_0 =
0.0 ; _rtB -> B_15_0_0 += _rtP -> P_24 * _rtX -> StateSpace_CSTATE ;
ssCallAccelRunBlock ( S , 15 , 1 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 15 , 2 , SS_CALL_MDL_OUTPUTS ) ; rtPrevAction =
_rtDW -> If3_ActiveSubsystem_i ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
rtAction = ( int8_T ) ! ( _rtB -> B_15_0_0 <= 0.0 ) ; _rtDW ->
If3_ActiveSubsystem_i = rtAction ; } else { rtAction = _rtDW ->
If3_ActiveSubsystem_i ; } if ( rtPrevAction != rtAction ) { switch (
rtPrevAction ) { case 0 : ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
( void ) memset ( & ( ( ( XDis_TBCC_Dyn_T * ) ssGetContStateDisabled ( S ) )
-> StateSpace_CSTATE_m ) , 1 , 3 * sizeof ( boolean_T ) ) ; switch ( _rtDW ->
If3_ActiveSubsystem_l ) { case 0 : TBCC_Dyn_IfActionSubsystem1_Disable ( S ,
& ( ( XDis_TBCC_Dyn_T * ) ssGetContStateDisabled ( S ) ) ->
IfActionSubsystem1_l ) ; break ; case 1 : TBCC_Dyn_IfActionSubsystem1_Disable
( S , & ( ( XDis_TBCC_Dyn_T * ) ssGetContStateDisabled ( S ) ) ->
IfActionSubsystem2_or ) ; break ; } _rtDW -> If3_ActiveSubsystem_l = - 1 ;
break ; case 1 : TBCC_Dyn_IfActionSubsystem1_Disable ( S , & ( (
XDis_TBCC_Dyn_T * ) ssGetContStateDisabled ( S ) ) -> IfActionSubsystem2_o )
; break ; } } switch ( rtAction ) { case 0 : if ( rtAction != rtPrevAction )
{ if ( ssGetTaskTime ( S , 0 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } ( void ) memset ( & ( (
( XDis_TBCC_Dyn_T * ) ssGetContStateDisabled ( S ) ) -> StateSpace_CSTATE_m )
, 0 , 3 * sizeof ( boolean_T ) ) ; } _rtB -> B_13_0_0 = 0.0 ; _rtB ->
B_13_0_0 += _rtP -> P_18 * _rtX -> StateSpace_CSTATE_m ; rtPrevAction = _rtDW
-> If3_ActiveSubsystem_l ; if ( ssIsMajorTimeStep ( S ) != 0 ) { rtAction = (
int8_T ) ! ( _rtB -> B_13_0_0 <= 1.0 ) ; _rtDW -> If3_ActiveSubsystem_l =
rtAction ; } else { rtAction = _rtDW -> If3_ActiveSubsystem_l ; } if (
rtPrevAction != rtAction ) { switch ( rtPrevAction ) { case 0 :
TBCC_Dyn_IfActionSubsystem1_Disable ( S , & ( ( XDis_TBCC_Dyn_T * )
ssGetContStateDisabled ( S ) ) -> IfActionSubsystem1_l ) ; break ; case 1 :
TBCC_Dyn_IfActionSubsystem1_Disable ( S , & ( ( XDis_TBCC_Dyn_T * )
ssGetContStateDisabled ( S ) ) -> IfActionSubsystem2_or ) ; break ; } }
switch ( rtAction ) { case 0 : if ( rtAction != rtPrevAction ) {
TBCC_Dyn_IfActionSubsystem1_Enable ( S , & ( ( XDis_TBCC_Dyn_T * )
ssGetContStateDisabled ( S ) ) -> IfActionSubsystem1_l ) ; }
TBCC_Dyn_IfActionSubsystem1 ( S , & _rtB -> B_54_11_0 , & _rtP ->
IfActionSubsystem1_l , & _rtX -> IfActionSubsystem1_l ) ; if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW -> IfActionSubsystem1_l .
IfActionSubsystem1_SubsysRanBC ) ; } break ; case 1 : if ( rtAction !=
rtPrevAction ) { TBCC_Dyn_IfActionSubsystem1_Enable ( S , & ( (
XDis_TBCC_Dyn_T * ) ssGetContStateDisabled ( S ) ) -> IfActionSubsystem2_or )
; } TBCC_Dyn_IfActionSubsystem1 ( S , & _rtB -> B_54_11_0 , & _rtP ->
IfActionSubsystem2_or , & _rtX -> IfActionSubsystem2_or ) ; if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW -> IfActionSubsystem2_or
. IfActionSubsystem1_SubsysRanBC ) ; } break ; } _rtB -> B_13_6_0 = rt_Lookup
( _rtP -> P_20 , 7 , ssGetT ( S ) , _rtP -> P_21 ) ; if ( ssIsMajorTimeStep (
S ) != 0 ) { srUpdateBC ( _rtDW -> IfActionSubsystem1_SubsysRanBC_g ) ; }
break ; case 1 : if ( rtAction != rtPrevAction ) {
TBCC_Dyn_IfActionSubsystem1_Enable ( S , & ( ( XDis_TBCC_Dyn_T * )
ssGetContStateDisabled ( S ) ) -> IfActionSubsystem2_o ) ; }
TBCC_Dyn_IfActionSubsystem1 ( S , & _rtB -> B_54_11_0 , & _rtP ->
IfActionSubsystem2_o , & _rtX -> IfActionSubsystem2_o ) ; if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW -> IfActionSubsystem2_o .
IfActionSubsystem1_SubsysRanBC ) ; } break ; } _rtB -> B_15_8_0 = rt_Lookup (
_rtP -> P_27 , 7 , ssGetT ( S ) , _rtP -> P_28 ) ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_p . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_p . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP
-> P_29 ; _rtB -> B_15_9_0 = TBCC_Dyn_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_c .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_c . Last , _rtDW ->
TransportDelay_IWORK_c . Tail , _rtDW -> TransportDelay_IWORK_c . Head , _rtP
-> P_30 , 1 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } if ( ssIsMajorTimeStep
( S ) != 0 ) { srUpdateBC ( _rtDW -> IfActionSubsystem1_SubsysRanBC ) ; }
break ; case 1 : if ( rtAction != rtPrevAction ) {
TBCC_Dyn_IfActionSubsystem3_Enable ( S ) ; } TBCC_Dyn_IfActionSubsystem3 ( S
, B_54_7_0 , & _rtB -> B_54_11_0 ) ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
srUpdateBC ( _rtDW -> IfActionSubsystem4_f . IfActionSubsystem3_SubsysRanBC )
; } break ; } _rtB -> B_54_12_0 = _rtX -> Integrator_CSTATE ; i =
ssIsSampleHit ( S , 2 , 0 ) ; if ( i != 0 ) { _rtB -> B_54_13_0 = _rtDW ->
UnitDelay_DSTATE ; } _rtB -> B_54_14_0 = _rtB -> B_54_12_0 + _rtB ->
B_54_13_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Saturation_MODE =
_rtB -> B_54_14_0 >= _rtP -> P_315 ? 1 : _rtB -> B_54_14_0 > _rtP -> P_316 ?
0 : - 1 ; } _rtB -> B_54_15_0 = _rtDW -> Saturation_MODE == 1 ? _rtP -> P_315
: _rtDW -> Saturation_MODE == - 1 ? _rtP -> P_316 : _rtB -> B_54_14_0 ; _rtB
-> B_54_16_0 = _rtB -> B_54_11_0 - _rtB -> B_54_15_0 ; if ( _rtDW ->
Integrator_IWORK != 0 ) { _rtX -> Integrator_CSTATE_f = _rtB -> B_54_10_0 ; }
_rtB -> B_54_18_0 = _rtX -> Integrator_CSTATE_f ; _rtB -> B_54_19_0 = _rtP ->
P_317 * _rtB -> B_54_16_0 + _rtB -> B_54_18_0 ; _rtB -> B_54_21_0 = rt_Lookup
( _rtP -> P_318 , 7 , ssGetT ( S ) , _rtP -> P_319 ) ; _rtB -> B_54_23_0 =
rt_Lookup ( _rtP -> P_320 , 7 , ssGetT ( S ) , _rtP -> P_321 ) ; _rtB ->
B_54_25_0 = rt_Lookup ( _rtP -> P_322 , 7 , ssGetT ( S ) , _rtP -> P_323 ) ;
if ( _rtDW -> Integrator_IWORK_k != 0 ) { _rtX -> Integrator_CSTATE_i = _rtB
-> B_54_9_0 ; } _rtB -> B_54_26_0 = _rtX -> Integrator_CSTATE_i ; i =
ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { Iter = 1 ; do { _rtB ->
B_52_10_0 = Iter ; _rtB -> B_52_0_0 = _rtB -> B_54_25_0 ; _rtB -> B_52_1_0 =
_rtB -> B_54_26_0 ; _rtB -> B_52_2_0 = _rtB -> B_54_25_0 ; _rtB -> B_52_3_0 =
_rtB -> B_54_26_0 ; _rtB -> B_52_4_0 [ 0 ] = _rtB -> B_54_21_0 ; _rtB ->
B_52_4_0 [ 1 ] = _rtB -> B_52_17_0 ; _rtB -> B_52_4_0 [ 2 ] = _rtB ->
B_54_23_0 ; ambientStruct . AFARc = _rtP -> P_198 ; ambientStruct .
X_A_AltVec = ( double * ) ( _rtP -> P_191 ) ; ambientStruct . T_A_TsVec = (
double * ) ( _rtP -> P_192 ) ; ambientStruct . T_A_PsVec = ( double * ) (
_rtP -> P_193 ) ; ambientStruct . X_A_FARVec = ( double * ) ( _rtP -> P_194 )
; ambientStruct . T_A_RtArray = ( double * ) ( _rtP -> P_196 ) ;
ambientStruct . Y_A_TVec = ( double * ) ( _rtP -> P_195 ) ; ambientStruct .
T_A_gammaArray = ( double * ) ( _rtP -> P_197 ) ; ambientStruct . A = 15 ;
ambientStruct . B = 7 ; ambientStruct . C = 2 ; ambientStruct . IWork = &
_rtDW -> AmbientEnvtoEngine_IWORK . Errors [ 0 ] ; ambientStruct . BlkNm =
 "TBCC_Dyn/涡喷发动机结构模型/InnerLoopPlant/Engine/Ambient/Ambient Env to Engine"
; Ambient_TMATS_body ( ( real_T * ) & _rtB -> B_52_5_0 [ 0 ] , ( real_T * ) &
_rtB -> B_52_4_0 [ 0 ] , & ambientStruct ) ; _rtB -> B_52_6_0 = _rtB ->
B_54_25_0 * _rtB -> B_52_5_0 [ 6 ] / _rtB -> B_52_1_0_m ; _rtB -> B_52_9_0 =
_rtB -> B_52_5_0 [ 2 ] * _rtB -> B_52_23_0_l * look1_binlcapw ( _rtB ->
B_52_5_0 [ 2 ] / _rtB -> B_52_5_0 [ 4 ] , _rtP -> P_200 , _rtP -> P_199 , 1U
) ; _rtB -> B_52_11_0 = _rtB -> B_52_0_0_g ; i = ssIsSampleHit ( S , 1 , 0 )
; if ( i != 0 ) { if ( _rtB -> B_52_11_0 > 0.0 ) { if ( ! _rtDW ->
JCwReset_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } _rtDW -> JCwReset_MODE =
true ; } i_0 = _rtB -> B_52_10_0 ; if ( _rtB -> B_52_10_0 < 0 ) { i_0 = 0 ; }
_rtB -> B_42_1_0 [ 0 ] = _rtDW -> UnitDelay_DSTATE_c [ 0 ] ; _rtB -> B_42_1_0
[ 1 ] = _rtDW -> UnitDelay_DSTATE_c [ 1 ] ; _rtB -> B_42_1_0 [ 2 ] = _rtDW ->
UnitDelay_DSTATE_c [ 2 ] ; i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) {
_rtB -> B_41_4_0 = muDoubleScalarMin ( ( real_T ) ( ( uint32_T ) i_0 != _rtB
-> B_42_1_0_n ) * _rtB -> B_41_4_0_b , _rtB -> B_41_1_0 + _rtDW ->
UnitDelay_DSTATE_g ) ; _rtB -> B_41_5_0 = ( _rtB -> B_41_4_0_b > _rtB ->
B_41_4_0 ) ; _rtB -> B_41_6_0 = _rtB -> B_41_5_0 ; if ( _rtB -> B_41_6_0 ) {
if ( ! _rtDW -> Jacobian_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart
( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; }
ssCallAccelRunBlock ( S , 39 , 0 , SS_CALL_RTW_GENERATED_ENABLE ) ; _rtDW ->
Jacobian_MODE = true ; } _rtB -> B_40_0_0 [ 0 ] = _rtDW -> UnitDelay2_DSTATE
[ 0 ] ; _rtB -> B_40_0_0 [ 1 ] = _rtDW -> UnitDelay2_DSTATE [ 1 ] ; _rtB ->
B_40_0_0 [ 2 ] = _rtDW -> UnitDelay2_DSTATE [ 2 ] ; memcpy ( & _rtB ->
B_40_1_0 [ 0 ] , & _rtDW -> UnitDelay_DSTATE_kh [ 0 ] , 9U * sizeof ( real_T
) ) ; memcpy ( & _rtB -> B_40_2_0 [ 0 ] , & _rtDW -> UnitDelay1_DSTATE [ 0 ]
, 21U * sizeof ( real_T ) ) ; _rtB -> B_40_3_0 = _rtDW -> UnitDelay3_DSTATE ;
ssCallAccelRunBlock ( S , 39 , 0 , SS_CALL_MDL_OUTPUTS ) ; _rtDW ->
Jacobian_SubsysRanBC = 4 ; } else { if ( _rtDW -> Jacobian_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; ssCallAccelRunBlock ( S ,
39 , 0 , SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> Jacobian_MODE = false ; }
} } if ( _rtB -> B_41_5_0 ) { B_49_3_0_idx_0 = _rtB -> B_39_0_1 [ 0 ] ; }
else { B_49_3_0_idx_0 = _rtB -> B_42_1_0 [ 0 ] ; } _rtB -> B_42_4_0 [ 0 ] =
B_49_3_0_idx_0 * _rtB -> B_42_0_0 [ 0 ] ; if ( _rtB -> B_41_5_0 ) {
B_49_3_0_idx_0 = _rtB -> B_39_0_1 [ 1 ] ; } else { B_49_3_0_idx_0 = _rtB ->
B_42_1_0 [ 1 ] ; } _rtB -> B_42_4_0 [ 1 ] = B_49_3_0_idx_0 * _rtB -> B_42_0_0
[ 1 ] ; if ( _rtB -> B_41_5_0 ) { B_49_3_0_idx_0 = _rtB -> B_39_0_1 [ 2 ] ; }
else { B_49_3_0_idx_0 = _rtB -> B_42_1_0 [ 2 ] ; } _rtB -> B_42_4_0 [ 2 ] =
B_49_3_0_idx_0 * _rtB -> B_42_0_0 [ 2 ] ; _rtDW -> JCwReset_SubsysRanBC = 4 ;
} else { if ( _rtDW -> JCwReset_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; if ( _rtDW ->
Jacobian_MODE ) { ssCallAccelRunBlock ( S , 39 , 0 ,
SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> Jacobian_MODE = false ; } _rtDW ->
JCwReset_MODE = false ; } } } _rtB -> B_52_13_0 [ 0 ] = _rtP -> P_201 [ 0 ] ;
_rtB -> B_52_13_0 [ 1 ] = _rtP -> P_201 [ 1 ] ; _rtB -> B_52_13_0 [ 2 ] =
_rtP -> P_201 [ 2 ] ; _rtB -> B_52_23_0 [ 0 ] = _rtB -> B_54_25_0 ; _rtB ->
B_52_23_0 [ 1 ] = _rtB -> B_52_5_0 [ 0 ] ; _rtB -> B_52_23_0 [ 2 ] = _rtB ->
B_52_5_0 [ 1 ] ; _rtB -> B_52_23_0 [ 3 ] = _rtB -> B_52_9_0 ; _rtB ->
B_52_23_0 [ 4 ] = _rtB -> B_52_5_0 [ 3 ] ; _rtB -> B_52_23_0 [ 5 ] = _rtB ->
B_54_26_0 ; _rtB -> B_52_23_0 [ 6 ] = _rtB -> B_42_4_0 [ 1 ] ; _rtB ->
B_52_23_0 [ 7 ] = _rtB -> B_52_6_0_d ; _rtB -> B_52_23_0 [ 8 ] = _rtB ->
B_52_2_0_n ; _rtB -> B_52_23_0 [ 9 ] = ( _rtB -> B_52_13_0 [ 0 ] * _rtB ->
B_52_16_0 + _rtB -> B_52_12_0 ) * _rtB -> B_52_4_0_l ; _rtB -> B_52_23_0 [ 10
] = ( _rtB -> B_52_13_0 [ 1 ] * _rtB -> B_52_16_0 + _rtB -> B_52_13_0_g ) *
_rtB -> B_52_3_0_p ; _rtB -> B_52_23_0 [ 11 ] = ( _rtB -> B_52_13_0 [ 2 ] *
_rtB -> B_52_16_0 + _rtB -> B_52_14_0 ) * _rtB -> B_52_5_0_j ;
compressorStruct . NcDes = _rtP -> P_216 ; compressorStruct . PRDes = _rtP ->
P_218 ; compressorStruct . EffDes = _rtP -> P_217 ; compressorStruct .
RlineDes = _rtP -> P_219 ; compressorStruct . IDes = _rtP -> P_214 ;
compressorStruct . SMNEn = _rtP -> P_215 ; compressorStruct . CustBldEn =
_rtP -> P_220 ; compressorStruct . FBldEn = _rtP -> P_221 ; compressorStruct
. CustBldNm = _rtP -> P_222 ; compressorStruct . FracBldNm = _rtP -> P_223 ;
compressorStruct . Y_C_Map_NcVec = ( double * ) ( _rtP -> P_202 ) ;
compressorStruct . X_C_RlineVec = ( double * ) ( _rtP -> P_203 ) ;
compressorStruct . Z_C_AlphaVec = ( double * ) ( & _rtP -> P_204 ) ;
compressorStruct . T_C_Map_WcArray = ( double * ) ( _rtP -> P_205 ) ;
compressorStruct . T_C_Map_PRArray = ( double * ) ( _rtP -> P_206 ) ;
compressorStruct . T_C_Map_EffArray = ( double * ) ( _rtP -> P_207 ) ;
compressorStruct . FracCusBldht = ( double * ) ( & _rtP -> P_208 ) ;
compressorStruct . FracCusBldPt = ( double * ) ( & _rtP -> P_209 ) ;
compressorStruct . FracBldht = ( double * ) ( & _rtP -> P_210 ) ;
compressorStruct . FracBldPt = ( double * ) ( & _rtP -> P_211 ) ;
compressorStruct . X_C_Map_WcSurgeVec = ( double * ) ( _rtP -> P_212 ) ;
compressorStruct . T_C_Map_PRSurgeVec = ( double * ) ( _rtP -> P_213 ) ;
compressorStruct . A = 13 ; compressorStruct . B = 11 ; compressorStruct . C
= 1 ; compressorStruct . D = 14 ; compressorStruct . WcMapCol = _rtP -> P_224
; compressorStruct . PRMapCol = _rtP -> P_225 ; compressorStruct . EffMapCol
= _rtP -> P_226 ; compressorStruct . WcMapRw = _rtP -> P_227 ;
compressorStruct . PRMapRw = _rtP -> P_228 ; compressorStruct . EffMapRw =
_rtP -> P_229 ; compressorStruct . WcMapLay = _rtP -> P_230 ;
compressorStruct . PRMapLay = _rtP -> P_231 ; compressorStruct . EffMapLay =
_rtP -> P_232 ; compressorStruct . IWork = & _rtDW -> Compressor_IWORK .
Errors [ 0 ] ; compressorStruct . BlkNm =
"TBCC_Dyn/涡喷发动机结构模型/InnerLoopPlant/Engine/Compressor/Compressor" ;
Compressor_TMATS_body ( ( real_T * ) & _rtB -> B_52_24_0 [ 0 ] , ( real_T * )
& _rtB -> B_52_24_1 [ 0 ] , ( real_T * ) & _rtB -> B_52_24_2 [ 0 ] , ( real_T
* ) & _rtB -> B_52_23_0 [ 0 ] , ( real_T * ) & _rtB -> B_52_7_0 , ( real_T *
) & _rtB -> B_52_8_0 , & compressorStruct ) ; _rtB -> B_52_25_0 [ 0 ] = _rtB
-> B_54_19_0 ; for ( i = 0 ; i < 5 ; i ++ ) { _rtB -> B_52_25_0 [ i + 1 ] =
_rtB -> B_52_24_0 [ i ] ; } burnPrms . LHV = _rtP -> P_233 ; burnPrms .
dPnormBurner = _rtP -> P_234 ; burnPrms . Eff = _rtP -> P_235 ; burnPrms .
LHVEn = _rtP -> P_236 ; burnPrms . hFuel = _rtP -> P_237 ; Burner_TMATS_body
( ( real_T * ) & _rtB -> B_52_26_0 [ 0 ] , ( real_T * ) & _rtB -> B_52_25_0 [
0 ] , & burnPrms ) ; _rtB -> B_52_27_0 = _rtB -> B_52_11_0_o ; if ( _rtB ->
B_52_27_0 ) { _rtB -> B_36_0_0 [ 0 ] = TBCC_Dyn_rtC ( S ) -> B_36_0_0 ; _rtB
-> B_36_0_0 [ 1 ] = _rtB -> B_52_24_0 [ 8 ] ; _rtB -> B_36_0_0 [ 2 ] = _rtB
-> B_52_24_0 [ 9 ] ; _rtB -> B_36_0_0 [ 3 ] = _rtB -> B_52_24_0 [ 10 ] ; _rtB
-> B_36_0_0 [ 4 ] = _rtB -> B_52_24_0 [ 11 ] ; { FILE * fp = fopen (
"iDes_TBCC_Dyn_涡喷发动机结构模型_InnerLoopPlant_Engine_Compressor.bin" , "w"
) ; int i ; for ( i = 1 ; i <= 4 ; i ++ ) { double Val = _rtB -> B_36_0_0 [ i
] ; if ( Val >= 99999 ) Val = 99999 ; else if ( Val <= - 99999 ) Val = -
99999 ; fprintf ( fp , "%f\n" , Val ) ; } fclose ( fp ) ; } _rtDW ->
Scalar_To_Workspace_SubsysRanBC_n = 4 ; } memcpy ( & _rtB -> B_52_29_0 [ 0 ]
, & _rtP -> P_238 [ 0 ] , 51U * sizeof ( real_T ) ) ; B_52_30_0 = _rtP ->
P_239 ; _rtB -> B_52_31_0 [ 0 ] = _rtP -> P_240 [ 0 ] ; _rtB -> B_52_31_0 [ 1
] = _rtP -> P_240 [ 1 ] ; for ( i = 0 ; i < 5 ; i ++ ) { _rtB -> B_52_38_0 [
i ] = _rtB -> B_52_26_0 [ i ] ; } _rtB -> B_52_38_0 [ 5 ] = _rtB -> B_54_26_0
; _rtB -> B_52_38_0 [ 6 ] = _rtB -> B_42_4_0 [ 2 ] ; _rtB -> B_52_38_0 [ 7 ]
= _rtB -> B_52_29_0_d ; _rtB -> B_52_38_0 [ 8 ] = ( _rtB -> B_52_31_0 [ 0 ] *
_rtB -> B_52_39_0_o + _rtB -> B_52_36_0 ) * _rtB -> B_52_31_0_l ; _rtB ->
B_52_38_0 [ 9 ] = _rtB -> B_52_30_0 ; _rtB -> B_52_38_0 [ 10 ] = ( _rtB ->
B_52_31_0 [ 1 ] * _rtB -> B_52_39_0_o + _rtB -> B_52_37_0 ) * _rtB ->
B_52_32_0 ; _rtB -> B_52_38_0 [ 11 ] = TBCC_Dyn_rtC ( S ) -> B_52_3_0 ;
turbineStruct . NcDes = _rtP -> P_249 ; turbineStruct . PRmapDes = _rtP ->
P_251 ; turbineStruct . EffDes = _rtP -> P_250 ; turbineStruct . NDes = _rtP
-> P_252 ; turbineStruct . IDes = _rtP -> P_248 ; turbineStruct . BldPosLeng
= _rtP -> P_246 ; turbineStruct . CoolFlwEn = _rtP -> P_247 ; turbineStruct .
ConfigNPSS = _rtP -> P_257 ; turbineStruct . Y_T_NcVec = ( double * ) ( _rtP
-> P_241 ) ; turbineStruct . X_T_PRVec = ( double * ) ( _rtP -> P_242 ) ;
turbineStruct . T_T_Map_WcArray = ( double * ) ( _rtP -> P_243 ) ;
turbineStruct . T_T_Map_EffArray = ( double * ) ( _rtP -> P_244 ) ;
turbineStruct . T_BldPos = ( double * ) ( & _rtP -> P_245 ) ; turbineStruct .
A = 6 ; turbineStruct . B = 20 ; turbineStruct . WcMapCol = _rtP -> P_253 ;
turbineStruct . EffMapCol = _rtP -> P_254 ; turbineStruct . WcMapRw = _rtP ->
P_255 ; turbineStruct . EffMapRw = _rtP -> P_256 ; turbineStruct . IWork = &
_rtDW -> Turbine_IWORK . Errors [ 0 ] ; turbineStruct . BlkNm =
"TBCC_Dyn/涡喷发动机结构模型/InnerLoopPlant/Engine/Turbine/Turbine" ;
Turbine_TMATS_body ( ( real_T * ) & _rtB -> B_52_39_0 [ 0 ] , ( real_T * ) &
_rtB -> B_52_38_0 [ 0 ] , ( real_T * ) & _rtB -> B_52_24_2 [ 0 ] , &
turbineStruct ) ; _rtB -> B_52_40_0 = _rtB -> B_52_39_0 [ 3 ] * _rtB ->
B_52_22_0 ; _rtB -> B_52_41_0 [ 0 ] = _rtB -> B_52_39_0 [ 0 ] ; _rtB ->
B_52_41_0 [ 1 ] = _rtB -> B_52_39_0 [ 1 ] ; _rtB -> B_52_41_0 [ 2 ] = _rtB ->
B_52_39_0 [ 2 ] ; _rtB -> B_52_41_0 [ 3 ] = _rtB -> B_52_40_0 ; _rtB ->
B_52_41_0 [ 4 ] = _rtB -> B_52_39_0 [ 4 ] ; _rtB -> B_52_41_0 [ 5 ] = _rtB ->
B_52_5_0 [ 4 ] ; _rtB -> B_52_41_0 [ 6 ] = _rtB -> B_52_28_0 ; _rtB ->
B_52_41_0 [ 7 ] = _rtB -> B_52_24_0_d ; nozzleStruct . SwitchType = _rtP ->
P_258 ; nozzleStruct . flowLoss = _rtP -> P_259 ; nozzleStruct . IDes = _rtP
-> P_270 ; nozzleStruct . WDes = _rtP -> P_271 ; nozzleStruct . CfgEn = _rtP
-> P_272 ; nozzleStruct . Y_N_FARVec = ( double * ) ( _rtP -> P_260 ) ;
nozzleStruct . T_N_RtArray = ( double * ) ( _rtP -> P_261 ) ; nozzleStruct .
X_N_TtVec = ( double * ) ( _rtP -> P_262 ) ; nozzleStruct .
T_N_MAP_gammaArray = ( double * ) ( _rtP -> P_263 ) ; nozzleStruct .
X_N_PEQPaVec = ( double * ) ( _rtP -> P_264 ) ; nozzleStruct . T_N_CdThArray
= ( double * ) ( _rtP -> P_265 ) ; nozzleStruct . T_N_CvArray = ( double * )
( _rtP -> P_266 ) ; nozzleStruct . T_N_CfgArray = ( double * ) ( _rtP ->
P_267 ) ; nozzleStruct . T_N_TGArray = ( double * ) ( _rtP -> P_268 ) ;
nozzleStruct . X_N_TtVecTG = ( double * ) ( _rtP -> P_269 ) ; nozzleStruct .
A = 8 ; nozzleStruct . B = 18 ; nozzleStruct . B1 = 2 ; nozzleStruct . C = 18
; nozzleStruct . IWork = & _rtDW -> Nozzle_IWORK . Errors [ 0 ] ;
nozzleStruct . BlkNm =
"TBCC_Dyn/涡喷发动机结构模型/InnerLoopPlant/Engine/Nozzle/Nozzle" ;
Nozzle_TMATS_body ( ( real_T * ) & _rtB -> B_52_42_0 [ 0 ] , ( real_T * ) &
_rtB -> B_52_41_0 [ 0 ] , & nozzleStruct ) ; _rtB -> B_52_43_0 = _rtB ->
B_52_27_0_n ; if ( _rtB -> B_52_43_0 ) { _rtB -> B_37_0_0 [ 0 ] =
TBCC_Dyn_rtC ( S ) -> B_37_0_0 ; _rtB -> B_37_0_0 [ 1 ] = _rtB -> B_52_42_0 [
3 ] ; _rtB -> B_37_0_0 [ 2 ] = _rtB -> B_52_42_0 [ 4 ] ; { FILE * fp = fopen
( "iDes_TBCC_Dyn_涡喷发动机结构模型_InnerLoopPlant_Engine_Nozzle.bin" , "w" )
; int i ; for ( i = 1 ; i <= 2 ; i ++ ) { double Val = _rtB -> B_37_0_0 [ i ]
; if ( Val >= 99999 ) Val = 99999 ; else if ( Val <= - 99999 ) Val = - 99999
; fprintf ( fp , "%f\n" , Val ) ; } fclose ( fp ) ; } _rtDW ->
C_To_Workspace_SubsysRanBC = 4 ; } _rtB -> B_52_48_0 = ( _rtB -> B_52_19_0 /
_rtB -> B_54_26_0 * _rtP -> P_273 + ( _rtB -> B_52_24_0 [ 5 ] + _rtB ->
B_52_39_0 [ 5 ] ) ) * _rtP -> P_274 ; _rtB -> B_52_49_0 = _rtB -> B_52_35_0 ;
if ( _rtB -> B_52_49_0 ) { _rtB -> B_38_0_0 [ 0 ] = TBCC_Dyn_rtC ( S ) ->
B_38_0_0 ; _rtB -> B_38_0_0 [ 1 ] = _rtB -> B_52_39_0 [ 7 ] ; _rtB ->
B_38_0_0 [ 2 ] = _rtB -> B_52_39_0 [ 8 ] ; _rtB -> B_38_0_0 [ 3 ] = _rtB ->
B_52_39_0 [ 9 ] ; _rtB -> B_38_0_0 [ 4 ] = _rtB -> B_52_39_0 [ 10 ] ; { FILE
* fp = fopen (
"iDes_TBCC_Dyn_涡喷发动机结构模型_InnerLoopPlant_Engine_Turbine.bin" , "w" )
; int i ; for ( i = 1 ; i <= 4 ; i ++ ) { double Val = _rtB -> B_38_0_0 [ i ]
; if ( Val >= 99999 ) Val = 99999 ; else if ( Val <= - 99999 ) Val = - 99999
; fprintf ( fp , "%f\n" , Val ) ; } fclose ( fp ) ; } _rtDW ->
Scalar_To_Workspace_SubsysRanBC = 4 ; } memcpy ( & _rtB -> B_52_51_0 [ 0 ] ,
& _rtP -> P_275 [ 0 ] , 48U * sizeof ( real_T ) ) ; _rtB -> B_52_52_0 = _rtB
-> B_52_0_0_g ; i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { if ( _rtB
-> B_52_52_0 > 0.0 ) { if ( ! _rtDW -> NRwCond_MODE ) { if ( ssGetTaskTime (
S , 1 ) != ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep (
S ) ; } _rtDW -> NRwCond_MODE = true ; } _rtB -> B_51_3_0 = ( ( uint8_T ) ( (
( uint32_T ) ( muDoubleScalarAbs ( _rtB -> B_52_42_0 [ 2 ] ) > _rtB ->
B_51_0_0 ) + ( muDoubleScalarAbs ( _rtB -> B_52_39_0 [ 6 ] ) > _rtB ->
B_51_0_0 ) ) + ( muDoubleScalarAbs ( _rtB -> B_52_24_0 [ 6 ] ) > _rtB ->
B_51_0_0 ) ) > _rtB -> B_51_1_0 ) ; rtb_B_51_5_0 = ( ( ! _rtB -> B_41_5_0 )
&& _rtB -> B_51_3_0 ) ; i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) {
_rtB -> B_50_0_0 = rtb_B_51_5_0 ; _rtB -> B_50_1_0 [ 0 ] = _rtDW ->
UnitDelay_DSTATE_k [ 0 ] ; _rtB -> B_50_1_0 [ 1 ] = _rtDW ->
UnitDelay_DSTATE_k [ 1 ] ; _rtB -> B_50_1_0 [ 2 ] = _rtDW ->
UnitDelay_DSTATE_k [ 2 ] ; i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) {
if ( _rtB -> B_50_0_0 ) { if ( ! _rtDW -> NRUpdate_MODE ) { if (
ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } _rtDW -> NRUpdate_MODE =
true ; } B_49_3_0_idx_0 = muDoubleScalarAbs ( _rtB -> B_49_9_0 * _rtB ->
B_49_7_0 * ( _rtDW -> Memory_PreviousInput [ 0 ] + _rtB -> B_50_1_0 [ 0 ] ) )
; B_49_3_0_idx_1 = muDoubleScalarAbs ( _rtB -> B_49_9_0 * _rtB -> B_49_7_0 *
( _rtDW -> Memory_PreviousInput [ 1 ] + _rtB -> B_50_1_0 [ 1 ] ) ) ;
B_49_3_0_idx_2 = muDoubleScalarAbs ( _rtB -> B_49_9_0 * _rtB -> B_49_7_0 * (
_rtDW -> Memory_PreviousInput [ 2 ] + _rtB -> B_50_1_0 [ 2 ] ) ) ; for ( i_0
= 0 ; i_0 < 3 ; i_0 ++ ) { tmp [ i_0 ] = _rtB -> B_39_0_2 [ i_0 + 6 ] * _rtB
-> B_52_24_0 [ 6 ] + ( _rtB -> B_39_0_2 [ i_0 + 3 ] * _rtB -> B_52_39_0 [ 6 ]
+ _rtB -> B_39_0_2 [ i_0 ] * _rtB -> B_52_42_0 [ 2 ] ) ; } _rtB -> B_49_11_0
[ 0 ] = muDoubleScalarMin ( _rtB -> B_49_1_0 [ 0 ] , muDoubleScalarMax ( _rtB
-> B_50_1_0 [ 0 ] - muDoubleScalarMax ( _rtB -> B_49_8_0 * B_49_3_0_idx_0 ,
muDoubleScalarMin ( B_49_3_0_idx_0 , tmp [ 0 ] ) ) , _rtB -> B_49_3_0 [ 0 ] )
) ; _rtB -> B_49_16_0 [ 0 ] = ( ( _rtB -> B_49_1_0 [ 0 ] == _rtB -> B_49_11_0
[ 0 ] ) || ( _rtB -> B_49_11_0 [ 0 ] == _rtB -> B_49_3_0 [ 0 ] ) || _rtDW ->
Memory_PreviousInput_h [ 0 ] ) ; _rtB -> B_49_11_0 [ 1 ] = muDoubleScalarMin
( _rtB -> B_49_1_0 [ 1 ] , muDoubleScalarMax ( _rtB -> B_50_1_0 [ 1 ] -
muDoubleScalarMax ( _rtB -> B_49_8_0 * B_49_3_0_idx_1 , muDoubleScalarMin (
B_49_3_0_idx_1 , tmp [ 1 ] ) ) , _rtB -> B_49_3_0 [ 1 ] ) ) ; _rtB ->
B_49_16_0 [ 1 ] = ( ( _rtB -> B_49_1_0 [ 1 ] == _rtB -> B_49_11_0 [ 1 ] ) ||
( _rtB -> B_49_11_0 [ 1 ] == _rtB -> B_49_3_0 [ 1 ] ) || _rtDW ->
Memory_PreviousInput_h [ 1 ] ) ; _rtB -> B_49_11_0 [ 2 ] = muDoubleScalarMin
( _rtB -> B_49_1_0 [ 2 ] , muDoubleScalarMax ( _rtB -> B_50_1_0 [ 2 ] -
muDoubleScalarMax ( _rtB -> B_49_8_0 * B_49_3_0_idx_2 , muDoubleScalarMin (
B_49_3_0_idx_2 , tmp [ 2 ] ) ) , _rtB -> B_49_3_0 [ 2 ] ) ) ; _rtB ->
B_49_16_0 [ 2 ] = ( ( _rtB -> B_49_1_0 [ 2 ] == _rtB -> B_49_11_0 [ 2 ] ) ||
( _rtB -> B_49_11_0 [ 2 ] == _rtB -> B_49_3_0 [ 2 ] ) || _rtDW ->
Memory_PreviousInput_h [ 2 ] ) ; B_49_17_0 [ 0 ] = _rtB -> B_49_16_0 [ 0 ] ;
B_49_17_0 [ 1 ] = _rtB -> B_49_16_0 [ 1 ] ; B_49_17_0 [ 2 ] = _rtB ->
B_49_16_0 [ 2 ] ; if ( B_49_17_0 [ 0 ] || B_49_17_0 [ 1 ] || B_49_17_0 [ 2 ]
) { if ( ! _rtDW -> HitLimits_MODE ) { if ( ssGetTaskTime ( S , 1 ) !=
ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; }
ssCallAccelRunBlock ( S , 43 , 0 , SS_CALL_RTW_GENERATED_ENABLE ) ; _rtDW ->
HitLimits_MODE = true ; } ssCallAccelRunBlock ( S , 43 , 0 ,
SS_CALL_MDL_OUTPUTS ) ; _rtDW -> HitLimits_SubsysRanBC = 4 ; } else { if (
_rtDW -> HitLimits_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S )
; ssCallAccelRunBlock ( S , 43 , 0 , SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW
-> HitLimits_MODE = false ; } } _rtB -> B_49_20_0 [ 0 ] = ( _rtB -> B_49_5_0
[ 0 ] || _rtDW -> Memory1_PreviousInput [ 0 ] ) ; _rtB -> B_49_20_0 [ 1 ] = (
_rtB -> B_49_5_0 [ 1 ] || _rtDW -> Memory1_PreviousInput [ 1 ] ) ; _rtB ->
B_49_20_0 [ 2 ] = ( _rtB -> B_49_5_0 [ 2 ] || _rtDW -> Memory1_PreviousInput
[ 2 ] ) ; B_49_21_0 [ 0 ] = _rtB -> B_49_20_0 [ 0 ] ; B_49_21_0 [ 1 ] = _rtB
-> B_49_20_0 [ 1 ] ; B_49_21_0 [ 2 ] = _rtB -> B_49_20_0 [ 2 ] ; if (
B_49_21_0 [ 0 ] || B_49_21_0 [ 1 ] || B_49_21_0 [ 2 ] ) { if ( ! _rtDW ->
ICsOutsideLimit_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) )
{ ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } ssCallAccelRunBlock (
S , 45 , 0 , SS_CALL_RTW_GENERATED_ENABLE ) ; _rtDW -> ICsOutsideLimit_MODE =
true ; } ssCallAccelRunBlock ( S , 45 , 0 , SS_CALL_MDL_OUTPUTS ) ; _rtDW ->
ICsOutsideLimit_SubsysRanBC = 4 ; } else { if ( _rtDW -> ICsOutsideLimit_MODE
) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; ssCallAccelRunBlock (
S , 45 , 0 , SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> ICsOutsideLimit_MODE
= false ; } } B_49_23_0 [ 0 ] = _rtB -> B_49_6_0 [ 0 ] ; B_49_23_0 [ 1 ] =
_rtB -> B_49_6_0 [ 1 ] ; B_49_23_0 [ 2 ] = _rtB -> B_49_6_0 [ 2 ] ; if (
B_49_23_0 [ 0 ] || B_49_23_0 [ 1 ] || B_49_23_0 [ 2 ] ) { if ( ! _rtDW ->
ZeroIC_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } ssCallAccelRunBlock ( S
, 47 , 0 , SS_CALL_RTW_GENERATED_ENABLE ) ; _rtDW -> ZeroIC_MODE = true ; }
ssCallAccelRunBlock ( S , 47 , 0 , SS_CALL_MDL_OUTPUTS ) ; _rtDW ->
ZeroIC_SubsysRanBC = 4 ; } else { if ( _rtDW -> ZeroIC_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; ssCallAccelRunBlock ( S ,
47 , 0 , SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> ZeroIC_MODE = false ; } }
_rtDW -> NRUpdate_SubsysRanBC = 4 ; } else { if ( _rtDW -> NRUpdate_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; if ( _rtDW ->
HitLimits_MODE ) { ssCallAccelRunBlock ( S , 43 , 0 ,
SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> HitLimits_MODE = false ; } if (
_rtDW -> ICsOutsideLimit_MODE ) { ssCallAccelRunBlock ( S , 45 , 0 ,
SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> ICsOutsideLimit_MODE = false ; }
if ( _rtDW -> ZeroIC_MODE ) { ssCallAccelRunBlock ( S , 47 , 0 ,
SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> ZeroIC_MODE = false ; } _rtDW ->
NRUpdate_MODE = false ; } } } if ( rtb_B_51_5_0 ) { _rtB -> B_50_3_0 [ 0 ] =
_rtB -> B_49_11_0 [ 0 ] ; _rtB -> B_50_3_0 [ 1 ] = _rtB -> B_49_11_0 [ 1 ] ;
_rtB -> B_50_3_0 [ 2 ] = _rtB -> B_49_11_0 [ 2 ] ; } else { _rtB -> B_50_3_0
[ 0 ] = _rtB -> B_50_1_0 [ 0 ] ; _rtB -> B_50_3_0 [ 1 ] = _rtB -> B_50_1_0 [
1 ] ; _rtB -> B_50_3_0 [ 2 ] = _rtB -> B_50_1_0 [ 2 ] ; } } _rtDW ->
NRwCond_SubsysRanBC = 4 ; } else { if ( _rtDW -> NRwCond_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; if ( _rtDW ->
NRUpdate_MODE ) { if ( _rtDW -> HitLimits_MODE ) { ssCallAccelRunBlock ( S ,
43 , 0 , SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> HitLimits_MODE = false ;
} if ( _rtDW -> ICsOutsideLimit_MODE ) { ssCallAccelRunBlock ( S , 45 , 0 ,
SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> ICsOutsideLimit_MODE = false ; }
if ( _rtDW -> ZeroIC_MODE ) { ssCallAccelRunBlock ( S , 47 , 0 ,
SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> ZeroIC_MODE = false ; } _rtDW ->
NRUpdate_MODE = false ; } _rtDW -> NRwCond_MODE = false ; } } } i =
ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { if ( _rtDW -> JCwReset_MODE ) {
_rtDW -> UnitDelay_DSTATE_c [ 0 ] = _rtB -> B_50_3_0 [ 0 ] ; _rtDW ->
UnitDelay_DSTATE_c [ 1 ] = _rtB -> B_50_3_0 [ 1 ] ; _rtDW ->
UnitDelay_DSTATE_c [ 2 ] = _rtB -> B_50_3_0 [ 2 ] ; i = ssIsSampleHit ( S , 1
, 0 ) ; if ( i != 0 ) { _rtDW -> UnitDelay_DSTATE_g = _rtB -> B_41_4_0 ; if (
_rtDW -> Jacobian_MODE ) { _rtDW -> UnitDelay2_DSTATE [ 0 ] = _rtB ->
B_52_42_0 [ 2 ] ; _rtDW -> UnitDelay2_DSTATE [ 1 ] = _rtB -> B_52_39_0 [ 6 ]
; _rtDW -> UnitDelay2_DSTATE [ 2 ] = _rtB -> B_52_24_0 [ 6 ] ; memcpy ( &
_rtDW -> UnitDelay_DSTATE_kh [ 0 ] , & _rtB -> B_39_0_3 [ 0 ] , 9U * sizeof (
real_T ) ) ; memcpy ( & _rtDW -> UnitDelay1_DSTATE [ 0 ] , & _rtB -> B_39_0_4
[ 0 ] , 21U * sizeof ( real_T ) ) ; _rtDW -> UnitDelay3_DSTATE = _rtB ->
B_39_0_5 ; } } } if ( _rtDW -> NRwCond_MODE ) { i = ssIsSampleHit ( S , 1 , 0
) ; if ( i != 0 ) { _rtDW -> UnitDelay_DSTATE_k [ 0 ] = _rtB -> B_50_3_0 [ 0
] ; _rtDW -> UnitDelay_DSTATE_k [ 1 ] = _rtB -> B_50_3_0 [ 1 ] ; _rtDW ->
UnitDelay_DSTATE_k [ 2 ] = _rtB -> B_50_3_0 [ 2 ] ; i = ssIsSampleHit ( S , 1
, 0 ) ; if ( ( i != 0 ) && _rtDW -> NRUpdate_MODE ) { _rtDW ->
Memory_PreviousInput [ 0 ] = _rtB -> B_50_1_0 [ 0 ] ; _rtDW ->
Memory_PreviousInput_h [ 0 ] = _rtB -> B_49_16_0 [ 0 ] ; _rtDW ->
Memory1_PreviousInput [ 0 ] = _rtB -> B_49_20_0 [ 0 ] ; _rtDW ->
Memory_PreviousInput [ 1 ] = _rtB -> B_50_1_0 [ 1 ] ; _rtDW ->
Memory_PreviousInput_h [ 1 ] = _rtB -> B_49_16_0 [ 1 ] ; _rtDW ->
Memory1_PreviousInput [ 1 ] = _rtB -> B_49_20_0 [ 1 ] ; _rtDW ->
Memory_PreviousInput [ 2 ] = _rtB -> B_50_1_0 [ 2 ] ; _rtDW ->
Memory_PreviousInput_h [ 2 ] = _rtB -> B_49_16_0 [ 2 ] ; _rtDW ->
Memory1_PreviousInput [ 2 ] = _rtB -> B_49_20_0 [ 2 ] ; } } } } Iter ++ ; }
while ( ( _rtB -> B_52_0_0_g != 0.0 ) && _rtB -> B_51_3_0 && ( Iter <= 200 )
) ; } i = ssIsSampleHit ( S , 3 , 0 ) ; if ( i != 0 ) { _rtB -> B_54_28_0 =
_rtP -> P_324 ; } rtPrevAction = _rtDW -> If2_ActiveSubsystem_i ; if (
ssIsMajorTimeStep ( S ) != 0 ) { rtAction = ( int8_T ) ! ( _rtB -> B_54_0_0 >
0.0 ) ; _rtDW -> If2_ActiveSubsystem_i = rtAction ; } else { rtAction = _rtDW
-> If2_ActiveSubsystem_i ; } if ( rtPrevAction != rtAction ) { switch (
rtPrevAction ) { case 0 : ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
( ( XDis_TBCC_Dyn_T * ) ssGetContStateDisabled ( S ) ) -> TransferFcn1_CSTATE
= 1 ; break ; case 1 : TBCC_Dyn_IfActionSubsystem3_Disable ( S ) ; break ; }
} switch ( rtAction ) { case 0 : if ( rtAction != rtPrevAction ) { if (
ssGetTaskTime ( S , 0 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } ( ( XDis_TBCC_Dyn_T * )
ssGetContStateDisabled ( S ) ) -> TransferFcn1_CSTATE = 0 ; } _rtB ->
B_54_32_0 = 0.0 ; _rtB -> B_54_32_0 += _rtP -> P_7 * _rtX ->
TransferFcn1_CSTATE ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC (
_rtDW -> IfActionSubsystem1_SubsysRanBC_p ) ; } break ; case 1 : if (
rtAction != rtPrevAction ) { TBCC_Dyn_IfActionSubsystem3_Enable ( S ) ; }
TBCC_Dyn_IfActionSubsystem3 ( S , _rtB -> B_54_28_0 , & _rtB -> B_54_32_0 ) ;
if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
IfActionSubsystem4 . IfActionSubsystem3_SubsysRanBC ) ; } break ; } _rtB ->
B_54_33_0 = _rtB -> B_54_32_0 * _rtB -> B_54_0_0 ; _rtB -> B_54_34_0 = _rtB
-> B_52_42_0 [ 1 ] + _rtB -> B_54_33_0 ; _rtB -> B_54_35_0 = _rtB ->
B_54_19_0 + _rtB -> B_54_0_0 ; rtPrevAction = _rtDW -> If1_ActiveSubsystem_g
; if ( ssIsMajorTimeStep ( S ) != 0 ) { rtAction = ( int8_T ) ! ( _rtB ->
B_54_5_0 > 0.0 ) ; _rtDW -> If1_ActiveSubsystem_g = rtAction ; } else {
rtAction = _rtDW -> If1_ActiveSubsystem_g ; } if ( rtPrevAction != rtAction )
{ switch ( rtPrevAction ) { case 0 :
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; ( ( XDis_TBCC_Dyn_T * )
ssGetContStateDisabled ( S ) ) -> StateSpace_CSTATE_o = 1 ; break ; case 1 :
TBCC_Dyn_IfActionSubsystem3_Disable ( S ) ; break ; } } switch ( rtAction ) {
case 0 : if ( rtAction != rtPrevAction ) { if ( ssGetTaskTime ( S , 0 ) !=
ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } (
( XDis_TBCC_Dyn_T * ) ssGetContStateDisabled ( S ) ) -> StateSpace_CSTATE_o =
0 ; } _rtB -> B_54_39_0 = 0.0 ; _rtB -> B_54_39_0 += _rtP -> P_12 * _rtX ->
StateSpace_CSTATE_o ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_k . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_k . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_14 ; _rtB -> B_9_1_0
= TBCC_Dyn_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> TransportDelay_IWORK_b . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_b . Last , _rtDW -> TransportDelay_IWORK_b . Tail ,
_rtDW -> TransportDelay_IWORK_b . Head , _rtP -> P_15 , 1 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
IfActionSubsystem2_SubsysRanBC ) ; } break ; case 1 : if ( rtAction !=
rtPrevAction ) { TBCC_Dyn_IfActionSubsystem3_Enable ( S ) ; }
TBCC_Dyn_IfActionSubsystem3 ( S , _rtB -> B_54_23_0 , & _rtB -> B_54_39_0 ) ;
if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
IfActionSubsystem3_m . IfActionSubsystem3_SubsysRanBC ) ; } break ; } if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtB -> B_54_40_0 = _rtB -> B_54_39_0 ;
_rtDW -> Max_MODE = 0 ; if ( _rtB -> B_54_5_0 > _rtB -> B_54_39_0 ) { _rtB ->
B_54_40_0 = _rtB -> B_54_5_0 ; _rtDW -> Max_MODE = 1 ; } } else { _rtB ->
B_54_40_0 = _rtB -> B_54_39_0 ; if ( _rtDW -> Max_MODE == 1 ) { _rtB ->
B_54_40_0 = _rtB -> B_54_5_0 ; } } rtPrevAction = _rtDW ->
If3_ActiveSubsystem ; if ( ssIsMajorTimeStep ( S ) != 0 ) { rtAction = (
int8_T ) ! ( _rtB -> B_54_40_0 > 2.5 ) ; _rtDW -> If3_ActiveSubsystem =
rtAction ; } else { rtAction = _rtDW -> If3_ActiveSubsystem ; } if (
rtPrevAction != rtAction ) { switch ( rtPrevAction ) { case 0 :
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; break ; case 1 :
TBCC_Dyn_IfActionSubsystem3_Disable ( S ) ; break ; } } switch ( rtAction ) {
case 0 : if ( rtAction != rtPrevAction ) { if ( ssGetTaskTime ( S , 0 ) !=
ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } }
{ real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_31 ; _rtB -> B_17_0_0 =
TBCC_Dyn_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer
, _rtDW -> TransportDelay_IWORK . CircularBufSize , & _rtDW ->
TransportDelay_IWORK . Last , _rtDW -> TransportDelay_IWORK . Tail , _rtDW ->
TransportDelay_IWORK . Head , _rtP -> P_32 , 1 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
IfActionSubsystem6_SubsysRanBC ) ; } break ; case 1 : if ( rtAction !=
rtPrevAction ) { TBCC_Dyn_IfActionSubsystem3_Enable ( S ) ; }
TBCC_Dyn_IfActionSubsystem3 ( S , _rtB -> B_54_40_0 , & _rtB -> B_54_46_0 ) ;
if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
IfActionSubsystem5 . IfActionSubsystem3_SubsysRanBC ) ; } break ; } _rtB ->
B_54_44_0 = _rtB -> B_17_0_0 ; i = ssIsSampleHit ( S , 1 , 0 ) ; if ( ( i !=
0 ) && ( ssIsMajorTimeStep ( S ) != 0 ) ) { if ( _rtB -> B_54_44_0 > 0.0 ) {
if ( ! _rtDW -> EnabledSubsystem_MODE ) { if ( ssGetTaskTime ( S , 1 ) !=
ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } (
( XDis_TBCC_Dyn_T * ) ssGetContStateDisabled ( S ) ) -> StateSpace_CSTATE_e =
0 ; _rtDW -> EnabledSubsystem_MODE = true ; } } else { if ( _rtDW ->
EnabledSubsystem_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
( ( XDis_TBCC_Dyn_T * ) ssGetContStateDisabled ( S ) ) -> StateSpace_CSTATE_e
= 1 ; _rtDW -> EnabledSubsystem_MODE = false ; } } } if ( _rtDW ->
EnabledSubsystem_MODE ) { _rtB -> B_2_0_0 = 0.0 ; _rtB -> B_2_0_0 += _rtP ->
P_2 * _rtX -> StateSpace_CSTATE_e ; ssCallAccelRunBlock ( S , 2 , 1 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 2 , 2 , SS_CALL_MDL_OUTPUTS
) ; ssCallAccelRunBlock ( S , 2 , 3 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 2 , 4 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock
( S , 2 , 5 , SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_54_46_0 = _rtB -> B_2_4_0 [ 0
] ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
EnabledSubsystem_SubsysRanBC ) ; } } if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtB -> B_54_47_0 = _rtB -> B_54_46_0 ; _rtDW -> Max1_MODE = 0 ; if ( _rtB ->
B_54_40_0 > _rtB -> B_54_46_0 ) { _rtB -> B_54_47_0 = _rtB -> B_54_40_0 ;
_rtDW -> Max1_MODE = 1 ; } } else { _rtB -> B_54_47_0 = _rtB -> B_54_46_0 ;
if ( _rtDW -> Max1_MODE == 1 ) { _rtB -> B_54_47_0 = _rtB -> B_54_40_0 ; } }
ssCallAccelRunBlock ( S , 54 , 48 , SS_CALL_MDL_OUTPUTS ) ; i = ssIsSampleHit
( S , 2 , 0 ) ; if ( i != 0 ) { B_54_49_0 = _rtB -> B_54_26_0 ; _rtB ->
B_54_52_0 = ( B_54_49_0 * _rtB -> B_54_3_0 + _rtB -> B_54_2_0 ) * _rtP ->
P_327 ; } _rtB -> B_54_53_0 = _rtP -> P_328 * _rtB -> B_54_12_0 ; _rtB ->
B_54_54_0 = _rtB -> B_54_52_0 - _rtB -> B_54_53_0 ; i = ssIsSampleHit ( S , 1
, 0 ) ; if ( i != 0 ) { _rtB -> B_54_55_0 = _rtB -> B_54_0_0_m ; }
TBCC_Dyn_NoiseGenerator ( S , _rtB -> B_54_55_0 , B_54_49_0 , _rtB ->
B_54_1_0_c , & _rtB -> NoiseGenerator , & _rtDW -> NoiseGenerator , & _rtP ->
NoiseGenerator , & _rtX -> NoiseGenerator , & ( ( XDis_TBCC_Dyn_T * )
ssGetContStateDisabled ( S ) ) -> NoiseGenerator ) ; _rtB -> B_54_57_0 = _rtP
-> P_329 * _rtB -> B_54_16_0 ; i = ssIsSampleHit ( S , 2 , 0 ) ; if ( i != 0
) { B_54_58_0 = _rtB -> B_52_0_0 ; B_54_59_0 = _rtB -> B_52_5_0 [ 3 ] ;
B_54_60_0 = _rtB -> B_52_24_0 [ 0 ] ; B_54_61_0 = _rtB -> B_52_24_0 [ 1 ] ;
B_54_62_0 = _rtB -> B_52_24_0 [ 2 ] ; B_54_63_0 = _rtB -> B_52_24_0 [ 3 ] ;
B_54_64_0 = _rtB -> B_52_24_0 [ 4 ] ; B_54_65_0 = _rtB -> B_52_26_0 [ 0 ] ;
B_54_66_0 = _rtB -> B_52_26_0 [ 1 ] ; B_54_67_0 = _rtB -> B_52_26_0 [ 2 ] ;
B_54_68_0 = _rtB -> B_52_26_0 [ 3 ] ; B_54_69_0 = _rtB -> B_52_5_0 [ 0 ] ;
B_54_70_0 = _rtB -> B_52_26_0 [ 4 ] ; B_54_71_0 = _rtB -> B_52_39_0 [ 0 ] ;
B_54_72_0 = _rtB -> B_52_39_0 [ 1 ] ; B_54_73_0 = _rtB -> B_52_39_0 [ 2 ] ;
B_54_74_0 = _rtB -> B_52_39_0 [ 3 ] ; B_54_75_0 = _rtB -> B_52_39_0 [ 4 ] ;
B_54_76_0 = _rtB -> B_52_39_0 [ 0 ] ; B_54_77_0 = _rtB -> B_52_39_0 [ 1 ] ;
B_54_78_0 = _rtB -> B_52_39_0 [ 2 ] ; B_54_79_0 = _rtB -> B_52_40_0 ;
B_54_80_0 = _rtB -> B_52_5_0 [ 1 ] ; B_54_81_0 = _rtB -> B_52_39_0 [ 4 ] ;
B_54_82_0 = _rtB -> B_52_5_0 [ 4 ] ; B_54_83_0 = _rtB -> B_52_5_0 [ 5 ] ;
B_54_84_0 = _rtB -> B_52_5_0 [ 6 ] ; B_54_85_0 = _rtB -> B_52_6_0 ; B_54_86_0
= _rtB -> B_52_5_0 [ 7 ] ; B_54_87_0 = _rtB -> B_52_24_0 [ 7 ] ; B_54_88_0 =
_rtB -> B_52_24_0 [ 8 ] ; B_54_89_0 = _rtB -> B_52_24_0 [ 9 ] ; B_54_90_0 =
_rtB -> B_52_24_0 [ 10 ] ; B_54_91_0 = _rtB -> B_52_5_0 [ 2 ] ; B_54_92_0 =
_rtB -> B_52_24_0 [ 11 ] ; B_54_93_0 = _rtB -> B_52_24_0 [ 12 ] ; B_54_94_0 =
_rtB -> B_52_24_0 [ 13 ] ; B_54_95_0 = _rtB -> B_52_24_0 [ 14 ] ; B_54_96_0 =
_rtB -> B_52_24_0 [ 15 ] ; B_54_97_0 = _rtB -> B_52_24_0 [ 16 ] ; B_54_98_0 =
_rtB -> B_52_24_0 [ 17 ] ; B_54_99_0 = _rtB -> B_52_24_0 [ 18 ] ; B_54_100_0
= _rtB -> B_52_24_0 [ 19 ] ; B_54_101_0 = _rtB -> B_52_24_0 [ 20 ] ;
B_54_102_0 = _rtB -> B_52_5_0 [ 3 ] ; B_54_103_0 = _rtB -> B_52_24_0 [ 21 ] ;
B_54_104_0 = _rtB -> B_52_24_0 [ 22 ] ; B_54_105_0 = _rtB -> B_52_24_0 [ 23 ]
; B_54_106_0 = _rtB -> B_52_24_0 [ 24 ] ; B_54_107_0 = _rtB -> B_52_24_0 [ 25
] ; B_54_108_0 = _rtB -> B_52_24_0 [ 26 ] ; B_54_109_0 = _rtB -> B_52_1_0 ;
memcpy ( & B_54_110_0 [ 0 ] , & _rtB -> B_52_29_0 [ 0 ] , 51U * sizeof (
real_T ) ) ; B_54_111_0 = _rtB -> B_52_13_0 [ 0 ] ; B_54_112_0 = _rtB ->
B_52_13_0 [ 1 ] ; B_54_113_0 = _rtB -> B_52_2_0 ; B_54_114_0 = _rtB ->
B_52_13_0 [ 2 ] ; B_54_115_0 = _rtB -> B_52_39_0 [ 7 ] ; B_54_116_0 = _rtB ->
B_52_39_0 [ 8 ] ; B_54_117_0 = _rtB -> B_52_39_0 [ 9 ] ; B_54_118_0 = _rtB ->
B_52_39_0 [ 10 ] ; B_54_119_0 = _rtB -> B_52_39_0 [ 11 ] ; B_54_120_0 = _rtB
-> B_52_39_0 [ 12 ] ; B_54_121_0 = _rtB -> B_52_39_0 [ 13 ] ; B_54_122_0 =
_rtB -> B_52_39_0 [ 14 ] ; B_54_123_0 = _rtB -> B_52_39_0 [ 15 ] ; B_54_124_0
= _rtB -> B_52_5_0 [ 0 ] ; B_54_125_0 = _rtB -> B_52_39_0 [ 16 ] ; B_54_126_0
= _rtB -> B_52_39_0 [ 17 ] ; B_54_127_0 = _rtB -> B_52_39_0 [ 18 ] ;
B_54_128_0 = _rtB -> B_52_39_0 [ 19 ] ; B_54_129_0 = _rtB -> B_52_3_0 ;
memcpy ( & B_54_130_0 [ 0 ] , & _rtB -> B_52_51_0 [ 0 ] , 48U * sizeof (
real_T ) ) ; B_54_131_0 = _rtB -> B_52_31_0 [ 0 ] ; B_54_132_0 = _rtB ->
B_52_31_0 [ 1 ] ; B_54_133_0 = _rtB -> B_52_42_0 [ 1 ] ; B_54_134_0 = _rtB ->
B_52_48_0 ; B_54_135_0 = _rtB -> B_52_5_0 [ 1 ] ; B_54_136_0 = _rtB ->
B_52_10_0 ; B_54_137_0 [ 0 ] = _rtB -> B_52_42_0 [ 2 ] ; B_54_137_0 [ 1 ] =
_rtB -> B_52_39_0 [ 6 ] ; B_54_137_0 [ 2 ] = _rtB -> B_52_24_0 [ 6 ] ;
B_54_138_0 = _rtB -> B_52_9_0 ; } ssCallAccelRunBlock ( S , 54 , 139 ,
SS_CALL_MDL_OUTPUTS ) ; i = ssIsSampleHit ( S , 3 , 0 ) ; if ( i != 0 ) {
_rtB -> B_54_143_0 = _rtP -> P_332 + _rtP -> P_333 ; _rtB -> B_54_144_0 =
_rtP -> P_331 / _rtB -> B_54_143_0 ; rtb_B_54_146_0 = _rtP -> P_332 / ( _rtP
-> P_332 + _rtP -> P_334 ) ; if ( ( _rtB -> B_54_144_0 < 0.0 ) && (
rtb_B_54_146_0 > muDoubleScalarFloor ( rtb_B_54_146_0 ) ) ) { B_49_3_0_idx_0
= - muDoubleScalarPower ( - _rtB -> B_54_144_0 , rtb_B_54_146_0 ) ; } else {
B_49_3_0_idx_0 = muDoubleScalarPower ( _rtB -> B_54_144_0 , rtb_B_54_146_0 )
; } _rtB -> B_54_148_0 = _rtP -> P_330 * B_49_3_0_idx_0 ; } rtb_B_54_149_0 =
_rtB -> B_54_0_0 / _rtB -> B_54_148_0 ; i = ssIsSampleHit ( S , 3 , 0 ) ; if
( i != 0 ) { rtb_B_54_154_0 = _rtP -> P_337 ; rtb_B_54_157_0 = _rtP -> P_335
* _rtB -> B_54_144_0 * _rtP -> P_336 / ( _rtP -> P_337 * _rtP -> P_338 ) ; if
( ssIsMajorTimeStep ( S ) != 0 ) { if ( _rtDW -> Sqrt_DWORK1 != 0 ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; _rtDW -> Sqrt_DWORK1 = 0 ;
} _rtB -> B_54_158_0 = muDoubleScalarSqrt ( rtb_B_54_157_0 ) ; } else if (
rtb_B_54_157_0 < 0.0 ) { _rtB -> B_54_158_0 = - muDoubleScalarSqrt (
muDoubleScalarAbs ( rtb_B_54_157_0 ) ) ; _rtDW -> Sqrt_DWORK1 = 1 ; } else {
_rtB -> B_54_158_0 = muDoubleScalarSqrt ( rtb_B_54_157_0 ) ; } }
rtb_B_54_159_0 = rtb_B_54_149_0 * _rtB -> B_54_158_0 ; i = ssIsSampleHit ( S
, 3 , 0 ) ; if ( i != 0 ) { rtb_B_54_161_0 = rtb_B_54_154_0 + _rtP -> P_340 ;
_rtB -> B_54_163_0 = _rtP -> P_341 ; rtb_B_54_165_0 = _rtB -> B_54_163_0 /
_rtP -> P_342 ; rtb_B_54_166_0 = rtb_B_54_161_0 / rtb_B_54_154_0 ; if ( (
rtb_B_54_165_0 < 0.0 ) && ( rtb_B_54_166_0 > muDoubleScalarFloor (
rtb_B_54_166_0 ) ) ) { B_49_3_0_idx_0 = - muDoubleScalarPower ( -
rtb_B_54_165_0 , rtb_B_54_166_0 ) ; } else { B_49_3_0_idx_0 =
muDoubleScalarPower ( rtb_B_54_165_0 , rtb_B_54_166_0 ) ; } rtb_B_54_169_0 =
_rtP -> P_339 / rtb_B_54_161_0 * ( B_49_3_0_idx_0 + _rtP -> P_343 ) ; if (
ssIsMajorTimeStep ( S ) != 0 ) { if ( _rtDW -> Sqrt2_DWORK1 != 0 ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; _rtDW -> Sqrt2_DWORK1 = 0
; } _rtB -> B_54_170_0 = muDoubleScalarSqrt ( rtb_B_54_169_0 ) ; } else if (
rtb_B_54_169_0 < 0.0 ) { _rtB -> B_54_170_0 = - muDoubleScalarSqrt (
muDoubleScalarAbs ( rtb_B_54_169_0 ) ) ; _rtDW -> Sqrt2_DWORK1 = 1 ; } else {
_rtB -> B_54_170_0 = muDoubleScalarSqrt ( rtb_B_54_169_0 ) ; } } i =
ssIsSampleHit ( S , 3 , 0 ) ; if ( i != 0 ) { rtb_B_54_172_0 = rtb_B_54_154_0
+ _rtP -> P_344 ; rtb_B_54_175_0 = ( rtb_B_54_169_0 * rtb_B_54_161_0 + _rtP
-> P_345 ) * ( 1.0 / rtb_B_54_172_0 ) ; rtb_B_54_177_0 = rtb_B_54_172_0 / (
_rtP -> P_346 * rtb_B_54_161_0 ) ; if ( ( rtb_B_54_175_0 < 0.0 ) && (
rtb_B_54_177_0 > muDoubleScalarFloor ( rtb_B_54_177_0 ) ) ) { _rtB ->
B_54_178_0 = - muDoubleScalarPower ( - rtb_B_54_175_0 , rtb_B_54_177_0 ) ; }
else { _rtB -> B_54_178_0 = muDoubleScalarPower ( rtb_B_54_175_0 ,
rtb_B_54_177_0 ) ; } } _rtB -> B_54_180_0 = rtb_B_54_159_0 / _rtB ->
B_54_170_0 * _rtB -> B_54_178_0 * ( 1.0 / rtb_B_54_159_0 ) ;
ssCallAccelRunBlock ( S , 54 , 181 , SS_CALL_MDL_OUTPUTS ) ; i =
ssIsSampleHit ( S , 3 , 0 ) ; if ( i != 0 ) { rtb_B_54_184_0 = rtb_B_54_169_0
* rtb_B_54_161_0 * _rtP -> P_347 + _rtP -> P_348 ; rtb_B_54_185_0 = 1.0 /
rtb_B_54_161_0 * rtb_B_54_154_0 ; if ( ( rtb_B_54_184_0 < 0.0 ) && (
rtb_B_54_185_0 > muDoubleScalarFloor ( rtb_B_54_185_0 ) ) ) { B_49_3_0_idx_0
= - muDoubleScalarPower ( - rtb_B_54_184_0 , rtb_B_54_185_0 ) ; } else {
B_49_3_0_idx_0 = muDoubleScalarPower ( rtb_B_54_184_0 , rtb_B_54_185_0 ) ; }
_rtB -> B_54_187_0 = 1.0 / B_49_3_0_idx_0 * _rtB -> B_54_163_0 ;
ssCallAccelRunBlock ( S , 54 , 188 , SS_CALL_MDL_OUTPUTS ) ; }
ssCallAccelRunBlock ( S , 54 , 189 , SS_CALL_MDL_OUTPUTS ) ; i =
ssIsSampleHit ( S , 3 , 0 ) ; if ( i != 0 ) { ssCallAccelRunBlock ( S , 54 ,
190 , SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_54_191_0 = _rtP -> P_349 ; _rtB ->
B_54_192_0 = _rtP -> P_350 ; rtb_B_54_193_0 = _rtP -> P_351 ; rtb_B_54_194_0
= _rtP -> P_352 ; rtb_B_54_195_0 = _rtB -> B_54_187_0 / _rtB -> B_54_163_0 ;
if ( ( rtb_B_54_195_0 < 0.0 ) && ( rtb_B_54_166_0 > muDoubleScalarFloor (
rtb_B_54_166_0 ) ) ) { B_49_3_0_idx_0 = - muDoubleScalarPower ( -
rtb_B_54_195_0 , rtb_B_54_166_0 ) ; } else { B_49_3_0_idx_0 =
muDoubleScalarPower ( rtb_B_54_195_0 , rtb_B_54_166_0 ) ; } rtb_B_54_198_0 =
- B_49_3_0_idx_0 + _rtP -> P_353 ; } rtb_B_54_200_0 = _rtB -> B_54_191_0 *
rtb_B_54_159_0 / _rtB -> B_54_192_0 ; i = ssIsSampleHit ( S , 3 , 0 ) ; if (
i != 0 ) { rtb_B_54_202_0 = _rtP -> P_354 * rtb_B_54_185_0 * rtb_B_54_198_0 *
rtb_B_54_193_0 * rtb_B_54_194_0 ; } if ( ssIsMajorTimeStep ( S ) != 0 ) { if
( _rtDW -> Sqrt1_DWORK1 != 0 ) { ssSetBlockStateForSolverChangedAtMajorStep (
S ) ; _rtDW -> Sqrt1_DWORK1 = 0 ; } rtb_B_54_203_0 = muDoubleScalarSqrt (
rtb_B_54_200_0 ) ; } else if ( rtb_B_54_200_0 < 0.0 ) { rtb_B_54_203_0 = -
muDoubleScalarSqrt ( muDoubleScalarAbs ( rtb_B_54_200_0 ) ) ; _rtDW ->
Sqrt1_DWORK1 = 1 ; } else { rtb_B_54_203_0 = muDoubleScalarSqrt (
rtb_B_54_200_0 ) ; } i = ssIsSampleHit ( S , 3 , 0 ) ; if ( i != 0 ) { if (
ssIsMajorTimeStep ( S ) != 0 ) { if ( _rtDW -> Sqrt3_DWORK1 != 0 ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; _rtDW -> Sqrt3_DWORK1 = 0
; } _rtB -> B_54_204_0 = muDoubleScalarSqrt ( rtb_B_54_202_0 ) ; } else if (
rtb_B_54_202_0 < 0.0 ) { _rtB -> B_54_204_0 = - muDoubleScalarSqrt (
muDoubleScalarAbs ( rtb_B_54_202_0 ) ) ; _rtDW -> Sqrt3_DWORK1 = 1 ; } else {
_rtB -> B_54_204_0 = muDoubleScalarSqrt ( rtb_B_54_202_0 ) ; } _rtB ->
B_54_205_0 = _rtP -> P_355 ; _rtB -> B_54_206_0 = _rtP -> P_356 ; }
rtPrevAction = _rtDW -> If_ActiveSubsystem ; if ( ssIsMajorTimeStep ( S ) !=
0 ) { rtAction = ( int8_T ) ! ( _rtB -> B_54_23_0 >= 1.0 ) ; _rtDW ->
If_ActiveSubsystem = rtAction ; } else { rtAction = _rtDW ->
If_ActiveSubsystem ; } if ( rtPrevAction != rtAction ) { switch (
rtPrevAction ) { case 0 : ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
break ; case 1 : TBCC_Dyn_IfActionSubsystem3_Disable ( S ) ; break ; } }
switch ( rtAction ) { case 0 : if ( rtAction != rtPrevAction ) { if (
ssGetTaskTime ( S , 0 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } } { real_T * * uBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_h . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_8 ; if ( _rtP -> P_8 == 0.0 ) _rtB -> B_54_210_0 = _rtB
-> B_54_206_0 ; else _rtB -> B_54_210_0 = TBCC_Dyn_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_j .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_j . Last , _rtDW ->
TransportDelay_IWORK_j . Tail , _rtDW -> TransportDelay_IWORK_j . Head , _rtP
-> P_9 , 1 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } if ( ssIsMajorTimeStep
( S ) != 0 ) { srUpdateBC ( _rtDW -> IfActionSubsystem_SubsysRanBC ) ; }
break ; case 1 : if ( rtAction != rtPrevAction ) {
TBCC_Dyn_IfActionSubsystem3_Enable ( S ) ; } TBCC_Dyn_IfActionSubsystem3 ( S
, _rtB -> B_54_205_0 , & _rtB -> B_54_210_0 ) ; if ( ssIsMajorTimeStep ( S )
!= 0 ) { srUpdateBC ( _rtDW -> IfActionSubsystem1_c .
IfActionSubsystem3_SubsysRanBC ) ; } break ; } _rtB -> B_54_211_0 = _rtB ->
B_54_39_0 + _rtB -> B_54_5_0 ; ssCallAccelRunBlock ( S , 54 , 212 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 54 , 213 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 54 , 214 ,
SS_CALL_MDL_OUTPUTS ) ; i = ssIsSampleHit ( S , 3 , 0 ) ; if ( i != 0 ) {
_rtB -> B_54_215_0 = _rtP -> P_357 ; _rtB -> B_54_216_0 = _rtP -> P_358 ;
_rtB -> B_54_217_0 = _rtP -> P_359 ; } ssCallAccelRunBlock ( S , 54 , 218 ,
SS_CALL_MDL_OUTPUTS ) ; B_54_220_0 = rt_Lookup ( _rtP -> P_360 , 7 , ssGetT (
S ) , _rtP -> P_361 ) ; i = ssIsSampleHit ( S , 3 , 0 ) ; if ( i != 0 ) {
_rtB -> B_54_221_0 = _rtP -> P_362 ; _rtB -> B_54_222_0 = _rtB -> B_54_221_0
+ _rtP -> P_363 ; } _rtB -> B_54_223_0 = _rtB -> B_54_0_0 / _rtB ->
B_54_222_0 ; ssCallAccelRunBlock ( S , 54 , 224 , SS_CALL_MDL_OUTPUTS ) ;
_rtB -> B_54_226_0 = _rtB -> B_54_0_0 * _rtB -> B_54_221_0 / _rtB ->
B_54_222_0 ; ssCallAccelRunBlock ( S , 54 , 227 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 54 , 228 , SS_CALL_MDL_OUTPUTS ) ; i =
ssIsSampleHit ( S , 3 , 0 ) ; if ( i != 0 ) { ssCallAccelRunBlock ( S , 54 ,
229 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 54 , 230 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_54_231_0 = _rtP -> P_364 ; } _rtB ->
B_54_232_0 = rtb_B_54_159_0 * _rtB -> B_54_231_0 ; ssCallAccelRunBlock ( S ,
54 , 233 , SS_CALL_MDL_OUTPUTS ) ; i = ssIsSampleHit ( S , 3 , 0 ) ; if ( i
!= 0 ) { _rtB -> B_54_234_0 = _rtP -> P_365 ; } _rtB -> B_54_235_0 =
rtb_B_54_203_0 * _rtB -> B_54_234_0 ; ssCallAccelRunBlock ( S , 54 , 236 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_54_239_0 = muDoubleScalarPower ( _rtP ->
P_366 * _rtB -> B_54_235_0 , 2.0 ) * _rtP -> P_367 ; ssCallAccelRunBlock ( S
, 54 , 240 , SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_54_242_0 = _rtB -> B_54_232_0
/ ( _rtP -> P_368 * _rtB -> B_54_239_0 ) ; ssCallAccelRunBlock ( S , 54 , 243
, SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_54_244_0 = _rtX -> Integrator_CSTATE_fq ;
i = ssIsSampleHit ( S , 2 , 0 ) ; if ( i != 0 ) { _rtB -> B_54_245_0 = _rtDW
-> UnitDelay_DSTATE_p ; } _rtB -> B_54_246_0 = _rtB -> B_54_244_0 + _rtB ->
B_54_245_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Saturation_MODE_i
= _rtB -> B_54_246_0 >= _rtP -> P_371 ? 1 : _rtB -> B_54_246_0 > _rtP ->
P_372 ? 0 : - 1 ; } _rtB -> B_54_247_0 = _rtDW -> Saturation_MODE_i == 1 ?
_rtP -> P_371 : _rtDW -> Saturation_MODE_i == - 1 ? _rtP -> P_372 : _rtB ->
B_54_246_0 ; _rtB -> B_54_248_0 = 0.0 - _rtB -> B_54_247_0 ; if ( _rtDW ->
Integrator_IWORK_b != 0 ) { _rtX -> Integrator_CSTATE_c = _rtB -> B_54_18_0_f
; } _rtB -> B_54_250_0 = _rtX -> Integrator_CSTATE_c ; _rtB -> B_54_251_0 =
_rtP -> P_373 * _rtB -> B_54_248_0 + _rtB -> B_54_250_0 ; _rtB -> B_54_253_0
= rt_Lookup ( _rtP -> P_374 , 7 , ssGetT ( S ) , _rtP -> P_375 ) ; _rtB ->
B_54_255_0 = rt_Lookup ( _rtP -> P_376 , 7 , ssGetT ( S ) , _rtP -> P_377 ) ;
_rtB -> B_54_257_0 = rt_Lookup ( _rtP -> P_378 , 7 , ssGetT ( S ) , _rtP ->
P_379 ) ; if ( _rtDW -> Integrator_IWORK_e != 0 ) { _rtX ->
Integrator_CSTATE_p = _rtB -> B_54_17_0 ; } _rtB -> B_54_258_0 = _rtX ->
Integrator_CSTATE_p ; i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { Iter
= 1 ; do { _rtB -> B_35_10_0 = Iter ; _rtB -> B_35_0_0 = _rtB -> B_54_257_0 ;
_rtB -> B_35_1_0 = _rtB -> B_54_258_0 ; _rtB -> B_35_2_0 = _rtB -> B_54_257_0
; _rtB -> B_35_3_0 = _rtB -> B_54_258_0 ; _rtB -> B_35_4_0 [ 0 ] = _rtB ->
B_54_253_0 ; _rtB -> B_35_4_0 [ 1 ] = _rtB -> B_35_17_0 ; _rtB -> B_35_4_0 [
2 ] = _rtB -> B_54_255_0 ; ambientStruct . AFARc = _rtP -> P_61 ;
ambientStruct . X_A_AltVec = ( double * ) ( _rtP -> P_54 ) ; ambientStruct .
T_A_TsVec = ( double * ) ( _rtP -> P_55 ) ; ambientStruct . T_A_PsVec = (
double * ) ( _rtP -> P_56 ) ; ambientStruct . X_A_FARVec = ( double * ) (
_rtP -> P_57 ) ; ambientStruct . T_A_RtArray = ( double * ) ( _rtP -> P_59 )
; ambientStruct . Y_A_TVec = ( double * ) ( _rtP -> P_58 ) ; ambientStruct .
T_A_gammaArray = ( double * ) ( _rtP -> P_60 ) ; ambientStruct . A = 15 ;
ambientStruct . B = 7 ; ambientStruct . C = 2 ; ambientStruct . IWork = &
_rtDW -> AmbientEnvtoEngine_IWORK_m . Errors [ 0 ] ; ambientStruct . BlkNm =
 "TBCC_Dyn/涡喷发动机内部结构模型2/InnerLoopPlant/Engine/Ambient/Ambient Env to Engine"
; Ambient_TMATS_body ( ( real_T * ) & _rtB -> B_35_5_0 [ 0 ] , ( real_T * ) &
_rtB -> B_35_4_0 [ 0 ] , & ambientStruct ) ; _rtB -> B_35_6_0 = _rtB ->
B_54_257_0 * _rtB -> B_35_5_0 [ 6 ] / _rtB -> B_35_1_0_b ; _rtB -> B_35_9_0 =
_rtB -> B_35_5_0 [ 2 ] * _rtB -> B_35_23_0_j * look1_binlcapw ( _rtB ->
B_35_5_0 [ 2 ] / _rtB -> B_35_5_0 [ 4 ] , _rtP -> P_63 , _rtP -> P_62 , 1U )
; _rtB -> B_35_11_0 = _rtB -> B_35_0_0_n ; i = ssIsSampleHit ( S , 1 , 0 ) ;
if ( i != 0 ) { if ( _rtB -> B_35_11_0 > 0.0 ) { if ( ! _rtDW ->
JCwReset_MODE_b ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } _rtDW -> JCwReset_MODE_b
= true ; } i_0 = _rtB -> B_35_10_0 ; if ( _rtB -> B_35_10_0 < 0 ) { i_0 = 0 ;
} _rtB -> B_25_1_0 [ 0 ] = _rtDW -> UnitDelay_DSTATE_o [ 0 ] ; _rtB ->
B_25_1_0 [ 1 ] = _rtDW -> UnitDelay_DSTATE_o [ 1 ] ; _rtB -> B_25_1_0 [ 2 ] =
_rtDW -> UnitDelay_DSTATE_o [ 2 ] ; i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i
!= 0 ) { _rtB -> B_24_4_0 = muDoubleScalarMin ( ( real_T ) ( ( uint32_T ) i_0
!= _rtB -> B_25_1_0_i ) * _rtB -> B_24_4_0_o , _rtB -> B_24_1_0 + _rtDW ->
UnitDelay_DSTATE_j ) ; _rtB -> B_24_5_0 = ( _rtB -> B_24_4_0_o > _rtB ->
B_24_4_0 ) ; _rtB -> B_24_6_0 = _rtB -> B_24_5_0 ; if ( _rtB -> B_24_6_0 ) {
if ( ! _rtDW -> Jacobian_MODE_j ) { if ( ssGetTaskTime ( S , 1 ) !=
ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; }
ssCallAccelRunBlock ( S , 22 , 0 , SS_CALL_RTW_GENERATED_ENABLE ) ; _rtDW ->
Jacobian_MODE_j = true ; } _rtB -> B_23_0_0 [ 0 ] = _rtDW ->
UnitDelay2_DSTATE_i [ 0 ] ; _rtB -> B_23_0_0 [ 1 ] = _rtDW ->
UnitDelay2_DSTATE_i [ 1 ] ; _rtB -> B_23_0_0 [ 2 ] = _rtDW ->
UnitDelay2_DSTATE_i [ 2 ] ; memcpy ( & _rtB -> B_23_1_0 [ 0 ] , & _rtDW ->
UnitDelay_DSTATE_f [ 0 ] , 9U * sizeof ( real_T ) ) ; memcpy ( & _rtB ->
B_23_2_0 [ 0 ] , & _rtDW -> UnitDelay1_DSTATE_d [ 0 ] , 21U * sizeof ( real_T
) ) ; _rtB -> B_23_3_0 = _rtDW -> UnitDelay3_DSTATE_j ; ssCallAccelRunBlock (
S , 22 , 0 , SS_CALL_MDL_OUTPUTS ) ; _rtDW -> Jacobian_SubsysRanBC_c = 4 ; }
else { if ( _rtDW -> Jacobian_MODE_j ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; ssCallAccelRunBlock ( S ,
22 , 0 , SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> Jacobian_MODE_j = false ;
} } } if ( _rtB -> B_24_5_0 ) { B_49_3_0_idx_0 = _rtB -> B_22_0_1 [ 0 ] ; }
else { B_49_3_0_idx_0 = _rtB -> B_25_1_0 [ 0 ] ; } _rtB -> B_25_4_0 [ 0 ] =
B_49_3_0_idx_0 * _rtB -> B_25_0_0 [ 0 ] ; if ( _rtB -> B_24_5_0 ) {
B_49_3_0_idx_0 = _rtB -> B_22_0_1 [ 1 ] ; } else { B_49_3_0_idx_0 = _rtB ->
B_25_1_0 [ 1 ] ; } _rtB -> B_25_4_0 [ 1 ] = B_49_3_0_idx_0 * _rtB -> B_25_0_0
[ 1 ] ; if ( _rtB -> B_24_5_0 ) { B_49_3_0_idx_0 = _rtB -> B_22_0_1 [ 2 ] ; }
else { B_49_3_0_idx_0 = _rtB -> B_25_1_0 [ 2 ] ; } _rtB -> B_25_4_0 [ 2 ] =
B_49_3_0_idx_0 * _rtB -> B_25_0_0 [ 2 ] ; _rtDW -> JCwReset_SubsysRanBC_a = 4
; } else { if ( _rtDW -> JCwReset_MODE_b ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; if ( _rtDW ->
Jacobian_MODE_j ) { ssCallAccelRunBlock ( S , 22 , 0 ,
SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> Jacobian_MODE_j = false ; } _rtDW
-> JCwReset_MODE_b = false ; } } } _rtB -> B_35_13_0 [ 0 ] = _rtP -> P_64 [ 0
] ; _rtB -> B_35_13_0 [ 1 ] = _rtP -> P_64 [ 1 ] ; _rtB -> B_35_13_0 [ 2 ] =
_rtP -> P_64 [ 2 ] ; _rtB -> B_35_23_0 [ 0 ] = _rtB -> B_54_257_0 ; _rtB ->
B_35_23_0 [ 1 ] = _rtB -> B_35_5_0 [ 0 ] ; _rtB -> B_35_23_0 [ 2 ] = _rtB ->
B_35_5_0 [ 1 ] ; _rtB -> B_35_23_0 [ 3 ] = _rtB -> B_35_9_0 ; _rtB ->
B_35_23_0 [ 4 ] = _rtB -> B_35_5_0 [ 3 ] ; _rtB -> B_35_23_0 [ 5 ] = _rtB ->
B_54_258_0 ; _rtB -> B_35_23_0 [ 6 ] = _rtB -> B_25_4_0 [ 1 ] ; _rtB ->
B_35_23_0 [ 7 ] = _rtB -> B_35_6_0_e ; _rtB -> B_35_23_0 [ 8 ] = _rtB ->
B_35_2_0_l ; _rtB -> B_35_23_0 [ 9 ] = ( _rtB -> B_35_13_0 [ 0 ] * _rtB ->
B_35_16_0 + _rtB -> B_35_12_0 ) * _rtB -> B_35_4_0_b ; _rtB -> B_35_23_0 [ 10
] = ( _rtB -> B_35_13_0 [ 1 ] * _rtB -> B_35_16_0 + _rtB -> B_35_13_0_b ) *
_rtB -> B_35_3_0_h ; _rtB -> B_35_23_0 [ 11 ] = ( _rtB -> B_35_13_0 [ 2 ] *
_rtB -> B_35_16_0 + _rtB -> B_35_14_0 ) * _rtB -> B_35_5_0_d ;
compressorStruct . NcDes = _rtP -> P_79 ; compressorStruct . PRDes = _rtP ->
P_81 ; compressorStruct . EffDes = _rtP -> P_80 ; compressorStruct . RlineDes
= _rtP -> P_82 ; compressorStruct . IDes = _rtP -> P_77 ; compressorStruct .
SMNEn = _rtP -> P_78 ; compressorStruct . CustBldEn = _rtP -> P_83 ;
compressorStruct . FBldEn = _rtP -> P_84 ; compressorStruct . CustBldNm =
_rtP -> P_85 ; compressorStruct . FracBldNm = _rtP -> P_86 ; compressorStruct
. Y_C_Map_NcVec = ( double * ) ( _rtP -> P_65 ) ; compressorStruct .
X_C_RlineVec = ( double * ) ( _rtP -> P_66 ) ; compressorStruct .
Z_C_AlphaVec = ( double * ) ( & _rtP -> P_67 ) ; compressorStruct .
T_C_Map_WcArray = ( double * ) ( _rtP -> P_68 ) ; compressorStruct .
T_C_Map_PRArray = ( double * ) ( _rtP -> P_69 ) ; compressorStruct .
T_C_Map_EffArray = ( double * ) ( _rtP -> P_70 ) ; compressorStruct .
FracCusBldht = ( double * ) ( & _rtP -> P_71 ) ; compressorStruct .
FracCusBldPt = ( double * ) ( & _rtP -> P_72 ) ; compressorStruct . FracBldht
= ( double * ) ( & _rtP -> P_73 ) ; compressorStruct . FracBldPt = ( double *
) ( & _rtP -> P_74 ) ; compressorStruct . X_C_Map_WcSurgeVec = ( double * ) (
_rtP -> P_75 ) ; compressorStruct . T_C_Map_PRSurgeVec = ( double * ) ( _rtP
-> P_76 ) ; compressorStruct . A = 13 ; compressorStruct . B = 11 ;
compressorStruct . C = 1 ; compressorStruct . D = 14 ; compressorStruct .
WcMapCol = _rtP -> P_87 ; compressorStruct . PRMapCol = _rtP -> P_88 ;
compressorStruct . EffMapCol = _rtP -> P_89 ; compressorStruct . WcMapRw =
_rtP -> P_90 ; compressorStruct . PRMapRw = _rtP -> P_91 ; compressorStruct .
EffMapRw = _rtP -> P_92 ; compressorStruct . WcMapLay = _rtP -> P_93 ;
compressorStruct . PRMapLay = _rtP -> P_94 ; compressorStruct . EffMapLay =
_rtP -> P_95 ; compressorStruct . IWork = & _rtDW -> Compressor_IWORK_d .
Errors [ 0 ] ; compressorStruct . BlkNm =
"TBCC_Dyn/涡喷发动机内部结构模型2/InnerLoopPlant/Engine/Compressor/Compressor"
; Compressor_TMATS_body ( ( real_T * ) & _rtB -> B_35_24_0 [ 0 ] , ( real_T *
) & _rtB -> B_35_24_1 [ 0 ] , ( real_T * ) & _rtB -> B_35_24_2 [ 0 ] , (
real_T * ) & _rtB -> B_35_23_0 [ 0 ] , ( real_T * ) & _rtB -> B_35_7_0 , (
real_T * ) & _rtB -> B_35_8_0 , & compressorStruct ) ; _rtB -> B_35_25_0 [ 0
] = _rtB -> B_54_251_0 ; for ( i = 0 ; i < 5 ; i ++ ) { _rtB -> B_35_25_0 [ i
+ 1 ] = _rtB -> B_35_24_0 [ i ] ; } burnPrms . LHV = _rtP -> P_96 ; burnPrms
. dPnormBurner = _rtP -> P_97 ; burnPrms . Eff = _rtP -> P_98 ; burnPrms .
LHVEn = _rtP -> P_99 ; burnPrms . hFuel = _rtP -> P_100 ; Burner_TMATS_body (
( real_T * ) & _rtB -> B_35_26_0 [ 0 ] , ( real_T * ) & _rtB -> B_35_25_0 [ 0
] , & burnPrms ) ; _rtB -> B_35_27_0 = _rtB -> B_35_11_0_m ; if ( _rtB ->
B_35_27_0 ) { _rtB -> B_19_0_0 [ 0 ] = TBCC_Dyn_rtC ( S ) -> B_19_0_0 ; _rtB
-> B_19_0_0 [ 1 ] = _rtB -> B_35_24_0 [ 8 ] ; _rtB -> B_19_0_0 [ 2 ] = _rtB
-> B_35_24_0 [ 9 ] ; _rtB -> B_19_0_0 [ 3 ] = _rtB -> B_35_24_0 [ 10 ] ; _rtB
-> B_19_0_0 [ 4 ] = _rtB -> B_35_24_0 [ 11 ] ; { FILE * fp = fopen (
"iDes_TBCC_Dyn_涡喷发动机内部结构模型2_InnerLoopPlant_Engine_Compressor.bin"
, "w" ) ; int i ; for ( i = 1 ; i <= 4 ; i ++ ) { double Val = _rtB ->
B_19_0_0 [ i ] ; if ( Val >= 99999 ) Val = 99999 ; else if ( Val <= - 99999 )
Val = - 99999 ; fprintf ( fp , "%f\n" , Val ) ; } fclose ( fp ) ; } _rtDW ->
Scalar_To_Workspace_SubsysRanBC_j = 4 ; } memcpy ( & _rtB -> B_35_29_0 [ 0 ]
, & _rtP -> P_101 [ 0 ] , 54U * sizeof ( real_T ) ) ; B_35_30_0 = _rtP ->
P_102 ; _rtB -> B_35_31_0 [ 0 ] = _rtP -> P_103 [ 0 ] ; _rtB -> B_35_31_0 [ 1
] = _rtP -> P_103 [ 1 ] ; for ( i = 0 ; i < 5 ; i ++ ) { _rtB -> B_35_38_0 [
i ] = _rtB -> B_35_26_0 [ i ] ; } _rtB -> B_35_38_0 [ 5 ] = _rtB ->
B_54_258_0 ; _rtB -> B_35_38_0 [ 6 ] = _rtB -> B_25_4_0 [ 2 ] ; _rtB ->
B_35_38_0 [ 7 ] = _rtB -> B_35_29_0_a ; _rtB -> B_35_38_0 [ 8 ] = ( _rtB ->
B_35_31_0 [ 0 ] * _rtB -> B_35_39_0_j + _rtB -> B_35_36_0 ) * _rtB ->
B_35_31_0_j ; _rtB -> B_35_38_0 [ 9 ] = _rtB -> B_35_30_0 ; _rtB -> B_35_38_0
[ 10 ] = ( _rtB -> B_35_31_0 [ 1 ] * _rtB -> B_35_39_0_j + _rtB -> B_35_37_0
) * _rtB -> B_35_32_0 ; _rtB -> B_35_38_0 [ 11 ] = TBCC_Dyn_rtC ( S ) ->
B_35_3_0 ; turbineStruct . NcDes = _rtP -> P_112 ; turbineStruct . PRmapDes =
_rtP -> P_114 ; turbineStruct . EffDes = _rtP -> P_113 ; turbineStruct . NDes
= _rtP -> P_115 ; turbineStruct . IDes = _rtP -> P_111 ; turbineStruct .
BldPosLeng = _rtP -> P_109 ; turbineStruct . CoolFlwEn = _rtP -> P_110 ;
turbineStruct . ConfigNPSS = _rtP -> P_120 ; turbineStruct . Y_T_NcVec = (
double * ) ( _rtP -> P_104 ) ; turbineStruct . X_T_PRVec = ( double * ) (
_rtP -> P_105 ) ; turbineStruct . T_T_Map_WcArray = ( double * ) ( _rtP ->
P_106 ) ; turbineStruct . T_T_Map_EffArray = ( double * ) ( _rtP -> P_107 ) ;
turbineStruct . T_BldPos = ( double * ) ( & _rtP -> P_108 ) ; turbineStruct .
A = 6 ; turbineStruct . B = 20 ; turbineStruct . WcMapCol = _rtP -> P_116 ;
turbineStruct . EffMapCol = _rtP -> P_117 ; turbineStruct . WcMapRw = _rtP ->
P_118 ; turbineStruct . EffMapRw = _rtP -> P_119 ; turbineStruct . IWork = &
_rtDW -> Turbine_IWORK_m . Errors [ 0 ] ; turbineStruct . BlkNm =
"TBCC_Dyn/涡喷发动机内部结构模型2/InnerLoopPlant/Engine/Turbine/Turbine" ;
Turbine_TMATS_body ( ( real_T * ) & _rtB -> B_35_39_0 [ 0 ] , ( real_T * ) &
_rtB -> B_35_38_0 [ 0 ] , ( real_T * ) & _rtB -> B_35_24_2 [ 0 ] , &
turbineStruct ) ; _rtB -> B_35_40_0 = _rtB -> B_35_39_0 [ 3 ] * _rtB ->
B_35_22_0 ; _rtB -> B_35_41_0 [ 0 ] = _rtB -> B_35_39_0 [ 0 ] ; _rtB ->
B_35_41_0 [ 1 ] = _rtB -> B_35_39_0 [ 1 ] ; _rtB -> B_35_41_0 [ 2 ] = _rtB ->
B_35_39_0 [ 2 ] ; _rtB -> B_35_41_0 [ 3 ] = _rtB -> B_35_40_0 ; _rtB ->
B_35_41_0 [ 4 ] = _rtB -> B_35_39_0 [ 4 ] ; _rtB -> B_35_41_0 [ 5 ] = _rtB ->
B_35_5_0 [ 4 ] ; _rtB -> B_35_41_0 [ 6 ] = _rtB -> B_35_28_0 ; _rtB ->
B_35_41_0 [ 7 ] = _rtB -> B_35_24_0_f ; nozzleStruct . SwitchType = _rtP ->
P_121 ; nozzleStruct . flowLoss = _rtP -> P_122 ; nozzleStruct . IDes = _rtP
-> P_133 ; nozzleStruct . WDes = _rtP -> P_134 ; nozzleStruct . CfgEn = _rtP
-> P_135 ; nozzleStruct . Y_N_FARVec = ( double * ) ( _rtP -> P_123 ) ;
nozzleStruct . T_N_RtArray = ( double * ) ( _rtP -> P_124 ) ; nozzleStruct .
X_N_TtVec = ( double * ) ( _rtP -> P_125 ) ; nozzleStruct .
T_N_MAP_gammaArray = ( double * ) ( _rtP -> P_126 ) ; nozzleStruct .
X_N_PEQPaVec = ( double * ) ( _rtP -> P_127 ) ; nozzleStruct . T_N_CdThArray
= ( double * ) ( _rtP -> P_128 ) ; nozzleStruct . T_N_CvArray = ( double * )
( _rtP -> P_129 ) ; nozzleStruct . T_N_CfgArray = ( double * ) ( _rtP ->
P_130 ) ; nozzleStruct . T_N_TGArray = ( double * ) ( _rtP -> P_131 ) ;
nozzleStruct . X_N_TtVecTG = ( double * ) ( _rtP -> P_132 ) ; nozzleStruct .
A = 8 ; nozzleStruct . B = 18 ; nozzleStruct . B1 = 2 ; nozzleStruct . C = 18
; nozzleStruct . IWork = & _rtDW -> Nozzle_IWORK_d . Errors [ 0 ] ;
nozzleStruct . BlkNm =
"TBCC_Dyn/涡喷发动机内部结构模型2/InnerLoopPlant/Engine/Nozzle/Nozzle" ;
Nozzle_TMATS_body ( ( real_T * ) & _rtB -> B_35_42_0 [ 0 ] , ( real_T * ) &
_rtB -> B_35_41_0 [ 0 ] , & nozzleStruct ) ; _rtB -> B_35_43_0 = _rtB ->
B_35_27_0_c ; if ( _rtB -> B_35_43_0 ) { _rtB -> B_20_0_0 [ 0 ] =
TBCC_Dyn_rtC ( S ) -> B_20_0_0 ; _rtB -> B_20_0_0 [ 1 ] = _rtB -> B_35_42_0 [
3 ] ; _rtB -> B_20_0_0 [ 2 ] = _rtB -> B_35_42_0 [ 4 ] ; { FILE * fp = fopen
( "iDes_TBCC_Dyn_涡喷发动机内部结构模型2_InnerLoopPlant_Engine_Nozzle.bin" ,
"w" ) ; int i ; for ( i = 1 ; i <= 2 ; i ++ ) { double Val = _rtB -> B_20_0_0
[ i ] ; if ( Val >= 99999 ) Val = 99999 ; else if ( Val <= - 99999 ) Val = -
99999 ; fprintf ( fp , "%f\n" , Val ) ; } fclose ( fp ) ; } _rtDW ->
C_To_Workspace_SubsysRanBC_p = 4 ; } _rtB -> B_35_48_0 = ( _rtB -> B_35_19_0
/ _rtB -> B_54_258_0 * _rtP -> P_136 + ( _rtB -> B_35_24_0 [ 5 ] + _rtB ->
B_35_39_0 [ 5 ] ) ) * _rtP -> P_137 ; _rtB -> B_35_49_0 = _rtB -> B_35_35_0 ;
if ( _rtB -> B_35_49_0 ) { _rtB -> B_21_0_0 [ 0 ] = TBCC_Dyn_rtC ( S ) ->
B_21_0_0 ; _rtB -> B_21_0_0 [ 1 ] = _rtB -> B_35_39_0 [ 7 ] ; _rtB ->
B_21_0_0 [ 2 ] = _rtB -> B_35_39_0 [ 8 ] ; _rtB -> B_21_0_0 [ 3 ] = _rtB ->
B_35_39_0 [ 9 ] ; _rtB -> B_21_0_0 [ 4 ] = _rtB -> B_35_39_0 [ 10 ] ; { FILE
* fp = fopen (
"iDes_TBCC_Dyn_涡喷发动机内部结构模型2_InnerLoopPlant_Engine_Turbine.bin" ,
"w" ) ; int i ; for ( i = 1 ; i <= 4 ; i ++ ) { double Val = _rtB -> B_21_0_0
[ i ] ; if ( Val >= 99999 ) Val = 99999 ; else if ( Val <= - 99999 ) Val = -
99999 ; fprintf ( fp , "%f\n" , Val ) ; } fclose ( fp ) ; } _rtDW ->
Scalar_To_Workspace_SubsysRanBC_a = 4 ; } memcpy ( & _rtB -> B_35_51_0 [ 0 ]
, & _rtP -> P_138 [ 0 ] , 51U * sizeof ( real_T ) ) ; _rtB -> B_35_52_0 =
_rtB -> B_35_0_0_n ; i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { if (
_rtB -> B_35_52_0 > 0.0 ) { if ( ! _rtDW -> NRwCond_MODE_c ) { if (
ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } _rtDW -> NRwCond_MODE_c
= true ; } _rtB -> B_34_3_0 = ( ( uint8_T ) ( ( ( uint32_T ) (
muDoubleScalarAbs ( _rtB -> B_35_42_0 [ 2 ] ) > _rtB -> B_34_0_0 ) + (
muDoubleScalarAbs ( _rtB -> B_35_39_0 [ 6 ] ) > _rtB -> B_34_0_0 ) ) + (
muDoubleScalarAbs ( _rtB -> B_35_24_0 [ 6 ] ) > _rtB -> B_34_0_0 ) ) > _rtB
-> B_34_1_0 ) ; rtb_B_34_5_0 = ( ( ! _rtB -> B_24_5_0 ) && _rtB -> B_34_3_0 )
; i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtB -> B_33_0_0 =
rtb_B_34_5_0 ; _rtB -> B_33_1_0 [ 0 ] = _rtDW -> UnitDelay_DSTATE_m [ 0 ] ;
_rtB -> B_33_1_0 [ 1 ] = _rtDW -> UnitDelay_DSTATE_m [ 1 ] ; _rtB -> B_33_1_0
[ 2 ] = _rtDW -> UnitDelay_DSTATE_m [ 2 ] ; i = ssIsSampleHit ( S , 1 , 0 ) ;
if ( i != 0 ) { if ( _rtB -> B_33_0_0 ) { if ( ! _rtDW -> NRUpdate_MODE_p ) {
if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } _rtDW -> NRUpdate_MODE_p
= true ; } B_49_3_0_idx_0 = muDoubleScalarAbs ( _rtB -> B_32_9_0 * _rtB ->
B_32_7_0 * ( _rtDW -> Memory_PreviousInput_g [ 0 ] + _rtB -> B_33_1_0 [ 0 ] )
) ; B_49_3_0_idx_1 = muDoubleScalarAbs ( _rtB -> B_32_9_0 * _rtB -> B_32_7_0
* ( _rtDW -> Memory_PreviousInput_g [ 1 ] + _rtB -> B_33_1_0 [ 1 ] ) ) ;
B_49_3_0_idx_2 = muDoubleScalarAbs ( _rtB -> B_32_9_0 * _rtB -> B_32_7_0 * (
_rtDW -> Memory_PreviousInput_g [ 2 ] + _rtB -> B_33_1_0 [ 2 ] ) ) ; for (
i_0 = 0 ; i_0 < 3 ; i_0 ++ ) { tmp [ i_0 ] = _rtB -> B_22_0_2 [ i_0 + 6 ] *
_rtB -> B_35_24_0 [ 6 ] + ( _rtB -> B_22_0_2 [ i_0 + 3 ] * _rtB -> B_35_39_0
[ 6 ] + _rtB -> B_22_0_2 [ i_0 ] * _rtB -> B_35_42_0 [ 2 ] ) ; } _rtB ->
B_32_11_0 [ 0 ] = muDoubleScalarMin ( _rtB -> B_32_1_0 [ 0 ] ,
muDoubleScalarMax ( _rtB -> B_33_1_0 [ 0 ] - muDoubleScalarMax ( _rtB ->
B_32_8_0 * B_49_3_0_idx_0 , muDoubleScalarMin ( B_49_3_0_idx_0 , tmp [ 0 ] )
) , _rtB -> B_32_3_0 [ 0 ] ) ) ; _rtB -> B_32_16_0 [ 0 ] = ( ( _rtB ->
B_32_1_0 [ 0 ] == _rtB -> B_32_11_0 [ 0 ] ) || ( _rtB -> B_32_11_0 [ 0 ] ==
_rtB -> B_32_3_0 [ 0 ] ) || _rtDW -> Memory_PreviousInput_e [ 0 ] ) ; _rtB ->
B_32_11_0 [ 1 ] = muDoubleScalarMin ( _rtB -> B_32_1_0 [ 1 ] ,
muDoubleScalarMax ( _rtB -> B_33_1_0 [ 1 ] - muDoubleScalarMax ( _rtB ->
B_32_8_0 * B_49_3_0_idx_1 , muDoubleScalarMin ( B_49_3_0_idx_1 , tmp [ 1 ] )
) , _rtB -> B_32_3_0 [ 1 ] ) ) ; _rtB -> B_32_16_0 [ 1 ] = ( ( _rtB ->
B_32_1_0 [ 1 ] == _rtB -> B_32_11_0 [ 1 ] ) || ( _rtB -> B_32_11_0 [ 1 ] ==
_rtB -> B_32_3_0 [ 1 ] ) || _rtDW -> Memory_PreviousInput_e [ 1 ] ) ; _rtB ->
B_32_11_0 [ 2 ] = muDoubleScalarMin ( _rtB -> B_32_1_0 [ 2 ] ,
muDoubleScalarMax ( _rtB -> B_33_1_0 [ 2 ] - muDoubleScalarMax ( _rtB ->
B_32_8_0 * B_49_3_0_idx_2 , muDoubleScalarMin ( B_49_3_0_idx_2 , tmp [ 2 ] )
) , _rtB -> B_32_3_0 [ 2 ] ) ) ; _rtB -> B_32_16_0 [ 2 ] = ( ( _rtB ->
B_32_1_0 [ 2 ] == _rtB -> B_32_11_0 [ 2 ] ) || ( _rtB -> B_32_11_0 [ 2 ] ==
_rtB -> B_32_3_0 [ 2 ] ) || _rtDW -> Memory_PreviousInput_e [ 2 ] ) ;
B_32_17_0 [ 0 ] = _rtB -> B_32_16_0 [ 0 ] ; B_32_17_0 [ 1 ] = _rtB ->
B_32_16_0 [ 1 ] ; B_32_17_0 [ 2 ] = _rtB -> B_32_16_0 [ 2 ] ; if ( B_32_17_0
[ 0 ] || B_32_17_0 [ 1 ] || B_32_17_0 [ 2 ] ) { if ( ! _rtDW ->
HitLimits_MODE_b ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } ssCallAccelRunBlock ( S
, 26 , 0 , SS_CALL_RTW_GENERATED_ENABLE ) ; _rtDW -> HitLimits_MODE_b = true
; } ssCallAccelRunBlock ( S , 26 , 0 , SS_CALL_MDL_OUTPUTS ) ; _rtDW ->
HitLimits_SubsysRanBC_i = 4 ; } else { if ( _rtDW -> HitLimits_MODE_b ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; ssCallAccelRunBlock ( S ,
26 , 0 , SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> HitLimits_MODE_b = false
; } } _rtB -> B_32_20_0 [ 0 ] = ( _rtB -> B_32_5_0 [ 0 ] || _rtDW ->
Memory1_PreviousInput_i [ 0 ] ) ; _rtB -> B_32_20_0 [ 1 ] = ( _rtB ->
B_32_5_0 [ 1 ] || _rtDW -> Memory1_PreviousInput_i [ 1 ] ) ; _rtB ->
B_32_20_0 [ 2 ] = ( _rtB -> B_32_5_0 [ 2 ] || _rtDW ->
Memory1_PreviousInput_i [ 2 ] ) ; B_32_21_0 [ 0 ] = _rtB -> B_32_20_0 [ 0 ] ;
B_32_21_0 [ 1 ] = _rtB -> B_32_20_0 [ 1 ] ; B_32_21_0 [ 2 ] = _rtB ->
B_32_20_0 [ 2 ] ; if ( B_32_21_0 [ 0 ] || B_32_21_0 [ 1 ] || B_32_21_0 [ 2 ]
) { if ( ! _rtDW -> ICsOutsideLimit_MODE_k ) { if ( ssGetTaskTime ( S , 1 )
!= ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; }
ssCallAccelRunBlock ( S , 28 , 0 , SS_CALL_RTW_GENERATED_ENABLE ) ; _rtDW ->
ICsOutsideLimit_MODE_k = true ; } ssCallAccelRunBlock ( S , 28 , 0 ,
SS_CALL_MDL_OUTPUTS ) ; _rtDW -> ICsOutsideLimit_SubsysRanBC_d = 4 ; } else {
if ( _rtDW -> ICsOutsideLimit_MODE_k ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; ssCallAccelRunBlock ( S ,
28 , 0 , SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> ICsOutsideLimit_MODE_k =
false ; } } B_32_23_0 [ 0 ] = _rtB -> B_32_6_0 [ 0 ] ; B_32_23_0 [ 1 ] = _rtB
-> B_32_6_0 [ 1 ] ; B_32_23_0 [ 2 ] = _rtB -> B_32_6_0 [ 2 ] ; if ( B_32_23_0
[ 0 ] || B_32_23_0 [ 1 ] || B_32_23_0 [ 2 ] ) { if ( ! _rtDW -> ZeroIC_MODE_c
) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } ssCallAccelRunBlock ( S
, 30 , 0 , SS_CALL_RTW_GENERATED_ENABLE ) ; _rtDW -> ZeroIC_MODE_c = true ; }
ssCallAccelRunBlock ( S , 30 , 0 , SS_CALL_MDL_OUTPUTS ) ; _rtDW ->
ZeroIC_SubsysRanBC_i = 4 ; } else { if ( _rtDW -> ZeroIC_MODE_c ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; ssCallAccelRunBlock ( S ,
30 , 0 , SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> ZeroIC_MODE_c = false ; }
} _rtDW -> NRUpdate_SubsysRanBC_o = 4 ; } else { if ( _rtDW ->
NRUpdate_MODE_p ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; if (
_rtDW -> HitLimits_MODE_b ) { ssCallAccelRunBlock ( S , 26 , 0 ,
SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> HitLimits_MODE_b = false ; } if (
_rtDW -> ICsOutsideLimit_MODE_k ) { ssCallAccelRunBlock ( S , 28 , 0 ,
SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> ICsOutsideLimit_MODE_k = false ; }
if ( _rtDW -> ZeroIC_MODE_c ) { ssCallAccelRunBlock ( S , 30 , 0 ,
SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> ZeroIC_MODE_c = false ; } _rtDW ->
NRUpdate_MODE_p = false ; } } } if ( rtb_B_34_5_0 ) { _rtB -> B_33_3_0 [ 0 ]
= _rtB -> B_32_11_0 [ 0 ] ; _rtB -> B_33_3_0 [ 1 ] = _rtB -> B_32_11_0 [ 1 ]
; _rtB -> B_33_3_0 [ 2 ] = _rtB -> B_32_11_0 [ 2 ] ; } else { _rtB ->
B_33_3_0 [ 0 ] = _rtB -> B_33_1_0 [ 0 ] ; _rtB -> B_33_3_0 [ 1 ] = _rtB ->
B_33_1_0 [ 1 ] ; _rtB -> B_33_3_0 [ 2 ] = _rtB -> B_33_1_0 [ 2 ] ; } } _rtDW
-> NRwCond_SubsysRanBC_n = 4 ; } else { if ( _rtDW -> NRwCond_MODE_c ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; if ( _rtDW ->
NRUpdate_MODE_p ) { if ( _rtDW -> HitLimits_MODE_b ) { ssCallAccelRunBlock (
S , 26 , 0 , SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> HitLimits_MODE_b =
false ; } if ( _rtDW -> ICsOutsideLimit_MODE_k ) { ssCallAccelRunBlock ( S ,
28 , 0 , SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> ICsOutsideLimit_MODE_k =
false ; } if ( _rtDW -> ZeroIC_MODE_c ) { ssCallAccelRunBlock ( S , 30 , 0 ,
SS_CALL_RTW_GENERATED_DISABLE ) ; _rtDW -> ZeroIC_MODE_c = false ; } _rtDW ->
NRUpdate_MODE_p = false ; } _rtDW -> NRwCond_MODE_c = false ; } } } i =
ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { if ( _rtDW -> JCwReset_MODE_b )
{ _rtDW -> UnitDelay_DSTATE_o [ 0 ] = _rtB -> B_33_3_0 [ 0 ] ; _rtDW ->
UnitDelay_DSTATE_o [ 1 ] = _rtB -> B_33_3_0 [ 1 ] ; _rtDW ->
UnitDelay_DSTATE_o [ 2 ] = _rtB -> B_33_3_0 [ 2 ] ; i = ssIsSampleHit ( S , 1
, 0 ) ; if ( i != 0 ) { _rtDW -> UnitDelay_DSTATE_j = _rtB -> B_24_4_0 ; if (
_rtDW -> Jacobian_MODE_j ) { _rtDW -> UnitDelay2_DSTATE_i [ 0 ] = _rtB ->
B_35_42_0 [ 2 ] ; _rtDW -> UnitDelay2_DSTATE_i [ 1 ] = _rtB -> B_35_39_0 [ 6
] ; _rtDW -> UnitDelay2_DSTATE_i [ 2 ] = _rtB -> B_35_24_0 [ 6 ] ; memcpy ( &
_rtDW -> UnitDelay_DSTATE_f [ 0 ] , & _rtB -> B_22_0_3 [ 0 ] , 9U * sizeof (
real_T ) ) ; memcpy ( & _rtDW -> UnitDelay1_DSTATE_d [ 0 ] , & _rtB ->
B_22_0_4 [ 0 ] , 21U * sizeof ( real_T ) ) ; _rtDW -> UnitDelay3_DSTATE_j =
_rtB -> B_22_0_5 ; } } } if ( _rtDW -> NRwCond_MODE_c ) { i = ssIsSampleHit (
S , 1 , 0 ) ; if ( i != 0 ) { _rtDW -> UnitDelay_DSTATE_m [ 0 ] = _rtB ->
B_33_3_0 [ 0 ] ; _rtDW -> UnitDelay_DSTATE_m [ 1 ] = _rtB -> B_33_3_0 [ 1 ] ;
_rtDW -> UnitDelay_DSTATE_m [ 2 ] = _rtB -> B_33_3_0 [ 2 ] ; i =
ssIsSampleHit ( S , 1 , 0 ) ; if ( ( i != 0 ) && _rtDW -> NRUpdate_MODE_p ) {
_rtDW -> Memory_PreviousInput_g [ 0 ] = _rtB -> B_33_1_0 [ 0 ] ; _rtDW ->
Memory_PreviousInput_e [ 0 ] = _rtB -> B_32_16_0 [ 0 ] ; _rtDW ->
Memory1_PreviousInput_i [ 0 ] = _rtB -> B_32_20_0 [ 0 ] ; _rtDW ->
Memory_PreviousInput_g [ 1 ] = _rtB -> B_33_1_0 [ 1 ] ; _rtDW ->
Memory_PreviousInput_e [ 1 ] = _rtB -> B_32_16_0 [ 1 ] ; _rtDW ->
Memory1_PreviousInput_i [ 1 ] = _rtB -> B_32_20_0 [ 1 ] ; _rtDW ->
Memory_PreviousInput_g [ 2 ] = _rtB -> B_33_1_0 [ 2 ] ; _rtDW ->
Memory_PreviousInput_e [ 2 ] = _rtB -> B_32_16_0 [ 2 ] ; _rtDW ->
Memory1_PreviousInput_i [ 2 ] = _rtB -> B_32_20_0 [ 2 ] ; } } } } Iter ++ ; }
while ( ( _rtB -> B_35_0_0_n != 0.0 ) && _rtB -> B_34_3_0 && ( Iter <= 200 )
) ; } ssCallAccelRunBlock ( S , 54 , 260 , SS_CALL_MDL_OUTPUTS ) ; i =
ssIsSampleHit ( S , 2 , 0 ) ; if ( i != 0 ) { B_54_261_0 = _rtB -> B_54_258_0
; _rtB -> B_54_264_0 = ( B_54_261_0 * _rtB -> B_54_15_0_c + _rtB ->
B_54_14_0_p ) * _rtP -> P_380 ; } _rtB -> B_54_265_0 = _rtP -> P_381 * _rtB
-> B_54_244_0 ; _rtB -> B_54_266_0 = _rtB -> B_54_264_0 - _rtB -> B_54_265_0
; i = ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { _rtB -> B_54_267_0 = _rtB
-> B_54_12_0_c ; } TBCC_Dyn_NoiseGenerator ( S , _rtB -> B_54_267_0 ,
B_54_261_0 , _rtB -> B_54_13_0_b , & _rtB -> NoiseGenerator_k , & _rtDW ->
NoiseGenerator_k , & _rtP -> NoiseGenerator_k , & _rtX -> NoiseGenerator_k ,
& ( ( XDis_TBCC_Dyn_T * ) ssGetContStateDisabled ( S ) ) -> NoiseGenerator_k
) ; _rtB -> B_54_269_0 = _rtP -> P_382 * _rtB -> B_54_248_0 ; i =
ssIsSampleHit ( S , 2 , 0 ) ; if ( i != 0 ) { B_54_270_0 = _rtB -> B_35_0_0 ;
B_54_271_0 = _rtB -> B_35_5_0 [ 3 ] ; B_54_272_0 = _rtB -> B_35_24_0 [ 0 ] ;
B_54_273_0 = _rtB -> B_35_24_0 [ 1 ] ; B_54_274_0 = _rtB -> B_35_24_0 [ 2 ] ;
B_54_275_0 = _rtB -> B_35_24_0 [ 3 ] ; B_54_276_0 = _rtB -> B_35_24_0 [ 4 ] ;
B_54_277_0 = _rtB -> B_35_26_0 [ 0 ] ; B_54_278_0 = _rtB -> B_35_26_0 [ 1 ] ;
B_54_279_0 = _rtB -> B_35_26_0 [ 2 ] ; B_54_280_0 = _rtB -> B_35_26_0 [ 3 ] ;
B_54_281_0 = _rtB -> B_35_5_0 [ 0 ] ; B_54_282_0 = _rtB -> B_35_26_0 [ 4 ] ;
B_54_283_0 = _rtB -> B_35_39_0 [ 0 ] ; B_54_284_0 = _rtB -> B_35_39_0 [ 1 ] ;
B_54_285_0 = _rtB -> B_35_39_0 [ 2 ] ; B_54_286_0 = _rtB -> B_35_39_0 [ 3 ] ;
B_54_287_0 = _rtB -> B_35_39_0 [ 4 ] ; B_54_288_0 = _rtB -> B_35_39_0 [ 0 ] ;
B_54_289_0 = _rtB -> B_35_39_0 [ 1 ] ; B_54_290_0 = _rtB -> B_35_39_0 [ 2 ] ;
B_54_291_0 = _rtB -> B_35_40_0 ; B_54_292_0 = _rtB -> B_35_5_0 [ 1 ] ;
B_54_293_0 = _rtB -> B_35_39_0 [ 4 ] ; B_54_294_0 = _rtB -> B_35_5_0 [ 4 ] ;
B_54_295_0 = _rtB -> B_35_5_0 [ 5 ] ; B_54_296_0 = _rtB -> B_35_5_0 [ 6 ] ;
B_54_297_0 = _rtB -> B_35_6_0 ; B_54_298_0 = _rtB -> B_35_5_0 [ 7 ] ;
B_54_299_0 = _rtB -> B_35_24_0 [ 7 ] ; B_54_300_0 = _rtB -> B_35_24_0 [ 8 ] ;
B_54_301_0 = _rtB -> B_35_24_0 [ 9 ] ; B_54_302_0 = _rtB -> B_35_24_0 [ 10 ]
; B_54_303_0 = _rtB -> B_35_5_0 [ 2 ] ; B_54_304_0 = _rtB -> B_35_24_0 [ 11 ]
; B_54_305_0 = _rtB -> B_35_24_0 [ 12 ] ; B_54_306_0 = _rtB -> B_35_24_0 [ 13
] ; B_54_307_0 = _rtB -> B_35_24_0 [ 14 ] ; B_54_308_0 = _rtB -> B_35_24_0 [
15 ] ; B_54_309_0 = _rtB -> B_35_24_0 [ 16 ] ; B_54_310_0 = _rtB -> B_35_24_0
[ 17 ] ; B_54_311_0 = _rtB -> B_35_24_0 [ 18 ] ; B_54_312_0 = _rtB ->
B_35_24_0 [ 19 ] ; B_54_313_0 = _rtB -> B_35_24_0 [ 20 ] ; B_54_314_0 = _rtB
-> B_35_5_0 [ 3 ] ; B_54_315_0 = _rtB -> B_35_24_0 [ 21 ] ; B_54_316_0 = _rtB
-> B_35_24_0 [ 22 ] ; B_54_317_0 = _rtB -> B_35_24_0 [ 23 ] ; B_54_318_0 =
_rtB -> B_35_24_0 [ 24 ] ; B_54_319_0 = _rtB -> B_35_24_0 [ 25 ] ; B_54_320_0
= _rtB -> B_35_24_0 [ 26 ] ; B_54_321_0 = _rtB -> B_35_1_0 ; memcpy ( &
B_54_322_0 [ 0 ] , & _rtB -> B_35_29_0 [ 0 ] , 54U * sizeof ( real_T ) ) ;
B_54_323_0 = _rtB -> B_35_13_0 [ 0 ] ; B_54_324_0 = _rtB -> B_35_13_0 [ 1 ] ;
B_54_325_0 = _rtB -> B_35_2_0 ; B_54_326_0 = _rtB -> B_35_13_0 [ 2 ] ;
B_54_327_0 = _rtB -> B_35_39_0 [ 7 ] ; B_54_328_0 = _rtB -> B_35_39_0 [ 8 ] ;
B_54_329_0 = _rtB -> B_35_39_0 [ 9 ] ; B_54_330_0 = _rtB -> B_35_39_0 [ 10 ]
; B_54_331_0 = _rtB -> B_35_39_0 [ 11 ] ; B_54_332_0 = _rtB -> B_35_39_0 [ 12
] ; B_54_333_0 = _rtB -> B_35_39_0 [ 13 ] ; B_54_334_0 = _rtB -> B_35_39_0 [
14 ] ; B_54_335_0 = _rtB -> B_35_39_0 [ 15 ] ; B_54_336_0 = _rtB -> B_35_5_0
[ 0 ] ; B_54_337_0 = _rtB -> B_35_39_0 [ 16 ] ; B_54_338_0 = _rtB ->
B_35_39_0 [ 17 ] ; B_54_339_0 = _rtB -> B_35_39_0 [ 18 ] ; B_54_340_0 = _rtB
-> B_35_39_0 [ 19 ] ; B_54_341_0 = _rtB -> B_35_3_0 ; memcpy ( & B_54_342_0 [
0 ] , & _rtB -> B_35_51_0 [ 0 ] , 51U * sizeof ( real_T ) ) ; B_54_343_0 =
_rtB -> B_35_31_0 [ 0 ] ; B_54_344_0 = _rtB -> B_35_31_0 [ 1 ] ; B_54_345_0 =
_rtB -> B_35_42_0 [ 1 ] ; B_54_346_0 = _rtB -> B_35_48_0 ; B_54_347_0 = _rtB
-> B_35_5_0 [ 1 ] ; B_54_348_0 = _rtB -> B_35_10_0 ; B_54_349_0 [ 0 ] = _rtB
-> B_35_42_0 [ 2 ] ; B_54_349_0 [ 1 ] = _rtB -> B_35_39_0 [ 6 ] ; B_54_349_0
[ 2 ] = _rtB -> B_35_24_0 [ 6 ] ; B_54_350_0 = _rtB -> B_35_9_0 ; } i =
ssIsSampleHit ( S , 1 , 0 ) ; if ( i != 0 ) { i = ssIsSampleHit ( S , 1 , 0 )
; if ( i != 0 ) { ssCallAccelRunBlock ( S , 53 , 0 , SS_CALL_MDL_OUTPUTS ) ;
_rtB -> B_53_1_0 [ 0 ] = _rtB -> B_53_0_0 ; _rtB -> B_53_1_0 [ 1 ] = _rtB ->
B_53_1_0_g ; } } UNUSED_PARAMETER ( tid ) ; } static void mdlOutputsTID4 (
SimStruct * S , int_T tid ) { int32_T B_52_18_0 ; int32_T B_35_18_0 ; int32_T
i ; B_TBCC_Dyn_T * _rtB ; P_TBCC_Dyn_T * _rtP ; _rtP = ( ( P_TBCC_Dyn_T * )
ssGetModelRtp ( S ) ) ; _rtB = ( ( B_TBCC_Dyn_T * ) _ssGetModelBlockIO ( S )
) ; _rtB -> B_54_0_0_m = _rtP -> P_383 ; _rtB -> B_54_1_0_c = _rtP -> P_384 ;
_rtB -> B_54_2_0 = _rtP -> P_385 ; _rtB -> B_54_3_0 = _rtP -> P_386 ; _rtB ->
B_54_5_0_k = _rtP -> P_387 ; _rtB -> B_54_6_0 = _rtP -> P_388 ; _rtB ->
B_54_7_0 = _rtP -> P_389 ; _rtB -> B_54_9_0 = _rtP -> P_390 ; _rtB ->
B_54_10_0 = _rtP -> P_391 ; _rtB -> B_52_0_0_g = _rtP -> P_276 ; _rtB ->
B_52_1_0_m = _rtP -> P_277 ; _rtB -> B_52_2_0_n = _rtP -> P_278 ; _rtB ->
B_52_3_0_p = _rtP -> P_279 ; _rtB -> B_52_4_0_l = _rtP -> P_280 ; _rtB ->
B_52_5_0_j = _rtP -> P_281 ; _rtB -> B_52_6_0_d = _rtP -> P_282 ; _rtB ->
B_52_7_0 = _rtP -> P_283 ; _rtB -> B_52_8_0 = _rtP -> P_284 ; _rtB ->
B_52_11_0_o = ( _rtP -> P_285 == _rtP -> P_286 ) ; _rtB -> B_52_12_0 = _rtP
-> P_287 ; _rtB -> B_52_13_0_g = _rtP -> P_288 ; _rtB -> B_52_14_0 = _rtP ->
P_289 ; _rtB -> B_52_16_0 = ! _rtB -> B_52_11_0_o ; _rtB -> B_52_17_0 = _rtP
-> P_290 ; B_52_18_0 = _rtP -> P_399 ; _rtB -> B_52_19_0 = _rtP -> P_291 ;
_rtB -> B_52_22_0 = _rtP -> P_292 - _rtP -> P_293 ; _rtB -> B_52_23_0_l =
_rtP -> P_294 ; _rtB -> B_52_24_0_d = _rtP -> P_295 ; _rtB -> B_52_27_0_n = (
_rtP -> P_296 == _rtP -> P_297 ) ; _rtB -> B_52_28_0 = _rtP -> P_298 ; _rtB
-> B_52_29_0_d = _rtP -> P_299 ; _rtB -> B_52_30_0 = _rtP -> P_300 ; _rtB ->
B_52_31_0_l = _rtP -> P_301 ; _rtB -> B_52_32_0 = _rtP -> P_302 ; _rtB ->
B_52_35_0 = ( _rtP -> P_303 == _rtP -> P_304 ) ; _rtB -> B_52_36_0 = _rtP ->
P_305 ; _rtB -> B_52_37_0 = _rtP -> P_306 ; _rtB -> B_52_39_0_o = ! _rtB ->
B_52_35_0 ; _rtB -> B_42_0_0 [ 0 ] = _rtP -> P_180 [ 0 ] ; _rtB -> B_42_0_0 [
1 ] = _rtP -> P_180 [ 1 ] ; _rtB -> B_42_0_0 [ 2 ] = _rtP -> P_180 [ 2 ] ;
_rtB -> B_42_1_0_n = _rtP -> P_401 ; for ( i = 0 ; i < 6 ; i ++ ) { _rtB ->
B_41_0_0 [ i ] = _rtP -> P_175 [ i ] ; } _rtB -> B_41_1_0 = _rtP -> P_176 ;
_rtB -> B_41_4_0_b = _rtP -> P_177 + _rtP -> P_178 ; _rtB -> B_51_0_0 = _rtP
-> P_189 ; _rtB -> B_51_1_0 = _rtP -> P_190 ; _rtB -> B_49_1_0 [ 0 ] = _rtP
-> P_183 [ 0 ] ; _rtB -> B_49_3_0 [ 0 ] = _rtP -> P_184 [ 0 ] ; _rtB ->
B_49_5_0 [ 0 ] = ( ( _rtP -> P_182 [ 0 ] >= _rtB -> B_49_1_0 [ 0 ] ) || (
_rtP -> P_182 [ 0 ] <= _rtB -> B_49_3_0 [ 0 ] ) ) ; _rtB -> B_49_6_0 [ 0 ] =
muDoubleScalarIsNaN ( _rtP -> P_182 [ 0 ] ) ; _rtB -> B_49_1_0 [ 1 ] = _rtP
-> P_183 [ 1 ] ; _rtB -> B_49_3_0 [ 1 ] = _rtP -> P_184 [ 1 ] ; _rtB ->
B_49_5_0 [ 1 ] = ( ( _rtP -> P_182 [ 1 ] >= _rtB -> B_49_1_0 [ 1 ] ) || (
_rtP -> P_182 [ 1 ] <= _rtB -> B_49_3_0 [ 1 ] ) ) ; _rtB -> B_49_6_0 [ 1 ] =
muDoubleScalarIsNaN ( _rtP -> P_182 [ 1 ] ) ; _rtB -> B_49_1_0 [ 2 ] = _rtP
-> P_183 [ 2 ] ; _rtB -> B_49_3_0 [ 2 ] = _rtP -> P_184 [ 2 ] ; _rtB ->
B_49_5_0 [ 2 ] = ( ( _rtP -> P_182 [ 2 ] >= _rtB -> B_49_1_0 [ 2 ] ) || (
_rtP -> P_182 [ 2 ] <= _rtB -> B_49_3_0 [ 2 ] ) ) ; _rtB -> B_49_6_0 [ 2 ] =
muDoubleScalarIsNaN ( _rtP -> P_182 [ 2 ] ) ; _rtB -> B_49_7_0 = _rtP ->
P_185 ; _rtB -> B_49_8_0 = _rtP -> P_186 ; _rtB -> B_49_9_0 = _rtP -> P_187 ;
_rtB -> B_54_12_0_c = _rtP -> P_392 ; _rtB -> B_54_13_0_b = _rtP -> P_393 ;
_rtB -> B_54_14_0_p = _rtP -> P_394 ; _rtB -> B_54_15_0_c = _rtP -> P_395 ;
_rtB -> B_54_17_0 = _rtP -> P_396 ; _rtB -> B_54_18_0_f = _rtP -> P_397 ;
_rtB -> B_35_0_0_n = _rtP -> P_139 ; _rtB -> B_35_1_0_b = _rtP -> P_140 ;
_rtB -> B_35_2_0_l = _rtP -> P_141 ; _rtB -> B_35_3_0_h = _rtP -> P_142 ;
_rtB -> B_35_4_0_b = _rtP -> P_143 ; _rtB -> B_35_5_0_d = _rtP -> P_144 ;
_rtB -> B_35_6_0_e = _rtP -> P_145 ; _rtB -> B_35_7_0 = _rtP -> P_146 ; _rtB
-> B_35_8_0 = _rtP -> P_147 ; _rtB -> B_35_11_0_m = ( _rtP -> P_148 == _rtP
-> P_149 ) ; _rtB -> B_35_12_0 = _rtP -> P_150 ; _rtB -> B_35_13_0_b = _rtP
-> P_151 ; _rtB -> B_35_14_0 = _rtP -> P_152 ; _rtB -> B_35_16_0 = ! _rtB ->
B_35_11_0_m ; _rtB -> B_35_17_0 = _rtP -> P_153 ; B_35_18_0 = _rtP -> P_398 ;
_rtB -> B_35_19_0 = _rtP -> P_154 ; _rtB -> B_35_22_0 = _rtP -> P_155 - _rtP
-> P_156 ; _rtB -> B_35_23_0_j = _rtP -> P_157 ; _rtB -> B_35_24_0_f = _rtP
-> P_158 ; _rtB -> B_35_27_0_c = ( _rtP -> P_159 == _rtP -> P_160 ) ; _rtB ->
B_35_28_0 = _rtP -> P_161 ; _rtB -> B_35_29_0_a = _rtP -> P_162 ; _rtB ->
B_35_30_0 = _rtP -> P_163 ; _rtB -> B_35_31_0_j = _rtP -> P_164 ; _rtB ->
B_35_32_0 = _rtP -> P_165 ; _rtB -> B_35_35_0 = ( _rtP -> P_166 == _rtP ->
P_167 ) ; _rtB -> B_35_36_0 = _rtP -> P_168 ; _rtB -> B_35_37_0 = _rtP ->
P_169 ; _rtB -> B_35_39_0_j = ! _rtB -> B_35_35_0 ; _rtB -> B_25_0_0 [ 0 ] =
_rtP -> P_43 [ 0 ] ; _rtB -> B_25_0_0 [ 1 ] = _rtP -> P_43 [ 1 ] ; _rtB ->
B_25_0_0 [ 2 ] = _rtP -> P_43 [ 2 ] ; _rtB -> B_25_1_0_i = _rtP -> P_400 ;
for ( i = 0 ; i < 6 ; i ++ ) { _rtB -> B_24_0_0 [ i ] = _rtP -> P_38 [ i ] ;
} _rtB -> B_24_1_0 = _rtP -> P_39 ; _rtB -> B_24_4_0_o = _rtP -> P_40 + _rtP
-> P_41 ; _rtB -> B_34_0_0 = _rtP -> P_52 ; _rtB -> B_34_1_0 = _rtP -> P_53 ;
_rtB -> B_32_1_0 [ 0 ] = _rtP -> P_46 [ 0 ] ; _rtB -> B_32_3_0 [ 0 ] = _rtP
-> P_47 [ 0 ] ; _rtB -> B_32_5_0 [ 0 ] = ( ( _rtP -> P_45 [ 0 ] >= _rtB ->
B_32_1_0 [ 0 ] ) || ( _rtP -> P_45 [ 0 ] <= _rtB -> B_32_3_0 [ 0 ] ) ) ; _rtB
-> B_32_6_0 [ 0 ] = muDoubleScalarIsNaN ( _rtP -> P_45 [ 0 ] ) ; _rtB ->
B_32_1_0 [ 1 ] = _rtP -> P_46 [ 1 ] ; _rtB -> B_32_3_0 [ 1 ] = _rtP -> P_47 [
1 ] ; _rtB -> B_32_5_0 [ 1 ] = ( ( _rtP -> P_45 [ 1 ] >= _rtB -> B_32_1_0 [ 1
] ) || ( _rtP -> P_45 [ 1 ] <= _rtB -> B_32_3_0 [ 1 ] ) ) ; _rtB -> B_32_6_0
[ 1 ] = muDoubleScalarIsNaN ( _rtP -> P_45 [ 1 ] ) ; _rtB -> B_32_1_0 [ 2 ] =
_rtP -> P_46 [ 2 ] ; _rtB -> B_32_3_0 [ 2 ] = _rtP -> P_47 [ 2 ] ; _rtB ->
B_32_5_0 [ 2 ] = ( ( _rtP -> P_45 [ 2 ] >= _rtB -> B_32_1_0 [ 2 ] ) || ( _rtP
-> P_45 [ 2 ] <= _rtB -> B_32_3_0 [ 2 ] ) ) ; _rtB -> B_32_6_0 [ 2 ] =
muDoubleScalarIsNaN ( _rtP -> P_45 [ 2 ] ) ; _rtB -> B_32_7_0 = _rtP -> P_48
; _rtB -> B_32_8_0 = _rtP -> P_49 ; _rtB -> B_32_9_0 = _rtP -> P_50 ; _rtB ->
B_53_0_0 = 0.0 ; _rtB -> B_53_1_0_g = 0.0 ; UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdate ( SimStruct * S , int_T tid ) { int32_T isHit ;
B_TBCC_Dyn_T * _rtB ; P_TBCC_Dyn_T * _rtP ; DW_TBCC_Dyn_T * _rtDW ; _rtDW = (
( DW_TBCC_Dyn_T * ) ssGetRootDWork ( S ) ) ; _rtP = ( ( P_TBCC_Dyn_T * )
ssGetModelRtp ( S ) ) ; _rtB = ( ( B_TBCC_Dyn_T * ) _ssGetModelBlockIO ( S )
) ; if ( _rtDW -> If2_ActiveSubsystem == 0 ) { { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_p . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_p . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_c .
Head = ( ( _rtDW -> TransportDelay_IWORK_c . Head < ( _rtDW ->
TransportDelay_IWORK_c . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_c . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_c . Head == _rtDW -> TransportDelay_IWORK_c . Tail ) {
if ( ! TBCC_Dyn_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_c . CircularBufSize , & _rtDW -> TransportDelay_IWORK_c
. Tail , & _rtDW -> TransportDelay_IWORK_c . Head , & _rtDW ->
TransportDelay_IWORK_c . Last , simTime - _rtP -> P_29 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_c .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_c . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_c . Head ] = _rtB ->
B_54_216_0 ; } } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> UnitDelay_DSTATE = _rtB -> NoiseGenerator . B_0_5_0 ; } _rtDW ->
Integrator_IWORK = 0 ; _rtDW -> Integrator_IWORK_k = 0 ; if ( _rtDW ->
If1_ActiveSubsystem_g == 0 ) { { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_k . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_k . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_b . Head = ( ( _rtDW
-> TransportDelay_IWORK_b . Head < ( _rtDW -> TransportDelay_IWORK_b .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_b . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_b . Head == _rtDW ->
TransportDelay_IWORK_b . Tail ) { if ( !
TBCC_Dyn_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW -> TransportDelay_IWORK_b
. CircularBufSize , & _rtDW -> TransportDelay_IWORK_b . Tail , & _rtDW ->
TransportDelay_IWORK_b . Head , & _rtDW -> TransportDelay_IWORK_b . Last ,
simTime - _rtP -> P_14 , tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 ,
false , & _rtDW -> TransportDelay_IWORK_b . MaxNewBufSize ) ) {
ssSetErrorStatus ( S , "tdelay memory allocation error" ) ; return ; } } ( *
tBuffer ) [ _rtDW -> TransportDelay_IWORK_b . Head ] = simTime ; ( * uBuffer
) [ _rtDW -> TransportDelay_IWORK_b . Head ] = _rtB -> B_54_215_0 ; } } if (
_rtDW -> If3_ActiveSubsystem == 0 ) { { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK . Head = ( ( _rtDW ->
TransportDelay_IWORK . Head < ( _rtDW -> TransportDelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK . Head + 1 ) : 0 )
; if ( _rtDW -> TransportDelay_IWORK . Head == _rtDW -> TransportDelay_IWORK
. Tail ) { if ( ! TBCC_Dyn_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK . CircularBufSize , & _rtDW -> TransportDelay_IWORK .
Tail , & _rtDW -> TransportDelay_IWORK . Head , & _rtDW ->
TransportDelay_IWORK . Last , simTime - _rtP -> P_31 , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK . Head ] = _rtB ->
B_54_217_0 ; } } TBCC_Dyn_NoiseGenerator_Update ( S , & _rtDW ->
NoiseGenerator , & _rtP -> NoiseGenerator ) ; if ( _rtDW ->
If_ActiveSubsystem == 0 ) { { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_h . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_h . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_j . Head = ( ( _rtDW ->
TransportDelay_IWORK_j . Head < ( _rtDW -> TransportDelay_IWORK_j .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_j . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_j . Head == _rtDW ->
TransportDelay_IWORK_j . Tail ) { if ( !
TBCC_Dyn_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW -> TransportDelay_IWORK_j
. CircularBufSize , & _rtDW -> TransportDelay_IWORK_j . Tail , & _rtDW ->
TransportDelay_IWORK_j . Head , & _rtDW -> TransportDelay_IWORK_j . Last ,
simTime - _rtP -> P_8 , tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 ,
false , & _rtDW -> TransportDelay_IWORK_j . MaxNewBufSize ) ) {
ssSetErrorStatus ( S , "tdelay memory allocation error" ) ; return ; } } ( *
tBuffer ) [ _rtDW -> TransportDelay_IWORK_j . Head ] = simTime ; ( * uBuffer
) [ _rtDW -> TransportDelay_IWORK_j . Head ] = _rtB -> B_54_206_0 ; } } isHit
= ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
UnitDelay_DSTATE_p = _rtB -> NoiseGenerator_k . B_0_5_0 ; } _rtDW ->
Integrator_IWORK_b = 0 ; _rtDW -> Integrator_IWORK_e = 0 ;
TBCC_Dyn_NoiseGenerator_Update ( S , & _rtDW -> NoiseGenerator_k , & _rtP ->
NoiseGenerator_k ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
ssCallAccelRunBlock ( S , 53 , 0 , SS_CALL_MDL_UPDATE ) ; } }
UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdateTID4 ( SimStruct * S , int_T tid ) { UNUSED_PARAMETER (
tid ) ; }
#define MDL_DERIVATIVES
static void mdlDerivatives ( SimStruct * S ) { B_TBCC_Dyn_T * _rtB ;
P_TBCC_Dyn_T * _rtP ; X_TBCC_Dyn_T * _rtX ; XDot_TBCC_Dyn_T * _rtXdot ;
DW_TBCC_Dyn_T * _rtDW ; _rtDW = ( ( DW_TBCC_Dyn_T * ) ssGetRootDWork ( S ) )
; _rtXdot = ( ( XDot_TBCC_Dyn_T * ) ssGetdX ( S ) ) ; _rtX = ( ( X_TBCC_Dyn_T
* ) ssGetContStates ( S ) ) ; _rtP = ( ( P_TBCC_Dyn_T * ) ssGetModelRtp ( S )
) ; _rtB = ( ( B_TBCC_Dyn_T * ) _ssGetModelBlockIO ( S ) ) ; _rtXdot ->
TransferFcn_CSTATE = 0.0 ; _rtXdot -> TransferFcn_CSTATE += _rtP -> P_307 *
_rtX -> TransferFcn_CSTATE ; _rtXdot -> TransferFcn_CSTATE += _rtB ->
B_54_210_0 ; ( ( XDot_TBCC_Dyn_T * ) ssGetdX ( S ) ) -> TransferFcn2_CSTATE =
0.0 ; if ( _rtDW -> If1_ActiveSubsystem == 0 ) { _rtXdot ->
TransferFcn2_CSTATE = 0.0 ; _rtXdot -> TransferFcn2_CSTATE += _rtP -> P_4 *
_rtX -> TransferFcn2_CSTATE ; _rtXdot -> TransferFcn2_CSTATE += _rtB ->
B_54_170_0 ; } { real_T * dx ; int_T i ; dx = & ( ( ( XDot_TBCC_Dyn_T * )
ssGetdX ( S ) ) -> StateSpace_CSTATE ) ; for ( i = 0 ; i < 5 ; i ++ ) { dx [
i ] = 0.0 ; } } if ( _rtDW -> If2_ActiveSubsystem == 0 ) { _rtXdot ->
StateSpace_CSTATE = 0.0 ; _rtXdot -> StateSpace_CSTATE += _rtP -> P_22 * _rtX
-> StateSpace_CSTATE ; _rtXdot -> StateSpace_CSTATE += _rtP -> P_23 * _rtB ->
B_15_9_0 ; { real_T * dx ; int_T i ; dx = & ( ( ( XDot_TBCC_Dyn_T * ) ssGetdX
( S ) ) -> StateSpace_CSTATE_m ) ; for ( i = 0 ; i < 3 ; i ++ ) { dx [ i ] =
0.0 ; } } ( ( XDot_TBCC_Dyn_T * ) ssGetdX ( S ) ) -> IfActionSubsystem2_o .
StateSpace_CSTATE_k = 0.0 ; switch ( _rtDW -> If3_ActiveSubsystem_i ) { case
0 : _rtXdot -> StateSpace_CSTATE_m = 0.0 ; _rtXdot -> StateSpace_CSTATE_m +=
_rtP -> P_16 * _rtX -> StateSpace_CSTATE_m ; _rtXdot -> StateSpace_CSTATE_m
+= _rtP -> P_17 * _rtB -> B_15_8_0 ; ( ( XDot_TBCC_Dyn_T * ) ssGetdX ( S ) )
-> IfActionSubsystem1_l . StateSpace_CSTATE_k = 0.0 ; ( ( XDot_TBCC_Dyn_T * )
ssGetdX ( S ) ) -> IfActionSubsystem2_or . StateSpace_CSTATE_k = 0.0 ; switch
( _rtDW -> If3_ActiveSubsystem_l ) { case 0 :
TBCC_Dyn_IfActionSubsystem1_Deriv ( S , _rtB -> B_13_6_0 , & _rtP ->
IfActionSubsystem1_l , & _rtX -> IfActionSubsystem1_l , & _rtXdot ->
IfActionSubsystem1_l ) ; break ; case 1 : TBCC_Dyn_IfActionSubsystem1_Deriv (
S , _rtB -> B_15_8_0 , & _rtP -> IfActionSubsystem2_or , & _rtX ->
IfActionSubsystem2_or , & _rtXdot -> IfActionSubsystem2_or ) ; break ; }
break ; case 1 : TBCC_Dyn_IfActionSubsystem1_Deriv ( S , _rtB -> B_15_9_0 , &
_rtP -> IfActionSubsystem2_o , & _rtX -> IfActionSubsystem2_o , & _rtXdot ->
IfActionSubsystem2_o ) ; break ; } } _rtXdot -> Integrator_CSTATE = _rtB ->
B_54_54_0 ; _rtXdot -> Integrator_CSTATE_f = _rtB -> B_54_57_0 ; _rtXdot ->
Integrator_CSTATE_i = _rtB -> B_52_48_0 ; ( ( XDot_TBCC_Dyn_T * ) ssGetdX ( S
) ) -> TransferFcn1_CSTATE = 0.0 ; if ( _rtDW -> If2_ActiveSubsystem_i == 0 )
{ _rtXdot -> TransferFcn1_CSTATE = 0.0 ; _rtXdot -> TransferFcn1_CSTATE +=
_rtP -> P_6 * _rtX -> TransferFcn1_CSTATE ; _rtXdot -> TransferFcn1_CSTATE +=
_rtB -> B_54_204_0 ; } ( ( XDot_TBCC_Dyn_T * ) ssGetdX ( S ) ) ->
StateSpace_CSTATE_o = 0.0 ; if ( _rtDW -> If1_ActiveSubsystem_g == 0 ) {
_rtXdot -> StateSpace_CSTATE_o = 0.0 ; _rtXdot -> StateSpace_CSTATE_o += _rtP
-> P_10 * _rtX -> StateSpace_CSTATE_o ; _rtXdot -> StateSpace_CSTATE_o +=
_rtP -> P_11 * _rtB -> B_9_1_0 ; } if ( _rtDW -> EnabledSubsystem_MODE ) {
_rtXdot -> StateSpace_CSTATE_e = 0.0 ; _rtXdot -> StateSpace_CSTATE_e += _rtP
-> P_0 * _rtX -> StateSpace_CSTATE_e ; _rtXdot -> StateSpace_CSTATE_e += _rtP
-> P_1 * _rtB -> B_54_7_0 ; } else { ( ( XDot_TBCC_Dyn_T * ) ssGetdX ( S ) )
-> StateSpace_CSTATE_e = 0.0 ; } TBCC_Dyn_NoiseGenerator_Deriv ( S , & _rtB
-> NoiseGenerator , & _rtDW -> NoiseGenerator , & _rtP -> NoiseGenerator , &
_rtX -> NoiseGenerator , & _rtXdot -> NoiseGenerator ) ; _rtXdot ->
Integrator_CSTATE_fq = _rtB -> B_54_266_0 ; _rtXdot -> Integrator_CSTATE_c =
_rtB -> B_54_269_0 ; _rtXdot -> Integrator_CSTATE_p = _rtB -> B_35_48_0 ;
TBCC_Dyn_NoiseGenerator_Deriv ( S , & _rtB -> NoiseGenerator_k , & _rtDW ->
NoiseGenerator_k , & _rtP -> NoiseGenerator_k , & _rtX -> NoiseGenerator_k ,
& _rtXdot -> NoiseGenerator_k ) ; }
#define MDL_ZERO_CROSSINGS
static void mdlZeroCrossings ( SimStruct * S ) { B_TBCC_Dyn_T * _rtB ;
P_TBCC_Dyn_T * _rtP ; ZCV_TBCC_Dyn_T * _rtZCSV ; DW_TBCC_Dyn_T * _rtDW ;
_rtDW = ( ( DW_TBCC_Dyn_T * ) ssGetRootDWork ( S ) ) ; _rtZCSV = ( (
ZCV_TBCC_Dyn_T * ) ssGetSolverZcSignalVector ( S ) ) ; _rtP = ( (
P_TBCC_Dyn_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( ( B_TBCC_Dyn_T * )
_ssGetModelBlockIO ( S ) ) ; _rtZCSV -> If1_IfInput_ZC = 0.0 ; if ( _rtB ->
B_54_0_0 > 0.0 ) { _rtZCSV -> If1_IfInput_ZC = 1.0 ; } _rtZCSV ->
If2_IfInput_ZC = 0.0 ; if ( _rtB -> B_54_5_0 > 0.0 ) { _rtZCSV ->
If2_IfInput_ZC = 1.0 ; } { real_T * zcsv = & ( ( ( ZCV_TBCC_Dyn_T * )
ssGetSolverZcSignalVector ( S ) ) -> If3_IfInput_ZC_a ) ; int_T i ; for ( i =
0 ; i < 2 ; i ++ ) { zcsv [ i ] = 0.0 ; } } if ( _rtDW -> If2_ActiveSubsystem
== 0 ) { _rtZCSV -> If3_IfInput_ZC_a = 0.0 ; if ( _rtB -> B_15_0_0 <= 0.0 ) {
_rtZCSV -> If3_IfInput_ZC_a = 1.0 ; } { ( ( ZCV_TBCC_Dyn_T * )
ssGetSolverZcSignalVector ( S ) ) -> If3_IfInput_ZC_k = 0.0 ; } if ( _rtDW ->
If3_ActiveSubsystem_i == 0 ) { _rtZCSV -> If3_IfInput_ZC_k = 0.0 ; if ( _rtB
-> B_13_0_0 <= 1.0 ) { _rtZCSV -> If3_IfInput_ZC_k = 1.0 ; } } } _rtZCSV ->
Saturation_UprLim_ZC = _rtB -> B_54_14_0 - _rtP -> P_315 ; _rtZCSV ->
Saturation_LwrLim_ZC = _rtB -> B_54_14_0 - _rtP -> P_316 ; _rtZCSV ->
If2_IfInput_ZC_c = 0.0 ; if ( _rtB -> B_54_0_0 > 0.0 ) { _rtZCSV ->
If2_IfInput_ZC_c = 1.0 ; } _rtZCSV -> If1_IfInput_ZC_k = 0.0 ; if ( _rtB ->
B_54_5_0 > 0.0 ) { _rtZCSV -> If1_IfInput_ZC_k = 1.0 ; } if ( _rtB ->
B_54_39_0 > _rtB -> B_54_5_0 ) { if ( _rtDW -> Max_MODE == 0 ) { _rtZCSV ->
Max_MinmaxInput_ZC = _rtB -> B_54_39_0 - _rtB -> B_54_39_0 ; } else { _rtZCSV
-> Max_MinmaxInput_ZC = _rtB -> B_54_39_0 - _rtB -> B_54_5_0 ; } } else if (
_rtDW -> Max_MODE == 0 ) { _rtZCSV -> Max_MinmaxInput_ZC = _rtB -> B_54_5_0 -
_rtB -> B_54_39_0 ; } else { _rtZCSV -> Max_MinmaxInput_ZC = _rtB -> B_54_5_0
- _rtB -> B_54_5_0 ; } _rtZCSV -> If3_IfInput_ZC = 0.0 ; if ( _rtB ->
B_54_40_0 > 2.5 ) { _rtZCSV -> If3_IfInput_ZC = 1.0 ; } _rtZCSV ->
EnabledSubsystem_Enable_ZC = _rtB -> B_54_44_0 ; if ( ! _rtDW ->
EnabledSubsystem_MODE ) { } if ( _rtB -> B_54_46_0 > _rtB -> B_54_40_0 ) { if
( _rtDW -> Max1_MODE == 0 ) { _rtZCSV -> Max1_MinmaxInput_ZC = _rtB ->
B_54_46_0 - _rtB -> B_54_46_0 ; } else { _rtZCSV -> Max1_MinmaxInput_ZC =
_rtB -> B_54_46_0 - _rtB -> B_54_40_0 ; } } else if ( _rtDW -> Max1_MODE == 0
) { _rtZCSV -> Max1_MinmaxInput_ZC = _rtB -> B_54_40_0 - _rtB -> B_54_46_0 ;
} else { _rtZCSV -> Max1_MinmaxInput_ZC = _rtB -> B_54_40_0 - _rtB ->
B_54_40_0 ; } _rtZCSV -> If_IfInput_ZC = 0.0 ; if ( _rtB -> B_54_23_0 >= 1.0
) { _rtZCSV -> If_IfInput_ZC = 1.0 ; } _rtZCSV -> Saturation_UprLim_ZC_o =
_rtB -> B_54_246_0 - _rtP -> P_371 ; _rtZCSV -> Saturation_LwrLim_ZC_b = _rtB
-> B_54_246_0 - _rtP -> P_372 ; } static void mdlInitializeSizes ( SimStruct
* S ) { ssSetChecksumVal ( S , 0 , 835476802U ) ; ssSetChecksumVal ( S , 1 ,
1508216492U ) ; ssSetChecksumVal ( S , 2 , 4062319209U ) ; ssSetChecksumVal (
S , 3 , 2005418652U ) ; { mxArray * slVerStructMat = NULL ; mxArray *
slStrMat = mxCreateString ( "simulink" ) ; char slVerChar [ 10 ] ; int status
= mexCallMATLAB ( 1 , & slVerStructMat , 1 , & slStrMat , "ver" ) ; if (
status == 0 ) { mxArray * slVerMat = mxGetField ( slVerStructMat , 0 ,
"Version" ) ; if ( slVerMat == NULL ) { status = 1 ; } else { status =
mxGetString ( slVerMat , slVerChar , 10 ) ; } } mxDestroyArray ( slStrMat ) ;
mxDestroyArray ( slVerStructMat ) ; if ( ( status == 1 ) || ( strcmp (
slVerChar , "10.0" ) != 0 ) ) { return ; } } ssSetOptions ( S ,
SS_OPTION_EXCEPTION_FREE_CODE ) ; if ( ssGetSizeofDWork ( S ) != sizeof (
DW_TBCC_Dyn_T ) ) { ssSetErrorStatus ( S ,
"Unexpected error: Internal DWork sizes do "
"not match for accelerator mex file." ) ; } if ( ssGetSizeofGlobalBlockIO ( S
) != sizeof ( B_TBCC_Dyn_T ) ) { ssSetErrorStatus ( S ,
"Unexpected error: Internal BlockIO sizes do "
"not match for accelerator mex file." ) ; } { int ssSizeofParams ;
ssGetSizeofParams ( S , & ssSizeofParams ) ; if ( ssSizeofParams != sizeof (
P_TBCC_Dyn_T ) ) { static char msg [ 256 ] ; sprintf ( msg ,
"Unexpected error: Internal Parameters sizes do "
"not match for accelerator mex file." ) ; } } _ssSetModelRtp ( S , ( real_T *
) & TBCC_Dyn_rtDefaultP ) ; _ssSetConstBlockIO ( S , & TBCC_Dyn_rtInvariant )
; rt_InitInfAndNaN ( sizeof ( real_T ) ) ; } static void
mdlInitializeSampleTimes ( SimStruct * S ) { { SimStruct * childS ;
SysOutputFcn * callSysFcns ; childS = ssGetSFunction ( S , 0 ) ; callSysFcns
= ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 1 ) ; callSysFcns =
ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 2 ) ; callSysFcns =
ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 3 ) ; callSysFcns =
ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 4 ) ; callSysFcns =
ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 5 ) ; callSysFcns =
ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 6 ) ; callSysFcns =
ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 7 ) ; callSysFcns =
ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; } slAccRegPrmChangeFcn ( S , mdlOutputsTID4 ) ; }
static void mdlTerminate ( SimStruct * S ) { }
#include "simulink.c"
