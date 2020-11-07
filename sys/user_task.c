#include <assert.h>

#include "FreeRTOS.h"
#include "task.h"

#include "user_task.h"

#define LOG_SUBSYSTEM "user_task"
#include <lanot/log.h>

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
