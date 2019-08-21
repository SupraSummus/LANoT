#include <stddef.h>

#include "app_error.h"
#include "nrf_fstorage.h"
#include "nrf_fstorage_nvmc.h"

#include "flash.h"

static void callback(nrf_fstorage_evt_t * p_evt);

NRF_FSTORAGE_DEF(nrf_fstorage_t user_flash_fsotrage) = {
	.evt_handler    = callback,
	.start_addr     = (uint32_t)user_flash,
	.end_addr       = (uint32_t)user_flash + user_flash_size,
};

static void callback(nrf_fstorage_evt_t * p_evt) {
}

static void fstorage_wait() {
	// FIXME busy waiting ;/
	while (nrf_fstorage_is_busy(&user_flash_fsotrage));
}

void flash_init(void) {
	fstorage_wait();
	ret_code_t ret = nrf_fstorage_init(
		&user_flash_fsotrage, /* You fstorage instance, previously defined. */
		&nrf_fstorage_nvmc,   /* Name of the backend. */
		NULL                  /* Optional parameter, backend-dependant. */
	);
	APP_ERROR_CHECK(ret);
	fstorage_wait();
}

bool flash_read_user_page(uint32_t page_num, uint8_t * buf) {
	fstorage_wait();
	// TODO read
	return true;
}

bool flash_write_user_page(uint32_t page_num, uint8_t const * buf) {
	fstorage_wait();
	ret_code_t ret = nrf_fstorage_write(
		&user_flash_fsotrage,
		(uint32_t)user_flash + page_num * FLASH_PAGE_SIZE,
		buf, FLASH_PAGE_SIZE,
		NULL
	);
	APP_ERROR_CHECK(ret);
	fstorage_wait();
	return ret == NRF_SUCCESS;
}
