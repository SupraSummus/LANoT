THIS_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
LANOT_ROOT := $(THIS_DIR)/../../

# ### compilation ###

INC_FOLDERS := $(INC_FOLDERS) $(THIS_DIR)

SRC_FILES := $(THIS_DIR)/hooks.c

LINKER_SCRIPT := $(THIS_DIR)/sys.ld

include $(LANOT_ROOT)sys/bt/Makefile
include $(LANOT_ROOT)sys/Makefile

# ### flashing ###

OPENOCD := $(LANOT_ROOT)sys/openocd/src/openocd
OPENOCD_SCRIPTS := $(LANOT_ROOT)sys/openocd/tcl/

.PHONY: flash

# flash using openocd/stlink
flash: _build/nrf52840_xxaa.out
	$(OPENOCD) -s $(OPENOCD_SCRIPTS) \
		-c "adapter_khz 1000" \
		-f interface/stlink.cfg \
		-f target/nrf52.cfg \
		-c "program $^ verify reset exit"
