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

// =============================
// VIDEO_LUT (Function 0x10)
// =============================

// Get VIDEO_LUT
// Get Cmd: byte count = 0
static const uint8_t TAU_CMD_GET_VIDEO_LUT[] = {
    0x6E, 0x00, 0x00, 0x10, 0x00, 0x00, 0x9C, 0xD8, 0x00, 0x00
    //           ^func  ^BC=0   ^^^^^ CRC1 ^^^^^  ^^^^ CRC2 ^^^^
};

// ---- Set VIDEO_LUT = White Hot (0x0000)
static const uint8_t TAU_CMD_SET_LUT_WHITE_HOT[] = {
    0x6E, 0x00, 0x00, 0x10, 0x00, 0x02, 0xBC, 0x9A, 0x00, 0x00, 0x00, 0x00
    //                       ^BC=2   ^^^^^ CRC1 ^^^^^  ^data^  ^^^^ CRC2 ^^^^
};

// ---- Set VIDEO_LUT = Black Hot (0x0001)
static const uint8_t TAU_CMD_SET_LUT_BLACK_HOT[] = {
    0x6E, 0x00, 0x00, 0x10, 0x00, 0x02, 0xBC, 0x9A, 0x00, 0x01, 0x10, 0x21
};

// ---- Set VIDEO_LUT = Fusion (0x0002)
static const uint8_t TAU_CMD_SET_LUT_FUSION[] = {
    0x6E, 0x00, 0x00, 0x10, 0x00, 0x02, 0xBC, 0x9A, 0x00, 0x02, 0x20, 0x42
};

// ---- Set VIDEO_LUT = Rainbow (0x0003)
static const uint8_t TAU_CMD_SET_LUT_RAINBOW[] = {
    0x6E, 0x00, 0x00, 0x10, 0x00, 0x02, 0xBC, 0x9A, 0x00, 0x03, 0x30, 0x63
};

// ---- Set VIDEO_LUT = Globow (0x0004)
static const uint8_t TAU_CMD_SET_LUT_GLOBOW[] = {
    0x6E, 0x00, 0x00, 0x10, 0x00, 0x02, 0xBC, 0x9A, 0x00, 0x04, 0x40, 0x84
};

// ---- Set VIDEO_LUT = Ironbow1 (0x0005)
static const uint8_t TAU_CMD_SET_LUT_IRONBOW1[] = {
    0x6E, 0x00, 0x00, 0x10, 0x00, 0x02, 0xBC, 0x9A, 0x00, 0x05, 0x50, 0xA5
};

// ---- Set VIDEO_LUT = Ironbow2 (0x0006)
static const uint8_t TAU_CMD_SET_LUT_IRONBOW2[] = {
    0x6E, 0x00, 0x00, 0x10, 0x00, 0x02, 0xBC, 0x9A, 0x00, 0x06, 0x60, 0xC6
};

// ---- Set VIDEO_LUT = Sepia (0x0007)
static const uint8_t TAU_CMD_SET_LUT_SEPIA[] = {
    0x6E, 0x00, 0x00, 0x10, 0x00, 0x02, 0xBC, 0x9A, 0x00, 0x07, 0x70, 0xE7
};

// ---- Set VIDEO_LUT = Color1 (0x0008)
static const uint8_t TAU_CMD_SET_LUT_COLOR1[] = {
    0x6E, 0x00, 0x00, 0x10, 0x00, 0x02, 0xBC, 0x9A, 0x00, 0x08, 0x81, 0x08
};

// ---- Set VIDEO_LUT = Color2 (0x0009)
static const uint8_t TAU_CMD_SET_LUT_COLOR2[] = {
    0x6E, 0x00, 0x00, 0x10, 0x00, 0x02, 0xBC, 0x9A, 0x00, 0x09, 0x91, 0x29
};

// ---- Set VIDEO_LUT = Ice & Fire (0x000A)
static const uint8_t TAU_CMD_SET_LUT_ICE_FIRE[] = {
    0x6E, 0x00, 0x00, 0x10, 0x00, 0x02, 0xBC, 0x9A, 0x00, 0x0A, 0xA1, 0x4A
};

// ---- Set VIDEO_LUT = Rain (0x000B)
static const uint8_t TAU_CMD_SET_LUT_RAIN[] = {
    0x6E, 0x00, 0x00, 0x10, 0x00, 0x02, 0xBC, 0x9A, 0x00, 0x0B, 0xB1, 0x6B
};

// ---- Set VIDEO_LUT = Custom #1 (0x000C)
static const uint8_t TAU_CMD_SET_LUT_CUSTOM1[] = {
    0x6E, 0x00, 0x00, 0x10, 0x00, 0x02, 0xBC, 0x9A, 0x00, 0x0C, 0xC1, 0x8C
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
