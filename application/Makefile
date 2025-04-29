########################################################################################################
# Makefile - Build Application for MCUboot (Magic Packet written via Linker)
########################################################################################################

TARGET = NucleoBaseExample
BUILD_DIR = build

DEBUG = 1
OPT = -Og

ROOT_DIR := $(abspath ..)

COMMON_DIR := $(ROOT_DIR)/common
COMMON_INC_DIR := $(COMMON_DIR)/include
COMMON_SRC_DIR := $(COMMON_DIR)/src

MCUBOOT_DIR := $(ROOT_DIR)/mcuboot/boot/bootutil
MCUBOOT_SRC_DIR := $(MCUBOOT_DIR)/src
MCUBOOT_INC_DIR := $(MCUBOOT_DIR)/include

MBEDTLS_DIR := $(ROOT_DIR)/mcuboot/ext/mbedtls/library

IMGTOOL_PY := $(ROOT_DIR)/mcuboot/scripts/imgtool.py
IMG_SIGN_KEY := $(ROOT_DIR)/DebugKeys/root-ec-p256.pem

PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

CPU = -mcpu=cortex-m4
FPU = -mfpu=fpv4-sp-d16
FLOAT-ABI = -mfloat-abi=hard
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

CFLAGS =
CFLAGS += $(MCU) -Wall -fdata-sections -ffunction-sections $(OPT)
CFLAGS += -DUSE_HAL_DRIVER -DSTM32L4A6xx
CFLAGS += -ICore/Inc
CFLAGS += -I../Drivers/STM32L4xx_HAL_Driver/Inc
CFLAGS += -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy
CFLAGS += -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include
CFLAGS += -I../Drivers/CMSIS/Include
CFLAGS += -I$(COMMON_INC_DIR)
CFLAGS += -I$(MCUBOOT_INC_DIR)
CFLAGS += -I$(ROOT_DIR)/mcuboot/ext/mbedtls/include

ifeq ($(DEBUG),1)
CFLAGS += -g -gdwarf-2
endif

CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

ASFLAGS = $(CFLAGS)

LDSCRIPT = STM32L4A6ZGTx_FLASH_APPLICATION.ld
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
LIBS = -lc -lm -lnosys

C_SOURCES = \
    Core/Src/main.c \
    Core/Src/stm32l4xx_it.c \
    Core/Src/stm32l4xx_hal_msp.c \
    Core/Src/log_port.c \
    $(COMMON_SRC_DIR)/mcuboot_port.c \
    Core/Src/shell_port.c \
    Core/Src/shell.c \
    Core/Src/shell_commands.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c \
    ../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c \
    Core/Src/system_stm32l4xx.c \
    Core/Src/sysmem.c \
    Core/Src/syscalls.c \
    $(MCUBOOT_SRC_DIR)/caps.c \
    $(MCUBOOT_SRC_DIR)/loader.c \
    $(MCUBOOT_SRC_DIR)/image_rsa.c \
    $(MCUBOOT_SRC_DIR)/swap_scratch.c \
    $(MCUBOOT_SRC_DIR)/fault_injection_hardening.c \
    $(MCUBOOT_SRC_DIR)/image_ecdsa.c \
    $(MCUBOOT_SRC_DIR)/boot_record.c \
    $(MCUBOOT_SRC_DIR)/bootutil_public.c \
    $(MCUBOOT_SRC_DIR)/tlv.c \
    $(MCUBOOT_SRC_DIR)/image_validate.c \
    $(MCUBOOT_SRC_DIR)/bootutil_misc.c \
    $(MCUBOOT_SRC_DIR)/fault_injection_hardening_delay_rng_mbedtls.c \
    $(MCUBOOT_SRC_DIR)/swap_move.c \
    $(MCUBOOT_SRC_DIR)/swap_misc.c \
    $(MCUBOOT_SRC_DIR)/image_ed25519.c \
    $(MCUBOOT_SRC_DIR)/swap_offset.c \
    $(MCUBOOT_SRC_DIR)/encrypted.c \
    $(MBEDTLS_DIR)/asn1parse.c \
    $(MBEDTLS_DIR)/asn1write.c \
    $(MBEDTLS_DIR)/ecp.c \
    $(MBEDTLS_DIR)/ecp_curves.c \
    $(MBEDTLS_DIR)/error.c \
    $(MBEDTLS_DIR)/oid.c \
    $(MBEDTLS_DIR)/pk.c \
    $(MBEDTLS_DIR)/sha256.c \
    $(MBEDTLS_DIR)/bignum.c \
    $(MBEDTLS_DIR)/ecdsa.c \
    $(MBEDTLS_DIR)/platform_util.c 

ASM_SOURCES = startup_stm32l4a6xx.s

OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.o) $(ASM_SOURCES:.s=.o)))

vpath %.c $(sort $(dir $(C_SOURCES)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

# Build rules
all: $(BUILD_DIR)/$(TARGET)-signed.bin

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@ 

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(ASFLAGS) $< -o $@ 

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@
	$(SZ) $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR)/$(TARGET)-signed.bin: $(BUILD_DIR)/$(TARGET).bin
	python3 $(IMGTOOL_PY) sign --key $(IMG_SIGN_KEY) --header-size 0x200 --align 8 -S 0x20000 --pad --pad-header -v 1.0.0 $< $@


$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
