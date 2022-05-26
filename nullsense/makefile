# BESI-C Null Data Transmitter
#   https://github.com/besi-c/besic-sensors
#   Penn Bauman <pcb8gb@virginia.edu>
CC = gcc
CFLAGS = -g -Wall -std=c17 -O3 -fPIE
prefix = /usr/local
bindir = $(prefix)/bin

nullsense.out: main.c
	$(CC) $(CFLAGS) $^ -pie -pthread -lbesic -o $@

install: nullsense.out
	install -m 0755 -d $(DESTDIR)$(bindir)
	install -m 0755 nullsense.out $(DESTDIR)$(bindir)/besic-nullsense

clean:
	rm -f *.o *.out
