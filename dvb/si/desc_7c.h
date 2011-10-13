/*****************************************************************************
 * desc_7c.h: ETSI EN 300 468 Descriptor 0x7c: AAC descriptor
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
 *  - ETSI EN 300 468 V1.12.1 (2011-06) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_DESC_7C_H__
#define __BITSTREAM_DVB_DESC_7C_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x7c: AAC descriptor
 *****************************************************************************/
#define DESC7C_HEADER_SIZE      (DESC_HEADER_SIZE + 1)
#define DESC7C_HEADER_SIZE2     (DESC_HEADER_SIZE + 3)

static inline void desc7c_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x7c);
    desc_set_length(p_desc, (DESC7C_HEADER_SIZE - DESC_HEADER_SIZE));
}

static inline uint8_t desc7c_get_profile_and_level(const uint8_t *p_desc)
{
    return p_desc[2];
}

static inline void desc7c_set_profile_and_level(uint8_t *p_desc, uint8_t i_profile_and_level)
{
    p_desc[2] = i_profile_and_level;
}

static inline bool desc7c_get_aac_type_flag(const uint8_t *p_desc)
{
    if (desc_get_length(p_desc) > 1)
        return (p_desc[3] & 0x80) == 0x80;
    else
        return false;
}

static inline void desc7c_set_aac_type_flag(uint8_t *p_desc, bool b_aac_type_flag)
{
    uint8_t i_desc_len = desc_get_length(p_desc);
    if (i_desc_len == 1 && b_aac_type_flag) {
        i_desc_len += 2;
        desc_set_length(p_desc, i_desc_len);
    }
    if (i_desc_len > 1 && !b_aac_type_flag) {
        i_desc_len -= 2;
        desc_set_length(p_desc, i_desc_len);
    }
    if (i_desc_len > 1)
        p_desc[3] = b_aac_type_flag ? 0xff : 0x7f;
}

static inline uint8_t desc7c_get_aac_type(const uint8_t *p_desc)
{
    if (!desc7c_get_aac_type_flag(p_desc))
        return 0;
    return p_desc[4];
}

static inline void desc7c_set_aac_type(uint8_t *p_desc, uint8_t i_aac_type)
{
    if (!desc7c_get_aac_type_flag(p_desc))
        return;
    p_desc[4] = i_aac_type;
}

static inline bool desc7c_validate(const uint8_t *p_desc)
{
    if (!desc7c_get_aac_type_flag(p_desc))
        return desc_get_length(p_desc) >= (DESC7C_HEADER_SIZE - DESC_HEADER_SIZE);
    else
        return desc_get_length(p_desc) >= (DESC7C_HEADER_SIZE2 - DESC_HEADER_SIZE);
}

static inline void desc7c_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 "<AAC_DESC profile_and_level=\"0x%02x\""
                 " aac_type_flag=\"%u\" aac_type=\"0x%02x\"/>",
                    desc7c_get_profile_and_level(p_desc),
                    desc7c_get_aac_type_flag(p_desc),
                    desc7c_get_aac_type(p_desc)
                );
        break;
    default:
        pf_print(opaque,
                 "    - desc 7c aac profile_and_level=\"0x%02x\""
                 " aac_type_flag=\"%u\" aac_type=\"0x%02x\"",
                    desc7c_get_profile_and_level(p_desc),
                    desc7c_get_aac_type_flag(p_desc),
                    desc7c_get_aac_type(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
