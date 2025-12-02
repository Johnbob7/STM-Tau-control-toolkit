#include "ili9341.h"
#include "main.h"
#include "stm32l1xx_hal_conf.h"
#include "gpio.h"
#include "spi.h"
#include "i2c.h"
#include "main.h"
#include <stdlib.h>
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

void ILI9341_WriteDataMultiple(const uint8_t *datas, uint32_t dataNums) {
     HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);   // Data mode
     HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); // Select LCD
     HAL_SPI_Transmit(&hspi1, (uint8_t *)datas, dataNums, HAL_MAX_DELAY);
     HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);   // Deselect LCD
 }

static void ILI9341_WriteColorMultiple(const uint16_t *colors, uint32_t count) {
     HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);   // Data mode
     HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); // Select LCD

     while (count--) {
         uint16_t color = *colors++;
         uint8_t bytes[2] = { (uint8_t)(color >> 8), (uint8_t)(color & 0xFF) };
         HAL_SPI_Transmit(&hspi1, bytes, 2, HAL_MAX_DELAY);
     }

     HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);   // Deselect LCD
 }

static void ILI9341_WriteColorRepeat(uint16_t color, uint32_t count) {
     uint8_t bytes[2] = { (uint8_t)(color >> 8), (uint8_t)(color & 0xFF) };

     HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);   // Data mode
     HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); // Select LCD

     while (count--) {
         HAL_SPI_Transmit(&hspi1, bytes, 2, HAL_MAX_DELAY);
     }

     HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);   // Deselect LCD
 }
void ILI9341_Reset(void) {
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(120);
}

// --- Core functions ---
void ILI9341_Init() {
    ILI9341_Reset();
    // command list is based on https://github.com/martnak/STM32-ILI9341
    // SOFTWARE RESET
    ILI9341_WriteCommand(0x01);
    HAL_Delay(500);

    // POWER CONTROL A
    ILI9341_WriteCommand(0xCB);
    {
        uint8_t data[] = { 0x39, 0x2C, 0x00, 0x34, 0x02 };
        ILI9341_WriteDataMultiple(data, 5);
    }

    // POWER CONTROL B
    ILI9341_WriteCommand(0xCF);
    {
        uint8_t data[] = { 0x00, 0xC1, 0x30 };
        ILI9341_WriteDataMultiple(data, 3);
    }

    // DRIVER TIMING CONTROL A
    ILI9341_WriteCommand(0xE8);
    {
        uint8_t data[] = { 0x85, 0x00, 0x78 };
        ILI9341_WriteDataMultiple(data, 3);
    }

    // DRIVER TIMING CONTROL B
    ILI9341_WriteCommand(0xEA);
    {
        uint8_t data[] = { 0x00, 0x00 };
        ILI9341_WriteDataMultiple(data, 2);
    }

    // POWER ON SEQUENCE CONTROL
    ILI9341_WriteCommand(0xED);
    {
        uint8_t data[] = { 0x04, 0x03, 0x12, 0x81 };
        ILI9341_WriteDataMultiple(data, 4);
    }

    // PUMP RATIO CONTROL
    ILI9341_WriteCommand(0xF7);
    {
        uint8_t data[] = { 0x20 };
        ILI9341_WriteDataMultiple(data, 1);
    }

    // POWER CONTROL,VRH[5:0]
    ILI9341_WriteCommand(0xC0);
    {
        uint8_t data[] = { 0x23 };
        ILI9341_WriteDataMultiple(data, 1);
    }

    // POWER CONTROL,SAP[2:0];BT[3:0]
    ILI9341_WriteCommand(0xC1);
    {
        uint8_t data[] = { 0x10 };
        ILI9341_WriteDataMultiple(data, 1);
    }

    // VCM CONTROL
    ILI9341_WriteCommand(0xC5);
    {
        uint8_t data[] = { 0x3E, 0x28 };
        ILI9341_WriteDataMultiple(data, 2);
    }

    // VCM CONTROL 2
    ILI9341_WriteCommand(0xC7);
    {
        uint8_t data[] = { 0x86 };
        ILI9341_WriteDataMultiple(data, 1);
    }

    // MEMORY ACCESS CONTROL
    ILI9341_WriteCommand(0x36);
    {
        uint8_t data[] = { 0x48 };
        ILI9341_WriteDataMultiple(data, 1);
    }

    // PIXEL FORMAT
    ILI9341_WriteCommand(0x3A);
    {
        uint8_t data[] = { 0x55 };
        ILI9341_WriteDataMultiple(data, 1);
    }

    // FRAME RATIO CONTROL, STANDARD RGB COLOR
    ILI9341_WriteCommand(0xB1);
    {
        uint8_t data[] = { 0x00, 0x18 };
        ILI9341_WriteDataMultiple(data, 2);
    }

    // DISPLAY FUNCTION CONTROL
    ILI9341_WriteCommand(0xB6);
    {
        uint8_t data[] = { 0x08, 0x82, 0x27 };
        ILI9341_WriteDataMultiple(data, 3);
    }

    // 3GAMMA FUNCTION DISABLE
    ILI9341_WriteCommand(0xF2);
    {
        uint8_t data[] = { 0x00 };
        ILI9341_WriteDataMultiple(data, 1);
    }

    // GAMMA CURVE SELECTED
    ILI9341_WriteCommand(0x26);
    {
        uint8_t data[] = { 0x01 };
        ILI9341_WriteDataMultiple(data, 1);
    }

    // POSITIVE GAMMA CORRECTION
    ILI9341_WriteCommand(0xE0);
    {
        uint8_t data[] = { 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
                           0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 };
        ILI9341_WriteDataMultiple(data, 15);
    }

    // NEGATIVE GAMMA CORRECTION
    ILI9341_WriteCommand(0xE1);
    {
        uint8_t data[] = { 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
                           0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F };
        ILI9341_WriteDataMultiple(data, 15);
    }

    // EXIT SLEEP
    ILI9341_WriteCommand(0x11);
    HAL_Delay(120);

    // TURN ON DISPLAY
    ILI9341_WriteCommand(0x29);

}

