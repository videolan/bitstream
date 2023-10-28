/*****************************************************************************
 * rfc8331.h: RTP Payload for
       Society of Motion Picture and Television Engineers (SMPTE)
                        ST 291-1 Ancillary Data
 *****************************************************************************
 * Copyright (C) 2023 Open Broadcast Systems Ltd.
 *
 * Authors: Kieran Kunhya <kierank@obe.tv>
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
 *  - IETF RFC 8331 RTP Payload for
       Society of Motion Picture and Television Engineers (SMPTE)
                        ST 291-1 Ancillary Data
 */

#ifndef __BITSTREAM_IETF_RFC8331_H__
#define __BITSTREAM_IETF_RFC8331_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */

#ifdef __cplusplus
extern "C"
{
#endif

#define RFC_8331_HEADER_LEN 8

#define RFC_8331_F_PROGRESSIVE 0
#define RFC_8331_F_FIELD_1 2
#define RFC_8331_F_FIELD_2 3

/*  
 * 0                   1                   2                   3
 * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   Extended Sequence Number    |           Length              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | ANC_Count     | F |                reserved                   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

static inline void rfc8331_set_extended_sequence_number(uint8_t *buf, uint16_t number)
{
    buf[0] = (number >> 8) & 0xff;
    buf[1] = number & 0xff;
}

static inline uint16_t rfc8331_get_extended_sequence_number(const uint8_t *buf)
{
    return (buf[0] << 8) | buf[1];
}

static inline void rfc8331_set_length(uint8_t *buf, uint16_t length)
{
    buf[2] = (length >> 8) & 0xff;
    buf[3] = length & 0xff;
}

static inline uint16_t rfc8331_get_length(const uint8_t *buf)
{
    return (buf[2] << 8) | buf[3];
}

static inline void rfc8331_set_anc_count(uint8_t *buf, uint8_t anc_count)
{
    buf[4] = anc_count;
}

static inline uint8_t rfc8331_get_anc_count(const uint8_t *buf)
{
    return buf[4];
}

static inline void rfc8331_set_f(uint8_t *buf, uint8_t f)
{
    buf[5] = (f & 0x3) << 6;
    buf[6] = 0;
    buf[7] = 0;
}

static inline uint8_t rfc8331_get_f(const uint8_t *buf)
{
    return buf[5] >> 6;
}

#ifdef __cplusplus
}
#endif

#endif
