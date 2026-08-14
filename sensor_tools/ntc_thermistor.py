#!/usr/bin/env python3
"""NTC thermistor (PA0, ADC channel 0) — convert between ADC code and temperature.

Divider: RX (10k fixed) on top, NTC on the bottom -> Vout across the NTC.
    R_ntc = RX * Vout / (VCC - Vout)
    T[K]  = (BETA * T0) / (T0 * ln(R_ntc / R0) + BETA)

Usage:
    python3 ntc_thermistor.py --adc 1850     # ADC code    -> temperature (C)
    python3 ntc_thermistor.py --temp 30      # temperature -> ADC threshold code
"""
import argparse
import math

VREF = 3.3
VCC = 3.3
ADC_MAX = 4095.0
RX = 10000.0     # fixed divider resistor
R0 = 10000.0     # NTC resistance at T0
T0 = 298.15      # 25 C in Kelvin
BETA = 3950.0


def adc_to_temp(adc):
    v = adc * VREF / ADC_MAX
    r_ntc = RX * v / (VCC - v)
    t_kelvin = (BETA * T0) / (T0 * math.log(r_ntc / R0) + BETA)
    return t_kelvin - 273.15


def temp_to_adc(temp_c):
    t_kelvin = temp_c + 273.15
    r_ntc = R0 * math.exp(BETA * (1.0 / t_kelvin - 1.0 / T0))
    vout = VCC * r_ntc / (RX + r_ntc)
    return int(vout / VREF * ADC_MAX)


def main():
    p = argparse.ArgumentParser(description="NTC thermistor conversions")
    g = p.add_mutually_exclusive_group(required=True)
    g.add_argument("--adc", type=int, help="ADC code (0-4095)")
    g.add_argument("--temp", type=float, help="temperature in Celsius")
    a = p.parse_args()

    if a.adc is not None:
        print(f"ADC {a.adc} -> {adc_to_temp(a.adc):.2f} C")
    else:
        print(f"{a.temp:.2f} C -> ADC threshold {temp_to_adc(a.temp)}")


if __name__ == "__main__":
    main()
