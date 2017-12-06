PREFIX ?= /usr/local
WARN = -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare
CPPFLAGS = -I.
CFLAGS := $(WARN) -O2 -g $(CFLAGS)
LDFLAGS := -lrt $(LDFLAGS)
OBJ = dvb_print_si dvb_gen_si dvb_ecmg dvb_ecmg_test mpeg_print_pcr rtp_check_seqnum mpeg_restamp

ifeq "$(shell uname -s)" "Darwin"
LDLIBS += -liconv
# Prevent generation of .dSYM debug directories
CFLAGS += -gstabs
endif

all: $(OBJ)

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

install: $(OBJ)
	install -d "$(DESTDIR)$(PREFIX)/bin"
	install -m 755 $(OBJ) "$(DESTDIR)$(PREFIX)/bin"

clean:
	$(RM) $(OBJ)
