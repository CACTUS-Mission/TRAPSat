#! /usr/bin/python

# sudo apt-get update && sudo apt-get upgrade -y
# sudo apt-get install python-smbus
# sudo apt-get install i2c-tools
# wget http://github.com/adafruit/Adafruit-Raspberry-Pi-Python-Code/tree/master/Adafruit_ADS1x15
# sudo apt-get update && sudo apt-get upgrade -y

import time, signal, sys
import smbus
from Adafruit_ADS1x15 import ADS1x15


ads_addr = 0x49   # change to be addr from i2cdetect -y 1
ads_ic_id = 0x01  # ads1115 ic id
ads_gain = 4096
ads_sps = 250

ads = ADS1x15(ads_addr, ads_ic_id, True)



while(1):
   volts = ads.readADCSingleEnded(0, ads_gain, ads_sps) / 1000
   print "%.6f" % (volts)
   time.sleep(1)
