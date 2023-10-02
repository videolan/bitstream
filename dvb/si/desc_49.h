/*****************************************************************************
 * desc_49.h: ETSI EN 300 468 Descriptor 0x49: Country availability descriptor
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *          Georgi Chorbadzhiyski <georgi@unixsol.org>
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

#ifndef __BITSTREAM_DVB_DESC_49_H__
#define __BITSTREAM_DVB_DESC_49_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/mpeg/psi/desc_0a.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x49: Country availability descriptor
 *****************************************************************************/
#define DESC49_HEADER_SIZE      (DESC_HEADER_SIZE + 1)
#define DESC49_CODE_SIZE        3

static inline void desc49_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x49);
    p_desc[2] = 0x7f;
}

static inline bool desc49_get_country_availability_flag(const uint8_t *p_desc)
{
    return (p_desc[2] & 0x80) == 0x80;
}

static inline void desc49_set_country_availability_flag(uint8_t *p_desc, bool b_available)
{
    p_desc[2] = b_available ? (p_desc[2] | 0x80) : (p_desc[2] &~ 0x80);
}

static inline uint8_t *desc49_next_code(const uint8_t *p_desc,
                                        const uint8_t *p_desc_n)
{
    if (!p_desc_n)
        p_desc_n = p_desc + DESC49_HEADER_SIZE;
    else
        p_desc_n += DESC49_CODE_SIZE;
    return desc_check(p_desc, p_desc_n, DESC49_CODE_SIZE);
}

#define desc49_each_code(DESC, DESC_N) \
    desc_each(DESC, DESC_N, desc49_next_code)
#define desc49_get_code(DESC, N) \
    desc_get_at(DESC, N, desc49_next_code)

#define desc49n_set_code desc0an_set_code
#define desc49n_get_code desc0an_get_code

static inline bool desc49_validate(const uint8_t *p_desc)
{
    return ((desc_get_length(p_desc) - 1) % DESC49_CODE_SIZE) == 0;
}

static inline void desc49_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    desc49_each_code(p_desc, p_desc_n) {
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque,
                "<COUNTRY_AVAILABILITY_DESC available=\"%u\" country=\"%3.3s\"/>",
                desc49_get_country_availability_flag(p_desc),
                (const char *)desc49n_get_code(p_desc_n)
            );
            break;
        default:
            pf_print(opaque,
                "    - desc 49 country_availability available=%u country=%3.3s",
                desc49_get_country_availability_flag(p_desc),
                (const char *)desc49n_get_code(p_desc_n)
            );
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
