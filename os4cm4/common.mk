CC := arm-none-eabi-gcc
LD := arm-none-eabi-ld
AS := arm-none-eabi-as
FLAGS :=
CFLAGS := -Wall -Wextra
LDFLAGS := -nostdlib

BUILD_DIR := $(abspath _build)/

FLAGS += -mcpu=cortex-m4
FLAGS += -mthumb -mabi=aapcs
FLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16

# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums
CFLAGS += -O3
CFLAGS += -DARMCM4_FP

CFLAGS += $(addprefix -I,$(INCLUDE_DIRS))

# link-time optimization
FLAGS += -flto

LDFLAGS += -Wl,--gc-sections

OBJECTS := $(SOURCES)
OBJECTS := $(addprefix $(BUILD_DIR),$(OBJECTS))
OBJECTS := $(addsuffix .o,$(OBJECTS))
OBJECTS := $(abspath $(OBJECTS))

OPENOCD_ROOT := $(OS4CM4_ROOT)../sys/openocd/
OPENOCD := $(OPENOCD_ROOT)src/openocd
OPENOCD_SCRIPTS := $(OPENOCD_ROOT)tcl/
OPENOCD_FLAGS := \
	-s $(OPENOCD_SCRIPTS) \
	-c "adapter speed 1000" \
	-f interface/stlink.cfg \
	-f target/nrf52.cfg \

all: $(BUILD_DIR)os4cm4

$(BUILD_DIR)os4cm4: $(OBJECTS)
	mkdir -p $(dir $@)
	$(CC) $(FLAGS) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $@ $^

$(BUILD_DIR)%.c.o: /%.c
	mkdir -p $(dir $@)
	$(CC) $(FLAGS) $(CFLAGS) -c -o $@ $^

$(BUILD_DIR)%.S.o: /%.S
	mkdir -p $(dir $@)
	$(CC) $(FLAGS) $(CFLAGS) -c -o $@ $^

clean:
	rm -r $(BUILD_DIR)
