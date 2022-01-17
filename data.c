#include "data.h"

Data readData() {
	Data reading;
	reading.timestamp = time(NULL);
	reading.lux = veml7700();
	reading.hum = hs3002() * 100;
	reading.tmp = tmp117();
	reading.prs = ms5607();
	return reading;
}

void printData(Data d) {
	printf("VEML7700 LUX: %10.2f\n", d.lux);
	printf("HS3002   HUM: %10.2f%%\n", d.hum);
	printf("TMP117   TMP: %10.2f°C\n", d.tmp, 176);
	printf("MS5607   PRS: %10.2f mBar\n", d.prs);
}

char* jsonData(Data d) {
	char* fmt = "{lux:%.2f,hum:%.2f,tmp:%.2f,prs:%.2f}";
	char* buf = malloc(128); // 69 with 10 width nums
	sprintf(buf, fmt, d.lux, d.hum, d.tmp, d.prs);
	return buf;
}
