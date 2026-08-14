# Chapter 4 — ADC cheat-sheet

## Clocks & pins
- `RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;`
- `RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;`  (ADC1 is on **APB2**)
- Analog pin: set `MODER` to **11** (analog). PA0→ch0, PA4→ch4 (PAx→chx for 0–7).

## Conversion formula (memorize)
```
Digital = floor[ Vin / Vref * (2^n - 1) ]      n=12, Vref=3.3, max=4095
```
Reverse (threshold): `Vin = Digital * Vref / 4095`.

## Single-conversion setup
```c
ADC1->CR2   |=  ADC_CR2_ADON;              // ADC on
ADC1->SMPR2 |=  ADC_SMPR2_SMP4;            // sample time for the channel
ADC1->SQR1  &= ~ADC_SQR1_L;               // L=0 -> 1 conversion
ADC1->SQR3  &= ~ADC_SQR3_SQ1;             // clear 1st slot
ADC1->SQR3  |=  (4 << ADC_SQR3_SQ1_Pos);  // 1st conversion = channel 4
```
Read loop:
```c
ADC1->CR2 |= ADC_CR2_SWSTART;             // start (re-set every read!)
while ((ADC1->SR & ADC_SR_EOC) == 0);     // wait End Of Conversion
value = ADC1->DR;                          // reading DR clears EOC
```

## Sensors (voltage-divider: RX=10k on top, sensor on bottom)
```
R_sensor = RX * Vout / (VCC - Vout)
```
- **NTC** (resistance ↓ as temp ↑):  `T[K] = (BETA*T0) / (T0*ln(R/R0) + BETA)`, then −273.15.
  Constants: R0=10k, T0=298.15K, BETA=3950.
- **LDR** (resistance ↓ as light ↑): `lux = 10^((log10(R) - OFFSET)/SLOPE)`.
  GL5528: SLOPE=−0.6875, OFFSET=5.1276 (verify against slide).

## FPU — enable BEFORE any float math
```c
SCB->CPACR |= (0b1111 << 20);   // CP10 & CP11 full access
__asm volatile("dsb");
__asm volatile("isb");
```

## Gotchas
- ADC clock on **APB2**, not APB1.
- `SWSTART` self-clears — re-set it each conversion.
- Skipping the FPU enable → float code **hard-faults**.
- Slide typo: 4.pdf p.28 says "MODER5" for PA4 → it's **MODER4**.
