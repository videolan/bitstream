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

static inline uint16_t rfc4175_get_extended_sequence_number(uint8_t *buf)
{
    return (buf[0] << 8) | buf[1];
}

static inline void rfc4175_set_line_1_length(uint8_t *buf, uint16_t length)
{
    buf[2] = (length >> 8) & 0xff;
    buf[3] = length & 0xff;
}

static inline uint16_t rfc4175_get_line_1_length(uint8_t *buf)
{
    return (buf[2] << 8) | buf[3];
}

static inline void rfc4175_set_line_1_field_id(uint8_t *buf, uint8_t id)
{
    buf[4] |= (!!id) << 7;
}

static inline uint8_t rfc4175_get_line_1_field_id(uint8_t *buf)
{
    return buf[4] >> 7;
}

static inline void rfc4175_set_line_1_number(uint8_t *buf, uint16_t number)
{
    buf[4] |= (number >> 8) & 0x80;
    buf[5]  = number & 0xff;
}

static inline uint16_t rfc4175_get_line_1_number(uint8_t *buf)
{
    return ((buf[4] & 0x80) << 8) | buf[5];
}

static inline void rfc4175_set_line_1_continuation(uint8_t *buf, uint8_t continuation)
{
    buf[6] |= (!!continuation) << 7;
}

static inline uint8_t rfc4175_get_line_1_continuation(uint8_t *buf)
{
    return buf[6] >> 7;
}

static inline void rfc4175_set_line_1_offset(uint8_t *buf, uint16_t offset)
{
    buf[6] |= (offset >> 8) & 0x80;
    buf[7]  = offset & 0xff;
}

static inline uint16_t rfc4175_get_line_1_offset(uint8_t *buf)
{
    return ((buf[6] & 0x80) << 8) | buf[7];
}

static inline void rfc4175_set_line_2_length(uint8_t *buf, uint16_t length)
{
    buf[8] = (length >> 8) & 0xff;
    buf[9] = length & 0xff;
}

static inline uint16_t rfc4175_get_line_2_length(uint8_t *buf)
{
    return (buf[8] << 8) | buf[9];
}

static inline void rfc4175_set_line_2_field_id(uint8_t *buf, uint8_t id)
{
    buf[10] |= (!!id) << 7;
}

static inline uint8_t rfc4175_get_line_2_field_id(uint8_t *buf)
{
    return buf[10] >> 7;
}

static inline void rfc4175_set_line_2_number(uint8_t *buf, uint16_t number)
{
    buf[11] |= (number >> 8) & 0x80;
    buf[12]  = number & 0xff;
}

static inline uint16_t rfc4175_get_line_2_number(uint8_t *buf)
{
    return ((buf[11] & 0x80) << 8) | buf[12];
}

static inline void rfc4175_set_line_2_continuation(uint8_t *buf, uint8_t continuation)
{
    buf[13] |= (!!continuation) << 7;
}

static inline uint8_t rfc4175_get_line_2_continuation(uint8_t *buf)
{
    return buf[13] >> 7;
}

static inline void rfc4175_set_line_2_offset(uint8_t *buf, uint16_t offset)
{
    buf[13] |= (offset >> 8) & 0x80;
    buf[14]  = offset & 0xff;
}

static inline uint16_t rfc4175_get_line_2_offset(uint8_t *buf)
{
    return ((buf[13] & 0x80) << 8) | buf[14];
}

#ifdef __cplusplus
}
#endif

#endif
