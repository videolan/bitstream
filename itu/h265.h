/*****************************************************************************
 * h265.h: ITU-T H.265 (video)
 *****************************************************************************
 * Copyright (C) 2016 VideoLAN
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
 *  - ITU-T H.265 (high efficiency video coding)
 */

#ifndef __BITSTREAM_ITU_H265_H__
#define __BITSTREAM_ITU_H265_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */
#include <string.h>   /* memset */

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * H265 network abstraction layer (annex B)
 *****************************************************************************/
#define H265NAL_TYPE_TRAIL_N      0 /* Trailing */
#define H265NAL_TYPE_TRAIL_R      1 /* Trailing Reference */
#define H265NAL_TYPE_TSA_N        2 /* Temporal Sublayer Access */
#define H265NAL_TYPE_TSA_R        3 /* Temporal Sublayer Access Reference */
#define H265NAL_TYPE_STSA_N       4 /* Stepwise Temporal Sublayer Access */
#define H265NAL_TYPE_STSA_R       5 /* Stepwise Temporal Sublayer Access Reference */
#define H265NAL_TYPE_RADL_N       6 /* Random Access Decodable Leading (display order) */
#define H265NAL_TYPE_RADL_R       7 /* Random Access Decodable Leading (display order) Reference */
#define H265NAL_TYPE_RASL_N       8 /* Random Access Skipped Leading (display order) */
#define H265NAL_TYPE_RASL_R       9 /* Random Access Skipped Leading (display order) Reference */
/* 10 to 15 reserved */
#define H265NAL_TYPE_RSV_VCL_N10  10
#define H265NAL_TYPE_RSV_VCL_N12  12
#define H265NAL_TYPE_RSV_VCL_N14  14
/* Key frames */
#define H265NAL_TYPE_BLA_W_LP     16 /* Broken Link Access with Associated RASL */
#define H265NAL_TYPE_BLA_W_RADL   17 /* Broken Link Access with Associated RADL */
#define H265NAL_TYPE_BLA_N_LP     18 /* Broken Link Access */
#define H265NAL_TYPE_IDR_W_RADL   19 /* Instantaneous Decoder Refresh with Associated RADL */
#define H265NAL_TYPE_IDR_N_LP     20 /* Instantaneous Decoder Refresh */
#define H265NAL_TYPE_CRA          21 /* Clean Random Access */
/* 22 to 31 reserved */
#define H265NAL_TYPE_IRAP_VCL22   22 /* Intra Random Access Point */
#define H265NAL_TYPE_IRAP_VCL23   23
/* Non VCL NAL*/
#define H265NAL_TYPE_VPS          32
#define H265NAL_TYPE_SPS          33
#define H265NAL_TYPE_PPS          34
#define H265NAL_TYPE_AUD          35 /* Access unit delimiter */
#define H265NAL_TYPE_EOS          36 /* End of sequence */
#define H265NAL_TYPE_EOB          37 /* End of bitstream */
#define H265NAL_TYPE_FD           38 /* Filler data*/
#define H265NAL_TYPE_PREF_SEI     39 /* Prefix SEI */
#define H265NAL_TYPE_SUFF_SEI     40 /* Suffix SEI */
/* 41 to 47 reserved */
#define H265NAL_TYPE_RSV_NVCL41   41 /* Reserved Non VCL */
#define H265NAL_TYPE_RSV_NVCL44   44
#define H265NAL_TYPE_RSV_NVCL45   45
#define H265NAL_TYPE_RSV_NVCL47   47
#define H265NAL_TYPE_UNSPEC48     48 /* Unspecified (custom) */
#define H265NAL_TYPE_UNSPEC55     55
#define H265NAL_TYPE_UNSPEC56     56
#define H265NAL_TYPE_UNSPEC63     63

static inline void h265nal_init(uint8_t *p_h265nal)
{
    p_h265nal[0] = 0x0;
    p_h265nal[1] = 0x0;
    p_h265nal[2] = 0x1;
    p_h265nal[3] = 0;
    p_h265nal[4] = 0;
}

static inline void h265nal_set_type(uint8_t *p_h265nal, uint8_t i_type)
{
    p_h265nal[3] &= ~0x7e;
    p_h265nal[3] |= (i_type << 1) & 0x7e;
}

