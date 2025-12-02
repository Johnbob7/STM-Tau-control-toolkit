#include "ILI9341_hal.h"
#include "main.h"
#include <stdlib.h>

#define CS_LOW()  HAL_GPIO_WritePin(handle->cs_port, handle->cs_pin, GPIO_PIN_RESET)
#define CS_HIGH() HAL_GPIO_WritePin(handle->cs_port, handle->cs_pin, GPIO_PIN_SET)
#define DC_LOW()  HAL_GPIO_WritePin(handle->dc_port, handle->dc_pin, GPIO_PIN_RESET)
#define DC_HIGH() HAL_GPIO_WritePin(handle->dc_port, handle->dc_pin, GPIO_PIN_SET)
#define RST_LOW() HAL_GPIO_WritePin(handle->rst_port, handle->rst_pin, GPIO_PIN_RESET)
#define RST_HIGH() HAL_GPIO_WritePin(handle->rst_port, handle->rst_pin, GPIO_PIN_SET)

static void writeCommand(ILI9341_Handle *handle, uint8_t cmd) {
    DC_LOW();
    HAL_SPI_Transmit(handle->hspi, &cmd, 1, HAL_MAX_DELAY);
}

static void writeData(ILI9341_Handle *handle, uint8_t data) {
    DC_HIGH();
    HAL_SPI_Transmit(handle->hspi, &data, 1, HAL_MAX_DELAY);
}

static void writeData16(ILI9341_Handle *handle, uint16_t data) {
    DC_HIGH();
    uint8_t buf[2];
    buf[0] = data >> 8;
    buf[1] = data & 0xFF;
    HAL_SPI_Transmit(handle->hspi, buf, 2, HAL_MAX_DELAY);
}

static void hardwareReset(ILI9341_Handle *handle) {
    if (handle->rst_port != NULL) {
        RST_HIGH();
        HAL_Delay(10);
        RST_LOW();
        HAL_Delay(10);
        RST_HIGH();
        HAL_Delay(120);
    }
}

void ILI9341_Init(ILI9341_Handle *handle, SPI_HandleTypeDef *hspi,
                  GPIO_TypeDef *cs_port, uint16_t cs_pin,
                  GPIO_TypeDef *dc_port, uint16_t dc_pin,
                  GPIO_TypeDef *rst_port, uint16_t rst_pin) {
    handle->hspi = hspi;
    handle->cs_port = cs_port;
    handle->cs_pin = cs_pin;
    handle->dc_port = dc_port;
    handle->dc_pin = dc_pin;
    handle->rst_port = rst_port;
    handle->rst_pin = rst_pin;
    handle->width = ILI9341_TFTWIDTH;
    handle->height = ILI9341_TFTHEIGHT;

    CS_HIGH();
    HAL_Delay(10);
    hardwareReset(handle);
    CS_LOW();

    writeCommand(handle, 0x01);
    HAL_Delay(150);

    writeCommand(handle, 0xCB);
    writeData(handle, 0x39);
    writeData(handle, 0x2C);
    writeData(handle, 0x00);
    writeData(handle, 0x34);
    writeData(handle, 0x02);

    writeCommand(handle, 0xCF);
    writeData(handle, 0x00);
    writeData(handle, 0xC1);
    writeData(handle, 0x30);

    writeCommand(handle, 0xE8);
    writeData(handle, 0x85);
    writeData(handle, 0x00);
    writeData(handle, 0x78);

    writeCommand(handle, 0xEA);
    writeData(handle, 0x00);
    writeData(handle, 0x00);

    writeCommand(handle, 0xED);
    writeData(handle, 0x64);
    writeData(handle, 0x03);
    writeData(handle, 0x12);
    writeData(handle, 0x81);

    writeCommand(handle, 0xF7);
    writeData(handle, 0x20);

    writeCommand(handle, 0xC0);
    writeData(handle, 0x23);

    writeCommand(handle, 0xC1);
    writeData(handle, 0x10);

    writeCommand(handle, 0xC5);
    writeData(handle, 0x3e);
    writeData(handle, 0x28);

    writeCommand(handle, 0xC7);
    writeData(handle, 0x86);

    writeCommand(handle, 0x36);
    writeData(handle, 0x48);

    writeCommand(handle, 0x3A);
    writeData(handle, 0x55);

    writeCommand(handle, 0xB1);
    writeData(handle, 0x00);
    writeData(handle, 0x18);

    writeCommand(handle, 0xB6);
    writeData(handle, 0x08);
    writeData(handle, 0x82);
    writeData(handle, 0x27);

    writeCommand(handle, 0xF2);
    writeData(handle, 0x00);

    writeCommand(handle, 0x26);
    writeData(handle, 0x01);

    writeCommand(handle, 0xE0);
    writeData(handle, 0x0F);
    writeData(handle, 0x31);
    writeData(handle, 0x2B);
    writeData(handle, 0x0C);
    writeData(handle, 0x0E);
    writeData(handle, 0x08);
    writeData(handle, 0x4E);
    writeData(handle, 0xF1);
    writeData(handle, 0x37);
    writeData(handle, 0x07);
    writeData(handle, 0x10);
    writeData(handle, 0x03);
    writeData(handle, 0x0E);
    writeData(handle, 0x09);
    writeData(handle, 0x00);

    writeCommand(handle, 0xE1);
    writeData(handle, 0x00);
    writeData(handle, 0x0E);
    writeData(handle, 0x14);
    writeData(handle, 0x03);
    writeData(handle, 0x11);
    writeData(handle, 0x07);
    writeData(handle, 0x31);
    writeData(handle, 0xC1);
    writeData(handle, 0x48);
    writeData(handle, 0x08);
    writeData(handle, 0x0F);
    writeData(handle, 0x0C);
    writeData(handle, 0x31);
    writeData(handle, 0x36);
    writeData(handle, 0x0F);

    writeCommand(handle, 0x11);
    HAL_Delay(120);

    writeCommand(handle, 0x29);
    HAL_Delay(20);

    CS_HIGH();
}

