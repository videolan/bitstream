/*****************************************************************************
 * rtp_check_seqnum.c: Prints RTP discontinuities
 *****************************************************************************
 * Copyright (C) 2011 VideoLAN
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
    uint16_t i_seqnum = 0;

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

        uint16_t i_new_seqnum = rtp_get_seqnum(p_buffer);
        if (i_new_seqnum != i_seqnum)
            fprintf(stderr, "received packet %hu while expecting %hu\n",
                    i_new_seqnum, i_seqnum);
        i_seqnum = (i_new_seqnum + 1) % 65536;
    }
}
