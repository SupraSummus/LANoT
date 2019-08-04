#include <stdbool.h>
#include <unistd.h>

#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "task.h"

#include "io.h"
#include "log.h"

// static storage area
#define LOG_BUFFER_SIZE (1024)
static uint8_t log_buffer[LOG_BUFFER_SIZE];
static StaticStreamBuffer_t log_stream_static;

static StreamBufferHandle_t log_stream = NULL;

// here we keep track of skipped writes (because of no space in buffer or uninitialized buffer or some other circumstance...)
static size_t log_skipped_bytes = 0;

static void log_write_skip (size_t count) {
	__sync_fetch_and_add(&log_skipped_bytes, count);
}

static int log_write (void * buf, size_t len, void * priv) {
	(void)priv;

	// log buffer not yet initialized
	if (log_stream == NULL) {
		log_write_skip(len);
		return len;
	}

	taskENTER_CRITICAL();  // critical section is required because there are multiple stream writers

		BaseType_t higher_prio_task_woken = pdFALSE;
		size_t sent = xStreamBufferSendFromISR(
			log_stream,
			buf, len,
			&higher_prio_task_woken
		);

	taskEXIT_CRITICAL();

	// not all data was writen maybe?
	if (sent < len) log_write_skip(len - sent);

	portYIELD_FROM_ISR(higher_prio_task_woken);

	return len;  // pretend all is writen to prevent blocking
}

#define LOG_TASK_STACK_DEPTH (128)
static StackType_t log_task_stack[LOG_TASK_STACK_DEPTH];
static StaticTask_t log_task;
static StackType_t log_task_discard_stack[LOG_TASK_STACK_DEPTH];
static StaticTask_t log_task_discard;

#define LOG_TASK_BUFFER_SIZE (16)
static char log_task_buffer[LOG_TASK_BUFFER_SIZE];
static char log_task_discard_buffer[LOG_TASK_BUFFER_SIZE];

void log_task_main(void * p) {
	(void)p;
	while (true) {
		size_t len;

		size_t skipped = __sync_lock_test_and_set(&log_skipped_bytes, 0);
		if (skipped > 0) {
			len = snprintf(log_task_buffer, LOG_TASK_BUFFER_SIZE, "!!! %d bytes skipped\n", skipped);
			len --;  // skip terminating \0
			if (len > LOG_TASK_BUFFER_SIZE) {  // we didnt fit into buffer
				len = LOG_TASK_BUFFER_SIZE;
				log_task_buffer[LOG_TASK_BUFFER_SIZE - 1] = '\n';
			}

		} else {
			len = xStreamBufferReceive(
				log_stream,
				log_task_buffer, LOG_TASK_BUFFER_SIZE,
				portMAX_DELAY
			);
		}

		// TODO check write result
		write(LOG_BACKEND_FD, log_task_buffer, len);
	}
}

void log_task_discard_main(void * p) {
	(void)p;
	while (true) {
		int len = read(LOG_BACKEND_FD, log_task_discard_buffer, LOG_TASK_BUFFER_SIZE);
		WARN("discarding %d bytes received on logging backend interface (fd %d)", len, LOG_BACKEND_FD);
	}
}

void log_init (void) {
	assert(log_stream == NULL);
	log_stream = xStreamBufferCreateStatic(
		LOG_BUFFER_SIZE - 1,
		1,  // trigger level
		log_buffer,
		&log_stream_static
	);
	assert(log_stream != NULL);

	io_register(STDERR_FILENO, io_direction_writer, log_write, NULL);

	INFO("initializing log on fd=%d with backend fd=%d", STDERR_FILENO, LOG_BACKEND_FD);

	// task to consume buffer
	TaskHandle_t task = NULL;
	task = xTaskCreateStatic(
		log_task_main,
		"log",
		LOG_TASK_STACK_DEPTH,
		NULL,  // task parameter
		LOG_TASK_PRIORITY | portPRIVILEGE_BIT,
		log_task_stack,
		&log_task
	);
	assert(task != NULL);

	// task to read data incoming on log backend interface
	task = xTaskCreateStatic(
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
