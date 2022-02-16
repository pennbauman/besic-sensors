CFLAGS = -Wall -std=c17 -O2 -fPIE -g
prefix = /usr/local

envsense.out: main.c device.o data.o
	$(CC) $(CFLAGS) $^ -pie -pthread -lcurl -lpigpio -lbesic -o $@

data.o: data.c data.h device.o
device.o: device.c device.h

install: envsense.out
	mkdir -p $(DESTDIR)$(prefix)/bin
	install -T envsense.out $(DESTDIR)$(prefix)/bin/besic-envsense

clean:
	rm -f *.o *.out
