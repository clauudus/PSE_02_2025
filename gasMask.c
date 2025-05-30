#include <stdio.h>
#include <stdlib.h>

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

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_cmu.h"
#include <stdbool.h>

#define STACK_SIZE_FOR_TASK    (configMINIMAL_STACK_SIZE + 10)
#define TASK_PRIORITY          (tskIDLE_PRIORITY + 1)
#define QUEUE_LENGTH           2

// Struct to send info
typedef struct {
    int co2Value;
    char message[30];
} Message_t;



typedef struct {
	int LedToToggle;
	char message[30];
} LedMessage_t;

// Global queue
QueueHandle_t xDataQueue;
QueueHandle_t xLedQueue;



void vDataSend(void *pvParameters)
{
	//int co2 = 1500;//(rand() % 8192) + 400;
	uint8_t co2;
	BSP_I2C_ReadRegister(0x5B, &co2);
	co2 = (co2 % 8192) + 400;
	 Message_t msgToSend;
	 const TickType_t delay = pdMS_TO_TICKS(3000);
	 msgToSend.co2Value = co2;
	 snprintf(msgToSend.message, sizeof(msgToSend.message), "Dades llegides");

	 printf("Value: %u \n", co2);

	 while(1)
	 {
		 xQueueSend(xDataQueue, &msgToSend, 0);
		 vTaskDelay(delay);
	 }
}


void vReceiverDegreeCalculator(void *pvParameters)
{
    Message_t receivedMsg;
    int LedNumber = 0;
    	// 400 - 1000 verd, 1000 - 2000 groc, 2000 - 8192 vermell
    while (1)
    {
        if (xQueueReceive(xDataQueue, &receivedMsg, pdMS_TO_TICKS(1000)) == pdPASS)
        {
        	if(receivedMsg.co2Value >= 1000 && receivedMsg.co2Value <= 2000)
        	{
        		LedNumber = 1;
        	}
        	else
        	{
        		if(receivedMsg.co2Value > 2000)
        		{
        			LedNumber = 2;
        		}
        	}

        	LedMessage_t msgToSend;
        	msgToSend.LedToToggle = LedNumber;
        	xQueueSend(xLedQueue, &msgToSend, 0);
        	snprintf(msgToSend.message, sizeof(msgToSend.message), "Led enviat");
        }
    }
}



void vReceiveLedNumber(void *pvParameters)
{
	LedMessage_t LedRebut;
	const TickType_t delay = pdMS_TO_TICKS(1000);

	 while (1)
		{

			if (xQueueReceive(xLedQueue, &LedRebut, pdMS_TO_TICKS(1000)) == pdPASS)
			{
				BSP_LedClear(0);
				BSP_LedClear(1);

				switch(LedRebut.LedToToggle)
				{
				case 0:
					BSP_LedToggle(0);
					printf("GREEN LIGHT \n");
					break;

				case 1:
					BSP_LedToggle(1);
					printf("YELLOW LIGHT \n");
					break;

				case 2:
					BSP_LedToggle(0);
					BSP_LedToggle(1);
					printf("RED LIGHT \n");
					break;

				default:
					break;
				}
				vTaskDelay(delay);
			}
		}

}




int main(void)
{
    /*CHIP_Init();
    BSP_TraceProfilerSetup();
    BSP_LedsInit();

    BSP_LedClear(0);
    BSP_LedClear(1);

    //BSP_I2C_Init(0xB6);
    BSP_I2C_Init(0x5A);
    uint8_t value;

    //Create the queue
    xLedQueue = xQueueCreate(QUEUE_LENGTH, sizeof(LedMessage_t));
    I2C_Test();


    BSP_I2C_ReadRegister(0x02, &value);
    printf("Valor: %00X\n", value);



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
    */
		CHIP_Init();
		BSP_TraceProfilerSetup();
	    BSP_LedsInit();

	    BSP_LedClear(0);
	    BSP_LedClear(1);


	    BSP_TraceProfilerSetup();

	    if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
	        while (1) ;
	      }

	    BSP_I2C_Init(0x5B << 1);
	    //BSP_I2C_Init(0x5A);
	    //uint8_t value;
	    //uint8_t value;

	    //BSP_I2C_ReadRegister(0x20, &value);

	    uint8_t co2;
	    BSP_I2C_ReadRegister(0x5B, &co2);
	    co2 = (co2 % 8192) + 400;

	    //Create the queue
	    xDataQueue = xQueueCreate(QUEUE_LENGTH, sizeof(Message_t));
	    xLedQueue = xQueueCreate(QUEUE_LENGTH, sizeof(LedMessage_t));

	    printf("Starting queues \n");

	    if (xDataQueue != NULL)
	    {
	    	xTaskCreate(vDataSend, "DATA Send", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);

	    	xTaskCreate(vReceiverDegreeCalculator, "DATA Received", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
	    }

	    if (xLedQueue != NULL)
	   	    {
	   	    	xTaskCreate(vReceiveLedNumber, "Led number received", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
	   	    }

	    vTaskStartScheduler();
	    while (1);

}

/*
We used for this exercice:
-vLedTask1 / vLedTask2: To send info about the led
-vReceiverTask: To receive info about the led and toggle its led
-We do everything thanks to a structure that we send through the queue (LedMessage_t)
*/