static inline uint8_t h265nal_get_type(const uint8_t *p_h265nal)
{
    return (p_h265nal[3] & 0x7e) >> 1;
}

static inline void h265nal_set_layerid(uint8_t *p_h265nal, uint8_t i_layerid)
{
    p_h265nal[3] &= ~0x1;
    p_h265nal[4] &= ~0xf8;
    p_h265nal[3] |= (i_layerid >> 5) & 0x1;
    p_h265nal[4] |= (i_layerid << 3) & 0xf8;
}

static inline uint8_t h265nal_get_layerid(const uint8_t *p_h265nal)
{
    return ((p_h265nal[3] & 0x1) << 5) | ((p_h265nal[4] & 0xf8) >> 3);
}

static inline void h265nal_set_temporalid1(uint8_t *p_h265nal, uint8_t i_temporalid1)
{
    p_h265nal[4] &= ~0x7;
    p_h265nal[4] |= i_temporalid1 & 0x7;
}

static inline uint8_t h265nal_get_temporalid1(const uint8_t *p_h265nal)
{
    return p_h265nal[4] & 0x7;
}

static inline uint8_t h265nalst_get_type(uint8_t start)
{
    return (start & 0x7e) >> 1;
}

/*****************************************************************************
 * H265 supplemental enhancement information
 *****************************************************************************/
#define H265SEI_HEADER_SIZE         5

#define H265SEI_BUFFERING_PERIOD    0
#define H265SEI_PIC_TIMING          1
#define H265SEI_PAN_SCAN_RECT       2
#define H265SEI_FILLER_PAYLOAD      3
#define H265SEI_USER_T_T35          4
#define H265SEI_USER_UNREGISTERED   5
#define H265SEI_RECOVERY_POINT      6
#define H265SEI_SCENE_INFO          9
#define H265SEI_FULL_FRAME_SNAP     15
#define H265SEI_PROG_REF_SEGMENT_S  16
#define H265SEI_PROG_REF_SEGMENT_E  17
#define H265SEI_FILM_GRAIN_CHAR     19
#define H265SEI_POST_FILTER_HINT    22
#define H265SEI_TONE_MAPPING_INFO   23
#define H265SEI_FRAME_PACK_ARRANG   45
#define H265SEI_DISPLAY_ORIENTATION 47
#define H265SEI_STRUCT_PICT_INFO    128
#define H265SEI_ACTIVE_PS           129
#define H265SEI_DEC_UNIT_INFO       130
#define H265SEI_TEMP_SUBL_ZERO_IDX  131
#define H265SEI_DEC_PICTURE_HASH    132
#define H265SEI_SCALABLE_NESTING    133
#define H265SEI_REGION_REFRESH_INFO 134

#define H265SEI_STRUCT_FRAME        0
#define H265SEI_STRUCT_TOP          1
#define H265SEI_STRUCT_BOT          2
#define H265SEI_STRUCT_TOP_BOT      3
#define H265SEI_STRUCT_BOT_TOP      4
#define H265SEI_STRUCT_TOP_BOT_TOP  5
#define H265SEI_STRUCT_BOT_TOP_BOT  6
#define H265SEI_STRUCT_DOUBLE       7
#define H265SEI_STRUCT_TRIPLE       8
#define H265SEI_STRUCT_TOP_PREV_BOT 9
#define H265SEI_STRUCT_BOT_PREV_TOP 10
#define H265SEI_STRUCT_TOP_NEXT_BOT 11
#define H265SEI_STRUCT_BOT_NEXT_TOP 12

static inline void h265sei_init(uint8_t *p_h265sei, bool b_prefix)
{
    h265nal_init(p_h265sei);
    h265nal_set_type(p_h265sei,
            b_prefix ? H265NAL_TYPE_PREF_SEI : H265NAL_TYPE_SUFF_SEI);
}

/*****************************************************************************
 * H265 profile tier level
 *****************************************************************************/
#define H265PTL_HEADER_SIZE         12
#define H265PTL_PROFILE_SIZE        11

/*****************************************************************************
 * H265 video parameter set
 *****************************************************************************/
#define H265VPS_HEADER_SIZE         9

#define H265VPS_ID_MAX              16

