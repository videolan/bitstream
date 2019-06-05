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
 *  - ISO/IEC 14496-15 2013 (AVC file format)
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

/*****************************************************************************
 * H265 hvcC structure
 *****************************************************************************/
#define H265HVCC_HEADER             23
#define H265HVCC_ARRAY_HEADER       3
#define H265HVCC_NALU_HEADER        2

static inline void h265hvcc_init(uint8_t *p)
{
    p[0] = 1; /* version */
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    p[4] = 0;
    p[5] = 0;
    p[6] = 0;
    p[7] = 0;
    p[8] = 0;
    p[9] = 0;
    p[10] = 0;
    p[11] = 0;
    p[12] = 0;
    p[13] = 0xf0;
    p[14] = 0;
    p[15] = 0xfc;
    p[16] = 0xfc;
    p[17] = 0xf8;
    p[18] = 0xf8;
    p[19] = 0;
    p[20] = 0;
    p[21] = 0;
    p[22] = 0;
}

static inline void h265hvcc_set_profile_space(uint8_t *p, uint8_t val)
{
    p[1] &= ~0xc0;
    p[1] |= val << 6;
}

static inline uint8_t h265hvcc_get_profile_space(const uint8_t *p)
{
    return p[1] >> 6;
}

static inline void h265hvcc_set_tier(uint8_t *p)
{
    p[1] |= 0x40;
}

static inline bool h265hvcc_get_tier(const uint8_t *p)
{
    return p[1] & 0x40;
}

static inline void h265hvcc_set_profile_idc(uint8_t *p, uint8_t val)
{
    p[1] &= ~0x1f;
    p[1] |= val & 0x1f;
}

static inline uint8_t h265hvcc_get_profile_idc(const uint8_t *p)
{
    return p[1] & 0x1f;
}

static inline void h265hvcc_set_profile_compatibility(uint8_t *p, uint32_t val)
{
    p[2] = val >> 24;
    p[3] = (val >> 16) & 0xff;
    p[4] = (val >> 8) & 0xff;
    p[5] = val & 0xff;
}

static inline uint32_t h265hvcc_get_profile_compatibility(const uint8_t *p)
{
    return (p[2] << 24) | (p[3] << 16) | (p[4] << 8) | p[5];
}

static inline void h265hvcc_set_constraint_indicator(uint8_t *p, uint64_t val)
{
    p[6] = val >> 40;
    p[7] = (val >> 32) & 0xff;
    p[8] = (val >> 24) & 0xff;
    p[9] = (val >> 16) & 0xff;
    p[10] = (val >> 8) & 0xff;
    p[11] = val & 0xff;
}

static inline uint64_t h265hvcc_get_constraint_indicator(const uint8_t *p)
{
    return ((uint64_t)p[6] << 40) |
           ((uint64_t)p[7] << 32) |
           ((uint64_t)p[8] << 24) |
           ((uint64_t)p[9] << 16) |
           ((uint64_t)p[10] << 8) |
           (uint64_t)p[11];
}

static inline void h265hvcc_set_level_idc(uint8_t *p, uint8_t val)
{
    p[12] = val;
}

static inline uint8_t h265hvcc_get_level_idc(const uint8_t *p)
{
    return p[12];
}

static inline void h265hvcc_set_min_spatial_segmentation_idc(uint8_t *p, uint16_t val)
{
    p[13] = 0xf0 | (val >> 8);
    p[14] = val & 0xff;
}

static inline uint16_t h265hvcc_get_min_spatial_segmentation_idc(const uint8_t *p)
{
    return (p[13] & 0xf) << 8 | p[14];
}

static inline void h265hvcc_set_parallelism_type(uint8_t *p, uint8_t val)
{
    p[15] = 0xfc | val;
}

static inline uint8_t h265hvcc_get_parallelism_type(const uint8_t *p)
{
    return p[15] & 0x3;
}

static inline void h265hvcc_set_chroma_format(uint8_t *p, uint8_t val)
{
    p[16] = 0xfc | val;
}

static inline uint8_t h265hvcc_get_chroma_format(const uint8_t *p)
{
    return p[16] & 0x3;
}

static inline void h265hvcc_set_bitdepth_luma_8(uint8_t *p, uint8_t val)
{
    p[17] = 0xf8 | val;
}

static inline uint8_t h265hvcc_get_bitdepth_luma_8(const uint8_t *p)
{
    return p[17] & 0x7;
}

static inline void h265hvcc_set_bitdepth_chroma_8(uint8_t *p, uint8_t val)
{
    p[18] = 0xf8 | val;
}

static inline uint8_t h265hvcc_get_bitdepth_chroma_8(const uint8_t *p)
{
    return p[18] & 0x7;
}

