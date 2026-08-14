/*=============================================================================
 * Lab 1.2 - Blink an LED
 *-----------------------------------------------------------------------------
 * GOAL      : Blink the blue LED on PA5 on and off forever.
 * BOARD     : STM32F411RE Nucleo + STEO shield.
 * WIRING    : PA5 (silkscreen D13) -> Blue LED (on-board, no jumpers needed).
 * EXPECTED  : The blue LED turns on ~1 s, off ~1 s, repeating.
 *
 * PATTERN   : 1) clock  2) pin mode  3) (no peripheral)  4) use (toggle ODR)
 *===========================================================================*/

#define STM32F411xE
#include "stm32f4xx.h"

/* Crude software delay: burns CPU cycles. Not accurate - just long enough to
 * see the LED blink. This BLOCKS the CPU, which is the problem interrupts fix. */
static void delay(volatile uint32_t count)
{
    while (count--) {
        __asm volatile ("nop");
    }
}

int main(void)
{
    /* 1. Enable the clock for GPIO port A (GPIO lives on the AHB1 bus). */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /* 2. Set PA5 as a digital output.
     *    MODER uses 2 bits per pin: clear the field first, then write 01. */
    GPIOA->MODER &= ~GPIO_MODER_MODER5;                    /* clear -> 00 */
    GPIOA->MODER |=  (0x01u << GPIO_MODER_MODER5_Pos);     /* set   -> 01 output */

    /* 4. Use it: toggle PA5 forever with a delay in between. */
    while (1) {
        GPIOA->ODR ^= GPIO_ODR_OD5;   /* XOR flips the pin: on -> off -> on ... */
        delay(1000000);
    }
}
