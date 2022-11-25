/*****************************************************************************
 * desc_54.h: ETSI EN 300 468 Descriptor 0x54: Content descriptor
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

#ifndef __BITSTREAM_DVB_DESC_54_H__
#define __BITSTREAM_DVB_DESC_54_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x54: Content descriptor
 *****************************************************************************/
#define DESC54_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC54_CONTENT_SIZE     2

static inline void desc54_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x54);
}

static inline uint8_t *desc54_next_content(const uint8_t *p_desc,
                                           const uint8_t *p_desc_n)
{
    if (!p_desc_n)
        p_desc_n = p_desc + DESC54_HEADER_SIZE;
    else
        p_desc_n += DESC54_CONTENT_SIZE;
    return desc_check(p_desc, p_desc_n, DESC54_CONTENT_SIZE);
}

#define desc54_each_content(DESC, DESC_N) \
    desc_each(DESC, DESC_N, desc54_next_content)
#define desc54_get_content(DESC, N) \
    desc_get_at(DESC, N, desc54_next_content)

static inline uint8_t desc54n_get_content_l1(uint8_t *p_desc_n)
{
    return p_desc_n[0] >> 4;
}

static inline void desc54n_set_content_l1(uint8_t *p_desc_n, uint8_t i_content_l1)
{
    p_desc_n[0] = (i_content_l1 << 4) | (p_desc_n[0] & 0x0f);
}

static inline uint8_t desc54n_get_content_l2(uint8_t *p_desc_n)
{
    return p_desc_n[0] & 0x0f;
}

static inline void desc54n_set_content_l2(uint8_t *p_desc_n, uint8_t i_content_l2)
{
    p_desc_n[0] = (i_content_l2 & 0x0f) | (p_desc_n[0] & 0xf0);
}

static inline uint8_t desc54n_get_user(uint8_t *p_desc_n)
{
    return p_desc_n[1];
}

static inline void desc54n_set_user(uint8_t *p_desc_n, uint8_t i_user)
{
    p_desc_n[1] = i_user;
}

static inline bool desc54_validate(const uint8_t *p_desc)
{
    return !(desc_get_length(p_desc) % DESC54_CONTENT_SIZE);
}

static inline void desc54_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    desc54_each_content(p_desc, p_desc_n) {
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque, "<CONTENT_DESC content_l1=\"%u\" content_l2=\"%u\" user=\"%u\"/>",
                     desc54n_get_content_l1(p_desc_n),
                     desc54n_get_content_l2(p_desc_n),
                     desc54n_get_user(p_desc_n)
                    );
            break;
        default:
            pf_print(opaque,"    - desc 54 content content_l1=%u content_l2=%u user=%u",
                     desc54n_get_content_l1(p_desc_n),
                     desc54n_get_content_l2(p_desc_n),
                     desc54n_get_user(p_desc_n)
                    );
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
