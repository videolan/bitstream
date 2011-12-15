PREFIX ?= /usr/local
INCLUDE = $(subst //,/,$(DESTDIR)/$(PREFIX)/include/bitstream)
VERSION = 1.0

all:
	@echo "Run \"make install\" to install biTStream into $(INCLUDE)"

install:
	@echo "INSTALL  $(INCLUDE)"
	@install -d $(INCLUDE)
	@install -m 644 common.h $(INCLUDE)/
	@install -d $(INCLUDE)/dvb
	@install -m 644 dvb/*.h $(INCLUDE)/dvb
	@install -d $(INCLUDE)/dvb/si
	@install -m 644 dvb/si/*.h $(INCLUDE)/dvb/si
	@install -d $(INCLUDE)/ietf
	@install -m 644 ietf/* $(INCLUDE)/ietf
	@install -d $(INCLUDE)/mpeg
	@install -m 644 mpeg/*.h $(INCLUDE)/mpeg
	@install -d $(INCLUDE)/mpeg/psi
	@install -m 644 mpeg/psi/*.h $(INCLUDE)/mpeg/psi

uninstall:
	@echo "REMOVE   $(INCLUDE)"
	@rm -rf $(INCLUDE)

distcheck:
	git archive --format=tar --prefix=bitstream-$(VERSION)/ master | bzip2 -9 > bitstream-$(VERSION).tar.bz2

.PHONY: install uninstall distcheck
