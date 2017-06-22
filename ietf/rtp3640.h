/*****************************************************************************
 * rtp3640.h: RTP Payload Format for Transport of MPEG-4 Elementary Streams
 *****************************************************************************
 * Copyright (C) 2017 VideoLAN
 *
 * Authors: Christophe Massiot
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
 *  - IETF RFC 3640 RTP Payload Format for Transport of MPEG-4 ES
 */

#ifndef __BITSTREAM_IETF_RTP3640_H__
# define __BITSTREAM_IETF_RTP3640_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define RTP3640_AU_HEADERS_LENGTH_SIZE  2
#define RTP3640_AU_HEADER_AAC_HBR_SIZE  2

static inline void rtp3640_set_au_headers_length(uint8_t *header, uint16_t size)
{
    header[0] = size >> 8;
    header[1] = size & 0xff;
}

static inline uint16_t rtp3640_get_au_headers_length(const uint8_t *header)
{
    return (header[0] << 8) | header[1];
}

static inline void rtp3640_set_aac_hbr_au_size(uint8_t *header, uint16_t size)
{
    header[0] = (size >> 5) & 0xff;
    header[1] &= ~0xf8;
    header[1] |= (size << 3) & 0xf8;
}

static inline uint16_t rtp3640_get_aac_hbr_au_size(const uint8_t *header)
{
    return (header[0] << 5) | (header[1] >> 3);
}


static inline void rtp3640_set_aac_hbr_au_index(uint8_t *header, uint8_t index)
{
    header[1] &= ~0x7;
    header[1] |= index & 0x7;
}

static inline uint8_t rtp3640_get_aac_hbr_au_index(const uint8_t *header)
{
    return (header[1] & 0x7);
}

#ifdef __cplusplus
}
#endif

#endif /* !__BITSTREAM_IETF_RTP3640_H__ */
