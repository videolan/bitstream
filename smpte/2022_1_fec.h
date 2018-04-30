/*****************************************************************************
 * 2022_1_fec.h
 *****************************************************************************
 * Copyright (C) 2014 Open Broadcast Systems Ltd
 *
 * Authors: Kieran Kunhya <kierank@obe.tv
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
 *  - SMPTE 2022-1
 */

#ifndef __BITSTREAM_SMPTE_2022_1_FEC_H__
#define __BITSTREAM_SMPTE_2022_1_FEC_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */

#ifdef __cplusplus
extern "C"
{
#endif

#define SMPTE_2022_FEC_HEADER_SIZE 16

/*
 * Reminder : FEC Header
    0               1               2               3
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |       SNBase low bits         |      Length Recovery          |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |E| PT recovery |                     Mask                      |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                         TS recovery                           |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |N|D|type |index|     Offset    |       NA      |SNBase ext bits|
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

static inline void smpte_fec_set_snbase_low(uint8_t *p_fec, uint16_t snbase_low)
{
    p_fec[0] = snbase_low >> 8;
    p_fec[1] = snbase_low & 0xff;
}

static inline uint16_t smpte_fec_get_snbase_low(const uint8_t *p_fec)
{
    return (p_fec[0] << 8) | p_fec[1];
}

static inline void smpte_fec_set_length_rec(uint8_t *p_fec, uint16_t length_rec)
{
    p_fec[2] = length_rec >> 8;
    p_fec[3] = length_rec & 0xff;
}

static inline uint16_t smpte_fec_get_length_rec(const uint8_t *p_fec)
{
    return (p_fec[2] << 8) | p_fec[3];
}

static inline void smpte_fec_set_extension(uint8_t *p_fec)
{
    p_fec[4] |= 0x80;
}

static inline bool smpte_fec_check_extension(const uint8_t *p_fec)
{
    return !!(p_fec[4] & 0x80);
}

static inline void smpte_fec_set_pt_recovery(uint8_t *p_fec, uint8_t pt_recovery)
{
    p_fec[4] |= pt_recovery & 0x7f;
}

static inline uint8_t smpte_fec_get_pt_recovery(const uint8_t *p_fec)
{
    return p_fec[4] & 0x7f;
}

static inline void smpte_fec_set_mask(uint8_t *p_fec, uint32_t mask)
{
    p_fec[5] = (mask >> 16) & 0xff;
    p_fec[6] = (mask >>  8) & 0xff;
    p_fec[7] = (mask      ) & 0xff;
}

static inline uint32_t smpte_fec_get_mask(const uint8_t *p_fec)
{
    return (p_fec[5] << 16) | (p_fec[6] << 8) | p_fec[7];
}

static inline void smpte_fec_set_ts_recovery(uint8_t *p_fec, uint32_t snbase)
{
    p_fec[8] =  (snbase >> 24) & 0xff;
    p_fec[9] =  (snbase >> 16) & 0xff;
    p_fec[10] = (snbase >> 8)  & 0xff;
    p_fec[11] = (snbase >> 0)  & 0xff;
}

static inline uint32_t smpte_fec_get_ts_recovery(const uint8_t *p_fec)
{
    return (p_fec[8] << 24) | (p_fec[9] << 16) | (p_fec[10] << 8) | p_fec[11];
}

static inline void smpte_fec_clear_n(uint8_t *p_fec)
{
    p_fec[12] &= ~0x80;
}

static inline void smpte_fec_set_n(uint8_t *p_fec)
{
    p_fec[12] |= 0x80;
}

static inline bool smpte_fec_check_n(const uint8_t *p_fec)
{
    return !!(p_fec[12] & 0x80);
}

static inline void smpte_fec_clear_d(uint8_t *p_fec)
{
    p_fec[12] &= ~0x40;
}

static inline void smpte_fec_set_d(uint8_t *p_fec)
{
    p_fec[12] |= 0x40;
}

static inline bool smpte_fec_check_d(const uint8_t *p_fec)
{
    return !!(p_fec[12] & 0x40);
}

static inline void smpte_fec_set_type(uint8_t *p_fec, uint8_t type)
{
    type &= 0x7;
    p_fec[12] = (p_fec[12] & 0xc7) | (type << 3);
}

static inline uint8_t smpte_fec_get_type(uint8_t *p_fec)
{
    return (p_fec[12] >> 3) & 0x7;
}

static inline void smpte_fec_set_index(uint8_t *p_fec, uint8_t index)
{
    index &= 0x7;
    p_fec[12] = (p_fec[12] & 0xf8) | index;
}

static inline uint8_t smpte_fec_get_index(uint8_t *p_fec)
{
    return p_fec[12] & 0x7;
}

static inline void smpte_fec_set_offset(uint8_t *p_fec, uint8_t offset)
{
    p_fec[13] = offset;
}

static inline uint8_t smpte_fec_get_offset(const uint8_t *p_fec)
{
    return p_fec[13];
}

static inline void smpte_fec_set_na(uint8_t *p_fec, uint8_t na)
{
    p_fec[14] = na;
}

static inline uint8_t smpte_fec_get_na(const uint8_t *p_fec)
{
    return p_fec[14];
}

static inline void smpte_fec_set_snbase_ext(uint8_t *p_fec, uint8_t snbase_ext)
{
    p_fec[15] = snbase_ext;
}

static inline uint8_t smpte_fec_get_snbase_ext(const uint8_t *p_fec)
{
    return p_fec[15];
}

#ifdef __cplusplus
}
#endif

#endif
