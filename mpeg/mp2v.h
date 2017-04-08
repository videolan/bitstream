/*****************************************************************************
 * mp2v.h: ISO/IEC 13818-2 (video)
 *****************************************************************************
 * Copyright (C) 2013-2015 VideoLAN
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
 *  - ISO/IEC 13818-2 (MPEG-2 video)
 *  - ISO/IEC JTC1/SC29/WG11 MPEG2007/m14868 (high progressive profile)
 */

#ifndef __BITSTREAM_MPEG_MP2V_H__
#define __BITSTREAM_MPEG_MP2V_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */
#include <string.h>   /* memset */

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * MP2V start code
 *****************************************************************************/
static inline void mp2vstart_init(uint8_t *p_mp2vstart, uint8_t startcode)
{
    p_mp2vstart[0] = 0x0;
    p_mp2vstart[1] = 0x0;
    p_mp2vstart[2] = 0x1;
    p_mp2vstart[3] = startcode;
}

/*****************************************************************************
 * MP2V extension
 *****************************************************************************/
#define MP2VX_HEADER_SIZE           4
#define MP2VX_START_CODE            0xb5

#define MP2VX_ID_SEQX               1
#define MP2VX_ID_SEQDX              2
#define MP2VX_ID_QUANTX             3
#define MP2VX_ID_SEQSCALX           5
#define MP2VX_ID_PICDX              7
#define MP2VX_ID_PICX               8
#define MP2VX_ID_PICSSCALX          9
#define MP2VX_ID_PICTSCALX          10

static inline uint8_t mp2vx_get_id(const uint8_t *p_mp2vx)
{
    return p_mp2vx[4] >> 4;
}

static inline void mp2vx_set_id(uint8_t *p_mp2vx, uint8_t id)
{
    p_mp2vx[4] &= 0xf;
    p_mp2vx[4] |= id << 4;
}

static inline uint8_t mp2vxst_get_id(uint8_t start)
{
    return start >> 4;
}

/*****************************************************************************
 * MP2V user data
 *****************************************************************************/
#define MP2VUSR_HEADER_SIZE         4
#define MP2VUSR_START_CODE          0xb2

/*****************************************************************************
 * MP2V sequence header
 *****************************************************************************/
#define MP2VSEQ_HEADER_SIZE         12
#define MP2VSEQ_START_CODE          0xb3

#define MP2VSEQ_ASPECT_SQUARE       1
#define MP2VSEQ_ASPECT_4_3          2
#define MP2VSEQ_ASPECT_16_9         3
#define MP2VSEQ_ASPECT_2_21         4

#define MP2VSEQ_FRAMERATE_23_976    1
#define MP2VSEQ_FRAMERATE_24        2
#define MP2VSEQ_FRAMERATE_25        3
#define MP2VSEQ_FRAMERATE_29_97     4
#define MP2VSEQ_FRAMERATE_30        5
#define MP2VSEQ_FRAMERATE_50        6
#define MP2VSEQ_FRAMERATE_59_94     7
#define MP2VSEQ_FRAMERATE_60        8
/* unofficial Xing */
#define MP2VSEQ_FRAMERATE_14_99     9
/* unofficial libmpeg3 */
#define MP2VSEQ_FRAMERATE_4_99      10
#define MP2VSEQ_FRAMERATE_9_99      11
#define MP2VSEQ_FRAMERATE_12_99     12

#define MP2VSEQ_VBVBUFFER_VBR       0xffff

static inline void mp2vseq_init(uint8_t *p_mp2vseq)
{
    mp2vstart_init(p_mp2vseq, MP2VSEQ_START_CODE);
    p_mp2vseq[10] = 0x20;
    p_mp2vseq[11] = 0x0;
}

static inline void mp2vseq_set_horizontal(uint8_t *p_mp2vseq,
                                          uint16_t i_horizontal)
{
    p_mp2vseq[4] = i_horizontal >> 4;
    p_mp2vseq[5] &= 0xf;
    p_mp2vseq[5] |= (i_horizontal << 4) & 0xff;
}

static inline uint16_t mp2vseq_get_horizontal(const uint8_t *p_mp2vseq)
{
    return (p_mp2vseq[4] << 4) | (p_mp2vseq[5] >> 4);
}

