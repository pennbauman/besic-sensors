#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <besic.h>

// global variables
pthread_mutex_t write_lock;
pthread_cond_t write_cv;
char *AUDIO_FILE, *SENSORS_FILE;


void writeJunk(char const *filename, unsigned int size) {
	FILE *fp;
	fp = fopen(filename, "a");
	if(fp == NULL) {
		perror("File Write Error");
		exit(1);
	}
	for (unsigned int i = 0; i < size; i++) {
		fprintf(fp, "%c", 48 + rand() % 16);
	}
	fprintf(fp, "\n");
	fclose(fp);
}


// Sensor reading thread
void *writing_run(void *args) {
	pthread_mutex_lock(&write_lock);
	while (1) {
		writeJunk(SENSORS_FILE, 55);
		writeJunk(AUDIO_FILE, 703);

		// Wait for next cycle
		pthread_cond_wait(&write_cv, &write_lock);
	}
	pthread_mutex_unlock(&write_lock);
	return NULL;
}


int main(int argc, char **argv) {
	srand(time(NULL));
	// Compute data file location
	char *data_path = besic_data_dir();
	if (data_path == NULL) {
		printf("Data path missing\n");
		return 1;
	}
	AUDIO_FILE = malloc(strlen(data_path) + 10);
	SENSORS_FILE = malloc(strlen(data_path) + 13);
	sprintf(SENSORS_FILE, "%s/sensors.csv", data_path);
	sprintf(AUDIO_FILE, "%s/audio.csv", data_path);

	// Setup console printing
	printf("Dummy CSV Data Generator\n");
	printf(">> %s\n", AUDIO_FILE);
	printf(">> %s\n", SENSORS_FILE);

	// Initialize mutex and locks
	if (pthread_mutex_init(&write_lock, NULL) != 0) {
		perror("mutex_lock");
		return 1;
	}
	if (pthread_cond_init(&write_cv, NULL) != 0) {
		perror("condition");
		return 1;
	}

	// Start data writing thread
	pthread_t write_thread;
	pthread_create(&write_thread, NULL, writing_run, 0);

	// Generate data once per second
	while (1) {
		sleep(1);
		pthread_mutex_lock(&write_lock);
		pthread_cond_broadcast(&write_cv);
		pthread_mutex_unlock(&write_lock);
	}
	return 0;
}
