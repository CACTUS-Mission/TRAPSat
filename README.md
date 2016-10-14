# TRAPSat
Flight and ground software for the TRAPSat payload. All flight software applications are developed within the NASA [cFS](https://cfs.gsfc.nasa.gov/) (Core Flight System) framework.

The flight software is designed to run on a Raspberry Pi Compute Module.

Kernel modules required for flight software:
- i2c
- spi
- DO NOT install serial login kernel module - it will break the camera software.
