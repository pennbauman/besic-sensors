prefix=/usr/local
bindir = $(prefix)/bin
sharedir = $(prefix)/share/besic

all:

install:
	install -m 0755 -d $(DESTDIR)$(bindir)
	echo "#!/bin/bash" > $(DESTDIR)$(bindir)/besic-audio
	echo "python3 $(DESTDIR)$(sharedir)/audio-py/main.py \$$@" >> $(DESTDIR)$(bindir)/besic-audio
	chmod 755 $(DESTDIR)$(bindir)/besic-audio
	install -m 0755 -d $(DESTDIR)$(sharedir)/audio-py
	install -m 0644 main.py $(DESTDIR)$(sharedir)/audio-py
	install -m 0755 -d $(DESTDIR)$(sharedir)/audio-py/lib
	install -m 755 lib/*.py $(DESTDIR)$(sharedir)/audio-py/lib

remove:
	rm -f $(DESTDIR)$(bindir)/besic-audio
	rm -rf $(DESTDIR)$(sharedir)/audio-py
