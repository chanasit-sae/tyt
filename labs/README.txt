STM32F411RE LAB SOLUTIONS
=========================

Complete, commented solutions for every lab in the STEO embedded systems
course. One folder per lab, each with a single main.c. Every main.c starts with
a header block: GOAL, WIRING, TERMINAL settings, and EXPECTED result.

    labs/
      ch1_gpio/
        lab1_2_blink/         Blink the blue LED (PA5).
        lab1_3_button/        Button (PB4) turns the LED on while pressed.
        lab1_5_polling/       Poll a button; each press toggles the LED.
      ch3_uart/
        lab3_1_tx/            Send 'K' every second over USART2.
        lab3_4_echo/          Echo typed characters; '1' lights the green LED.
      ch4_adc/
        lab4_1_potentiometer/ Read the pot; LED on past the midpoint (2048).
        lab4_2_ntc/           Read the NTC; print temperature over UART.
        lab4_3_ldr/           Read the LDR; print lux; LED on when dark.
      ch5_interrupts/
        lab5_1_uart_rx/       UART receive by interrupt (RXNEIE).
        lab5_2_adc_eoc/       ADC read by End-Of-Conversion interrupt (EOCIE).
        lab5_3_exti/          Button press raises an EXTI interrupt.


-------------------------------------------------------------------------------
HOW TO BUILD AND FLASH (STM32CubeIDE)
-------------------------------------------------------------------------------
These files use bare CMSIS register access (no HAL). Steps:

  1. File > New > STM32 Project. Search the board "NUCLEO-F411RE" (or the MCU
     STM32F411RE). Choose an EMPTY / register-level project when asked - decline
     the HAL/graphical setup so nothing fights your register writes.
  2. Replace the generated Core/Src/main.c with the main.c from the lab folder.
  3. Make sure the project defines the device and includes CMSIS. The top two
     lines of each file already do this:
         #define STM32F411xE
         #include "stm32f4xx.h"
     (CubeIDE usually defines STM32F411xE in the project settings too - that is
     why the compiler may warn "macro redefined". It is harmless; you can delete
     the local #define if you prefer.)
  4. Build (hammer icon), then Run/Debug to flash over the on-board ST-Link.

Keil uVIL / other IDEs work the same way: create an F411RE project with CMSIS,
drop in main.c, build, flash.


-------------------------------------------------------------------------------
SERIAL TERMINAL (labs 3.1, 3.4, 4.2, 4.3, 5.1)
-------------------------------------------------------------------------------
Connect to the "STMicroelectronics ST-Link Virtual COM Port" at:
    115200 baud, 8 data bits, No parity, 1 stop bit  (8-N-1)
Terminals: PuTTY, Tera Term, CoolTerm, or https://serialterminal.com


-------------------------------------------------------------------------------
NOTES / GOTCHAS
-------------------------------------------------------------------------------
* Clock first. Every peripheral needs its RCC enable bit set or writes do
  nothing (GPIO -> AHB1, USART2 -> APB1, ADC1 & SYSCFG -> APB2).
* Register macro spellings can vary slightly between CMSIS header versions
  (e.g. GPIO_PUPDR_PUPD4 vs GPIO_PUPDR_PUPDR4). If one does not compile, check
  the exact name in stm32f4xx.h.
* The float labs (4.2, 4.3) enable the FPU before any float math - do not remove
  that block or the code hard-faults.
* Lab 4.3 SLOPE/OFFSET are the GL5528 LDR constants from the slide; verify them
  against your assignment.
* The software delay() loops are approximate, not calibrated to real seconds.
