#include "main.h"
#include "adc_keypad.h"

// Use USART2 (STLink VCP)
extern UART_HandleTypeDef huart2;

void ADC_ButtonInit(void) {
    // No ADC needed for UART-driven lab; nothing to init here.
}

static char normalize(char c) {
    if (c >= 'a' && c <= 'z') c -= 32; // to upper
    return (c == 'U' || c == 'D' || c == 'L' || c == 'R' || c == 'S') ? c : 0;
}

char GetButton(void) {
    uint8_t b;
    // Blocks until a byte arrives from PuTTY
    if (HAL_UART_Receive(&huart2, &b, 1, HAL_MAX_DELAY) != HAL_OK) return 0;

    char c = (char)b;
    if (c == '\r' || c == '\n') return 0;     // ignore CR/LF
    c = normalize(c);
    return c;                                  // returns 'U','D','L','R','S' or 0
}
