/*****************************************************************************
 * rtp_check_cc.c: Prints RTP discontinuities
 *****************************************************************************
 * Copyright (C) 2011 VideoLAN
 * $Id: dvb_print_si.c 27 2011-04-10 16:57:59Z massiot $
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include <bitstream/ietf/rtp.h>

/*****************************************************************************
 * Local declarations
 *****************************************************************************/
#define DEFAULT_PACKET_SIZE 1328

/*****************************************************************************
 * Main loop
 *****************************************************************************/
static void usage(const char *psz)
{
    fprintf(stderr, "usage: multicat -u -m <packet size> @<addr>:<port> | %s [<packet size>]\n", psz);
    exit(EXIT_FAILURE);
}

int main(int i_argc, char **ppsz_argv)
{
    int i_packet_size = DEFAULT_PACKET_SIZE;

    if (ppsz_argv[1] != NULL &&
        (!strcmp(ppsz_argv[1], "-h") || !strcmp(ppsz_argv[1], "--help")))
        usage(ppsz_argv[0]);

    if (ppsz_argv[1] != NULL)
        i_packet_size = atoi(ppsz_argv[1]);
    if (i_packet_size > 65535 || i_packet_size < RTP_HEADER_SIZE)
        usage(ppsz_argv[0]);

    uint8_t p_buffer[i_packet_size];
    uint16_t i_cc = 0;

    for ( ; ; ) {
        ssize_t toto;
        if ((toto=read(STDIN_FILENO, p_buffer, i_packet_size)) != i_packet_size) {
            fprintf(stderr, "couldn't read %zd\n", toto);
            exit(EXIT_FAILURE);
        }

        if (!rtp_check_hdr(p_buffer)) {
            fprintf(stderr, "invalid RTP header\n");
            continue;
        }

        uint16_t i_new_cc = rtp_get_cc(p_buffer);
        if (i_new_cc != i_cc)
            fprintf(stderr, "received packet %hu while expecting %hu\n",
                    i_new_cc, i_cc);
        i_cc = (i_new_cc + 1) % 65536;
    }
}
