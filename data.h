#ifndef DATA_H
#define DATA_H

#include <stdlib.h>
#include <time.h>

#include "device.h"

typedef struct {
	time_t timestamp;
	float lux;
	float hum;
	float tmp;
	float prs;
} Data;

Data readData();
void printData(Data d);
char* jsonData(Data d);

#endif
