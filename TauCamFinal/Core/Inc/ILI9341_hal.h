#ifndef ILI9341_HAL_H
#define ILI9341_HAL_H

#include "stm32l1xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

#define ILI9341_CASET   0x2A
#define ILI9341_PASET   0x2B
#define ILI9341_RAMWR   0x2C

#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320

#define ILI9341_BLACK   0x0000
#define ILI9341_BLUE    0x001F
#define ILI9341_RED     0xF800
#define ILI9341_GREEN   0x07E0
#define ILI9341_CYAN    0x07FF
#define ILI9341_MAGENTA 0xF81F
#define ILI9341_YELLOW  0xFFE0
#define ILI9341_WHITE   0xFFFF

typedef struct {
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
    GPIO_TypeDef *dc_port;
    uint16_t dc_pin;
    GPIO_TypeDef *rst_port;
    uint16_t rst_pin;
    uint16_t width;
    uint16_t height;
} ILI9341_Handle;

void ILI9341_Init(ILI9341_Handle *handle, SPI_HandleTypeDef *hspi,
                  GPIO_TypeDef *cs_port, uint16_t cs_pin,
                  GPIO_TypeDef *dc_port, uint16_t dc_pin,
                  GPIO_TypeDef *rst_port, uint16_t rst_pin);
void ILI9341_StartWrite(ILI9341_Handle *handle);
void ILI9341_EndWrite(ILI9341_Handle *handle);
void ILI9341_SetAddrWindow(ILI9341_Handle *handle, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void ILI9341_WritePixel(ILI9341_Handle *handle, uint16_t color);
void ILI9341_DrawPixel(ILI9341_Handle *handle, uint16_t x, uint16_t y, uint16_t color);
void ILI9341_FillScreen(ILI9341_Handle *handle, uint16_t color);
void ILI9341_FillScreenFast(ILI9341_Handle *handle, uint16_t color);
void ILI9341_FillRect(ILI9341_Handle *handle, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ILI9341_DrawRect(ILI9341_Handle *handle, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ILI9341_DrawHLine(ILI9341_Handle *handle, uint16_t x, uint16_t y, uint16_t w, uint16_t color);
void ILI9341_DrawVLine(ILI9341_Handle *handle, uint16_t x, uint16_t y, uint16_t h, uint16_t color);
void ILI9341_DrawLine(ILI9341_Handle *handle, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void ILI9341_DrawImage(ILI9341_Handle *handle, uint16_t x, uint16_t y,
                       uint16_t w, uint16_t h, const uint16_t *data);

#endif
