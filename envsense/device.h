#ifndef DEVICE_H
#define DEVICE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pigpio.h>
#include <besic.h>

float veml7700();
float hs3002();
float tmp117();
float ms5607();

void readData(besic_data *d);

#endif
