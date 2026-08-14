/*=============================================================================
 * Lab 1.5 - Polling (and why blocking is dangerous)
 *-----------------------------------------------------------------------------
 * GOAL      : Demonstrate a polling loop: each button press (PB4) toggles the
 *             blue LED (PA5).
 * WIRING    : PA5 (D13) -> Blue LED.   PB4 (D5) -> push button to GND.
 * EXPECTED  : Each press flips the LED state (on <-> off).
 *
 * TEACHING  : This loop POLLS the button - it constantly checks IDR. The
 *             debounce delay below BLOCKS the CPU: nothing else can run during
 *             it, and a fast event could be missed. Chapter 5 replaces this
 *             with an EXTI interrupt (Lab 5.3).
 *===========================================================================*/

#define STM32F411xE
#include "stm32f4xx.h"

static void delay(volatile uint32_t count)
{
    while (count--) {
        __asm volatile ("nop");
    }
}

int main(void)
{
    /* 1. Clocks for port A (LED) and port B (button). */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;

    /* 2a. PA5 output. */
    GPIOA->MODER &= ~GPIO_MODER_MODER5;
    GPIOA->MODER |=  (0x01u << GPIO_MODER_MODER5_Pos);

    /* 2b. PB4 input with pull-up (idles HIGH, pressed = LOW). */
    GPIOB->MODER &= ~GPIO_MODER_MODER4;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD4;
    GPIOB->PUPDR |=  (0x01u << GPIO_PUPDR_PUPD4_Pos);

    uint8_t was_pressed = 0;

    /* 4. Poll forever, toggling the LED on each new press. */
    while (1) {
        uint8_t pressed = ((GPIOB->IDR & GPIO_IDR_ID4) == 0);

        /* Act only on the transition released -> pressed (edge, not level). */
        if (pressed && !was_pressed) {
            GPIOA->ODR ^= GPIO_ODR_OD5;   /* toggle the LED */
            delay(200000);                /* crude debounce - BLOCKS the CPU */
        }
        was_pressed = pressed;
    }
}
