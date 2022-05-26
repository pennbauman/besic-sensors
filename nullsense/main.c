// BESI-C Null Data Transmitter
//   https://github.com/besi-c/besic-sensors
//   Penn Bauman <pcb8gb@virginia.edu>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <besic.h>

// global variables
pthread_mutex_t send_lock;
pthread_cond_t send_cv;


// Data sending thread
void *sending_run(void *args) {
	// Define data
	besic_data data;
	data.lux = 0.0;
	data.tmp = -273.15;
	data.prs = 0.0;
	data.hum = 100.0;

	pthread_mutex_lock(&send_lock);
	while (1) {
		// Set time
		struct timeval time;
		gettimeofday(&time, NULL);
		data.timestamp = time.tv_sec*1000LL + time.tv_usec/1000LL;

		// Send data heartbeat
		int res = besic_data_heartbeat(&data);
		if (res < 0) {
			printf("CURL failed\n");
		} else if (res > 0) {
			printf("HTTP Error %d\n", res);
		}

		// Wait for next cycle
		pthread_cond_wait(&send_cv, &send_lock);
	}
	pthread_mutex_unlock(&send_lock);
	return NULL;
}


int main(int argc, char **argv) {
	// Initialize mutex and locks
	if (pthread_mutex_init(&send_lock, NULL) != 0) {
		perror("mutex_lock");
		return 1;
	}
	if (pthread_cond_init(&send_cv, NULL) != 0) {
		perror("condition");
		return 1;
	}

	// Start data sending thread
	pthread_t sending_thread;
	pthread_create(&sending_thread, NULL, sending_run, 0);

	// Send data once per second
	while (1) {
		sleep(1);
		pthread_mutex_lock(&send_lock);
		pthread_cond_broadcast(&send_cv);
		pthread_mutex_unlock(&send_lock);
	}
	return 0;
}
