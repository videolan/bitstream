/*****************************************************************************
 * desc_07.h: ISO/IEC 13818-1 Descriptor 0x07 (Target Background Grid)
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

#ifndef __BITSTREAM_MPEG_DESC_07_H__
#define __BITSTREAM_MPEG_DESC_07_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x07: Target Background Grid descriptor
 *****************************************************************************/
#define DESC07_HEADER_SIZE      (DESC_HEADER_SIZE + 4)

static inline void desc07_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x07);
    desc_set_length(p_desc, DESC07_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline uint16_t desc07_get_horizontal_size(const uint8_t *p_desc)
{
    return ((uint16_t)p_desc[2] << 6) | (p_desc[3] >> 2);
}

static inline void desc07_set_horizontal_size(uint8_t *p_desc, uint16_t i_horizontal_size)
{
    p_desc[2] = (i_horizontal_size >> 6) & 0xff;
    p_desc[3] = ((i_horizontal_size & 0xff) << 2) | (p_desc[3] & 0x03);
}

static inline uint16_t desc07_get_vertical_size(const uint8_t *p_desc)
{
    return ((uint16_t)(p_desc[3] & 0x03) << 12) |
            (uint16_t)(p_desc[4] << 4) |
                      ((p_desc[5] & 0xf0) >> 4);
}

static inline void desc07_set_vertical_size(uint8_t *p_desc, uint16_t i_vertical_size)
{
    p_desc[3] = (p_desc[3] & 0xfc) | ((i_vertical_size >> 12) & 0x03);
    p_desc[4] = (i_vertical_size >> 4) & 0xff;
    p_desc[5] = ((i_vertical_size & 0x0f) << 4) | (p_desc[5] & 0x0f);
}

static inline uint8_t desc07_get_aspect_ratio_info(const uint8_t *p_desc)
{
    return p_desc[5] & 0x0f;
}

static inline void desc07_set_aspect_ratio_info(uint8_t *p_desc, uint8_t i_aspect_ratio_info)
{
    p_desc[5] = (p_desc[5] & 0xf0) | (i_aspect_ratio_info & 0x0f);
}

static inline bool desc07_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC07_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc07_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<TARGET_BACKGROUND_GRID_DESC horizontal_size=\"%u\" vertical_size=\"%u\" aspect_ratio_info=\"%u\"/>",
                 desc07_get_horizontal_size(p_desc),
                 desc07_get_vertical_size(p_desc),
                 desc07_get_aspect_ratio_info(p_desc)
                );
        break;
    default:
        pf_print(opaque, "    - desc 07 target_background_grid horizontal_size=%u vertical_size=%u aspect_ratio_info=%u",
                 desc07_get_horizontal_size(p_desc),
                 desc07_get_vertical_size(p_desc),
                 desc07_get_aspect_ratio_info(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
