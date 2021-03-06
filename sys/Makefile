PROJECT_NAME     := test
TARGETS          := nrf52840_xxaa
OUTPUT_DIRECTORY := _build

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
THIS_DIR := $(dir $(mkfile_path))

SDK_ROOT := $(THIS_DIR)/nRF5_SDK_15.3.0_59ac345/
NRF_SDK_ROOT := $(SDK_ROOT)
FREERTOS_ROOT := $(THIS_DIR)/freertos/FreeRTOS/Source/

# Source files common to all targets
SRC_FILES := $(SRC_FILES) \
  $(FREERTOS_ROOT)/event_groups.c \
  $(FREERTOS_ROOT)/list.c \
  $(FREERTOS_ROOT)/portable/GCC/ARM_CM4_MPU/port.c \
  $(FREERTOS_ROOT)/queue.c \
  $(FREERTOS_ROOT)/stream_buffer.c \
  $(FREERTOS_ROOT)/tasks.c \
  $(FREERTOS_ROOT)/timers.c \
  $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_nvmc.c \
  $(SDK_ROOT)/components/libraries/usbd/app_usbd.c \
  $(SDK_ROOT)/components/libraries/usbd/app_usbd_core.c \
  $(SDK_ROOT)/components/libraries/usbd/app_usbd_string_desc.c \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc/acm/app_usbd_cdc_acm.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_power.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_clock.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_power.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_systick.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_usbd.c \
  $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52840.S \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52840.c \
  $(SDK_ROOT)/modules/nrfx/soc/nrfx_atomic.c \
  $(THIS_DIR)/abort.c \
  $(THIS_DIR)/assert.c \
  $(THIS_DIR)/fault_handlers.c \
  $(THIS_DIR)/flash.c \
  $(THIS_DIR)/interface.c \
  $(THIS_DIR)/io.c \
  $(THIS_DIR)/log.c \
  $(THIS_DIR)/sys.c \
  $(THIS_DIR)/usb_io.c \
  $(THIS_DIR)/user_task.c \

# Include folders common to all targets
INC_FOLDERS := $(INC_FOLDERS) \
  $(FREERTOS_ROOT)/include/ \
  $(FREERTOS_ROOT)/portable/GCC/ARM_CM4_MPU/ \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/libraries/atomic/ \
  $(SDK_ROOT)/components/libraries/delay \
  $(SDK_ROOT)/components/libraries/experimental_section_vars/ \
  $(SDK_ROOT)/components/libraries/fstorage/ \
  $(SDK_ROOT)/components/libraries/log/ \
  $(SDK_ROOT)/components/libraries/log/src/ \
  $(SDK_ROOT)/components/libraries/strerror/ \
  $(SDK_ROOT)/components/libraries/usbd/ \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc/ \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc/acm/ \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/softdevice/common/ \
  $(SDK_ROOT)/components/softdevice/s140/headers/ \
  $(SDK_ROOT)/components/softdevice/s140/headers/nrf52/ \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/external/utf_converter/ \
  $(SDK_ROOT)/integration/nrfx \
  $(SDK_ROOT)/integration/nrfx/legacy/ \
  $(SDK_ROOT)/modules/nrfx \
  $(SDK_ROOT)/modules/nrfx/drivers/include/ \
  $(SDK_ROOT)/modules/nrfx/hal \
  $(SDK_ROOT)/modules/nrfx/mdk \
  $(THIS_DIR)/include \
  $(THIS_DIR)/ \

# Libraries common to all targets
LIB_FILES += \

SDK_CONFIG_FILE := $(THIS_DIR)/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar

include $(LANOT_ROOT)sys/modules/nrf_log_handler/Makefile
include $(LANOT_ROOT)sys/modules/semihosting_log/module.mk

# Optimization flags
OPT = -O3 -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DNRF52840_XXAA
CFLAGS += -DSWI_DISABLE0
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall
#CFLAGS += -Werror
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# C++ flags common to all targets
CXXFLAGS += $(OPT)

# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DNRF52840_XXAA
ASMFLAGS += -DSWI_DISABLE0

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

nrf52840_xxaa: CFLAGS += -D__HEAP_SIZE=4096
nrf52840_xxaa: CFLAGS += -D__STACK_SIZE=4096
nrf52840_xxaa: ASMFLAGS += -D__HEAP_SIZE=4096
nrf52840_xxaa: ASMFLAGS += -D__STACK_SIZE=4096

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm


.PHONY: default help

# Default target - first one defined
VERSION_FILE := $(OUTPUT_DIRECTORY)/version.c
default: $(VERSION_FILE) nrf52840_xxaa

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		nrf52840_xxaa
	@echo		flash_softdevice
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary

# dynamicaly generated version file
.PHONY: $(VERSION_FILE)
$(VERSION_FILE):
	mkdir -p "$(OUTPUT_DIRECTORY)"
	echo "const char version_string[] = \"$$(git describe --long --tags --dirty --always)\";" > $@
SRC_FILES += $(VERSION_FILE)

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc

include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY: flash_softdevice erase

OPENOCD := $(LANOT_ROOT)sys/openocd/src/openocd
OPENOCD_SCRIPTS := $(LANOT_ROOT)sys/openocd/tcl/

# flash softdevice using openocd/stlink
flash_softdevice:
	$(OPENOCD) -s $(OPENOCD_SCRIPTS) \
		-c "adapter_khz 1000" \
		-f interface/stlink.cfg \
		-f target/nrf52.cfg \
		-c "program $(SDK_ROOT)/components/softdevice/s140/hex/s140_nrf52_6.1.1_softdevice.hex verify reset exit"

sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
