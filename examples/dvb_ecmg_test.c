/*****************************************************************************
 * dvb_ecmg_test.c: Simulation of an SCS to stress-test an ECMG
 *****************************************************************************
 * Copyright (C) 2010 VideoLAN
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

/*
 * You want to use this in conjunction with netcat, for instance:
 *
 * mkfifo /tmp/ecmg_in
 * mkfifo /tmp/ecmg_out
 * nc 192.168.0.1 6900 < /tmp/ecmg_out > /tmp/ecmg_in &
 * dvb_ecmg_test 10 10 > /tmp/ecmg_out < /tmp/ecmg_in
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <time.h>

#include <bitstream/dvb/sim.h>
#include <bitstream/mpeg/psi.h>

/* normative size is 64 ko, but we will not output messages longer than this */
#define MAX_TLV_SIZE    (1024 - TLV_HEADER_SIZE)
#define SELECT_TIMEOUT  1 /* s */

/*****************************************************************************
 * Local declarations
 *****************************************************************************/
typedef struct stream_t {
    uint16_t i_streamid;
    uint16_t i_ecmid;
    uint16_t i_cp_number;
    bool b_init;
} stream_t;

static stream_t **pp_streams = NULL;
static int i_nb_streams = 0;
static time_t i_last_time;

static bool b_init = true;
static uint8_t i_version = 1;
static uint16_t i_channelid = 0;
static uint32_t i_supercasid;
static uint8_t i_sectiontspkt;
static uint8_t i_nb_cw;
static uint8_t i_lead_cw;

static int i_wanted_streams;
static int i_period;

/*****************************************************************************
 * read_wrapper
 *****************************************************************************/
ssize_t read_wrapper(void *p_buf, size_t i_count)
{
    size_t i_received = 0;

    do {
        ssize_t i_read = read(STDIN_FILENO, p_buf + i_received,
                              i_count - i_received);
        if (i_read < 0 && errno == EINTR) continue;
        if (i_read < 0) {
            fprintf(stderr, "read error, aborting (%m)");
            return i_read;
        }
        if (!i_read) {
            fprintf(stderr, "read end-of-file, aborting (%m)");
            return i_read;
        }
        i_received += i_read;
    } while (i_received < i_count);

    return i_count;
}

/*****************************************************************************
 * write_wrapper
 *****************************************************************************/
ssize_t write_wrapper(const void *p_buf, size_t i_count)
{
    size_t i_sent = 0;

    do {
        ssize_t i_written = write(STDOUT_FILENO,
                                  p_buf + i_sent, i_count - i_sent);
        if (i_written < 0 && errno == EINTR) continue;
        if (i_written < 0) {
            fprintf(stderr, "write error (%m)");
            return i_written;
        }
        i_sent += i_written;
    } while (i_sent < i_count);

    return i_count;
}

/*****************************************************************************
 * send_channel_setup
 *****************************************************************************/
static void send_channel_setup(void)
{
    uint8_t p_tlv_h[MAX_TLV_SIZE];
    uint8_t *p_tlv = tlvh_get_tlv(p_tlv_h);
    uint8_t *p_tlv_n;

    tlvh_set_version(p_tlv_h, i_version);
    ecmg_init(p_tlv);
    tlv_set_type(p_tlv, ECMG_TYPE_CHANNEL_SETUP);
    /* length will be written at the end */
    tlv_set_length(p_tlv, MAX_TLV_SIZE);

    ecmg_append_channelid(p_tlv, i_channelid);
    ecmg_append_supercasid(p_tlv, i_supercasid);

    p_tlv_n = tlv_find_param(p_tlv, TLV_PARAM_EMPTY, 0);
    tlv_set_length(p_tlv, p_tlv_n - p_tlv - TLV_HEADER_SIZE);
    write_wrapper(p_tlv_h, p_tlv_n - p_tlv_h);
}

/*****************************************************************************
 * send_channel_error
 *****************************************************************************/
