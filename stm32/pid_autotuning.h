#ifndef PID_AUTOTUNING_H
#define PID_AUTOTUNING_H

void calcularGanhosPID(double A, double T120, double d, double Ts,
                       double *Kp, double *Ti, double *Td);

#endif