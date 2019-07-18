#include <unistd.h>
#include <stdio.h>

#include "nrf_sdm.h"
#include "app_error.h"
#include "nrf_drv_clock.h"
#include "nrf_delay.h"

#include "FreeRTOS.h"
#include "task.h"
#include "usb_io.h"

extern void user_main(void*);
extern void user_main_write(void*);
extern void user_main_echo(void*);

extern uint32_t __isr_vector;


TaskParameters_t user_task_parameters = {
	user_main, // pvTaskCode
	"blink1", // pcName;
	256, // usStackDepth;
	(void*)1000, // pvParameters
	1 | portPRIVILEGE_BIT, // priority
	NULL, // puxStackBuffer;
	{
		//{(void*)0x50000000, 0x00000800 , portMPU_REGION_READ_WRITE},  // gpio regs
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0},
	}, // xRegions;
};

int main(void) {
	ret_code_t ret;

	// disable sd
	sd_softdevice_disable();

	// set vtor
	SCB->VTOR = 0x00026000UL;
	__DSB();

	// enable clock needed by usbd
	ret = nrf_drv_clock_init();
	APP_ERROR_CHECK(ret);
	nrf_drv_clock_lfclk_request(NULL);
	while(!nrf_drv_clock_lfclk_is_running()) {
		/* Just waiting */
	}

	// enable usb serial communication
	//usb_io_init();

	//xTaskCreateRestricted(&user_task_parameters, NULL);
	//TaskHandle_t blink1_handle;
	xTaskCreate(
		user_main, /* The function that implements the task. */
		"blink1", /* Text name for the task. */
		512, /* Stack depth in words. */
		(void *)1000, /* Task parameters. */
		1 | portPRIVILEGE_BIT, /* Priority and mode */
		NULL /* Handle. */
	);
	//vTaskAllocateMPURegions(
	//	blink1_handle,
	//	xRegions
	//);
	//xTaskCreate(
	//	user_main_echo, /* The function that implements the task. */
	//	"echo", /* Text name for the task. */
	//	256, /* Stack depth in words. */
	//	NULL, /* Task parameters. */
	//	( 1 | portPRIVILEGE_BIT ), /* Priority and mode (Privileged in this case). */
	//	NULL /* Handle. */
	//);

	vTaskStartScheduler();

	/* Should not reach here as the scheduler is already started. */
	for( ; ; ) {
	}
}

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
