#include "main.h"
#include "adc_keypad.h"

#include <stddef.h>

// Use USART2 (STLink VCP)
extern UART_HandleTypeDef huart2;

void ADC_ButtonInit(void) {
    // No ADC needed for UART-driven commands; nothing to init here.
}

static int8_t hex_value(char c) {
    if (c >= '0' && c <= '9') return (int8_t)(c - '0');
    if (c >= 'a' && c <= 'f') return (int8_t)(10 + c - 'a');
    if (c >= 'A' && c <= 'F') return (int8_t)(10 + c - 'A');
    return -1;
}

char GetButton(void) {
    char line[64];
    size_t len = 0;

    // Read an entire command line (terminated by LF) from PuTTY
    while (1) {
        uint8_t b;
        if (HAL_UART_Receive(&huart2, &b, 1, HAL_MAX_DELAY) != HAL_OK) return 0;

        char c = (char)b;
        if (c == '\r') continue; // Ignore carriage return
        if (c == '\n') {
            if (len == 0) continue; // Skip empty lines
            break;
        }

        if (len < sizeof(line) - 1) {
            line[len++] = c;
        }
    }

    // Trim whitespace at both ends
    size_t start = 0;
    while (start < len && (line[start] == ' ' || line[start] == '\t')) start++;
    size_t end = len;
    while (end > start && (line[end - 1] == ' ' || line[end - 1] == '\t')) end--;

    if (end - start != 2) return 0; // Expect exactly two hex digits

    int8_t hi = hex_value(line[start]);
    int8_t lo = hex_value(line[start + 1]);
    if (hi < 0 || lo < 0) return 0; // Invalid hex character(s)

    return (char)((hi << 4) | lo);
}
