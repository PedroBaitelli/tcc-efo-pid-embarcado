/* pid_controller.h — PID na forma SÉRIE (Bazanella et al. 2017, eq. 5)
 * Pedro Baitelli — TCC2 UFRGS
 *
 *   C(s) = Kp · (1 + 1/(Ti·s)) · (1 + Td·s)
 *
 * Implementação como cascata de dois blocos:
 *   1) Bloco PD: y_pd[k] = e[k] + (Td/Ts)·(e[k] - e[k-1])
 *   2) Bloco PI: u[k]    = Kp·y_pd[k] + Kp/Ti · ∫y_pd dt
 *
 * Por que SÉRIE e não paralela:
 *   As fórmulas de sintonia da Tabela I do Bazanella (Kp = 0,97/M120,
 *   Ti = 0,90·T120, Td = 0,028·T120) foram derivadas analiticamente
 *   para a forma série da eq. 5. Implementar na forma paralela
 *   introduz um erro de ~3% no termo cruzado Td/Ti.
 *
 * Discretização:
 *   Bloco PD: derivada por Backward Euler (de/dt ≈ (e[k]-e[k-1])/Ts)
 *   Bloco PI: integral por Tustin (bilinear)
 *
 * Anti-windup: clamping — a integral só acumula se a saída
 *   não saturou (Åström & Hägglund, 2006).
 */
#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

typedef struct {
    float Kp, Ti, Td, Ts;
    float u_min, u_max;
    float integral;       /* acumulador da integral do bloco PI */
    float erro_ant;       /* e[k-1] para o bloco PD */
    float y_pd_ant;       /* y_pd[k-1] para a integral Tustin do PI */
} PID_t;

/* Inicializa a struct com os ganhos e limites de saturação.
 * Zera todos os estados internos. */
void PID_Init(PID_t *pid, float Kp, float Ti, float Td,
              float Ts, float u_min, float u_max);

/* Calcula um passo do PID. Recebe o erro (r - y), retorna u.
 * Deve ser chamado a cada Ts (1 ms). */
float PID_Compute(PID_t *pid, float erro);

/* Zera os estados internos sem alterar os ganhos.
 * Útil para reiniciar o controlador sem reconfigurar. */
void PID_Reset(PID_t *pid);

#endif
