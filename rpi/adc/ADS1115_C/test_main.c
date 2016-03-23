#include <stdio.h>
#include <math.h>
#include "ADS1115_read.h"

#define SERIES_RES  300000       	   // ohms
#define THERM_NOM_RES  10000      // ohms
#define THERM_NOM_TEMP  25          // degrees C
#define THERM_NOM_BCOEF  3892    // K

// ADS Definitions
#define ADS_IC_ID  0x01  // ads1115 ic id
#define ADS_GAIN  4096  // gain?
#define ADS_SPS  250     // samples per second
#define ads_addr  0x49    // change to be addr from i2cdetect -y 1

double volts_to_c(float volts);

int main()
{
    printf("Starting call\n");

  while(1) {
	float voltage0 = ads1115_read(0);
	float voltage1 = ads1115_read(1);
	float voltage2 = ads1115_read(2);
	float voltage3 = ads1115_read(3);
	printf("Therm 0: %f C\n", volts_to_c(voltage0));
	printf("Therm 1: %f C\n", volts_to_c(voltage1));
	printf("Therm 2: %f C\n", volts_to_c(voltage2));
	printf("Therm 3: %f C\n", volts_to_c(voltage3));
	sleep(1);
  }    
    return 0;
}

double volts_to_c(float volts) {
	float therm_r = SERIES_RES / (((ADS_GAIN - 1) / volts) - 1);
	double temp = therm_r / THERM_NOM_RES;
	temp = log(temp);
	temp = temp / THERM_NOM_BCOEF;
	temp = temp + (1.0 / (THERM_NOM_TEMP + 273.15));
	temp = 1.0 / temp;
	return (temp - 273.15);
}
