#include "ili9341.h"
#include "main.h"
#include "stm32l1xx_hal_conf.h"
#include "gpio.h"
#include "spi.h"
#include "i2c.h"
#include "main.h"
// Use the SPI handle you configured in CubeMX
extern SPI_HandleTypeDef hspi1; // change to hspi2 if needed

// --- Low-level helpers ---
 void ILI9341_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET); // Command mode
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); // Select LCD
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);   // Deselect LCD
}

 void ILI9341_WriteData(uint8_t data) {
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);   // Data mode
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); // Select LCD
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);   // Deselect LCD
}
void ILI9341_WriteDataMultiple(uint16_t * datas, uint32_t dataNums) {
     while (dataNums--)
     {
         ILI9341_WriteData(*datas++);
     }
 }

// --- Core functions ---
void ILI9341_Init(void) {
    // Reset the display
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(120);

    // Initialization commands
    ILI9341_WriteCommand(0xEF);
    uint8_t data1[] = {0x03, 0x80, 0x02};
    HAL_SPI_Transmit(&hspi1, data1, sizeof(data1), HAL_MAX_DELAY);

    ILI9341_WriteCommand(0xCF);
    uint8_t data2[] = {0x00, 0xC1, 0x30};
    HAL_SPI_Transmit(&hspi1, data2, sizeof(data2), HAL_MAX_DELAY);

    ILI9341_WriteCommand(0xED);
    uint8_t data3[] = {0x64, 0x03, 0x12, 0x81};
    HAL_SPI_Transmit(&hspi1, data3, sizeof(data3), HAL_MAX_DELAY);

    // Add other initialization commands as per the ILI9341 datasheet
}

void ILI9341_SetAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    ILI9341_WriteCommand(0x2A); // Column addr set
    ILI9341_WriteData(x >> 8);
    ILI9341_WriteData(x & 0xFF);
    ILI9341_WriteData((x+w-1) >> 8);
    ILI9341_WriteData((x+w-1) & 0xFF);

    ILI9341_WriteCommand(0x2B); // Row addr set
    ILI9341_WriteData(y >> 8);
    ILI9341_WriteData(y & 0xFF);
    ILI9341_WriteData((y+h-1) >> 8);
    ILI9341_WriteData((y+h-1) & 0xFF);

    ILI9341_WriteCommand(0x2C); // Write to RAM
}

void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if ((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
    ILI9341_SetAddrWindow(x, y, 1, 1);
    ILI9341_WriteData(color >> 8);
    ILI9341_WriteData(color & 0xFF);
}

void ILI9341_FillScreen(uint16_t color) {
    ILI9341_SetAddrWindow(0, 0, ILI9341_WIDTH - 1, ILI9341_HEIGHT - 1);
    uint8_t data[] = {color >> 8, color & 0xFF};

    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);  // Data mode
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);  // Select LCD

    for (uint32_t i = 0; i < ILI9341_WIDTH * ILI9341_HEIGHT; i++) {
        HAL_SPI_Transmit(&hspi1, data, 2, HAL_MAX_DELAY);
    }

    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);  // Deselect LCD
}

void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
    if((x + w - 1) >= ILI9341_WIDTH) return;
    if((y + h - 1) >= ILI9341_HEIGHT) return;

    ILI9341_SetAddrWindow(x, y, x+w-1, y+h-1);
    ILI9341_WriteDataMultiple((uint16_t*)data, w*h);
}
void ILI9341_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
    int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy, e2;

    while (1) {
        ILI9341_DrawPixel(x0, y0, color);  // Draw the current pixel
        if (x0 == x1 && y0 == y1) break;  // Stop if the end point is reached
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}
