/*****************************************************************************
 * rtp3551.h: RTP Profile for Audio and Video
 *****************************************************************************
 * Copyright (C) 2013 VideoLAN
 *
 * Authors: Benjamin Cohen <bencoh@notk.org>
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
 *  - IETF RFC 3551 RTP Profile for Audio and Video Conferences
 *                  with Minimal Control (July 2003)
 */

#ifndef __BITSTREAM_IETF_RTP3551_H__
#define __BITSTREAM_IETF_RTP3551_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include "rtp.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define RTP_TYPE_GSM         3
#define RTP_TYPE_L16        10
#define RTP_TYPE_L16MONO    11
#define RTP_TYPE_MPA        14
#define RTP_TYPE_MPV        32

static inline uint32_t rtp_3551_get_clock_rate(uint8_t i_type)
{
    /* rtp type is a 7bit positive integer, thus 0xff is invalid */
    struct {
        uint8_t i_type;
        uint32_t i_rate;
    } profiles[] = {
        { RTP_TYPE_TS,      90000 },
        { RTP_TYPE_MPA,     90000 },
        { RTP_TYPE_MPV,     90000 },
        { RTP_TYPE_L16,     44100 },
        { RTP_TYPE_L16MONO, 44100 },
        { RTP_TYPE_GSM,      8000 },

        { 0xff,             0xffff}
    };

    int i = 0;
    while(profiles[i].i_type != 0xff) {
        if (i_type == profiles[i].i_type) {
            return profiles[i].i_rate;
        }
        i++;
    }
    return 0; /* invalid clock rate for unknown type */
}

#ifdef __cplusplus
}
#endif

#endif
