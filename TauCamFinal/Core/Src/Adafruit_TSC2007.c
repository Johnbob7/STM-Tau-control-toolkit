/*!
 *  @file Adafruit_TSC2007.cpp
 *
 *  @mainpage Adafruit TSC2007 Resistive Touch Panel library
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for the Adafruit TSC2007 Resistive Touch Panel Sensor library
 * for Arduino
 *
 * 	This is a library for the Adafruit TSC2007 breakout:
 * 	https://www.adafruit.com/
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section dependencies Dependencies
 *  This library depends on the Adafruit BusIO library
 *
 *  @section author Author
 *
 *  Limor Fried (Adafruit Industries)
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "Adafruit_TSC2007.h"

/*!
 *    @brief  Instantiates a new TSC2007 class
 */

/*!
 *    @brief  Sets up the hardware and initializes I2C
 *    @param  address
 *            The I2C address to use, defaults to 0x48
 *    @param  wire
 *            The Wire object to be used for I2C connections.
 *    @return True if initialization was successful, otherwise false.
 */
bool TSC2007_Init(TSC2007_Handle *handle, I2C_HandleTypeDef *hi2c,
                  uint16_t screen_width, uint16_t screen_height) {
    handle->hi2c = hi2c;
    handle->screen_width = screen_width;
    handle->screen_height = screen_height;


  /*
  uint8_t setupcmd = 0b10110000;
  if (!i2c_dev->write(&setupcmd, 1)) {
    return false;
  }
  */

  TSC2007_Command(handle, MEASURE_Z1, ADON_IRQOFF, ADC_12BIT);
  return true;
}

/*!
 *    @brief  Send a command and read 2 bytes from TSC
 *    @param  func The command function to make
 *    @param  pwr  The power mode to enter after command
 *    @param  res  The ADC resolution
 *    @return 12 bits of data shifted from the 16-bit read value
 */
uint16_t TSC2007_Command(TSC2007_Handle *handle, adafruit_tsc2007_function func,
		adafruit_tsc2007_power pwr, adafruit_tsc2007_resolution res) {
    // Build command byte
    uint8_t cmd = (uint8_t)func << 4;
    cmd |= (uint8_t)pwr << 2;
    cmd |= (uint8_t)res << 1;

    uint8_t reply[2];

    // Write command
    if (HAL_I2C_Master_Transmit(handle->hi2c, TSC2007_I2C_ADDR, &cmd, 1, 100) != HAL_OK) {
        return 0;
    }

    // Wait for conversion (500 microseconds)
    HAL_Delay(1);  // Minimum 1ms delay

    // Read response
    if (HAL_I2C_Master_Receive(handle->hi2c, TSC2007_I2C_ADDR, reply, 2, 100) != HAL_OK) {
        return 0;
    }

    // Return 12-bit value
    return ((uint16_t)reply[0] << 4) | (reply[1] >> 4);
}

/*!
 *    @brief  Read touch data from the TSC and then power down
 *    @param  x Pointer to 16-bit value we will store x reading
 *    @param  y Pointer to 16-bit value we will store y reading
 *    @param  z1 Pointer to 16-bit value we will store z1 pressure reading
 *    @param  z2 Pointer to 16-bit value we will store z2 pressure reading
 *    @return True if ADC was able to read the x & y values
 */
bool TSC2007_ReadTouch(TSC2007_Handle *handle,uint16_t *x,uint16_t *y, uint16_t *z1, uint16_t *z2) {
    // Avoid division by zero
	    *x  = TSC2007_Command(handle, MEASURE_X,  ADON_IRQOFF, ADC_12BIT);
	    *y  = TSC2007_Command(handle, MEASURE_Y,  ADON_IRQOFF, ADC_12BIT);
	    *z1 = TSC2007_Command(handle, MEASURE_Z1, ADON_IRQOFF, ADC_12BIT);
	    *z2 = TSC2007_Command(handle, MEASURE_Z2, POWERDOWN_IRQON, ADC_12BIT);

	    // Basic validity check
	        if (*z1 == 0)
	            return true;

	        return true;
    }

/*!
 *  @brief  Function to get a point object rather than passing in pointers
 *  @returns A TS_Point, all values will be 0 if touchscreen read failed
 */
TS_Point TSC2007_GetPoint(TSC2007_Handle *handle) {
    uint16_t x, y, z1, z2;
    TS_Point point = {0, 0, 0};

    if (TSC2007_ReadTouch(handle, &x, &y, &z1, &z2)) {
        point.x = x;
        point.y = y;
        point.z = z1;  // Use z1 as pressure indicator
    }

    return point;
}

//check if being touched
bool TSC2007_IsTouched(TSC2007_Handle *handle) {
    uint16_t x, y, z1, z2;
    return TSC2007_ReadTouch(handle, &x, &y, &z1, &z2);
}
void TSC2007_MapToPixel(TSC2007_Handle *handle, uint16_t adc_x, uint16_t adc_y,
                        uint16_t *pixel_x, uint16_t *pixel_y) {
    // Linear mapping: pixel = (adc * screen_dimension) / 4095
    *pixel_x = ((uint32_t)adc_x * handle->screen_width) / 4095;
    *pixel_y = ((uint32_t)adc_y * handle->screen_height) / 4095;

    // Clamp to screen bounds
    if (*pixel_x >= handle->screen_width) {
        *pixel_x = handle->screen_width - 1;
    }
    if (*pixel_y >= handle->screen_height) {
        *pixel_y = handle->screen_height - 1;
    }
}

/*!
 *    @brief  Get current touch position in pixel coordinates (non-blocking)
 *    @param  handle Pointer to TSC2007_Handle
 *    @param  pixel_x Pointer to store pixel X coordinate (0 to screen_width-1)
 *    @param  pixel_y Pointer to store pixel Y coordinate (0 to screen_height-1)
 *    @return True if touch detected, false if no touch
 */
bool TSC2007_GetTouchPixel(TSC2007_Handle *handle, uint16_t *pixel_x, uint16_t *pixel_y) {
    uint16_t adc_x, adc_y, z1, z2;

    // Read touch data
    if (TSC2007_ReadTouch(handle, &adc_x, &adc_y, &z1, &z2)) {
        // Touch detected - map to pixel coordinates
        TSC2007_MapToPixel(handle, adc_x, adc_y, pixel_x, pixel_y);
        return true;
    }

    return false;  // No touch detected
}

/*!
 *    @brief  Wait for touch and return pixel coordinates (blocking)
 *    @param  handle Pointer to TSC2007_Handle
 *    @param  pixel_x Pointer to store pixel X coordinate
 *    @param  pixel_y Pointer to store pixel Y coordinate
 *    @param  timeout_ms Maximum time to wait in milliseconds (0 = wait forever)
 *    @return True if touch detected within timeout, false if timeout
 */
bool TSC2007_WaitForTouch(TSC2007_Handle *handle, uint16_t *pixel_x, uint16_t *pixel_y, uint32_t timeout_ms) {
    uint32_t start_time = HAL_GetTick();

    while (1) {
        // Check for touch
        if (TSC2007_GetTouchPixel(handle, pixel_x, pixel_y)) {
            return true;  // Touch detected!
        }

        // Check timeout (if enabled)
        if (timeout_ms > 0) {
            if ((HAL_GetTick() - start_time) >= timeout_ms) {
                return false;  // Timeout
            }
        }

        HAL_Delay(10);  // Poll every 10ms to avoid hammering the I2C bus
    }
}

