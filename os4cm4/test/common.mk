THIS_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
OS4CM4_ROOT := $(THIS_DIR)../

INCLUDE_DIRS := \
	$(OS4CM4_ROOT)../sys/include/ \

SOURCES += \
	$(OS4CM4_ROOT)../sys/modules/semihosting_log/semihosting_log.c \

include $(OS4CM4_ROOT)os4cm4.mk
include $(OS4CM4_ROOT)common.mk

#run: $(BUILD_DIR)os4cm4
#	qemu-system-aarch64 -cpu cortex-m4 -machine virt -m 256k $^

hard: $(BUILD_DIR)os4cm4
	$(OPENOCD) $(OPENOCD_FLAGS) \
		-c "program $^ verify" \
		-c "arm semihosting enable" \
		-c "reset run"