static inline void mp2vseq_set_vertical(uint8_t *p_mp2vseq, uint16_t i_vertical)
{
    p_mp2vseq[5] &= 0xf0;
    p_mp2vseq[5] |= (i_vertical >> 8) & 0xf;
    p_mp2vseq[6] = i_vertical & 0xff;
}

static inline uint16_t mp2vseq_get_vertical(const uint8_t *p_mp2vseq)
{
    return ((p_mp2vseq[5] & 0xf) << 8) | p_mp2vseq[6];
}

static inline void mp2vseq_set_aspect(uint8_t *p_mp2vseq, uint8_t i_aspect)
{
    p_mp2vseq[7] &= 0xf;
    p_mp2vseq[7] |= (i_aspect << 4) & 0xf0;
}

static inline uint8_t mp2vseq_get_aspect(const uint8_t *p_mp2vseq)
{
    return (p_mp2vseq[7] >> 4);
}

static inline void mp2vseq_set_framerate(uint8_t *p_mp2vseq,
                                         uint8_t i_framerate)
{
    p_mp2vseq[7] &= 0xf0;
    p_mp2vseq[7] |= i_framerate & 0xf;
}

static inline uint8_t mp2vseq_get_framerate(const uint8_t *p_mp2vseq)
{
    return (p_mp2vseq[7] & 0xf);
}

static inline void mp2vseq_set_bitrate(uint8_t *p_mp2vseq, uint32_t i_bitrate)
{
    p_mp2vseq[8] = i_bitrate >> 10;
    p_mp2vseq[9] = (i_bitrate >> 2) & 0xff;
    p_mp2vseq[10] &= 0x3f;
    p_mp2vseq[10] |= (i_bitrate << 6) & 0xff;
}

static inline uint32_t mp2vseq_get_bitrate(const uint8_t *p_mp2vseq)
{
    return (p_mp2vseq[8] << 10) | (p_mp2vseq[9] << 2) | (p_mp2vseq[10] >> 6);
}

static inline void mp2vseq_set_vbvbuffer(uint8_t *p_mp2vseq,
                                         uint16_t i_vbvbuffer)
{
    p_mp2vseq[10] &= 0xe0;
    p_mp2vseq[10] |= (i_vbvbuffer >> 5) & 0x1f;
    p_mp2vseq[11] &= 0x7;
    p_mp2vseq[11] |= (i_vbvbuffer << 3) & 0xf8;
}

static inline uint16_t mp2vseq_get_vbvbuffer(const uint8_t *p_mp2vseq)
{
    return ((p_mp2vseq[10] & 0x1f) << 5) | (p_mp2vseq[11] >> 3);
}

static inline void mp2vseq_set_constrained(uint8_t *p_mp2vseq)
{
    p_mp2vseq[11] |= 0x4;
}

static inline bool mp2vseq_get_constrained(const uint8_t *p_mp2vseq)
{
    return !!(p_mp2vseq[11] & 0x4);
}

static inline void mp2vseq_set_intramatrix(uint8_t *p_mp2vseq,
                                           const uint8_t p_matrix[64])
{
    p_mp2vseq[11] |= 0x2;
    if (p_matrix != NULL) {
        int i;
        for (i = 0; i < 64; i++) {
            p_mp2vseq[11 + i] |= p_matrix[i] >> 7;
            p_mp2vseq[12 + i] = (p_matrix[i] << 1) & 0xfe;
        }
    } else
        memset(p_mp2vseq + 12, 0, 64);
}

static inline bool mp2vseq_get_intramatrix(const uint8_t *p_mp2vseq,
                                           uint64_t p_matrix[64])
{
    if (!(p_mp2vseq[11] & 0x2))
        return false;
    if (p_matrix != NULL) {
        int i;
        for (i = 0; i < 64; i++)
            p_matrix[i] = ((p_mp2vseq[11 + i] << 7) & 0x80) |
                           (p_mp2vseq[12 + i] >> 1);
    }
    return true;
}

static inline void mp2vseq_set_nonintramatrix(uint8_t *p_mp2vseq,
                                              const uint8_t p_matrix[64])
{
    unsigned int offset = 11;
    if (p_mp2vseq[11] & 0x2)
        offset += 64;
    p_mp2vseq[offset] |= 0x1;
    if (p_matrix != NULL) {
        int i;
        for (i = 0; i < 64; i++) {
            p_mp2vseq[offset + i] |= p_matrix[i] >> 7;
            p_mp2vseq[offset + 1 + i] = (p_matrix[i] << 1) & 0xfe;
        }
    } else
        memset(p_mp2vseq + offset + 1, 0, 64);
}

