/*
 * EXAM Q3 (alt) — Two buttons with edge detection (one press = one action)
 * GOAL:   Button A press -> TOGGLE the LED on/off.
 *         Button B press -> print the current ADC value ONCE.
 * WIRING: LED on PA5 (blue, D13). Button A on PB4 (D5, active-low).
 *         Button B on PB5 (active-low, pull-up). Sensor on PA4 (ADC ch4).
 *         UART: PA2 = Tx, PA3 = Rx (AF7), USART2.
 * TERMINAL: 115200 baud, 8-N-1.
 *
 * DIFFERENCE vs q3_button_led_uart.c: that one acts WHILE a button is held
 * (lab 1.3 style). This one acts only on the falling edge — the moment of a
 * new press — so one push = one toggle / one print.
 */
#define STM32F411xE
#include "stm32f4xx.h"
#include <stdio.h>

#define ADC_CH 4

// ---------- UART ----------
static void uart_putc(char c) {
    while ((USART2->SR & USART_SR_TXE) == 0);   // wait Tx buffer empty
    USART2->DR = c;
}
static void uart_print(const char *s) {
    while (*s) uart_putc(*s++);
}

// ---------- ADC ----------
static uint16_t adc_read(void) {
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while ((ADC1->SR & ADC_SR_EOC) == 0);
    return ADC1->DR;
}

int main(void) {
    // --- clocks ---
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;        // LED + UART pins + analog
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;        // buttons
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;       // USART2 (on APB1!)
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;         // ADC (on APB2!)

    // --- PA5 = output (LED) ---
    GPIOA->MODER &= ~(0b11 << (5 * 2));
    GPIOA->MODER |=  (0b01 << (5 * 2));

    // --- PA4 = analog ---
    GPIOA->MODER |= (0b11 << (ADC_CH * 2));

    // --- PB4, PB5 = input with pull-up (active-low buttons) ---
    for (int p = 4; p <= 5; p++) {
        GPIOB->MODER &= ~(0b11 << (p * 2));                 // input
        GPIOB->PUPDR &= ~(0b11 << (p * 2));
        GPIOB->PUPDR |=  (0b01 << (p * 2));                 // pull-up
    }

    // --- USART2: PA2 Tx, PA3 Rx, AF7 ---
    GPIOA->MODER |= (0b10 << (2 * 2)) | (0b10 << (3 * 2));  // alternate
    GPIOA->AFR[0] |= (7 << (2 * 4)) | (7 << (3 * 4));       // AF7
    USART2->BRR = 139;                                      // 115200 @ 16 MHz
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

    // --- ADC single conversion ---
    ADC1->CR2  |=  ADC_CR2_ADON;
    ADC1->SQR1 &= ~ADC_SQR1_L;
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;
    ADC1->SQR3 |=  (ADC_CH << ADC_SQR3_SQ1_Pos);

    int prevA = 1, prevB = 1;   // previous button states (idle HIGH)
    char buf[40];

    while (1) {
        int a = (GPIOB->IDR & (1 << 4)) ? 1 : 0;   // Button A
        int b = (GPIOB->IDR & (1 << 5)) ? 1 : 0;   // Button B

        if (prevA == 1 && a == 0)                  // A: falling edge = new press
            GPIOA->ODR ^= (1 << 5);                //    toggle LED

        if (prevB == 1 && b == 0) {                // B: falling edge = new press
            sprintf(buf, "ADC = %d\r\n", adc_read());
            uart_print(buf);
        }

        prevA = a;
        prevB = b;
        for (volatile int i = 0; i < 50000; i++);  // crude debounce delay
    }
}
