/*****************************************************************************
 * dvb_ecmg.c: Example of a basic DVB Simulcrypt ECMG server (ETSI TS 103 197)
 *****************************************************************************
 * Copyright (C) 2010 VideoLAN
 * $Id$
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 *****************************************************************************/

/*
 * You usually want to start this from xinetd, like:
 *
 * service dvb_ecmg
 * {
 *      port        = 6900
 *      disable     = no
 *      type        = UNLISTED
 *      id          = dvb_ecmg
 *      socket_type = stream
 *      protocol    = tcp
 *      user        = nobody
 *      wait        = no
 *      server      = /usr/local/bin/dvb_ecmg.sh
 * }
 *
 * With the following dvb_ecmg.sh wrapper:
 *
 * #!/bin/sh
 * mkfifo /tmp/dvb_ecmg_stderr.$$
 * logger -t "dvb_ecmg[$$]" < /tmp/dvb_ecmg_stderr.$$ &
 * /usr/local/bin/dvb_ecmg 2>/tmp/dvb_ecmg_stderr.$$
 * rm /tmp/dvb_ecmg_stderr.$$
 */


#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>

#include <bitstream/dvb/sim.h>
#include <bitstream/mpeg/psi.h>

/* normative size is 64 ko, but we will not output messages longer than this */
#define MAX_TLV_SIZE    (1024 - TLV_HEADER_SIZE)
#define SELECT_TIMEOUT  10 /* s */
#define DELAY_START     200 /* ms */
#define DELAY_STOP      0
#define REP_PERIOD      300 /* ms */
#define MAX_STREAMS     500
#define MIN_CP_DUR      3 /* 100 ms */
#define LEAD_CW         1
#define CW_PER_MSG      2
#define MAX_COMP_TIME   100 /* ms */

/*****************************************************************************
 * Local declarations
 *****************************************************************************/
typedef struct stream_t {
    uint16_t i_streamid;
    uint16_t i_ecmid;
    uint16_t i_cp_duration;
} stream_t;

static stream_t **pp_streams = NULL;
static int i_nb_streams = 0;

static bool b_init = true;
static uint8_t i_version = 1;
static uint16_t i_channelid = 0;
static uint32_t i_supercasid;

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
            fprintf(stderr, "read end-of-file, aborting");
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
 * send_channel_status
 *****************************************************************************/
static void send_channel_status(void)
{
    uint8_t p_tlv_h[MAX_TLV_SIZE];
    uint8_t *p_tlv = tlvh_get_tlv(p_tlv_h);
    uint8_t *p_tlv_n;

    tlvh_set_version(p_tlv_h, i_version);
    ecmg_init(p_tlv);
    tlv_set_type(p_tlv, ECMG_TYPE_CHANNEL_STATUS);
    /* length will be written at the end */
    tlv_set_length(p_tlv, MAX_TLV_SIZE - TLV_HEADER_SIZE - TLVH_HEADER_SIZE);

    ecmg_append_channelid(p_tlv, i_channelid);
    ecmg_append_sectiontspkt(p_tlv, 0x0); /* sections */
    ecmg_append_delaystart(p_tlv, DELAY_START);
    ecmg_append_delaystop(p_tlv, DELAY_STOP);
    ecmg_append_repperiod(p_tlv, REP_PERIOD);
    ecmg_append_maxstreams(p_tlv, MAX_STREAMS);
    ecmg_append_mincpdur(p_tlv, MIN_CP_DUR);
    ecmg_append_leadcw(p_tlv, LEAD_CW);
    ecmg_append_cwpermsg(p_tlv, CW_PER_MSG);
    ecmg_append_maxcomptime(p_tlv, MAX_COMP_TIME);

    p_tlv_n = tlv_find_param(p_tlv, TLV_PARAM_EMPTY, 0);
    tlv_set_length(p_tlv, p_tlv_n - p_tlv - TLV_HEADER_SIZE);
    write_wrapper(p_tlv_h, p_tlv_n - p_tlv_h);
}

/*****************************************************************************
 * send_channel_test
 *****************************************************************************/
