/*****************************************************************************
 * desc_0e.h: ISO/IEC 13818-1 Descriptor 0x0e (Maximum bitrate descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_0E_H__
#define __BITSTREAM_MPEG_DESC_0E_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x0e: Maximum bitrate descriptor
 *****************************************************************************/
#define DESC0E_HEADER_SIZE      (DESC_HEADER_SIZE + 3)

static inline void desc0e_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x0E);
    desc_set_length(p_desc, DESC0E_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline uint32_t desc0e_get_max_bitrate(const uint8_t *p_desc)
{
    return ((p_desc[2] &~ 0xc0) << 16) | (p_desc[3] << 8) | p_desc[4]; // 11xxxxxx xxxxxxxx xxxxxxxx
}

static inline void desc0e_set_max_bitrate(uint8_t *p_desc, uint32_t max_bitrate)
{
    p_desc[2] = ((max_bitrate >> 16) & 0xff) | 0xc0;
    p_desc[3] =  (max_bitrate >>  8) & 0xff;
    p_desc[4] =   max_bitrate        & 0xff;
}

static inline bool desc0e_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC0E_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc0e_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<MAXIMUM_BITRATE_DESC max_bitrate=\"%u\" max_bitrate_decoded=\"%u\"/>",
                 desc0e_get_max_bitrate(p_desc), desc0e_get_max_bitrate(p_desc) * 50);
        break;
    default:
        pf_print(opaque, "    - desc 0e maximum_bitrate max_bitrate=%u max_bitrate_decoded=%u",
                 desc0e_get_max_bitrate(p_desc), desc0e_get_max_bitrate(p_desc) * 50);
    }
}

#ifdef __cplusplus
}
#endif

#endif