void ILI9341_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // column address set
    ILI9341_WriteCommand(0x2A); // CASET
    {
        ILI9341_WriteData(x0 >> 8);
        ILI9341_WriteData(x0 & 0x00FF);
        ILI9341_WriteData(x1 >> 8);
        ILI9341_WriteData(x1 & 0x00FF);
    }
    // row address set
    ILI9341_WriteCommand(0x2B); // RASET
    {
        ILI9341_WriteData(y0 >> 8);
        ILI9341_WriteData(y0 & 0x00FF);
        ILI9341_WriteData(y1 >> 8);
        ILI9341_WriteData(y1 & 0x00FF);
    }
    // write to RAM
    ILI9341_WriteCommand(0x2C); // RAMWR
}

void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if ((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
    ILI9341_SetAddressWindow(x, y, x, y);
    uint16_t pixel = color;
    ILI9341_WriteColorMultiple(&pixel, 1);
}

void ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // clipping
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
    if((x + w - 1) >= ILI9341_WIDTH) w = ILI9341_WIDTH - x;
    if((y + h - 1) >= ILI9341_HEIGHT) h = ILI9341_HEIGHT - y;

    ILI9341_SetAddressWindow(x, y, x+w-1, y+h-1);
    ILI9341_WriteColorRepeat(color, (uint32_t)w * h);
}

void ILI9341_FillScreen(uint16_t color) {
	ILI9341_FillRectangle(0, 0, ILI9341_WIDTH, ILI9341_HEIGHT, color);
}

void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
    if((x + w - 1) >= ILI9341_WIDTH) return;
    if((y + h - 1) >= ILI9341_HEIGHT) return;

    ILI9341_SetAddressWindow(x, y, x+w-1, y+h-1);
    ILI9341_WriteColorMultiple(data, w*h);
}
void ILI9341_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	uint16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	uint16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	uint16_t err = dx + dy, e2;

    while (1) {
        ILI9341_DrawPixel(x0, y0, color);  // Draw the current pixel
        if (x0 == x1 && y0 == y1) break;  // Stop if the end point is reached
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}
