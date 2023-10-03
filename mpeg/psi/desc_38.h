/*****************************************************************************
 * desc_38.h: ETSI EN 300 468 Descriptor 0x38: HEVC video descriptor
 *****************************************************************************
 * Copyright (C) 2022 EasyTools
 *
 * Authors: Arnaud de Turckheim <adeturckheim@easytools.tv>
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
 *  - ISO/IEC 13818-1:2021 (MPEG-2 Systems)
 */

#ifndef __BITSTREAM_MPEG_DESC_38_H__
#define __BITSTREAM_MPEG_DESC_38_H__

#include <inttypes.h>

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x38: HEVC video descriptor
 *****************************************************************************/
#define DESC38_HEADER_SIZE              (DESC_HEADER_SIZE + 13)
#define DESC38_TEMP_LAYER_SUBSET_SIZE   2

static inline void desc38_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x38);
    desc_set_length(p_desc, (DESC38_HEADER_SIZE - DESC_HEADER_SIZE));
}

BITSTREAM_GET_SET(desc38, profile_space, uint8, 16, 2);
BITSTREAM_GET_SET(desc38, tier_flag, uint8, 18, 1);
BITSTREAM_GET_SET(desc38, profile_idc, uint8, 19, 5);
BITSTREAM_GET_SET(desc38, profile_compatibility_indication, uint32, 24, 32);
BITSTREAM_GET_SET(desc38, progressive_source_flag, uint8, 56, 1);
BITSTREAM_GET_SET(desc38, interlaced_source_flag, uint8, 57, 1);
BITSTREAM_GET_SET(desc38, non_packed_constraint_flag, uint8, 58, 1);
BITSTREAM_GET_SET(desc38, frame_only_constraint_flag, uint8, 59, 1);
BITSTREAM_GET_SET(desc38, copied_44bits, uint64, 60, 44);
BITSTREAM_GET_SET(desc38, level_idc, uint8, 104, 8);
BITSTREAM_GET_SET(desc38, temporal_layer_subset_flag, uint8, 112, 1);
BITSTREAM_GET_SET(desc38, HEVC_still_present_flag, uint8, 113, 1);
BITSTREAM_GET_SET(desc38, HEVC_24hr_picture_present_flag, uint8, 114, 1);
BITSTREAM_GET_SET(desc38, sub_pic_hrd_params_not_present_flag, uint8, 115, 1);
BITSTREAM_GET_SET(desc38, HDR_WCG_idc, uint8, 118, 2);

static inline uint8_t *desc38_get_temporal_layer_subset(const uint8_t *p_desc)
{
    return desc38_get_temporal_layer_subset_flag(p_desc) ?
        (uint8_t *)(p_desc + DESC38_HEADER_SIZE) : NULL;
}

BITSTREAM_GET_SET(desc38_temp_layer_subset, id_min, uint8, 0, 3);
BITSTREAM_GET_SET(desc38_temp_layer_subset, id_max, uint8, 8, 3);

static inline bool desc38_validate(const uint8_t *p_desc)
{
    uint8_t desc_length = desc_get_length(p_desc);
    uint8_t size = DESC38_HEADER_SIZE - DESC_HEADER_SIZE;
    if (size > desc_length)
        return false;
    if (desc38_get_temporal_layer_subset_flag(p_desc) &&
        size + DESC38_TEMP_LAYER_SUBSET_SIZE > desc_length)
        return false;
    return true;
}

static inline void desc38_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t *p;

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 "<HEVC_VIDEO_DESC"
                 " profile_space=\"%u\""
                 " tier=\"%s\""
                 " profile_idc=\"%u\""
                 " profile_compatibility_indication=\"%08x\""
                 " progressive_source=\"%u\""
                 " interlaced_source=\"%u\""
                 " non_packed_constraint=\"%u\""
                 " frame_only_constraint=\"%u\""
                 " copied_44bits=\"%" PRIu64 "\""
                 " level_idc=\"%u\""
                 " HEVC_still_present=\"%u\""
                 " HEVC_24hr_picture_present=\"%u\""
                 " sub_pic_hrd_params_not_present=\"%u\""
                 " HDR_WCG_idc=\"%u\""
                 ">",
                 desc38_get_profile_space(p_desc),
                 desc38_get_tier_flag(p_desc) ? "high" : "main",
                 desc38_get_profile_idc(p_desc),
                 desc38_get_profile_compatibility_indication(p_desc),
                 desc38_get_progressive_source_flag(p_desc),
                 desc38_get_interlaced_source_flag(p_desc),
                 desc38_get_non_packed_constraint_flag(p_desc),
                 desc38_get_frame_only_constraint_flag(p_desc),
                 desc38_get_copied_44bits(p_desc),
                 desc38_get_level_idc(p_desc),
                 desc38_get_HEVC_still_present_flag(p_desc),
                 desc38_get_HEVC_24hr_picture_present_flag(p_desc),
                 desc38_get_sub_pic_hrd_params_not_present_flag(p_desc),
                 desc38_get_HDR_WCG_idc(p_desc)
                );
        if ((p = desc38_get_temporal_layer_subset(p_desc))) {
            pf_print(opaque, "<HEVC_VIDEO_TEMPORAL_LAYER_SUBSET"
                     " id_min=\"%u\" id_max=\"%u\"/>",
                     desc38_temp_layer_subset_get_id_min(p),
                     desc38_temp_layer_subset_get_id_max(p));
        }
        pf_print(opaque, "</HEVC_VIDEO_DESC>");
        break;

    default:
        pf_print(opaque,
                 "    - desc 38 HEVC_video"
                 " profile_space=%u"
                 "%s"
                 " profile_idc=%u"
                 " profile_compatibility_indication=%08x"
                 "%s%s%s%s"
                 " copied_44bits=%" PRIu64
                 " level_idc=%u"
                 "%s%s%s"
                 " HDR_WCG_idc=%u",
                 desc38_get_profile_space(p_desc),
                 desc38_get_tier_flag(p_desc) ? " high" : " main",
                 desc38_get_profile_idc(p_desc),
                 desc38_get_profile_compatibility_indication(p_desc),
                 desc38_get_progressive_source_flag(p_desc) ?
                     " progressive_source" : "",
                 desc38_get_interlaced_source_flag(p_desc) ?
                     " interlaced_source" : "",
                 desc38_get_non_packed_constraint_flag(p_desc) ?
                     " non_packed_constraint" : "",
                 desc38_get_frame_only_constraint_flag(p_desc) ?
                     " frame_only_constraint" : "",
                 desc38_get_copied_44bits(p_desc),
                 desc38_get_level_idc(p_desc),
                 desc38_get_HEVC_still_present_flag(p_desc) ?
                     " HEVC_still_present" : "",
                 desc38_get_HEVC_24hr_picture_present_flag(p_desc) ?
                     " HEVC_24hr_picture_present" : "",
                 desc38_get_sub_pic_hrd_params_not_present_flag(p_desc) ?
                     " sub_pic_hrd_params_not_present" : "",
                 desc38_get_HDR_WCG_idc(p_desc)
                );
        if ((p = desc38_get_temporal_layer_subset(p_desc))) {
            pf_print(opaque, "        - temporal layer subset"
                     " id_min=%u id_max=%u/>",
                     desc38_temp_layer_subset_get_id_min(p),
                     desc38_temp_layer_subset_get_id_max(p));
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
