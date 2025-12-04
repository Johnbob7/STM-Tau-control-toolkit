/*!
 *  @file Adafruit_TSC2007.h
 *
 * 	I2C Driver for the Adafruit TSC2007 Resistive Touch Panel Sensor library
 *for Arduino
 *
 * 	This is a library for the Adafruit TSC2007 breakout:
 * 	https://www.adafruit.com/
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *
 *	BSD license (see license.txt)
 */

#ifndef _ADAFRUIT_TSC2007_H
#define _ADAFRUIT_TSC2007_H

#include "stm32l1xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define TSC2007_I2C_ADDR (0x48<<1) ///< TSC2007 default i2c address

/*!
 *  @brief  Class for working with points
 */

/*!
 *    @brief  Different function commands
 */
typedef enum {
  MEASURE_TEMP0 = 0,
  MEASURE_AUX = 2,
  MEASURE_TEMP1 = 4,
  ACTIVATE_X = 8,
  ACTIVATE_Y = 9,
  ACTIVATE_YPLUS_X = 10,
  SETUP_COMMAND = 11,
  MEASURE_X = 12,
  MEASURE_Y = 13,
  MEASURE_Z1 = 14,
  MEASURE_Z2 = 15
} adafruit_tsc2007_function;

/*!
 *    @brief  Power and IRQ modes
 */
typedef enum {
  POWERDOWN_IRQON = 0,
  ADON_IRQOFF = 1,
  ADOFF_IRQON = 2,
} adafruit_tsc2007_power;

/*!
 *    @brief  ADC resolution
 */
typedef enum {
  ADC_12BIT = 0,
  ADC_8BIT = 1,
} adafruit_tsc2007_resolution;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} TS_Point;

typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint16_t screen_width;
    uint16_t screen_height;
} TSC2007_Handle;

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            the TSC2007 driver
 */
bool TSC2007_Init(TSC2007_Handle *handle, I2C_HandleTypeDef *hi2c,
                  uint16_t screen_width, uint16_t screen_height);
uint16_t TSC2007_Command(TSC2007_Handle *handle, adafruit_tsc2007_function func,
		adafruit_tsc2007_power pwr, adafruit_tsc2007_resolution res);
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
