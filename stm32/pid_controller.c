#include "pid_controller.h"

void PID_Init(PID_t *pid, float Kp, float Ti, float Td,
              float Ts, float u_min, float u_max) {
    pid->Kp = Kp;  pid->Ti = Ti;  pid->Td = Td;  pid->Ts = Ts;
    pid->u_min = u_min;  pid->u_max = u_max;
    pid->integral = 0.0f;
    pid->erro_ant = 0.0f;
    pid->y_pd_ant = 0.0f;
}

float PID_Compute(PID_t *pid, float erro) {
    /* =========================================================
     * BLOCO PD 
     * ========================================================= */
    float y_pd;
    if (pid->Ts > 1e-10f) {
        y_pd = erro + (pid->Td / pid->Ts) * (erro - pid->erro_ant);
    } else {
        y_pd = erro;  
    }

    /* =========================================================
     * BLOCO PI 
     * ========================================================= */
    float P = pid->Kp * y_pd;

    float I_inc = 0.0f;
    if (pid->Ti > 1e-10f) {
        I_inc = (pid->Kp / pid->Ti) * (pid->Ts / 2.0f)
              * (y_pd + pid->y_pd_ant);
    }

    float u_pre = P + (pid->integral + I_inc);

    /* =========================================================
     * Anti-windup por clamping 
     * ========================================================= */
    float u_out;
    if (u_pre > pid->u_max) {
        u_out = pid->u_max;
    } else if (u_pre < pid->u_min) {
        u_out = pid->u_min;
    } else {
        u_out = u_pre;
        pid->integral += I_inc; 
    }

    pid->erro_ant = erro;
    pid->y_pd_ant = y_pd;

    return u_out;
}

void PID_Reset(PID_t *pid) {
    pid->integral = 0.0f;
    pid->erro_ant = 0.0f;
    pid->y_pd_ant = 0.0f;
}