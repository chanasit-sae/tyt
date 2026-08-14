# Chapter 1 — GPIO cheat-sheet

## The 4-step pattern (memorize)
1. **Clock** — `RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;`
2. **Pin mode** — set `MODER` (clear then set)
3. *(no peripheral to configure for plain GPIO)*
4. **Use** — write/read `ODR` / `IDR`

## Key registers (per port, e.g. GPIOA)
| Register | Bits/pin | Purpose | Values |
|----------|----------|---------|--------|
| `MODER`  | 2 | pin mode | `00` input · `01` output · `10` alternate · `11` analog |
| `OTYPER` | 1 | output type | `0` push-pull · `1` open-drain |
| `PUPDR`  | 2 | pull resistor | `00` none · `01` pull-up · `10` pull-down |
| `IDR`    | 1 | input data (read-only) | read pin level |
| `ODR`    | 1 | output data | write pin level |

## Bit idioms
```c
REG |=  (1 << n);   // set
REG &= ~(1 << n);   // clear
REG ^=  (1 << n);   // toggle
if (REG & (1 << n)) // test
```
Multi-bit field: **clear then set** — `REG &= ~mask; REG |= (val << pos);`

## Buttons
- Button on the shield is **active-low**: pull-up idles HIGH (1), pressed = LOW (0).
- Test: `if ((GPIOB->IDR & GPIO_IDR_ID4) == 0) // pressed`

## Gotchas
- Forgetting the RCC clock = writes silently do nothing (#1 bug).
- Software `delay()` **blocks** the CPU — the pain that motivates interrupts (Ch 5).
- Pins: PA5 blue LED (D13), PB4 button (D5).
