/*****************************************************************************
 * ts.h: ISO/IEC 13818-1 Transport Stream
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
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
 *  - ISO/IEC 13818-1:2007(E) (MPEG-2 systems)
 */

#ifndef __BITSTREAM_MPEG_TS_H__
#define __BITSTREAM_MPEG_TS_H__

#include <stdlib.h>   /* malloc */
#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */
#include <string.h>   /* memset */

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * TS header
 *****************************************************************************/
#define TS_SIZE             188
#define TS_HEADER_SIZE      4
#define TS_HEADER_SIZE_AF   6
#define TS_HEADER_SIZE_PCR  12

#define TS_DECLARE(p_ts)    \
    uint8_t p_ts[TS_SIZE]

static inline uint8_t *ts_allocate(void)
{
    return malloc(TS_SIZE * sizeof(uint8_t));
}

static inline void ts_init(uint8_t *p_ts)
{
    p_ts[0] = 0x47;
    p_ts[1] = 0x0;
    p_ts[2] = 0x0;
    p_ts[3] = 0x0;
}

static inline void ts_set_transporterror(uint8_t *p_ts)
{
    p_ts[1] |= 0x80;
}

static inline bool ts_get_transporterror(const uint8_t *p_ts)
{
    return !!(p_ts[1] & 0x80);
}

static inline void ts_set_unitstart(uint8_t *p_ts)
{
    p_ts[1] |= 0x40;
}

static inline bool ts_get_unitstart(const uint8_t *p_ts)
{
    return !!(p_ts[1] & 0x40);
}

static inline void ts_set_transportpriority(uint8_t *p_ts)
{
    p_ts[1] |= 0x20;
}

static inline bool ts_get_transportpriority(const uint8_t *p_ts)
{
    return !!(p_ts[1] & 0x20);
}

static inline void ts_set_pid(uint8_t *p_ts, uint16_t i_pid)
{
    p_ts[1] &= ~0x1f;
    p_ts[1] |= (i_pid >> 8) & 0x1f;
    p_ts[2] = i_pid & 0xff;
}

static inline uint16_t ts_get_pid(const uint8_t *p_ts)
{
    return ((p_ts[1] & 0x1f) << 8) | p_ts[2];
}

static inline void ts_set_cc(uint8_t *p_ts, uint8_t i_cc)
{
    p_ts[3] &= ~0xf;
    p_ts[3] |= (i_cc & 0xf);
}

static inline uint8_t ts_get_cc(const uint8_t *p_ts)
{
    return p_ts[3] & 0xf;
}

static inline void ts_set_payload(uint8_t *p_ts)
{
    p_ts[3] |= 0x10;
}

static inline bool ts_has_payload(const uint8_t *p_ts)
{
    return !!(p_ts[3] & 0x10);
}

static inline void ts_set_adaptation(uint8_t *p_ts, uint8_t i_length)
{
    p_ts[3] |= 0x20;
    p_ts[4] = i_length;
    if (i_length)
        p_ts[5] = 0x0;
    if (i_length > 1)
        memset(&p_ts[6], 0xff, i_length - 1); /* stuffing */
}

static inline bool ts_has_adaptation(const uint8_t *p_ts)
{
    return !!(p_ts[3] & 0x20);
}

static inline uint8_t ts_get_adaptation(const uint8_t *p_ts)
{
    return p_ts[4];
}

static inline void ts_set_scrambling(uint8_t *p_ts, uint8_t i_scrambling)
{
    p_ts[3] &= ~0xc0;
    p_ts[3] |= i_scrambling << 6;
}

static inline uint8_t ts_get_scrambling(const uint8_t *p_ts)
{
    return (p_ts[3] & 0xc0) >> 6;
}

static inline bool ts_validate(const uint8_t *p_ts)
{
    return p_ts[0] == 0x47;
}

/*****************************************************************************
 * TS payload
 *****************************************************************************/
static inline void ts_pad(uint8_t *p_ts)
{
    ts_init(p_ts);
    ts_set_pid(p_ts, 0x1fff);
    ts_set_cc(p_ts, 0);
    ts_set_payload(p_ts);
    memset(p_ts + 4, 0xff, TS_SIZE - 4);
}

static inline uint8_t *ts_payload(uint8_t *p_ts)
{
    if (!ts_has_payload(p_ts))
        return p_ts + TS_SIZE;
    if (!ts_has_adaptation(p_ts))
        return p_ts + TS_HEADER_SIZE;
    return p_ts + TS_HEADER_SIZE + 1 + ts_get_adaptation(p_ts);
}

static inline uint8_t *ts_section(uint8_t *p_ts)
{
    if (!ts_get_unitstart(p_ts))
        return ts_payload(p_ts);

    return ts_payload(p_ts) + 1; /* pointer_field */
}

static inline uint8_t *ts_next_section(uint8_t *p_ts)
{
    uint8_t *p_payload;

    if (!ts_get_unitstart(p_ts))
        return p_ts + TS_SIZE;
    p_payload = ts_payload(p_ts);
    if (p_payload >= p_ts + TS_SIZE)
        return p_ts + TS_SIZE;

    return p_payload + *p_payload + 1; /* pointer_field */
}

/*****************************************************************************
 * Adaptation field
 *****************************************************************************/
static inline void tsaf_set_discontinuity(uint8_t *p_ts)
{
    p_ts[5] |= 0x80;
}

static inline bool tsaf_has_discontinuity(const uint8_t *p_ts)
{
    return !!(p_ts[5] & 0x80);
}

static inline void tsaf_set_randomaccess(uint8_t *p_ts)
{
    p_ts[5] |= 0x40;
}

static inline bool tsaf_has_randomaccess(const uint8_t *p_ts)
{
    return !!(p_ts[5] & 0x40);
}

static inline void tsaf_set_streampriority(uint8_t *p_ts)
{
    p_ts[5] |= 0x20;
}

static inline void tsaf_set_pcr(uint8_t *p_ts, uint64_t i_pcr)
{
    p_ts[5] |= 0x10;
    p_ts[6] = (i_pcr >> 25) & 0xff;
    p_ts[7] = (i_pcr >> 17) & 0xff;
    p_ts[8] = (i_pcr >> 9) & 0xff;
    p_ts[9] = (i_pcr >> 1) & 0xff;
    p_ts[10] = 0x7e | ((i_pcr << 7) & 0x80);
    p_ts[11] = 0;
}

static inline void tsaf_set_pcrext(uint8_t *p_ts, uint16_t i_pcr_ext)
{
    p_ts[10] |= (i_pcr_ext >> 8) & 0x1;
    p_ts[11] = i_pcr_ext & 0xff;
}

static inline bool tsaf_has_pcr(const uint8_t *p_ts)
{
    return !!(p_ts[5] & 0x10);
}

static inline uint64_t tsaf_get_pcr(const uint8_t *p_ts)
{
    return ((uint64_t) p_ts[6] << 25) | (p_ts[7] << 17) | (p_ts[8] << 9) | (p_ts[9] << 1) |
           (p_ts[10] >> 7);
}

static inline uint64_t tsaf_get_pcrext(const uint8_t *p_ts)
{
    return ((p_ts[10] & 1) << 8) | p_ts[11];
}

/*****************************************************************************
 * TS payload gathering
 *****************************************************************************/
static inline bool ts_check_duplicate(uint8_t i_cc, uint8_t i_last_cc)
{
    return i_last_cc == i_cc;
}

static inline bool ts_check_discontinuity(uint8_t i_cc, uint8_t i_last_cc)
{
    return (i_last_cc + 17 - i_cc) % 16;
}

#ifdef __cplusplus
}
#endif

#endif
