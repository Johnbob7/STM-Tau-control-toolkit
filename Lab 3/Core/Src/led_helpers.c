#include "led_helpers.h"

/* Active-LOW LEDs: RESET = ON, SET = OFF */
void LED_AllOff(void) {
    // OFF = drive pins HIGH
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11 | GPIO_PIN_12, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10 | GPIO_PIN_11, GPIO_PIN_SET);
}

void LED_ShowProgress(uint32_t n) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, (n >= 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, (n >= 2) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, (n >= 3) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, (n >= 4) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}
