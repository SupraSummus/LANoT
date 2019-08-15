MEMORY {
	FLASH                     (rx)  : ORIGIN = 0x00000000, LENGTH = 1M
	FLASH_SYS                 (rx)  : ORIGIN = 0x00000000, LENGTH = 128k
	FLASH_USER                (rx)  : ORIGIN = 0x00020000, LENGTH = 896k

	RAM                       (rwx) : ORIGIN = 0x20000000, LENGTH = 256k
	RAM_SYS                   (rwx) : ORIGIN = 0x20000000, LENGTH = 64k
	RAM_USER                  (rwx) : ORIGIN = 0x20010000, LENGTH = 192k
}
