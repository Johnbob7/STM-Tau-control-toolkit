#include "ili9341_gfx.h"
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
    // Hardware reset
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(120);

    // Minimal init sequence (expand with full ILI9341 init if needed)
    ILI9341_WriteCommand(0x01); // Software reset
    HAL_Delay(5);
    ILI9341_WriteCommand(0x28); // Display OFF
    ILI9341_WriteCommand(0x29); // Display ON
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
    ILI9341_SetAddrWindow(0, 0, ILI9341_WIDTH, ILI9341_HEIGHT);
    for (uint32_t i = 0; i < ILI9341_WIDTH * ILI9341_HEIGHT; i++) {
        ILI9341_WriteData(color >> 8);
        ILI9341_WriteData(color & 0xFF);
    }
}

void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
    if((x + w - 1) >= ILI9341_WIDTH) return;
    if((y + h - 1) >= ILI9341_HEIGHT) return;

    ILI9341_SetAddrWindow(x, y, x+w-1, y+h-1);
    ILI9341_WriteDataMultiple((uint16_t*)data, w*h);
}
