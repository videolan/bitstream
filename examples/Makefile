# $Id$

CFLAGS = -Wall -O2
LDFLAGS = 
OBJ = dvb_print_si

all: $(OBJ)

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	-rm $(OBJ)
