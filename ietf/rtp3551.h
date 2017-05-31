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

#define RTP_TYPE_PCMU           0
/* Reserved                     1 */
/* Reserved                     2 */
#define RTP_TYPE_GSM            3
#define RTP_TYPE_G723           4
#define RTP_TYPE_DVI4_8000      5
#define RTP_TYPE_DVI4_16000     6
#define RTP_TYPE_LPC            7
#define RTP_TYPE_PCMA           8
#define RTP_TYPE_G722           9
#define RTP_TYPE_L16            10
#define RTP_TYPE_L16MONO        11
#define RTP_TYPE_QCELP          12
#define RTP_TYPE_CN             13
#define RTP_TYPE_MPA            14
#define RTP_TYPE_G728           15
#define RTP_TYPE_DVI4_11025     16
#define RTP_TYPE_DVI4_22050     17
#define RTP_TYPE_G729           18
/* Reserved                     19 */
/* Unassigned                   20 */
/* Unassigned                   21 */
/* Unassigned                   22 */
/* Unassigned                   23 */
/* Unassigned                   24 */
#define RTP_TYPE_CELB           25
#define RTP_TYPE_JPEG           26
/* Unassigned                   27 */
#define RTP_TYPE_NV             28
/* Unassigned                   29 */
/* Unassigned                   30 */
#define RTP_TYPE_H261           31
#define RTP_TYPE_MPV            32
#define RTP_TYPE_MP2T           33
#define RTP_TYPE_H263           34
/* Unassigned                   35-71 */
/* Reserved                     72-76 */
/* Unassigned                   77-95 */
#define RTP_TYPE_DYNAMIC_FIRST  96
#define RTP_TYPE_DYNAMIC_LAST   127

static inline uint32_t rtp_3551_get_clock_rate(uint8_t i_type)
{
    switch (i_type) {
    case RTP_TYPE_PCMU:
    case RTP_TYPE_GSM:
    case RTP_TYPE_G723:
    case RTP_TYPE_DVI4_8000:
    case RTP_TYPE_LPC:
    case RTP_TYPE_PCMA:
    case RTP_TYPE_G722:
    case RTP_TYPE_QCELP:
    case RTP_TYPE_CN:
    case RTP_TYPE_G728:
    case RTP_TYPE_G729:
        return 8000;

    case RTP_TYPE_DVI4_16000:
        return 16000;

    case RTP_TYPE_L16:
    case RTP_TYPE_L16MONO:
        return 44100;

    case RTP_TYPE_DVI4_11025:
        return 11025;

    case RTP_TYPE_DVI4_22050:
        return 22050;

    case RTP_TYPE_MPA:
    case RTP_TYPE_CELB:
    case RTP_TYPE_JPEG:
    case RTP_TYPE_NV:
    case RTP_TYPE_H261:
    case RTP_TYPE_MPV:
    case RTP_TYPE_MP2T:
    case RTP_TYPE_H263:
        return 90000;
    }
    return 0; /* invalid clock rate for unknown type */
}

#ifdef __cplusplus
}
#endif

#endif
