/*=============================================================================
 * Lab 3.1 - UART transmit
 *-----------------------------------------------------------------------------
 * GOAL      : Send the character 'K' over USART2 once per second.
 * WIRING    : USART2 is wired to the ST-Link USB virtual COM port (no jumpers).
 *             PA2 = USART2_Tx, PA3 = USART2_Rx (alternate function AF7).
 * TERMINAL  : Open a serial terminal on the "ST-Link Virtual COM Port" at
 *             115200 baud, 8 data bits, No parity, 1 stop bit (8-N-1).
 * EXPECTED  : A stream of 'K' characters appears, one per second.
 *
 * BRR = 139 -> 16 MHz / 115200 baud ~= 139 (oversampling by 16).
 *===========================================================================*/

#define STM32F411xE
#include "stm32f4xx.h"

#define UART_DATA_ASCII  (75U)   /* ASCII code for 'K' */

static void delay(volatile uint32_t count)
{
    while (count--) {
        __asm volatile ("nop");
    }
}

int main(void)
{
    /* 1. Clocks: GPIOA on AHB1, USART2 on APB1. */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    /* 2. PA2 -> alternate function mode (10), then select AF7 (USART2_Tx). */
    GPIOA->MODER &= ~GPIO_MODER_MODER2;
    GPIOA->MODER |=  (0x02u << GPIO_MODER_MODER2_Pos);   /* 10 = alternate */
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL2;
    GPIOA->AFR[0] |=  (7u << GPIO_AFRL_AFSEL2_Pos);      /* AF7 */

    /* 3. Configure USART2: enable, 8 data bits, 1 stop bit, baud, Tx on. */
    USART2->CR1 |=  USART_CR1_UE;      /* USART enable (master switch) */
    USART2->CR1 &= ~USART_CR1_M;       /* M = 0 -> 8 data bits */
    USART2->CR2 &= ~USART_CR2_STOP;    /* STOP = 00 -> 1 stop bit */
    USART2->BRR  =  139;               /* baud rate divisor for 115200 @ 16 MHz */
    USART2->CR1 |=  USART_CR1_TE;      /* transmitter enable */

    /* 4. Send 'K' forever. */
    while (1) {
        while ((USART2->SR & USART_SR_TXE) == 0) { }   /* wait: Tx buffer empty */
        USART2->DR = UART_DATA_ASCII;                   /* write clears TXE */
        delay(1000000);
    }
}