static void send_channel_error(uint16_t i_wanted_channelid, uint16_t i_error)
{
    uint8_t p_tlv_h[MAX_TLV_SIZE];
    uint8_t *p_tlv = tlvh_get_tlv(p_tlv_h);
    uint8_t *p_tlv_n;

    fprintf(stderr, "sending error on channel ID=0x%hx error=0x%hx\n",
            i_wanted_channelid, i_error);

    tlvh_set_version(p_tlv_h, i_version);
    ecmg_init(p_tlv);
    tlv_set_type(p_tlv, ECMG_TYPE_CHANNEL_ERROR);
    /* length will be written at the end */
    tlv_set_length(p_tlv, MAX_TLV_SIZE - TLV_HEADER_SIZE - TLVH_HEADER_SIZE);

    ecmg_append_channelid(p_tlv, i_wanted_channelid);
    ecmg_append_errorstatus(p_tlv, i_error);

    p_tlv_n = tlv_find_param(p_tlv, TLV_PARAM_EMPTY, 0);
    tlv_set_length(p_tlv, p_tlv_n - p_tlv - TLV_HEADER_SIZE);
    write_wrapper(p_tlv_h, p_tlv_n - p_tlv_h);
}

/*****************************************************************************
 * send_stream_setup
 *****************************************************************************/
static void send_stream_setup(stream_t *p_stream)
{
    uint8_t p_tlv_h[MAX_TLV_SIZE];
    uint8_t *p_tlv = tlvh_get_tlv(p_tlv_h);
    uint8_t *p_tlv_n;

    tlvh_set_version(p_tlv_h, i_version);
    ecmg_init(p_tlv);
    tlv_set_type(p_tlv, ECMG_TYPE_STREAM_SETUP);
    /* length will be written at the end */
    tlv_set_length(p_tlv, MAX_TLV_SIZE - TLV_HEADER_SIZE - TLVH_HEADER_SIZE);

    ecmg_append_channelid(p_tlv, i_channelid);
    ecmg_append_streamid(p_tlv, p_stream->i_streamid);
    if (i_version >= 2)
        ecmg_append_ecmid(p_tlv, p_stream->i_ecmid);
    ecmg_append_nomcpdur(p_tlv, i_period * 10);

    p_tlv_n = tlv_find_param(p_tlv, TLV_PARAM_EMPTY, 0);
    tlv_set_length(p_tlv, p_tlv_n - p_tlv - TLV_HEADER_SIZE);
    write_wrapper(p_tlv_h, p_tlv_n - p_tlv_h);
}

/*****************************************************************************
 * send_stream_error
 *****************************************************************************/
static void send_stream_error(uint16_t i_wanted_streamid, uint16_t i_error)
{
    uint8_t p_tlv_h[MAX_TLV_SIZE];
    uint8_t *p_tlv = tlvh_get_tlv(p_tlv_h);
    uint8_t *p_tlv_n;

    fprintf(stderr, "sending error on stream ID=0x%hx error=0x%hx\n",
            i_wanted_streamid, i_error);

    tlvh_set_version(p_tlv_h, i_version);
    ecmg_init(p_tlv);
    tlv_set_type(p_tlv, ECMG_TYPE_STREAM_ERROR);
    /* length will be written at the end */
    tlv_set_length(p_tlv, MAX_TLV_SIZE - TLV_HEADER_SIZE - TLVH_HEADER_SIZE);

    ecmg_append_channelid(p_tlv, i_channelid);
    ecmg_append_streamid(p_tlv, i_wanted_streamid);
    ecmg_append_errorstatus(p_tlv, i_error);

    p_tlv_n = tlv_find_param(p_tlv, TLV_PARAM_EMPTY, 0);
    tlv_set_length(p_tlv, p_tlv_n - p_tlv - TLV_HEADER_SIZE);
    write_wrapper(p_tlv_h, p_tlv_n - p_tlv_h);
}

/*****************************************************************************
 * send_cw
 *****************************************************************************/
