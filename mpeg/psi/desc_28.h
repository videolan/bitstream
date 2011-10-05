/*****************************************************************************
 * desc_28.h: ISO/IEC 13818-1 Descriptor 0x28 (AVC video descriptor)
 *****************************************************************************
 * Copyright (C) 2011 Unix Solutions Ltd.
 *
 * Authors: Georgi Chorbadzhiyski <georgi@unixsol.org>
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
 *  - ISO/IEC 13818-1:2007(E) (MPEG-2 Systems)
 */

#ifndef __BITSTREAM_MPEG_DESC_28_H__
#define __BITSTREAM_MPEG_DESC_28_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x28 (AVC video descriptor)
 *****************************************************************************/
#define DESC28_HEADER_SIZE      (DESC_HEADER_SIZE + 4)

static inline void desc28_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x28);
    desc_set_length(p_desc, DESC28_HEADER_SIZE - DESC_HEADER_SIZE);
    p_desc[5] = 0x3f;
}

static inline bool desc28_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC28_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline uint8_t desc28_get_profile_idc(const uint8_t *p_desc)
{
    return p_desc[2];
}

static inline void desc28_set_profile_idc(uint8_t *p_desc, uint8_t i_profile_idc)
{
    p_desc[2] = i_profile_idc;
}

static inline bool desc28_get_constraint_set0_flag(const uint8_t *p_desc)
{
    return (p_desc[3] & 0x80) == 0x80;
}

static inline void desc28_set_constraint_set0_flag(uint8_t *p_desc, bool b_constraint_set0_flag)
{
    p_desc[3] = b_constraint_set0_flag ? (p_desc[3] | 0x80) : (p_desc[3] &~ 0x80);
}

static inline bool desc28_get_constraint_set1_flag(const uint8_t *p_desc)
{
    return (p_desc[3] & 0x40) == 0x40;
}

static inline void desc28_set_constraint_set1_flag(uint8_t *p_desc, bool b_constraint_set1_flag)
{
    p_desc[3] = b_constraint_set1_flag ? (p_desc[3] | 0x40) : (p_desc[3] &~ 0x40);
}

static inline bool desc28_get_constraint_set2_flag(const uint8_t *p_desc)
{
    return (p_desc[3] & 0x20) == 0x20;
}

static inline void desc28_set_constraint_set2_flag(uint8_t *p_desc, bool b_constraint_set2_flag)
{
    p_desc[3] = b_constraint_set2_flag ? (p_desc[3] | 0x20) : (p_desc[3] &~ 0x20);
}

static inline uint8_t desc28_get_avc_compatible_flags(const uint8_t *p_desc)
{
    return p_desc[3] & 0x1f;
}

static inline void desc28_set_avc_compatible_flags(uint8_t *p_desc, uint8_t i_avc_compatible_flags)
{
    p_desc[3] = (p_desc[3] & 0xe0) | (i_avc_compatible_flags & 0x1f);
}

static inline uint8_t desc28_get_level_idc(const uint8_t *p_desc)
{
    return p_desc[4];
}

static inline void desc28_set_level_idc(uint8_t *p_desc, uint8_t i_level_idc)
{
    p_desc[4] = i_level_idc;
}

static inline bool desc28_get_avc_still_present(const uint8_t *p_desc)
{
    return (p_desc[5] & 0x80) == 0x80;
}

static inline void desc28_set_avc_still_present(uint8_t *p_desc, bool b_avc_still_present)
{
    p_desc[5] = b_avc_still_present ? (p_desc[5] | 0x80) : (p_desc[5] &~ 0x80);
}

static inline bool desc28_get_avc_24_hour_picture_flag(const uint8_t *p_desc)
{
    return (p_desc[5] & 0x40) == 0x40;
}

static inline void desc28_set_avc_24_hour_picture_flag(uint8_t *p_desc, bool b_avc_24_hour_picture_flag)
{
    p_desc[5] = b_avc_24_hour_picture_flag ? (p_desc[5] | 0x40) : (p_desc[5] &~ 0x40);
}

static inline void desc28_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<AVC_VIDEO_DESC profile_idc=\"0x%02x\" constraint_set0_flag=\"%u\""
                " constraint_set1_flag=\"%u\" constraint_set2_flag=\"%u\""
                " AVC_compatible_flags=\"0x%02x\" level_idc=\"0x%02x\""
                " AVC_still_present=\"%u\" AVC_24_hour_picture_flag=\"%u\"/>",
                 desc28_get_profile_idc(p_desc),
                 desc28_get_constraint_set0_flag(p_desc),
                 desc28_get_constraint_set1_flag(p_desc),
                 desc28_get_constraint_set2_flag(p_desc),
                 desc28_get_avc_compatible_flags(p_desc),
                 desc28_get_level_idc(p_desc),
                 desc28_get_avc_still_present(p_desc),
                 desc28_get_avc_24_hour_picture_flag(p_desc)
                );
        break;
    default:
        pf_print(opaque,"    - desc 28 avc_video profile_idc=0x%02x constraint_set0_flag=%u"
                " constraint_set1_flag=%u constraint_set2_flag=%u"
                " AVC_compatible_flags=0x%02x level_idc=0x%02x"
                " AVC_still_present=%u AVC_24_hour_picture_flag=%u",
                 desc28_get_profile_idc(p_desc),
                 desc28_get_constraint_set0_flag(p_desc),
                 desc28_get_constraint_set1_flag(p_desc),
                 desc28_get_constraint_set2_flag(p_desc),
                 desc28_get_avc_compatible_flags(p_desc),
                 desc28_get_level_idc(p_desc),
                 desc28_get_avc_still_present(p_desc),
                 desc28_get_avc_24_hour_picture_flag(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
