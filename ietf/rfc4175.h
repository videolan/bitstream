/*****************************************************************************
 * rfc4175.h: RFC Profile for uncompressed video
 *****************************************************************************
 * Copyright (C) 2016 Open Broadcast Systems Ltd.
 *
 * Authors: Rostislav Pehlivanov <rpehlivanov@obe.tv>
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
 *  - IETF RFC 4175 Uncompressed
 */

#ifndef __BITSTREAM_IETF_RFC4175_H__
#define __BITSTREAM_IETF_RFC4175_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */

#ifdef __cplusplus
extern "C"
{
#endif

#define RFC_4175_EXT_SEQ_NUM_LEN 2
#define RFC_4175_HEADER_LEN 6 /* Note: extended sequence number not included */

/*  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   Extended Sequence Number    |            Length             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |F|          Line No            |C|           Offset            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |            Length             |F|          Line No            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |C|           Offset            |                               .
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               .
 * .                                                               .
 * .                 Two (partial) lines of video data             .
 * .                                                               .
 * +---------------------------------------------------------------+
 */


static inline void rfc4175_set_extended_sequence_number(uint8_t *buf, uint16_t number)
{
    buf[0] = (number >> 8) & 0xff;
    buf[1] = number & 0xff;
}

static inline uint16_t rfc4175_get_extended_sequence_number(const uint8_t *buf)
{
    return (buf[0] << 8) | buf[1];
}

static inline void rfc4175_set_line_length(uint8_t *buf, uint16_t length)
{
    buf[0] = (length >> 8) & 0xff;
    buf[1] = length & 0xff;
}

static inline uint16_t rfc4175_get_line_length(const uint8_t *buf)
{
    return (buf[0] << 8) | buf[1];
}

static inline void rfc4175_set_line_field_id(uint8_t *buf, uint8_t id)
{
    buf[2] |= (!!id) << 7;
}

static inline uint8_t rfc4175_get_line_field_id(const uint8_t *buf)
{
    return buf[2] >> 7;
}

static inline void rfc4175_set_line_number(uint8_t *buf, uint16_t number)
{
    buf[2] |= (number >> 8) & 0x7f;
    buf[3]  = number & 0xff;
}

static inline uint16_t rfc4175_get_line_number(const uint8_t *buf)
{
    return ((buf[2] & 0x7f) << 8) | buf[3];
}

static inline void rfc4175_set_line_continuation(uint8_t *buf, uint8_t continuation)
{
    buf[4] |= (!!continuation) << 7;
}

static inline uint8_t rfc4175_get_line_continuation(const uint8_t *buf)
{
    return buf[4] >> 7;
}

static inline void rfc4175_set_line_offset(uint8_t *buf, uint16_t offset)
{
    buf[4] |= (offset >> 8) & 0x7f;
    buf[5]  = offset & 0xff;
}

static inline uint16_t rfc4175_get_line_offset(const uint8_t *buf)
{
    return ((buf[4] & 0x7f) << 8) | buf[5];
}


#ifdef __cplusplus
}
#endif

#endif