static void send_cw(stream_t *p_stream)
{
    uint8_t p_tlv_h[MAX_TLV_SIZE];
    uint8_t *p_tlv = tlvh_get_tlv(p_tlv_h);
    uint8_t *p_tlv_n;
    int i;

    tlvh_set_version(p_tlv_h, i_version);
    ecmg_init(p_tlv);
    tlv_set_type(p_tlv, ECMG_TYPE_CW);
    /* length will be written at the end */
    tlv_set_length(p_tlv, MAX_TLV_SIZE);

    ecmg_append_channelid(p_tlv, i_channelid);
    ecmg_append_streamid(p_tlv, p_stream->i_streamid);
    ecmg_append_cpnumber(p_tlv, p_stream->i_cp_number);

    for (i = 1; i <= i_nb_cw; i++) {
        uint8_t p_cw[8 + 2];
        uint8_t *pi_cw = ecmgcw_get_cw(p_cw);
        uint32_t i_rand;

        ecmgcw_set_cpnum(p_cw,
                         p_stream->i_cp_number + i - (i_nb_cw - i_lead_cw));

        i_rand = rand();
        memcpy(pi_cw, &i_rand, 4);
        i_rand = rand();
        memcpy(pi_cw + 4, &i_rand, 4);

        tlv_append_data(p_tlv, ECMG_PARAM_CPCWCOMB, p_cw, 8 + 2);
    }

    p_tlv_n = tlv_find_param(p_tlv, TLV_PARAM_EMPTY, 0);
    tlv_set_length(p_tlv, p_tlv_n - p_tlv - TLV_HEADER_SIZE);
    write_wrapper(p_tlv_h, p_tlv_n - p_tlv_h);
}

/*****************************************************************************
 * check_cw
 *****************************************************************************/
static void check_cw(void)
{
    int i_turn = i_last_time % i_period;
    int i;

    for (i = 0; i < i_nb_streams; i++)
        if (!pp_streams[i]->b_init && (i % i_period) == i_turn)
            send_cw(pp_streams[i]);
}

/*****************************************************************************
 * find_stream
 *****************************************************************************/
static stream_t *find_stream(uint16_t i_streamid)
{
    int i;

    for (i = 0; i < i_nb_streams; i++)
        if (pp_streams[i] != NULL && pp_streams[i]->i_streamid == i_streamid)
            return pp_streams[i];

    return NULL;
}

/*****************************************************************************
 * create_stream
 *****************************************************************************/
static void create_stream(void)
{
    stream_t *p_stream = malloc(sizeof(stream_t));
    pp_streams = realloc(pp_streams, ++i_nb_streams * sizeof(stream_t *));
    pp_streams[i_nb_streams - 1] = p_stream;
    p_stream->i_streamid = i_nb_streams;
    p_stream->i_ecmid = i_nb_streams;
    p_stream->i_cp_number = UINT8_MAX;
    p_stream->b_init = true;

    send_stream_setup(p_stream);

    fprintf(stderr, "starting new stream id=0x%hx\n", p_stream->i_streamid);
}

/*****************************************************************************
 * handle_channel_status
 *****************************************************************************/
static void handle_channel_status(uint8_t *p_tlv)
{
    uint16_t i_wanted_channelid = ecmg_find_channelid(p_tlv, 0);

    if (i_wanted_channelid != i_channelid) {
        send_channel_error(i_wanted_channelid, 0x6);
        exit(EXIT_FAILURE);
    }

    if (!b_init)
        return;

    if (ecmg_find_maxstreams(p_tlv, 0) < i_wanted_streams) {
        fprintf(stderr, "not enough supported streams (%hu)\n",
                ecmg_find_maxstreams(p_tlv, 0));
        exit(EXIT_FAILURE);
    }

    if (ecmg_find_mincpdur(p_tlv, 0) > i_period * 10) {
        fprintf(stderr, "crypto-period too short (%hu)\n",
                ecmg_find_mincpdur(p_tlv, 0));
        exit(EXIT_FAILURE);
    }

    i_sectiontspkt = ecmg_find_sectiontspkt(p_tlv, 0);
    i_nb_cw = ecmg_find_cwpermsg(p_tlv, 0);
    i_lead_cw = ecmg_find_leadcw(p_tlv, 0);

    fprintf(stderr,
            "starting channel ID=0x%hx version=%hhu super CAS ID=0x%x\n",
            i_channelid, i_version, i_supercasid);
    b_init = false;
}

/*****************************************************************************
 * handle_channel_test
 *****************************************************************************/
