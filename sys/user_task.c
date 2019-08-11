#include <assert.h>

#include "FreeRTOS.h"
#include "task.h"

#include "log.h"

// TODO deduplicate constants with common/memory.ld
#define user_flash ((void *)0x00040000)
#define user_flash_size (736 * 1024)
#define user_ram ((void *)0x20010000)
#define user_ram_size (192 * 1024)

// fakes
#define user_flash ((void *)0x00050000)
#define user_ram_size (1 * 1024)

static StaticTask_t user_task_static;
static TaskParameters_t user_task_parameters = {
	user_flash, // pvTaskCode
	"user", // pcName;
	user_ram_size / 4, // usStackDepth;
	"good luck, have fun", // pvParameters
	1, // priority
	user_ram, // puxStackBuffer;
	{
		{(void*)0x00000000, 1024 * 1024 , portMPU_REGION_READ_ONLY | portMPU_REGION_CACHEABLE_BUFFERABLE},  // whole flash
		{0, 0, 0},  // unused slot
		{0, 0, 0},  // unused slot
	}, // xRegions;
	&user_task_static, // Holds the task's data structure.
};
static TaskHandle_t user_task_handle = NULL;

void user_task_start(void) {
	if (user_task_handle != NULL) {
		WARN("user task already running");
		return;
	}
	BaseType_t ret = xTaskCreateRestrictedStatic(&user_task_parameters, &user_task_handle);
	assert(ret == pdPASS);
	INFO("user task started");
}

void user_task_kill(void) {
	if (user_task_handle == NULL) {
		WARN("user task not running");
		return;
	}
	assert(0);  // not implemented yet
}
