PREFIX ?= /usr/local
INCLUDE = $(subst //,/,$(DESTDIR)/$(PREFIX)/include/bitstream)

all:
	@echo "Run \"make install\" to install biTStream into $(INCLUDE)"

install:
	@echo "INSTALL  $(INCLUDE)"
	@install -d $(INCLUDE)
	@install -m 644 common.h $(INCLUDE)/
	@install -d $(INCLUDE)/dvb
	@install -m 644 dvb/* $(INCLUDE)/dvb
	@install -d $(INCLUDE)/ietf
	@install -m 644 ietf/* $(INCLUDE)/ietf
	@install -d $(INCLUDE)/mpeg
	@install -m 644 mpeg/* $(INCLUDE)/mpeg

uninstall:
	@echo "REMOVE   $(INCLUDE)"
	@rm -rf $(INCLUDE)

.PHONY: install uninstall
