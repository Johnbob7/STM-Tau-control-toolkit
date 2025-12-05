# STM Tau Control Toolkit

This repository contains STM32CubeIDE projects for the STM32L152RE family that demonstrate how to control a FLIR Tau thermal camera over UART. The projects target NUCLEO-L152RE-class hardware (STM32L152RET6 MCU) and can be opened directly in STM32CubeIDE.

## Repository layout
- **`stmhardwareproject/`** – "camcontrol" firmware that exposes a UART console for commanding a FLIR Tau module and relaying its responses. It uses USART1 for the Tau connection and USART2 (ST-Link VCP) for a host terminal.

## Hardware overview
- **MCU / board**: STM32L152RET6 in LQFP64 (e.g., NUCLEO-L152RE) as configured in the `.ioc` files.
- **Tau link (stmhardwareproject)**: USART1 at 57,600 baud (PA9=TX, PA10=RX). Connect to the Tau serial interface with appropriate level shifting if required by your module variant.
- **Debug console**: USART2 at 115,200 baud (PA2=TX, PA3=RX) presented via the on-board ST-Link VCP; use a terminal like PuTTY with 8-N-1.
- **User LED**: LD2 on PA5 is available for quick sanity checks in the camcontrol project.

## Building and flashing
The project is STM32CubeIDE-ready:
1. Open STM32CubeIDE and choose **File → Import → Existing Projects into Workspace**.
2. Point to the repository root and select `stmhardwareproject`.
3. Build the project; CubeIDE regenerates code from the `.ioc` files as needed.
4. Connect the NUCLEO board over USB and launch a debug session to flash and run.

## Using the Tau console (stmhardwareproject)
The camcontrol firmware initializes both UARTs at boot and then polls the debug console for single-character commands.

1. Wire the Tau module to USART1 (PA9/PA10) and power the camera according to its datasheet.
2. Open a serial terminal to the board’s VCP at 115,200 baud and reset the MCU.
3. On startup you will see a help banner that lists the available commands.
4. Press one of the hotkeys below; the firmware forwards the corresponding packet to the Tau, waits for its reply, and prints the status bytes.

Available commands (case-insensitive where noted):
- `a` – set FFC mode to **Auto** (0x0001) and print the response.
- `G` – request the current FFC mode.
- LUT hotkeys (Video LUT function 0x10):
  - `w` white hot, `b` black hot, `f` fusion, `r` rainbow
  - `g` globow, `i` ironbow1, `k` ironbow2, `s` sepia
  - `1` color1, `2` color2, `x` ice & fire, `n` rain, `c` custom1
- `l` – query the current LUT and print its numeric ID plus a friendly name.
- Any other key reprints the help banner.

Internally, `CamControl_Init` stores the UART handles, prints the help text, and subsequent calls to `CamControl_Task` perform non-blocking character polling on USART2. When a valid command arrives, helper functions such as `Tau_SendSetFfcAuto`, `Tau_GetVideoLUT`, and the various `Tau_SetLUT_*` routines transmit the predefined Tau protocol frames, read back a 12-byte reply, and decode the status code for the console.

## License
The project includes STMicroelectronics’ default copyright header; see `LICENSE` for details.
