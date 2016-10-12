# ADS1115 cFS Module
This cFS app contains the read loop and command interface for the ADS1115 Analog-to-Digital Converter (ADC).

TRAPSat uses the ADS1115 ADC to read temperature sensors.

`ads1115.c` includes the interface for getting telemetry and changing how often the application reads the temperature. `ads1115_child.c` contains the loop in which the ADC is read and the values are saved.