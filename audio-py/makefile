prefix=/usr/local
bindir = $(prefix)/bin
sharedir = $(prefix)/share/besic
etcdir = $(prefix)/etc/besic

all:

install:
	install -m 0755 -d $(DESTDIR)$(bindir)
	install -m 0755 test-audio.sh $(DESTDIR)$(bindir)/besic-test-audio
	install -m 0755 -d $(DESTDIR)$(sharedir)/audio-py
	install -m 0644 AudioRead.py $(DESTDIR)$(sharedir)/audio-py
	install -m 0755 -d $(DESTDIR)$(sharedir)/audio-py/lib
	install -m 755 lib/* $(DESTDIR)$(sharedir)/audio-py/lib
	install -m 0755 -d $(DESTDIR)$(etcdir)
	install -m 0644 audio.conf $(DESTDIR)$(etcdir)
