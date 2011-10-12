/*****************************************************************************
 * desc_2a.h: ISO/IEC 13818-1 Descriptor 0x2a (AVC timing and HRD descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_2A_H__
#define __BITSTREAM_MPEG_DESC_2A_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x2a (AVC timing and HRD descriptor)
 *****************************************************************************/
#define DESC2A_HEADER_SIZE      (DESC_HEADER_SIZE + 2)
#define DESC2A_HEADER_SIZE2     (DESC_HEADER_SIZE + 2 + 5)
#define DESC2A_HEADER_SIZE3     (DESC_HEADER_SIZE + 2 + 5 + 8)

static inline void desc2a_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x2a);
    desc_set_length(p_desc, DESC2A_HEADER_SIZE - DESC_HEADER_SIZE);
    p_desc[2] = 0x7e;
}

static inline bool desc2a_get_hrd_management_valid_flag(const uint8_t *p_desc)
{
    return (p_desc[2] & 0x80) == 0x80;
}

static inline void desc2a_set_hrd_management_valid_flag(uint8_t *p_desc, bool b_hrd_management_valid_flag)
{
    p_desc[2] = b_hrd_management_valid_flag ? (p_desc[2] | 0x80) : (p_desc[2] &~ 0x80);
}

static inline bool desc2a_get_picture_and_timing_info_present(const uint8_t *p_desc)
{
    return (p_desc[2] & 0x01) == 0x01;
}

static inline void desc2a_set_picture_and_timing_info_present(uint8_t *p_desc, bool b_picture_and_timing_info_present)
{
    if (b_picture_and_timing_info_present)
        desc_set_length(p_desc, DESC2A_HEADER_SIZE2 - DESC_HEADER_SIZE);
    else
        desc_set_length(p_desc, DESC2A_HEADER_SIZE - DESC_HEADER_SIZE);
    p_desc[2] = b_picture_and_timing_info_present ? (p_desc[2] | 0x01) : (p_desc[2] &~ 0x01);
}

static inline bool desc2a_get_90khz_flag(const uint8_t *p_desc)
{
    if (!desc2a_get_picture_and_timing_info_present(p_desc))
        return false;
    return (p_desc[3] & 0x80) == 0x80;
}

static inline void desc2a_set_90khz_flag(uint8_t *p_desc, bool b_90khz_flag)
{
    if (!desc2a_get_picture_and_timing_info_present(p_desc))
        return;
    if (b_90khz_flag)
        desc_set_length(p_desc, DESC2A_HEADER_SIZE2 - DESC_HEADER_SIZE);
    else
        desc_set_length(p_desc, DESC2A_HEADER_SIZE3 - DESC_HEADER_SIZE);
    p_desc[3] = 0x7f;
    p_desc[3] = b_90khz_flag ? (p_desc[3] | 0x80) : (p_desc[3] &~ 0x80);
}

static inline uint32_t desc2a_get_N(const uint8_t *p_desc)
{
    if (!desc2a_get_picture_and_timing_info_present(p_desc))
        return 0;
    if (desc2a_get_90khz_flag(p_desc))
        return 0;
    return (p_desc[4] << 24) | (p_desc[5] << 16) | (p_desc[6] << 8) | p_desc[7];
}

static inline void desc2a_set_N(uint8_t *p_desc, uint32_t i_N)
{
    if (!desc2a_get_picture_and_timing_info_present(p_desc))
        return;
    if (desc2a_get_90khz_flag(p_desc))
        return;
    p_desc[4] = (i_N >> 24) & 0xff;
    p_desc[5] = (i_N >> 16) & 0xff;
    p_desc[6] = (i_N >>  8) & 0xff;
    p_desc[7] =  i_N        & 0xff;
}

static inline uint32_t desc2a_get_K(const uint8_t *p_desc)
{
    if (!desc2a_get_picture_and_timing_info_present(p_desc))
        return 0;
    if (desc2a_get_90khz_flag(p_desc))
        return 0;
    return (p_desc[8] << 24) | (p_desc[9] << 16) | (p_desc[10] << 8) | p_desc[11];
}

static inline void desc2a_set_K(uint8_t *p_desc, uint32_t i_K)
{
    if (!desc2a_get_picture_and_timing_info_present(p_desc))
        return;
    if (desc2a_get_90khz_flag(p_desc))
        return;
    p_desc[ 8] = (i_K >> 24) & 0xff;
    p_desc[ 9] = (i_K >> 16) & 0xff;
    p_desc[10] = (i_K >>  8) & 0xff;
    p_desc[11] =  i_K        & 0xff;
}

static inline uint32_t desc2a_get_num_units_in_tick(const uint8_t *p_desc)
{
    uint8_t ofs;
    if (!desc2a_get_picture_and_timing_info_present(p_desc))
        return 0;
    ofs = desc2a_get_90khz_flag(p_desc) == 0 ? 12 : 4;
    return (p_desc[ofs+0] << 24) | (p_desc[ofs+1] << 16) | (p_desc[ofs+2] << 8) | p_desc[ofs+3];
}