static inline bool mp2vseq_get_nonintramatrix(const uint8_t *p_mp2vseq,
                                              uint64_t p_matrix[64])
{
    unsigned int offset = 11;
    if (p_mp2vseq[11] & 0x2)
        offset += 64;
    if (!(p_mp2vseq[offset] & 0x1))
        return false;
    if (p_matrix != NULL) {
        int i;
        for (i = 0; i < 64; i++)
            p_matrix[i] = ((p_mp2vseq[offset + i] << 7) & 0x80) |
                           (p_mp2vseq[offset + 1 + i] >> 1);
    }
    return true;
}

/*****************************************************************************
 * MP2V sequence extension
 *****************************************************************************/
#define MP2VSEQX_HEADER_SIZE        10

#define MP2VSEQX_PROFILE_MASK       0x70
#define MP2VSEQX_PROFILE_SIMPLE     (5 << 4)
#define MP2VSEQX_PROFILE_MAIN       (4 << 4)
#define MP2VSEQX_PROFILE_SNR_SCAL   (3 << 4)
#define MP2VSEQX_PROFILE_SPAT_SCAL  (2 << 4)
#define MP2VSEQX_PROFILE_HIGH       (1 << 4)
#define MP2VSEQX_LEVEL_MASK         0x8f /* with escape bit */
#define MP2VSEQX_LEVEL_LOW          10
#define MP2VSEQX_LEVEL_MAIN         8
#define MP2VSEQX_LEVEL_HIGH1440     6
#define MP2VSEQX_LEVEL_HIGH         4
#define MP2VSEQX_LEVEL_HIGHP        2
#define MPV2SEQX_LEVEL_LOW_MV       0x8e
#define MPV2SEQX_LEVEL_MAIN_MV      0x8d
#define MPV2SEQX_LEVEL_HIGH_MV      0x8b
#define MPV2SEQX_LEVEL_HIGH1440_MV  0x8a
#define MPV2SEQX_LEVEL_MAIN_422     0x85
#define MPV2SEQX_LEVEL_HIGH_422     0x82

#define MP2VSEQX_CHROMA_420         1
#define MP2VSEQX_CHROMA_422         2
#define MP2VSEQX_CHROMA_444         3

static inline void mp2vseqx_init(uint8_t *p_mp2vseq)
{
    mp2vstart_init(p_mp2vseq, MP2VX_START_CODE);
    p_mp2vseq[4] = (MP2VX_ID_SEQX << 4) & 0xf0;
    p_mp2vseq[5] = 0;
    p_mp2vseq[7] = 0x1;
    p_mp2vseq[9] = 0;
}

static inline void mp2vseqx_set_profilelevel(uint8_t *p_mp2vseqx,
                                             uint8_t i_profilelevel)
{
    p_mp2vseqx[4] &= 0xf0;
    p_mp2vseqx[4] |= i_profilelevel >> 4;
    p_mp2vseqx[5] &= 0x0f;
    p_mp2vseqx[5] |= (i_profilelevel << 4) & 0xf0;
}

static inline uint8_t mp2vseqx_get_profilelevel(const uint8_t *p_mp2vseqx)
{
    return ((p_mp2vseqx[4] & 0x0f) << 4) | (p_mp2vseqx[5] >> 4);
}

static inline const char *mp2vseqx_get_profile_txt(uint8_t i_profilelevel)
{
    if (i_profilelevel & (1 << 7)) {
        return i_profilelevel == MPV2SEQX_LEVEL_LOW_MV  ||
               i_profilelevel == MPV2SEQX_LEVEL_MAIN_MV ||
               i_profilelevel == MPV2SEQX_LEVEL_HIGH_MV ||
               i_profilelevel == MPV2SEQX_LEVEL_HIGH1440_MV ? "Multi-view" :
               i_profilelevel == MPV2SEQX_LEVEL_MAIN_422 ||
               i_profilelevel == MPV2SEQX_LEVEL_HIGH_422 ? "4:2:2" : "Reserved";
    }
    else {
        i_profilelevel &= 0x70;
        return i_profilelevel == MP2VSEQX_PROFILE_SIMPLE ? "Simple" :
               i_profilelevel == MP2VSEQX_PROFILE_MAIN ? "Main" :
               i_profilelevel == MP2VSEQX_PROFILE_SNR_SCAL ? "SNR Scalable" :
               i_profilelevel == MP2VSEQX_PROFILE_SPAT_SCAL ? "Spatially Scalable" :
               i_profilelevel == MP2VSEQX_PROFILE_HIGH ? "High" : "Reserved";
    }
}

