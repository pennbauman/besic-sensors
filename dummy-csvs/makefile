CC = gcc
CFLAGS = -g -Wall -std=c17 -O3 -fPIE
prefix = /usr/local
bindir = $(prefix)/bin

dummy-csvs.out: main.c
	$(CC) $(CFLAGS) $^ -pie -pthread -lbesic -o $@

install: dummy-csvs.out
	install -m 0755 -d $(DESTDIR)$(bindir)
	install -m 0755 dummy-csvs.out $(DESTDIR)$(bindir)/besic-dummy-csvs

clean:
	rm -f *.o *.out
