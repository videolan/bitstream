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
 *  - ISO/IEC 14496-15 (advanced video coding file format)
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
#define H264NAL_TYPE_NONIDR             1
#define H264NAL_TYPE_PARTA              2
#define H264NAL_TYPE_PARTB              3
#define H264NAL_TYPE_PARTC              4
#define H264NAL_TYPE_IDR                5
#define H264NAL_TYPE_SEI                6
#define H264NAL_TYPE_SPS                7
#define H264NAL_TYPE_PPS                8
#define H264NAL_TYPE_AUD                9
#define H264NAL_TYPE_ENDSEQ             10
#define H264NAL_TYPE_ENDSTR             11
#define H264NAL_TYPE_FILLER             12
#define H264NAL_TYPE_SPSX               13
#define H264NAL_TYPE_PFX                14
#define H264NAL_TYPE_SSPS               15

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

static inline void h264nalst_init(uint8_t *p_start)
{
    p_start[0] = 0;
}

static inline void h264nalst_set_ref(uint8_t *p_start, uint8_t ref)
{
    p_start[0] &= 0x1f;
    p_start[0] |= ref << 5;
}

static inline uint8_t h264nalst_get_ref(uint8_t start)
{
    return (start & 0x60) >> 5;
}

static inline void h264nalst_set_type(uint8_t *p_start, uint8_t type)
{
    p_start[0] &= 0xe0;
    p_start[0] |= type;
}

static inline uint8_t h264nalst_get_type(uint8_t start)
{
    return start & 0x1f;
}

static inline bool h264naltype_is_slice(uint8_t type)
{
    switch (type) {
        case H264NAL_TYPE_IDR:
        case H264NAL_TYPE_NONIDR:
        case H264NAL_TYPE_PARTA:
        case H264NAL_TYPE_PARTB:
        case H264NAL_TYPE_PARTC:
            return true;
        default:
            return false;
    }
}

static inline bool h264naltype_is_vcl(uint8_t type)
{
    switch (type) {
        case H264NAL_TYPE_IDR:
        case H264NAL_TYPE_NONIDR:
        case H264NAL_TYPE_PARTA:
        case H264NAL_TYPE_PARTB:
        case H264NAL_TYPE_PARTC:
        case H264NAL_TYPE_SEI:
            return true;
        default:
            return false;
    }
}

/*****************************************************************************
 * H264 supplemental enhancement information
 *****************************************************************************/
#define H264SEI_HEADER_SIZE         4

#define H264SEI_BUFFERING_PERIOD    0
#define H264SEI_PIC_TIMING          1
#define H264SEI_PAN_SCAN_RECT       2
#define H264SEI_FILLER_PAYLOAD      3
#define H264SEI_USER_T_T35          4
#define H264SEI_USER_UNREGISTERED   5
#define H264SEI_RECOVERY_POINT      6
#define H264SEI_DEC_REF_PIC_MARK_R  7
#define H264SEI_SPARE_PIC           8
#define H264SEI_SCENE_INFO          9
#define H264SEI_SUB_SEQ_INFO        10
#define H264SEI_SUB_SEQ_LAYER_CHAR  11
#define H264SEI_SUB_SEQ_CHAR        12
#define H264SEI_FULL_FRAME_FREEZE   13
#define H264SEI_FULL_FRAME_FREEZE_R 14
#define H264SEI_FULL_FRAME_SNAP     15
#define H264SEI_PROG_REF_SEGMENT_S  16
#define H264SEI_PROG_REF_SEGMENT_E  17
#define H264SEI_MOTION_CONS_SGRP_S  18
#define H264SEI_FILM_GRAIN_CHAR     19
#define H264SEI_DEBLOCK_DIS_PREF    20
#define H264SEI_STEREO_VIDEO_INFO   21
#define H264SEI_POST_FILTER_HINT    22
#define H264SEI_TONE_MAPPING_INFO   23
#define H264SEI_SCALABILITY_INFO    24
#define H264SEI_SUBPIC_SCAL_LAYER   25
#define H264SEI_NONREQ_LAYER_REP    26
#define H264SEI_PRIO_LAYER_INFO     27
#define H264SEI_LAYERS_NOT_PRES     28
#define H264SEI_LAYER_DEP_CHANGE    29
#define H264SEI_SCALABLE_NESTING    30
#define H264SEI_BASE_LAYER_TEMP_HRD 31
#define H264SEI_QUAL_LAYER_INT_CHK  32
#define H264SEI_REDUND_PIC_PROP     33
#define H264SEI_TL0_DEP_REP         34
#define H264SEI_TL_SWITCHING_POINT  35
#define H264SEI_PARALLEL_DEC_INFO   36
#define H264SEI_MVC_SCAL_NESTING    37
#define H264SEI_VIEW_SCAL_INFO      38
#define H264SEI_MULTIVIEW_SCN_INFO  39
#define H264SEI_MULTIVIEW_ACQ_INFO  40
#define H264SEI_NONREQ_VIEW_COMP    41
#define H264SEI_VIEW_DEP_CHANGE     42
#define H264SEI_OP_POINTS_NOT_PRES  43
#define H264SEI_BASE_VIEW_TEMP_HRD  44
#define H264SEI_FRAME_PACK_ARRANG   45