static void handle_channel_test(uint8_t *p_tlv)
{
    uint16_t i_wanted_channelid = ecmg_find_channelid(p_tlv, 0);

    if (i_wanted_channelid != i_channelid) {
        send_channel_error(i_wanted_channelid, 0x6);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "channel test received\n");
    exit(EXIT_FAILURE);
}

/*****************************************************************************
 * handle_channel_error
 *****************************************************************************/
static void handle_channel_error(uint8_t *p_tlv)
{
    uint16_t i_wanted_channelid = ecmg_find_channelid(p_tlv, 0);

    fprintf(stderr, "receiving error channel ID=0x%hu error=0x%hx\n",
            i_wanted_channelid, ecmg_find_errorstatus(p_tlv, 0));
    exit(EXIT_FAILURE);
}

/*****************************************************************************
 * handle_stream_status
 *****************************************************************************/
static void handle_stream_status(uint8_t *p_tlv)
{
    uint16_t i_wanted_channelid = ecmg_find_channelid(p_tlv, 0);
    uint16_t i_streamid = ecmg_find_streamid(p_tlv, 0);
    stream_t *p_stream;

    if (i_wanted_channelid != i_channelid) {
        send_channel_error(i_wanted_channelid, 0x6);
        exit(EXIT_FAILURE);
    }

    if ((p_stream = find_stream(i_streamid)) == NULL) {
        send_stream_error(i_streamid, 0x7);
        exit(EXIT_FAILURE);
    }

    if (i_version >= 2 && tlv_count_param(p_tlv, ECMG_PARAM_ECMID) != 1) {
        send_stream_error(i_streamid, 0x10);
        exit(EXIT_FAILURE);
    }

    p_stream->b_init = false;
}

/*****************************************************************************
 * handle_stream_test
 *****************************************************************************/
static void handle_stream_test(uint8_t *p_tlv)
{
    uint16_t i_wanted_channelid = ecmg_find_channelid(p_tlv, 0);
    uint16_t i_streamid = ecmg_find_streamid(p_tlv, 0);
    stream_t *p_stream;

    if (i_wanted_channelid != i_channelid) {
        send_channel_error(i_wanted_channelid, 0x6);
        exit(EXIT_FAILURE);
    }

    if ((p_stream = find_stream(i_streamid)) == NULL) {
        send_stream_error(i_streamid, 0x7);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "stream test received\n");
    exit(EXIT_FAILURE);
}

/*****************************************************************************
 * handle_stream_error
 *****************************************************************************/
static void handle_stream_error(uint8_t *p_tlv)
{
    uint16_t i_wanted_channelid = ecmg_find_channelid(p_tlv, 0);
    uint16_t i_streamid = ecmg_find_streamid(p_tlv, 0);

    fprintf(stderr,
            "receiving error channel ID=0x%hu stream ID=0x%hx error=0x%hx\n",
            i_wanted_channelid, i_streamid, ecmg_find_errorstatus(p_tlv, 0));
}

/*****************************************************************************
 * handle_ecm
 *****************************************************************************/
static void handle_ecm(uint8_t *p_tlv)
{
    uint16_t i_wanted_channelid = ecmg_find_channelid(p_tlv, 0);
    uint16_t i_streamid = ecmg_find_streamid(p_tlv, 0);
    stream_t *p_stream;
    uint16_t i_cp_number;
    uint8_t *p_ecm;
    uint16_t i_ecm_length;

    if (i_wanted_channelid != i_channelid) {
        send_channel_error(i_wanted_channelid, 0x6);
        return;
    }

    if ((p_stream = find_stream(i_streamid)) == NULL) {
        send_stream_error(i_streamid, 0x7);
        return;
    }

    i_cp_number = ecmg_find_cpnumber(p_tlv, 0);
    if (i_cp_number != p_stream->i_cp_number)
        fprintf(stderr, "late ECM packet\n");

    if (i_sectiontspkt != 0) return;
    p_ecm = tlv_find_data(p_tlv, ECMG_PARAM_ECM, 0, &i_ecm_length);

    if (i_ecm_length < PSI_HEADER_SIZE) {
        fprintf(stderr, "too short ECM (%hu)\n", i_ecm_length);
        return;
    }
    if (i_ecm_length != psi_get_length(p_ecm) + PSI_HEADER_SIZE) {
        fprintf(stderr, "ECM size mismatch\n");
        return;
    }
    if (!psi_validate(p_ecm))
        fprintf(stderr, "invalid PSI section\n");
}

