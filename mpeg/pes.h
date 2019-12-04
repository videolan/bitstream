/*****************************************************************************
 * pes.h: ISO/IEC 13818-1 Packetized Elementary Stream
 *****************************************************************************
 * Copyright (C) 2010 VideoLAN
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

#ifndef __BITSTREAM_MPEG_PES_H__
#define __BITSTREAM_MPEG_PES_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */
#include <string.h>   /* memset */

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * PES header
 *****************************************************************************/
#define PES_HEADER_SIZE             6
#define PES_HEADER_SIZE_NOPTS       9
#define PES_HEADER_SIZE_PTS         14
#define PES_HEADER_SIZE_PTSDTS      19
#define PES_HEADER_OPTIONAL_SIZE    3
#define PES_HEADER_TS_SIZE          5

#define PES_STREAM_ID_MIN           0xbc
#define PES_STREAM_ID_PSM           0xbc
#define PES_STREAM_ID_PRIVATE_1     0xbd
#define PES_STREAM_ID_PADDING       0xbe
#define PES_STREAM_ID_PRIVATE_2     0xbf
#define PES_STREAM_ID_AUDIO_MPEG    0xc0 /* and following */
#define PES_STREAM_ID_VIDEO_MPEG    0xe0 /* and following */
#define PES_STREAM_ID_ECM           0xf0
#define PES_STREAM_ID_EMM           0xf1
#define PES_STREAM_ID_DSMCC         0xf2
#define PES_STREAM_ID_MHEG          0xf3
#define PES_STREAM_ID_H222_1_E      0xf8
#define PES_STREAM_ID_PSD           0xff

static inline void pes_init(uint8_t *p_pes)
{
    p_pes[0] = 0x0;
    p_pes[1] = 0x0;
    p_pes[2] = 0x1;
}

static inline void pes_set_streamid(uint8_t *p_pes, uint8_t i_stream_id)
{
    p_pes[3] = i_stream_id;
}

static inline uint8_t pes_get_streamid(const uint8_t *p_pes)
{
    return p_pes[3];
}

static inline void pes_set_length(uint8_t *p_pes, uint16_t i_length)
{
    p_pes[4] = i_length >> 8;
    p_pes[5] = i_length & 0xff;
}

static inline uint16_t pes_get_length(const uint8_t *p_pes)
{
    return (p_pes[4] << 8) | p_pes[5];
}

static inline void pes_set_headerlength(uint8_t *p_pes, uint8_t i_length)
{
    p_pes[6] = 0x80;
    p_pes[7] = 0x0;
    p_pes[8] = i_length;
    if ( i_length > 0 )
        memset( &p_pes[9], 0xff, i_length ); /* stuffing */
}

static inline uint8_t pes_get_headerlength(const uint8_t *p_pes)
{
    return p_pes[8];
}

static inline void pes_set_dataalignment(uint8_t *p_pes)
{
    p_pes[6] |= 0x4;
}

static inline bool pes_get_dataalignment(const uint8_t *p_pes)
{
    return !!(p_pes[6] & 0x4);
}

static inline bool pes_has_pts(const uint8_t *p_pes)
{
    return !!(p_pes[7] & 0x80);
}

static inline bool pes_has_dts(const uint8_t *p_pes)
{
    return (p_pes[7] & 0xc0) == 0xc0;
}

static inline void pes_set_pts(uint8_t *p_pes, uint64_t i_pts)
{
    p_pes[7] |= 0x80;
    if (p_pes[8] < 5)
        p_pes[8] = 5;
    uint8_t marker = pes_has_dts(p_pes) ? 0x30 : 0x20;
    p_pes[9] = marker | 0x1 | ((i_pts >> 29) & 0xe);
    p_pes[10] = (i_pts >> 22) & 0xff;
    p_pes[11] = 0x1 | ((i_pts >> 14) & 0xfe);
    p_pes[12] = (i_pts >> 7) & 0xff;
    p_pes[13] = 0x1 | ((i_pts << 1) & 0xfe);
}

static inline bool pes_validate_pts(const uint8_t *p_pes)
{
    return ((p_pes[9] & 0xe1) == 0x21)
            && (p_pes[11] & 0x1) && (p_pes[13] & 0x1);
}

static inline uint64_t pes_get_pts(const uint8_t *p_pes)
{
    return (((uint64_t)p_pes[9] & 0xe)) << 29 | (p_pes[10] << 22) |
           ((p_pes[11] & 0xfe) << 14) | (p_pes[12] << 7) |
           ((p_pes[13] & 0xfe) >> 1);
}

static inline void pes_set_dts(uint8_t *p_pes, uint64_t i_dts)
{
    p_pes[7] |= 0x40;
    if (p_pes[8] < 10)
        p_pes[8] = 10;
    p_pes[9] &= 0x0f;
    p_pes[9] |= 0x30;
    p_pes[14] = 0x11 | ((i_dts >> 29) & 0xe);
    p_pes[15] = (i_dts >> 22) & 0xff;
    p_pes[16] = 0x1 | ((i_dts >> 14) & 0xfe);
    p_pes[17] = (i_dts >> 7) & 0xff;
    p_pes[18] = 0x1 | ((i_dts << 1) & 0xfe);
}

static inline bool pes_validate_dts(const uint8_t *p_pes)
{
    return (p_pes[9] & 0x10) && ((p_pes[14] & 0xf1) == 0x11)
            && (p_pes[16] & 0x1) && (p_pes[18] & 0x1);
}

static inline uint64_t pes_get_dts(const uint8_t *p_pes)
{
    return (((uint64_t)p_pes[14] & 0xe)) << 29 | (p_pes[15] << 22) |
           ((p_pes[16] & 0xfe) << 14) | (p_pes[17] << 7) |
           ((p_pes[18] & 0xfe) >> 1);
}

static inline bool pes_validate(const uint8_t *p_pes)
{
    return (p_pes[0] == 0x0 && p_pes[1] == 0x0 && p_pes[2] == 0x1
             && p_pes[3] >= PES_STREAM_ID_MIN);
}

static inline bool pes_validate_header(const uint8_t *p_pes)
{
    return ((p_pes[6] & 0xc0) == 0x80);
}

/*****************************************************************************
 * PES payload
 *****************************************************************************/
static inline uint8_t *pes_payload(uint8_t *p_pes)
{
    return p_pes + PES_HEADER_SIZE + PES_HEADER_OPTIONAL_SIZE + pes_get_headerlength(p_pes);
}

#ifdef __cplusplus
}
#endif

#endif
