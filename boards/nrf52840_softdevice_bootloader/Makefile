THIS_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
PROJECT_ROOT := $(THIS_DIR)/../..

# ### compilation ###

INC_FOLDERS := $(THIS_DIR)

SRC_FILES := $(THIS_DIR)/hooks.c

LINKER_SCRIPT := $(THIS_DIR)/sys.ld

include $(PROJECT_ROOT)/sys/Makefile

# ### flashing + USB communication ###

.PHONY: flash comm log

_build/pkg.zip: _build/nrf52840_xxaa.hex
	pipenv run nrfutil pkg generate --application $^ --hw-version 52 --sd-req 0xB6 --application-version 1 --key-file fanstel.pem $@

flash: _build/pkg.zip
	pipenv run nrfutil dfu usb-serial --package $^ --port /dev/ttyACM0

comm:
	socat /dev/ttyACM0,raw,echo=0 -

log:
	socat /dev/ttyACM1,raw,echo=0 -
