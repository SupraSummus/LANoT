#include "flash.h"

//static void callback(nrf_fstorage_evt_t * p_evt);

//NRF_FSTORAGE_DEF(nrf_fstorage_t my_instance) = {
//	.evt_handler    = callback,
//	.start_addr     = 0xFD000,
//	.end_addr       = 0xFFFFF,
//};

void flash_init(void) {
	//nrf_fstorage_init(
	//    &my_instance,       /* You fstorage instance, previously defined. */
	//	&nrf_fstorage_sd,   /* Name of the backend. */
	//	NULL                /* Optional parameter, backend-dependant. */
	//);
}

bool flash_read_user_page(uint32_t page_num, uint8_t * buf) {
	return true;
}

bool flash_write_user_page(uint32_t page_num, uint8_t const * buf) {
	//nrf_fstorage_write(
	//	nrf_fstorage_t const *p_fs,
	//	uint32_t dest,
	//	buf, FLASH_PAGE_SIZE,
	//	void *p_param
	//);
	return true;
}
