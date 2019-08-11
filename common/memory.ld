MEMORY {
	FLASH                     (rx)  : ORIGIN = 0x00000000, LENGTH = 1M
	FLASH_MBR                 (rx)  : ORIGIN = 0x00000000, LENGTH = 4k
	FLASH_SD                  (rx)  : ORIGIN = 0x00001000, LENGTH = 148k
	FLASH_APP                 (rx)  : ORIGIN = 0x00026000, LENGTH = 840k
	FLASH_APP_SYS             (rx)  : ORIGIN = 0x00026000, LENGTH = 104k
	FLASH_APP_USER            (rx)  : ORIGIN = 0x00040000, LENGTH = 736k
	FLASH_BOOTLOADER          (rx)  : ORIGIN = 0x000F8000, LENGTH = 24k
	FLASH_MBR_PARAMS          (r)   : ORIGIN = 0x000FE000, LENGTH = 4k
	FLASH_BOOTLOADER_SETTINGS (r)   : ORIGIN = 0x000FF000, LENGTH = 4k

	RAM                       (rwx) : ORIGIN = 0x20000000, LENGTH = 256k
	RAM_SD                    (rwx) : ORIGIN = 0x20000000, LENGTH = 0x22e0
	RAM_APP_SYS               (rwx) : ORIGIN = 0x200022e0, LENGTH = 0xdd20
	RAM_APP_USER              (rwx) : ORIGIN = 0x20010000, LENGTH = 192k
}
