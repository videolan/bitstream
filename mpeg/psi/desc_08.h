/*****************************************************************************
 * desc_08.h: ISO/IEC 13818-1 Descriptor 0x08 (Video Window descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_08_H__
#define __BITSTREAM_MPEG_DESC_08_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/mpeg/psi/desc_07.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x08: Video Window descriptor
 *****************************************************************************/
#define DESC08_HEADER_SIZE      (DESC_HEADER_SIZE + 4)

static inline void desc08_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x08);
    desc_set_length(p_desc, DESC08_HEADER_SIZE - DESC_HEADER_SIZE);
}

#define desc08_get_horizontal_offset desc07_get_horizontal_size
#define desc08_set_horizontal_offset desc07_set_horizontal_size

#define desc08_get_vertical_offset desc07_get_vertical_size
#define desc08_set_vertical_offset desc07_set_vertical_size

#define desc08_get_window_priority desc07_get_aspect_ratio_info
#define desc08_set_window_priority desc07_set_aspect_ratio_info

static inline bool desc08_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC08_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc08_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<VIDEO_WINDOW_DESC horizontal_offset=\"%u\" vertical_offset=\"%u\" window_priority=\"%u\"/>",
                 desc08_get_horizontal_offset(p_desc),
                 desc08_get_vertical_offset(p_desc),
                 desc08_get_window_priority(p_desc)
                );
        break;
    default:
        pf_print(opaque, "    - desc 08 video_window horizontal_offset=%u vertical_offset=%u window_priority=%u",
                 desc08_get_horizontal_offset(p_desc),
                 desc08_get_vertical_offset(p_desc),
                 desc08_get_window_priority(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
