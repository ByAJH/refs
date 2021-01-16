#include "__cf_TBCC_Dyn.h"
#ifndef RTW_HEADER_TBCC_Dyn_acc_h_
#define RTW_HEADER_TBCC_Dyn_acc_h_
#include <stddef.h>
#include <float.h>
#include <string.h>
#ifndef TBCC_Dyn_acc_COMMON_INCLUDES_
#define TBCC_Dyn_acc_COMMON_INCLUDES_
#include <stdlib.h>
#define S_FUNCTION_NAME simulink_only_sfcn 
#define S_FUNCTION_LEVEL 2
#define RTW_GENERATED_S_FUNCTION
#include "rtwtypes.h"
#include "simstruc.h"
#include "fixedpoint.h"
#include "stdio.h"
#include "types_TMATS.h"
#endif
#include "TBCC_Dyn_acc_types.h"
#include "multiword_types.h"
#include "mwmathutil.h"
#include "rt_look.h"
#include "rt_look1d.h"
#include "rt_defines.h"
#include "rt_nonfinite.h"
typedef struct { real_T B_0_3_0 ; real_T B_0_4_0 ; real_T B_0_5_0 ; }
B_NoiseGenerator_TBCC_Dyn_T ; typedef struct { real_T NextOutput ; int32_T
NoiseGenerator_sysIdxToRun ; uint32_T RandSeed ; int8_T
NoiseGenerator_SubsysRanBC ; boolean_T NoiseGenerator_MODE ; char_T
pad_NoiseGenerator_MODE [ 6 ] ; } DW_NoiseGenerator_TBCC_Dyn_T ; typedef
struct { real_T noisegeneratingfilter_CSTATE ; } X_NoiseGenerator_TBCC_Dyn_T
; typedef struct { real_T noisegeneratingfilter_CSTATE ; }
XDot_NoiseGenerator_TBCC_Dyn_T ; typedef struct { boolean_T
noisegeneratingfilter_CSTATE ; } XDis_NoiseGenerator_TBCC_Dyn_T ; typedef
struct { real_T noisegeneratingfilter_CSTATE ; }
XAbsTol_NoiseGenerator_TBCC_Dyn_T ; typedef struct { int32_T
IfActionSubsystem3_sysIdxToRun ; int8_T IfActionSubsystem3_SubsysRanBC ;
char_T pad_IfActionSubsystem3_SubsysRanBC [ 3 ] ; }
DW_IfActionSubsystem3_TBCC_Dyn_T ; typedef struct { int32_T
IfActionSubsystem1_sysIdxToRun ; int8_T IfActionSubsystem1_SubsysRanBC ;
char_T pad_IfActionSubsystem1_SubsysRanBC [ 3 ] ; }
DW_IfActionSubsystem1_TBCC_Dyn_T ; typedef struct { real_T
StateSpace_CSTATE_k ; } X_IfActionSubsystem1_TBCC_Dyn_T ; typedef struct {
real_T StateSpace_CSTATE_k ; } XDot_IfActionSubsystem1_TBCC_Dyn_T ; typedef
struct { boolean_T StateSpace_CSTATE_k ; } XDis_IfActionSubsystem1_TBCC_Dyn_T
; typedef struct { real_T StateSpace_CSTATE_k ; }
XAbsTol_IfActionSubsystem1_TBCC_Dyn_T ; typedef struct { real_T B_54_0_0 ;
real_T B_54_1_0 ; real_T B_54_5_0 ; real_T B_54_11_0 ; real_T B_54_12_0 ;
real_T B_54_13_0 ; real_T B_54_14_0 ; real_T B_54_15_0 ; real_T B_54_16_0 ;
real_T B_54_18_0 ; real_T B_54_19_0 ; real_T B_54_21_0 ; real_T B_54_23_0 ;
real_T B_54_25_0 ; real_T B_54_26_0 ; real_T B_54_28_0 ; real_T B_54_32_0 ;
real_T B_54_33_0 ; real_T B_54_34_0 ; real_T B_54_35_0 ; real_T B_54_39_0 ;
real_T B_54_40_0 ; real_T B_54_44_0 ; real_T B_54_46_0 ; real_T B_54_47_0 ;
real_T B_54_52_0 ; real_T B_54_53_0 ; real_T B_54_54_0 ; real_T B_54_55_0 ;
real_T B_54_57_0 ; real_T B_54_143_0 ; real_T B_54_144_0 ; real_T B_54_148_0
; real_T B_54_158_0 ; real_T B_54_163_0 ; real_T B_54_170_0 ; real_T
B_54_178_0 ; real_T B_54_180_0 ; real_T B_54_187_0 ; real_T B_54_191_0 ;
real_T B_54_192_0 ; real_T B_54_204_0 ; real_T B_54_205_0 ; real_T B_54_206_0
; real_T B_54_210_0 ; real_T B_54_211_0 ; real_T B_54_215_0 ; real_T
B_54_216_0 ; real_T B_54_217_0 ; real_T B_54_221_0 ; real_T B_54_222_0 ;
real_T B_54_223_0 ; real_T B_54_226_0 ; real_T B_54_231_0 ; real_T B_54_232_0
; real_T B_54_234_0 ; real_T B_54_235_0 ; real_T B_54_239_0 ; real_T
B_54_242_0 ; real_T B_54_244_0 ; real_T B_54_245_0 ; real_T B_54_246_0 ;
real_T B_54_247_0 ; real_T B_54_248_0 ; real_T B_54_250_0 ; real_T B_54_251_0
; real_T B_54_253_0 ; real_T B_54_255_0 ; real_T B_54_257_0 ; real_T
B_54_258_0 ; real_T B_54_264_0 ; real_T B_54_265_0 ; real_T B_54_266_0 ;
real_T B_54_267_0 ; real_T B_54_269_0 ; real_T B_54_0_0_m ; real_T B_54_1_0_c
; real_T B_54_2_0 ; real_T B_54_3_0 ; real_T B_54_5_0_k ; real_T B_54_6_0 ;
real_T B_54_7_0 ; real_T B_54_9_0 ; real_T B_54_10_0 ; real_T B_54_12_0_c ;
real_T B_54_13_0_b ; real_T B_54_14_0_p ; real_T B_54_15_0_c ; real_T
B_54_17_0 ; real_T B_54_18_0_f ; real_T B_53_1_0 [ 2 ] ; real_T B_53_0_0 ;
real_T B_53_1_0_g ; real_T B_52_0_0 ; real_T B_52_1_0 ; real_T B_52_2_0 ;
real_T B_52_3_0 ; real_T B_52_4_0 [ 3 ] ; real_T B_52_5_0 [ 8 ] ; real_T
B_52_6_0 ; real_T B_52_9_0 ; real_T B_52_11_0 ; real_T B_52_13_0 [ 3 ] ;
real_T B_52_23_0 [ 12 ] ; real_T B_52_24_0 [ 27 ] ; real_T B_52_24_1 [ 5 ] ;
real_T B_52_24_2 [ 5 ] ; real_T B_52_25_0 [ 6 ] ; real_T B_52_26_0 [ 6 ] ;
real_T B_52_29_0 [ 51 ] ; real_T B_52_31_0 [ 2 ] ; real_T B_52_38_0 [ 12 ] ;
real_T B_52_39_0 [ 20 ] ; real_T B_52_40_0 ; real_T B_52_41_0 [ 8 ] ; real_T
B_52_42_0 [ 17 ] ; real_T B_52_48_0 ; real_T B_52_51_0 [ 48 ] ; real_T
B_52_52_0 ; real_T B_52_0_0_g ; real_T B_52_1_0_m ; real_T B_52_2_0_n ;
real_T B_52_3_0_p ; real_T B_52_4_0_l ; real_T B_52_5_0_j ; real_T B_52_6_0_d
; real_T B_52_7_0 ; real_T B_52_8_0 ; real_T B_52_12_0 ; real_T B_52_13_0_g ;
real_T B_52_14_0 ; real_T B_52_16_0 ; real_T B_52_17_0 ; real_T B_52_19_0 ;
real_T B_52_22_0 ; real_T B_52_23_0_l ; real_T B_52_24_0_d ; real_T B_52_28_0
; real_T B_52_29_0_d ; real_T B_52_30_0 ; real_T B_52_31_0_l ; real_T
B_52_32_0 ; real_T B_52_36_0 ; real_T B_52_37_0 ; real_T B_52_39_0_o ; real_T
B_51_0_0 ; real_T B_51_1_0 ; real_T B_50_1_0 [ 3 ] ; real_T B_50_3_0 [ 3 ] ;
real_T B_49_11_0 [ 3 ] ; real_T B_49_1_0 [ 3 ] ; real_T B_49_3_0 [ 3 ] ;
real_T B_49_7_0 ; real_T B_49_8_0 ; real_T B_49_9_0 ; real_T B_42_1_0 [ 3 ] ;
real_T B_42_4_0 [ 3 ] ; real_T B_42_0_0 [ 3 ] ; real_T B_41_4_0 ; real_T
B_41_0_0 [ 6 ] ; real_T B_41_1_0 ; real_T B_41_4_0_b ; real_T B_40_0_0 [ 3 ]
; real_T B_40_1_0 [ 9 ] ; real_T B_40_2_0 [ 21 ] ; real_T B_40_3_0 ; real_T
B_39_0_1 [ 3 ] ; real_T B_39_0_2 [ 9 ] ; real_T B_39_0_3 [ 9 ] ; real_T
B_39_0_4 [ 21 ] ; real_T B_39_0_5 ; real_T B_38_0_0 [ 5 ] ; real_T B_37_0_0 [
3 ] ; real_T B_36_0_0 [ 5 ] ; real_T B_35_0_0 ; real_T B_35_1_0 ; real_T
B_35_2_0 ; real_T B_35_3_0 ; real_T B_35_4_0 [ 3 ] ; real_T B_35_5_0 [ 8 ] ;
real_T B_35_6_0 ; real_T B_35_9_0 ; real_T B_35_11_0 ; real_T B_35_13_0 [ 3 ]
; real_T B_35_23_0 [ 12 ] ; real_T B_35_24_0 [ 27 ] ; real_T B_35_24_1 [ 5 ]
; real_T B_35_24_2 [ 5 ] ; real_T B_35_25_0 [ 6 ] ; real_T B_35_26_0 [ 6 ] ;
real_T B_35_29_0 [ 54 ] ; real_T B_35_31_0 [ 2 ] ; real_T B_35_38_0 [ 12 ] ;
real_T B_35_39_0 [ 20 ] ; real_T B_35_40_0 ; real_T B_35_41_0 [ 8 ] ; real_T
B_35_42_0 [ 17 ] ; real_T B_35_48_0 ; real_T B_35_51_0 [ 51 ] ; real_T
B_35_52_0 ; real_T B_35_0_0_n ; real_T B_35_1_0_b ; real_T B_35_2_0_l ;
real_T B_35_3_0_h ; real_T B_35_4_0_b ; real_T B_35_5_0_d ; real_T B_35_6_0_e
; real_T B_35_7_0 ; real_T B_35_8_0 ; real_T B_35_12_0 ; real_T B_35_13_0_b ;
real_T B_35_14_0 ; real_T B_35_16_0 ; real_T B_35_17_0 ; real_T B_35_19_0 ;
real_T B_35_22_0 ; real_T B_35_23_0_j ; real_T B_35_24_0_f ; real_T B_35_28_0
; real_T B_35_29_0_a ; real_T B_35_30_0 ; real_T B_35_31_0_j ; real_T
B_35_32_0 ; real_T B_35_36_0 ; real_T B_35_37_0 ; real_T B_35_39_0_j ; real_T
B_34_0_0 ; real_T B_34_1_0 ; real_T B_33_1_0 [ 3 ] ; real_T B_33_3_0 [ 3 ] ;
real_T B_32_11_0 [ 3 ] ; real_T B_32_1_0 [ 3 ] ; real_T B_32_3_0 [ 3 ] ;
real_T B_32_7_0 ; real_T B_32_8_0 ; real_T B_32_9_0 ; real_T B_25_1_0 [ 3 ] ;
real_T B_25_4_0 [ 3 ] ; real_T B_25_0_0 [ 3 ] ; real_T B_24_4_0 ; real_T
B_24_0_0 [ 6 ] ; real_T B_24_1_0 ; real_T B_24_4_0_o ; real_T B_23_0_0 [ 3 ]
; real_T B_23_1_0 [ 9 ] ; real_T B_23_2_0 [ 21 ] ; real_T B_23_3_0 ; real_T
B_22_0_1 [ 3 ] ; real_T B_22_0_2 [ 9 ] ; real_T B_22_0_3 [ 9 ] ; real_T
B_22_0_4 [ 21 ] ; real_T B_22_0_5 ; real_T B_21_0_0 [ 5 ] ; real_T B_20_0_0 [
3 ] ; real_T B_19_0_0 [ 5 ] ; real_T B_17_0_0 ; real_T B_15_0_0 ; real_T
B_15_8_0 ; real_T B_15_9_0 ; real_T B_13_0_0 ; real_T B_13_6_0 ; real_T
B_9_1_0 ; real_T B_2_0_0 ; real_T B_2_1_0 [ 5 ] ; real_T B_2_2_0 [ 3 ] ;
real_T B_2_3_0 [ 5 ] ; real_T B_2_4_0 [ 3 ] ; uint32_T B_42_1_0_n ; uint32_T
B_25_1_0_i ; int32_T B_52_10_0 ; int32_T B_35_10_0 ; boolean_T B_52_27_0 ;
boolean_T B_52_43_0 ; boolean_T B_52_49_0 ; boolean_T B_52_11_0_o ; boolean_T
B_52_27_0_n ; boolean_T B_52_35_0 ; boolean_T B_51_3_0 ; boolean_T B_50_0_0 ;
boolean_T B_49_16_0 [ 3 ] ; boolean_T B_49_20_0 [ 3 ] ; boolean_T B_49_5_0 [
3 ] ; boolean_T B_49_6_0 [ 3 ] ; boolean_T B_41_5_0 ; boolean_T B_41_6_0 ;
boolean_T B_35_27_0 ; boolean_T B_35_43_0 ; boolean_T B_35_49_0 ; boolean_T
B_35_11_0_m ; boolean_T B_35_27_0_c ; boolean_T B_35_35_0 ; boolean_T
B_34_3_0 ; boolean_T B_33_0_0 ; boolean_T B_32_16_0 [ 3 ] ; boolean_T
B_32_20_0 [ 3 ] ; boolean_T B_32_5_0 [ 3 ] ; boolean_T B_32_6_0 [ 3 ] ;
boolean_T B_24_5_0 ; boolean_T B_24_6_0 ; char_T pad_B_24_6_0 [ 4 ] ;
B_NoiseGenerator_TBCC_Dyn_T NoiseGenerator_k ; B_NoiseGenerator_TBCC_Dyn_T
NoiseGenerator ; } B_TBCC_Dyn_T ; typedef struct { real_T UnitDelay_DSTATE ;
real_T UnitDelay_DSTATE_p ; real_T UnitDelay_DSTATE_k [ 3 ] ; real_T
UnitDelay_DSTATE_c [ 3 ] ; real_T UnitDelay_DSTATE_g ; real_T
UnitDelay2_DSTATE [ 3 ] ; real_T UnitDelay_DSTATE_kh [ 9 ] ; real_T
UnitDelay1_DSTATE [ 21 ] ; real_T UnitDelay3_DSTATE ; real_T
UnitDelay_DSTATE_m [ 3 ] ; real_T UnitDelay_DSTATE_o [ 3 ] ; real_T
UnitDelay_DSTATE_j ; real_T UnitDelay2_DSTATE_i [ 3 ] ; real_T
UnitDelay_DSTATE_f [ 9 ] ; real_T UnitDelay1_DSTATE_d [ 21 ] ; real_T
UnitDelay3_DSTATE_j ; real_T Memory_PreviousInput [ 3 ] ; real_T
Memory_PreviousInput_g [ 3 ] ; struct { real_T modelTStart ; }
TransportDelay_RWORK ; struct { real_T modelTStart ; } TransportDelay_RWORK_a
; struct { real_T modelTStart ; } TransportDelay_RWORK_k ; struct { real_T
modelTStart ; } TransportDelay_RWORK_ac ; void * Scope_PWORK [ 6 ] ; void *
Scope_PWORK_c ; void * Scope1_PWORK ; void * Scope2_PWORK ; void *
Scope3_PWORK ; void * Scope1_PWORK_a [ 3 ] ; void * Scope2_PWORK_b [ 3 ] ;
void * Scope3_PWORK_o [ 3 ] ; void * Scope4_PWORK ; void * Scope5_PWORK ;
void * Scope6_PWORK ; void * Scope_PWORK_p ; void * Scope1_PWORK_c ; void *
Scope2_PWORK_g ; void * Scope3_PWORK_p ; void * Scope2_PWORK_a [ 6 ] ; struct
{ void * TUbufferPtrs [ 2 ] ; } TransportDelay_PWORK ; void * Scope_PWORK_f ;
struct { void * TUbufferPtrs [ 2 ] ; } TransportDelay_PWORK_p ; struct { void
* TUbufferPtrs [ 2 ] ; } TransportDelay_PWORK_k ; struct { void *
TUbufferPtrs [ 2 ] ; } TransportDelay_PWORK_h ; void * Scope1_PWORK_k [ 4 ] ;
int32_T XYGraph_sysIdxToRun ; int32_T _sysIdxToRun ; int32_T
NRwCond_sysIdxToRun ; int32_T NewtonRaphsonSolver_sysIdxToRun ; int32_T
NRUpdate_sysIdxToRun ; int32_T ZeroIC_sysIdxToRun ; int32_T
MATLABFunction_sysIdxToRun ; int32_T ICsOutsideLimit_sysIdxToRun ; int32_T
MATLABFunction_sysIdxToRun_o ; int32_T HitLimits_sysIdxToRun ; int32_T
MATLABFunction_sysIdxToRun_d ; int32_T JCwReset_sysIdxToRun ; int32_T
JacobianCalculator_sysIdxToRun ; int32_T Jacobian_sysIdxToRun ; int32_T
JacobianCalc_sysIdxToRun ; int32_T Scalar_To_Workspace_sysIdxToRun ; int32_T
C_To_Workspace_sysIdxToRun ; int32_T Scalar_To_Workspace_sysIdxToRun_c ;
int32_T u_sysIdxToRun ; int32_T NRwCond_sysIdxToRun_d ; int32_T
NewtonRaphsonSolver_sysIdxToRun_h ; int32_T NRUpdate_sysIdxToRun_d ; int32_T
ZeroIC_sysIdxToRun_h ; int32_T MATLABFunction_sysIdxToRun_e ; int32_T
ICsOutsideLimit_sysIdxToRun_p ; int32_T MATLABFunction_sysIdxToRun_k ;
int32_T HitLimits_sysIdxToRun_d ; int32_T MATLABFunction_sysIdxToRun_b ;
int32_T JCwReset_sysIdxToRun_a ; int32_T JacobianCalculator_sysIdxToRun_g ;
int32_T Jacobian_sysIdxToRun_b ; int32_T JacobianCalc_sysIdxToRun_h ; int32_T
Scalar_To_Workspace_sysIdxToRun_f ; int32_T C_To_Workspace_sysIdxToRun_l ;
int32_T Scalar_To_Workspace_sysIdxToRun_d ; int32_T
IfActionSubsystem6_sysIdxToRun ; int32_T IfActionSubsystem1_sysIdxToRun ;
int32_T IfActionSubsystem1_sysIdxToRun_n ; int32_T
IfActionSubsystem2_sysIdxToRun ; int32_T IfActionSubsystem_sysIdxToRun ;
int32_T IfActionSubsystem1_sysIdxToRun_i ; int32_T
IfActionSubsystem2_sysIdxToRun_o ; int32_T EnabledSubsystem_sysIdxToRun ;
uint32_T SumofElements_DWORK1 ; int_T Integrator_IWORK ; int_T
Integrator_IWORK_k ; int_T Integrator_IWORK_b ; int_T Integrator_IWORK_e ;
struct { int_T Errors [ 5 ] ; } AmbientEnvtoEngine_IWORK ; struct { int_T
Errors [ 5 ] ; } Compressor_IWORK ; struct { int_T Errors [ 5 ] ; }
Turbine_IWORK ; struct { int_T Errors [ 16 ] ; } Nozzle_IWORK ; struct {
int_T Errors [ 5 ] ; } AmbientEnvtoEngine_IWORK_m ; struct { int_T Errors [ 5
] ; } Compressor_IWORK_d ; struct { int_T Errors [ 5 ] ; } Turbine_IWORK_m ;
struct { int_T Errors [ 16 ] ; } Nozzle_IWORK_d ; struct { int_T Tail ; int_T
Head ; int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK ; struct { int_T Tail ; int_T Head ; int_T Last ; int_T
CircularBufSize ; int_T MaxNewBufSize ; } TransportDelay_IWORK_c ; struct {
int_T Tail ; int_T Head ; int_T Last ; int_T CircularBufSize ; int_T
MaxNewBufSize ; } TransportDelay_IWORK_b ; struct { int_T Tail ; int_T Head ;
int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
TransportDelay_IWORK_j ; int_T Saturation_MODE ; int_T Max_MODE ; int_T
Max1_MODE ; int_T Saturation_MODE_i ; int8_T If1_ActiveSubsystem ; int8_T
If2_ActiveSubsystem ; int8_T If2_ActiveSubsystem_i ; int8_T
If1_ActiveSubsystem_g ; int8_T If3_ActiveSubsystem ; int8_T Sqrt_DWORK1 ;
int8_T Sqrt2_DWORK1 ; int8_T Sqrt1_DWORK1 ; int8_T Sqrt3_DWORK1 ; int8_T
If_ActiveSubsystem ; int8_T NRwCond_SubsysRanBC ; int8_T NRUpdate_SubsysRanBC
; int8_T ZeroIC_SubsysRanBC ; int8_T ICsOutsideLimit_SubsysRanBC ; int8_T
HitLimits_SubsysRanBC ; int8_T JCwReset_SubsysRanBC ; int8_T
Jacobian_SubsysRanBC ; int8_T Scalar_To_Workspace_SubsysRanBC ; int8_T
C_To_Workspace_SubsysRanBC ; int8_T Scalar_To_Workspace_SubsysRanBC_n ;
int8_T NRwCond_SubsysRanBC_n ; int8_T NRUpdate_SubsysRanBC_o ; int8_T
ZeroIC_SubsysRanBC_i ; int8_T ICsOutsideLimit_SubsysRanBC_d ; int8_T
HitLimits_SubsysRanBC_i ; int8_T JCwReset_SubsysRanBC_a ; int8_T
Jacobian_SubsysRanBC_c ; int8_T Scalar_To_Workspace_SubsysRanBC_a ; int8_T
C_To_Workspace_SubsysRanBC_p ; int8_T Scalar_To_Workspace_SubsysRanBC_j ;
int8_T IfActionSubsystem6_SubsysRanBC ; int8_T IfActionSubsystem1_SubsysRanBC
; int8_T If3_ActiveSubsystem_i ; int8_T IfActionSubsystem1_SubsysRanBC_g ;
int8_T If3_ActiveSubsystem_l ; int8_T IfActionSubsystem2_SubsysRanBC ; int8_T
IfActionSubsystem_SubsysRanBC ; int8_T IfActionSubsystem1_SubsysRanBC_p ;
int8_T IfActionSubsystem2_SubsysRanBC_a ; int8_T EnabledSubsystem_SubsysRanBC
; boolean_T Memory_PreviousInput_h [ 3 ] ; boolean_T Memory1_PreviousInput [
3 ] ; boolean_T Memory_PreviousInput_e [ 3 ] ; boolean_T
Memory1_PreviousInput_i [ 3 ] ; boolean_T NRwCond_MODE ; boolean_T
NRUpdate_MODE ; boolean_T ZeroIC_MODE ; boolean_T ICsOutsideLimit_MODE ;
boolean_T HitLimits_MODE ; boolean_T JCwReset_MODE ; boolean_T Jacobian_MODE
; boolean_T NRwCond_MODE_c ; boolean_T NRUpdate_MODE_p ; boolean_T
ZeroIC_MODE_c ; boolean_T ICsOutsideLimit_MODE_k ; boolean_T HitLimits_MODE_b
; boolean_T JCwReset_MODE_b ; boolean_T Jacobian_MODE_j ; boolean_T
EnabledSubsystem_MODE ; char_T pad_EnabledSubsystem_MODE [ 5 ] ;
DW_IfActionSubsystem3_TBCC_Dyn_T IfActionSubsystem5 ;
DW_IfActionSubsystem3_TBCC_Dyn_T IfActionSubsystem4_f ;
DW_IfActionSubsystem1_TBCC_Dyn_T IfActionSubsystem2_o ;
DW_IfActionSubsystem1_TBCC_Dyn_T IfActionSubsystem2_or ;
DW_IfActionSubsystem1_TBCC_Dyn_T IfActionSubsystem1_l ;
DW_IfActionSubsystem3_TBCC_Dyn_T IfActionSubsystem3_m ;
DW_IfActionSubsystem3_TBCC_Dyn_T IfActionSubsystem1_c ;
DW_IfActionSubsystem3_TBCC_Dyn_T IfActionSubsystem4 ;
DW_IfActionSubsystem3_TBCC_Dyn_T IfActionSubsystem3 ;
DW_NoiseGenerator_TBCC_Dyn_T NoiseGenerator_k ; DW_NoiseGenerator_TBCC_Dyn_T
NoiseGenerator ; } DW_TBCC_Dyn_T ; typedef struct { real_T TransferFcn_CSTATE
; real_T Integrator_CSTATE ; real_T Integrator_CSTATE_f ; real_T
Integrator_CSTATE_i ; real_T Integrator_CSTATE_fq ; real_T
Integrator_CSTATE_c ; real_T Integrator_CSTATE_p ; real_T StateSpace_CSTATE ;
X_IfActionSubsystem1_TBCC_Dyn_T IfActionSubsystem2_o ; real_T
StateSpace_CSTATE_m ; X_IfActionSubsystem1_TBCC_Dyn_T IfActionSubsystem2_or ;
X_IfActionSubsystem1_TBCC_Dyn_T IfActionSubsystem1_l ; real_T
StateSpace_CSTATE_o ; real_T TransferFcn1_CSTATE ; real_T TransferFcn2_CSTATE
; real_T StateSpace_CSTATE_e ; X_NoiseGenerator_TBCC_Dyn_T NoiseGenerator_k ;
X_NoiseGenerator_TBCC_Dyn_T NoiseGenerator ; } X_TBCC_Dyn_T ; typedef struct
{ real_T TransferFcn_CSTATE ; real_T Integrator_CSTATE ; real_T
Integrator_CSTATE_f ; real_T Integrator_CSTATE_i ; real_T
Integrator_CSTATE_fq ; real_T Integrator_CSTATE_c ; real_T
Integrator_CSTATE_p ; real_T StateSpace_CSTATE ;
XDot_IfActionSubsystem1_TBCC_Dyn_T IfActionSubsystem2_o ; real_T
StateSpace_CSTATE_m ; XDot_IfActionSubsystem1_TBCC_Dyn_T
IfActionSubsystem2_or ; XDot_IfActionSubsystem1_TBCC_Dyn_T
IfActionSubsystem1_l ; real_T StateSpace_CSTATE_o ; real_T
TransferFcn1_CSTATE ; real_T TransferFcn2_CSTATE ; real_T StateSpace_CSTATE_e
; XDot_NoiseGenerator_TBCC_Dyn_T NoiseGenerator_k ;
XDot_NoiseGenerator_TBCC_Dyn_T NoiseGenerator ; } XDot_TBCC_Dyn_T ; typedef
struct { boolean_T TransferFcn_CSTATE ; boolean_T Integrator_CSTATE ;
boolean_T Integrator_CSTATE_f ; boolean_T Integrator_CSTATE_i ; boolean_T
Integrator_CSTATE_fq ; boolean_T Integrator_CSTATE_c ; boolean_T
Integrator_CSTATE_p ; boolean_T StateSpace_CSTATE ;
XDis_IfActionSubsystem1_TBCC_Dyn_T IfActionSubsystem2_o ; boolean_T
StateSpace_CSTATE_m ; XDis_IfActionSubsystem1_TBCC_Dyn_T
IfActionSubsystem2_or ; XDis_IfActionSubsystem1_TBCC_Dyn_T
IfActionSubsystem1_l ; boolean_T StateSpace_CSTATE_o ; boolean_T
TransferFcn1_CSTATE ; boolean_T TransferFcn2_CSTATE ; boolean_T
StateSpace_CSTATE_e ; XDis_NoiseGenerator_TBCC_Dyn_T NoiseGenerator_k ;
XDis_NoiseGenerator_TBCC_Dyn_T NoiseGenerator ; } XDis_TBCC_Dyn_T ; typedef
struct { real_T TransferFcn_CSTATE ; real_T Integrator_CSTATE ; real_T
Integrator_CSTATE_f ; real_T Integrator_CSTATE_i ; real_T
Integrator_CSTATE_fq ; real_T Integrator_CSTATE_c ; real_T
Integrator_CSTATE_p ; real_T StateSpace_CSTATE ;
XAbsTol_IfActionSubsystem1_TBCC_Dyn_T IfActionSubsystem2_o ; real_T
StateSpace_CSTATE_m ; XAbsTol_IfActionSubsystem1_TBCC_Dyn_T
IfActionSubsystem2_or ; XAbsTol_IfActionSubsystem1_TBCC_Dyn_T
IfActionSubsystem1_l ; real_T StateSpace_CSTATE_o ; real_T
TransferFcn1_CSTATE ; real_T TransferFcn2_CSTATE ; real_T StateSpace_CSTATE_e
; XAbsTol_NoiseGenerator_TBCC_Dyn_T NoiseGenerator_k ;
XAbsTol_NoiseGenerator_TBCC_Dyn_T NoiseGenerator ; } CStateAbsTol_TBCC_Dyn_T
; typedef struct { real_T If1_IfInput_ZC ; real_T If2_IfInput_ZC ; real_T
Saturation_UprLim_ZC ; real_T Saturation_LwrLim_ZC ; real_T If2_IfInput_ZC_c
; real_T If1_IfInput_ZC_k ; real_T Max_MinmaxInput_ZC ; real_T If3_IfInput_ZC
; real_T Max1_MinmaxInput_ZC ; real_T If_IfInput_ZC ; real_T
Saturation_UprLim_ZC_o ; real_T Saturation_LwrLim_ZC_b ; real_T
If3_IfInput_ZC_a ; real_T If3_IfInput_ZC_k ; real_T
EnabledSubsystem_Enable_ZC ; } ZCV_TBCC_Dyn_T ; typedef struct { ZCSigState
If1_IfInput_ZCE ; ZCSigState If2_IfInput_ZCE ; ZCSigState
Saturation_UprLim_ZCE ; ZCSigState Saturation_LwrLim_ZCE ; ZCSigState
If2_IfInput_ZCE_h ; ZCSigState If1_IfInput_ZCE_b ; ZCSigState
Max_MinmaxInput_ZCE ; ZCSigState If3_IfInput_ZCE ; ZCSigState
Max1_MinmaxInput_ZCE ; ZCSigState If_IfInput_ZCE ; ZCSigState
Saturation_UprLim_ZCE_h ; ZCSigState Saturation_LwrLim_ZCE_c ; ZCSigState
If3_IfInput_ZCE_m ; ZCSigState If3_IfInput_ZCE_n ; ZCSigState
EnabledSubsystem_Enable_ZCE ; } PrevZCX_TBCC_Dyn_T ; typedef struct { const
real_T B_52_3_0 ; const real_T B_38_0_0 ; const real_T B_37_0_0 ; const
real_T B_36_0_0 ; const real_T B_35_3_0 ; const real_T B_21_0_0 ; const
real_T B_20_0_0 ; const real_T B_19_0_0 ; } ConstB_TBCC_Dyn_T ;
#define TBCC_Dyn_rtC(S) ((ConstB_TBCC_Dyn_T *) _ssGetConstBlockIO(S))
typedef struct { real_T pooled1 [ 2 ] ; real_T pooled2 [ 86 ] ; real_T
pooled3 [ 2 ] ; real_T pooled4 [ 83 ] ; } ConstP_TBCC_Dyn_T ; struct
P_NoiseGenerator_TBCC_Dyn_T_ { real_T P_0 ; real_T P_1 ; real_T P_2 ; real_T
P_3 ; real_T P_4 ; real_T P_5 ; real_T P_6 ; real_T P_7 ; real_T P_8 ; real_T
P_9 ; real_T P_10 ; } ; struct P_IfActionSubsystem1_TBCC_Dyn_T_ { real_T P_0
; real_T P_1 ; real_T P_2 ; real_T P_3 ; } ; struct P_TBCC_Dyn_T_ { real_T
P_0 ; real_T P_1 ; real_T P_2 ; real_T P_3 ; real_T P_4 ; real_T P_5 ; real_T
P_6 ; real_T P_7 ; real_T P_8 ; real_T P_9 ; real_T P_10 ; real_T P_11 ;
real_T P_12 ; real_T P_13 ; real_T P_14 ; real_T P_15 ; real_T P_16 ; real_T
P_17 ; real_T P_18 ; real_T P_19 ; real_T P_20 [ 7 ] ; real_T P_21 [ 7 ] ;
real_T P_22 ; real_T P_23 ; real_T P_24 ; real_T P_25 ; real_T P_27 [ 7 ] ;
real_T P_28 [ 7 ] ; real_T P_29 ; real_T P_30 ; real_T P_31 ; real_T P_32 ;
real_T P_33 [ 3 ] ; real_T P_34 [ 9 ] ; real_T P_35 [ 21 ] ; real_T P_36 ;
real_T P_37 ; real_T P_38 [ 6 ] ; real_T P_39 ; real_T P_40 ; real_T P_41 ;
real_T P_42 [ 3 ] ; real_T P_43 [ 3 ] ; real_T P_44 [ 3 ] ; real_T P_45 [ 3 ]
; real_T P_46 [ 3 ] ; real_T P_47 [ 3 ] ; real_T P_48 ; real_T P_49 ; real_T
P_50 ; real_T P_51 [ 3 ] ; real_T P_52 ; real_T P_53 ; real_T P_54 [ 15 ] ;
real_T P_55 [ 15 ] ; real_T P_56 [ 15 ] ; real_T P_57 [ 7 ] ; real_T P_58 [ 2
] ; real_T P_59 [ 7 ] ; real_T P_60 [ 14 ] ; real_T P_61 ; real_T P_62 [ 2 ]
; real_T P_63 [ 2 ] ; real_T P_64 [ 3 ] ; real_T P_65 [ 13 ] ; real_T P_66 [
11 ] ; real_T P_67 ; real_T P_68 [ 143 ] ; real_T P_69 [ 143 ] ; real_T P_70
[ 143 ] ; real_T P_71 ; real_T P_72 ; real_T P_73 ; real_T P_74 ; real_T P_75
[ 14 ] ; real_T P_76 [ 14 ] ; real_T P_77 ; real_T P_78 ; real_T P_79 ;
real_T P_80 ; real_T P_81 ; real_T P_82 ; real_T P_83 ; real_T P_84 ; real_T
P_85 ; real_T P_86 ; real_T P_87 ; real_T P_88 ; real_T P_89 ; real_T P_90 ;
real_T P_91 ; real_T P_92 ; real_T P_93 ; real_T P_94 ; real_T P_95 ; real_T
P_96 ; real_T P_97 ; real_T P_98 ; real_T P_99 ; real_T P_100 ; real_T P_101
[ 54 ] ; real_T P_102 ; real_T P_103 [ 2 ] ; real_T P_104 [ 6 ] ; real_T
P_105 [ 20 ] ; real_T P_106 [ 120 ] ; real_T P_107 [ 120 ] ; real_T P_108 ;
real_T P_109 ; real_T P_110 ; real_T P_111 ; real_T P_112 ; real_T P_113 ;
real_T P_114 ; real_T P_115 ; real_T P_116 ; real_T P_117 ; real_T P_118 ;
real_T P_119 ; real_T P_120 ; real_T P_121 ; real_T P_122 ; real_T P_123 [ 8
] ; real_T P_124 [ 8 ] ; real_T P_125 [ 18 ] ; real_T P_126 [ 144 ] ; real_T
P_127 [ 2 ] ; real_T P_128 [ 2 ] ; real_T P_129 [ 2 ] ; real_T P_130 [ 2 ] ;
real_T P_131 [ 18 ] ; real_T P_132 [ 18 ] ; real_T P_133 ; real_T P_134 ;
real_T P_135 ; real_T P_136 ; real_T P_137 ; real_T P_138 [ 51 ] ; real_T
P_139 ; real_T P_140 ; real_T P_141 ; real_T P_142 ; real_T P_143 ; real_T
P_144 ; real_T P_145 ; real_T P_146 ; real_T P_147 ; real_T P_148 ; real_T
P_149 ; real_T P_150 ; real_T P_151 ; real_T P_152 ; real_T P_153 ; real_T
P_154 ; real_T P_155 ; real_T P_156 ; real_T P_157 ; real_T P_158 ; real_T
P_159 ; real_T P_160 ; real_T P_161 ; real_T P_162 ; real_T P_163 ; real_T
P_164 ; real_T P_165 ; real_T P_166 ; real_T P_167 ; real_T P_168 ; real_T
P_169 ; real_T P_170 [ 3 ] ; real_T P_171 [ 9 ] ; real_T P_172 [ 21 ] ;
real_T P_173 ; real_T P_174 ; real_T P_175 [ 6 ] ; real_T P_176 ; real_T
P_177 ; real_T P_178 ; real_T P_179 [ 3 ] ; real_T P_180 [ 3 ] ; real_T P_181
[ 3 ] ; real_T P_182 [ 3 ] ; real_T P_183 [ 3 ] ; real_T P_184 [ 3 ] ; real_T
P_185 ; real_T P_186 ; real_T P_187 ; real_T P_188 [ 3 ] ; real_T P_189 ;
real_T P_190 ; real_T P_191 [ 15 ] ; real_T P_192 [ 15 ] ; real_T P_193 [ 15
] ; real_T P_194 [ 7 ] ; real_T P_195 [ 2 ] ; real_T P_196 [ 7 ] ; real_T
P_197 [ 14 ] ; real_T P_198 ; real_T P_199 [ 2 ] ; real_T P_200 [ 2 ] ;
real_T P_201 [ 3 ] ; real_T P_202 [ 13 ] ; real_T P_203 [ 11 ] ; real_T P_204
; real_T P_205 [ 143 ] ; real_T P_206 [ 143 ] ; real_T P_207 [ 143 ] ; real_T
P_208 ; real_T P_209 ; real_T P_210 ; real_T P_211 ; real_T P_212 [ 14 ] ;
real_T P_213 [ 14 ] ; real_T P_214 ; real_T P_215 ; real_T P_216 ; real_T
P_217 ; real_T P_218 ; real_T P_219 ; real_T P_220 ; real_T P_221 ; real_T
P_222 ; real_T P_223 ; real_T P_224 ; real_T P_225 ; real_T P_226 ; real_T
P_227 ; real_T P_228 ; real_T P_229 ; real_T P_230 ; real_T P_231 ; real_T
P_232 ; real_T P_233 ; real_T P_234 ; real_T P_235 ; real_T P_236 ; real_T
P_237 ; real_T P_238 [ 51 ] ; real_T P_239 ; real_T P_240 [ 2 ] ; real_T
P_241 [ 6 ] ; real_T P_242 [ 20 ] ; real_T P_243 [ 120 ] ; real_T P_244 [ 120
] ; real_T P_245 ; real_T P_246 ; real_T P_247 ; real_T P_248 ; real_T P_249
; real_T P_250 ; real_T P_251 ; real_T P_252 ; real_T P_253 ; real_T P_254 ;
real_T P_255 ; real_T P_256 ; real_T P_257 ; real_T P_258 ; real_T P_259 ;
real_T P_260 [ 8 ] ; real_T P_261 [ 8 ] ; real_T P_262 [ 18 ] ; real_T P_263
[ 144 ] ; real_T P_264 [ 2 ] ; real_T P_265 [ 2 ] ; real_T P_266 [ 2 ] ;
real_T P_267 [ 2 ] ; real_T P_268 [ 18 ] ; real_T P_269 [ 18 ] ; real_T P_270
; real_T P_271 ; real_T P_272 ; real_T P_273 ; real_T P_274 ; real_T P_275 [
48 ] ; real_T P_276 ; real_T P_277 ; real_T P_278 ; real_T P_279 ; real_T
P_280 ; real_T P_281 ; real_T P_282 ; real_T P_283 ; real_T P_284 ; real_T
P_285 ; real_T P_286 ; real_T P_287 ; real_T P_288 ; real_T P_289 ; real_T
P_290 ; real_T P_291 ; real_T P_292 ; real_T P_293 ; real_T P_294 ; real_T
P_295 ; real_T P_296 ; real_T P_297 ; real_T P_298 ; real_T P_299 ; real_T
P_300 ; real_T P_301 ; real_T P_302 ; real_T P_303 ; real_T P_304 ; real_T
P_305 ; real_T P_306 ; real_T P_307 ; real_T P_308 ; real_T P_309 ; real_T
P_310 [ 7 ] ; real_T P_311 [ 7 ] ; real_T P_312 ; real_T P_313 ; real_T P_314
; real_T P_315 ; real_T P_316 ; real_T P_317 ; real_T P_318 [ 7 ] ; real_T
P_319 [ 7 ] ; real_T P_320 [ 7 ] ; real_T P_321 [ 7 ] ; real_T P_322 [ 7 ] ;
real_T P_323 [ 7 ] ; real_T P_324 ; real_T P_325 ; real_T P_326 ; real_T
P_327 ; real_T P_328 ; real_T P_329 ; real_T P_330 ; real_T P_331 ; real_T
P_332 ; real_T P_333 ; real_T P_334 ; real_T P_335 ; real_T P_336 ; real_T
P_337 ; real_T P_338 ; real_T P_339 ; real_T P_340 ; real_T P_341 ; real_T
P_342 ; real_T P_343 ; real_T P_344 ; real_T P_345 ; real_T P_346 ; real_T
P_347 ; real_T P_348 ; real_T P_349 ; real_T P_350 ; real_T P_351 ; real_T
P_352 ; real_T P_353 ; real_T P_354 ; real_T P_355 ; real_T P_356 ; real_T
P_357 ; real_T P_358 ; real_T P_359 ; real_T P_360 [ 7 ] ; real_T P_361 [ 7 ]
; real_T P_362 ; real_T P_363 ; real_T P_364 ; real_T P_365 ; real_T P_366 ;
real_T P_367 ; real_T P_368 ; real_T P_369 ; real_T P_370 ; real_T P_371 ;
real_T P_372 ; real_T P_373 ; real_T P_374 [ 7 ] ; real_T P_375 [ 7 ] ;
real_T P_376 [ 7 ] ; real_T P_377 [ 7 ] ; real_T P_378 [ 7 ] ; real_T P_379 [
7 ] ; real_T P_380 ; real_T P_381 ; real_T P_382 ; real_T P_383 ; real_T
P_384 ; real_T P_385 ; real_T P_386 ; real_T P_387 ; real_T P_388 ; real_T
P_389 ; real_T P_390 ; real_T P_391 ; real_T P_392 ; real_T P_393 ; real_T
P_394 ; real_T P_395 ; real_T P_396 ; real_T P_397 ; int32_T P_398 ; int32_T
P_399 ; uint32_T P_400 ; uint32_T P_401 ; boolean_T P_402 ; boolean_T P_403 ;
boolean_T P_404 ; boolean_T P_405 ; char_T pad_P_405 [ 4 ] ;
P_IfActionSubsystem1_TBCC_Dyn_T IfActionSubsystem2_o ;
P_IfActionSubsystem1_TBCC_Dyn_T IfActionSubsystem2_or ;
P_IfActionSubsystem1_TBCC_Dyn_T IfActionSubsystem1_l ;
P_NoiseGenerator_TBCC_Dyn_T NoiseGenerator_k ; P_NoiseGenerator_TBCC_Dyn_T
NoiseGenerator ; } ; extern P_TBCC_Dyn_T TBCC_Dyn_rtDefaultP ; extern const
ConstB_TBCC_Dyn_T TBCC_Dyn_rtInvariant ; extern const ConstP_TBCC_Dyn_T
TBCC_Dyn_rtConstP ;
#endif
