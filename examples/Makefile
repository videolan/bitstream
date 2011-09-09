# $Id$

CFLAGS = -Wall -O2 -g
CFLAGS_LOCAL = -Wall -O2 -g -I. -I../..
LDFLAGS = 
OBJ = dvb_print_si dvb_ecmg dvb_ecmg_test mpeg_print_pcr rtp_check_seqnum

all: $(OBJ)

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

local: CFLAGS = $(CFLAGS_LOCAL)
local: all

clean:
	-rm $(OBJ)