static inline const char *mp2vseqx_get_level_txt(uint8_t i_profilelevel)
{
    if (i_profilelevel & (1 << 7)) {
        return i_profilelevel == MPV2SEQX_LEVEL_LOW_MV ? "Low" :
               i_profilelevel == MPV2SEQX_LEVEL_MAIN_MV ||
               i_profilelevel == MPV2SEQX_LEVEL_MAIN_422 ? "Main" :
               i_profilelevel == MPV2SEQX_LEVEL_HIGH1440_MV ||
               i_profilelevel == MPV2SEQX_LEVEL_HIGH_422 ? "High 1440" :
               i_profilelevel == MPV2SEQX_LEVEL_HIGH_MV  ? "High" : "Reserved";
    }
    else {
        i_profilelevel &= 0xf;
        return i_profilelevel == MP2VSEQX_LEVEL_LOW ? "Low" :
               i_profilelevel == MP2VSEQX_LEVEL_MAIN ? "Main" :
               i_profilelevel == MP2VSEQX_LEVEL_HIGH1440 ? "High 1440" :
               i_profilelevel == MP2VSEQX_LEVEL_HIGH ? "High" :
               i_profilelevel == MP2VSEQX_LEVEL_HIGHP ? "HighP" : "Reserved";
    }
}

static inline void mp2vseqx_set_progressive(uint8_t *p_mp2vseqx)
{
    p_mp2vseqx[5] |= 0x8;
}

static inline bool mp2vseqx_get_progressive(const uint8_t *p_mp2vseqx)
{
    return !!(p_mp2vseqx[5] & 0x8);
}

static inline void mp2vseqx_set_chroma(uint8_t *p_mp2vseqx, uint8_t i_chroma)
{
    p_mp2vseqx[5] &= 0xf9;
    p_mp2vseqx[5] |= (i_chroma & 0x3) << 1;
}

static inline uint8_t mp2vseqx_get_chroma(const uint8_t *p_mp2vseqx)
{
    return ((p_mp2vseqx[5] & 0x06) >> 1);
}

static inline void mp2vseqx_set_horizontal(uint8_t *p_mp2vseqx,
                                           uint8_t i_horizontal)
{
    p_mp2vseqx[5] &= 0xfe;
    p_mp2vseqx[5] |= (i_horizontal >> 1) & 0x1;
    p_mp2vseqx[6] &= 0x7f;
    p_mp2vseqx[6] |= (i_horizontal << 7) & 0x80;
}

static inline uint8_t mp2vseqx_get_horizontal(const uint8_t *p_mp2vseqx)
{
    return ((p_mp2vseqx[5] & 0x1) << 1) | (p_mp2vseqx[6] >> 7);
}

static inline void mp2vseqx_set_vertical(uint8_t *p_mp2vseqx,
                                         uint8_t i_vertical)
{
    p_mp2vseqx[6] &= 0x9f;
    p_mp2vseqx[6] |= (i_vertical << 5) & 0x60;
}

static inline uint8_t mp2vseqx_get_vertical(const uint8_t *p_mp2vseqx)
{
    return (p_mp2vseqx[6] & 0x60) >> 5;
}

static inline void mp2vseqx_set_bitrate(uint8_t *p_mp2vseqx, uint16_t i_bitrate)
{
    p_mp2vseqx[6] &= 0xe0;
    p_mp2vseqx[6] |= (i_bitrate >> 7) & 0x1f;
    p_mp2vseqx[7] = ((i_bitrate << 1) & 0xfe) | 0x1;
}

static inline uint16_t mp2vseqx_get_bitrate(const uint8_t *p_mp2vseqx)
{
    return ((p_mp2vseqx[6] & 0x1f) << 7) | (p_mp2vseqx[7] >> 1);
}

