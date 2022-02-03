prefix=/usr/local

all:

install:
	mkdir -p $(DESTDIR)$(prefix)/bin
	mkdir -p $(DESTDIR)$(prefix)/share/besic/env-py
	mkdir -p $(DESTDIR)/etc/besic
	install -T test-env.sh $(DESTDIR)$(prefix)/bin/besic-test-env
	cp EnvRead.py $(DESTDIR)$(prefix)/share/besic/env-py
	cp -r lib $(DESTDIR)$(prefix)/share/besic/env-py
	cp env.conf $(DESTDIR)/etc/besic
