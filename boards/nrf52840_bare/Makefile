THIS_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
LANOT_ROOT := $(THIS_DIR)/../../

# ### compilation ###

INC_FOLDERS := $(INC_FOLDERS) $(THIS_DIR)

SRC_FILES := $(THIS_DIR)/hooks.c

LINKER_SCRIPT := $(THIS_DIR)/sys.ld

#include $(LANOT_ROOT)sys/bt/Makefile
include $(LANOT_ROOT)sys/Makefile

# ### flashing ###

OPENOCD := $(LANOT_ROOT)sys/openocd/src/openocd
OPENOCD_SCRIPTS := $(LANOT_ROOT)sys/openocd/tcl/
# If you have openocd newer than 0.10.0 installed system-wide.
# OPENOCD := openocd
# OPENOCD_SCRIPTS := /usr/share/openocd/scripts/

.PHONY: flash

# flash using openocd/stlink
flash: _build/nrf52840_xxaa.out
	$(OPENOCD) -s $(OPENOCD_SCRIPTS) \
		-c "adapter speed 1000" \
		-f interface/stlink.cfg \
		-f target/nrf52.cfg \
		-c "program $^ verify reset exit"

log:
	$(OPENOCD) -s $(OPENOCD_SCRIPTS) \
		-c "adapter speed 1000" \
		-f interface/stlink.cfg \
		-f target/nrf52.cfg \
		-c "init" \
		-c "arm semihosting enable" \
		-c "reset run" \

usb_log:
	socat /dev/ttyACM1,raw,echo=0 -