static inline void mp2vseqx_set_vbvbuffer(uint8_t *p_mp2vseqx,
                                          uint8_t i_vbvbuffer)
{
    p_mp2vseqx[8] = i_vbvbuffer;
}

static inline uint8_t mp2vseqx_get_vbvbuffer(const uint8_t *p_mp2vseqx)
{
    return p_mp2vseqx[8];
}

static inline void mp2vseqx_set_lowdelay(uint8_t *p_mp2vseqx)
{
    p_mp2vseqx[9] |= 0x8;
}

static inline bool mp2vseqx_get_lowdelay(const uint8_t *p_mp2vseqx)
{
    return !!(p_mp2vseqx[9] & 0x8);
}

static inline void mp2vseqx_set_frameraten(uint8_t *p_mp2vseqx,
                                           uint8_t i_frameraten)
{
    p_mp2vseqx[9] &= 0x9f;
    p_mp2vseqx[9] |= (i_frameraten << 5) & 0x60;
}

static inline uint8_t mp2vseqx_get_frameraten(const uint8_t *p_mp2vseqx)
{
    return (p_mp2vseqx[9] & 0x60) >> 5;
}

static inline void mp2vseqx_set_framerated(uint8_t *p_mp2vseqx,
                                           uint8_t i_framerated)
{
    p_mp2vseqx[9] &= 0xe0;
    p_mp2vseqx[9] |= i_framerated & 0x1f;
}

static inline uint8_t mp2vseqx_get_framerated(const uint8_t *p_mp2vseqx)
{
    return (p_mp2vseqx[9] & 0x1f);
}

/*****************************************************************************
 * MP2V sequence display extension
 *****************************************************************************/
#define MP2VSEQDX_HEADER_SIZE        9
#define MP2VSEQDX_COLOR_SIZE         3

static inline void mp2vseqdx_init(uint8_t *p_mp2vseq)
{
    mp2vstart_init(p_mp2vseq, MP2VX_START_CODE);
    p_mp2vseq[4] = (MP2VX_ID_SEQDX << 4) & 0xf0;
}

static inline void mp2vseqdx_set_format(uint8_t *p_mp2vseqdx,
                                        uint8_t i_format)
{
    p_mp2vseqdx[4] &= 0xf1;
    p_mp2vseqdx[4] |= (i_format << 1) & 0xe;
}

static inline uint8_t mp2vseqdx_get_format(const uint8_t *p_mp2vseqdx)
{
    return ((p_mp2vseqdx[4] & 0x0e) >> 1);
}

static inline void mp2vseqdx_set_color(uint8_t *p_mp2vseqdx)
{
    p_mp2vseqdx[4] |= 0x1;
}

static inline bool mp2vseqdx_get_color(const uint8_t *p_mp2vseqdx)
{
    return !!(p_mp2vseqdx[4] & 0x1);
}

static inline void mp2vseqdx_set_primaries(uint8_t *p_mp2vseqdx,
                                                 uint8_t i_primaries)
{
    mp2vseqdx_set_color(p_mp2vseqdx);
    p_mp2vseqdx[5] = i_primaries;
}

static inline uint8_t mp2vseqdx_get_primaries(const uint8_t *p_mp2vseqdx)
{
    if (!mp2vseqdx_get_color(p_mp2vseqdx))
        return 1;
    return p_mp2vseqdx[5];
}

static inline void mp2vseqdx_set_transfer(uint8_t *p_mp2vseqdx,
                                          uint8_t i_transfer)
{
    mp2vseqdx_set_color(p_mp2vseqdx);
    p_mp2vseqdx[6] = i_transfer;
}

static inline uint8_t mp2vseqdx_get_transfer(const uint8_t *p_mp2vseqdx)
{
    if (!mp2vseqdx_get_color(p_mp2vseqdx))
        return 1;
    return p_mp2vseqdx[6];
}

static inline void mp2vseqdx_set_matrixcoeffs(uint8_t *p_mp2vseqdx,
                                              uint8_t i_matrixcoeffs)
{
    mp2vseqdx_set_color(p_mp2vseqdx);
    p_mp2vseqdx[7] = i_matrixcoeffs;
}

static inline uint8_t mp2vseqdx_get_matrixcoeffs(const uint8_t *p_mp2vseqdx)
{
    if (!mp2vseqdx_get_color(p_mp2vseqdx))
        return 1;
    return p_mp2vseqdx[7];
}

