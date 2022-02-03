prefix=/usr/local

all:

install:
	mkdir -p $(DESTDIR)$(prefix)/bin
	mkdir -p $(DESTDIR)$(prefix)/share/besic/audio-py
	mkdir -p $(DESTDIR)/etc/besic
	install -T test-audio.sh $(DESTDIR)$(prefix)/bin/besic-test-audio
	cp AudioRead.py $(DESTDIR)$(prefix)/share/besic/audio-py
	cp -r lib $(DESTDIR)$(prefix)/share/besic/audio-py
	cp audio.conf $(DESTDIR)/etc/besic
