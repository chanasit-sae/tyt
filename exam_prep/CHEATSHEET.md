# Exam prep — quick reference

Every answer follows the same skeleton: **1.clock → 2.pin mode → 3.peripheral config → 4.use**.
Clock buses: GPIO→**AHB1**, USART2→**APB1**, ADC1 & SYSCFG→**APB2**.

---

## Q1 — 7-segment (`q1_seven_segment.c`)
- Segments a..g on **PA0..PA6**, output mode. Common-cathode: HIGH = on.
- Digit = a bit pattern; write it straight to `ODR`:
  ```c
  GPIOA->ODR = DIGIT[d];   // bit0=a ... bit6=g
  ```
- Common-anode display → invert every pattern (on = LOW).

## Q2 — ADC → LED bar (`q2_adc_led_bar.c`)
- Analog pin `MODER = 11`. Channel number = pin number (PA4→ch4, PA0→ch0).
- Read pattern (re-set SWSTART every time):
  ```c
  ADC1->CR2 |= ADC_CR2_SWSTART;
  while ((ADC1->SR & ADC_SR_EOC) == 0);
  v = ADC1->DR;
  ```
- Map 0..4095 to levels: `level = v / 820;` (4096/5). Light the lowest `level` LEDs.
- Switch sensor = change `ADC_CH` + the analog pin. Pot=PA4, LDR/NTC=PA0.

## Q3 — buttons + LED + UART (`q3_button_led_uart.c`)
- Buttons **active-low**: input + pull-up, pressed reads 0.
- **Edge detect** so one press = one action:
  ```c
  if (prev == 1 && now == 0) { ...act... }   // falling edge
  prev = now;
  ```
- LED toggle: `GPIOA->ODR ^= (1 << 5);`
- Print a number over UART:
  ```c
  char buf[40];
  sprintf(buf, "ADC = %d\r\n", adc_read());
  while (*p) { while(!(USART2->SR & USART_SR_TXE)); USART2->DR = *p++; }
  ```
- UART setup: PA2 Tx / PA3 Rx, **AF7**, `BRR = 139` (115200 @ 16 MHz), enable TE|RE|UE.

---

## Pins used (adjust to your board)
| Signal | Pin | Note |
|--------|-----|------|
| Blue LED | PA5 | D13 |
| Button A | PB4 | D5, active-low |
| Button B | PB5 | active-low |
| 7-seg a..g | PA0..PA6 | output |
| LED bar | PB0..PB3 | output |
| Pot | PA4 | ADC ch4 |
| NTC / LDR | PA0 | ADC ch0 |
| UART Tx/Rx | PA2 / PA3 | AF7, USART2 |

## Exam gotchas
- **Clock first** — forgot RCC enable = writes do nothing (#1 bug).
- ADC clock is **APB2**, USART2 is **APB1** — wrong bus = dead peripheral.
- `SWSTART` self-clears → re-set it each read.
- Terminal must match: **115200, 8-N-1**.
- Debounce buttons with a small delay or you'll register one press many times.
