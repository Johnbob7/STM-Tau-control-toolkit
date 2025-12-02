/*
 * rs232_driver.h
 *
 * Created on: Jul 12, 2024
 * Author: Arda
 */

#ifndef RS232_DRIVER_H_
#define RS232_DRIVER_H_

#include "stm32l1xx_hal.h"

/**
 * @brief Initialize the RS232 communication via UART.
 *
 * This function initializes the UART peripheral for RS232 communication.
 * @param huart Pointer to the UART handle.
 */
void RS232_Init(UART_HandleTypeDef *huart);

/**
 * @brief Transmit data over RS232.
 *
 * This function transmits data over the UART interface.
 * @param huart Pointer to the UART handle.
 * @param pData Pointer to the data buffer to be transmitted.
 * @param Size Size of the data to be transmitted.
 * @return HAL status indicating success or failure.
 */
HAL_StatusTypeDef RS232_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

/**
 * @brief Receive data over RS232.
 *
 * This function receives data over the UART interface.
 * @param huart Pointer to the UART handle.
 * @param pData Pointer to the buffer where received data will be stored.
 * @param Size Size of the data to be received.
 * @return HAL status indicating success or failure.
 */
HAL_StatusTypeDef RS232_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

#endif /* RS232_DRIVER_H_ */
