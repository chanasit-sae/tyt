# Chapter 3 — UART cheat-sheet

## Clocks & pins
- `RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;`  (pins)
- `RCC->APB1ENR |= RCC_APB1ENR_USART2EN;` (USART2 is on **APB1**)
- **PA2 = Tx**, **PA3 = Rx**, alternate function **AF7**.

## Setup order
```c
GPIOA->MODER |= (0b10 << GPIO_MODER_MODER2_Pos);  // alternate mode
GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFSEL2_Pos);     // AF7
USART2->CR1 |= USART_CR1_UE;      // enable
USART2->CR1 &= ~USART_CR1_M;      // 8 data bits
USART2->CR2 &= ~USART_CR2_STOP;   // 1 stop bit
USART2->BRR  = 139;               // 115200 @ 16 MHz
USART2->CR1 |= USART_CR1_TE;      // transmitter
USART2->CR1 |= USART_CR1_RE;      // receiver
```

## Key flags (SR)
| Flag | Meaning | Wait rule |
|------|---------|-----------|
| `TXE`  | Tx buffer empty | wait **before** writing `DR` |
| `RXNE` | byte received | wait **before** reading `DR` |
| `TC`   | transmission complete | — |

- Writing `DR` clears `TXE`. Reading `DR` clears `RXNE`.

## Frame facts
- Frame = start(0) + 5–9 data + optional parity + stop(1/1.5/2). **LSB first.**
- Baud = bits/sec; both ends must match. Common: **115200**, 9600.
- Parity **detects** a 1-bit error, cannot correct. Terminal setting: **8-N-1**.
- ASCII: `'A'`=65, `'K'`=75, `'0'`=48, `'1'`=49.

## sprintf
```c
char buf[50];
sprintf(buf, "val = %d\n", x);   // %d dec, %x hex, %c char
```

## Gotchas
- Wrong bus (APB2 instead of APB1) = USART never clocks.
- Sending a string: loop until `'\0'`, waiting for `TXE` each byte.
- BRR = f_clk / baud = 16e6 / 115200 ≈ **139**.
