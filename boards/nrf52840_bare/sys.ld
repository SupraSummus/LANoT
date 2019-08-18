MEMORY {
	FLASH                     (rx)  : ORIGIN = 0x00000000, LENGTH = 1M
	FLASH_SYS                 (rx)  : ORIGIN = 0x00000000, LENGTH = 128k
	FLASH_USER                (rx)  : ORIGIN = 0x00020000, LENGTH = 896k

	RAM                       (rwx) : ORIGIN = 0x20000000, LENGTH = 256k
	RAM_SYS                   (rwx) : ORIGIN = 0x20000000, LENGTH = 64k
	RAM_USER                  (rwx) : ORIGIN = 0x20010000, LENGTH = 192k
}

/* ….define the variables required by FreeRTOS-MPU. First ensure the section sizes
are a binary power of two to comply with the MPU region size rules. */
_Privileged_Functions_Region_Size = LENGTH(FLASH_SYS);
_Privileged_Data_Region_Size = LENGTH(RAM_SYS);

/* TODO deduplicate with Makefile one day */
INCLUDE ../../sys/sys.ld
