THIS_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

SRC_FILES += \
	$(NRF_SDK_ROOT)components/libraries/log/src/nrf_log_backend_serial.c \
	$(NRF_SDK_ROOT)components/libraries/log/src/nrf_log_frontend.c \
	$(NRF_SDK_ROOT)components/libraries/log/src/nrf_log_str_formatter.c \
	$(NRF_SDK_ROOT)external/fprintf/nrf_fprintf.c \
	$(NRF_SDK_ROOT)external/fprintf/nrf_fprintf_format.c \
	$(THIS_DIR)nrf_log_handler.c \

INC_FOLDERS += \
	$(NRF_SDK_ROOT)components/libraries/log/ \
	$(NRF_SDK_ROOT)components/libraries/log/src/ \
	$(NRF_SDK_ROOT)external/fprintf/ \
	$(THIS_DIR)include/ \

include $(LANOT_ROOT)sys/modules/nrf_sdk/balloc.mk
include $(LANOT_ROOT)sys/modules/nrf_sdk/memobj.mk
include $(LANOT_ROOT)sys/modules/nrf_sdk/ringbuf.mk
