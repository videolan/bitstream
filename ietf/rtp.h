/*****************************************************************************
 * rtp.h: Real-Time Protocol
 *****************************************************************************
 * Copyright (C) 2009 VideoLAN
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
 * Normative references:
 *  - IETF RFC 3550 Real-Time Protocol (July 2003)
 */

#ifndef __BITSTREAM_IETF_RTP_H__
#define __BITSTREAM_IETF_RTP_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */

#ifdef __cplusplus
extern "C"
{
#endif

#define RTP_HEADER_SIZE     12
#define RTP_TYPE_TS         33

/*
 * Reminder : RTP header
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|X|  CC   |M|     PT      |       sequence number         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           timestamp                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |           synchronization source (SSRC) identifier            |
   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
   |            contributing source (CSRC) identifiers             |
   |                             ....                              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

static inline void rtp_set_hdr(uint8_t *p_rtp)
{
    p_rtp[0] = 0x80;
}

static inline bool rtp_check_hdr(const uint8_t *p_rtp)
{
    return (p_rtp[0] & 0xc0) == 0x80;
}

static inline void rtp_set_type(uint8_t *p_rtp, uint8_t i_type)
{
    p_rtp[1] = i_type;
}

static inline uint8_t rtp_get_type(const uint8_t *p_rtp)
{
    return p_rtp[1] & 0x7f;
}

static inline void rtp_set_seqnum(uint8_t *p_rtp, uint16_t i_seqnum)
{
    p_rtp[2] = i_seqnum >> 8;
    p_rtp[3] = i_seqnum & 0xff;
}

static inline uint16_t rtp_get_seqnum(uint8_t *p_rtp)
{
    return (p_rtp[2] << 8) | p_rtp[3];
}

static inline void rtp_set_timestamp(uint8_t *p_rtp, uint32_t i_timestamp)
{
    p_rtp[4] = (i_timestamp >> 24) & 0xff;
    p_rtp[5] = (i_timestamp >> 16) & 0xff;
    p_rtp[6] = (i_timestamp >> 8) & 0xff;
    p_rtp[7] = i_timestamp & 0xff;
}

static inline uint32_t rtp_get_timestamp(uint8_t *p_rtp)
{
    return (p_rtp[4] << 24) | (p_rtp[5] << 16) | (p_rtp[6] << 8) | p_rtp[7];
}

static inline void rtp_set_ssrc(uint8_t *p_rtp, const uint8_t pi_ssrc[4])
{
    p_rtp[8] = pi_ssrc[0];
    p_rtp[9] = pi_ssrc[1];
    p_rtp[10] = pi_ssrc[2];
    p_rtp[11] = pi_ssrc[3];
}

static inline void rtp_get_ssrc(uint8_t *p_rtp, uint8_t pi_ssrc[4])
{
    pi_ssrc[0] = p_rtp[8];
    pi_ssrc[1] = p_rtp[9];
    pi_ssrc[2] = p_rtp[10];
    pi_ssrc[3] = p_rtp[11];
}

static inline uint8_t *rtp_payload(uint8_t *p_rtp)
{
    unsigned int i_size = RTP_HEADER_SIZE;
    i_size += 4 * (p_rtp[0] & 0xf);
    if (p_rtp[0] & 0x10) /* header extension */
        i_size += 4 * (1 + (p_rtp[i_size + 2] << 8) + p_rtp[i_size + 3]);
    return p_rtp + i_size;
}

#ifdef __cplusplus
}
#endif

#endif
