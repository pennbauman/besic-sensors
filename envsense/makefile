# BESI-C Environmental Sensors
#   https://github.com/besi-c/besic-sensors
#   Penn Bauman <pcb8gb@virginia.edu>
CC = gcc
CFLAGS = -g -Wall -std=c17 -O3 -fPIE
prefix = /usr/local
bindir = $(prefix)/bin

envsense.out: main.c device.o
	$(CC) $(CFLAGS) $^ -pie -pthread -lpigpio -lbesic -o $@

device.o: device.c device.h

install: envsense.out
	install -m 0755 -d $(DESTDIR)$(bindir)
	install -m 0755 envsense.out $(DESTDIR)$(bindir)/besic-envsense

clean:
	rm -f *.o *.out
