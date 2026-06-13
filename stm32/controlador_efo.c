/*
 * File: controlador_efo.c
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

#include "controlador_efo.h"
#include "controlador_efo_private.h"

/* Block states (default storage) */
DW_controlador_efo_T controlador_efo_DW;

/* External inputs (root inport signals with default storage) */
ExtU_controlador_efo_T controlador_efo_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_controlador_efo_T controlador_efo_Y;

/* Real-time model */
RT_MODEL_controlador_efo_T controlador_efo_M_;
RT_MODEL_controlador_efo_T *const controlador_efo_M = &controlador_efo_M_;

/* Model step function */
void controlador_efo_step(void)
{
  /* local block i/o variables */
  real_T rtb_Gain;
  real_T rtb_Relay;
  uint32_T qY;

  /* Sum: '<Root>/Sum' incorporates:
   *  Inport: '<Root>/r'
   *  Inport: '<Root>/y '
   */
  rtb_Relay = controlador_efo_U.r - controlador_efo_U.y;

  /* Relay: '<Root>/Relay' */
  if (rtb_Relay >= controlador_efo_P.Relay_OnVal) {
    controlador_efo_DW.Relay_Mode = true;
  } else {
    if (rtb_Relay <= controlador_efo_P.Relay_OffVal) {
      controlador_efo_DW.Relay_Mode = false;
    }
  }

  if (controlador_efo_DW.Relay_Mode) {
    rtb_Relay = controlador_efo_P.Relay_YOn;
  } else {
    rtb_Relay = controlador_efo_P.Relay_YOff;
  }

  /* End of Relay: '<Root>/Relay' */

  /* Gain: '<Root>/Gain' */
  rtb_Gain = controlador_efo_P.Gain_Gain * rtb_Relay;

  /* DiscreteZeroPole: '<Root>/Discrete Zero-Pole' incorporates:
   *  Outport: '<Root>/u'
   */
  {
    {
      static const int_T colCidxRow0[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
        11 };

      const int_T *pCidx = &colCidxRow0[0];
      const real_T *pC0 = controlador_efo_P.DiscreteZeroPole_C;
      const real_T *xd = &controlador_efo_DW.DiscreteZeroPole_DSTATE[0];
      real_T *y0 = &controlador_efo_Y.u;
      int_T numNonZero = 11;
      *y0 = (*pC0++) * xd[*pCidx++];
      while (numNonZero--) {
        *y0 += (*pC0++) * xd[*pCidx++];
      }
    }

    controlador_efo_Y.u += controlador_efo_P.DiscreteZeroPole_D*rtb_Gain;
  }

  /* MATLAB Function: '<Root>/MATLAB Function' incorporates:
   *  Constant: '<Root>/Constant1'
   *  Inport: '<Root>/y '
   */
  if (!controlador_efo_DW.y_prev_not_empty) {
    controlador_efo_DW.y_prev = controlador_efo_U.y;
    controlador_efo_DW.y_prev_not_empty = true;
  }

  qY = controlador_efo_DW.contador + /*MW:OvSatOk*/ 1U;
  if (qY < controlador_efo_DW.contador) {
    qY = MAX_uint32_T;
  }

  controlador_efo_DW.contador = qY;
  if (controlador_efo_U.y > controlador_efo_DW.y_prev) {
    if (!controlador_efo_DW.subindo) {
      controlador_efo_DW.ultimo_vale = controlador_efo_DW.y_prev;
      controlador_efo_DW.subindo = true;
    }
  } else {
    if ((controlador_efo_U.y < controlador_efo_DW.y_prev) &&
        controlador_efo_DW.subindo) {
      controlador_efo_DW.subindo = false;
      controlador_efo_DW.A_out = (controlador_efo_DW.y_prev -
        controlador_efo_DW.ultimo_vale) / 2.0;
      if (controlador_efo_DW.primeiro_pico) {
        controlador_efo_DW.primeiro_pico = false;
      } else {
        controlador_efo_DW.T_out = (real_T)controlador_efo_DW.contador *
          controlador_efo_P.Constant1_Value;
      }

      controlador_efo_DW.contador = 0U;
    }
  }

  controlador_efo_DW.y_prev = controlador_efo_U.y;

  /* Outport: '<Root>/A' incorporates:
   *  MATLAB Function: '<Root>/MATLAB Function'
   */
  controlador_efo_Y.A = controlador_efo_DW.A_out;

  /* Outport: '<Root>/T' incorporates:
   *  MATLAB Function: '<Root>/MATLAB Function'
   */
  controlador_efo_Y.T = controlador_efo_DW.T_out;

  /* Update for DiscreteZeroPole: '<Root>/Discrete Zero-Pole' incorporates:
   *  Outport: '<Root>/u'
   */
  {
    real_T xnew[12];
    xnew[0] = (controlador_efo_P.DiscreteZeroPole_A[0])*
      controlador_efo_DW.DiscreteZeroPole_DSTATE[0]
      + (controlador_efo_P.DiscreteZeroPole_A[1])*
      controlador_efo_DW.DiscreteZeroPole_DSTATE[1];
    xnew[0] += (controlador_efo_P.DiscreteZeroPole_B[0])*rtb_Gain;
    xnew[1] = (controlador_efo_P.DiscreteZeroPole_A[2])*
      controlador_efo_DW.DiscreteZeroPole_DSTATE[0];
    xnew[2] = (controlador_efo_P.DiscreteZeroPole_A[3])*
      controlador_efo_DW.DiscreteZeroPole_DSTATE[0]
      + (controlador_efo_P.DiscreteZeroPole_A[4])*
      controlador_efo_DW.DiscreteZeroPole_DSTATE[1]
      + (controlador_efo_P.DiscreteZeroPole_A[5])*
      controlador_efo_DW.DiscreteZeroPole_DSTATE[2]
      + (controlador_efo_P.DiscreteZeroPole_A[6])*
      controlador_efo_DW.DiscreteZeroPole_DSTATE[3];
    xnew[2] += (controlador_efo_P.DiscreteZeroPole_B[1])*rtb_Gain;
    xnew[3] = (controlador_efo_P.DiscreteZeroPole_A[7])*
      controlador_efo_DW.DiscreteZeroPole_DSTATE[2];

    {
      static const int_T colAidxRow4[6] = { 0, 1, 2, 3, 4, 5 };

      const int_T *pAidx = &colAidxRow4[0];
      const real_T *pA8 = &controlador_efo_P.DiscreteZeroPole_A[8];
      const real_T *xd = &controlador_efo_DW.DiscreteZeroPole_DSTATE[0];
      real_T *pxnew4 = &xnew[4];
      int_T numNonZero = 5;
      *pxnew4 = (*pA8++) * xd[*pAidx++];
      while (numNonZero--) {
        *pxnew4 += (*pA8++) * xd[*pAidx++];
      }
    }

    xnew[4] += (controlador_efo_P.DiscreteZeroPole_B[2])*rtb_Gain;
    xnew[5] = (controlador_efo_P.DiscreteZeroPole_A[14])*
      controlador_efo_DW.DiscreteZeroPole_DSTATE[4];

    {
      static const int_T colAidxRow6[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };

      const int_T *pAidx = &colAidxRow6[0];
      const real_T *pA15 = &controlador_efo_P.DiscreteZeroPole_A[15];
      const real_T *xd = &controlador_efo_DW.DiscreteZeroPole_DSTATE[0];
      real_T *pxnew6 = &xnew[6];
      int_T numNonZero = 7;
      *pxnew6 = (*pA15++) * xd[*pAidx++];
      while (numNonZero--) {
        *pxnew6 += (*pA15++) * xd[*pAidx++];
      }
    }

    xnew[6] += (controlador_efo_P.DiscreteZeroPole_B[3])*rtb_Gain;
    xnew[7] = (controlador_efo_P.DiscreteZeroPole_A[23])*
      controlador_efo_DW.DiscreteZeroPole_DSTATE[6];

    {
      static const int_T colAidxRow8[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

      const int_T *pAidx = &colAidxRow8[0];
      const real_T *pA24 = &controlador_efo_P.DiscreteZeroPole_A[24];
      const real_T *xd = &controlador_efo_DW.DiscreteZeroPole_DSTATE[0];
      real_T *pxnew8 = &xnew[8];
      int_T numNonZero = 9;
      *pxnew8 = (*pA24++) * xd[*pAidx++];
      while (numNonZero--) {
        *pxnew8 += (*pA24++) * xd[*pAidx++];
      }
    }

    xnew[8] += (controlador_efo_P.DiscreteZeroPole_B[4])*rtb_Gain;
    xnew[9] = (controlador_efo_P.DiscreteZeroPole_A[34])*
      controlador_efo_DW.DiscreteZeroPole_DSTATE[8];

    {
      static const int_T colAidxRow10[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
        11 };

      const int_T *pAidx = &colAidxRow10[0];
      const real_T *pA35 = &controlador_efo_P.DiscreteZeroPole_A[35];
      const real_T *xd = &controlador_efo_DW.DiscreteZeroPole_DSTATE[0];
      real_T *pxnew10 = &xnew[10];
      int_T numNonZero = 11;
      *pxnew10 = (*pA35++) * xd[*pAidx++];
      while (numNonZero--) {
        *pxnew10 += (*pA35++) * xd[*pAidx++];
      }
    }

    xnew[10] += (controlador_efo_P.DiscreteZeroPole_B[5])*rtb_Gain;
    xnew[11] = (controlador_efo_P.DiscreteZeroPole_A[47])*
      controlador_efo_DW.DiscreteZeroPole_DSTATE[10];
    (void) memcpy(&controlador_efo_DW.DiscreteZeroPole_DSTATE[0], xnew,
                  sizeof(real_T)*12);
  }
}

/* Model initialize function */
void controlador_efo_initialize(void)
{
  /* Registration code */

  /* initialize error status */
  rtmSetErrorStatus(controlador_efo_M, (NULL));

  /* states (dwork) */
  (void) memset((void *)&controlador_efo_DW, 0,
                sizeof(DW_controlador_efo_T));

  /* external inputs */
  (void)memset((void *)&controlador_efo_U, 0, sizeof(ExtU_controlador_efo_T));

  /* external outputs */
  (void) memset((void *)&controlador_efo_Y, 0,
                sizeof(ExtY_controlador_efo_T));

  /* InitializeConditions for DiscreteZeroPole: '<Root>/Discrete Zero-Pole' incorporates:
   *  Outport: '<Root>/u'
   */

  /* SystemInitialize for MATLAB Function: '<Root>/MATLAB Function' */
  controlador_efo_DW.y_prev_not_empty = false;
  controlador_efo_DW.subindo = true;
  controlador_efo_DW.ultimo_vale = 0.0;
  controlador_efo_DW.contador = 0U;
  controlador_efo_DW.A_out = 0.0;
  controlador_efo_DW.T_out = 0.0;
  controlador_efo_DW.primeiro_pico = true;
}

/* Model terminate function */
void controlador_efo_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
