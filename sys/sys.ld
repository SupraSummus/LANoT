/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)
OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
ENTRY(Reset_Handler)

INCLUDE common/memory.ld

/* ….define the variables required by FreeRTOS-MPU. First ensure the section sizes
are a binary power of two to comply with the MPU region size rules. */
_Privileged_Functions_Region_Size = 256K;
ASSERT(
	_Privileged_Functions_Region_Size == LENGTH(FLASH_MBR) + LENGTH(FLASH_SD) + LENGTH(FLASH_APP_SYS),
	"error in privileged functions region size"
)
_Privileged_Data_Region_Size = 32k;
ASSERT(
	_Privileged_Data_Region_Size == LENGTH(RAM_SD) + LENGTH(RAM_APP_SYS),
	"error in privileged data region size"
)

/* Then define the variables themselves. */
__FLASH_segment_start__ = ORIGIN( FLASH );
__FLASH_segment_end__ = __FLASH_segment_start__ + LENGTH( FLASH );
__privileged_functions_start__ = ORIGIN( FLASH );
__privileged_functions_end__ = __privileged_functions_start__ + _Privileged_Functions_Region_Size;
__SRAM_segment_start__ = ORIGIN( RAM );
__SRAM_segment_end__ = __SRAM_segment_start__ + LENGTH( RAM );
__privileged_data_start__ = ORIGIN( RAM );
__privileged_data_end__ = ORIGIN( RAM ) + _Privileged_Data_Region_Size;

SECTIONS {
	/* interupt vector */
	/* .isr_vector : {
		KEEP(*(.isr_vector))
	} > FLASH */

	.text : {
		KEEP(*(.isr_vector))

		*(privileged_functions)  /* freertos private code */
		*(.text*)

		KEEP(*(.init))
		KEEP(*(.fini))

		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)

		/* .dtors */
		*crtbegin.o(.dtors)
		*crtbegin?.o(.dtors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
		*(SORT(.dtors.*))
		*(.dtors)

		*(.rodata*)

		KEEP(*(.eh_frame*))
	} > FLASH_APP_SYS

	.text.nrf_stuff : {
		PROVIDE(__start_sdh_soc_observers = .);
		KEEP(*(SORT(.sdh_soc_observers*)))
		PROVIDE(__stop_sdh_soc_observers = .);

		PROVIDE(__start_sdh_stack_observers = .);
		KEEP(*(SORT(.sdh_stack_observers*)))
		PROVIDE(__stop_sdh_stack_observers = .);

		/* PROVIDE(__start_pwr_mgmt_data = .);
		KEEP(*(SORT(.pwr_mgmt_data*)))
		PROVIDE(__stop_pwr_mgmt_data = .);

		PROVIDE(__start_log_const_data = .);
		KEEP(*(SORT(.log_const_data*)))
		PROVIDE(__stop_log_const_data = .); */
	} > FLASH_APP_SYS

	.ARM.extab : {
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > FLASH_APP_SYS

	__exidx_start = .;
	.ARM.exidx : {
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > FLASH_APP_SYS
	__exidx_end = .;

	__etext = .;

	freertos_system_calls : {
		*(freertos_system_calls)
	} > FLASH_APP_SYSCALLS

	.data : AT (__etext) {
		. = ALIGN(4);
        __data_start__ = .;

        *(vtable)
        *(privileged_data)
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);


        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))

        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;

    } > RAM_APP_SYS

	.bss : {
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
	} > RAM_APP_SYS

	.heap (COPY): {
		. = ALIGN(4);
        __HeapBase = .;
        __end__ = .;
        PROVIDE(end = .);
        KEEP(*(.heap*))
        . = ALIGN(4);
        __HeapLimit = .;
    } > RAM_APP_SYS

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy (COPY):
    {
        KEEP(*(.stack*))
    } > RAM_APP_SYS

    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ORIGIN(RAM_APP_SYS) + LENGTH(RAM_APP_SYS);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __HeapLimit, "region RAM_APP_SYS overflowed with stack")
    
    /* Check if text sections + data exceeds FLASH limit */
    DataInitFlashUsed = __bss_start__ - __data_start__;
    CodeFlashUsed = __etext - ORIGIN(FLASH_APP_SYS);
    TotalFlashUsed = CodeFlashUsed + DataInitFlashUsed;
    ASSERT(
		TotalFlashUsed <= LENGTH(FLASH_APP_SYS),
		"region FLASH_APP_SYS overflowed with .data and user data"
	)
}