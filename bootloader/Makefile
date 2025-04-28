ROOT_DIR := $(abspath ..)
BUILD_DIR := $(ROOT_DIR)/bootloader/build

COMPILER := arm-none-eabi-gcc
OBJCOPY  := arm-none-eabi-objcopy

# External dependencies
MCUBOOT_DIR     := $(ROOT_DIR)/mcuboot/boot/bootutil
CMSIS_CORE_INC  := $(ROOT_DIR)/STM32CubeL4/Drivers/CMSIS/Core/Include
CMSIS_DEV_INC   := $(ROOT_DIR)/STM32CubeL4/Drivers/CMSIS/Device/ST/STM32L4xx/Include
HAL_INC_DIR     := $(ROOT_DIR)/STM32CubeL4/Drivers/STM32L4xx_HAL_Driver/Inc
HAL_SRC_DIR     := $(ROOT_DIR)/STM32CubeL4/Drivers/STM32L4xx_HAL_Driver/Src

# Directories
PROJECT_DIR      := $(ROOT_DIR)/bootloader
MCUBOOT_SRC_DIR  := $(MCUBOOT_DIR)/src
MCUBOOT_INC_DIR  := $(MCUBOOT_DIR)/include
TINYCRYPT_DIR     := $(ROOT_DIR)/mcuboot/ext/tinycrypt/lib
TINYCRYPT_SRC_DIR := $(TINYCRYPT_DIR)/source
TINYCRYPT_INC_DIR := $(TINYCRYPT_DIR)/include

# Source files
SRC_FILES := \
  $(PROJECT_DIR)/main.c \
  $(PROJECT_DIR)/mcuboot_port.c \
  $(PROJECT_DIR)/serial_interface.c \
  $(PROJECT_DIR)/startup_stm32l4a6xx.s \
  $(PROJECT_DIR)/system_stm32l4xx.c \
  $(PROJECT_DIR)/flash_map_backend/flash_map.c \
  $(PROJECT_DIR)/flash_map_backend/flash_map_backend.c \
  $(PROJECT_DIR)/flash_map_backend/flash_map_extra.c \
  $(MCUBOOT_SRC_DIR)/loader.c \
  $(MCUBOOT_SRC_DIR)/bootutil_misc.c \
  $(MCUBOOT_SRC_DIR)/bootutil_public.c \
  $(MCUBOOT_SRC_DIR)/fault_injection_hardening.c \
  $(MCUBOOT_SRC_DIR)/boot_record.c \
  $(MCUBOOT_SRC_DIR)/caps.c \
  $(MCUBOOT_SRC_DIR)/encrypted.c \
  $(MCUBOOT_SRC_DIR)/fault_injection_hardening_delay_rng_mbedtls.c \
  $(MCUBOOT_SRC_DIR)/image_ed25519.c \
  $(MCUBOOT_SRC_DIR)/image_rsa.c \
  $(MCUBOOT_SRC_DIR)/image_validate.c \
  $(MCUBOOT_SRC_DIR)/swap_misc.c \
  $(MCUBOOT_SRC_DIR)/swap_move.c \
  $(MCUBOOT_SRC_DIR)/swap_scratch.c \
  $(MCUBOOT_SRC_DIR)/tlv.c \
  $(TINYCRYPT_SRC_DIR)/aes_decrypt.c \
  $(TINYCRYPT_SRC_DIR)/aes_encrypt.c \
  $(TINYCRYPT_SRC_DIR)/cbc_mode.c \
  $(TINYCRYPT_SRC_DIR)/ccm_mode.c \
  $(TINYCRYPT_SRC_DIR)/cmac_mode.c \
  $(TINYCRYPT_SRC_DIR)/ctr_mode.c \
  $(TINYCRYPT_SRC_DIR)/ctr_prng.c \
  $(TINYCRYPT_SRC_DIR)/hmac.c \
  $(TINYCRYPT_SRC_DIR)/hmac_prng.c \
  $(TINYCRYPT_SRC_DIR)/sha256.c \
  $(TINYCRYPT_SRC_DIR)/utils.c \
  $(HAL_SRC_DIR)/stm32l4xx_hal.c \
  $(HAL_SRC_DIR)/stm32l4xx_hal_cortex.c \
  $(HAL_SRC_DIR)/stm32l4xx_hal_gpio.c \
  $(HAL_SRC_DIR)/stm32l4xx_hal_rcc.c \
  $(HAL_SRC_DIR)/stm32l4xx_hal_rcc_ex.c \
  $(HAL_SRC_DIR)/stm32l4xx_hal_pwr.c \
  $(HAL_SRC_DIR)/stm32l4xx_hal_pwr_ex.c \
  $(HAL_SRC_DIR)/stm32l4xx_hal_uart.c \
+ $(HAL_SRC_DIR)/stm32l4xx_hal_dma.c

# Include paths
INCLUDE_PATHS := \
  -I$(PROJECT_DIR) \
  -I$(PROJECT_DIR)/flash_map_backend \
  -I$(PROJECT_DIR)/mcuboot_config \
  -I$(PROJECT_DIR)/sysflash \
  -I$(TINYCRYPT_INC_DIR) \
  -I$(MCUBOOT_INC_DIR) \
  -I$(CMSIS_CORE_INC) \
  -I$(CMSIS_DEV_INC) \
  -I$(HAL_INC_DIR)

# Compiler flags
CFLAGS := \
  -DSTM32L4A6xx \
  -mcpu=cortex-m4 \
  -mthumb \
  -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
  -Os \
  -g3 \
  -Wall \
  -ffunction-sections -fdata-sections \
  -nostdlib

# Linker flags
LDFLAGS := \
  -T$(PROJECT_DIR)/bootloader.ld \
  -Wl,-Map,$(BUILD_DIR)/bootloader.map \
  --specs=nano.specs \
  -Wl,--gc-sections

# Object files
OBJ_FILES := $(patsubst $(ROOT_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter %.c,$(SRC_FILES)))
OBJ_FILES += $(patsubst $(ROOT_DIR)/%.s,$(BUILD_DIR)/%.o,$(filter %.s,$(SRC_FILES)))

# Targets
TARGET     := bootloader
TARGET_ELF := $(BUILD_DIR)/$(TARGET).elf
TARGET_BIN := $(BUILD_DIR)/$(TARGET).bin

all: $(TARGET_BIN)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Link ELF
$(TARGET_ELF): $(OBJ_FILES) | $(BUILD_DIR)
	@echo "Linking ELF..."
	$(COMPILER) $(OBJ_FILES) $(CFLAGS) $(LDFLAGS) \
	  -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group -o $@

# Generate BIN
$(TARGET_BIN): $(TARGET_ELF)
	@echo "Generating BIN..."
	$(OBJCOPY) -O binary $< $@

# Compile C
$(BUILD_DIR)/%.o: $(ROOT_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(COMPILER) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Compile ASM
$(BUILD_DIR)/%.o: $(ROOT_DIR)/%.s | $(BUILD_DIR)
	@echo "Compiling $< (ASM)..."
	@mkdir -p $(dir $@)
	$(COMPILER) $(CFLAGS) $(INCLUDE_PATHS) -x assembler-with-cpp -c $< -o $@

flash: $(TARGET_BIN)
	openocd -f interface/stlink.cfg -f target/stm32l4x.cfg \
	  -c "program $(TARGET_BIN) verify reset exit 0x08000000"

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all flash clean