void ILI9341_StartWrite(ILI9341_Handle *handle) {
    CS_LOW();
}

void ILI9341_EndWrite(ILI9341_Handle *handle) {
    CS_HIGH();
}

void ILI9341_SetAddrWindow(ILI9341_Handle *handle, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    uint16_t x_end = x + w - 1;
    uint16_t y_end = y + h - 1;

    writeCommand(handle, ILI9341_CASET);
    writeData16(handle, x);
    writeData16(handle, x_end);

    writeCommand(handle, ILI9341_PASET);
    writeData16(handle, y);
    writeData16(handle, y_end);

    writeCommand(handle, ILI9341_RAMWR);
}

void ILI9341_WritePixel(ILI9341_Handle *handle, uint16_t color) {
    writeData16(handle, color);
}

void ILI9341_DrawPixel(ILI9341_Handle *handle, uint16_t x, uint16_t y, uint16_t color) {
    if (x >= handle->width || y >= handle->height) return;

    ILI9341_StartWrite(handle);
    ILI9341_SetAddrWindow(handle, x, y, 1, 1);
    ILI9341_WritePixel(handle, color);
    ILI9341_EndWrite(handle);
}

void ILI9341_FillScreen(ILI9341_Handle *handle, uint16_t color) {
    ILI9341_StartWrite(handle);
    ILI9341_SetAddrWindow(handle, 0, 0, handle->width, handle->height);

    DC_HIGH();
    uint8_t colorBuf[2];
    colorBuf[0] = color >> 8;
    colorBuf[1] = color & 0xFF;

    uint32_t totalPixels = (uint32_t)handle->width * handle->height;
    for (uint32_t i = 0; i < totalPixels; i++) {
        HAL_SPI_Transmit(handle->hspi, colorBuf, 2, HAL_MAX_DELAY);
    }

    ILI9341_EndWrite(handle);
}

void ILI9341_FillScreenFast(ILI9341_Handle *handle, uint16_t color) {
    ILI9341_StartWrite(handle);
    ILI9341_SetAddrWindow(handle, 0, 0, handle->width, handle->height);

    DC_HIGH();
    uint8_t colorBuf[64];
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;

    for (int i = 0; i < 64; i += 2) {
        colorBuf[i] = hi;
        colorBuf[i + 1] = lo;
    }

    uint32_t totalPixels = (uint32_t)handle->width * handle->height;
    uint32_t totalBytes = totalPixels * 2;
    while (totalBytes > 0) {
        uint16_t chunk = (totalBytes > sizeof(colorBuf)) ? sizeof(colorBuf) : totalBytes;
        HAL_SPI_Transmit(handle->hspi, colorBuf, chunk, HAL_MAX_DELAY);
        totalBytes -= chunk;
    }

    ILI9341_EndWrite(handle);
}

