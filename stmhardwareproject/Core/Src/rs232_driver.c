/*
 * rs232_driver.c
 *
 * Created on: Jul 12, 2024
 * Author: Arda
 */

#include "rs232_driver.h"

/**
 * @brief Initialize the RS232 communication via UART.
 *
 * This function initializes the UART peripheral for RS232 communication.
 * @param huart Pointer to the UART handle.
 */
void RS232_Init(UART_HandleTypeDef *huart)
{
    huart->Instance = USART1;  // Change USART1 to the correct USART instance
    huart->Init.BaudRate = 9600;  // Set the desired baud rate
    huart->Init.WordLength = UART_WORDLENGTH_8B;
    huart->Init.StopBits = UART_STOPBITS_1;
    huart->Init.Parity = UART_PARITY_NONE;
    huart->Init.Mode = UART_MODE_TX_RX;
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart->Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(huart) != HAL_OK)
    {
        // Initialization Error
        Error_Handler();
    }
}

/**
 * @brief Transmit data over RS232.
 *
 * This function transmits data over the UART interface.
 * @param huart Pointer to the UART handle.
 * @param pData Pointer to the data buffer to be transmitted.
 * @param Size Size of the data to be transmitted.
 * @return HAL status indicating success or failure.
 */
HAL_StatusTypeDef RS232_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    return HAL_UART_Transmit(huart, pData, Size, HAL_MAX_DELAY);
}

/**
 * @brief Receive data over RS232.
 *
 * This function receives data over the UART interface.
 * @param huart Pointer to the UART handle.
 * @param pData Pointer to the buffer where received data will be stored.
 * @param Size Size of the data to be received.
 * @return HAL status indicating success or failure.
 */
HAL_StatusTypeDef RS232_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    return HAL_UART_Receive(huart, pData, Size, HAL_MAX_DELAY);
}
