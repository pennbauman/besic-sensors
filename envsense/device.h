#ifndef DEVICE_H
#define DEVICE_H

#include <pigpio.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

float veml7700();
float hs3002();
float tmp117();
float ms5607();

#endif