#define H264SEI_STRUCT_FRAME        0
#define H264SEI_STRUCT_TOP          1
#define H264SEI_STRUCT_BOT          2
#define H264SEI_STRUCT_TOP_BOT      3
#define H264SEI_STRUCT_BOT_TOP      4
#define H264SEI_STRUCT_TOP_BOT_TOP  5
#define H264SEI_STRUCT_BOT_TOP_BOT  6
#define H264SEI_STRUCT_DOUBLE       7
#define H264SEI_STRUCT_TRIPLE       8

static inline void h264sei_init(uint8_t *p_h264sei)
{
    h264nal_init(p_h264sei);
    h264nal_set_type(p_h264sei, H264NAL_TYPE_SEI);
}

/*****************************************************************************
 * H264 sequence parameter set
 *****************************************************************************/
#define H264SPS_HEADER_SIZE         7

#define H264SPS_ID_MAX              32

#define H264SPS_CHROMA_MONO         0
#define H264SPS_CHROMA_420          1
#define H264SPS_CHROMA_422          2
#define H264SPS_CHROMA_444          3

static inline void h264sps_init(uint8_t *p_h264sps)
{
    h264nal_init(p_h264sps);
    h264nal_set_ref(p_h264sps, 1);
    h264nal_set_type(p_h264sps, H264NAL_TYPE_SPS);
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

/* Note: doesn't do the constrained profile checks */
static inline const char *h264_sps_get_profile_txt(uint8_t i_profile)
{
    return i_profile == 44 ? "CAVLC 4:4:4 Intra" :
           i_profile == 66 ? "Baseline" :
           i_profile == 77 ? "Main" :
           i_profile == 88 ? "Extended" :
           i_profile == 100 ? "High" :
           i_profile == 110 ? "High 10" :
           i_profile == 122 ? "High 4:2:2" :
           i_profile == 144 ? "High 4:4:4 Predictive" : "Reserved";
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
#define H264PPS_HEADER_SIZE         7

#define H264PPS_ID_MAX              256

static inline void h264pps_init(uint8_t *p_h264pps)
{
    h264nal_init(p_h264pps);
    h264nal_set_ref(p_h264pps, 1);
    h264nal_set_type(p_h264pps, H264NAL_TYPE_PPS);
}

/*****************************************************************************
 * H264 access unit delimiter
 *****************************************************************************/
#define H264AUD_HEADER_SIZE         5

static inline void h264aud_init(uint8_t *p_h264aud)
{
    h264nal_init(p_h264aud);
    h264nal_set_type(p_h264aud, H264NAL_TYPE_AUD);
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
#define H264ENDSEQ_HEADER_SIZE      4

static inline void h264endseq_init(uint8_t *p_h264endseq)
{
    h264nal_init(p_h264endseq);
    h264nal_set_type(p_h264endseq, H264NAL_TYPE_ENDSEQ);
}

/*****************************************************************************
 * H264 end stream
 *****************************************************************************/
#define H264ENDSTR_HEADER_SIZE      4

static inline void h264endstr_init(uint8_t *p_h264endstr)
{
    h264nal_init(p_h264endstr);
    h264nal_set_type(p_h264endstr, H264NAL_TYPE_ENDSTR);
}

/*****************************************************************************
 * H264 sequence parameter set extension
 *****************************************************************************/
#define H264SPSX_HEADER_SIZE        4

static inline void h264spsx_init(uint8_t *p_h264spsx)
{
    h264nal_init(p_h264spsx);
    h264nal_set_ref(p_h264spsx, 1);
    h264nal_set_type(p_h264spsx, H264NAL_TYPE_SPSX);
}

/*****************************************************************************
 * H264 subset sequence parameter set
 *****************************************************************************/
#define H264SSPS_HEADER_SIZE        4

static inline void h264ssps_init(uint8_t *p_h264ssps)
{
    h264nal_init(p_h264ssps);
    h264nal_set_ref(p_h264ssps, 1);
    h264nal_set_type(p_h264ssps, H264NAL_TYPE_SSPS);
}

/*****************************************************************************
 * H264 video usability information
 *****************************************************************************/
#define H264VUI_AR_EXTENDED         255

/*****************************************************************************
 * H264 slices
 *****************************************************************************/
#define H264SLI_TYPE_P              0
#define H264SLI_TYPE_B              1
#define H264SLI_TYPE_I              2
#define H264SLI_TYPE_SP             3
#define H264SLI_TYPE_SI             4

/*****************************************************************************
 * H264 avcC structure
 *****************************************************************************/
#define H264AVCC_HEADER             6
#define H264AVCC_HEADER2            1
#define H264AVCC_SPS_HEADER         2
#define H264AVCC_PPS_HEADER         2

static inline void h264avcc_init(uint8_t *p)
{
    p[0] = 1; /* version */
    p[4] = 0xfc;
    p[5] = 0xe0;
}

static inline void h264avcc_set_profile(uint8_t *p, uint8_t val)
{
    p[1] = val;
}

static inline uint8_t h264avcc_get_profile(const uint8_t *p)
{
    return p[1];
}

static inline void h264avcc_set_profile_compatibility(uint8_t *p, uint8_t val)
{
    p[2] = val;
}

static inline uint8_t h264avcc_get_profile_compatibility(const uint8_t *p)
{
    return p[2];
}

static inline void h264avcc_set_level(uint8_t *p, uint8_t val)
{
    p[3] = val;
}

static inline uint8_t h264avcc_get_level(const uint8_t *p)
{
    return p[3];
}

static inline void h264avcc_set_length_size_1(uint8_t *p, uint8_t val)
{
    p[4] = 0xfc | val;
}

static inline uint8_t h264avcc_get_length_size_1(const uint8_t *p)
{
    return p[4] & 0x3;
}

static inline void h264avcc_set_nb_sps(uint8_t *p, uint8_t val)
{
    p[5] = 0xe0 | val;
}

static inline uint8_t h264avcc_get_nb_sps(const uint8_t *p)
{
    return p[5] & 0x1f;
}

static inline void h264avcc_spsh_set_length(uint8_t *p, uint16_t val)
{
    p[0] = val >> 8;
    p[1] = val & 0xff;
}

static inline uint16_t h264avcc_spsh_get_length(const uint8_t *p)
{
    return (p[0] << 8) | p[1];
}

static inline uint8_t *h264avcc_spsh_get_sps(const uint8_t *p)
{
    return (uint8_t *)p + H264AVCC_SPS_HEADER;
}

static inline uint8_t *h264avcc_get_spsh(const uint8_t *p, uint8_t n)
{
    p += H264AVCC_HEADER;
    while (n) {
        uint16_t length = h264avcc_spsh_get_length(p);
        p += H264AVCC_SPS_HEADER + length;
        n--;
    }
    return (uint8_t *)p;
}

static inline void h264avcc_set_nb_pps(uint8_t *p, uint8_t val)
{
    p = h264avcc_get_spsh(p, h264avcc_get_nb_sps(p));
    p[0] = val;
}

static inline uint8_t h264avcc_get_nb_pps(const uint8_t *p)
{
    p = h264avcc_get_spsh(p, h264avcc_get_nb_sps(p));
    return p[0];
}

static inline void h264avcc_ppsh_set_length(uint8_t *p, uint16_t val)
{
    p[0] = val >> 8;
    p[1] = val & 0xff;
}

static inline uint16_t h264avcc_ppsh_get_length(const uint8_t *p)
{
    return (p[0] << 8) | p[1];
}

static inline uint8_t *h264avcc_ppsh_get_pps(const uint8_t *p)
{
    return (uint8_t *)p + H264AVCC_PPS_HEADER;
}

static inline uint8_t *h264avcc_get_ppsh(const uint8_t *p, uint8_t n)
{
    p = h264avcc_get_spsh(p, h264avcc_get_nb_sps(p));
    p += H264AVCC_HEADER2;
    while (n) {
        uint16_t length = h264avcc_ppsh_get_length(p);
        p += H264AVCC_PPS_HEADER + length;
        n--;
    }
    return (uint8_t *)p;
}

static inline bool h264avcc_validate(const uint8_t *p, size_t size)
{
    if (p[0] != 1)
        return false;
    /* don't check reserved bits as it is a common mistake */

    if (size < H264AVCC_HEADER + H264AVCC_HEADER2)
        return false;

    uint8_t nb = h264avcc_get_nb_sps(p);
    uint8_t n = 0;
    while (n < nb)
        if (h264avcc_get_spsh(p, n++) + H264AVCC_SPS_HEADER > p + size)
            return false;

    if (h264avcc_get_spsh(p, n) + H264AVCC_HEADER2 > p + size)
        return false;

    nb = h264avcc_get_nb_pps(p);
    n = 0;
    while (n < nb)
        if (h264avcc_get_ppsh(p, n++) + H264AVCC_PPS_HEADER > p + size)
            return false;

    if (h264avcc_get_ppsh(p, n) > p + size)
        return false;
    return true;
}

#ifdef __cplusplus
}
#endif

#endif
