#include "data.h"

Data getData() {
	Data reading;
	readData(&reading);
	return reading;
}

void readData(Data *d) {
	d->time = time(NULL);
	d->lux = veml7700();
	d->tmp = tmp117();
	d->prs = ms5607();
	d->hum = hs3002() * 100;
}

void printData(Data const *d) {
	printf("VEML7700 LUX: %10.2f\n", d->lux);
	printf("TMP117   TMP: %10.2f°C\n", d->tmp);
	printf("MS5607   PRS: %10.2f mBar\n", d->prs);
	printf("HS3002   HUM: %10.2f%%\n", d->hum);
}

char *jsonData(Data const *d) {
	char *fmt = "{\"lux\":%.2f,\"tmp\":%.2f,\"prs\":%.2f,\"hum\":%.2f}";
	char *buf = malloc(128);
	sprintf(buf, fmt, d->lux, d->tmp, d->prs, d->hum);
	return buf;
}

void writeData(Data const *d, char const *filename) {
	FILE *fp;
	fp = fopen(filename, "a");
	if(fp == NULL) {
		perror("File Write Error: ");
		exit(1);
	}
	fprintf(fp, "%ld,%f,%f,%f,%f\n", d->time, d->lux, d->tmp, d->prs, d->hum);
	fclose(fp);
}
