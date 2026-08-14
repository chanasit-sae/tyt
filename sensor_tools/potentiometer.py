#!/usr/bin/env python3
"""Potentiometer (PA4, ADC channel 4) — convert between ADC code, voltage, and %.

Usage:
    python3 potentiometer.py --adc 2655      # ADC code  -> voltage / percent
    python3 potentiometer.py --volt 2.14     # voltage    -> ADC code
    python3 potentiometer.py --percent 50    # percent    -> ADC code (threshold)
"""
import argparse

VREF = 3.3
ADC_MAX = 4095  # 12-bit -> 2^12 - 1


def adc_to_voltage(adc):
    return adc * VREF / ADC_MAX


def voltage_to_adc(v):
    # Digital = floor[ Vin / Vref * (2^n - 1) ]
    return int(v / VREF * ADC_MAX)


def main():
    p = argparse.ArgumentParser(description="Potentiometer conversions")
    g = p.add_mutually_exclusive_group(required=True)
    g.add_argument("--adc", type=int, help="ADC code (0-4095)")
    g.add_argument("--volt", type=float, help="voltage at the pin")
    g.add_argument("--percent", type=float, help="turn percentage (0-100)")
    a = p.parse_args()

    if a.adc is not None:
        v = adc_to_voltage(a.adc)
        print(f"ADC {a.adc} -> {v:.3f} V -> {a.adc / ADC_MAX * 100:.1f} %")
    elif a.volt is not None:
        print(f"{a.volt:.3f} V -> ADC code {voltage_to_adc(a.volt)}")
    else:
        v = a.percent / 100 * VREF
        print(f"{a.percent:.1f} % -> {v:.3f} V -> ADC threshold {voltage_to_adc(v)}")


if __name__ == "__main__":
    main()
