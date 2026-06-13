/*
 * File: controlador_efo_data.c
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

/* Block parameters (default storage) */
P_controlador_efo_T controlador_efo_P = {
  /* Expression: 0
   * Referenced by: '<Root>/Relay'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<Root>/Relay'
   */
  0.0,

  /* Expression: 1
   * Referenced by: '<Root>/Relay'
   */
  1.0,

  /* Expression: -1
   * Referenced by: '<Root>/Relay'
   */
  -1.0,

  /* Expression: 10
   * Referenced by: '<Root>/Gain'
   */
  10.0,


  /* Computed Parameter: DiscreteZeroPole_A
   * Referenced by: '<Root>/Discrete Zero-Pole'
   */
  { 1.2755965341618367E-8, -2.3262489154958196E-18, 2.32624891549582E-18,
    33.48807757351905, -4.8145948333505768E+10, 0.22577406630238636,
    -0.034144624816112579, 0.034144624816112586, 33.48807757351905,
    -4.8145948333505768E+10, 0.14574760585024291, -0.029915282879109904,
    1.5284204215884127, -0.75510451363541642, 0.75510451363541642,
    33.48807757351905, -4.8145948333505768E+10, 0.14574760585024291,
    -0.029915282879109904, 0.3900501858765959, -0.36986848574864667,
    1.9539759933843446, -0.97690665853916481, 0.97690665853916492,
    33.48807757351905, -4.8145948333505768E+10, 0.14574760585024291,
    -0.029915282879109904, 0.3900501858765959, -0.36986848574864667,
    0.057319558571440554, -0.057123185274628889, 1.9961182298606264,
    -0.99805854003169225, 0.998058540031692, 33.48807757351905,
    -4.8145948333505768E+10, 0.14574760585024291, -0.029915282879109904,
    0.3900501858765959, -0.36986848574864667, 0.057319558571440554,
    -0.057123185274628889, 0.0049962351110071079, -0.0049948392963198689,
    1.9996767529618338, -0.99983837249649787, 0.99983837249649776 },

  /* Computed Parameter: DiscreteZeroPole_B
   * Referenced by: '<Root>/Discrete Zero-Pole'
   */
  { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },

  /* Computed Parameter: DiscreteZeroPole_C
   * Referenced by: '<Root>/Discrete Zero-Pole'
   */
  { 0.0016028401660721139, -2.30440996958269E+6, 6.9759190043905452E-6,
    -1.4318354606286254E-6, 1.8668975647657782E-5, -1.7703018748110967E-5,
    2.743486561094154E-6, -2.7340875441758872E-6, 2.3913484724469454E-7,
    -2.3906803935343272E-7, 1.996692684138047E-8, -1.9966463625403498E-8 },

  /* Computed Parameter: DiscreteZeroPole_D
   * Referenced by: '<Root>/Discrete Zero-Pole'
   */
  4.7863009232263959E-5,

  /* Expression: 0.001
   * Referenced by: '<Root>/Constant1'
   */
  0.001
};

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
