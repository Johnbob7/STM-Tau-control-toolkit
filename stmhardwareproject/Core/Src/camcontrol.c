#include "camcontrol.h"

static UART_HandleTypeDef *s_huart_tau = NULL; // USART1 → Tau
static UART_HandleTypeDef *s_huart_dbg = NULL; // USART2 → PuTTY

// Set FFC mode = Auto (0x0001)
static const uint8_t TAU_CMD_SET_FFC_AUTO[] = {
    0x6E, 0x00, 0x00, 0x0B, 0x00, 0x02, 0x0F, 0x08, 0x00, 0x01, 0x10, 0x21
};

// Get FFC mode
static const uint8_t TAU_CMD_GET_FFC_MODE[] = {
    0x6E, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x2F, 0x4A, 0x00, 0x00
};

static void dbg_printf_internal(const char *fmt, va_list args)
{
    char buf[128];
    int len = vsnprintf(buf, sizeof(buf), fmt, args);
    if (len < 0) {
        return;
    }
    if (len > (int)sizeof(buf)) {
        len = sizeof(buf);
    }
    HAL_UART_Transmit(s_huart_dbg, (uint8_t *)buf, (uint16_t)len, 100);
}

void dbg_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    dbg_printf_internal(fmt, args);
    va_end(args);
}

void CamControl_Init(UART_HandleTypeDef *huart_tau, UART_HandleTypeDef *huart_dbg)
{
    s_huart_tau = huart_tau;
    s_huart_dbg = huart_dbg;
}

void Tau_SendSetFfcAuto(void)
{
    HAL_UART_Transmit(s_huart_tau,
                      (uint8_t *)TAU_CMD_SET_FFC_AUTO,
                      sizeof(TAU_CMD_SET_FFC_AUTO),
                      200);
}

void Tau_SendGetFfcMode(void)
{
    HAL_UART_Transmit(s_huart_tau,
                      (uint8_t *)TAU_CMD_GET_FFC_MODE,
                      sizeof(TAU_CMD_GET_FFC_MODE),
                      200);
}

void Tau_ReadAndPrintReply(void)
{
    uint8_t rx_buf[32];
    const uint16_t rx_expected = 12; // Example responses in Table B-5 are 12 bytes long

    if (HAL_UART_Receive(s_huart_tau, rx_buf, rx_expected, 200) != HAL_OK) {
        dbg_printf("Tau: no reply or UART error\r\n");
        return;
    }

    uint8_t status = rx_buf[1];

    dbg_printf("Tau reply (raw):");
    for (uint16_t i = 0; i < rx_expected; ++i) {
        dbg_printf(" %02X", rx_buf[i]);
    }
    dbg_printf("\r\n");

    dbg_printf("Tau status: 0x%02X\r\n", status);

    switch (status) {
    case 0x00: dbg_printf("Status meaning: CAM_OK\r\n"); break;
    case 0x01: dbg_printf("Status meaning: CAM_BUSY\r\n"); break;
    case 0x02: dbg_printf("Status meaning: CAM_NOT_READY\r\n"); break;
    case 0x03: dbg_printf("Status meaning: RANGE_ERROR\r\n"); break;
    case 0x04: dbg_printf("Status meaning: CHECKSUM_ERROR\r\n"); break;
    case 0x05: dbg_printf("Status meaning: PROCESS_ERROR\r\n"); break;
    case 0x06: dbg_printf("Status meaning: FUNCTION_ERROR\r\n"); break;
    case 0x07: dbg_printf("Status meaning: TIMEOUT\r\n"); break;
    case 0x09: dbg_printf("Status meaning: BYTE_COUNT_ERROR\r\n"); break;
    case 0x0A: dbg_printf("Status meaning: FEATURE_NOT_ENABLED\r\n"); break;
    default:   dbg_printf("Status meaning: unknown\r\n"); break;
    }
}

void CamControl_Task(void)
{
    uint8_t ch;

    if (HAL_UART_Receive(s_huart_dbg, &ch, 1, 0) != HAL_OK) {
        return;
    }

    switch (ch) {
    case 'a':
    case 'A':
        dbg_printf("\r\n[CMD] Set FFC mode = Auto\r\n");
        Tau_SendSetFfcAuto();
        Tau_ReadAndPrintReply();
        break;

    case 'g':
    case 'G':
        dbg_printf("\r\n[CMD] Get FFC mode\r\n");
        Tau_SendGetFfcMode();
        Tau_ReadAndPrintReply();
        break;

    default:
        dbg_printf("\r\nCommands: 'a' = set FFC Auto, 'g' = get FFC mode\r\n");
        break;
    }
}
