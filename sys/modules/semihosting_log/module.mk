THIS_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

SRC_FILES += \
  $(THIS_DIR)semihosting_log.c \
