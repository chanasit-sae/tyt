/*=============================================================================
 * Lab 4.1 - Read the potentiometer with the ADC
 *-----------------------------------------------------------------------------
 * GOAL      : Read the potentiometer on PA4 (ADC channel 4). Turn the blue LED
 *             (PA5) on when the reading is past the midpoint (2048).
 * WIRING    : PA4 -> potentiometer wiper.  PA5 (D13) -> Blue LED.
 * EXPECTED  : Turn the pot up past halfway -> LED on; below halfway -> LED off.
 *
 * ADC is 12-bit (0-4095). Vref = 3.3 V. Digital = floor[Vin/Vref * 4095].
 *===========================================================================*/

#define STM32F411xE
#include "stm32f4xx.h"

int main(void)
{
    /* 1. Clocks: GPIOA on AHB1, ADC1 on APB2 (note: ADC is APB2, not APB1!). */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    /* 2a. PA5 output for the LED. */
    GPIOA->MODER &= ~GPIO_MODER_MODER5;
    GPIOA->MODER |=  (0x01u << GPIO_MODER_MODER5_Pos);

    /* 2b. PA4 -> analog mode (11) so the ADC can sample it. */
    GPIOA->MODER &= ~GPIO_MODER_MODER4;
    GPIOA->MODER |=  (0x03u << GPIO_MODER_MODER4_Pos);   /* 11 = analog */

    /* 3. Configure ADC1 for a single conversion on channel 4. */
    ADC1->CR2   |=  ADC_CR2_ADON;               /* turn the ADC on */
    ADC1->SMPR2 |=  ADC_SMPR2_SMP4;             /* channel 4: 480-cycle sample time */
    ADC1->SQR1  &= ~ADC_SQR1_L;                 /* L = 0 -> 1 conversion in sequence */
    ADC1->SQR3  &= ~ADC_SQR3_SQ1;               /* clear the 1st-conversion slot */
    ADC1->SQR3  |=  (4u << ADC_SQR3_SQ1_Pos);   /* 1st conversion = channel 4 */

    /* 4. Convert repeatedly and drive the LED. */
    while (1) {
        ADC1->CR2 |= ADC_CR2_SWSTART;               /* start a conversion */
        while ((ADC1->SR & ADC_SR_EOC) == 0) { }    /* wait for End Of Conversion */

        if (ADC1->DR > 2048) {                      /* reading DR clears EOC */
            GPIOA->ODR |=  GPIO_ODR_OD5;
        } else {
            GPIOA->ODR &= ~GPIO_ODR_OD5;
        }
    }
}
