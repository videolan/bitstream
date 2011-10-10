/*****************************************************************************
 * desc_10.h: ISO/IEC 13818-1 Descriptor 0x10 (Smoothing buffer descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_10_H__
#define __BITSTREAM_MPEG_DESC_10_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x10: Smoothing buffer descriptor
 *****************************************************************************/
#define DESC10_HEADER_SIZE      (DESC_HEADER_SIZE + 6)

static inline void desc10_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x10);
    desc_set_length(p_desc, DESC10_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline uint32_t desc10_get_sb_leak_rate(const uint8_t *p_desc)
{
    return ((p_desc[2] &~ 0xc0) << 16) | (p_desc[3] << 8) | p_desc[4]; // 11xxxxxx xxxxxxxx xxxxxxxx;
}

static inline void desc10_set_sb_leak_rate(uint8_t *p_desc, uint32_t sb_leak_rate)
{
    p_desc[2] = ((sb_leak_rate >> 16) & 0xff) | 0xc0;
    p_desc[3] =  (sb_leak_rate >>  8) & 0xff;
    p_desc[4] =   sb_leak_rate        & 0xff;
}

static inline uint32_t desc10_get_sb_size(const uint8_t *p_desc)
{
    return ((p_desc[5] &~ 0xc0) << 16) | (p_desc[6] << 8) | p_desc[7]; // 11xxxxxx xxxxxxxx xxxxxxxx;
}

static inline void desc10_set_sb_size(uint8_t *p_desc, uint32_t sb_size)
{
    p_desc[5] = ((sb_size >> 16) & 0xff) | 0xc0;
    p_desc[6] =  (sb_size >> 8)  & 0xff;
    p_desc[7] =   sb_size        & 0xff;
}

static inline bool desc10_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC10_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc10_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<SMOOTHING_BUFFER_DESC sb_leak_rate=\"%u\" sb_size=\"%u\"/>",
                 desc10_get_sb_leak_rate(p_desc), desc10_get_sb_size(p_desc));
        break;
    default:
        pf_print(opaque, "    - desc 10 smoothing_buffer sb_leak_rate=%u sb_size=%u",
                 desc10_get_sb_leak_rate(p_desc), desc10_get_sb_size(p_desc));
    }
}

#ifdef __cplusplus
}
#endif

#endif
