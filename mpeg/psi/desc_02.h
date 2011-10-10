/*****************************************************************************
 * desc_02.h: ISO/IEC 13818-1 Descriptor 0x02 (Video stream descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_02_H__
#define __BITSTREAM_MPEG_DESC_02_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x02: Video stream descriptor
 *****************************************************************************/
#define DESC02_HEADER_SIZE      (DESC_HEADER_SIZE + 1)

static inline void desc02_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x02);
    desc_set_length(p_desc, DESC02_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline bool desc02_get_multiple_frame_rate(const uint8_t *p_desc) {
    return (p_desc[2] & 0x80) == 0x80;
}

static inline void desc02_set_multiple_frame_rate(uint8_t *p_desc, bool b_multiple_frame_rate) {
    p_desc[2] = b_multiple_frame_rate ? (p_desc[2] | 0x80) : (p_desc[2] &~ 0x80);
}

static inline uint8_t desc02_get_frame_rate_code(const uint8_t *p_desc) {
    return (p_desc[2] & 0x78) >> 3; /* 1xxxx111 */
}

static inline void desc02_set_frame_rate_code(uint8_t *p_desc, uint8_t i_frame_rate_code) {
    p_desc[2] = (p_desc[2] & 0x87) | (i_frame_rate_code & 0x0f) << 3;
}

static inline const char *desc02_get_frame_rate_txt(const uint8_t frame_rate_code) {
    return frame_rate_code == 0 ? "forbidden" :
           frame_rate_code == 1 ? "23.976" :
           frame_rate_code == 2 ? "24.00" :
           frame_rate_code == 3 ? "25.00" :
           frame_rate_code == 4 ? "29.97" :
           frame_rate_code == 5 ? "30.00" :
           frame_rate_code == 6 ? "50.00" :
           frame_rate_code == 7 ? "59.94" :
           frame_rate_code == 8 ? "60.00" : "reserved";
}

static inline bool desc02_get_mpeg1_only_flag(const uint8_t *p_desc) {
    return (p_desc[2] & 0x04) == 0x04;
}

static inline void desc02_set_mpeg1_only_flag(uint8_t *p_desc, bool b_mpeg1_only_flag) {
    p_desc[2] = b_mpeg1_only_flag ? (p_desc[2] | 0x04) : (p_desc[2] &~ 0x04);
    if (b_mpeg1_only_flag)
        desc_set_length(p_desc, DESC02_HEADER_SIZE - DESC_HEADER_SIZE);
    else
        desc_set_length(p_desc, (DESC02_HEADER_SIZE + 2) - DESC_HEADER_SIZE);
}

static inline bool desc02_get_constraint_parameter(const uint8_t *p_desc) {
    return (p_desc[2] & 0x02) == 0x02;
}

static inline void desc02_set_constraint_parameter(uint8_t *p_desc, bool b_constraint_parameter) {
    p_desc[2] = b_constraint_parameter ? (p_desc[2] | 0x02) : (p_desc[2] &~ 0x02);
}


static inline bool desc02_get_still_picture(const uint8_t *p_desc) {
    return (p_desc[2] & 0x01) == 0x01;
}

static inline void desc02_set_still_picture(uint8_t *p_desc, bool b_still_picture) {
    p_desc[2] = b_still_picture ? (p_desc[2] | 0x01) : (p_desc[2] &~ 0x01);
}

static inline uint8_t desc02_get_profile(const uint8_t *p_desc) {
   return p_desc[3] >> 4;
}

static inline void desc02_set_profile(uint8_t *p_desc, uint8_t i_profile) {
   p_desc[3] = ((i_profile & 0x0f) << 4) | (p_desc[3] & 0x0f);
}

static inline const char *desc02_get_profile_txt(const uint8_t profile) {
    return profile == 1 ? "High"               :
           profile == 2 ? "Spatially Scalable" :
           profile == 3 ? "SNR Scalable"       :
           profile == 4 ? "Main"               :
           profile == 5 ? "Simple"             : "Reserved";
}

static inline uint8_t desc02_get_level(const uint8_t *p_desc) {
   return p_desc[3] & 0x0f;
}

static inline void desc02_set_level(uint8_t *p_desc, uint8_t i_level) {
   p_desc[3] = (p_desc[3] & 0xf0) | (i_level & 0x0f);
}

static inline const char *desc02_get_level_txt(const uint8_t level) {
    return level == 4  ? "High"      :
           level == 6  ? "High 1440" :
           level == 8  ? "Main"      :
           level == 10 ? "Low"       : "Reserved";
}

static inline uint8_t desc02_get_chroma_format(const uint8_t *p_desc) {
   return p_desc[4] >> 6;
}

static inline void desc02_set_chroma_format(uint8_t *p_desc, uint8_t i_chroma_format) {
   p_desc[4] = (i_chroma_format << 6) | (p_desc[4] & 0x20) | 0x1f;
}

static inline const char *desc02_get_chroma_format_txt(const uint8_t chroma_format) {
    return chroma_format == 0 ? "reserved" :
           chroma_format == 1 ? "4:2:0" :
           chroma_format == 2 ? "4:2:2" :
           chroma_format == 3 ? "4:4:4" : "unknown";
}

static inline bool desc02_get_frame_rate_extension(const uint8_t *p_desc) {
    return (p_desc[4] & 0x20) == 0x20;
}

static inline void desc02_set_frame_rate_extension(uint8_t *p_desc, bool b_frame_rate_extension) {
    p_desc[4] = (b_frame_rate_extension ? (p_desc[4] | 0x20) : (p_desc[4] &~ 0x20)) | 0x1f;
}

static inline bool desc02_validate(const uint8_t *p_desc)
{
    if (desc02_get_mpeg1_only_flag(p_desc))
        return desc_get_length(p_desc) >= DESC02_HEADER_SIZE - DESC_HEADER_SIZE;
    else
        return desc_get_length(p_desc) >= (DESC02_HEADER_SIZE + 2) - DESC_HEADER_SIZE;
}

static inline void desc02_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        if (desc02_get_mpeg1_only_flag(p_desc)) {
            pf_print(opaque,
                "<VIDEO_STREAM_DESC multiple_frame_rate=\"%u\" frame_rate_code=\"%u\" frame_rate_txt=\"%s\""
                " mpeg1_only=\"%u\" constraint_parameter=\"%u\" still_picture=\"%u\"/>",
                     desc02_get_multiple_frame_rate(p_desc),
                     desc02_get_frame_rate_code(p_desc),
                     desc02_get_frame_rate_txt(desc02_get_frame_rate_code(p_desc)),
                     desc02_get_mpeg1_only_flag(p_desc),
                     desc02_get_constraint_parameter(p_desc),
                     desc02_get_still_picture(p_desc)
            );
        } else {
            pf_print(opaque,
                "<VIDEO_STREAM_DESC multiple_frame_rate=\"%u\" frame_rate_code=\"%u\" frame_rate_txt=\"%s\""
                " mpeg1_only=\"%u\" constraint_parameter=\"%u\" still_picture=\"%u\""
                " profile=\"%u\" profile_txt=\"%s\" level=\"%u\" level_txt=\"%s\""
                " chroma_format=\"%u\" chroma_format_txt=\"%s\""
                " frame_rate_extension=\"%u\"/>",
                     desc02_get_multiple_frame_rate(p_desc),
                     desc02_get_frame_rate_code(p_desc),
                     desc02_get_frame_rate_txt(desc02_get_frame_rate_code(p_desc)),
                     desc02_get_mpeg1_only_flag(p_desc),
                     desc02_get_constraint_parameter(p_desc),
                     desc02_get_still_picture(p_desc),
                     desc02_get_profile(p_desc),
                     desc02_get_profile_txt(desc02_get_profile(p_desc)),
                     desc02_get_level(p_desc),
                     desc02_get_level_txt(desc02_get_level(p_desc)),
                     desc02_get_chroma_format(p_desc),
                     desc02_get_chroma_format_txt(desc02_get_chroma_format(p_desc)),
                     desc02_get_frame_rate_extension(p_desc)
            );
        }
        break;
    default:
        if (desc02_get_mpeg1_only_flag(p_desc)) {
            pf_print(opaque,
                "    - desc 02 video_stream multiple_frame_rate=%u frame_rate_code=%u frame_rate_txt=\"%s\""
                " mpeg1_only=%u constraint_parameter=%u still_picture=%u",
                     desc02_get_multiple_frame_rate(p_desc),
                     desc02_get_frame_rate_code(p_desc),
                     desc02_get_frame_rate_txt(desc02_get_frame_rate_code(p_desc)),
                     desc02_get_mpeg1_only_flag(p_desc),
                     desc02_get_constraint_parameter(p_desc),
                     desc02_get_still_picture(p_desc)
            );
        } else {
            pf_print(opaque,
                "    - desc 02 video_stream multiple_frame_rate=%u frame_rate_code=%u frame_rate_txt=\"%s\""
                " mpeg1_only=%u constraint_parameter=%u still_picture=%u"
                " profile=%u profile_txt=\"%s\" level=%u level_txt=\"%s\""
                " chroma_format=%u chroma_format_txt=\"%s\""
                " frame_rate_extension=%u",
                     desc02_get_multiple_frame_rate(p_desc),
                     desc02_get_frame_rate_code(p_desc),
                     desc02_get_frame_rate_txt(desc02_get_frame_rate_code(p_desc)),
                     desc02_get_mpeg1_only_flag(p_desc),
                     desc02_get_constraint_parameter(p_desc),
                     desc02_get_still_picture(p_desc),
                     desc02_get_profile(p_desc),
                     desc02_get_profile_txt(desc02_get_profile(p_desc)),
                     desc02_get_level(p_desc),
                     desc02_get_level_txt(desc02_get_level(p_desc)),
                     desc02_get_chroma_format(p_desc),
                     desc02_get_chroma_format_txt(desc02_get_chroma_format(p_desc)),
                     desc02_get_frame_rate_extension(p_desc)
            );
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
