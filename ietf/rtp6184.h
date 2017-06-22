/*****************************************************************************
 * rtp6184.h: RTP Payload Format for H.264 Video
 *****************************************************************************
 * Copyright (c) 2015 Arnaud de Turckheim <quarium@gmail.com>
 * Copyright (C) 2017 VideoLAN
 *
 * Authors: Arnaud de Turckheim
 *          Christophe Massiot
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
 *  - IETF RFC 6184 RTP Payload Format for H.264 Video (May 2011)
 */

#ifndef __BITSTREAM_IETF_RTP6184_H__
# define __BITSTREAM_IETF_RTP6184_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define RTP_6184_CLOCKRATE          90000

#define RTP_6184_STAP_A             24
#define RTP_6184_STAP_B             25
#define RTP_6184_MTAP16             26
#define RTP_6184_MTAP24             27
#define RTP_6184_FU_A               28
#define RTP_6184_FU_B               29

#define RTP_6184_STAP_HEADER_SIZE   2

static inline void rtp_6184_stap_set_size(uint8_t *header, uint16_t size)
{
    header[0] = size >> 8;
    header[1] = size & 0xff;
}

static inline uint16_t rtp_6184_stap_get_size(const uint8_t *header)
{
    return (header[0] << 8) | header[1];
}

static inline void rtp_6184_fu_set_start(uint8_t *header)
{
    header[0] |= 0x80;
}

static inline bool rtp_6184_fu_check_start(uint8_t header)
{
    return header & 0x80;
}

static inline void rtp_6184_fu_set_end(uint8_t *header)
{
    header[0] |= 0x40;
}

static inline bool rtp_6184_fu_check_end(uint8_t header)
{
    return header & 0x40;
}

#ifdef __cplusplus
}
#endif

#endif /* !__BITSTREAM_IETF_RTP6184_H__ */
