/*
 * EXAM Q1 — Control a 7-segment display
 * GOAL:   Show digits 0-9 on a single 7-segment display.
 * WIRING: Segments a..g on PA0..PA6 (common-cathode: HIGH = segment on).
 *         Common-anode display -> invert (segment on = LOW).
 * PATTERN: 1.clock  2.PA0..PA6 output  4.write ODR with the digit's bit pattern
 */
#define STM32F411xE
#include "stm32f4xx.h"

#define SEG_MASK 0x7F   // PA0..PA6

//                   gfedcba   (bit0=a ... bit6=g)
static const uint8_t DIGIT[10] = {
    0b0111111, // 0
    0b0000110, // 1
    0b1011011, // 2
    0b1001111, // 3
    0b1100110, // 4
    0b1101101, // 5
    0b1111101, // 6
    0b0000111, // 7
    0b1111111, // 8
    0b1101111, // 9
};

static void delay(volatile int n) { while (n--); }

int main(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;             // 1. clock

    for (int p = 0; p <= 6; p++) {                   // 2. PA0..PA6 = output
        GPIOA->MODER &= ~(0b11 << (p * 2));
        GPIOA->MODER |=  (0b01 << (p * 2));
    }

    while (1) {
        for (uint8_t d = 0; d < 10; d++) {           // 4. count 0..9
            GPIOA->ODR = DIGIT[d];                    // whole digit at once
            delay(1000000);
        }
    }
}
