CC = gcc
CFLAGS = -Wall
WIRINGPI = /usr/lib/libwiringPi.so


sensors.out: device.o data.o
	$(CC) $(CFLAGS) main.c $^ $(WIRINGPI) -o $@

data.o: data.c data.h device.o
	$(CC) -c $< -o $@

device.o: device.c device.h
	$(CC) -c $< -o $@


clean:
	rm -f *.o *.out
