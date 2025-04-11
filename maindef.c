#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "em_chip.h"
#include "bsp.h"
#include "bsp_trace.h"

#define STACK_SIZE_FOR_TASK    (configMINIMAL_STACK_SIZE + 10)
#define TASK_PRIORITY          (tskIDLE_PRIORITY + 1)
#define QUEUE_LENGTH           5

// Struct to send info
typedef struct {
    uint8_t ledNumber;
    char message[30];
} LedMessage_t;

// Global queue
QueueHandle_t xLedQueue;

void vLedTask1(void *pvParameters)
{
    LedMessage_t msgToSend;
    const TickType_t delay = pdMS_TO_TICKS(500);

    msgToSend.ledNumber = 0;
    snprintf(msgToSend.message, sizeof(msgToSend.message), "LED 0 Blink");

    while (1) {
        xQueueSend(xLedQueue, &msgToSend, 0);
        vTaskDelay(delay);
    }
}

void vLedTask2(void *pvParameters)
{
    LedMessage_t msgToSend;
    const TickType_t delay = pdMS_TO_TICKS(800);

    msgToSend.ledNumber = 1;
    snprintf(msgToSend.message, sizeof(msgToSend.message), "LED 1 Blink");

    while (1) {
        xQueueSend(xLedQueue, &msgToSend, 0);
        vTaskDelay(delay);
    }
}

void vReceiverTask(void *pvParameters)
{
    LedMessage_t receivedMsg;

    while (1) {
        if (xQueueReceive(xLedQueue, &receivedMsg, pdMS_TO_TICKS(1000)) == pdPASS) {
            BSP_LedToggle(receivedMsg.ledNumber);
            printf("Rebut: %s\n", receivedMsg.message);
        }
    }
}

int main(void)
{
    CHIP_Init();
    BSP_TraceProfilerSetup();
    BSP_LedsInit();

    BSP_LedClear(0);
    BSP_LedClear(1);

    //Create the queue
    xLedQueue = xQueueCreate(QUEUE_LENGTH, sizeof(LedMessage_t));

    if (xLedQueue != NULL)
    {
        //Creation of two task (each for each led) and a task to recieve the info
        xTaskCreate(vLedTask1, "LED0 Task", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
        xTaskCreate(vLedTask2, "LED1 Task", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
        xTaskCreate(vReceiverTask, "Receiver Task", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);

        vTaskStartScheduler();
    }

    // If the queue creation fails its task
    while (1);
}

/*
We used for this exercice:
-vLedTask1 / vLedTask2: To send info about the led
-vReceiverTask: To receive info about the led and toggle its led
-We do everything thanks to a structure that we send through the queue (LedMessage_t)
*/
