/*****************************************************************************
 * a52.h: ATSC A/52:2012 Digital Audio Compression Standard
 *****************************************************************************
 * Copyright (C) 2013-2018 VideoLAN
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
 * - ATSC A/52:2012
 */

#ifndef __BITSTREAM_ATSC_A52_H__
#define __BITSTREAM_ATSC_A52_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <string.h>   /* memcmp */
#include <stdbool.h>  /* bool */

#ifdef __cplusplus
extern "C"
{
#endif

#define A52_BSID                8
#define A52_BSID_ANNEX_D        1
#define A52_BSID_ANNEX_E        16

#define A52_FSCOD_48KHZ         0
#define A52_FSCOD_441KHZ        1
#define A52_FSCOD_32KHZ         2
#define A52_FSCOD_RESERVED      3
#define A52_FRAME_SAMPLES       1536

/*****************************************************************************
 * A/52 syncinfo (A/52:2012 Table 5.1)
 * - syncword       16
 * - crc1           16
 * - fscod          2
 * - frmsizecod     6
 *****************************************************************************/
#define A52_SYNCINFO_SIZE       5

/* A/52 Frame Size Code Table (A/52:2012 Table 5.18) */
static const uint16_t a52_frame_size_code_tab[38][3] = {
    { 64,   69,   96   },
    { 64,   70,   96   },
    { 80,   87,   120  },
    { 80,   88,   120  },
    { 96,   104,  144  },
    { 96,   105,  144  },
    { 112,  121,  168  },
    { 112,  122,  168  },
    { 128,  139,  192  },
    { 128,  140,  192  },
    { 160,  174,  240  },
    { 160,  175,  240  },
    { 192,  208,  288  },
    { 192,  209,  288  },
    { 224,  243,  336  },
    { 224,  244,  336  },
    { 256,  278,  384  },
    { 256,  279,  384  },
    { 320,  348,  480  },
    { 320,  349,  480  },
    { 384,  417,  576  },
    { 384,  418,  576  },
    { 448,  487,  672  },
    { 448,  488,  672  },
    { 512,  557,  768  },
    { 512,  558,  768  },
    { 640,  696,  960  },
    { 640,  697,  960  },
    { 768,  835,  1152 },
    { 768,  836,  1152 },
    { 896,  975,  1344 },   
    { 896,  976,  1344 },
    { 1024, 1114, 1536 },
    { 1024, 1115, 1536 },
    { 1152, 1253, 1728 },
    { 1152, 1254, 1728 },
    { 1280, 1393, 1920 },
    { 1280, 1394, 1920 },
};

/* A/52 Bitrate Table (A/52:2012 Table 5.18) */
static const uint16_t a52_bitrate_tab[38] = {
    32, 32,
    40, 40,
    48, 48,
    56, 56,
    64, 64,
    80, 80,
    96, 96,
    112, 112,
    128, 128,
    160, 160,
    192, 192,
    224, 224,
    256, 256,
    320, 320,
    384, 384,
    448, 448,
    512, 512,
    576, 576,
    640, 640
};

static inline uint16_t a52_get_sync(const uint8_t *p_a52)
{
    return (p_a52[0] << 8 | p_a52[1]);
}

static inline void a52_set_sync(uint8_t *p_a52)
{
    p_a52[0] = 0xb;
    p_a52[1] = 0x77;
}

static inline uint16_t a52_get_crc(const uint8_t *p_a52)
{
    return (p_a52[2] << 8 | p_a52[3]);
}

static inline void a52_set_crc(uint8_t *p_a52, uint16_t i_crc)
{
    p_a52[2] = i_crc >> 8;
    p_a52[3] = i_crc & 0xff;
}

static inline uint8_t a52_get_fscod(const uint8_t *p_a52)
{
    return (p_a52[4] & 0xc0) >> 6;
}

static inline void a52_set_fscod(uint8_t *p_a52, uint8_t i_fscod)
{
    p_a52[4] &= ~0xc0;
    p_a52[4] |= (i_fscod & 0x3) << 6;
}

static inline uint8_t a52_get_frmsizecod(const uint8_t *p_a52)
{
    return p_a52[4] & 0x3f;
}

static inline void a52_set_frmsizecod(uint8_t *p_a52, uint8_t i_frmsizecod)
{
    p_a52[4] &= ~0x3f;
    p_a52[4] |= i_frmsizecod & 0x3f;
}

static inline uint16_t a52_get_frame_size(uint8_t i_fscod, uint8_t i_frmsizecod)
{
    i_fscod &= 0x3;
    if ((i_fscod == A52_FSCOD_RESERVED) || (i_frmsizecod > 37)) {
        return 0;
    }
    return a52_frame_size_code_tab[i_frmsizecod][i_fscod] * 2;
}

static inline bool a52_sync_compare_formats(const uint8_t *p_a521, const uint8_t *p_a522)
{
    return p_a521[0] == p_a522[0] && p_a521[1] == p_a522[1] &&
           p_a521[4] == p_a522[4];
}

/*****************************************************************************
 * A/52 bsi (Bit Stream Information)
 *****************************************************************************/

static inline uint8_t a52_get_bsid(const uint8_t *p_a52)
{
    return (p_a52[5] & 0xf8) >> 3;
}

static inline void a52_set_bsid(uint8_t *p_a52, uint8_t i_bsid)
{
    p_a52[5] &= ~0xf8;
    p_a52[5] |= (i_bsid & 0xf8) << 3;
}

static inline uint8_t a52_get_bsmod(const uint8_t *p_a52)
{
    return (p_a52[5] & 0x7);
}

static inline void a52_set_bsmod(uint8_t *p_a52, uint8_t i_bsid)
{
    p_a52[5] &= ~0x7;
    p_a52[5] |= i_bsid & 0x7;
}

static inline uint8_t a52_get_acmod(const uint8_t *p_a52)
{
    return (p_a52[6] & 0xe0) >> 5;
}

static inline void a52_set_acmod(uint8_t *p_a52, uint8_t i_acmod)
{
    p_a52[6] &= ~0xe0;
    p_a52[6] |= (i_acmod & 0x7) << 5;
}

static inline uint8_t a52_get_cmixlev(const uint8_t *p_a52)
{
    return (p_a52[6] & 0x18) >> 3;
}

static inline void a52_set_cmixlev(uint8_t *p_a52, uint8_t i_cmixlev)
{
    p_a52[6] &= ~0x18;
    p_a52[6] |= (i_cmixlev & 0x3) << 3;
}

/*****************************************************************************
 * A/52 Annex E 
 *****************************************************************************/
#define A52E_FSCOD2_24KHZ       0
#define A52E_FSCOD2_2205KHZ     1
#define A52E_FSCOD2_16KHZ       2
#define A52E_FSCOD2_RESERVED    3

static inline uint8_t a52e_get_strmtyp(const uint8_t *p_a52)
{
    return (p_a52[2] & 0xc0) >> 5;
}

static inline void a52e_set_strmtyp(uint8_t *p_a52, uint8_t i_strmtyp)
{
    p_a52[2] &= ~0xc0;
    p_a52[2] |= (i_strmtyp & 0x3) << 6;
}

static inline uint8_t a52e_get_substreamid(const uint8_t *p_a52)
{
    return (p_a52[2] & 0x38) >> 3;
}

static inline void a52e_set_substreamid(uint8_t *p_a52, uint8_t i_substreamid)
{
    p_a52[2] &= ~0x38;
    p_a52[2] |= (i_substreamid & 0x7) << 3;
}

static inline uint16_t a52e_get_frmsiz(const uint8_t *p_a52)
{
    return ((p_a52[2] & 0x7) << 8) | p_a52[3];
}

static inline void a52e_set_frmsiz(uint8_t *p_a52, uint16_t i_frmsiz)
{
    p_a52[2] &= ~0x7;
    p_a52[2] |= (i_frmsiz >> 8) & 0x7;
    p_a52[3] = i_frmsiz & 0xff;
}

static inline uint16_t a52e_get_frame_size(uint16_t i_frmsiz)
{
    return (i_frmsiz + 1) * 2;
}

static inline uint8_t a52e_get_fscod(const uint8_t *p_a52)
{
    return (p_a52[4] & 0xc0) >> 6;
}

static inline void a52e_set_fscod(uint8_t *p_a52, uint8_t i_fscod)
{
    p_a52[4] &= ~0xc0;
    p_a52[4] |= (i_fscod & 0x3) << 6;
}

static inline uint8_t a52e_get_fscod2(const uint8_t *p_a52)
{
    return (p_a52[4] & 0x30) >> 4;
}

static inline void a52e_set_fscod2(uint8_t *p_a52, uint8_t i_fscod)
{
    p_a52[4] &= ~0x30;
    p_a52[4] |= (i_fscod & 0x3) << 4;
}

static inline uint8_t a52e_get_acmod(const uint8_t *p_a52)
{
    return (p_a52[4] & 0xe) >> 1;
}

static inline void a52e_set_acmod(uint8_t *p_a52, uint8_t i_acmod)
{
    p_a52[4] &= ~0xe;
    p_a52[4] |= (i_acmod & 0x7) << 1;
}

static inline uint8_t a52e_get_lfeon(const uint8_t *p_a52)
{
    return p_a52[4] & 0x1;
}

static inline void a52e_set_lfeon(uint8_t *p_a52, uint8_t i_lfeon)
{
    p_a52[4] &= ~0x1;
    p_a52[4] |= i_lfeon & 0x1;
}

static inline bool a52e_sync_compare_formats(const uint8_t *p_a521, const uint8_t *p_a522)
{
    return !memcmp(p_a521, p_a522, A52_SYNCINFO_SIZE);
}

#ifdef __cplusplus
}
#endif

#endif