void ILI9341_FillRect(ILI9341_Handle *handle, uint16_t x, uint16_t y,
                      uint16_t w, uint16_t h, uint16_t color) {
    if (x >= handle->width || y >= handle->height) return;
    if (x + w > handle->width) w = handle->width - x;
    if (y + h > handle->height) h = handle->height - y;

    ILI9341_StartWrite(handle);
    ILI9341_SetAddrWindow(handle, x, y, w, h);

    DC_HIGH();
    uint8_t colorBuf[2];
    colorBuf[0] = color >> 8;
    colorBuf[1] = color & 0xFF;

    uint32_t totalPixels = (uint32_t)w * h;
    for (uint32_t i = 0; i < totalPixels; i++) {
        HAL_SPI_Transmit(handle->hspi, colorBuf, 2, HAL_MAX_DELAY);
    }

    ILI9341_EndWrite(handle);
}

void ILI9341_DrawRect(ILI9341_Handle *handle, uint16_t x, uint16_t y,
                      uint16_t w, uint16_t h, uint16_t color) {
    ILI9341_DrawHLine(handle, x, y, w, color);
    ILI9341_DrawHLine(handle, x, y + h - 1, w, color);
    ILI9341_DrawVLine(handle, x, y, h, color);
    ILI9341_DrawVLine(handle, x + w - 1, y, h, color);
}

void ILI9341_DrawHLine(ILI9341_Handle *handle, uint16_t x, uint16_t y,
                       uint16_t w, uint16_t color) {
    if (x >= handle->width || y >= handle->height) return;
    if (x + w > handle->width) w = handle->width - x;

    ILI9341_StartWrite(handle);
    ILI9341_SetAddrWindow(handle, x, y, w, 1);

    DC_HIGH();
    uint8_t colorBuf[2];
    colorBuf[0] = color >> 8;
    colorBuf[1] = color & 0xFF;

    for (uint16_t i = 0; i < w; i++) {
        HAL_SPI_Transmit(handle->hspi, colorBuf, 2, HAL_MAX_DELAY);
    }

    ILI9341_EndWrite(handle);
}

void ILI9341_DrawVLine(ILI9341_Handle *handle, uint16_t x, uint16_t y,
                       uint16_t h, uint16_t color) {
    if (x >= handle->width || y >= handle->height) return;
    if (y + h > handle->height) h = handle->height - y;

    ILI9341_StartWrite(handle);
    ILI9341_SetAddrWindow(handle, x, y, 1, h);

    DC_HIGH();
    uint8_t colorBuf[2];
    colorBuf[0] = color >> 8;
    colorBuf[1] = color & 0xFF;

    for (uint16_t i = 0; i < h; i++) {
        HAL_SPI_Transmit(handle->hspi, colorBuf, 2, HAL_MAX_DELAY);
    }

    ILI9341_EndWrite(handle);
}

void ILI9341_DrawLine(ILI9341_Handle *handle, uint16_t x0, uint16_t y0,
                      uint16_t x1, uint16_t y1, uint16_t color) {
    int16_t dx = abs((int16_t)x1 - (int16_t)x0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t dy = -abs((int16_t)y1 - (int16_t)y0);
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx + dy;

    while (1) {
        ILI9341_DrawPixel(handle, x0, y0, color);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        int16_t e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void ILI9341_DrawImage(ILI9341_Handle *handle, uint16_t x, uint16_t y,
                       uint16_t w, uint16_t h, const uint16_t *data) {
    if (x >= handle->width || y >= handle->height) {
        return;
    }
    if (x + w > handle->width) {
        w = handle->width - x;
    }
    if (y + h > handle->height) {
        h = handle->height - y;
    }

    ILI9341_StartWrite(handle);
    ILI9341_SetAddrWindow(handle, x, y, w, h);

    uint32_t totalPixels = (uint32_t)w * h;
    for (uint32_t i = 0; i < totalPixels; i++) {
        writeData16(handle, data[i]);
    }

    ILI9341_EndWrite(handle);
}