static inline void mp2vseqdx_set_horizontal(uint8_t *p_mp2vseqdx,
                                            uint16_t i_horizontal)
{
    unsigned int base = mp2vseqdx_get_color(p_mp2vseqdx) ? 8 : 5;
    p_mp2vseqdx[base] &= 0xfe;
    p_mp2vseqdx[base] = (i_horizontal >> 6) & 0xff;
    p_mp2vseqdx[base + 1] &= 0x1;
    p_mp2vseqdx[base + 1] |= ((i_horizontal << 2) & 0xfc) | 0x2;
}

static inline uint16_t mp2vseqdx_get_horizontal(const uint8_t *p_mp2vseqdx)
{
    unsigned int base = mp2vseqdx_get_color(p_mp2vseqdx) ? 8 : 5;
    return (p_mp2vseqdx[base] << 6) | (p_mp2vseqdx[base + 1] >> 2);
}

static inline void mp2vseqdx_set_vertical(uint8_t *p_mp2vseqdx,
                                          uint16_t i_vertical)
{
    unsigned int base = mp2vseqdx_get_color(p_mp2vseqdx) ? 8 : 5;
    p_mp2vseqdx[base + 1] &= 0xfe;
    p_mp2vseqdx[base + 1] |= (i_vertical >> 13) & 0x1;
    p_mp2vseqdx[base + 2] = (i_vertical >> 5) & 0xff;
    p_mp2vseqdx[base + 3] = i_vertical << 3;
}

static inline uint8_t mp2vseqdx_get_vertical(const uint8_t *p_mp2vseqdx)
{
    unsigned int base = mp2vseqdx_get_color(p_mp2vseqdx) ? 8 : 5;
    return ((p_mp2vseqdx[base + 1] & 0x1) << 13) |
           (p_mp2vseqdx[base + 2] << 5) | (p_mp2vseqdx[base + 3] >> 3);
}

/*****************************************************************************
 * MP2V GOP header
 *****************************************************************************/
#define MP2VGOP_HEADER_SIZE         8
#define MP2VGOP_START_CODE          0xb8

static inline void mp2vgop_init(uint8_t *p_mp2vgop)
{
    mp2vstart_init(p_mp2vgop, MP2VGOP_START_CODE);
    p_mp2vgop[7] = 0x0;
}

static inline void mp2vgop_set_timecode(uint8_t *p_mp2vgop, uint32_t i_timecode)
{
    p_mp2vgop[4] = i_timecode >> 17;
    p_mp2vgop[5] = (i_timecode >> 9) & 0xff;
    p_mp2vgop[6] = (i_timecode >> 1) & 0xff;
    p_mp2vgop[7] &= 0x7f;
    p_mp2vgop[7] |= (i_timecode << 7) & 0x80;
}

static inline uint16_t mp2vgop_get_timecode(const uint8_t *p_mp2vgop)
{
    return (p_mp2vgop[4] << 17) | (p_mp2vgop[5] << 9) | (p_mp2vgop[6] << 1) |
           (p_mp2vgop[7] >> 7);
}

static inline void mp2vgop_set_closedgop(uint8_t *p_mp2vgop)
{
    p_mp2vgop[7] |= 0x40;
}

static inline bool mp2vgop_get_closedgop(const uint8_t *p_mp2vgop)
{
    return !!(p_mp2vgop[7] & 0x40);
}

static inline void mp2vgop_set_brokenlink(uint8_t *p_mp2vgop)
{
    p_mp2vgop[7] |= 0x20;
}

static inline bool mp2vgop_get_brokenlink(const uint8_t *p_mp2vgop)
{
    return !!(p_mp2vgop[7] & 0x20);
}

/*****************************************************************************
 * MP2V picture header
 *****************************************************************************/
#define MP2VPIC_HEADER_SIZE         8
#define MP2VPIC_START_CODE          0x0
/* last slice header */
#define MP2VPIC_LAST_CODE           0xaf

#define MP2VPIC_TYPE_I              1
#define MP2VPIC_TYPE_P              2
#define MP2VPIC_TYPE_B              3
#define MP2VPIC_TYPE_D              4

static inline void mp2vpic_init(uint8_t *p_mp2vpic)
{
    mp2vstart_init(p_mp2vpic, MP2VPIC_START_CODE);
    p_mp2vpic[7] = 0x0;
}

