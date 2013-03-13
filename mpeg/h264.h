/*****************************************************************************
 * h264.h: ISO/IEC 14496-10 (video)
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
 *  - ISO/IEC 14496-10 (advanced video coding)
 */

#ifndef __BITSTREAM_MPEG_H264_H__
#define __BITSTREAM_MPEG_H264_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */
#include <string.h>   /* memset */

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * H264 network abstraction layer (annex B)
 *****************************************************************************/
static inline void h264nal_init(uint8_t *p_h264nal)
{
    p_h264nal[0] = 0x0;
    p_h264nal[1] = 0x0;
    p_h264nal[2] = 0x1;
    p_h264nal[3] = 0;
}

static inline void h264nal_set_ref(uint8_t *p_h264nal, uint8_t ref)
{
    p_h264nal[3] &= 0x1f;
    p_h264nal[3] |= ref << 5;
}

static inline uint8_t h264nal_get_ref(const uint8_t *p_h264nal)
{
    return (p_h264nal[3] & 0x60) >> 5;
}

static inline void h264nal_set_type(uint8_t *p_h264nal, uint8_t type)
{
    p_h264nal[3] &= 0xe0;
    p_h264nal[3] |= type;
}

static inline uint8_t h264nal_get_type(const uint8_t *p_h264nal)
{
    return p_h264nal[3] & 0x1f;
}

static inline uint8_t h264nalst_get_ref(uint8_t start)
{
    return (start & 0x60) >> 5;
}

static inline uint8_t h264nalst_get_type(uint8_t start)
{
    return start & 0x1f;
}

/*****************************************************************************
 * H264 slices
 *****************************************************************************/
#define H264SLI_NONIDR_START_CODE   1
#define H264SLI_PARTA_START_CODE    2
#define H264SLI_PARTB_START_CODE    3
#define H264SLI_PARTC_START_CODE    4
#define H264SLI_IDR_START_CODE      5

/*****************************************************************************
 * H264 supplemental enhancement information
 *****************************************************************************/
#define H264SEI_START_CODE          6
#define H264SEI_HEADER_SIZE         4

static inline void h264sei_init(uint8_t *p_h264sei)
{
    h264nal_init(p_h264sei);
    h264nal_set_type(p_h264sei, H264SEI_START_CODE);
}

/*****************************************************************************
 * H264 sequence parameter set
 *****************************************************************************/
#define H264SPS_START_CODE          7
#define H264SPS_HEADER_SIZE         7

#define H264SPS_ID_MAX              32

static inline void h264sps_init(uint8_t *p_h264sps)
{
    h264nal_init(p_h264sps);
    h264nal_set_ref(p_h264sps, 1);
    h264nal_set_type(p_h264sps, H264SPS_START_CODE);
    p_h264sps[5] = 0x0;
}

static inline void h264sps_set_profile(uint8_t *p_h264sps, uint8_t i_profile)
{
    p_h264sps[4] = i_profile;
}

static inline uint8_t h264sps_get_profile(const uint8_t *p_h264sps)
{
    return p_h264sps[4];
}

static inline void h264sps_set_level(uint8_t *p_h264sps, uint8_t i_level)
{
    p_h264sps[6] = i_level;
}

static inline uint8_t h264sps_get_level(const uint8_t *p_h264sps)
{
    return p_h264sps[6];
}

/*****************************************************************************
 * H264 picture parameter set
 *****************************************************************************/
#define H264PPS_START_CODE          8
#define H264PPS_HEADER_SIZE         7

#define H264PPS_ID_MAX              256

static inline void h264pps_init(uint8_t *p_h264pps)
{
    h264nal_init(p_h264pps);
    h264nal_set_ref(p_h264pps, 1);
    h264nal_set_type(p_h264pps, H264PPS_START_CODE);
}

/*****************************************************************************
 * H264 access unit delimiter
 *****************************************************************************/
#define H264AUD_START_CODE          9
#define H264AUD_HEADER_SIZE         5

static inline void h264aud_init(uint8_t *p_h264aud)
{
    h264nal_init(p_h264aud);
    h264nal_set_type(p_h264aud, H264AUD_START_CODE);
    p_h264aud[4] = 0x10;
}

static inline void h264aud_set_pic_type(uint8_t *p_h264aud, uint8_t i_type)
{
    p_h264aud[4] = i_type << 5;
}

static inline uint8_t h264aud_get_pic_type(const uint8_t *p_h264aud)
{
    return p_h264aud[4] >> 5;
}

/*****************************************************************************
 * H264 end sequence
 *****************************************************************************/
#define H264ENDSEQ_START_CODE       10
#define H264ENDSEQ_HEADER_SIZE      4

static inline void h264endseq_init(uint8_t *p_h264endseq)
{
    h264nal_init(p_h264endseq);
    h264nal_set_type(p_h264endseq, H264ENDSEQ_START_CODE);
}

/*****************************************************************************
 * H264 end stream
 *****************************************************************************/
#define H264ENDSTR_START_CODE       11
#define H264ENDSTR_HEADER_SIZE      4

static inline void h264endstr_init(uint8_t *p_h264endstr)
{
    h264nal_init(p_h264endstr);
    h264nal_set_type(p_h264endstr, H264ENDSTR_START_CODE);
}

/*****************************************************************************
 * H264 sequence parameter set extension
 *****************************************************************************/
#define H264SPSX_START_CODE         13
#define H264SPSX_HEADER_SIZE        4

static inline void h264spsx_init(uint8_t *p_h264spsx)
{
    h264nal_init(p_h264spsx);
    h264nal_set_ref(p_h264spsx, 1);
    h264nal_set_type(p_h264spsx, H264SPSX_START_CODE);
}

/*****************************************************************************
 * H264 subset sequence parameter set
 *****************************************************************************/
#define H264SSPS_START_CODE         15
#define H264SSPS_HEADER_SIZE        4

static inline void h264ssps_init(uint8_t *p_h264ssps)
{
    h264nal_init(p_h264ssps);
    h264nal_set_ref(p_h264ssps, 1);
    h264nal_set_type(p_h264ssps, H264SSPS_START_CODE);
}

#ifdef __cplusplus
}
#endif

#endif
