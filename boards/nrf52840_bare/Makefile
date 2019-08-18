THIS_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
PROJECT_ROOT := $(THIS_DIR)/../..

# ### compilation ###

INC_FOLDERS := $(THIS_DIR)

SRC_FILES := $(THIS_DIR)/hooks.c

LINKER_SCRIPT := $(THIS_DIR)/sys.ld

include $(PROJECT_ROOT)/sys/Makefile


# ### flashing ###

OPENOCD := $(PROJECT_ROOT)/sys/openocd/src/openocd
OPENOCD_SCRIPTS := $(PROJECT_ROOT)/sys/openocd/tcl/

.PHONY: flash

# flash using openocd/stlink
flash: _build/nrf52840_xxaa.out
	$(OPENOCD) -s $(OPENOCD_SCRIPTS) \
		-c "adapter_khz 1000" \
		-f interface/stlink.cfg \
		-f target/nrf52.cfg \
		-c "program $^ verify reset exit"
