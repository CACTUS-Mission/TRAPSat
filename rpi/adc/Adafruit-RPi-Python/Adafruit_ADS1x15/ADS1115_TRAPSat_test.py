#! /usr/bin/python

# sudo apt-get update && sudo apt-get upgrade -y
# sudo apt-get install python-smbus
# sudo apt-get install i2c-tools
# wget http://github.com/adafruit/Adafruit-Raspberry-Pi-Python-Code/tree/master/Adafruit_ADS1x15
# sudo apt-get update && sudo apt-get upgrade -y

import time, signal, sys
import math
import smbus
from Adafruit_ADS1x15 import ADS1x15

# Thermistor Definitions
SERIES_RES = 300000       # ohms
THERM_NOM_RES = 10000     # ohms
THERM_NOM_TEMP = 25       # degrees C
THERM_NOM_BCOEF = 3892    # K

# ADS Definitions
ADS_IC_ID = 0x01  # ads1115 ic id
ADS_GAIN = 4096   # gain?
ADS_SPS = 250     # samples per second
ads_addr = 0x49   # change to be addr from i2cdetect -y 1

# initialize ADS1115
ads = ADS1x15(ads_addr, ADS_IC_ID, True)

# read from channel 1 continuously
while(1):   
   # Read voltage from ADS
   volts = ads.readADCSingleEnded(0, ADS_GAIN, ADS_SPS)
   #print "Raw Voltage Reading: %.6f volts" % (volts)
   
   # Convert voltage to thermistor resistance
   therm_r = SERIES_RES / (((ADS_GAIN - 1) / volts) - 1)
   #print "Calculated Thermistor Resistance: %.6f ohms" % (therm_r)
   
   # Convert thermistor resistance to temperature
   # using steinhart formula found at:
   # https://learn.adafruit.com/themistor/using-a-thermistor
   temp = therm_r / THERM_NOM_RES
   temp = math.log(temp)
   temp = temp / THERM_NOM_BCOEF
   temp = temp + (1.0 / (THERM_NOM_TEMP + 273.15))
   temp = 1.0 / temp
   temp = temp - 273.15   # convert to C
   #print "Temperature %.6f degrees C" % (temp)
   print "%.6f C" % (temp)

   #print ""
   time.sleep(1)
