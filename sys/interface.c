#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "FreeRTOS.h"
#include "task.h"

#include "flash.h"
#include "io_helpers.h"
#include "user_task.h"
#include "version.h"

#define LOG_SUBSYSTEM "interface"
#include "log.h"

#define INPUT_FD (STDIN_FILENO)
#define OUTPUT_FD (STDOUT_FILENO)

enum command_t {
	// command replies
	COMMAND_INVALID = '?',
	COMMAND_OK = 'k',

	// special case - reply to this command is COMMAND_ECHO
	COMMAND_ECHO = '\n',

	COMMAND_FLASH_CHUNK_SIZE = 12,
	COMMAND_FLASH_CHUNK_COUNT = 11,
	COMMAND_FLASH_READ = 'r',
	COMMAND_FLASH_WRITE = 'w',
	COMMAND_KILL = 'k',
	COMMAND_START = 's',

	COMMAND_VERSION = 'v',
};

static bool handle_command(void) {
	static uint8_t page_buffer[FLASH_PAGE_SIZE];

	unsigned char code;
	if (read(INPUT_FD, &code, 1) != 1) {
		WARN("failed to read command code");
		return false;
	}

	switch (code) {
		case COMMAND_ECHO: {
			write(OUTPUT_FD, &code, 1);
			return true;  // special case - we dont send ACK as usual
		}

		/**
		 * - receive uint32 page number
		 * - read page from flash into RAM buffer
		 * - write buffer to communication interface
		 */
		case COMMAND_FLASH_READ: {
			uint32_t page_num;
			if (!read_uint32(INPUT_FD, &page_num)) {
				WARN("couldn't read page number");
				write_uint8(OUTPUT_FD, COMMAND_INVALID);
				return false;
			}
			if (!flash_read_user_page(page_num, page_buffer)) {
				WARN("failed to access page %lu", page_num);
				write_uint8(OUTPUT_FD, COMMAND_INVALID);
				return false;
			}
			if (write_all(OUTPUT_FD, page_buffer, FLASH_PAGE_SIZE) != FLASH_PAGE_SIZE) {
				WARN("failed to send page content");
				write_uint8(OUTPUT_FD, COMMAND_INVALID);
				return false;
			}
			return true;
		}


		/**
		 * - receive uint32 page number
		 * - read page from communication interface into RAM buffer
		 * - write buffer to flash
		 */
		case COMMAND_FLASH_WRITE: {
			uint32_t page_num;
			if (!read_uint32(INPUT_FD, &page_num)) {
				WARN("couldn't read page number");
				write_uint8(OUTPUT_FD, COMMAND_INVALID);
				return false;
			}
			if (read_all(INPUT_FD, page_buffer, FLASH_PAGE_SIZE) != FLASH_PAGE_SIZE) {
				WARN("failed to receive page content");
				write_uint8(OUTPUT_FD, COMMAND_INVALID);
				return false;
			}
			if (!flash_write_user_page(page_num, page_buffer)) {
				WARN("failed to write page %lu", page_num);
				write_uint8(OUTPUT_FD, COMMAND_INVALID);
				return false;
			}
			return true;
		}

		case COMMAND_FLASH_CHUNK_SIZE: {
			if (!write_uint32(OUTPUT_FD, FLASH_PAGE_SIZE)) {
				WARN("couldn't write page size");
				write_uint8(OUTPUT_FD, COMMAND_INVALID);
				return false;
			}
			return true;
		}

		case COMMAND_FLASH_CHUNK_COUNT: {
			if (!write_uint32(OUTPUT_FD, FLASH_PAGE_COUNT)) {
				WARN("couldn't write page count");
				write_uint8(OUTPUT_FD, COMMAND_INVALID);
				return false;
			}
			return true;
		}

		case COMMAND_KILL: {
			user_task_kill();
			break;
		}
		case COMMAND_START: {
			user_task_start();
			break;
		}

		case COMMAND_VERSION: {
			write(OUTPUT_FD, version_string, strlen(version_string));
			write(OUTPUT_FD, "\n", 1);
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
	close(OUTPUT_FD);
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
		SYSTEM_TASK_PRIORITY | portPRIVILEGE_BIT,
		interface_stack,
		&interface_task
	);
	assert(task != NULL);
}
