#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "task.h"

#include "io.h"
#include "log.h"
#include "led.h"

#define buffer_size (1024)
static char buffer[buffer_size];
static size_t buffer_data_start = 0;
static size_t buffer_data_size = 0;

static inline size_t min (size_t a, size_t b) {
	if (a < b) return a;
	return b;
}

#define buffer_read_ptr (buffer + buffer_data_start)
#define buffer_read_size (min( \
	buffer_data_size,  /* all data is continous */ \
	buffer_size - buffer_data_start  /* data wraps around so we have to end reading earlier */ \
))

#define buffer_free_start ((buffer_data_start + buffer_data_size) % buffer_size)
#define buffer_free_size (buffer_size - buffer_data_size)
#define buffer_write_ptr (buffer + buffer_free_start)
#define buffer_write_size (min( \
	buffer_free_size, \
	buffer_size - buffer_free_start  /* free space wraps around */ \
))

static inline void buffer_consume (size_t size) {
	assert(size <= buffer_data_size);
	buffer_data_start = (buffer_data_start + size) % buffer_size;
	buffer_data_size -= size;
}

static inline void buffer_increase (size_t size) {
	assert(size <= buffer_free_size);
	buffer_data_size += size;
}

/**
 * here we keep track of skipped writes
 * (because of no space in buffer or uninitialized buffer or some other circumstance...)
 */
static size_t dropped_bytes = 0;

/**
 * setting this switch to true will cause buffering to be skipped
 */
static bool synchronous_operation = false;

static TaskHandle_t log_task_handle;

/**
 * write whole buffer to backend fd
 */
static void process_buffer(void) {
	#define sprintf_buffer_size (50)
	static char sprintf_buffer[sprintf_buffer_size];

	while (true) {
		taskENTER_CRITICAL();

		if (buffer_data_size <= 0) {
			taskEXIT_CRITICAL();
			return;
		}

		if (dropped_bytes > 0) {
			// inform about dropped bytes
			size_t local_dropped_bytes = dropped_bytes;
			dropped_bytes = 0;
			taskEXIT_CRITICAL();

			size_t len = snprintf(
				sprintf_buffer, sprintf_buffer_size,
				"\n!!! dropped %d bytes !!!\n", local_dropped_bytes
			);
			write(LOG_BACKEND_FD, sprintf_buffer, min(len, sprintf_buffer_size));

		} else {
			taskEXIT_CRITICAL();

			// write buffer contents
			ssize_t r = write(LOG_BACKEND_FD, buffer_read_ptr, buffer_read_size);
			assert(r > 0);

			taskENTER_CRITICAL();
			buffer_consume(r);
			taskEXIT_CRITICAL();
		}
	}
}

static int log_write (void * buf, size_t len, void * priv) {
	(void)priv;

	// synchronous mode bypasses the buffer
	if (synchronous_operation) {
		return write(LOG_BACKEND_FD, buf, len);
	}

	taskENTER_CRITICAL();

		// discard old messages to make room for this one
		if (buffer_free_size == 0) {
			buffer_consume(len);
			dropped_bytes += len;
		}

		// write to buffer
		uint16_t write_size = min(
			len,
			buffer_write_size
		);
		memcpy(buffer_write_ptr, buf, write_size);
		buffer_increase(write_size);

	taskEXIT_CRITICAL();

	// wake the transfer task
	if (log_task_handle != NULL) {
		BaseType_t higher_prio_task_woken = pdFALSE;
		xTaskNotifyFromISR(
			log_task_handle,
			0, eNoAction,
			&higher_prio_task_woken
		);
		portYIELD_FROM_ISR(higher_prio_task_woken);
	}

	return write_size;
}

#define LOG_TASK_STACK_DEPTH (128)
static StackType_t log_task_stack[LOG_TASK_STACK_DEPTH];
static StaticTask_t log_task;
static StackType_t log_task_discard_stack[LOG_TASK_STACK_DEPTH];
static StaticTask_t log_task_discard;

static void log_task_main(void * p) {
	(void)p;
	process_buffer();
	while (true) {
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
		process_buffer();
	}
}

static void log_task_discard_main(void * p) {
	(void)p;
	#define discard_buffer_size (16)
	static char discard_buffer[discard_buffer_size];
	while (true) {
		int len = read(LOG_BACKEND_FD, discard_buffer, discard_buffer_size);
		WARN("discarding %d bytes received on logging backend interface (fd %d)", len, LOG_BACKEND_FD);
	}
}

static int log_use_synchronous_mode_handler(void * p) {
	(void)p;
	int ret;

	portENTER_CRITICAL();
		if (synchronous_operation) {
			errno = EPERM;
			ret = -1;
		} else {
			INFO("enabling synchronous logging");

			// its crucial to enable synchronous mode for backend before disabling log buffer
			io_use_synchronous_mode(LOG_BACKEND_FD);
			synchronous_operation = true;

			process_buffer();
			ret = 0;
		}
	portEXIT_CRITICAL();

	return ret;
}

void log_init (void) {
	//synchronous_opration = true;
	io_register_write_handler(STDERR_FILENO, log_write, NULL);
	io_register_use_synchronous_mode_handler(STDERR_FILENO, log_use_synchronous_mode_handler, NULL);

	INFO("initializing log on fd=%d with backend fd=%d", STDERR_FILENO, LOG_BACKEND_FD);

	// task to consume buffer
	log_task_handle = xTaskCreateStatic(
		log_task_main,
		"log",
		LOG_TASK_STACK_DEPTH,
		NULL,  // task parameter
		LOG_TASK_PRIORITY | portPRIVILEGE_BIT,
		log_task_stack,
		&log_task
	);
	assert(log_task_handle != NULL);

	// task to read data incoming on log backend interface
	TaskHandle_t task = xTaskCreateStatic(
		log_task_discard_main,
		"log2",
		LOG_TASK_STACK_DEPTH,
		NULL,  // task parameter
		LOG_TASK_PRIORITY | portPRIVILEGE_BIT,
		log_task_discard_stack,
		&log_task_discard
	);
	assert(task != NULL);
}

void log_use_synchronous_mode (void) {
	io_use_synchronous_mode(STDERR_FILENO);
}
