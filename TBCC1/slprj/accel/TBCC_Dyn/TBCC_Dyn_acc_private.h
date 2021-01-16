#include "__cf_TBCC_Dyn.h"
#ifndef RTW_HEADER_TBCC_Dyn_acc_private_h_
#define RTW_HEADER_TBCC_Dyn_acc_private_h_
#include "rtwtypes.h"
#include "multiword_types.h"
#include "TBCC_Dyn_acc.h"
#if !defined(ss_VALIDATE_MEMORY)
#define ss_VALIDATE_MEMORY(S, ptr)   if(!(ptr)) {\
  ssSetErrorStatus(S, RT_MEMORY_ALLOCATION_ERROR);\
  }
#endif
#if !defined(rt_FREE)
#if !defined(_WIN32)
#define rt_FREE(ptr)   if((ptr) != (NULL)) {\
  free((ptr));\
  (ptr) = (NULL);\
  }
#else
#define rt_FREE(ptr)   if((ptr) != (NULL)) {\
  free((void *)(ptr));\
  (ptr) = (NULL);\
  }
#endif
#endif
#ifndef __RTW_UTFREE__
extern void * utMalloc ( size_t ) ; extern void utFree ( void * ) ;
#endif
extern real_T rt_urand_Upu32_Yd_f_pw_snf ( uint32_T * u ) ; extern real_T
rt_nrand_Upu32_Yd_f_pw_snf ( uint32_T * u ) ; boolean_T
TBCC_Dyn_acc_rt_TDelayUpdateTailOrGrowBuf ( int_T * bufSzPtr , int_T *
tailPtr , int_T * headPtr , int_T * lastPtr , real_T tMinusDelay , real_T * *
tBufPtr , real_T * * uBufPtr , real_T * * xBufPtr , boolean_T isfixedbuf ,
boolean_T istransportdelay , int_T * maxNewBufSzPtr ) ; real_T
TBCC_Dyn_acc_rt_TDelayInterpolate ( real_T tMinusDelay , real_T tStart ,
real_T * tBuf , real_T * uBuf , int_T bufSz , int_T * lastIdx , int_T
oldestIdx , int_T newIdx , real_T initOutput , boolean_T discrete , boolean_T
minorStepAndTAtLastMajorOutput ) ; extern real_T look1_binlcapw ( real_T u0 ,
const real_T bp0 [ ] , const real_T table [ ] , uint32_T maxIndex ) ; void
TBCC_Dyn_NoiseGenerator_Init ( SimStruct * S , DW_NoiseGenerator_TBCC_Dyn_T *
localDW , P_NoiseGenerator_TBCC_Dyn_T * localP , X_NoiseGenerator_TBCC_Dyn_T
* localX ) ; void TBCC_Dyn_NoiseGenerator_Deriv ( SimStruct * S ,
B_NoiseGenerator_TBCC_Dyn_T * localB , DW_NoiseGenerator_TBCC_Dyn_T * localDW
, P_NoiseGenerator_TBCC_Dyn_T * localP , X_NoiseGenerator_TBCC_Dyn_T * localX
, XDot_NoiseGenerator_TBCC_Dyn_T * localXdot ) ; void
TBCC_Dyn_NoiseGenerator_Disable ( SimStruct * S ,
DW_NoiseGenerator_TBCC_Dyn_T * localDW ) ; void
TBCC_Dyn_NoiseGenerator_Update ( SimStruct * S , DW_NoiseGenerator_TBCC_Dyn_T
* localDW , P_NoiseGenerator_TBCC_Dyn_T * localP ) ; void
TBCC_Dyn_NoiseGenerator ( SimStruct * S , real_T rtu_Enable , real_T
rtu_actual , real_T rtu_sensormaxrange , B_NoiseGenerator_TBCC_Dyn_T * localB
, DW_NoiseGenerator_TBCC_Dyn_T * localDW , P_NoiseGenerator_TBCC_Dyn_T *
localP , X_NoiseGenerator_TBCC_Dyn_T * localX ,
XDis_NoiseGenerator_TBCC_Dyn_T * localXdis ) ; void
TBCC_Dyn_IfActionSubsystem3_Enable ( SimStruct * S ) ; void
TBCC_Dyn_IfActionSubsystem3_Disable ( SimStruct * S ) ; void
TBCC_Dyn_IfActionSubsystem3 ( SimStruct * S , real_T rtu_In1 , real_T *
rty_Out1 ) ; void TBCC_Dyn_IfActionSubsystem1_Init ( SimStruct * S ,
P_IfActionSubsystem1_TBCC_Dyn_T * localP , X_IfActionSubsystem1_TBCC_Dyn_T *
localX ) ; void TBCC_Dyn_IfActionSubsystem1_Enable ( SimStruct * S ,
XDis_IfActionSubsystem1_TBCC_Dyn_T * localXdis ) ; void
TBCC_Dyn_IfActionSubsystem1_Deriv ( SimStruct * S , real_T rtu_In1 ,
P_IfActionSubsystem1_TBCC_Dyn_T * localP , X_IfActionSubsystem1_TBCC_Dyn_T *
localX , XDot_IfActionSubsystem1_TBCC_Dyn_T * localXdot ) ; void
TBCC_Dyn_IfActionSubsystem1_Disable ( SimStruct * S ,
XDis_IfActionSubsystem1_TBCC_Dyn_T * localXdis ) ; void
TBCC_Dyn_IfActionSubsystem1 ( SimStruct * S , real_T * rty_Out1 ,
P_IfActionSubsystem1_TBCC_Dyn_T * localP , X_IfActionSubsystem1_TBCC_Dyn_T *
localX ) ;
#endif
