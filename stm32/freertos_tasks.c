#include "freertos_tasks.h"
#include "controlador_efo.h"   
#include "pid_controller.h"
#include "pid_autotuning.h"
#include "main.h"
#include "usart.h"      
#include "gpio.h"       
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

/* =================================================================
 * PARÂMETROS DO SISTEMA
 * ================================================================= */
#define TS_S            0.001f      
#define RELAY_GAIN_D    10.0f      


/* =================================================================
 * VARIÁVEIS GLOBAIS
 * ================================================================= */
QueueHandle_t xQueueYInput  = NULL;
QueueHandle_t xQueueUOutput = NULL;
QueueHandle_t xQueueHost    = NULL;

TaskHandle_t xHandleTaskEFO  = NULL;
TaskHandle_t xHandleTaskUART = NULL;
TaskHandle_t xHandleTaskHost = NULL;

SemaphoreHandle_t xSemaphoreUartRx = NULL;

/* =================================================================
 * MÁQUINA DE ESTADOS
 *
 * O sistema opera em 3 fases sequenciais:
 *  MODO_ENSAIO->MODO_CALCULO_PID->MODO_CONTROLE_PID
 * ================================================================= */
typedef enum {
    MODO_ENSAIO = 0,
    MODO_CALCULO_PID,
    MODO_CONTROLE_PID
} SistemaEstado_t;

static volatile SistemaEstado_t sistemaEstado = MODO_ENSAIO;

//Ganhos PID 
static float pid_Kp = 0.0f;
static float pid_Ti = 0.0f;
static float pid_Td = 0.0f;


static PID_t pidCtrl;

/* =================================================================
 * CRITÉRIO DE CONVERGÊNCIA DO ENSAIO EFO
 * ================================================================= */
#define CONV_N_CICLOS       10
#define CONV_TOLERANCIA_T   0.030f   /* 3% */
#define CONV_TOLERANCIA_A   0.050f   /* 5% */

/* =================================================================
 * vTaskEFO — Prioridade 3 (mais alta)
 * ================================================================= */
void vTaskEFO(void *pvParameters) {
    float y_recebido = 0.0f;
    float u_calculado = 0.0f;
    DadosSupervisao_t dadosSup;

    /* Variáveis do critério de convergência */
    uint8_t convergencia_count = 0;
    float T_anterior = 0.0f;
    float A_anterior = 0.0f;

    /* Inicializa o modelo EFO gerado pelo Simulink */
    controlador_efo_initialize();

	uint32_t dwt_period_last = 0;
	uint32_t max_step_cycles = 0;
	uint32_t max_period_jitter = 0;
	uint32_t hil_sample_count = 0;

	char msg[] = "\r\n=== Pressione USER (azul) para iniciar ===\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 50);
	while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET) {
		vTaskDelay(pdMS_TO_TICKS(50));
	}
	/* Debounce */
	vTaskDelay(pdMS_TO_TICKS(200));

    for (;;) {
        // Aguarda notificação do TIM6 (a cada Ts) 
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // medir jitter do período 
		uint32_t dwt_now = DWT->CYCCNT;
		if (dwt_period_last != 0) {
			uint32_t period = dwt_now - dwt_period_last;
			uint32_t ts_ideal = 80000;  /* 80 MHz × 1 ms = 80000 ciclos */
			uint32_t jitter = (period > ts_ideal) ?
				(period - ts_ideal) : (ts_ideal - period);
			if (jitter > max_period_jitter) max_period_jitter = jitter;
		}
		dwt_period_last = dwt_now;
		hil_sample_count++;

        HAL_GPIO_WritePin(LED_JITTER_GPIO_Port, LED_JITTER_Pin, GPIO_PIN_SET);

        xQueueReceive(xQueueYInput, &y_recebido, 0);

        /* ---------- MÁQUINA DE ESTADOS ---------- */
        switch (sistemaEstado) {

        case MODO_ENSAIO: {
            controlador_efo_U.y = (real_T)y_recebido;
            controlador_efo_U.r = 1.0;

            uint32_t dwt_step_start = DWT->CYCCNT;

            /* Executa 1 passo do EFO */
            controlador_efo_step();

            uint32_t dwt_step_end = DWT->CYCCNT;
            uint32_t step_cycles = dwt_step_end - dwt_step_start;
            if (step_cycles > max_step_cycles) max_step_cycles = step_cycles;

            u_calculado = (float)controlador_efo_Y.u;

            // Verificação de convergência
            float T_atual = (float)controlador_efo_Y.T;
            float A_atual = (float)controlador_efo_Y.A;

            if (T_atual != T_anterior || A_atual != A_anterior) {
                if (T_anterior > 0.0f && A_anterior > 0.0f) {
                    float deltaT = fabsf(T_atual - T_anterior) / T_anterior;
                    float deltaA = fabsf(A_atual - A_anterior) / A_anterior;

                    if (deltaT < CONV_TOLERANCIA_T &&
                        deltaA < CONV_TOLERANCIA_A) {
                        convergencia_count++;
                    } else {
                        convergencia_count = 0;
                    }
                }
                T_anterior = T_atual;
                A_anterior = A_atual;
            }

            if (convergencia_count >= CONV_N_CICLOS) {
                sistemaEstado = MODO_CALCULO_PID;
            }
            break;
        }

        case MODO_CALCULO_PID: {
            double Kp_d, Ti_d, Td_d;
            calcularGanhosPID((double)A_anterior,
                              (double)T_anterior,
                              (double)RELAY_GAIN_D,
                              (double)TS_S,
                              &Kp_d, &Ti_d, &Td_d);

            pid_Kp = (float)Kp_d;
            pid_Ti = (float)Ti_d;
            pid_Td = (float)Td_d;

            char gbuf[128];
            int glen = snprintf(gbuf, sizeof(gbuf),
                "\r\n=== PID GAINS ===\r\n"
                "A=%.6f T=%.6f\r\n"
                "Kp=%.4f Ti=%.6f Td=%.8f\r\n",
                (double)A_anterior, (double)T_anterior,
                (double)pid_Kp, (double)pid_Ti, (double)pid_Td);
            HAL_UART_Transmit(&huart2, (uint8_t*)gbuf, glen, 50);

			{
				char prof[200];
				float step_max_us = (float)max_step_cycles / 80.0f;
				float jitter_max_us = (float)max_period_jitter / 80.0f;
				int plen = snprintf(prof, sizeof(prof),
					"\r\n=== HiL PROFILING ===\r\n"
					"Amostras: %lu\r\n"
					"Step max: %.2f us\r\n"
					"Jitter Ts max: %.2f us\r\n"
					"Margem: %.1f%%\r\n",
					(unsigned long)hil_sample_count,
					(double)step_max_us,
					(double)jitter_max_us,
					(double)(100.0f * (1000.0f - step_max_us) / 1000.0f));
				HAL_UART_Transmit(&huart2, (uint8_t*)prof, plen, 100);
			}

            // Inicializa o controlador PID 
            PID_Init(&pidCtrl, pid_Kp, pid_Ti, pid_Td, TS_S,
                     -10.0f, 10.0f);

            sistemaEstado = MODO_CONTROLE_PID;
            u_calculado = 0.0f;
            break;
        }

        case MODO_CONTROLE_PID: {
            float erro = 1.0f - y_recebido;   // r = 1.0 
            u_calculado = PID_Compute(&pidCtrl, erro);
            break;
        }

        }

        xQueueOverwrite(xQueueUOutput, &u_calculado);

        dadosSup.y_atual       = y_recebido;
        dadosSup.u_atual       = u_calculado;
        dadosSup.amplitude_osc = (float)controlador_efo_Y.A;
        dadosSup.periodo_osc   = (float)controlador_efo_Y.T;
        dadosSup.estado        = (uint8_t)sistemaEstado;
        dadosSup.tick          = xTaskGetTickCount();
        xQueueOverwrite(xQueueHost, &dadosSup);

        HAL_GPIO_WritePin(LED_JITTER_GPIO_Port, LED_JITTER_Pin, GPIO_PIN_RESET);
    }
}

