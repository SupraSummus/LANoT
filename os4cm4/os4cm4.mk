THIS_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SRC_DIR := $(THIS_DIR)src/
CMSIS_ROOT := $(THIS_DIR)CMSIS_5/
CMSIS_SOURCES := $(CMSIS_ROOT)Device/ARM/ARMCM4/Source/

INCLUDE_DIRS += \
	$(CMSIS_ROOT)CMSIS/Core/Include/ \
	$(CMSIS_ROOT)Device/ARM/ARMCM4/Include/ \
	$(THIS_DIR)include/ \
	$(THIS_DIR)nanoprintf/ \

SOURCES += \
	$(CMSIS_SOURCES)GCC/startup_ARMCM4.S \
	$(CMSIS_SOURCES)system_ARMCM4.c \
	$(SRC_DIR)assert.c \
	$(SRC_DIR)fault.c \
	$(SRC_DIR)printk.c \
	$(SRC_DIR)shim.c \
	$(SRC_DIR)svc.c \
	$(SRC_DIR)switch.c \
	$(SRC_DIR)thread.c \
	$(SRC_DIR)user.c \

LINKER_SCRIPT := $(CMSIS_SOURCES)GCC/gcc_arm.ld
