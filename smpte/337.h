/*****************************************************************************
 * 337.h: SMPTE 337 Non-PCM data in AES3
 *****************************************************************************
 * Copyright (C) 2016 OpenHeadend
 *
 * Authors: Christophe Massiot <cmassiot@openheadend.tv>
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
 *  - SMPTE 337
 *  - SMPTE 338
 */

#ifndef __BITSTREAM_SMPTE_337_H__
#define __BITSTREAM_SMPTE_337_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * SMPTE 337 burst preamble (16-bit mode only)
 *****************************************************************************/
#define S337_PREAMBLE_SIZE          8
#define S337_PREAMBLE_A1            0xf8
#define S337_PREAMBLE_A2            0x72
#define S337_PREAMBLE_B1            0x4e
#define S337_PREAMBLE_B2            0x1f

#define S337_TYPE_NULL              0
#define S337_TYPE_A52               1
#define S337_TYPE_TIMESTAMP         2
#define S337_TYPE_PAUSE             3
#define S337_TYPE_A52E              16
#define S337_TYPE_VSYNC             26
#define S337_TYPE_DOLBY_E           28
#define S337_TYPE_CAPTION           29
#define S337_TYPE_USER              30

#define S337_MODE_16                0
#define S337_MODE_20                1
#define S337_MODE_24                2
#define S337_MODE_RESERVED          3

#define S337_TYPE_A52_REP_RATE_FLAG 0x08
#define S337_TYPE_A52_NOT_FULL_SVC  0x10

static inline void s337_set_data_type(uint8_t *p_s337, uint8_t i_data_type)
{
    p_s337[5] &= ~0x1f;
    p_s337[5] |= i_data_type & 0x1f;
}

static inline uint8_t s337_get_data_type(const uint8_t *p_s337)
{
    return p_s337[5] & 0x1f;
}

static inline void s337_set_data_mode(uint8_t *p_s337, uint8_t i_data_mode)
{
    p_s337[5] &= ~0x60;
    p_s337[5] |= (i_data_mode << 5) & 0x60;
}

static inline uint8_t s337_get_data_mode(const uint8_t *p_s337)
{
    return (p_s337[5] & 0x60) >> 5;
}

static inline void s337_set_error(uint8_t *p_s337)
{
    p_s337[5] |= 0x80;
}

static inline void s337_clear_error(uint8_t *p_s337)
{
    p_s337[5] &= ~0x80;
}

static inline bool s337_get_error(const uint8_t *p_s337)
{
    return p_s337[5] & 0x80;
}

static inline void s337_set_data_type_dep(uint8_t *p_s337, uint8_t i_data_type_dep)
{
    p_s337[4] &= ~0x1f;
    p_s337[4] |= i_data_type_dep & 0x1f;
}

static inline uint8_t s337_get_data_type_dep(const uint8_t *p_s337)
{
    return p_s337[4] & 0x1f;
}

static inline void s337_set_data_stream(uint8_t *p_s337, uint8_t i_data_stream)
{
    p_s337[4] &= ~0xe0;
    p_s337[4] |= (i_data_stream << 5) & 0xe0;
}

static inline uint8_t s337_get_data_stream(const uint8_t *p_s337)
{
    return (p_s337[4] & 0xe0) >> 5;
}

static inline void s337_set_length(uint8_t *p_s337, uint16_t i_length)
{
    p_s337[6] = i_length >> 8;
    p_s337[7] = i_length & 0xff;
}

static inline uint16_t s337_get_length(const uint8_t *p_s337)
{
    return (p_s337[6] << 8) | p_s337[7];
}

#ifdef __cplusplus
}
#endif

#endif
