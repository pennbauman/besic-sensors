CC = clang
CFLAGS = -Wall

sensors.out: main.c device.o data.o
	$(CC) $(CFLAGS) -pthread -lcurl -lwiringPi $^ -o $@

data.o: data.c data.h device.o
device.o: device.c device.h

clean:
	rm -f *.o *.out
