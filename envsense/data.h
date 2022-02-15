#ifndef DATA_H
#define DATA_H

#include <stdlib.h>
#include <time.h>

#include "device.h"

typedef struct {
	time_t time;
	float lux;
	float tmp;
	float prs;
	float hum;
} Data;

Data getData();
void readData(Data *d);
void printData(Data const *d);
char *jsonData(Data const *d);
void writeData(Data const *d, char const *filename);

#endif
