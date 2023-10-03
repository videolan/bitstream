/*****************************************************************************
 * desc_3f03.h: ETSI EN 300 468 Extension descriptor 0x03: HEVC timing and HRD
 * descriptor
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

#ifndef __BITSTREAM_MPEG_DESC_3F03_H__
#define __BITSTREAM_MPEG_DESC_3F03_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/mpeg/psi/desc_3f.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Extension descriptor 0x03: HEVC timing and HRD descriptor
 *****************************************************************************/
#define DESC3F03_HEADER_SIZE    (DESC3F_HEADER_SIZE + 1)
#define DESC3F03_PTI_SIZE       13
#define DESC3F03_PTI_SCALE_SIZE 8
#define DESC3F03_PTI_90K_SIZE   (DESC3F03_PTI_SIZE - DESC3F03_PTI_SCALE_SIZE)

static inline void desc3f03_init(uint8_t *p_desc)
{
    desc3f_init(p_desc);
    desc_set_length(p_desc, (DESC3F03_HEADER_SIZE - DESC_HEADER_SIZE));
    desc3f_set_tag_extension(p_desc, DESC3F_TAG_HEVC_TIMING_HRD);
}

BITSTREAM_GET_SET(desc3f03, hrd_management_valid_flag, uint8, 24, 1);
BITSTREAM_GET_SET(desc3f03, target_schedule_idx_not_present_flag, uint8, 25, 1);
BITSTREAM_GET_SET(desc3f03, target_schedule_idx, uint8, 26, 5);
BITSTREAM_GET_SET(desc3f03, picture_and_timing_info_present_flag, uint8, 31, 1);

static inline uint8_t *
desc3f03_get_picture_and_timing_info(const uint8_t *p_desc)
{
    return desc3f03_get_picture_and_timing_info_present_flag(p_desc) ?
        (uint8_t *)(p_desc + DESC3F03_HEADER_SIZE) : NULL;
}

BITSTREAM_GET_SET(desc3f03_pti, 90kHz_flag, uint8, 0, 1);

static inline uint8_t *desc3f03_pti_get_scale(const uint8_t *p_desc_n)
{
    return !desc3f03_pti_get_90kHz_flag(p_desc_n) ?
        (uint8_t *)(p_desc_n + 1) : NULL;
}

BITSTREAM_GET_SET(desc3f03_pti_scale, n, uint32, 0, 32);
BITSTREAM_GET_SET(desc3f03_pti_scale, k, uint32, 32, 32);

static inline uint8_t *desc3f03_pti_get_trailer(const uint8_t *p_desc_n)
{
    return desc3f03_pti_get_90kHz_flag(p_desc_n) ?
        (uint8_t *)(p_desc_n + 1) :
        (uint8_t *)(p_desc_n + 1 + DESC3F03_PTI_SCALE_SIZE);
}

BITSTREAM_GET_SET(desc3f03_pti_trailer, num_units_in_tick, uint32, 0, 32);

static inline uint32_t
desc3f03_pti_get_num_units_in_tick(const uint8_t *p_desc_n)
{
    return desc3f03_pti_trailer_get_num_units_in_tick(
        desc3f03_pti_get_trailer(p_desc_n));
}

static inline void
desc3f03_pti_set_num_units_in_tick(uint8_t *p_desc_n, uint32_t v)
{
    return desc3f03_pti_trailer_set_num_units_in_tick(
        desc3f03_pti_get_trailer(p_desc_n), v);
}

static inline bool desc3f03_validate(const uint8_t *p_desc)
{
    uint8_t desc_length = desc_get_length(p_desc);
    uint8_t size = DESC3F03_HEADER_SIZE - DESC_HEADER_SIZE;
    uint8_t *p;

    if (size > desc_length)
        return false;
    if ((p = desc3f03_get_picture_and_timing_info(p_desc))) {
        if (size + DESC3F03_PTI_90K_SIZE > desc_length)
            return false;

        if (!desc3f03_pti_get_90kHz_flag(p) &&
            size + DESC3F03_PTI_SIZE > desc_length)
            return false;
    }
    return true;
}

static inline void desc3f03_print(const uint8_t *p_desc, f_print pf_print,
                                  void *opaque, print_type_t i_print_type)
{
    uint8_t *pti = desc3f03_get_picture_and_timing_info(p_desc);
    uint8_t *scale = pti ? desc3f03_pti_get_scale(pti) : NULL;

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<HEVC_TIMING_HRD_DESC"
                 " hrd_management_valid=\"%u\""
                 ">",
                 desc3f03_get_hrd_management_valid_flag(p_desc));
        if (!desc3f03_get_target_schedule_idx_not_present_flag(p_desc))
            pf_print(opaque, "<HEVC_TIMING_HRD schedule_idx=\"%u\"/>",
                     desc3f03_get_target_schedule_idx(p_desc));
        if (pti) {
            pf_print(opaque, "<HEVC_PICTURE_AND_TIMING_INFO"
                     " num_units_in_tick=\"%u\">",
                     desc3f03_pti_get_num_units_in_tick(pti));
            if (scale)
                pf_print(opaque, "<HEVC_PICTURE_AND_TIMING_SCALE"
                         " n=\"%u\" k=\"%u\"/>",
                         desc3f03_pti_scale_get_n(scale),
                         desc3f03_pti_scale_get_k(scale));
            pf_print(opaque, "</HEVC_PICTURE_AND_TIMING_INFO>");
        }
        pf_print(opaque, "</HEVC_TIMING_HRD_DESC>");
        break;

    default:
        pf_print(opaque, "    - desc 3f03 HEVC_timing_HRD%s",
                 desc3f03_get_hrd_management_valid_flag(p_desc) ?
                 " hrd_management_valid" : "");
        if (!desc3f03_get_target_schedule_idx_not_present_flag(p_desc))
            pf_print(opaque, "        - schedule_idx=%u",
                     desc3f03_get_target_schedule_idx(p_desc));
        if (pti) {
            pf_print(opaque, "        - picture_and_timing_info"
                     " num_units_in_tick=%u",
                     desc3f03_pti_get_num_units_in_tick(pti));
            if (scale)
                pf_print(opaque, "            - scale n=%u k=%u",
                         desc3f03_pti_scale_get_n(scale),
                         desc3f03_pti_scale_get_k(scale));
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
