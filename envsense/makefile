CFLAGS = -Wall -std=c17 -O2 -fPIE -g
prefix = /usr/local
bindir = $(prefix)/bin

envsense.out: main.c device.o data.o
	$(CC) $(CFLAGS) $^ -pie -pthread -lcurl -lpigpio -lbesic -o $@

data.o: data.c data.h device.o
device.o: device.c device.h

install: envsense.out
	install -m 0755 -d $(DESTDIR)$(bindir)
	install -m 0755 envsense.out $(DESTDIR)$(bindir)/besic-envsense

clean:
	rm -f *.o *.out
