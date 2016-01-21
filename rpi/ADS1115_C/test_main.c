#include <stdio.h>
#include "ADS1115_read.h"

int main()
{
    printf("Starting call\n");
    float voltage = ads1115_read(0);
    printf("Voltage: %f\n", voltage);
    return 0;
}
