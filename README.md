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

-> Removed until I have a working example for now. 

Don't want to "Do a False Advertising 😅"

Goals

✅ Get MCUboot running on STM32 without CubeMX.

✅ Learn how to manually manage flash layout, linker scripts, and memory offsets.

✅ Use pure Makefiles and GCC, no extra bloat.

✅ Build something simple first, then add security later if needed.
