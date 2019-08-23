THIS_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
NIMBLE_ROOT := $(THIS_DIR)mynewt-nimble/

NIMBLE_IGNORE += \
  $(NIMBLE_ROOT)/nimble/host/src/ble_gatts_lcl.c \

include $(NIMBLE_ROOT)porting/nimble/Makefile.defs
include $(NIMBLE_ROOT)porting/nimble/Makefile.tinycrypt

SRC_FILES += \
  $(NIMBLE_ROOT)porting/npl/freertos/src/npl_os_freertos.c \
  $(NIMBLE_SRC) \
  $(THIS_DIR)bt.c \
  $(TINYCRYPT_SRC) \
  $(NIMBLE_ROOT)nimble/transport/ram/src/ble_hci_ram.c \

INC_FOLDERS += \
  $(NIMBLE_INCLUDE) \
  $(NIMBLE_ROOT)porting/npl/freertos/include/ \
  $(THIS_DIR)include/ \
  $(TINYCRYPT_INCLUDE) \
  $(NIMBLE_ROOT)nimble/transport/ram/include/ \