/* =================================================================
 * vTaskUART_ESP — Prioridade 2 (média)
 * ================================================================= */
void vTaskUART_ESP(void *pvParameters) {
    float u_enviar  = 0.0f;
    float y_receber = 0.0f;
    uint8_t pacote_tx[8];
    uint8_t pacote_rx[8];

    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        xQueuePeek(xQueueUOutput, &u_enviar, 0);

        pacote_tx[0] = 0xAA;
        pacote_tx[1] = 0x55;
        memcpy(&pacote_tx[2], &u_enviar, 4);
        pacote_tx[6] = 0x00;
        pacote_tx[7] = pacote_tx[2] ^ pacote_tx[3]
                     ^ pacote_tx[4] ^ pacote_tx[5];

        HAL_UART_Transmit(&huart1, pacote_tx, 8, 2);
        HAL_UART_Receive_IT(&huart1, pacote_rx, 8);

        BaseType_t got_data = xSemaphoreTake(xSemaphoreUartRx,
                                             pdMS_TO_TICKS(3));

        if (got_data == pdTRUE) {
            if (pacote_rx[0] == 0xBB && pacote_rx[1] == 0x66) {
                uint8_t chk = pacote_rx[2] ^ pacote_rx[3]
                            ^ pacote_rx[4] ^ pacote_rx[5];
                if (chk == pacote_rx[7]) {
                    memcpy(&y_receber, &pacote_rx[2], 4);
                    xQueueOverwrite(xQueueYInput, &y_receber);
                }
            }
        } else {
            HAL_UART_AbortReceive(&huart1);
        }
    }
}

/* =================================================================
 * vTaskHost — Prioridade 1 (mais baixa)
 * ================================================================= */
void vTaskHost(void *pvParameters) {
    DadosSupervisao_t dados;
    char buf[128];

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(100));

        if (xQueuePeek(xQueueHost, &dados, 0) == pdTRUE) {
            int len = snprintf(buf, sizeof(buf),
                "y=%.6f,u=%.6f,A=%.6f,T=%.6f,st=%d,tk=%lu\r\n",
                (double)dados.y_atual,
                (double)dados.u_atual,
                (double)dados.amplitude_osc,
                (double)dados.periodo_osc,
                dados.estado,
                (unsigned long)dados.tick);
            HAL_UART_Transmit(&huart2, (uint8_t*)buf, len, 20);
        }
    }
}


/* =================================================================
 * CALLBACKS UART
 * ================================================================= */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(xSemaphoreUartRx,
                              &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        __HAL_UART_CLEAR_FLAG(huart,
            UART_CLEAR_OREF | UART_CLEAR_NEF |
            UART_CLEAR_PEF  | UART_CLEAR_FEF);
        HAL_UART_AbortReceive(huart);
    }
}