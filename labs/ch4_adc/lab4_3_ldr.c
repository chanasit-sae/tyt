/*=============================================================================
 * Lab 4.3 - Read light intensity from an LDR (photoresistor)
 *-----------------------------------------------------------------------------
 * GOAL      : Read the LDR on PA0 (ADC channel 0), convert to light intensity
 *             in lux, print it over UART once per second, and turn the blue LED
 *             (PA5) on when it is dark (LDR covered).
 * WIRING    : PA0 -> divider midpoint (10k fixed on top, LDR on the bottom).
 *             PA2 -> USART2_Tx (AF7).  PA5 (D13) -> Blue LED.
 * TERMINAL  : 115200 baud, 8-N-1.
 * EXPECTED  : Lines like "Light intensity = 25742 Lux". Cover the LDR -> LED on.
 *
 * IMPORTANT : Enable the FPU before the floating-point math.
 * NOTE      : SLOPE / OFFSET are the GL5528 constants from the slide - verify
 *             them against your assignment before trusting exact lux numbers.
 *
 * lux = 10 ^ ((log10(R_ldr) - OFFSET) / SLOPE)
 *===========================================================================*/

#define STM32F411xE
#include "stm32f4xx.h"
#include <stdio.h>
#include <math.h>

#define VREF        3.3f
#define VCC         3.3f
#define ADC_MAXRES  4095.0f
#define RX          10000.0f
#define SLOPE       (-0.6875f)
#define OFFSET      5.1276f
#define DARK_LUX    50.0f       /* below this = "dark", turn the LED on */

static void delay(volatile uint32_t count)
{
    while (count--) { __asm volatile ("nop"); }
}

static void UART2_TxString(char *strOut)
{
    for (uint32_t idx = 0; strOut[idx] != '\0'; idx++) {
        while ((USART2->SR & USART_SR_TXE) == 0) { }
        USART2->DR = strOut[idx];
    }
}

int main(void)
{
    char buf[64];

    /* 0. FPU on before float math. */
    SCB->CPACR |= (0x0Fu << 20);
    __asm volatile ("dsb");
    __asm volatile ("isb");

    /* 1. Clocks. */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    /* 2a. PA2 -> AF7 (USART2_Tx). */
    GPIOA->MODER &= ~GPIO_MODER_MODER2;
    GPIOA->MODER |=  (0x02u << GPIO_MODER_MODER2_Pos);
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL2;
    GPIOA->AFR[0] |=  (7u << GPIO_AFRL_AFSEL2_Pos);

    /* 2b. PA5 output (LED). */
    GPIOA->MODER &= ~GPIO_MODER_MODER5;
    GPIOA->MODER |=  (0x01u << GPIO_MODER_MODER5_Pos);

    /* 2c. PA0 analog (LDR input). */
    GPIOA->MODER &= ~GPIO_MODER_MODER0;
    GPIOA->MODER |=  (0x03u << GPIO_MODER_MODER0_Pos);

    /* 3a. USART2. */
    USART2->CR1 |=  USART_CR1_UE;
    USART2->CR1 &= ~USART_CR1_M;
    USART2->CR2 &= ~USART_CR2_STOP;
    USART2->BRR  =  139;
    USART2->CR1 |=  USART_CR1_TE;

    /* 3b. ADC1 single conversion on channel 0. */
    ADC1->CR2   |=  ADC_CR2_ADON;
    ADC1->SMPR2 |=  ADC_SMPR2_SMP0;
    ADC1->SQR1  &= ~ADC_SQR1_L;
    ADC1->SQR3  &= ~ADC_SQR3_SQ1;

    /* 4. Convert, compute lux, print, drive LED. */
    while (1) {
        ADC1->CR2 |= ADC_CR2_SWSTART;
        while ((ADC1->SR & ADC_SR_EOC) == 0) { }

        float adc_voltage = (ADC1->DR * VREF) / ADC_MAXRES;
        float r_ldr = RX * adc_voltage / (VCC - adc_voltage);
        float lux = powf(10.0f, (log10f(r_ldr) - OFFSET) / SLOPE);

        sprintf(buf, "Light intensity = %d Lux\n", (int32_t)lux);
        UART2_TxString(buf);

        if (lux < DARK_LUX) {
            GPIOA->ODR |=  GPIO_ODR_OD5;   /* dark -> LED on */
        } else {
            GPIOA->ODR &= ~GPIO_ODR_OD5;
        }

        delay(1000000);
    }
}
