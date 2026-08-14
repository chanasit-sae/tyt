/*=============================================================================
 * Lab 5.1 - UART receive by interrupt (instead of polling)
 *-----------------------------------------------------------------------------
 * GOAL      : Same behaviour as Lab 3.4's receive, but driven by an interrupt.
 *             Typing '1' turns the blue LED (PA5) on; anything else turns it off.
 * WIRING    : PA2/PA3 -> USART2.  PA5 (D13) -> Blue LED.
 * TERMINAL  : 115200 baud, 8-N-1.
 * EXPECTED  : The main loop stays empty; the LED updates from inside the ISR
 *             whenever a byte arrives.
 *
 * TRIGGER   : With RXNEIE set, the USART raises its interrupt whenever RXNE=1
 *             (a byte has arrived). USART2's IRQ number is 38 (USART2_IRQn).
 *===========================================================================*/

#define STM32F411xE
#include "stm32f4xx.h"

/* The ISR name is fixed by CMSIS: <Acronym>_IRQHandler, void/void. */
void USART2_IRQHandler(void)
{
    if ((USART2->SR & USART_SR_RXNE) != 0) {   /* which event? a received byte */
        char c = USART2->DR;                    /* reading DR clears RXNE */
        if (c == '1') {
            GPIOA->ODR |=  GPIO_ODR_OD5;
        } else {
            GPIOA->ODR &= ~GPIO_ODR_OD5;
        }
    }
}

int main(void)
{
    /* 1. Clocks: GPIOA on AHB1, USART2 on APB1. */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    /* 2a. PA3 (Rx) -> AF7. */
    GPIOA->MODER &= ~GPIO_MODER_MODER3;
    GPIOA->MODER |=  (0x02u << GPIO_MODER_MODER3_Pos);
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL3;
    GPIOA->AFR[0] |=  (7u << GPIO_AFRL_AFSEL3_Pos);

    /* 2b. PA5 output (LED). */
    GPIOA->MODER &= ~GPIO_MODER_MODER5;
    GPIOA->MODER |=  (0x01u << GPIO_MODER_MODER5_Pos);

    /* 3. USART2: enable, 8-N-1, baud, receiver on. */
    USART2->CR1 |=  USART_CR1_UE;
    USART2->CR1 &= ~USART_CR1_M;
    USART2->CR2 &= ~USART_CR2_STOP;
    USART2->BRR  =  139;
    USART2->CR1 |=  USART_CR1_RE;

    /* 3+. Turn the receive interrupt on and register it with the NVIC. */
    USART2->CR1 |= USART_CR1_RXNEIE;        /* peripheral-side interrupt enable */
    NVIC_SetPriority(USART2_IRQn, 0);
    NVIC_EnableIRQ(USART2_IRQn);            /* NVIC-side enable */

    /* 4. Nothing to do here - the ISR does the work. */
    while (1) { }
}
