#ifndef ILI9341_H
#define ILI9341_H

#include "stm32l1xx_hal.h"  // Adjust to your MCU family
#include "stm32l1xx_hal_conf.h"

// LCD dimensions
#define ILI9341_WIDTH   240
#define ILI9341_HEIGHT  320

// Common colors (RGB565)
#define ILI9341_BLACK   0x0000
#define ILI9341_BLUE    0x001F
#define ILI9341_RED     0xF800
#define ILI9341_GREEN   0x07E0
#define ILI9341_CYAN    0x07FF
#define ILI9341_MAGENTA 0xF81F
#define ILI9341_YELLOW  0xFFE0
#define ILI9341_WHITE   0xFFFF

// Public functions
void ILI9341_Reset(void);
void ILI9341_Init(void);
void ILI9341_FillScreen(uint16_t color);
void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
void ILI9341_SetAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ILI9341_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void ILI9341_WriteDataMultiple(uint16_t *datas, uint32_t dataNums);

#endif
