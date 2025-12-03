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
void Tau_GetVideoLUT(void);
void Tau_SetLUT_WhiteHot(void);
void Tau_SetLUT_BlackHot(void);
void Tau_SetLUT_Fusion(void);
void Tau_SetLUT_Rainbow(void);
void Tau_SetLUT_Globow(void);
void Tau_SetLUT_Ironbow1(void);
void Tau_SetLUT_Ironbow2(void);
void Tau_SetLUT_Sepia(void);
void Tau_SetLUT_Color1(void);
void Tau_SetLUT_Color2(void);
void Tau_SetLUT_IceFire(void);
void Tau_SetLUT_Rain(void);
void Tau_SetLUT_Custom1(void);
const char *Tau_LUT_NameFromId(uint16_t lut);
void Tau_PrintHelp(void);
void dbg_printf(const char *fmt, ...);

#endif
