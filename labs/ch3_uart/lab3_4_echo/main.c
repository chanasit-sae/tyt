/*=============================================================================
 * Lab 3.3 / 3.4 - UART receive and echo
 *-----------------------------------------------------------------------------
 * GOAL      : Echo back every character typed in the terminal, and turn the
 *             green LED (PB6) on when the user types '1', off otherwise.
 * WIRING    : PA2 = USART2_Tx, PA3 = USART2_Rx (AF7). PB6 (D10) -> Green LED.
 * TERMINAL  : 115200 baud, 8-N-1, on the ST-Link Virtual COM Port.
 * EXPECTED  : Type a key -> board replies "You've entered: X". '1' lights the
 *             green LED; anything else turns it off.
 *===========================================================================*/

#define STM32F411xE
#include "stm32f4xx.h"
#include <stdio.h>      /* sprintf */

/* Send a null-terminated string, one byte at a time, waiting for TXE. */
static void UART2_TxString(char *strOut)
{
    for (uint32_t idx = 0; strOut[idx] != '\0'; idx++) {
        while ((USART2->SR & USART_SR_TXE) == 0) { }   /* wait until Tx ready */
        USART2->DR = strOut[idx];
    }
}

int main(void)
{
    char buf[50];

    /* 1. Clocks: GPIOA + GPIOB on AHB1, USART2 on APB1. */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    /* 2a. PA2 (Tx) and PA3 (Rx) -> alternate function AF7. */
    GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
    GPIOA->MODER |=  (0x02u << GPIO_MODER_MODER2_Pos);
    GPIOA->MODER |=  (0x02u << GPIO_MODER_MODER3_Pos);
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3);
    GPIOA->AFR[0] |=  (7u << GPIO_AFRL_AFSEL2_Pos);
    GPIOA->AFR[0] |=  (7u << GPIO_AFRL_AFSEL3_Pos);

    /* 2b. PB6 output for the green LED. */
    GPIOB->MODER &= ~GPIO_MODER_MODER6;
    GPIOB->MODER |=  (0x01u << GPIO_MODER_MODER6_Pos);

    /* 3. Configure USART2: enable, 8-N-1, baud, transmitter AND receiver on. */
    USART2->CR1 |=  USART_CR1_UE;
    USART2->CR1 &= ~USART_CR1_M;
    USART2->CR2 &= ~USART_CR2_STOP;
    USART2->BRR  =  139;
    USART2->CR1 |=  USART_CR1_TE;
    USART2->CR1 |=  USART_CR1_RE;      /* receiver enable */

    /* 4. Poll for received bytes and react. */
    while (1) {
        if ((USART2->SR & USART_SR_RXNE) != 0) {   /* a byte has arrived */
            char c = USART2->DR;                    /* reading DR clears RXNE */

            sprintf(buf, "You've entered: %c\n\r", c);
            UART2_TxString(buf);

            if (c == '1') {
                GPIOB->ODR |=  GPIO_ODR_OD6;   /* green LED on  */
            } else {
                GPIOB->ODR &= ~GPIO_ODR_OD6;   /* green LED off */
            }
        }
    }
}
