#ifndef TSC2007_HAL_H
#define TSC2007_HAL_H

#include "stm32l1xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

// TSC2007 I2C Address
#define TSC2007_I2C_ADDR  (0x48 << 1)  // 7-bit address shifted for HAL

// Touch pressure thresholds
#define TSC2007_MIN_PRESSURE    10   // Minimum pressure to register as touch
#define TSC2007_MAX_PRESSURE    5000  // Maximum valid pressure
// Command Functions
typedef enum {
    MEASURE_TEMP0 = 0x0,
    MEASURE_AUX   = 0x2,
    MEASURE_TEMP1 = 0x4,
    ACTIVATE_X    = 0x8,
    ACTIVATE_Y    = 0x9,
    ACTIVATE_YX   = 0xA,
    SETUP         = 0xB,
    MEASURE_X     = 0xC,
    MEASURE_Y     = 0xD,
    MEASURE_Z1    = 0xE,
    MEASURE_Z2    = 0xF
} TSC2007_Function;

// Power modes
typedef enum {
    POWERDOWN_IRQON  = 0x0,
    ADON_IRQOFF      = 0x1,
    ADOFF_IRQON      = 0x2,
    ADON_IRQON       = 0x3
} TSC2007_Power;

// ADC Resolution
typedef enum {
    ADC_12BIT = 0x0,
    ADC_8BIT  = 0x1
} TSC2007_Resolution;

// Touch point structure
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} TS_Point;

// TSC2007 Handle
typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint16_t screen_width;
    uint16_t screen_height;
} TSC2007_Handle;

// Function prototypes
bool TSC2007_Init(TSC2007_Handle *handle, I2C_HandleTypeDef *hi2c,
                  uint16_t screen_width, uint16_t screen_height);
uint16_t TSC2007_Command(TSC2007_Handle *handle, TSC2007_Function func,
                         TSC2007_Power pwr, TSC2007_Resolution res);
bool TSC2007_ReadTouch(TSC2007_Handle *handle, uint16_t *x, uint16_t *y,
                       uint16_t *z1, uint16_t *z2);
TS_Point TSC2007_GetPoint(TSC2007_Handle *handle);
bool TSC2007_IsTouched(TSC2007_Handle *handle);

// Mapping functions - convert ADC readings to pixel coordinates
void TSC2007_MapToPixel(TSC2007_Handle *handle, uint16_t adc_x, uint16_t adc_y,
                        uint16_t *pixel_x, uint16_t *pixel_y);
// Add to function prototypes section
bool TSC2007_WaitForTouch(TSC2007_Handle *handle, uint16_t *pixel_x, uint16_t *pixel_y, uint32_t timeout_ms);
bool TSC2007_GetTouchPixel(TSC2007_Handle *handle, uint16_t *pixel_x, uint16_t *pixel_y);

#endif
