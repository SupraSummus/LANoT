#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

#include "log.h"

// static storage area
#define LOG_BUFFER_SIZE (1024 - 1)
static uint8_t log_buffer[LOG_BUFFER_SIZE + 1];
static StaticStreamBuffer_t log_stream_static;

static StreamBufferHandle_t log_stream = NULL;

// here we keep track of skipped writes (because of no space in buffer or uninitialized buffer or some other circumstance...)
static size_t log_skipped_bytes = 0;

static void log_write_skip (size_t count) {
	__sync_fetch_and_add(&log_skipped_bytes, count);
}

void log_init (void) {
	assert(log_stream == NULL);
	log_stream = xStreamBufferCreateStatic(
		LOG_BUFFER_SIZE,
		1,  // trigger level
		log_buffer,
		&log_stream_static
	);
	assert(log_stream != NULL);
}

void log_write (const char * buf, size_t len) {
	// log buffer not yet initialized
	if (log_stream == NULL) {
		log_write_skip(len);
		return;
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
}
