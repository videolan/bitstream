PREFIX ?= /usr/local
INCLUDE = $(subst //,/,$(DESTDIR)/$(PREFIX)/include/bitstream)
VERSION = 1.1

all:
	@echo "Run \"make install\" to install biTStream into $(INCLUDE)"

install:
	@echo "INSTALL  $(INCLUDE)"
	@install -d $(INCLUDE)
	@install -m 644 common.h $(INCLUDE)/
	@install -d $(INCLUDE)/atsc
	@install -m 644 atsc/*.h $(INCLUDE)/atsc
	@install -d $(INCLUDE)/dvb
	@install -m 644 dvb/*.h $(INCLUDE)/dvb
	@install -d $(INCLUDE)/dvb/si
	@install -m 644 dvb/si/*.h $(INCLUDE)/dvb/si
	@install -d $(INCLUDE)/ietf
	@install -m 644 ietf/* $(INCLUDE)/ietf
	@install -d $(INCLUDE)/ieee
	@install -m 644 ieee/* $(INCLUDE)/ieee
	@install -d $(INCLUDE)/mpeg
	@install -m 644 mpeg/*.h $(INCLUDE)/mpeg
	@install -d $(INCLUDE)/mpeg/psi
	@install -m 644 mpeg/psi/*.h $(INCLUDE)/mpeg/psi
	@install -d $(INCLUDE)/scte
	@install -m 644 scte/*.h $(INCLUDE)/scte

uninstall:
	@echo "REMOVE   $(INCLUDE)"
	@rm -rf $(INCLUDE)

dist:
	git archive --format=tar --prefix=bitstream-$(VERSION)/ master | bzip2 -9 > bitstream-$(VERSION).tar.bz2

.PHONY: install uninstall distcheck
