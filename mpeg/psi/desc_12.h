/*****************************************************************************
 * desc_12.h: ISO/IEC 13818-1 Descriptor 0x12 (IBP descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_12_H__
#define __BITSTREAM_MPEG_DESC_12_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x12: IBP descriptor
 *****************************************************************************/
#define DESC12_HEADER_SIZE      (DESC_HEADER_SIZE + 2)

static inline void desc12_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x12);
    desc_set_length(p_desc, DESC12_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline bool desc12_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC12_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline bool desc12_get_closed_gop_flag(const uint8_t *p_desc) {
    return (p_desc[2] & 0x80) == 0x80;
}

static inline void desc12_set_closed_gop_flag(uint8_t *p_desc, bool b_closed_gop)
{
    p_desc[2] = b_closed_gop ? (p_desc[2] | 0x80) : (p_desc[2] &~ 0x80);
}

static inline bool desc12_get_identical_gop_flag(const uint8_t *p_desc)
{
    return (p_desc[2] & 0x40) == 0x40;
}

static inline void desc12_set_identical_gop_flag(uint8_t *p_desc, bool b_identical_gop_flag)
{
    p_desc[2] = b_identical_gop_flag ? (p_desc[2] | 0x40) : (p_desc[2] &~ 0x40);
}

static inline uint16_t desc12_get_max_gop_length(const uint8_t *p_desc)
{
    return ((uint16_t)(p_desc[2] & 0x3f) << 8) | p_desc[3];
}

static inline void desc12_set_max_gop_length(uint8_t *p_desc, uint16_t i_max_gop_length) {
    p_desc[2] = (p_desc[2] & 0xC0) | ((i_max_gop_length & 0x3fff) >> 8);
    p_desc[3] = i_max_gop_length & 0xff;
}

static inline void desc12_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<IBP_DESC closed_gop_flag=\"%u\" identical_gop_flag=\"%u\" max_gop_len=\"%u\"/>",
                 desc12_get_closed_gop_flag(p_desc),
                 desc12_get_identical_gop_flag(p_desc),
                 desc12_get_max_gop_length(p_desc));
        break;
    default:
        pf_print(opaque,"    - desc 12 ibp closed_gop_flag=%u identical_gop_flag=%u max_gop_len=%u",
                 desc12_get_closed_gop_flag(p_desc),
                 desc12_get_identical_gop_flag(p_desc),
                 desc12_get_max_gop_length(p_desc));
    }
}

#ifdef __cplusplus
}
#endif

#endif