#define H265VPS_LEVEL_1_0           30
#define H265VPS_LEVEL_2_0           60
#define H265VPS_LEVEL_2_1           63
#define H265VPS_LEVEL_3_0           90
#define H265VPS_LEVEL_3_1           93
#define H265VPS_LEVEL_4_0           120
#define H265VPS_LEVEL_4_1           123
#define H265VPS_LEVEL_5_0           150
#define H265VPS_LEVEL_5_1           153
#define H265VPS_LEVEL_5_2           156
#define H265VPS_LEVEL_6_0           180
#define H265VPS_LEVEL_6_1           183
#define H265VPS_LEVEL_6_2           186

static inline void h265vps_init(uint8_t *p_h265vps)
{
    h265nal_init(p_h265vps);
    h265nal_set_type(p_h265vps, H265NAL_TYPE_VPS);
    p_h265vps[5] = 0x6;
}

static inline void h265vps_set_vps_id(uint8_t *p_h265vps, uint8_t i_vps_id)
{
    p_h265vps[5] &= ~0xf0;
    p_h265vps[5] |= (i_vps_id << 4) & 0xf0;
}

static inline uint8_t h265vps_get_vps_id(const uint8_t *p_h265vps)
{
    return (p_h265vps[5] & 0xf0) >> 4;
}

/*****************************************************************************
 * H265 sequence parameter set
 *****************************************************************************/
#define H265SPS_HEADER_SIZE         6

#define H265SPS_ID_MAX              16

#define H265SPS_CHROMA_MONO         0
#define H265SPS_CHROMA_420          1
#define H265SPS_CHROMA_422          2
#define H265SPS_CHROMA_444          3

static inline void h265sps_init(uint8_t *p_h265sps)
{
    h265nal_init(p_h265sps);
    h265nal_set_type(p_h265sps, H265NAL_TYPE_SPS);
    p_h265sps[5] = 0x0;
}

static inline void h265sps_set_vps_id(uint8_t *p_h265sps, uint8_t i_vps_id)
{
    p_h265sps[5] &= ~0xf0;
    p_h265sps[5] |= (i_vps_id << 4) & 0xf0;
}

static inline uint8_t h265sps_get_vps_id(const uint8_t *p_h265sps)
{
    return (p_h265sps[5] & 0xf0) >> 4;
}

/*****************************************************************************
 * H265 picture parameter set
 *****************************************************************************/
#define H265PPS_HEADER_SIZE         5

#define H265PPS_ID_MAX              64

static inline void h265pps_init(uint8_t *p_h265pps)
{
    h265nal_init(p_h265pps);
    h265nal_set_type(p_h265pps, H265NAL_TYPE_PPS);
}

/*****************************************************************************
 * H265 access unit delimiter
 *****************************************************************************/
#define H265AUD_HEADER_SIZE         6

static inline void h265aud_init(uint8_t *p_h265aud)
{
    h265nal_init(p_h265aud);
    h265nal_set_type(p_h265aud, H265NAL_TYPE_AUD);
    p_h265aud[5] = 0x10;
}

static inline void h265aud_set_pic_type(uint8_t *p_h265aud, uint8_t i_type)
{
    p_h265aud[5] = i_type << 5;
}

static inline uint8_t h265aud_get_pic_type(const uint8_t *p_h265aud)
{
    return p_h265aud[5] >> 5;
}

/*****************************************************************************
 * H265 end sequence
 *****************************************************************************/
#define H265EOS_HEADER_SIZE         5

static inline void h265eos_init(uint8_t *p_h265eos)
{
    h265nal_init(p_h265eos);
    h265nal_set_type(p_h265eos, H265NAL_TYPE_EOS);
}

/*****************************************************************************
 * H265 end bitstream
 *****************************************************************************/
#define H265EOB_HEADER_SIZE      5

static inline void h265eob_init(uint8_t *p_h265eob)
{
    h265nal_init(p_h265eob);
    h265nal_set_type(p_h265eob, H265NAL_TYPE_EOB);
}

/*****************************************************************************
 * H265 video usability information
 *****************************************************************************/
#define H265VUI_AR_EXTENDED         255

/*****************************************************************************
 * H265 slices
 *****************************************************************************/
#define H265SLI_TYPE_B              0
#define H265SLI_TYPE_P              1
#define H265SLI_TYPE_I              2

#ifdef __cplusplus
}
#endif

#endif
