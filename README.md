# STM32-MCUBoot

markdown
Copy
Edit
# STM32 Nucleo-L4A6ZG MCUboot Example (GCC/Make)

## About

This project sets up **MCUboot** on the **STM32 Nucleo-L4A6ZG** board.

It includes:

- A basic MCUboot bootloader (no encryption, no rollback).
- A simple application (blinky LED).
- GCC/Makefile build system.
- No STM32CubeMX, no Mbed, no HAL unless needed.
- Clean and simple setup to help understand MCUboot on STM32.

MCUboot is pulled directly from upstream as a Git submodule.

This project follows ideas from:
- [Memfault MCUboot Overview](https://interrupt.memfault.com/blog/mcuboot-overview)
- [MCUboot GitHub](https://github.com/mcu-tools/mcuboot)

---

## Flash Layout

Flash is divided like this:

| Region         | Start Address | Size    | Purpose              |
|-----------------|---------------|---------|----------------------|
| Bootloader      | 0x08000000     | 32 KB   | MCUboot bootloader    |
| Primary Slot    | 0x08008000     | 128 KB  | Main application     |
| Secondary Slot  | 0x08028000     | 128 KB  | New firmware update  |

- No scratch area.
- Overwrite-only update mode.

---

## Build Instructions

**Clone project:**
```bash
git clone --recursive <repo-url>
(If you forgot --recursive, do git submodule update --init --recursive.)

Toolchain:

GCC ARM Embedded (arm-none-eabi-gcc)

Python 3 (for imgtool.py)

Build bootloader:

bash
Copy
Edit
cd bootloader
make
Build application:

bash
Copy
Edit
cd ../app
make
Sign application image:

bash
Copy
Edit
python ../mcuboot/scripts/imgtool.py sign \
    --header-size 0x100 --align 8 --pad-header \
    --version 1.0.0 --slot-size 0x20000 \
    --key none \
    app.bin app_flash.bin
Flash Instructions
Flash bootloader to 0x08000000:

bash
Copy
Edit
openocd -f board/st_nucleo_l4.cfg -c "program bootloader.bin verify reset exit 0x08000000"
Flash signed app to 0x08008000:

bash
Copy
Edit
openocd -f board/st_nucleo_l4.cfg -c "program app_flash.bin verify reset exit 0x08008000"
(Or use STM32CubeProgrammer or ST-Link Utility.)

Reset board.
Bootloader will run, find the app, and jump to it.
You should see the user LED blinking.

Firmware Update Flow (Manual Test)
Build new app (different blink rate).

Sign it with a higher version (e.g., 2.0.0).

Flash new signed app to secondary slot (0x08028000).

Reset the board.

Bootloader will detect new firmware, copy it to primary slot, and boot it.

Notes
No signature checking (no crypto).

No rollback.

Flash operations done using STM32 internal flash HAL.

App must be linked to start after 0x100-byte MCUboot header.

Bootloader sets up vector table for app before jumping.

Goals
✅ Get MCUboot running on STM32 without CubeMX.
✅ Learn how to manually manage flash layout, linker scripts, and memory offsets.
✅ Use pure Makefiles and GCC, no extra bloat.
✅ Build something simple first, then add security later if needed.