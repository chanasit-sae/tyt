/*=============================================================================
 * Lab 1.3 / 1.4 - Read a button with an internal pull-up
 *-----------------------------------------------------------------------------
 * GOAL      : Light the blue LED (PA5) while a button (PB4) is pressed.
 * WIRING    : PA5 (D13) -> Blue LED.   PB4 (D5) -> push button to GND.
 * EXPECTED  : Press the button -> LED on. Release -> LED off.
 *
 * NOTE      : The button is ACTIVE-LOW. With the internal pull-up the pin idles
 *             HIGH (reads 1); pressing shorts it to GND so it reads 0.
 *===========================================================================*/

#define STM32F411xE
#include "stm32f4xx.h"

int main(void)
{
    /* 1. Enable clocks for BOTH ports: A (LED) and B (button). */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;

    /* 2a. PA5 as output (the LED). */
    GPIOA->MODER &= ~GPIO_MODER_MODER5;
    GPIOA->MODER |=  (0x01u << GPIO_MODER_MODER5_Pos);   /* 01 = output */

    /* 2b. PB4 as input (00 - the reset default, cleared here to be explicit). */
    GPIOB->MODER &= ~GPIO_MODER_MODER4;                  /* 00 = input */

    /* 2c. Enable the internal pull-up on PB4 so the pin doesn't float.
     *     PUPDR is 2 bits per pin: clear then write 01 (pull-up). */
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD4;
    GPIOB->PUPDR |=  (0x01u << GPIO_PUPDR_PUPD4_Pos);    /* 01 = pull-up */

    /* 4. Poll the button forever. */
    while (1) {
        /* Isolate PB4's bit in IDR. Result is 0 only when the pin is LOW. */
        if ((GPIOB->IDR & GPIO_IDR_ID4) == 0) {
            GPIOA->ODR |=  GPIO_ODR_OD5;   /* pressed  -> LED on  */
        } else {
            GPIOA->ODR &= ~GPIO_ODR_OD5;   /* released -> LED off */
        }
    }
}
