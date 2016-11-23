/*****************************************************************************
 * 2022_6_hbrmt.h
 *****************************************************************************
 * Copyright (C) 2015 Open Broadcast Systems Ltd
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
 *  - SMPTE 2022-6
 */

#ifndef __BITSTREAM_SMPTE_2022_6_HBRMT_H__
#define __BITSTREAM_SMPTE_2022_6_HBRMT_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */

#ifdef __cplusplus
extern "C"
{
#endif

#define HBRMT_HEADER_SIZE 8 /* plus timestamp and extension */
#define HBRMT_DATA_SIZE 1376

/*
 * Reminder : High Bit Rate Media Payload Header
    0               1               2               3
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |Ext    |F|VSID | FRCount       | R | S | FEC | CF    | RESERVE |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |  MAP  |     FRAME     |     FRAME     |SAMPLE |  FMT-RESERVE  |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |               Video timestamp (only if CF>0)                  |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |              Header extension (only if Ext > 0)               |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

static inline void smpte_hbrmt_set_ext(uint8_t *p_hbrmt, uint8_t ext)
{
    p_hbrmt[0] |= (ext & 0xf) << 4;
}

static inline uint8_t smpte_hbrmt_get_ext(const uint8_t *p_hbrmt)
{
    return p_hbrmt[0] >> 4;
}

static inline void smpte_hbrmt_set_video_source_format(uint8_t *p_hbrmt)
{
    p_hbrmt[0] |= 0x8;
}

static inline bool smpte_hbrmt_check_video_source_format(const uint8_t *p_hbrmt)
{
    return !!(p_hbrmt[0] & 0x8);
}

static inline void smpte_hbrmt_set_video_source_id(uint8_t *p_hbrmt, uint8_t vsid)
{
    p_hbrmt[0] |= (vsid & 0x7);
}

static inline uint8_t smpte_hbrmt_get_video_source_id(const uint8_t *p_hbrmt)
{
    return p_hbrmt[0] & 0x7;
}

static inline void smpte_hbrmt_set_frame_count(uint8_t *p_hbrmt, uint8_t frcount)
{
    p_hbrmt[1] = frcount;
}

static inline uint8_t smpte_hbrmt_get_frame_count(const uint8_t *p_hbrmt)
{
    return p_hbrmt[1];
}

static inline void smpte_hbrmt_set_reference_for_time_stamp(uint8_t *p_hbrmt, uint8_t r)
{
    p_hbrmt[2] |= (r & 0x3) << 6;
}

static inline uint8_t smpte_hbrmt_get_reference_for_time_stamp(const uint8_t *p_hbrmt)
{
    return p_hbrmt[2] >> 6;
}

static inline void smpte_hbrmt_set_video_payload_scrambling(uint8_t *p_hbrmt, uint8_t s)
{
    p_hbrmt[2] |= (s & 0x3) << 4;
}

static inline uint8_t smpte_hbrmt_get_video_payload_scrambling(const uint8_t *p_hbrmt)
{
    return (p_hbrmt[2] >> 4) & 0x3;
}

static inline void smpte_hbrmt_set_fec(uint8_t *p_hbrmt, uint8_t fec)
{
    p_hbrmt[2] |= (fec & 0x7) << 1;
}

static inline uint8_t smpte_hbrmt_get_fec(const uint8_t *p_hbrmt)
{
    return (p_hbrmt[2] >> 1) & 0x7;
}

static inline void smpte_hbrmt_set_clock_frequency(uint8_t *p_hbrmt, uint8_t cf)
{
    p_hbrmt[2] |= (cf >> 3) & 1;
    p_hbrmt[3] |= (cf & 0x7) << 5;
}

static inline uint8_t smpte_hbrmt_get_clock_frequency(const uint8_t *p_hbrmt)
{
    return ((p_hbrmt[2] & 1) << 3) | p_hbrmt[3] >> 5;
}

static inline void smpte_hbrmt_set_reserved(uint8_t *p_hbrmt)
{
    p_hbrmt[3] &= ~0x1f;
}

static inline void smpte_hbrmt_set_map(uint8_t *p_hbrmt, uint8_t map)
{
    p_hbrmt[4] |= (map & 0xf) << 4;
}

static inline uint8_t smpte_hbrmt_get_map(const uint8_t *p_hbrmt)
{
    return p_hbrmt[4] >> 4;
}

static inline void smpte_hbrmt_set_frame(uint8_t *p_hbrmt, uint8_t frame)
{
    p_hbrmt[4] |= (frame >> 4);
    p_hbrmt[5] |= (frame & 0xf) << 4;
}

static inline uint8_t smpte_hbrmt_get_frame(const uint8_t *p_hbrmt)
{
    return ((p_hbrmt[4] & 0xf) << 4) | (p_hbrmt[5] >> 4);
}

static inline void smpte_hbrmt_set_frate(uint8_t *p_hbrmt, uint8_t frate)
{
    p_hbrmt[5] |= (frate >> 4);
    p_hbrmt[6] |= (frate & 0xf) << 4;
}

static inline uint8_t smpte_hbrmt_get_frate(const uint8_t *p_hbrmt)
{
    return ((p_hbrmt[5] & 0xf) << 4) | (p_hbrmt[6] >> 4);
}

static inline void smpte_hbrmt_set_sample(uint8_t *p_hbrmt, uint8_t sample)
{
    p_hbrmt[6] |= (sample & 0xf);
}

static inline uint8_t smpte_hbrmt_get_sample(const uint8_t *p_hbrmt)
{
    return p_hbrmt[6] & 0xf;
}

static inline void smpte_hbrmt_set_fmt_reserve(uint8_t *p_hbrmt)
{
    p_hbrmt[7] = 0;
}

static inline void smpte_hbrmt_set_timestamp(uint8_t *p_hbrmt, uint32_t timestamp)
{
    p_hbrmt[8] =  (timestamp >> 24) & 0xff;
    p_hbrmt[9] =  (timestamp >> 16) & 0xff;
    p_hbrmt[10] = (timestamp >>  8) & 0xff;
    p_hbrmt[11] = (timestamp >>  0) & 0xff;
}

static inline uint32_t smpte_hbrmt_get_timestamp(const uint8_t *p_hbrmt)
{
    return (p_hbrmt[8] << 24) | (p_hbrmt[9] << 16) | (p_hbrmt[10] << 8) | p_hbrmt[11];
}

#ifdef __cplusplus
}
#endif

#endif
