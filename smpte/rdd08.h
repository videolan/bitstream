/*****************************************************************************
 * rdd08.h: SMPTE RDD 8 Teletext Subtitles for HDTV (aka OP-47)
 *****************************************************************************
 * Copyright (C) 2016 OpenHeadend
 *
 * Authors: Christophe Massiot <cmassiot@openheadend.tv>
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
 *  - SMPTE RDD 8-2008
 */

#ifndef __BITSTREAM_SMPTE_RDD08_H__
#define __BITSTREAM_SMPTE_RDD08_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * SMPTE RDD 8 Subtitling Distribution Packet
 *****************************************************************************/
#define RDD08SDP_HEADER_SIZE        9
#define RDD08SDP_FOOTER_SIZE        4
#define RDD08SDP_B_SIZE             45

#define RDD08SDP_IDENT1             0x0151
#define RDD08SDP_IDENT2             0x0115
#define RDD08SDP_FOOTER             0x0274

#define RDD08SDP_FORMAT_WST         0x0102
#define RDD08SDP_FRAMING_CODE       0x0227

static inline uint8_t rdd08sdp_get_length(const uint16_t *p_rdd08)
{
    return p_rdd08[2] & 0xff;
}

static inline uint16_t rdd08sdp_get_format(const uint16_t *p_rdd08)
{
    return p_rdd08[3];
}

static inline uint8_t rdd08sdp_get_a(const uint16_t *p_rdd08, uint8_t n)
{
    return p_rdd08[4 + n] & 0xff;
}

static inline uint8_t rdd08sdpa_get_line(uint8_t i_rdd08a)
{
    return i_rdd08a & 0x1f;
}

static inline bool rdd08sdpa_get_field(uint8_t i_rdd08a)
{
    return i_rdd08a & 0x80;
}

static inline uint16_t *rdd08sdp_get_b(const uint16_t *p_rdd08, uint8_t n)
{
    const uint16_t *b = p_rdd08 + RDD08SDP_HEADER_SIZE;
    unsigned int i;
    for (i = 0; i < n; i++)
        if (rdd08sdp_get_a(p_rdd08, i))
            b += RDD08SDP_B_SIZE;
    return (uint16_t *)b;
}

static inline uint16_t rdd08sdp_get_footer(const uint16_t *p_rdd08)
{
    uint8_t i_length = rdd08sdp_get_length(p_rdd08);
    return p_rdd08[i_length - 4];
}

static inline uint16_t rdd08sdp_get_counter(const uint16_t *p_rdd08)
{
    uint8_t i_length = rdd08sdp_get_length(p_rdd08);
    return ((p_rdd08[i_length - 3] & 0xff) << 8) |
           (p_rdd08[i_length - 2] & 0xff);
}

static inline bool rdd08sdp_check_cs(const uint16_t *p_rdd08)
{
    uint8_t i_length = rdd08sdp_get_length(p_rdd08);
    uint8_t i_cs = 0;
    unsigned int i;
    for (i = 0; i < i_length; i++)
        i_cs += p_rdd08[i] & 0xff;
    return !i_cs;
}

static inline bool rdd08sdp_validate(const uint16_t *p_rdd08)
{
    uint8_t i_length = rdd08sdp_get_length(p_rdd08);
    const uint16_t *b = rdd08sdp_get_b(p_rdd08, 5);
    return b <= p_rdd08 + i_length - 4;
}

#ifdef __cplusplus
}
#endif

#endif
