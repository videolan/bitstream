/*****************************************************************************
 * desc_6b.h: ETSI EN 300 468 Descriptor 0x6b: Ancillary data descriptor
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
 *  - ETSI EN 300 468 V1.11.1 (2010-04) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_DESC_6B_H__
#define __BITSTREAM_DVB_DESC_6B_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x6b: Ancillary data descriptor
 *****************************************************************************/
#define DESC6B_HEADER_SIZE      (DESC_HEADER_SIZE + 1)

static inline void desc6b_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x6b);
    desc_set_length(p_desc, (DESC6B_HEADER_SIZE - DESC_HEADER_SIZE));
    p_desc[2] = 0x80;
}

static inline uint8_t desc6b_get_ancillary_data_indentifier(const uint8_t *p_desc)
{
    return p_desc[2];
}

static inline void desc6b_set_ancillary_data_indentifier(uint8_t *p_desc, uint8_t i_data)
{
    p_desc[2] = i_data | 0x80;
}

#define __DEFINE_FLAG(FLAGNAME, bit) \
    static inline bool desc6b_get_##FLAGNAME##_flag(const uint8_t *p_desc) \
    { \
        return (p_desc[2] & bit) == bit; \
    } \
    \
    static inline void desc6b_set_##FLAGNAME##_flag(uint8_t *p_desc, bool b_##FLAGNAME) \
    { \
        p_desc[2] = b_##FLAGNAME ? (p_desc[2] | bit) : (p_desc[2] &~ bit); \
    }

__DEFINE_FLAG (dvd_video_ancillary_data     , 0x01)
__DEFINE_FLAG (extended_ancillary_data      , 0x02)
__DEFINE_FLAG (announcement_switching_data  , 0x04)
__DEFINE_FLAG (dab_ancillary_data           , 0x08)
__DEFINE_FLAG (scale_factor_error_check     , 0x10)
__DEFINE_FLAG (mpeg4_ancillary_data         , 0x20)
__DEFINE_FLAG (rds_via_uecp                 , 0x40)

#undef __DEFINE_FLAG

static inline bool desc6b_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC6B_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc6b_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 "<ANCILLARY_DATA_DESC"
                 " ancillary_data_indentifier=\"0x%02x\""
                 " dvd_video_ancillary_data_flag=\"%u\""
                 " extended_ancillary_data_flag=\"%u\""
                 " announcement_switching_data_flag=\"%u\""
                 " dab_ancillary_data_flag=\"%u\""
                 " scale_factor_error_check_flag=\"%u\""
                 " mpeg4_ancillary_data_flag=\"%u\""
                 " rds_via_uecp_flag=\"%u\"/>",
                 desc6b_get_ancillary_data_indentifier(p_desc),
                 desc6b_get_dvd_video_ancillary_data_flag(p_desc),
                 desc6b_get_extended_ancillary_data_flag(p_desc),
                 desc6b_get_announcement_switching_data_flag(p_desc),
                 desc6b_get_dab_ancillary_data_flag(p_desc),
                 desc6b_get_scale_factor_error_check_flag(p_desc),
                 desc6b_get_mpeg4_ancillary_data_flag(p_desc),
                 desc6b_get_rds_via_uecp_flag(p_desc)
                );
        break;
    default:
        pf_print(opaque,
                 "    - desc 6b ancillary_data"
                 " ancillary_data_indentifier=0x%02x"
                 " dvd_video_ancillary_data_flag=%u"
                 " extended_ancillary_data_flag=%u"
                 " announcement_switching_data_flag=%u"
                 " dab_ancillary_data_flag=%u"
                 " scale_factor_error_check_flag=%u"
                 " mpeg4_ancillary_data_flag=%u"
                 " rds_via_uecp_flag=%u",
                 desc6b_get_ancillary_data_indentifier(p_desc),
                 desc6b_get_dvd_video_ancillary_data_flag(p_desc),
                 desc6b_get_extended_ancillary_data_flag(p_desc),
                 desc6b_get_announcement_switching_data_flag(p_desc),
                 desc6b_get_dab_ancillary_data_flag(p_desc),
                 desc6b_get_scale_factor_error_check_flag(p_desc),
                 desc6b_get_mpeg4_ancillary_data_flag(p_desc),
                 desc6b_get_rds_via_uecp_flag(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
