#!/usr/bin/env python3
"""LDR / photoresistor (PA0, ADC channel 0) — convert between ADC code and lux.

Divider: RX (10k fixed) on top, LDR on the bottom -> Vout across the LDR.
    R_ldr = RX * Vout / (VCC - Vout)
    log10(R_ldr) = SLOPE * log10(lux) + OFFSET
    lux = 10 ^ ((log10(R_ldr) - OFFSET) / SLOPE)

NOTE: SLOPE and OFFSET are the GL5528 constants from Lab 4.3. Verify them
against the slide before trusting exact lux numbers.

Usage:
    python3 ldr.py --adc 1200    # ADC code -> light intensity (lux)
    python3 ldr.py --lux 300     # lux      -> ADC threshold code
"""
import argparse
import math

VREF = 3.3
VCC = 3.3
ADC_MAX = 4095.0
RX = 10000.0
SLOPE = -0.6875
OFFSET = 5.1276


def adc_to_lux(adc):
    v = adc * VREF / ADC_MAX
    r_ldr = RX * v / (VCC - v)
    return 10 ** ((math.log10(r_ldr) - OFFSET) / SLOPE)


def lux_to_adc(lux):
    r_ldr = 10 ** (SLOPE * math.log10(lux) + OFFSET)
    vout = VCC * r_ldr / (RX + r_ldr)
    return int(vout / VREF * ADC_MAX)


def main():
    p = argparse.ArgumentParser(description="LDR light-intensity conversions")
    g = p.add_mutually_exclusive_group(required=True)
    g.add_argument("--adc", type=int, help="ADC code (0-4095)")
    g.add_argument("--lux", type=float, help="light intensity in lux")
    a = p.parse_args()

    if a.adc is not None:
        print(f"ADC {a.adc} -> {adc_to_lux(a.adc):.0f} lux")
    else:
        print(f"{a.lux:.0f} lux -> ADC threshold {lux_to_adc(a.lux)}")


if __name__ == "__main__":
    main()