static void send_channel_test(void)
{
    uint8_t p_tlv_h[MAX_TLV_SIZE];
    uint8_t *p_tlv = tlvh_get_tlv(p_tlv_h);
    uint8_t *p_tlv_n;

    fprintf(stderr, "sending test on channel ID=0x%hx\n", i_channelid);

    tlvh_set_version(p_tlv_h, i_version);
    ecmg_init(p_tlv);
    tlv_set_type(p_tlv, ECMG_TYPE_CHANNEL_TEST);
    /* length will be written at the end */
    tlv_set_length(p_tlv, MAX_TLV_SIZE - TLV_HEADER_SIZE - TLVH_HEADER_SIZE);

    ecmg_append_channelid(p_tlv, i_channelid);

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
 * send_stream_status
 *****************************************************************************/
static void send_stream_status(stream_t *p_stream)
{
    uint8_t p_tlv_h[MAX_TLV_SIZE];
    uint8_t *p_tlv = tlvh_get_tlv(p_tlv_h);
    uint8_t *p_tlv_n;

    tlvh_set_version(p_tlv_h, i_version);
    ecmg_init(p_tlv);
    tlv_set_type(p_tlv, ECMG_TYPE_STREAM_STATUS);
    /* length will be written at the end */
    tlv_set_length(p_tlv, MAX_TLV_SIZE - TLV_HEADER_SIZE - TLVH_HEADER_SIZE);

    ecmg_append_channelid(p_tlv, i_channelid);
    ecmg_append_streamid(p_tlv, p_stream->i_streamid);
    if (i_version >= 2)
        ecmg_append_ecmid(p_tlv, p_stream->i_ecmid);
    ecmg_append_accesscritmode(p_tlv, true);

    p_tlv_n = tlv_find_param(p_tlv, TLV_PARAM_EMPTY, 0);
    tlv_set_length(p_tlv, p_tlv_n - p_tlv - TLV_HEADER_SIZE);
    write_wrapper(p_tlv_h, p_tlv_n - p_tlv_h);
}

/*****************************************************************************
 * send_stream_close
 *****************************************************************************/
static void send_stream_close(stream_t *p_stream)
{
    uint8_t p_tlv_h[MAX_TLV_SIZE];
    uint8_t *p_tlv = tlvh_get_tlv(p_tlv_h);
    uint8_t *p_tlv_n;

    tlvh_set_version(p_tlv_h, i_version);
    ecmg_init(p_tlv);
    tlv_set_type(p_tlv, ECMG_TYPE_STREAM_CLOSERESP);
    /* length will be written at the end */
    tlv_set_length(p_tlv, MAX_TLV_SIZE - TLV_HEADER_SIZE - TLVH_HEADER_SIZE);

    ecmg_append_channelid(p_tlv, i_channelid);
    ecmg_append_streamid(p_tlv, p_stream->i_streamid);

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
 * send_ecm
 *****************************************************************************/
static void send_ecm(stream_t *p_stream, uint16_t i_cp_number,
                     const uint8_t *p_ecm, uint16_t i_length)
{
    uint8_t p_tlv_h[MAX_TLV_SIZE + i_length]; /* this is oversized */
    uint8_t *p_tlv = tlvh_get_tlv(p_tlv_h);
    uint8_t *p_tlv_n;

    tlvh_set_version(p_tlv_h, i_version);
    ecmg_init(p_tlv);
    tlv_set_type(p_tlv, ECMG_TYPE_ECM);
    /* length will be written at the end */
    tlv_set_length(p_tlv, MAX_TLV_SIZE - TLV_HEADER_SIZE - TLVH_HEADER_SIZE
                           + i_length);

    ecmg_append_channelid(p_tlv, i_channelid);
    ecmg_append_streamid(p_tlv, p_stream->i_streamid);
    ecmg_append_cpnumber(p_tlv, i_cp_number);
    tlv_append_data(p_tlv, ECMG_PARAM_ECM, p_ecm, i_length);

    p_tlv_n = tlv_find_param(p_tlv, TLV_PARAM_EMPTY, 0);
    tlv_set_length(p_tlv, p_tlv_n - p_tlv - TLV_HEADER_SIZE);
    write_wrapper(p_tlv_h, p_tlv_n - p_tlv_h);
}

/*****************************************************************************
 * build_ecm
 *****************************************************************************/
static void build_ecm(stream_t *p_stream, uint16_t i_cp_number,
                      uint8_t * const ppi_cw[2],
                      uint8_t *p_accesscrit_param)
{
    /* you will want to customize this function */
    static const uint8_t pi_xor[8] =
        {0x42, 0x12, 0x02, 0x24, 0x21, 0x20, 0x66, 0x88};
    PSI_DECLARE(p_section);
    uint8_t *pi_ecm;
    int i, j;

    psi_init(p_section, false);
    psi_set_tableid(p_section, 0x80 | (i_cp_number & 0x1));
    psi_set_length(p_section, 16);

    pi_ecm = p_section + 3;
    for (i = 0; i < 2; i++)
        for (j = 0; j < 8; j++)
            pi_ecm[i * 8 + j] = ppi_cw[i][j] ^ pi_xor[j];

    send_ecm(p_stream, i_cp_number, p_section,
             psi_get_length(p_section) + PSI_HEADER_SIZE);
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
 * handle_channel_setup
 *****************************************************************************/
static void handle_channel_setup(uint8_t *p_tlv)
{
    uint16_t i_wanted_channelid = ecmg_find_channelid(p_tlv, 0);

    if (!b_init) {
        if (i_wanted_channelid != i_channelid)
            send_channel_error(i_wanted_channelid, 0x6);
        return;
    }

    i_channelid = i_wanted_channelid;
    i_supercasid = ecmg_find_supercasid(p_tlv, 0);

    fprintf(stderr,
            "starting channel ID=0x%hx version=%hhu super CAS ID=0x%x\n",
            i_channelid, i_version, i_supercasid);
    b_init = false;

    send_channel_status();
}

/*****************************************************************************
 * handle_channel_test
 *****************************************************************************/
static void handle_channel_test(uint8_t *p_tlv)
{
    uint16_t i_wanted_channelid = ecmg_find_channelid(p_tlv, 0);

    if (i_wanted_channelid != i_channelid) {
        send_channel_error(i_wanted_channelid, 0x6);
        return;
    }

    send_channel_status();
}

/*****************************************************************************
 * handle_channel_close
 *****************************************************************************/
static void handle_channel_close(uint8_t *p_tlv)
{
    uint16_t i_wanted_channelid = ecmg_find_channelid(p_tlv, 0);
    int i;

    if (i_wanted_channelid != i_channelid) {
        send_channel_error(i_wanted_channelid, 0x6);
        return;
    }

    fprintf(stderr, "stopping channel ID=0x%x\n", i_channelid);

    b_init = true;

    for (i = 0; i < i_nb_streams; i++)
        free(pp_streams[i]);
    free(pp_streams);
    pp_streams = NULL;
    i_nb_streams = 0;
}

/*****************************************************************************
 * handle_channel_error
 *****************************************************************************/
static void handle_channel_error(uint8_t *p_tlv)
{
    uint16_t i_wanted_channelid = ecmg_find_channelid(p_tlv, 0);

    fprintf(stderr, "receiving error channel ID=0x%hu error=0x%hx\n",
            i_wanted_channelid, ecmg_find_errorstatus(p_tlv, 0));
}

/*****************************************************************************
 * handle_stream_setup
 *****************************************************************************/
static void handle_stream_setup(uint8_t *p_tlv)
{
    uint16_t i_wanted_channelid = ecmg_find_channelid(p_tlv, 0);
    uint16_t i_streamid = ecmg_find_streamid(p_tlv, 0);
    stream_t *p_stream;
    int i;

    if (i_wanted_channelid != i_channelid) {
        send_channel_error(i_wanted_channelid, 0x6);
        return;
    }

    if ((p_stream = find_stream(i_streamid)) != NULL) {
        send_stream_error(i_streamid, 0x14);
        return;
    }

    if (i_version >= 2 && tlv_count_param(p_tlv, ECMG_PARAM_ECMID) != 1) {
        send_stream_error(i_streamid, 0x10);
        return;
    }

    p_stream = malloc(sizeof(stream_t));

    for (i = 0; i < i_nb_streams; i++) {
        if (pp_streams[i] == NULL) {
            pp_streams[i] = p_stream;
            break;
        }
    }

    if (i == i_nb_streams) {
        pp_streams = realloc(pp_streams, ++i_nb_streams * sizeof(stream_t *));
        pp_streams[i_nb_streams - 1] = p_stream;
    }

    p_stream->i_streamid = i_streamid;
    if (i_version >= 2)
        p_stream->i_ecmid = ecmg_find_ecmid(p_tlv, 0);
    else
        p_stream->i_ecmid = 0;
    p_stream->i_cp_duration = ecmg_find_nomcpdur(p_tlv, 0);
    fprintf(stderr,
            "starting new stream id=0x%hx ecmid=0x%hx CP duration=%u ms\n",
            p_stream->i_streamid, p_stream->i_ecmid,
            p_stream->i_cp_duration * 100);

    send_stream_status(p_stream);
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
        return;
    }

    if ((p_stream = find_stream(i_streamid)) == NULL) {
        send_stream_error(i_streamid, 0x7);
        return;
    }

    send_stream_status(p_stream);
}

/*****************************************************************************
 * handle_stream_close
 *****************************************************************************/
static void handle_stream_close(uint8_t *p_tlv)
{
    uint16_t i_wanted_channelid = ecmg_find_channelid(p_tlv, 0);
    uint16_t i_streamid = ecmg_find_streamid(p_tlv, 0);
    stream_t *p_stream;
    int i;

    if (i_wanted_channelid != i_channelid) {
        send_channel_error(i_wanted_channelid, 0x6);
        return;
    }

    if ((p_stream = find_stream(i_streamid)) == NULL) {
        send_stream_error(i_streamid, 0x7);
        return;
    }

    fprintf(stderr, "stopping stream ID=0x%hx\n", i_streamid);
    send_stream_close(p_stream);

    for (i = 0; i < i_nb_streams; i++) {
        if (pp_streams[i] == p_stream) {
            pp_streams[i] = NULL;
            break;
        }
    }

    free(p_stream);
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
 * handle_cw
 *****************************************************************************/
static void handle_cw(uint8_t *p_tlv)
{
    uint16_t i_wanted_channelid = ecmg_find_channelid(p_tlv, 0);
    uint16_t i_streamid = ecmg_find_streamid(p_tlv, 0);
    stream_t *p_stream;
    uint16_t i_cp_number, i_cw_cp_number;
    uint16_t i_cw_length;
    uint8_t *p_cw_param, *p_accesscrit_param;
    uint8_t *ppi_cw[2] = {NULL, NULL};

    if (i_wanted_channelid != i_channelid) {
        send_channel_error(i_wanted_channelid, 0x6);
        return;
    }

    if ((p_stream = find_stream(i_streamid)) == NULL) {
        send_stream_error(i_streamid, 0x7);
        return;
    }

    if (tlv_count_param(p_tlv, ECMG_PARAM_CWENCRYPT)) {
        send_stream_error(i_streamid, 0xe);
        return;
    }

    if (tlv_count_param(p_tlv, ECMG_PARAM_CPCWCOMB) != 2) {
        send_stream_error(i_streamid, 0xb);
        return;
    }

    i_cp_number = ecmg_find_cpnumber(p_tlv, 0);

    p_cw_param = tlv_find_data(p_tlv, ECMG_PARAM_CPCWCOMB, 0, &i_cw_length);
    if (i_cw_length != 10) {
        send_stream_error(i_streamid, 0xf);
        return;
    }
    i_cw_cp_number = ecmgcw_get_cpnum(p_cw_param);
    ppi_cw[i_cw_cp_number & 0x1] = ecmgcw_get_cw(p_cw_param);

    p_cw_param = tlv_find_data(p_tlv, ECMG_PARAM_CPCWCOMB, 1, &i_cw_length);
    if (i_cw_length != 10) {
        send_stream_error(i_streamid, 0xf);
        return;
    }
    i_cw_cp_number = ecmgcw_get_cpnum(p_cw_param);
    ppi_cw[i_cw_cp_number & 0x1] = ecmgcw_get_cw(p_cw_param);

    if (ppi_cw[0] == NULL || ppi_cw[1] == NULL) {
        send_stream_error(i_streamid, 0xb);
        return;
    }

    p_accesscrit_param = tlv_find_param(p_tlv, ECMG_PARAM_ACCESSCRIT, 0);

    build_ecm(p_stream, i_cp_number, ppi_cw, p_accesscrit_param);
}

/*****************************************************************************
 * Main loop
 *****************************************************************************/
int main(int i_argc, char **ppsz_argv)
{
    if (i_argc > 1) {
        fprintf(stderr, "usage: %s < <input stream> > <output stream>\n",
                ppsz_argv[0]);
        return EXIT_FAILURE;
    }

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
            if (b_init) return EXIT_FAILURE;
            /* timeout - send a packet so that the communication is reset
             * if it is really dead */
            send_channel_test();
            continue;
        }

        if (read_wrapper(p_tlv_h, TLVH_HEADER_SIZE + TLV_HEADER_SIZE) <= 0)
            return EXIT_FAILURE;

        p_tlv_h = realloc(p_tlv_h, TLVH_HEADER_SIZE + TLV_HEADER_SIZE
                                    + tlv_get_length(p_tlv));
        p_tlv = tlvh_get_tlv(p_tlv_h);
        if (read_wrapper(p_tlv + TLV_HEADER_SIZE, tlv_get_length(p_tlv)) <= 0)
            return EXIT_FAILURE;

        if ((b_init && tlvh_get_version(p_tlv_h) > 3) ||
            (!b_init && tlvh_get_version(p_tlv_h) != i_version)) {
            send_channel_error(i_channelid, 0x2);
            free(p_tlv_h);
            continue;
        }

        i_type = tlv_get_type(p_tlv);

        if (!tlv_validate(p_tlv)) {
            send_channel_error(i_channelid, 0x1);
            free(p_tlv_h);
            continue;
        }

        if (!ecmg_validate(p_tlv)) {
            send_channel_error(i_channelid, 0xf);
            free(p_tlv_h);
            continue;
        }

        if (b_init && i_type != ECMG_TYPE_CHANNEL_SETUP) {
            send_channel_error(i_channelid, 0x6);
            free(p_tlv_h);
            continue;
        }

        switch (i_type) {
            case ECMG_TYPE_CHANNEL_SETUP:
                if (b_init)
                    i_version = tlvh_get_version(p_tlv_h);
                handle_channel_setup(p_tlv);
                break;

            case ECMG_TYPE_CHANNEL_TEST:
                handle_channel_test(p_tlv);
                break;

            case ECMG_TYPE_CHANNEL_CLOSE:
                handle_channel_close(p_tlv);
                break;

            case ECMG_TYPE_CHANNEL_ERROR:
                handle_channel_error(p_tlv);
                break;

            case ECMG_TYPE_STREAM_SETUP:
                handle_stream_setup(p_tlv);
                break;

            case ECMG_TYPE_STREAM_TEST:
                handle_stream_test(p_tlv);
                break;

            case ECMG_TYPE_STREAM_CLOSEREQ:
                handle_stream_close(p_tlv);
                break;

            case ECMG_TYPE_STREAM_ERROR:
                handle_stream_error(p_tlv);
                break;

            case ECMG_TYPE_CHANNEL_STATUS:
            case ECMG_TYPE_STREAM_STATUS:
                break;

            case ECMG_TYPE_CW:
                handle_cw(p_tlv);
                break;

            default:
                send_channel_error(i_channelid, 0x3);
                break;
        }

        free(p_tlv_h);
    }

    return EXIT_SUCCESS;
}
