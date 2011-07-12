# $Id$

CFLAGS = -Wall -O2 -g
LDFLAGS = 
OBJ = dvb_print_si dvb_ecmg dvb_ecmg_test mpeg_print_pcr

all: $(OBJ)

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	-rm $(OBJ)
