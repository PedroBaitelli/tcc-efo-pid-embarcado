#ifndef FREERTOS_TASKS_H
#define FREERTOS_TASKS_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


typedef struct {
    float y_atual;          /* y(t) — saída da planta */
    float u_atual;          /* u(t) — sinal de controle */
    float amplitude_osc;    /* A — amplitude da oscilação EFO */
    float periodo_osc;      /* T — período da oscilação EFO */
    uint8_t estado;         /* 0=ENSAIO, 1=CALCULO, 2=CONTROLE */
    uint32_t tick;          /* timestamp em ticks do FreeRTOS */
} DadosSupervisao_t;

/* ---------------------------------------------------------------
 * Handles das tarefas e filas.
 * --------------------------------------------------------------- */
extern TaskHandle_t  xHandleTaskEFO;
extern TaskHandle_t  xHandleTaskUART;
extern TaskHandle_t  xHandleTaskHost;

extern QueueHandle_t xQueueYInput;    /* y(t): ESP32 -> EFO */
extern QueueHandle_t xQueueUOutput;   /* u(t): EFO -> ESP32 */
extern QueueHandle_t xQueueHost;      /* supervisão -> PC */

extern SemaphoreHandle_t xSemaphoreUartRx;

void vTaskEFO(void *pvParameters);
void vTaskUART_ESP(void *pvParameters);
void vTaskHost(void *pvParameters);

#endif /* FREERTOS_TASKS_H */
