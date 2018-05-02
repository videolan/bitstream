/*****************************************************************************
 * rtp3611.h: RTP Control Protocol Extended Reports (RTCP XR)
 *****************************************************************************
 * Copyright (C) 2018 Open Broadcast Systems Ltd.
 *
 * Authors: Rafaël Carré <funman@videolam.org>
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
 * Normative references:
 *  - IETF RFC 3611
 */

#ifndef __BITSTREAM_IETF_RTCP3611_H__
#define __BITSTREAM_IETF_RTCP3611_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */

#ifdef __cplusplus
extern "C"
{
#endif

#define RTCP_PT_XR 207
#define RTCP_XR_HEADER_SIZE 8

/*  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |V=2|P|reserved |   PT=XR=207   |             length            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                              SSRC                             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * :                         report blocks                         :
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

static inline void rtcp_xr_set_ssrc_sender(uint8_t *p_rtcp_xr,
                                               const uint8_t pi_ssrc[4])
{
    p_rtcp_xr[4] = pi_ssrc[0];
    p_rtcp_xr[5] = pi_ssrc[1];
    p_rtcp_xr[6] = pi_ssrc[2];
    p_rtcp_xr[7] = pi_ssrc[3];
}

static inline void rtcp_xr_get_ssrc_sender(const uint8_t *p_rtcp_xr,
                                               uint8_t pi_ssrc[4])
{
    pi_ssrc[0] = p_rtcp_xr[4];
    pi_ssrc[1] = p_rtcp_xr[5];
    pi_ssrc[2] = p_rtcp_xr[6];
    pi_ssrc[3] = p_rtcp_xr[7];
}

static inline uint8_t rtcp_xr_get_bt(const uint8_t *p_rtcp_xr_b)
{
    return p_rtcp_xr_b[0];
}

static inline void rtcp_xr_set_bt(uint8_t *p_rtcp_xr_b,
                                      uint8_t bt)
{
    p_rtcp_xr_b[0] = bt;
}

static inline uint16_t rtcp_xr_get_length(const uint8_t *p_rtcp_xr_b)
{
    return (p_rtcp_xr_b[2] << 8) | p_rtcp_xr_b[3];
}

static inline void rtcp_xr_set_length(uint8_t *p_rtcp_xr_b,
                                      uint16_t length)
{
    p_rtcp_xr_b[2] = length >> 8;
    p_rtcp_xr_b[3] = length & 0xff;
}

/* Loss RLE Report Block
 * TODO */
#define RTCP_XR_LRLER_BT 1

/* Duplicate RLE Report Block
 * TODO */
#define RTCP_XR_DRLER_BT 2

/* Packet Receipt Times Report Block
 * TODO */
#define RTCP_XR_PRTR_BT 3

/* Receiver Reference Time Report Block */
#define RTCP_XR_RRTP_BT 4
#define RTCP_XR_RRTP_SIZE 12

static inline void rtcp_xr_rrtp_set_reserved(uint8_t *p_rtcp_xr_rrtp)
{
    p_rtcp_xr_rrtp[1] = 0;
}

static inline uint64_t rtcp_xr_rrtp_get_ntp(const uint8_t *p_rtcp_xr_b)
{
    return ((uint64_t)p_rtcp_xr_b[4] << 56) | ((uint64_t)p_rtcp_xr_b[5] << 48) |
        ((uint64_t)p_rtcp_xr_b[6]  << 40) | ((uint64_t)p_rtcp_xr_b[7] << 32) |
        ((uint64_t)p_rtcp_xr_b[8]  << 24) | ((uint64_t)p_rtcp_xr_b[9] << 16) |
        ((uint64_t)p_rtcp_xr_b[10] <<  8) |  (uint64_t)p_rtcp_xr_b[11];
}

static inline void rtcp_xr_rrtp_set_ntp(uint8_t *p_rtcp_xr_b,
                                      uint64_t ntp)
{
    p_rtcp_xr_b[4]  = (ntp >> 56) & 0xff;
    p_rtcp_xr_b[5]  = (ntp >> 48) & 0xff;
    p_rtcp_xr_b[6]  = (ntp >> 40) & 0xff;
    p_rtcp_xr_b[7]  = (ntp >> 32) & 0xff;
    p_rtcp_xr_b[8]  = (ntp >> 24) & 0xff;
    p_rtcp_xr_b[9]  = (ntp >> 16) & 0xff;
    p_rtcp_xr_b[10] = (ntp >>  8) & 0xff;
    p_rtcp_xr_b[11] = (ntp      ) & 0xff;
}

/* DLRR Report Block */
#define RTCP_XR_DLRR_BT 5
#define RTCP_XR_DLRR_SIZE 16

static inline void rtcp_xr_dlrr_set_reserved(uint8_t *p_rtcp_xr_dlrr)
{
    p_rtcp_xr_dlrr[1] = 0;
}

static inline void rtcp_xr_dlrr_set_ssrc_receiver(uint8_t *p_rtcp_xr_dlrr,
                                               const uint8_t pi_ssrc[4])
{
    p_rtcp_xr_dlrr[4] = pi_ssrc[0];
    p_rtcp_xr_dlrr[5] = pi_ssrc[1];
    p_rtcp_xr_dlrr[6] = pi_ssrc[2];
    p_rtcp_xr_dlrr[7] = pi_ssrc[3];
}

static inline void rtcp_xr_dlrr_get_ssrc_receiver(const uint8_t *p_rtcp_xr_dlrr,
                                               uint8_t pi_ssrc[4])
{
    pi_ssrc[0] = p_rtcp_xr_dlrr[4];
    pi_ssrc[1] = p_rtcp_xr_dlrr[5];
    pi_ssrc[2] = p_rtcp_xr_dlrr[6];
    pi_ssrc[3] = p_rtcp_xr_dlrr[7];
}

static inline uint32_t rtcp_xr_dlrr_get_lrr(const uint8_t *p_rtcp_xr_dlrr)
{
    return (p_rtcp_xr_dlrr[8] << 24) | (p_rtcp_xr_dlrr[9] << 16) |
        (p_rtcp_xr_dlrr[10] << 8) | p_rtcp_xr_dlrr[11];
}

static inline void rtcp_xr_dlrr_set_lrr(uint8_t *p_rtcp_xr_dlrr,
                                      uint32_t lrr)
{
    p_rtcp_xr_dlrr[8]  = (lrr >> 24) & 0xff;
    p_rtcp_xr_dlrr[9]  = (lrr >> 16) & 0xff;
    p_rtcp_xr_dlrr[10] = (lrr >>  8) & 0xff;
    p_rtcp_xr_dlrr[11] = (lrr      ) & 0xff;
}

static inline uint32_t rtcp_xr_dlrr_get_dlrr(const uint8_t *p_rtcp_xr_dlrr)
{
    return (p_rtcp_xr_dlrr[12] << 24) | (p_rtcp_xr_dlrr[13] << 16) |
        (p_rtcp_xr_dlrr[14] << 8) | p_rtcp_xr_dlrr[15];
}

static inline void rtcp_xr_dlrr_set_dlrr(uint8_t *p_rtcp_xr_dlrr,
                                      uint32_t lrr)
{
    p_rtcp_xr_dlrr[12] = (lrr >> 24) & 0xff;
    p_rtcp_xr_dlrr[13] = (lrr >> 16) & 0xff;
    p_rtcp_xr_dlrr[14] = (lrr >>  8) & 0xff;
    p_rtcp_xr_dlrr[15] = (lrr      ) & 0xff;
}

/* Statistics Summary Report Block
 * TODO */
#define RTCP_XR_SSR_BT 6

/* VoIP Metrics Report Block
 * TODO */
#define RTCP_XR_VMR_BT 7

#ifdef __cplusplus
}
#endif

#endif