static inline void mp2vpic_set_temporalreference(uint8_t *p_mp2vpic,
                                                 uint16_t i_temporalreference)
{
    p_mp2vpic[4] = i_temporalreference >> 2;
    p_mp2vpic[5] &= 0x3f;
    p_mp2vpic[5] |= (i_temporalreference << 6) & 0xff;
}

static inline uint16_t mp2vpic_get_temporalreference(const uint8_t *p_mp2vpic)
{
    return (p_mp2vpic[4] << 2) | (p_mp2vpic[5] >> 6);
}

static inline void mp2vpic_set_codingtype(uint8_t *p_mp2vpic,
                                          uint8_t i_codingtype)
{
    p_mp2vpic[5] &= ~0x38;
    p_mp2vpic[5] |= (i_codingtype << 3) & 0x38;
}

static inline uint16_t mp2vpic_get_codingtype(const uint8_t *p_mp2vpic)
{
    return (p_mp2vpic[5] >> 3) & 0x7;
}

static inline void mp2vpic_set_vbvdelay(uint8_t *p_mp2vpic, uint16_t i_vbvdelay)
{
    p_mp2vpic[5] &= 0xf8;
    p_mp2vpic[5] |= i_vbvdelay >> 13;
    p_mp2vpic[6] = (i_vbvdelay >> 5) & 0xff;
    p_mp2vpic[7] &= 0x7;
    p_mp2vpic[7] |= (i_vbvdelay << 3) & 0xf8;
}

static inline uint16_t mp2vpic_get_vbvdelay(const uint8_t *p_mp2vpic)
{
    return ((p_mp2vpic[5] & 0x7) << 13) | (p_mp2vpic[6] << 5) |
           (p_mp2vpic[7] >>3);
}

/*****************************************************************************
 * MP2V picture extension
 *****************************************************************************/
#define MP2VPICX_HEADER_SIZE        9

#define MP2VPICX_TOP_FIELD          1
#define MP2VPICX_BOTTOM_FIELD       2
#define MP2VPICX_FRAME_PICTURE      3

static inline void mp2vpicx_init(uint8_t *p_mp2vpic)
{
    mp2vstart_init(p_mp2vpic, MP2VX_START_CODE);
    p_mp2vpic[4] = (MP2VX_ID_PICX << 4) & 0xf0;
    p_mp2vpic[7] = 0;
    p_mp2vpic[8] = 0;
}

static inline void mp2vpicx_set_fcode00(uint8_t *p_mp2vpicx,
                                        uint8_t i_fcode00)
{
    p_mp2vpicx[4] &= 0xf0;
    p_mp2vpicx[4] |= i_fcode00 & 0xf;
}

static inline uint8_t mp2vpicx_get_fcode00(const uint8_t *p_mp2vpicx)
{
    return (p_mp2vpicx[4] & 0xf);
}

static inline void mp2vpicx_set_fcode01(uint8_t *p_mp2vpicx,
                                        uint8_t i_fcode01)
{
    p_mp2vpicx[5] &= 0x0f;
    p_mp2vpicx[5] |= (i_fcode01 << 4) & 0xf0;
}

static inline uint8_t mp2vpicx_get_fcode01(const uint8_t *p_mp2vpicx)
{
    return (p_mp2vpicx[5] >> 4);
}

static inline void mp2vpicx_set_fcode10(uint8_t *p_mp2vpicx,
                                        uint8_t i_fcode10)
{
    p_mp2vpicx[5] &= 0xf0;
    p_mp2vpicx[5] |= i_fcode10 & 0xf;
}

static inline uint8_t mp2vpicx_get_fcode10(const uint8_t *p_mp2vpicx)
{
    return (p_mp2vpicx[5] & 0xf);
}

static inline void mp2vpicx_set_fcode11(uint8_t *p_mp2vpicx,
                                        uint8_t i_fcode11)
{
    p_mp2vpicx[6] &= 0x0f;
    p_mp2vpicx[6] |= (i_fcode11 << 4) & 0xf0;
}

static inline uint8_t mp2vpicx_get_fcode11(const uint8_t *p_mp2vpicx)
{
    return (p_mp2vpicx[6] >> 4);
}

static inline void mp2vpicx_set_intradc(uint8_t *p_mp2vpicx,
                                        uint8_t i_intradc)
{
    p_mp2vpicx[6] &= 0xf3;
    p_mp2vpicx[6] |= (i_intradc << 2) & 0xc;
}

