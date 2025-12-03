#ifndef CAMCONTROL_H
#define CAMCONTROL_H

#include "stm32l1xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

void CamControl_Init(UART_HandleTypeDef *huart_tau, UART_HandleTypeDef *huart_dbg);
void CamControl_Task(void);

// (for clarity, also expose these helpers)
void Tau_SendSetFfcAuto(void);
void Tau_SendGetFfcMode(void);
void Tau_ReadAndPrintReply(void);
void dbg_printf(const char *fmt, ...);

#endif
