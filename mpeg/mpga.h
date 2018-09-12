/*****************************************************************************
 * mpga.h: ISO/IEC 11172-3 and 13818-3 MPEG audio
 *****************************************************************************
 * Copyright (C) 2013 VideoLAN
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
 *  - ISO/IEC 11172-3
 *  - ISO/IEC 13818-3
 */

#ifndef __BITSTREAM_MPEG_MPGA_H__
#define __BITSTREAM_MPEG_MPGA_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * MPEG audio header
 *****************************************************************************/
#define MPGA_HEADER_SIZE        4

#define MPGA_ID_1               1
#define MPGA_ID_2               0

#define MPGA_LAYER_1            3
#define MPGA_LAYER_2            2
#define MPGA_LAYER_3            1
#define MPGA_LAYER_ADTS         0

#define MPGA_BITRATE_INVALID    15
#define MPGA_SAMPLERATE_INVALID 3

#define MPGA_MODE_STEREO        0
#define MPGA_MODE_JOINT         1
#define MPGA_MODE_DUAL_MONO     2
#define MPGA_MODE_MONO          3

#define MPGA_EMPHASIS_NONE      0
#define MPGA_EMPHASIS_50_15     1
#define MPGA_EMPHASIS_INVALID   2
#define MPGA_EMPHASIS_J_17      3

static inline void mpga_set_sync(uint8_t *p_mpga)
{
    p_mpga[0] = 0xff;
    p_mpga[1] = 0xf9;
    p_mpga[2] = 0x0;
    p_mpga[3] = 0x0;
}

/* unofficial extension */
static inline bool mpga_get_mpeg25(const uint8_t *p_mpga)
{
    return !(p_mpga[1] & 0x10);
}

static inline void mpga_set_mpeg25(uint8_t *p_mpga)
{
    p_mpga[1] &= ~0x10;
}

static inline bool mpga_get_id(const uint8_t *p_mpga)
{
    return !!(p_mpga[1] & 0x08);
}

static inline void mpga_clear_id(uint8_t *p_mpga)
{
    p_mpga[1] &= ~0x08;
}

static inline uint8_t mpga_get_layer(const uint8_t *p_mpga)
{
    return (p_mpga[1] & 0x06) >> 1;
}

static inline void mpga_set_layer(uint8_t *p_mpga, uint8_t i_layer)
{
    p_mpga[1] &= ~0x06;
    p_mpga[1] |= (i_layer & 0x3) << 1;
}

static inline bool mpga_get_protection_absent(const uint8_t *p_mpga)
{
    return !!(p_mpga[1] & 0x01);
}

static inline void mpga_clear_protection_absent(uint8_t *p_mpga)
{
    p_mpga[1] &= ~0x01;
}

static inline uint8_t mpga_get_bitrate_index(const uint8_t *p_mpga)
{
    return p_mpga[2] >> 4;
}

static inline void mpga_set_bitrate_index(uint8_t *p_mpga, uint8_t i_index)
{
    p_mpga[2] &= ~0xf0;
    p_mpga[2] |= i_index << 4;
}

static inline uint8_t mpga_get_sampling_freq(const uint8_t *p_mpga)
{
    return (p_mpga[2] & 0xc) >> 2;
}

static inline void mpga_set_sampling_freq(uint8_t *p_mpga, uint8_t i_freq)
{
    p_mpga[2] &= ~0x0c;
    p_mpga[2] |= (i_freq & 0x3) << 2;
}

static inline bool mpga_get_padding(const uint8_t *p_mpga)
{
    return !!(p_mpga[2] & 0x02);
}

static inline void mpga_set_padding(uint8_t *p_mpga)
{
    p_mpga[2] |= 0x02;
}

static inline uint8_t mpga_get_mode(const uint8_t *p_mpga)
{
    return (p_mpga[3] & 0xc0) >> 6;
}

static inline void mpga_set_mode(uint8_t *p_mpga, uint8_t i_mode)
{
    p_mpga[3] &= ~0xc0;
    p_mpga[3] |= i_mode << 6;
}

static inline uint8_t mpga_get_mode_ext(const uint8_t *p_mpga)
{
    return (p_mpga[3] & 0x30) >> 4;
}

static inline void mpga_set_mode_ext(uint8_t *p_mpga, uint8_t i_mode_ext)
{
    p_mpga[3] &= ~0x30;
    p_mpga[3] |= (i_mode_ext & 0x3) << 4;
}

static inline bool mpga_get_copyright(const uint8_t *p_mpga)
{
    return !!(p_mpga[3] & 0x80);
}

static inline void mpga_set_copyright(uint8_t *p_mpga)
{
    p_mpga[3] |= 0x80;
}

static inline bool mpga_get_original(const uint8_t *p_mpga)
{
    return !!(p_mpga[3] & 0x40);
}

static inline void mpga_set_original(uint8_t *p_mpga)
{
    p_mpga[3] |= 0x40;
}

static inline uint8_t mpga_get_emphasis(const uint8_t *p_mpga)
{
    return p_mpga[3] & 0x03;
}

static inline void mpga_set_emphasis(uint8_t *p_mpga, uint8_t i_emphasis)
{
    p_mpga[3] &= ~0x03;
    p_mpga[3] |= i_emphasis & 0x03;
}

static inline bool mpga_sync_compare(const uint8_t *p_mpga1, const uint8_t *p_mpga2)
{
    return p_mpga1[0] == p_mpga2[0] &&
           p_mpga1[1] == p_mpga2[1] &&
           (p_mpga1[2] & 0xfc) == (p_mpga2[2] & 0xfc) &&
           p_mpga1[3] == p_mpga2[3];
}

/* same but only takes into account meaningful fields */
static inline bool mpga_sync_compare_formats(const uint8_t *p_mpga1, const uint8_t *p_mpga2)
{
    if (!(p_mpga1[0] == p_mpga2[0] &&
          (p_mpga1[1] & 0xfe) == (p_mpga2[1] & 0xfe) &&
          (p_mpga1[2] & 0xfc) == (p_mpga2[2] & 0xfc)))
        return false;

    /* consider stereo and joint stereo the same - because encoders can
     * switch on the fly */
    uint8_t i_mode1 = mpga_get_mode(p_mpga1);
    uint8_t i_mode2 = mpga_get_mode(p_mpga2);
    if (i_mode1 == MPGA_MODE_JOINT) i_mode1 = MPGA_MODE_STEREO;
    if (i_mode2 == MPGA_MODE_JOINT) i_mode2 = MPGA_MODE_STEREO;
    return i_mode1 == i_mode2;
}

/* same but without comparing bitrate (free bitrate) */
static inline bool mpga_sync_compare_formats_free(const uint8_t *p_mpga1, const uint8_t *p_mpga2)
{
    if (!(p_mpga1[0] == p_mpga2[0] &&
          (p_mpga1[1] & 0xfe) == (p_mpga2[1] & 0xfe) &&
          (p_mpga1[2] & 0xc) == (p_mpga2[2] & 0xc)))
        return false;

    /* consider stereo and joint stereo the same - because encoders can
     * switch on the fly */
    uint8_t i_mode1 = mpga_get_mode(p_mpga1);
    uint8_t i_mode2 = mpga_get_mode(p_mpga2);
    if (i_mode1 == MPGA_MODE_JOINT) i_mode1 = MPGA_MODE_STEREO;
    if (i_mode2 == MPGA_MODE_JOINT) i_mode2 = MPGA_MODE_STEREO;
    return i_mode1 == i_mode2;
}

#ifdef __cplusplus
}
#endif

#endif
