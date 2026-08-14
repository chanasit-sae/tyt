/*=============================================================================
 * Lab 4.2 - Read temperature from an NTC thermistor
 *-----------------------------------------------------------------------------
 * GOAL      : Read the NTC on PA0 (ADC channel 0), convert the ADC value to a
 *             temperature, and print it over UART once per second.
 * WIRING    : PA0 -> divider midpoint (10k fixed on top, NTC on the bottom).
 *             PA2/PA3 -> USART2 (ST-Link virtual COM port).
 * TERMINAL  : 115200 baud, 8-N-1.
 * EXPECTED  : Lines like "Temperature = 25430 millidegree Celcius".
 *
 * IMPORTANT : This lab uses floating-point math, so the FPU must be enabled
 *             first - otherwise float code hard-faults.
 *
 * Beta model: T[K] = (BETA*T0) / (T0*ln(R/R0) + BETA)
 *===========================================================================*/

#define STM32F411xE
#include "stm32f4xx.h"
#include <stdio.h>      /* sprintf */
#include <math.h>       /* log (natural log) */

#define VREF        3.3f
#define VCC         3.3f
#define ADC_MAXRES  4095.0f
#define RX          10000.0f    /* fixed divider resistor */
#define R0          10000.0f    /* NTC resistance at T0 */
#define T0          298.15f     /* 25 C in Kelvin */
#define BETA        3950.0f

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

    /* 0. Enable the FPU (CP10 & CP11 full access) BEFORE any float math. */
    SCB->CPACR |= (0x0Fu << 20);
    __asm volatile ("dsb");
    __asm volatile ("isb");

    /* 1. Clocks: GPIOA on AHB1, USART2 on APB1, ADC1 on APB2. */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    /* 2a. PA2 (Tx) -> AF7. */
    GPIOA->MODER &= ~GPIO_MODER_MODER2;
    GPIOA->MODER |=  (0x02u << GPIO_MODER_MODER2_Pos);
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL2;
    GPIOA->AFR[0] |=  (7u << GPIO_AFRL_AFSEL2_Pos);

    /* 2b. PA0 -> analog mode for the ADC input. */
    GPIOA->MODER &= ~GPIO_MODER_MODER0;
    GPIOA->MODER |=  (0x03u << GPIO_MODER_MODER0_Pos);

    /* 3a. USART2: enable, 8-N-1, baud, transmitter on. */
    USART2->CR1 |=  USART_CR1_UE;
    USART2->CR1 &= ~USART_CR1_M;
    USART2->CR2 &= ~USART_CR2_STOP;
    USART2->BRR  =  139;
    USART2->CR1 |=  USART_CR1_TE;

    /* 3b. ADC1: single conversion on channel 0. */
    ADC1->CR2   |=  ADC_CR2_ADON;
    ADC1->SMPR2 |=  ADC_SMPR2_SMP0;            /* channel 0 sample time */
    ADC1->SQR1  &= ~ADC_SQR1_L;               /* 1 conversion */
    ADC1->SQR3  &= ~ADC_SQR3_SQ1;             /* 1st conversion = channel 0 */

    /* 4. Convert, compute temperature, print, repeat. */
    while (1) {
        ADC1->CR2 |= ADC_CR2_SWSTART;
        while ((ADC1->SR & ADC_SR_EOC) == 0) { }

        float adc_voltage = (ADC1->DR * VREF) / ADC_MAXRES;
        float r_ntc = RX * adc_voltage / (VCC - adc_voltage);
        float temperature =
            ((BETA * T0) / (T0 * logf(r_ntc / R0) + BETA)) - 273.15f;

        /* Print in milli-degrees so we keep 3 decimals with an integer. */
        sprintf(buf, "Temperature = %d millidegree Celcius\n",
                (int32_t)(temperature * 1000.0f));
        UART2_TxString(buf);

        delay(1000000);
    }
}
