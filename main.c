#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <curl/curl.h>

#include <besic.h>

#include "data.h"

// global variables
pthread_mutex_t read_lock;
pthread_cond_t read_cv, send_cv;
char *MAC, *PASSWORD, *API_URL, *DATA_PATH, *DATA_FILE;
char print = 0;


// Sensor reading thread
void *readings_run(void *args) {
	Data *reading = (Data*)args;
	while (1) {
		pthread_mutex_lock(&read_lock);

		// Read data and start message
		readData(reading);
		pthread_cond_broadcast(&send_cv);
		pthread_mutex_unlock(&read_lock);

		if (!print)
			writeData(reading, DATA_FILE);

		// Wait for new cycle
		pthread_mutex_lock(&read_lock);
		pthread_cond_wait(&read_cv, &read_lock);
		pthread_mutex_unlock(&read_lock);
	}
	return NULL;
}

// Data sending thread
void *sending_run(void *args) {
	Data *reading = (Data*)args;
	// Setup curl
	CURL *curl = curl_easy_init();
	char url[64];
	sprintf(url, "%s/device/data", API_URL);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L); // 15sec
	struct curl_slist *hs=NULL;
	hs = curl_slist_append(hs, "Content-Type: application/json");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);

	while (1) {
		// Wait for data
		pthread_mutex_lock(&read_lock);
		pthread_cond_wait(&send_cv, &read_lock);

		// Print data
		if (print) {
			printf("#############################\n");
			printData(reading);
			pthread_mutex_unlock(&read_lock);
		} else {
			// Prepare json payload
			char* data = jsonData(reading);
			pthread_mutex_unlock(&read_lock);
			char json[256];
			char *fmt = "{\"mac\":\"%s\",\"password\":\"%s\",\"data\":%s}";
			sprintf(json, fmt, MAC, PASSWORD, data);

			// Make API request
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
			CURLcode res = curl_easy_perform(curl);
			if(res != CURLE_OK)
				printf("CURL failed\n");

			free(data);
		}
	}
	return NULL;
}


int main(int argc, char **argv) {
	// Print simple test reading
	if ((argc > 1) && (0 == strcmp("test", argv[1]))) {
		Data reading = getData();
		printData(&reading);
		return 0;
	}

	// Read configuration settings
	MAC = getDeviceMAC();
	if (MAC == NULL) {
		printf("MAC missing\n");
		return 1;
	}
	PASSWORD = getDevicePassword();
	if (PASSWORD == NULL) {
		printf("Password missing\n");
		return 1;
	}
	API_URL = getApiUrl();
	if (API_URL == NULL) {
		printf("API URL missing\n");
		return 1;
	}
	DATA_PATH = getDataPath();
	if (DATA_PATH == NULL) {
		printf("Data path missing\n");
		return 1;
	}

	// Compute data file location
	char *file_name = "/sensors.csv";
	DATA_FILE = malloc(strlen(DATA_PATH) + strlen(file_name) - 1);
	memcpy(DATA_FILE, DATA_PATH, strlen(DATA_PATH));
	memcpy(DATA_FILE + strlen(DATA_PATH), file_name, strlen(file_name));
	DATA_FILE[strlen(DATA_PATH) + strlen(file_name)] = 0;

	// Setup console printing
	if ((argc > 1) && (0 == strcmp("print", argv[1]))) {
		print = 1;
		printf("Environmental Sensors\n");
		printf("MAC:        %s\n", MAC);
		printf("DATA_FILE:  %s\n", DATA_FILE);
		printf("API_URL:    %s\n\n", API_URL);
	}

	// Initialize mutex and locks
	if (pthread_mutex_init(&read_lock, NULL) != 0) {
		perror("mutex_lock: ");
		return 1;
	}
	if (pthread_cond_init(&read_cv, NULL) != 0) {
		perror("condition: ");
		return 1;
	}
	if (pthread_cond_init(&send_cv, NULL) != 0) {
		perror("condition: ");
		return 1;
	}

	Data reading;
	pthread_t readings_thread, sending_thread;

	// Start sensors and data sending thread
	pthread_create(&readings_thread, NULL, readings_run, (void*)&reading);
	pthread_create(&sending_thread, NULL, sending_run, (void*)&reading);

	// Start reading once per second
	while (1) {
		sleep(1);
		pthread_mutex_lock(&read_lock);
		pthread_cond_broadcast(&read_cv);
		pthread_mutex_unlock(&read_lock);
	}
	return 0;
}