/*****************************************************************************
 * Main loop
 *****************************************************************************/
int main(int i_argc, char **ppsz_argv)
{
    if (i_argc != 3) {
        fprintf(stderr, "usage: %s <# streams> <crypto-period (s)> < < <input stream> > <output stream>\n",
                ppsz_argv[0]);
        return EXIT_FAILURE;
    }

    i_wanted_streams = atoi(ppsz_argv[1]);
    i_period = atoi(ppsz_argv[2]);
    i_last_time = time(NULL);

    srand(time(NULL));
    i_channelid = rand();
    i_supercasid = rand();
    send_channel_setup();

    for ( ; ; ) {
        uint8_t *p_tlv_h = malloc(TLVH_HEADER_SIZE + TLV_HEADER_SIZE);
        uint8_t *p_tlv = tlvh_get_tlv(p_tlv_h);
        uint16_t i_type;
        fd_set rset;
        struct timeval timeout;
        int i_ret;

        FD_ZERO(&rset);
        FD_SET(STDIN_FILENO, &rset);
        timeout.tv_sec = SELECT_TIMEOUT;
        timeout.tv_usec = 0;

        if ((i_ret = select(STDIN_FILENO + 1, &rset, NULL, NULL, &timeout)) < 0)
            return EXIT_FAILURE;
        if (!i_ret) {
            goto no_packet;
        }

        if (read_wrapper(p_tlv_h, TLVH_HEADER_SIZE + TLV_HEADER_SIZE) <= 0)
            return EXIT_FAILURE;

        p_tlv_h = realloc(p_tlv_h, TLVH_HEADER_SIZE + TLV_HEADER_SIZE
                                    + tlv_get_length(p_tlv));
        p_tlv = tlvh_get_tlv(p_tlv_h);
        if (read_wrapper(p_tlv + TLV_HEADER_SIZE, tlv_get_length(p_tlv)) <= 0)
            return EXIT_FAILURE;

        if (tlvh_get_version(p_tlv_h) != i_version) {
            send_channel_error(i_channelid, 0x2);
            free(p_tlv_h);
            exit(EXIT_FAILURE);
        }

        i_type = tlv_get_type(p_tlv);

        if (!tlv_validate(p_tlv)) {
            send_channel_error(i_channelid, 0x1);
            free(p_tlv_h);
            exit(EXIT_FAILURE);
        }

        if (!ecmg_validate(p_tlv)) {
            send_channel_error(i_channelid, 0xf);
            free(p_tlv_h);
            exit(EXIT_FAILURE);
        }

        if (b_init && i_type != ECMG_TYPE_CHANNEL_STATUS) {
            send_channel_error(i_channelid, 0x6);
            free(p_tlv_h);
            exit(EXIT_FAILURE);
        }

        switch (i_type) {
            case ECMG_TYPE_CHANNEL_STATUS:
                handle_channel_status(p_tlv);
                break;

            case ECMG_TYPE_CHANNEL_TEST:
                handle_channel_test(p_tlv);
                break;

            case ECMG_TYPE_CHANNEL_ERROR:
                handle_channel_error(p_tlv);
                break;

            case ECMG_TYPE_STREAM_STATUS:
                handle_stream_status(p_tlv);
                break;

            case ECMG_TYPE_STREAM_TEST:
                handle_stream_test(p_tlv);
                break;

            case ECMG_TYPE_STREAM_ERROR:
                handle_stream_error(p_tlv);
                break;

            case ECMG_TYPE_ECM:
                handle_ecm(p_tlv);
                break;

            default:
                send_channel_error(i_channelid, 0x3);
                break;
        }

no_packet:
        free(p_tlv_h);

        if (!b_init) {
            time_t i_time = time(NULL);

            if (i_nb_streams < i_wanted_streams)
                create_stream();

            while (i_last_time < i_time) {
                i_last_time++;
                check_cw();
            }
        }
    }

    return EXIT_SUCCESS;
}
