/*****************************************************************************
 * rtp2250.h: Real-time Transport Protocol for MPEG-1/MPEG-2 audio/video
 *****************************************************************************
 * Copyright (C) 2016 VideoLAN
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
 *  - IETF RFC 2250 RTP Payload Format for MPEG1/MPEG2 Video (January 1998)
 */

#ifndef __BITSTREAM_IETF_RTP2250_H__
#define __BITSTREAM_IETF_RTP2250_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */

#include <bitstream/ietf/rtp.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Reminder : MPEG video-specific header
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |    MBZ  |T|         TR        | |N|S|B|E|  P  | | BFC | | FFC |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                                   AN              FBV     FFV
 */

#define RTP2250V_HEADER_SIZE 4

static inline void rtp2250v_set_hdr(uint8_t *p_rtpv)
{
    p_rtpv[0] = 0x0;
    p_rtpv[1] = 0x0;
    p_rtpv[2] = 0x0;
    p_rtpv[3] = 0x0;
}

static inline bool rtp2250v_check_hdr(const uint8_t *p_rtpv)
{
    return (p_rtpv[0] & 0xf8) == 0x0;
}

static inline void rtp2250v_set_mpeg2(uint8_t *p_rtpv)
{
    p_rtpv[0] |= 0x4;
}

static inline bool rtp2250v_check_mpeg2(const uint8_t *p_rtpv)
{
    return !!(p_rtpv[0] & 0x4);
}

/*
 * Reminder : MPEG-2 video-specific header extension
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |X|E|f_[0,0]|f_[0,1]|f_[1,0]|f_[1,1]| DC| PS|T|P|C|Q|V|A|R|H|G|D|
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

#define RTP2250VX_HEADER_SIZE 4

static inline void rtp2250v_set_mpeg2_hdr(uint8_t *p_rtpv)
{
    p_rtpv[4] = 0x0;
    p_rtpv[5] = 0x0;
    p_rtpv[6] = 0x0;
    p_rtpv[7] = 0x0;
}

/*
 * Reminder : MPEG Audio-specific header
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |             MBZ               |          Frag_offset          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

#define RTP2250A_HEADER_SIZE 4

static inline void rtp2250a_set_hdr(uint8_t *p_rtpa)
{
    p_rtpa[0] = 0x0;
    p_rtpa[1] = 0x0;
    p_rtpa[2] = 0x0;
    p_rtpa[3] = 0x0;
}

static inline bool rtp2250a_check_hdr(const uint8_t *p_rtpa)
{
    return p_rtpa[0] == 0x0 && p_rtpa[1] == 0x0;
}

#ifdef __cplusplus
}
#endif

#endif
