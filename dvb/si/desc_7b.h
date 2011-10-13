/*****************************************************************************
 * desc_7b.h: ETSI EN 300 468 Descriptor 0x7b: DTS descriptor
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

#ifndef __BITSTREAM_DVB_DESC_7B_H__
#define __BITSTREAM_DVB_DESC_7B_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x7b: DTS descriptor
 *****************************************************************************/
#define DESC7B_HEADER_SIZE      (DESC_HEADER_SIZE + 5)

static inline void desc7b_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x7b);
    desc_set_length(p_desc, (DESC7B_HEADER_SIZE - DESC_HEADER_SIZE));
}

static inline uint8_t desc7b_get_sample_rate_code(const uint8_t *p_desc)
{
    return ((p_desc[2] & 0xf0) >> 4); // 1111xxxx
}

static inline void desc7b_set_sample_rate_code(uint8_t *p_desc, uint8_t i_sample_rate_code)
{
    p_desc[2] = (p_desc[2] & 0x0f) | (i_sample_rate_code << 4); // 1111xxxx
}

static inline uint8_t desc7b_get_bit_rate_code(const uint8_t *p_desc)
{
    return ((p_desc[2] & 0x0f) << 2) | ((p_desc[3] & 0xc0) >> 6); // xxxx1111 11xxxxxx
}

static inline void desc7b_set_bit_rate_code(uint8_t *p_desc, uint8_t i_bit_rate_code)
{
    p_desc[2] = (p_desc[2] & 0xf0) | ((i_bit_rate_code >> 2) & 0x0f); // xxxx1111
    p_desc[3] = (i_bit_rate_code << 6) | (p_desc[2] & 0x3f); // 11xxxxxx
}

static inline uint8_t desc7b_get_nblks(const uint8_t *p_desc)
{
    return ((p_desc[3] & 0x3f) << 1) | ((p_desc[4] & 0x80) >> 7); // xx111111 1xxxxxxx
}

static inline void desc7b_set_nblks(uint8_t *p_desc, uint8_t i_nblks)
{
    p_desc[3] = (p_desc[3] & 0xc0) | ((i_nblks >> 1) & 0x3f); // xx111111
    p_desc[4] = (i_nblks << 7) | (p_desc[4] & 0x7f); // 1xxxxxxx
}

static inline uint16_t desc7b_get_fsize(const uint8_t *p_desc)
{
    return ((p_desc[4] & 0x7f) << 7) | ((p_desc[5] & 0xfe) >> 1); // x1111111 1111111x
}

static inline void desc7b_set_fsize(uint8_t *p_desc, uint16_t i_fsize)
{
    i_fsize &= 0x3fff;
    p_desc[4] = (p_desc[4] & 0x80) | (i_fsize >> 7); // x1111111
    p_desc[5] = ((i_fsize & 0x7f) << 1) | (p_desc[5] & 0x01); // 1111111x
}

static inline uint8_t desc7b_get_surround_mode(const uint8_t *p_desc)
{
    return ((p_desc[5] & 0x01) << 5) | ((p_desc[6] & 0xf8) >> 3); // xxxxxxx1 11111xxx
}

static inline void desc7b_set_surround_mode(uint8_t *p_desc, uint8_t i_surround_mode)
{
    i_surround_mode &= 0x3f;
    p_desc[5] = (p_desc[5] & 0xfe) | (i_surround_mode >> 5); // xxxxxxx1
    p_desc[6] = ((i_surround_mode & 0x1f) << 3) | (p_desc[6] & 0x07); // 11111xxx
}

static inline bool desc7b_get_lfe_flag(const uint8_t *p_desc)
{
    return (p_desc[6] & 0x04) == 0x04; // xxxxx1xx
}

static inline void desc7b_set_lfe_flag(uint8_t *p_desc, bool b_lfe_flag)
{
    p_desc[6] = b_lfe_flag ? (p_desc[6] | 0x04) : (p_desc[6] &~ 0x04); // xxxxx1xx
}

static inline uint8_t desc7b_get_extended_surround_flag(const uint8_t *p_desc)
{
    return p_desc[6] & 0x03; // xxxxxx11
}

static inline void desc7b_set_extended_surround_flag(uint8_t *p_desc, uint8_t i_extended_surround_flag)
{
    p_desc[6] = (p_desc[6] & 0xfc) | (i_extended_surround_flag & 0x03); // xxxxxx11
}

static inline bool desc7b_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= (DESC7B_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline void desc7b_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 "<DTS_DESC sample_rate_code=\"%u\" bit_rate_code=\"%u\""
                 " nblks=\"%u\" fsize=\"%u\" surround_mode=\"%u\""
                 " lfe_flag=\"%u\" extended_surround_flag=\"%u\"/>",
                    desc7b_get_sample_rate_code(p_desc),
                    desc7b_get_bit_rate_code(p_desc),
                    desc7b_get_nblks(p_desc),
                    desc7b_get_fsize(p_desc),
                    desc7b_get_surround_mode(p_desc),
                    desc7b_get_lfe_flag(p_desc),
                    desc7b_get_extended_surround_flag(p_desc)
                );
        break;
    default:
        pf_print(opaque,
                 "    - desc 7b dts sample_rate_code=%u bit_rate_code=%u"
                 " nblks=%u fsize=%u surround_mode=%u"
                 " lfe_flag=%u extended_surround_flag=%u",
                    desc7b_get_sample_rate_code(p_desc),
                    desc7b_get_bit_rate_code(p_desc),
                    desc7b_get_nblks(p_desc),
                    desc7b_get_fsize(p_desc),
                    desc7b_get_surround_mode(p_desc),
                    desc7b_get_lfe_flag(p_desc),
                    desc7b_get_extended_surround_flag(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
