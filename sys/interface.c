#include <unistd.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

#include "log.h"
#include "user_task.h"

#define INPUT_FD (STDIN_FILENO)
#define OUTPUT_FD (STDOUT_FILENO)

enum command_t {
	COMMAND_INVALID = '?',
	COMMAND_OK = 'k',
	COMMAND_ECHO = '\n',
	//COMMAND_FLASH_CHUNK_SIZE = 10,
	//COMMAND_FLASH_READ = 11,
	COMMAND_FLASH_WRITE = 'f',
	COMMAND_KILL = 'k',
	COMMAND_START = 's',
};

static bool handle_command(void) {
	unsigned char code;
	if (read(INPUT_FD, &code, 1) != 1) {
		WARN("failed to read command code");
		return false;
	}
	switch (code) {
		case COMMAND_ECHO: {
			write(OUTPUT_FD, &code, 1);
			return true;
		}
		case COMMAND_FLASH_WRITE: {
			break;
		}
		case COMMAND_KILL: {
			user_task_kill();
			break;
		}
		case COMMAND_START: {
			user_task_start();
			break;
		}

		default: {
			WARN("unknown command %x", code);
			code = COMMAND_INVALID;
			write(OUTPUT_FD, &code, 1);
			return false;
		}
	}

	code = COMMAND_OK;
	write(OUTPUT_FD, &code, 1);
	return true;
}

void interface_main(void * p) {
	(void)p;

	INFO("starting");
	while (handle_command());

	/* Kill ourselves. */
	INFO("terminating");
	vTaskDelete( NULL );
	(void)handle_command;
}

#define INTERFACE_STACK_DEPTH (128)
static StackType_t interface_stack[INTERFACE_STACK_DEPTH];
static StaticTask_t interface_task;

void interface_task_create(void) {
	TaskHandle_t task = xTaskCreateStatic(
		interface_main,
		"ifce",
		INTERFACE_STACK_DEPTH,
		NULL,  // task parameter
		1 | portPRIVILEGE_BIT,
		interface_stack,
		&interface_task
	);
	assert(task != NULL);
}