static inline uint8_t mp2vpicx_get_intradc(const uint8_t *p_mp2vpicx)
{
    return ((p_mp2vpicx[6] & 0x0c) >> 2);
}

static inline void mp2vpicx_set_structure(uint8_t *p_mp2vpicx,
                                          uint8_t i_structure)
{
    p_mp2vpicx[6] &= 0xfc;
    p_mp2vpicx[6] |= i_structure & 0x3;
}

static inline uint8_t mp2vpicx_get_structure(const uint8_t *p_mp2vpicx)
{
    return (p_mp2vpicx[6] & 0x3);
}

static inline void mp2vpicx_set_tff(uint8_t *p_mp2vpicx)
{
    p_mp2vpicx[7] |= 0x80;
}

static inline bool mp2vpicx_get_tff(const uint8_t *p_mp2vpicx)
{
    return !!(p_mp2vpicx[7] & 0x80);
}

static inline void mp2vpicx_set_framepreddct(uint8_t *p_mp2vpicx)
{
    p_mp2vpicx[7] |= 0x40;
}

static inline bool mp2vpicx_get_framepreddct(const uint8_t *p_mp2vpicx)
{
    return !!(p_mp2vpicx[7] & 0x40);
}

static inline void mp2vpicx_set_concealmentmv(uint8_t *p_mp2vpicx)
{
    p_mp2vpicx[7] |= 0x20;
}

static inline bool mp2vpicx_get_concealmentmv(const uint8_t *p_mp2vpicx)
{
    return !!(p_mp2vpicx[7] & 0x20);
}

static inline void mp2vpicx_set_qscale(uint8_t *p_mp2vpicx)
{
    p_mp2vpicx[7] |= 0x10;
}

static inline bool mp2vpicx_get_qscale(const uint8_t *p_mp2vpicx)
{
    return !!(p_mp2vpicx[7] & 0x10);
}

static inline void mp2vpicx_set_intravlc(uint8_t *p_mp2vpicx)
{
    p_mp2vpicx[7] |= 0x8;
}

static inline bool mp2vpicx_get_intravlc(const uint8_t *p_mp2vpicx)
{
    return !!(p_mp2vpicx[7] & 0x8);
}

static inline void mp2vpicx_set_alternatescan(uint8_t *p_mp2vpicx)
{
    p_mp2vpicx[7] |= 0x4;
}

static inline bool mp2vpicx_get_alternatescan(const uint8_t *p_mp2vpicx)
{
    return !!(p_mp2vpicx[7] & 0x4);
}

static inline void mp2vpicx_set_rff(uint8_t *p_mp2vpicx)
{
    p_mp2vpicx[7] |= 0x2;
}

static inline bool mp2vpicx_get_rff(const uint8_t *p_mp2vpicx)
{
    return !!(p_mp2vpicx[7] & 0x2);
}

static inline void mp2vpicx_set_chroma420(uint8_t *p_mp2vpicx)
{
    p_mp2vpicx[7] |= 0x1;
}

static inline bool mp2vpicx_get_chroma420(const uint8_t *p_mp2vpicx)
{
    return !!(p_mp2vpicx[7] & 0x1);
}

static inline void mp2vpicx_set_progressive(uint8_t *p_mp2vpicx)
{
    p_mp2vpicx[8] |= 0x80;
}

static inline bool mp2vpicx_get_progressive(const uint8_t *p_mp2vpicx)
{
    return !!(p_mp2vpicx[8] & 0x80);
}

static inline void mp2vpicx_set_compositedisplay(uint8_t *p_mp2vpicx)
{
    p_mp2vpicx[8] |= 0x40;
}

static inline bool mp2vpicx_get_compositedisplay(const uint8_t *p_mp2vpicx)
{
    return !!(p_mp2vpicx[8] & 0x40);
}

/*****************************************************************************
 * MP2V end sequence
 *****************************************************************************/
#define MP2VEND_HEADER_SIZE         4
#define MP2VEND_START_CODE          0xb7

static inline void mp2vend_init(uint8_t *p_mp2vend)
{
    mp2vstart_init(p_mp2vend, MP2VEND_START_CODE);
}

#ifdef __cplusplus
}
#endif

#endif
