#include <unistd.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "log.h"

#define _READ(buff, count) (read(STDIN_FILENO, buff, count))
#define _WRITE(buff, count) (write(STDOUT_FILENO, buff, count))

enum command_t {
	COMMAND_INVALID = 0,
	COMMAND_OK = 1,
	//COMMAND_FLASH_CHUNK_SIZE = 10,
	//COMMAND_FLASH_READ = 11,
	COMMAND_FLASH_WRITE = 12,
	COMMAND_STOP = 20,
	COMMAND_START = 21,
};

//#define BUFF_SIZE 1024
//static char buff[BUFF_SIZE];

static bool handle_command(void) {
	unsigned char code;
	if (_READ(&code, 1) != 1) {
		WARN("failed to read command code");
		return false;
	}
	switch (code) {
		case COMMAND_FLASH_WRITE: {
			break;
		}
		case COMMAND_STOP: {
			break;
		}
		case COMMAND_START: {
			break;
		}

		// testing
		case 'a': {
			code = 'b';
			_WRITE(&code, 1);
			return true;
		}
		case '\n': {
			_WRITE(&code, 1);
			return true;
		}

		default: {
			WARN("unknown command %x", code);
			code = COMMAND_INVALID;
			_WRITE(&code, 1);
			return false;
		}
	}
	code = COMMAND_OK;
	_WRITE(&code, 1);
	return true;
}

#include "led.h"

void interface_main(void * p) {
	(void)p;

	led_on();

	#define BUFSIZE (70)  // you dont want to go below 64 (see *_USBD_EPSIZE) here as it seems buggy with usbd implementation
	char buf[BUFSIZE + 1];

	while (true) {
		ssize_t s = read(STDIN_FILENO, buf, BUFSIZE);
		if (s > 0) {
			buf[s] = '\0';
			//write(STDOUT_FILENO, "got >>>", 7);
			//write(STDOUT_FILENO, buf, s);
			//write(STDOUT_FILENO, "<<<\n", 4);
			printf("got %d:\n", s);
			write(STDOUT_FILENO, buf, s);
		} else {
			printf("read returned %d\n", s);
		}
	}

	
	//write(2, "2\n", 2);

	//INFO("starting");
	//while (handle_command());

	/* Kill ourselves. */
	//INFO("terminating");
	vTaskDelete( NULL );
	(void)handle_command;
}

#define INTERFACE_STACK_DEPTH (512)
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
