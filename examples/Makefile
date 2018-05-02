PREFIX ?= /usr/local
WARN = -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare
CPPFLAGS = -I. -I.. -I../..
CFLAGS := $(WARN) -O2 -g $(CFLAGS)
OBJ = dvb_print_si dvb_gen_si dvb_ecmg dvb_ecmg_test mpeg_print_pcr rtp_check_seqnum mpeg_restamp

ifeq "$(shell uname -s)" "Linux"
LDFLAGS += -lrt
endif

ifeq "$(shell uname -s)" "Darwin"
LDFLAGS += -liconv
endif

all: $(OBJ)

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

install: $(OBJ)
	install -d "$(DESTDIR)$(PREFIX)/bin"
	install -m 755 $(OBJ) "$(DESTDIR)$(PREFIX)/bin"

clean:
	$(RM) -rf $(OBJ) *.dSYM
