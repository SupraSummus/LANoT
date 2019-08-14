#ifndef FLASH_H
#define FLASH_H

#include <stdbool.h>
#include <inttypes.h>

#define FLASH_PAGE_SIZE (256)

extern void flash_init(void);

extern bool flash_read_user_page(uint32_t page_num, uint8_t * buf);
extern bool flash_write_user_page(uint32_t page_num, uint8_t const * buf);

#endif
