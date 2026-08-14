/*
 * EXAM Q2 — Read an ADC sensor, light LEDs in sequence by value
 * GOAL:   Read a sensor (potentiometer / LDR / NTC) and light more LEDs the
 *         higher the value — a "bar graph".
 * WIRING: Sensor on PA4 (ADC channel 4). Change to PA0/ch0 for LDR or NTC.
 *         4 LEDs on PB0..PB3 (LED0 = lowest level ... LED3 = highest).
 * IDEA:   0..4095 is split into 5 zones -> light 0,1,2,3 or 4 LEDs.
 *
 * To switch sensor: change ADC_CH and the analog pin below.
 *   potentiometer -> PA4, channel 4
 *   LDR / NTC     -> PA0, channel 0
 */
#define STM32F411xE
#include "stm32f4xx.h"

#define ADC_CH   4        // channel number = pin number (PA4 -> 4)
#define LED_MASK 0x0F     // PB0..PB3

static uint16_t adc_read(void) {
    ADC1->CR2 |= ADC_CR2_SWSTART;                 // start (re-set every read!)
    while ((ADC1->SR & ADC_SR_EOC) == 0);         // wait End Of Conversion
    return ADC1->DR;                              // reading DR clears EOC
}

int main(void) {
    // --- clocks ---
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;          // analog pin port
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;          // LED port
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;           // ADC (on APB2!)

    // --- PA4 = analog (MODER 11) ---
    GPIOA->MODER |= (0b11 << (ADC_CH * 2));

    // --- PB0..PB3 = output ---
    for (int p = 0; p <= 3; p++) {
        GPIOB->MODER &= ~(0b11 << (p * 2));
        GPIOB->MODER |=  (0b01 << (p * 2));
    }

    // --- ADC: single channel, single conversion ---
    ADC1->CR2  |=  ADC_CR2_ADON;                  // ADC on
    ADC1->SQR1 &= ~ADC_SQR1_L;                    // L=0 -> 1 conversion
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;                  // clear 1st slot
    ADC1->SQR3 |=  (ADC_CH << ADC_SQR3_SQ1_Pos);  // 1st conversion = our channel

    while (1) {
        uint16_t v = adc_read();
        int level = v / 820;                      // 4096/5 ~ 820 -> level 0..4

        // light the lowest `level` LEDs, clear the rest
        uint8_t pattern = 0;
        for (int i = 0; i < level; i++) pattern |= (1 << i);
        GPIOB->ODR = (GPIOB->ODR & ~LED_MASK) | pattern;
    }
}
