# Chapter 5 — Interrupts cheat-sheet

## The 3-edit recipe (polling → interrupt)
1. Set the peripheral's **interrupt-enable bit** (`RXNEIE` / `EOCIE` / EXTI `IMR`).
2. `NVIC_EnableIRQ(<periph>_IRQn);` (and optionally `NVIC_SetPriority`).
3. Move the reaction code into the correctly-named **`_IRQHandler`**.

## ISR rule (fixed by CMSIS)
```c
void USART2_IRQHandler(void) { ... }   // void, no args, exact name
```
A typo in the name = your ISR never runs (it won't link into the vector table).

## NVIC facts
- Manages enable / priority / **nesting** (higher priority preempts lower).
- **Lower priority number = higher urgency.**
- Flow: IRQ → NVIC → save state → jump to ISR (vector table) → exception return.

## Peripheral interrupts
| Source | Enable bit | ISR name | IRQn |
|--------|-----------|----------|------|
| UART Rx | `USART2->CR1 |= USART_CR1_RXNEIE` | `USART2_IRQHandler` | `USART2_IRQn` (38) |
| ADC EOC | `ADC1->CR1  |= ADC_CR1_EOCIE`     | `ADC_IRQHandler`    | `ADC_IRQn` (18) |

## EXTI (external interrupt from a GPIO pin)
- Line **0–15 maps to pin 0–15**; SYSCFG_EXTICR picks the port (`0001`=port B).
- Needs the **SYSCFG** clock: `RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;`
- Components: edge detect, `RTSR`/`FTSR` (edge), `IMR` (mask), `PR` (pending).
```c
SYSCFG->EXTICR[1] |= (0b0001 << SYSCFG_EXTICR2_EXTI4_Pos); // PB4
EXTI->RTSR |= EXTI_RTSR_TR4;   // rising
EXTI->FTSR |= EXTI_FTSR_TR4;   // falling
EXTI->IMR  |= EXTI_IMR_MR4;    // unmask
NVIC_EnableIRQ(EXTI4_IRQn);
```
- Shared vectors: lines 5–9 → `EXTI9_5_IRQHandler`, 10–15 → `EXTI15_10_IRQHandler`.
  Lines 0–4 have their own handlers.

## #1 EXTI mistake
Forgetting to **clear the pending bit** in the ISR by writing **1**:
```c
EXTI->PR |= EXTI_PR_PR4;   // else the interrupt re-fires forever
```
