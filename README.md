# STM Tau Control Toolkit

Firmware and utilities for driving an STM32L152RE-based controller with an SPI-connected ILI9341 TFT display and a TSC2007 resistive touch panel. The CubeIDE project demonstrates rendering a 240×320 RGB565 image and reloading it whenever the screen is touched, plus a Python helper script for converting standard BMP assets into RGB565 headers consumable by the firmware.

## Repository layout
- `TauCamFinal/`: STM32CubeIDE project targeting the STM32L152RET6 MCU, configured with SPI1 for the ILI9341 display, I2C1 for the TSC2007 touch controller, and USART2 for logging/debug. Generated with STM32CubeMX 6.15.0 and GCC toolchain settings preserved in the `.ioc` file.
- `TauCamFinal/Core/Src/main.c`: Application entry point that initializes HAL drivers, brings up the display driver, configures the touch controller, and redraws the image when a touch is detected.
- `TauCamFinal/Core/Inc/ILI9341_hal.h`: Minimal HAL-based driver API for the ILI9341 TFT (initialization, pixel/rect drawing, and image blitting helpers).
- `TauCamFinal/Core/Inc/Adafruit_TSC2007.h`: STM32 HAL wrapper for the TSC2007 touch controller, including helpers to map ADC readings to display pixels.
- `bmp to stmdata.py`: Python conversion utility that reads a 24/32-bit BMP and emits both a raw RGB565 binary and a C header containing a `uint16_t` pixel array.
- `tiger.bmp`, `tiger_rgb565.bin`, `tiger_rgb565.h`: Example image asset and the generated outputs from the conversion script used by the firmware sample.

## Building and flashing
1. Open STM32CubeIDE (tested with 1.15.x) and choose **File → Import → Existing Projects into Workspace**.
2. Select the `TauCamFinal` directory; the IDE will pick up the `.project`, linker scripts, and CubeMX configuration.
3. Build the project with the default Debug configuration. The project targets an STM32L152RET6 (e.g., the NUCLEO-L152RE board) with external ST-LINK programming/debug support.
4. Connect the target board via ST-LINK, ensure the ILI9341 and TSC2007 hardware are wired to the pins defined in `Core/Inc/main.h` (`LCD_CS/DC/RST` plus SPI1, I2C1, and the PC13 user button), and flash the image.

## Runtime behavior
- On boot, the firmware initializes the HAL, configures system clocks, and sets up GPIO, SPI, I2C, and USART peripherals.
- The display is cleared and an RGB565 image is drawn at coordinates (0,0).
- The main loop polls the TSC2007; any touch event triggers a redraw of the image with a short debounce delay.

## Customizing the displayed image
Use the included Python helper to swap in your own asset:

1. Place a 24- or 32-bit uncompressed BMP in the repository root (or provide a path to it).
2. Edit the call in `bmp to stmdata.py` (at the bottom of the file) to reference your BMP and desired output filenames/array name, then run the script:
   ```bash
   python "bmp to stmdata.py"
   ```
   The default invocation converts `tiger.bmp` and emits `tiger_rgb565.bin` plus `tiger_rgb565.h`.
3. Copy the generated header into `TauCamFinal/Core/Inc/` and update `Core/Src/main.c` to include it and pass the array pointer to `ILI9341_DrawImage`.
4. Rebuild and flash to show the new image.

## Notes
- Pin mappings for the display control lines are documented in `Core/Inc/main.h` and should match your hardware (the sample uses GPIOB6 for CS, GPIOC7 for DC, and GPIOA10 for reset along with SPI1 data lines).
- The touch-screen mapping helpers expect the full 240×320 display area; adjust the screen dimensions in `TSC2007_Init` if you change the panel orientation or resolution.
