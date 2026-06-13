/*
 * File: controlador_efo.h
 *
 * Code generated for Simulink model 'controlador_efo'.
 *
 * Model version                  : 1.4
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Thu Mar 19 17:29:45 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_controlador_efo_h_
#define RTW_HEADER_controlador_efo_h_
#include <string.h>
#include <stddef.h>
#ifndef controlador_efo_COMMON_INCLUDES_
# define controlador_efo_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* controlador_efo_COMMON_INCLUDES_ */

#include "controlador_efo_types.h"
#include "MW_target_hardware_resources.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real_T DiscreteZeroPole_DSTATE[12];  /* '<Root>/Discrete Zero-Pole' */
  real_T y_prev;                       /* '<Root>/MATLAB Function' */
  real_T ultimo_vale;                  /* '<Root>/MATLAB Function' */
  real_T A_out;                        /* '<Root>/MATLAB Function' */
  real_T T_out;                        /* '<Root>/MATLAB Function' */
  uint32_T contador;                   /* '<Root>/MATLAB Function' */
  boolean_T Relay_Mode;                /* '<Root>/Relay' */
  boolean_T y_prev_not_empty;          /* '<Root>/MATLAB Function' */
  boolean_T subindo;                   /* '<Root>/MATLAB Function' */
  boolean_T primeiro_pico;             /* '<Root>/MATLAB Function' */
} DW_controlador_efo_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T y;                            /* '<Root>/y ' */
  real_T r;                            /* '<Root>/r' */
} ExtU_controlador_efo_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T u;                            /* '<Root>/u' */
  real_T A;                            /* '<Root>/A' */
  real_T T;                            /* '<Root>/T' */
} ExtY_controlador_efo_T;

/* Parameters (default storage) */
struct P_controlador_efo_T_ {
  real_T Relay_OnVal;                  /* Expression: 0
                                        * Referenced by: '<Root>/Relay'
                                        */
  real_T Relay_OffVal;                 /* Expression: 0
                                        * Referenced by: '<Root>/Relay'
                                        */
  real_T Relay_YOn;                    /* Expression: 1
                                        * Referenced by: '<Root>/Relay'
                                        */
  real_T Relay_YOff;                   /* Expression: -1
                                        * Referenced by: '<Root>/Relay'
                                        */
  real_T Gain_Gain;                    /* Expression: 10
                                        * Referenced by: '<Root>/Gain'
                                        */
  real_T DiscreteZeroPole_A[48];       /* Computed Parameter: DiscreteZeroPole_A
                                        * Referenced by: '<Root>/Discrete Zero-Pole'
                                        */
  real_T DiscreteZeroPole_B[6];        /* Computed Parameter: DiscreteZeroPole_B
                                        * Referenced by: '<Root>/Discrete Zero-Pole'
                                        */
  real_T DiscreteZeroPole_C[12];       /* Computed Parameter: DiscreteZeroPole_C
                                        * Referenced by: '<Root>/Discrete Zero-Pole'
                                        */
  real_T DiscreteZeroPole_D;           /* Computed Parameter: DiscreteZeroPole_D
                                        * Referenced by: '<Root>/Discrete Zero-Pole'
                                        */
  real_T Constant1_Value;              /* Expression: 0.001
                                        * Referenced by: '<Root>/Constant1'
                                        */
};

/* Real-time Model Data Structure */
struct tag_RTM_controlador_efo_T {
  const char_T *errorStatus;
};

/* Block parameters (default storage) */
extern P_controlador_efo_T controlador_efo_P;

/* Block states (default storage) */
extern DW_controlador_efo_T controlador_efo_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_controlador_efo_T controlador_efo_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_controlador_efo_T controlador_efo_Y;

/* Model entry point functions */
extern void controlador_efo_initialize(void);
extern void controlador_efo_step(void);
extern void controlador_efo_terminate(void);

/* Real-time Model object */
extern RT_MODEL_controlador_efo_T *const controlador_efo_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'controlador_efo'
 * '<S1>'   : 'controlador_efo/MATLAB Function'
 */
#endif                                 /* RTW_HEADER_controlador_efo_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
