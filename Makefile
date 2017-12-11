PREFIX ?= /usr/local
INCLUDEDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib
INCLUDE = $(DESTDIR)$(INCLUDEDIR)/bitstream
PKGCONFIG = $(DESTDIR)$(LIBDIR)/pkgconfig
VERSION = 1.2

all:
	ln -nsf .. examples/bitstream
	$(MAKE) -C examples

bitstream.pc: bitstream.pc.in
	@echo "GEN      $@"
	@sed -e 's|@PREFIX@|$(PREFIX)|' \
	     -e 's|@INCLUDEDIR@|$(INCLUDEDIR)|' \
	     -e 's|@VERSION@|$(VERSION)|' \
	     $< > $@

install: bitstream.pc
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
	@install -d $(INCLUDE)/itu
	@install -m 644 itu/* $(INCLUDE)/itu
	@install -d $(INCLUDE)/mpeg
	@install -m 644 mpeg/*.h $(INCLUDE)/mpeg
	@install -d $(INCLUDE)/mpeg/psi
	@install -m 644 mpeg/psi/*.h $(INCLUDE)/mpeg/psi
	@install -d $(INCLUDE)/scte
	@install -m 644 scte/*.h $(INCLUDE)/scte
	@install -d $(INCLUDE)/smpte
	@install -m 644 smpte/*.h $(INCLUDE)/smpte
	@echo "INSTALL  $(PKGCONFIG)/bitstream.pc"
	@install -d $(PKGCONFIG)
	@install -m 644 bitstream.pc $(PKGCONFIG)

uninstall:
	@echo "REMOVE   $(INCLUDE)"
	@$(RM) -r $(INCLUDE)
	@echo "REMOVE   $(PKGCONFIG)/bitstream.pc"
	@$(RM) $(PKGCONFIG)/bitstream.pc

dist:
	git archive --format=tar --prefix=bitstream-$(VERSION)/ master | \
	  bzip2 -9 > bitstream-$(VERSION).tar.bz2

clean:
	$(RM) bitstream.pc
	$(MAKE) -C examples clean

.PHONY: all install uninstall dist clean
