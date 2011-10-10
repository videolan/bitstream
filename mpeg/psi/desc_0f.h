/*****************************************************************************
 * desc_0f.h: ISO/IEC 13818-1 Descriptor 0x0f (Private data indicator)
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

#ifndef __BITSTREAM_MPEG_DESC_0F_H__
#define __BITSTREAM_MPEG_DESC_0F_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x0f: Private data indicator descriptor
 *****************************************************************************/
#define DESC0F_HEADER_SIZE      (DESC_HEADER_SIZE + 4)

static inline void desc0f_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x0F);
    desc_set_length(p_desc, DESC0F_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline uint32_t desc0f_get_private_indicator(const uint8_t *p_desc)
{
    return (p_desc[2] << 24) | (p_desc[3] << 16) | (p_desc[4] << 8) | p_desc[5];
}

static inline void desc0f_set_private_indicator(uint8_t *p_desc, uint32_t i_private_indicator)
{
    p_desc[2] = (i_private_indicator >> 24) & 0xff;
    p_desc[3] = (i_private_indicator >> 16) & 0xff;
    p_desc[4] = (i_private_indicator >>  8) & 0xff;
    p_desc[5] =  i_private_indicator        & 0xff;;
}

static inline bool desc0f_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC0F_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc0f_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<PRIVATE_DATA_INDICATOR_DESC private_indicator=\"0x%08x\"/>",
                 desc0f_get_private_indicator(p_desc));
        break;
    default:
        pf_print(opaque, "    - desc 0f private_data_indicator private_indicator=0x%08x",
                 desc0f_get_private_indicator(p_desc));
    }
}

#ifdef __cplusplus
}
#endif

#endif