static inline void h265hvcc_set_avg_frame_rate(uint8_t *p, uint16_t val)
{
    p[19] = val >> 8;
    p[20] = val & 0xff;
}

static inline uint16_t h265hvcc_get_avg_frame_rate(const uint8_t *p)
{
    return (p[19] << 8) | p[20];
}

static inline void h265hvcc_set_constant_frame_rate(uint8_t *p, uint8_t val)
{
    p[21] &= ~0xc0;
    p[21] |= val << 6;
}

static inline uint8_t h265hvcc_get_constant_frame_rate(const uint8_t *p)
{
    return p[21] >> 6;
}

static inline void h265hvcc_set_num_temporal_layers(uint8_t *p, uint8_t val)
{
    p[21] &= ~0x38;
    p[21] |= val << 3;
}

static inline uint8_t h265hvcc_get_num_temporal_layers(const uint8_t *p)
{
    return (p[21] >> 3) & 0x3;
}

static inline void h265hvcc_set_temporal_id_nested(uint8_t *p)
{
    p[21] |= 0x4;
}

static inline bool h265hvcc_get_temporal_id_nested(const uint8_t *p)
{
    return p[21] & 0x4;
}

static inline void h265hvcc_set_length_size_1(uint8_t *p, uint8_t val)
{
    p[21] &= ~0x3;
    p[21] |= val;
}

static inline uint8_t h265hvcc_get_length_size_1(const uint8_t *p)
{
    return p[21] & 0x3;
}

static inline void h265hvcc_set_num_of_arrays(uint8_t *p, uint8_t val)
{
    p[22] = val;
}

static inline uint8_t h265hvcc_get_num_of_arrays(const uint8_t *p)
{
    return p[22];
}

static inline void h265hvcc_nalu_set_length(uint8_t *p, uint16_t val)
{
    p[0] = val >> 8;
    p[1] = val & 0xff;
}

static inline uint16_t h265hvcc_nalu_get_length(const uint8_t *p)
{
    return (p[0] << 8) | p[1];
}

static inline uint8_t *h265hvcc_nalu_get_nalu(const uint8_t *p)
{
    return (uint8_t *)p + H265HVCC_NALU_HEADER;
}

static inline void h265hvcc_array_init(uint8_t *p)
{
    p[0] = 0;
}

static inline void h265hvcc_array_set_completeness(uint8_t *p)
{
    p[0] |= 0x80;
}

static inline bool h265hvcc_array_get_completeness(const uint8_t *p)
{
    return p[0] & 0x80;
}

static inline void h265hvcc_array_set_nal_unit_type(uint8_t *p, uint8_t val)
{
    p[0] &= ~0x3f;
    p[0] |= val;
}

static inline uint8_t h265hvcc_array_get_nal_unit_type(const uint8_t *p)
{
    return p[0] & 0x3f;
}

static inline void h265hvcc_array_set_num_nalus(uint8_t *p, uint16_t val)
{
    p[1] = val >> 8;
    p[2] = val & 0xff;
}

static inline uint16_t h265hvcc_array_get_num_nalus(const uint8_t *p)
{
    return (p[1] << 8) | p[2];
}

static inline uint8_t *h265hvcc_array_get_nalu(const uint8_t *p, uint8_t n)
{
    p += H265HVCC_ARRAY_HEADER;
    while (n) {
        uint16_t length = h265hvcc_nalu_get_length(p);
        p += H265HVCC_NALU_HEADER + length;
        n--;
    }
    return (uint8_t *)p;
}

static inline uint8_t *h265hvcc_get_array(const uint8_t *p, uint8_t n)
{
    p += H265HVCC_HEADER;
    while (n) {
        uint16_t num_nalus = h265hvcc_array_get_num_nalus(p);
        p = h265hvcc_array_get_nalu(p, num_nalus);
        n--;
    }
    return (uint8_t *)p;
}

static inline bool h265hvcc_validate(const uint8_t *p, size_t size)
{
    /* apparenty there is a prerelease with version = 0 */
    if (p[0] != 1 && p[0] != 0)
        return false;

    if (size < H265HVCC_HEADER)
        return false;

    uint8_t num_arrays = h265hvcc_get_num_of_arrays(p);
    uint8_t array = 0;
    while (array < num_arrays) {
        const uint8_t *a = h265hvcc_get_array(p, array++);
        if (a + H265HVCC_ARRAY_HEADER > p + size)
            return false;

        uint8_t num_nalus = h265hvcc_array_get_num_nalus(a);
        uint8_t nalu = 0;
        while (nalu < num_nalus)
            if (h265hvcc_array_get_nalu(a, nalu++) + H265HVCC_NALU_HEADER > p + size)
                return false;
    }

    if (h265hvcc_get_array(p, num_arrays) > p + size)
        return false;
    return true;
}

#ifdef __cplusplus
}
#endif

#endif