static inline void desc2a_set_num_units_in_tick(uint8_t *p_desc, uint32_t i_num_units_in_tick)
{
    uint8_t ofs;
    if (!desc2a_get_picture_and_timing_info_present(p_desc))
        return;
    ofs = desc2a_get_90khz_flag(p_desc) == 0 ? 12 : 4;
    p_desc[ofs+0] = (i_num_units_in_tick >> 24) & 0xff;
    p_desc[ofs+1] = (i_num_units_in_tick >> 16) & 0xff;
    p_desc[ofs+2] = (i_num_units_in_tick >>  8) & 0xff;
    p_desc[ofs+3] =  i_num_units_in_tick        & 0xff;
}

#define desc2a_init_offset \
    uint8_t ofs = 3; \
    do { \
        if (desc2a_get_picture_and_timing_info_present(p_desc)) \
        { \
            ofs += 5; \
            if (desc2a_get_90khz_flag(p_desc) == false) \
                ofs += 8; \
        } \
    } while(0)

static inline bool desc2a_get_fixed_frame_rate_flag(const uint8_t *p_desc)
{
    desc2a_init_offset;
    return (p_desc[ofs] & 0x80) == 0x80;
}

static inline void desc2a_set_fixed_frame_rate_flag(uint8_t *p_desc, bool i_fixed_frame_rate_flag)
{
    desc2a_init_offset;
    p_desc[ofs] |= 0x1f;
    p_desc[ofs]  = i_fixed_frame_rate_flag ? (p_desc[ofs] | 0x80) : (p_desc[ofs] &~ 0x80);
}

static inline bool desc2a_get_temporal_poc_flag(const uint8_t *p_desc)
{
    desc2a_init_offset;
    return (p_desc[ofs] & 0x40) == 0x40;
}

static inline void desc2a_set_temporal_poc_flag(uint8_t *p_desc, bool i_temporal_poc_flag)
{
    desc2a_init_offset;
    p_desc[ofs] |= 0x1f;
    p_desc[ofs]  = i_temporal_poc_flag ? (p_desc[ofs] | 0x40) : (p_desc[ofs] &~ 0x40);
}

static inline bool desc2a_get_picture_to_display_conversion_flag(const uint8_t *p_desc)
{
    desc2a_init_offset;
    return (p_desc[ofs] & 0x20) == 0x20;
}

static inline void desc2a_set_picture_to_display_conversion_flag(uint8_t *p_desc, bool i_picture_to_display_conversion)
{
    desc2a_init_offset;
    p_desc[ofs] |= 0x1f;
    p_desc[ofs]  = i_picture_to_display_conversion ? (p_desc[ofs] | 0x20) : (p_desc[ofs] &~ 0x20);
}
#undef desc2a_init_offset

static inline bool desc2a_validate(const uint8_t *p_desc)
{
    uint8_t desc2a_header_size = DESC2A_HEADER_SIZE;
    if (desc2a_get_picture_and_timing_info_present(p_desc))
    {
        desc2a_header_size = DESC2A_HEADER_SIZE2;
        if (desc2a_get_90khz_flag(p_desc) == false)
            desc2a_header_size = DESC2A_HEADER_SIZE3;
    }
    return desc_get_length(p_desc) >= desc2a_header_size - DESC_HEADER_SIZE;
}

static inline void desc2a_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,"<AVC_TIMING_AND_HRD_DESC hrd_management_valid_flag=\"%u\""
                " picture_and_timing_info_present=\"%u\""
                " flag_90khz=\"%u\" N=\"%u\" K=\"%u\" num_units_in_tick=\"%u\""
                " fixed_frame_rate_flag=\"%u\" temporal_poc_flag=\"%u\""
                " picture_to_display_conversion_flag=\"%u\"/>",
                 desc2a_get_hrd_management_valid_flag(p_desc),
                 desc2a_get_picture_and_timing_info_present(p_desc),
                 desc2a_get_90khz_flag(p_desc),
                 desc2a_get_N(p_desc),
                 desc2a_get_K(p_desc),
                 desc2a_get_num_units_in_tick(p_desc),
                 desc2a_get_fixed_frame_rate_flag(p_desc),
                 desc2a_get_temporal_poc_flag(p_desc),
                 desc2a_get_picture_to_display_conversion_flag(p_desc)
                );
        break;
    default:
        pf_print(opaque,"    - desc 2a avc_timing_and_hrd hrd_management_valid_flag=%u"
                " picture_and_timing_info_present=%u"
                " flag_90khz=%u N=%u K=%u num_units_in_tick=%u"
                " fixed_frame_rate_flag=%u temporal_poc_flag=%u"
                " picture_to_display_conversion_flag=%u",
                 desc2a_get_hrd_management_valid_flag(p_desc),
                 desc2a_get_picture_and_timing_info_present(p_desc),
                 desc2a_get_90khz_flag(p_desc),
                 desc2a_get_N(p_desc),
                 desc2a_get_K(p_desc),
                 desc2a_get_num_units_in_tick(p_desc),
                 desc2a_get_fixed_frame_rate_flag(p_desc),
                 desc2a_get_temporal_poc_flag(p_desc),
                 desc2a_get_picture_to_display_conversion_flag(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
