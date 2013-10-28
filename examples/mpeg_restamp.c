/*****************************************************************************
 * mpeg_restamp.c: Restamps PCR and PTS/DTS to make them appear continuous
 *****************************************************************************
 * Copyright (C) 2013 VideoLAN
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

/* Limitation: this supposes the PES header is not fragmented over several
 * TS packets, and that the stream is not scrambled. */

#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#include <bitstream/mpeg/ts.h>
#include <bitstream/mpeg/pes.h>

#define UINT33_MAX UINT64_C(8589934592)
#define TS_CLOCK_MAX UINT33_MAX
#define CLOCK_FREQ UINT64_C(90000)
#define MAX_PCR_INTERVAL (CLOCK_FREQ / 10)

/*****************************************************************************
 * Local declarations
 *****************************************************************************/
static uint64_t i_last_pcr_date;
static uint64_t i_last_pcr = TS_CLOCK_MAX;
static uint64_t i_pcr_offset = 0;

/*****************************************************************************
 * get_date:
 *****************************************************************************/
static uint64_t get_date(void)
{
    struct timespec ts;

    /* Try to use POSIX monotonic clock if available */
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == EINVAL)
        /* Run-time fallback to real-time clock (always available) */
        (void)clock_gettime(CLOCK_REALTIME, &ts);

    return ((uint64_t)ts.tv_sec * CLOCK_FREQ)
            + ((uint64_t)ts.tv_nsec * (CLOCK_FREQ / 10000) / 100000);
}

/*****************************************************************************
 * handle_pcr:
 *****************************************************************************/
static void handle_pcr(uint8_t *p_ts, uint64_t i_date)
{
    uint64_t i_pcr = tsaf_get_pcr(p_ts);
    bool b_discontinuity = tsaf_has_discontinuity(p_ts);

    if (i_last_pcr == TS_CLOCK_MAX)
        i_last_pcr = i_pcr;
    else {
        /* handle 2^33 wrap-arounds */
        uint64_t i_delta =
            (TS_CLOCK_MAX + i_pcr -
             (i_last_pcr % TS_CLOCK_MAX)) % TS_CLOCK_MAX;
        if (i_delta <= MAX_PCR_INTERVAL && !b_discontinuity)
            i_last_pcr = i_pcr;
        else {
            i_last_pcr += i_date - i_last_pcr_date;
            i_last_pcr %= TS_CLOCK_MAX;
            i_pcr_offset += TS_CLOCK_MAX + i_last_pcr - i_pcr;
            i_pcr_offset %= TS_CLOCK_MAX;
            i_last_pcr = i_pcr;
        }
    }
    i_last_pcr_date = i_date;
    if (!i_pcr_offset)
        return;

    i_pcr += i_pcr_offset;
    i_pcr %= TS_CLOCK_MAX;
    tsaf_set_pcr(p_ts, i_pcr);
    tsaf_clear_discontinuity(p_ts);
}

/*****************************************************************************
 * handle_ts:
 *****************************************************************************/
static uint64_t handle_ts(uint64_t i_ts)
{
    i_ts += i_pcr_offset;
    i_ts %= TS_CLOCK_MAX;
    return i_ts;
}

/*****************************************************************************
 * Main loop
 *****************************************************************************/
static void usage(const char *psz)
{
    fprintf(stderr, "usage: %s [<mtu>] < <input file> [> <output>]\n", psz);
    exit(EXIT_FAILURE);
}

int main(int i_argc, char **ppsz_argv)
{
    if (i_argc > 2 || i_argc < 1 ||
        (!strcmp(ppsz_argv[1], "-h") || !strcmp(ppsz_argv[1], "--help")))
        usage(ppsz_argv[0]);

    unsigned int i_mtu = TS_SIZE;
    if (i_argc == 2) {
        i_mtu = strtoul(ppsz_argv[1], NULL, 0);
        if (!i_mtu)
            usage(ppsz_argv[0]);
    }

    for ( ; ; ) {
        uint8_t p_buffer[i_mtu];
        ssize_t i_read = read(STDIN_FILENO, p_buffer, i_mtu);
        uint64_t date = get_date();
        if (i_read == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;
            exit(EXIT_FAILURE);
        }

        if (!i_read)
            exit(EXIT_SUCCESS);

        uint8_t *p_ts = p_buffer;
        while (p_ts < p_buffer + i_mtu && ts_validate(p_ts)) {
            if (ts_has_adaptation(p_ts) && ts_get_adaptation(p_ts) &&
                tsaf_has_pcr(p_ts))
                handle_pcr(p_ts, date);

            uint16_t header_size = TS_HEADER_SIZE +
                                   (ts_has_adaptation(p_ts) ? 1 : 0) +
                                   ts_get_adaptation(p_ts);
            if (ts_get_unitstart(p_ts) && ts_has_payload(p_ts) &&
                header_size + PES_HEADER_SIZE_PTS <= TS_SIZE &&
                pes_validate(p_ts + header_size) &&
                pes_get_streamid(p_ts + header_size) !=
                    PES_STREAM_ID_PRIVATE_2 &&
                pes_validate_header(p_ts + header_size) &&
                pes_has_pts(p_ts + header_size) &&
                pes_validate_pts(p_ts + header_size)) {
                pes_set_pts(p_ts + header_size,
                            handle_ts(pes_get_pts(p_ts + header_size)));

                if (header_size + PES_HEADER_SIZE_PTSDTS <= TS_SIZE &&
                    pes_has_dts(p_ts + header_size) &&
                    pes_validate_dts(p_ts + header_size))
                    pes_set_dts(p_ts + header_size,
                                handle_ts(pes_get_dts(p_ts + header_size)));
            }

            p_ts += TS_SIZE;
        }

        ssize_t i_written = write(STDOUT_FILENO, p_buffer, i_mtu);
        if (i_written == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_FAILURE;
}
