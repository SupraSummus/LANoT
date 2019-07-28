#include <unistd.h>
#include <stdio.h>

#include "nrf_sdm.h"
#include "app_error.h"
#include "nrf_drv_clock.h"

#include "FreeRTOS.h"
#include "task.h"
#include "usb_io.h"
#include "interface.h"

#define user_main ((void *)0x00041000)
#define user_mem ((void *)0x20008000)

void * align_for_mpu(void * p, size_t size) {
    if (size == 0) return p;
    uint32_t remainder = (uint32_t)p % size;
    if (remainder == 0) return p;
    return (void *)((uint32_t)p + size - remainder);
}

TaskParameters_t user_tasks_parameters[] = {
	{0},
	{
		user_main, // pvTaskCode
		"blink1", // pcName;
		512, // usStackDepth;
		(void*)1000, // pvParameters
		1, // priority
		NULL, // puxStackBuffer;
		{
			{(void*)0x00000000, 1024 * 1024 , portMPU_REGION_READ_ONLY | portMPU_REGION_CACHEABLE_BUFFERABLE},  // whole flash
			{0, 0, 0},  // unused slot
			{0, 0, 0},  // unused slot
		}, // xRegions;
	},
	{
		user_main, // pvTaskCode
		"blink2", // pcName;
		512, // usStackDepth;
		(void*)888, // pvParameters
		1, // priority
		NULL, // puxStackBuffer;
		{
			{(void*)0x00000000, 1024 * 1024 , portMPU_REGION_READ_ONLY | portMPU_REGION_CACHEABLE_BUFFERABLE},  // whole flash
			{0, 0, 0},  // unused slot
			{0, 0, 0},  // unused slot
		}, // xRegions;
	},
	{0}
};

int main(void) {
	ret_code_t ret;

	// disable sd
	sd_softdevice_disable();

	// set vtor point to our mem
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
	usb_io_init();

	// start interface task (for programming, obtaining status etc)
	interface_task_create();

	// make a task for each entry in user_tasks_parameters with autoallocated stack
	//void * task_mem = user_mem;
	//for (TaskParameters_t * t = user_tasks_parameters; t->pvTaskCode != NULL; t++) {
	//	t->puxStackBuffer = align_for_mpu(task_mem, t->usStackDepth);
	//	task_mem = t->puxStackBuffer + t->usStackDepth;
	//	xTaskCreateRestricted(t, NULL);
	//}

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