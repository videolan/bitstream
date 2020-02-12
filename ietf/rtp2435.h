/*****************************************************************************
 * rtp2435.h: RFC Profile for JPEG
 *****************************************************************************
 * Copyright (C) 2019 Josh de Kock
 *
 * Authors:  Josh de Kock <josh@itanimul.li>
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
 * RTP Payload Format for JPEG
 */
#ifndef __BITSTREAM_IETF_RTP2435_H__
#define __BITSTREAM_IETF_RTP2435_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define RTP_2435_HEADER_SIZE     8
#define RTP_2435_RESTART_HEADER_SIZE 4
#define RTP_2435_QUANTIZATION_HEADER_SIZE 4

/*
 * Reminder : JPEG header which immediately follows the RTP header
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   | Type-specific |              Fragment Offset                  |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |      Type     |       Q       |     Width     |     Height    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

static inline void rtp2435_set_ts(uint8_t *header, uint8_t ts)
{
    header[0] = ts;
}

static inline uint8_t rtp2435_get_ts(uint8_t *header)
{
    return header[0];
}

static inline void rtp2435_set_offset(uint8_t *header, uint32_t offset)
{
    header[1] = (offset & 0xFF0000) >> 16;
    header[2] = (offset & 0xFF00) >> 8;
    header[3] = offset & 0xFF;
}

static inline uint32_t rtp2435_get_offset(uint8_t *header)
{
    return header[1] << 16 | header[2] << 8 | header[3];
}

static inline void rtp2435_set_type(uint8_t *header, uint8_t type)
{
    header[4] = type;
}

static inline uint8_t rtp2435_get_type(uint8_t *header)
{
    return header[4];
}

static inline void rtp2435_set_q(uint8_t *header, uint8_t q)
{
    header[5] = q;
}

static inline uint8_t rtp2435_get_q(uint8_t *header)
{
    return header[5];
}

static inline void rtp2435_set_width(uint8_t *header, uint8_t width)
{
    header[6] = width;
}

static inline uint8_t rtp2435_get_width(uint8_t *header)
{
    return header[6];
}

static inline void rtp2435_set_height(uint8_t *header, uint8_t height)
{
    header[7] = height;
}

static inline uint8_t rtp2435_get_height(uint8_t *header)
{
    return header[7];
}

static inline uint8_t *rtp2435_restart(uint8_t *header)
{
    uint8_t t = rtp2435_get_type(header);
    if (64 <= t && t <= 127)
        return header + RTP_2435_HEADER_SIZE;
    else
        return NULL;
}

static inline uint8_t *rtp2435_quantization_table(uint8_t *header)
{
    uint8_t q = rtp2435_get_q(header);
    if (128 <= q && q <= 255) {
        uint8_t *p;
        if ((p = rtp2435_restart(header)))
            return p + RTP_2435_RESTART_HEADER_SIZE;
        else
            return header + RTP_2435_HEADER_SIZE;
    }

    return NULL;
}

/*
 * Reminder : Restart Marker header
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |       Restart Interval        |F|L|       Restart Count       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

static inline void rtp2435r_set_first(uint8_t *header)
{
    header[2] |= 0x1;
}

static inline void rtp2435r_clear_first(uint8_t *header)
{
    header[2] &= ~0x1;
}

static inline bool rtp2435r_check_first(uint8_t *header)
{
    return !!(header[2] & 0x1);
}

static inline void rtp2435r_set_last(uint8_t *header)
{
    header[2] |= 0x2;
}

static inline void rtp2435r_clear_last(uint8_t *header)
{
    header[2] &= ~0x2;
}

static inline bool rtp2435r_check_last(uint8_t *header)
{
    return !!(header[2] & 0x2);
}

static inline uint16_t rtp2435r_get_count(uint8_t *header)
{
    return (header[2] << 8 | header[3]) >> 2;
}

static inline void rtp2435r_set_count(uint8_t *header, uint16_t count)
{
    count = count << 2;
    header[2] |= count >> 8;
    header[3] = count & 0xFF;
}

static inline uint16_t rtp2435r_get_interval(uint8_t *header)
{
    return header[0] << 8 | header[1];
}

static inline void rtp2435r_set_interval(uint8_t *header, uint16_t interval)
{
    header[0] = (interval & 0xFF00) >> 8;
    header[1] = interval & 0xFF;
}

/*
 * Reminder : Quantization Table header
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |      MBZ      |   Precision   |             Length            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Quantization Table Data                    |
   |                              ...                              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

static inline void rtp2435q_set_mbz(uint8_t *header, uint8_t mbz)
{
    header[0] = mbz;
}

static inline uint8_t rtp2435q_get_mbz(uint8_t *header)
{
    return header[0];
}

static inline void rtp2435q_set_precision(uint8_t *header, uint8_t precision)
{
    header[1] = precision;
}

static inline uint8_t rtp2435q_get_precision(uint8_t *header)
{
    return header[1];
}

static inline uint16_t rtp2435q_get_length(uint8_t *header)
{
    return header[2] << 16 | header[3];
}

static inline void rtp2435q_set_length(uint8_t *header, uint16_t length)
{
    header[2] = (length & 0xFF00) >> 8;
    header[3] = length & 0xFF;
}

#ifdef __cplusplus
}
#endif

#endif /* !__BITSTREAM_IETF_RTP2435_H__ */

