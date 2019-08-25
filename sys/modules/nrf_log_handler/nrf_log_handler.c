#include <unistd.h>

#include "nrf_log.h"
#include "nrf_log_backend_interface.h"
#include "nrf_log_backend_serial.h"
#include "nrf_log_ctrl.h"

#include "io.h"
#include "io_helpers.h"

#define LOG_SUBSYSTEM "nrf_log_handler"
#include "log.h"

static const int LOGGING_FILENO = 2;

static void tx_func (void const * user_p, char const * buf, size_t len) {
	(void)user_p;
	write_all(LOGGING_FILENO, buf, len);
}

static void put (nrf_log_backend_t const *p_backend, nrf_log_entry_t *p_msg) {
	static uint8_t buf[16];
	nrf_log_backend_serial_put(p_backend, p_msg, buf, 16, tx_func);
}

static void panic_set (nrf_log_backend_t const *p_backend) {
	(void)p_backend;
	io_use_synchronous_mode(LOGGING_FILENO);
}

// no-op
static void flush (nrf_log_backend_t const *p_backend) {
	(void)p_backend;
}

static nrf_log_backend_api_t fileno_log_backend_api = {
	.put = put,
	.panic_set = panic_set,
	.flush = flush,
};

NRF_LOG_BACKEND_DEF(fileno_log_backend, fileno_log_backend_api, NULL);

void nrf_log_handler_init(void) {
	INFO("initializing nrf log (backend fd=%d)", LOGGING_FILENO);
	ret_code_t ret = NRF_LOG_INIT(NULL);
	if (ret != NRF_SUCCESS) {
		WARN("failed to init nrf log module (code %ld)", ret);
		return;
	}
	int32_t ret2 = nrf_log_backend_add(&fileno_log_backend, NRF_LOG_SEVERITY_DEBUG);
	if (ret2 < 0) {
		WARN("failed to add backend for nrf log module (code %ld)", ret2);
		return;
	}
	nrf_log_backend_enable(&fileno_log_backend);
	NRF_LOG_INFO("nrf log is operational");
	NRF_LOG_FLUSH();
}
