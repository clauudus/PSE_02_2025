/***************************************
Main objective of this code:
Create two tasks and a queue to handle
the on and off turning of a red led
Still in process
****************************************/

#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "croutine.h"

#include "em_chip.h"
#include "bsp.h"
#include "bsp_trace.h"

#include "bsp_i2c.h"


#define STACK_SIZE_FOR_TASK    (configMINIMAL_STACK_SIZE + 10)
#define TASK_PRIORITY          (tskIDLE_PRIORITY + 1)
#define mainQUEUE_LENGTH		   2

const portTickType delay1 = pdMS_TO_TICKS(500);

const QueueHandle_t xQueue;
const xQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(unsigned long));


/***************************************************************************//**
 * @brief Simple task which is blinking led
 * @param *pParameters pointer to parameters passed to the function
 ******************************************************************************/
static void LedBlink(void *pParameters)
{
  (void) pParameters;
  const portTickType delaya = pdMS_TO_TICKS(500);
  const portTickType delayb = pdMS_TO_TICKS(250);

  for (;; ) {
    BSP_LedToggle(1);
    printf("Task 1\n");
    vTaskDelay(delaya);

    BSP_LedToggle(0);
    printf("Task 2\n");
    vTaskDelay(delayb);
  }
}

static void prvQueueSENDtask(void *pvParameters)
{
	TickType_t xNextWakeTime;
	const TickType_t xFrequency = 10;

	const portTickType nextTaskSend;
	const unsigned long ValuetoSend = 100;
	xNextWakeTime = xTaskGetTickCount();

	QueueHandle_t xQueue;

	for(;;)
	{
		vTaskDelayUntil(&xNextWakeTime, xFrequency);
		xQueueSend(xQueue, &ValuetoSend, 0);
	}
}

static void prvQueueReceivetask(void *pvParameters)
{
	unsigned long ReceivedValue;
	QueueHandle_t xQueue;
	for(;;)
	{
		xQueueReceive(xQueue, &ReceivedValue, delay1);
		if(ReceivedValue == 100)
		{
			printf("FUNCIONO");
		}
	}
}



/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();
  /* If first word of user data page is non-zero, enable Energy Profiler trace */
  BSP_TraceProfilerSetup();

  /* Initialize LED driver */
  BSP_LedsInit();
  /* Setting state of leds*/
  BSP_LedSet(0);
  BSP_LedSet(1);

  /*Create one queue for blinking leds */



  /*Create two task for blinking leds*/
  if(xQueue != NULL)
  {

	  xTaskCreate(prvQueueReceivetask, (const char *) "Receive", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
	  xTaskCreate(prvQueueSENDtask, (const char *) "Send", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
	  vTaskStartScheduler();
  }



  	  /*xTaskCreate(LedBlink, (const char *) "LedBlink1", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);*/
	  /*xTaskCreate(LedBlink, (const char *) "LedBlink2", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);*/
  /*Start FreeRTOS Scheduler*/
  vTaskStartScheduler();

  return 0;
}
