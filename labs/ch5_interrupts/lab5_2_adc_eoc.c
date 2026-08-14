/*=============================================================================
 * Lab 5.2 - ADC End-Of-Conversion interrupt (instead of polling)
 *-----------------------------------------------------------------------------
 * GOAL      : Same idea as Lab 4.1, but the "read the result and decide" step
 *             runs inside an interrupt. The blue LED (PA5) turns on when the
 *             potentiometer (PA4, channel 4) reading is <= 2048.
 * WIRING    : PA4 -> potentiometer.  PA5 (D13) -> Blue LED.
 * EXPECTED  : main only starts a conversion every ~100 ms; the ISR reads DR
 *             and updates the LED.
 *
 * TRIGGER   : With EOCIE set, the ADC raises its interrupt when EOC=1 (a
 *             conversion finished). The ADC's IRQ number is ADC_IRQn.
 *===========================================================================*/

#define STM32F411xE
#include "stm32f4xx.h"

static void delay(volatile uint32_t count)
{
    while (count--) { __asm volatile ("nop"); }
}

void ADC_IRQHandler(void)
{
    if ((ADC1->SR & ADC_SR_EOC) != 0) {   /* conversion complete */
        if (ADC1->DR <= 2048) {           /* reading DR clears EOC */
            GPIOA->ODR |=  GPIO_ODR_OD5;
        } else {
            GPIOA->ODR &= ~GPIO_ODR_OD5;
        }
    }
}

int main(void)
{
    /* 1. Clocks: GPIOA on AHB1, ADC1 on APB2. */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    /* 2a. PA5 output (LED). */
    GPIOA->MODER &= ~GPIO_MODER_MODER5;
    GPIOA->MODER |=  (0x01u << GPIO_MODER_MODER5_Pos);

    /* 2b. PA4 analog (pot input). */
    GPIOA->MODER &= ~GPIO_MODER_MODER4;
    GPIOA->MODER |=  (0x03u << GPIO_MODER_MODER4_Pos);

    /* 3. ADC1: single conversion on channel 4. */
    ADC1->CR2   |=  ADC_CR2_ADON;
    ADC1->SMPR2 |=  ADC_SMPR2_SMP4;
    ADC1->SQR1  &= ~ADC_SQR1_L;
    ADC1->SQR3  &= ~ADC_SQR3_SQ1;
    ADC1->SQR3  |=  (4u << ADC_SQR3_SQ1_Pos);

    /* 3+. Enable the EOC interrupt and register it with the NVIC. */
    ADC1->CR1 |= ADC_CR1_EOCIE;             /* peripheral-side interrupt enable */
    NVIC_SetPriority(ADC_IRQn, 0);
    NVIC_EnableIRQ(ADC_IRQn);

    /* 4. Kick off a new conversion periodically; the ISR handles the result. */
    while (1) {
        ADC1->CR2 |= ADC_CR2_SWSTART;
        delay(100000);
    }
}
