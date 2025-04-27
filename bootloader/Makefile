ROOT_DIR := $(abspath ..)
BUILD_DIR := $(ROOT_DIR)/bootloader/build

COMPILER := arm-none-eabi-gcc
OBJCOPY  := arm-none-eabi-objcopy

# External dependencies
MCUBOOT_DIR := $(ROOT_DIR)/mcuboot/boot/bootutil
CMSIS_INC_DIR := $(ROOT_DIR)/cmsis/CMSIS/Core/Include
STM32_HAL_INC_DIR := $(ROOT_DIR)/external/STM32CUBE/Drivers/STM32L4xx_HAL_Driver/Inc
STM32_CMSIS_DEVICE_INC_DIR := $(ROOT_DIR)/external/STM32CUBE/Drivers/CMSIS/Device/ST/STM32L4xx/Include
STM32_CMSIS_CORE_INC_DIR := $(ROOT_DIR)/external/STM32CUBE/Drivers/CMSIS/Include

# Directories
PROJECT_DIR := $(ROOT_DIR)/bootloader
MCUBOOT_SRC_DIR := $(MCUBOOT_DIR)/src
MCUBOOT_INC_DIR := $(MCUBOOT_DIR)/include

# Source files
SRC_FILES := \
    $(PROJECT_DIR)/main.c \
    $(PROJECT_DIR)/mcuboot_port.c \
    $(PROJECT_DIR)/startup_stm32l4a6xx.s \
    $(PROJECT_DIR)/system_stm32l4xx.c \
    $(PROJECT_DIR)/flash_map_backend/flash_map.c \
    $(PROJECT_DIR)/flash_map_backend/flash_map_backend.c \
    $(PROJECT_DIR)/flash_map_backend/flash_map_extra.c \
    $(MCUBOOT_SRC_DIR)/loader.c \
    $(MCUBOOT_SRC_DIR)/bootutil_misc.c \
    $(MCUBOOT_SRC_DIR)/bootutil_public.c \
    $(MCUBOOT_SRC_DIR)/fault_injection_hardening.c

# Include paths
INCLUDE_PATHS := \
    -I$(PROJECT_DIR) \
    -I$(PROJECT_DIR)/flash_map_backend \
    -I$(PROJECT_DIR)/mcuboot_config \
    -I$(PROJECT_DIR)/sysflash \
    -I$(MCUBOOT_INC_DIR) \
    -I$(CMSIS_INC_DIR) \
    -I$(STM32_HAL_INC_DIR) \
    -I$(STM32_CMSIS_DEVICE_INC_DIR) \
    -I$(STM32_CMSIS_CORE_INC_DIR)

# Compiler flags
CFLAGS := \
    -mcpu=cortex-m4 \
    -mthumb \
    -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
    -Os \
    -g3 \
    -Wall -Werror \
    -ffunction-sections -fdata-sections \
    -nostdlib

# Linker flags
LDFLAGS := \
    -T$(PROJECT_DIR)/bootloader.ld \
    -Wl,-Map,$(BUILD_DIR)/bootloader.map \
    --specs=nano.specs \
    -Wl,--gc-sections

# Object files
OBJ_FILES := $(patsubst $(ROOT_DIR)/%.c, $(BUILD_DIR)/%.o, $(filter %.c, $(SRC_FILES)))
OBJ_FILES += $(patsubst $(ROOT_DIR)/%.s, $(BUILD_DIR)/%.o, $(filter %.s, $(SRC_FILES)))

# Targets
TARGET := bootloader
TARGET_ELF := $(BUILD_DIR)/$(TARGET).elf
TARGET_BIN := $(BUILD_DIR)/$(TARGET).bin

# Default target
all: $(TARGET_BIN)

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Linking ELF
$(TARGET_ELF): $(OBJ_FILES) | $(BUILD_DIR)
	@echo "Linking ELF..."
	$(COMPILER) $(OBJ_FILES) $(CFLAGS) $(LDFLAGS) -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group -o $@

# Generating BIN
$(TARGET_BIN): $(TARGET_ELF)
	@echo "Generating BIN..."
	$(OBJCOPY) -O binary $< $@

# Compile C files
$(BUILD_DIR)/%.o: $(ROOT_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(COMPILER) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Compile ASM files
$(BUILD_DIR)/%.o: $(ROOT_DIR)/%.s | $(BUILD_DIR)
	@echo "Compiling $< (ASM)..."
	@mkdir -p $(dir $@)
	$(COMPILER) $(CFLAGS) $(INCLUDE_PATHS) -x assembler-with-cpp -c $< -o $@

# Flash target
flash: $(TARGET_BIN)
	openocd -f interface/stlink.cfg -f target/stm32l4x.cfg -c "program $(TARGET_BIN) verify reset exit 0x08000000"

# Clean target
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all flash clean
