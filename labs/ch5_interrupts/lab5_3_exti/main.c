/*=============================================================================
 * Lab 5.3 - External interrupt from a GPIO pin (EXTI)
 *-----------------------------------------------------------------------------
 * GOAL      : A button press on PB4 raises an EXTI interrupt that updates the
 *             blue LED (PA5) - no polling in the main loop.
 * WIRING    : PB4 (D5) -> push button to GND.  PA5 (D13) -> Blue LED.
 * EXPECTED  : Press the button -> LED on; release -> LED off, handled entirely
 *             inside the EXTI ISR.
 *
 * KEY IDEA  : EXTI line 4 maps to pin 4 of ONE port. SYSCFG_EXTICR2 selects
 *             which port (0001 = port B). Line 4 has its own vector:
 *             EXTI4_IRQHandler. ALWAYS clear the pending bit (PR) in the ISR.
 *===========================================================================*/

#define STM32F411xE
#include "stm32f4xx.h"

void EXTI4_IRQHandler(void)
{
    if ((EXTI->PR & EXTI_PR_PR4) != 0) {        /* did line 4 fire? */
        if ((GPIOB->IDR & GPIO_IDR_ID4) == 0) { /* button currently pressed? */
            GPIOA->ODR |=  GPIO_ODR_OD5;
        } else {
            GPIOA->ODR &= ~GPIO_ODR_OD5;
        }
        EXTI->PR |= EXTI_PR_PR4;                 /* CLEAR pending: write 1! */
    }
}

int main(void)
{
    /* 1. Clocks: GPIOA + GPIOB on AHB1, and SYSCFG on APB2 (needed for EXTI). */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    /* 2a. PA5 output (LED). */
    GPIOA->MODER &= ~GPIO_MODER_MODER5;
    GPIOA->MODER |=  (0x01u << GPIO_MODER_MODER5_Pos);

    /* 2b. PB4 input with pull-up (button idles HIGH, pressed = LOW). */
    GPIOB->MODER &= ~GPIO_MODER_MODER4;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD4;
    GPIOB->PUPDR |=  (0x01u << GPIO_PUPDR_PUPD4_Pos);

    /* 3. Configure EXTI line 4. */
    /* 3a. Route line 4 to port B via SYSCFG_EXTICR2 (index [1] in EXTICR[]). */
    SYSCFG->EXTICR[1] &= ~SYSCFG_EXTICR2_EXTI4;
    SYSCFG->EXTICR[1] |=  (0x0001u << SYSCFG_EXTICR2_EXTI4_Pos);   /* port B */

    /* 3b. Trigger on both edges so press AND release are detected. */
    EXTI->RTSR |= EXTI_RTSR_TR4;             /* rising edge  */
    EXTI->FTSR |= EXTI_FTSR_TR4;             /* falling edge */

    /* 3c. Unmask line 4 so its interrupt is allowed through. */
    EXTI->IMR |= EXTI_IMR_MR4;

    /* 3+. Register the EXTI4 interrupt with the NVIC. */
    NVIC_SetPriority(EXTI4_IRQn, 0);
    NVIC_EnableIRQ(EXTI4_IRQn);

    /* 4. Idle - the ISR does all the work. */
    while (1) { }
}
