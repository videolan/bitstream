/*****************************************************************************
 * desc_04.h: ISO/IEC 13818-1 Descriptor 0x04 (Hierarchy descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_04_H__
#define __BITSTREAM_MPEG_DESC_04_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x04: Hierarchy descriptor
 *****************************************************************************/
#define DESC04_HEADER_SIZE      (DESC_HEADER_SIZE + 4)

static inline void desc04_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x04);
    desc_set_length(p_desc, DESC04_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline uint8_t desc04_get_type(const uint8_t *p_desc)
{
    return p_desc[2] & 0x0f;
}

static inline void desc04_set_type(uint8_t *p_desc, uint8_t i_type)
{
    p_desc[2] = (i_type & 0x0f) | 0xf0;
}

static inline uint8_t desc04_get_layer_index(const uint8_t *p_desc)
{
    return p_desc[3] & 0x3f;
}

static inline void desc04_set_layer_index(uint8_t *p_desc, uint8_t i_layer_index)
{
    p_desc[3] = i_layer_index | 0xc0;
}

static inline uint8_t desc04_get_embedded_layer_index(const uint8_t *p_desc)
{
    return p_desc[4] & 0x3f;
}

static inline void desc04_set_embedded_layer_index(uint8_t *p_desc, uint8_t i_embedded_layer_index)
{
    p_desc[4] = i_embedded_layer_index | 0xc0;
}

static inline uint8_t desc04_get_channel(const uint8_t *p_desc)
{
    return p_desc[5] & 0x3f;
}

static inline void desc04_set_channel(uint8_t *p_desc, uint8_t i_channel)
{
    p_desc[5] = i_channel | 0xc0;
}

static inline const char *desc04_get_type_txt(uint8_t i_type)
{
    return i_type == 0 ? "reserved" :
           i_type == 1 ? "Spatial Scalability" :
           i_type == 2 ? "SNR Scalability" :
           i_type == 3 ? "Temporal Scalability" :
           i_type == 4 ? "Data paritioning" :
           i_type == 5 ? "Extension bitstream" :
           i_type == 6 ? "Private Stream" :
           i_type == 7 ? "Multi-view Profile" :
           i_type >= 8 && i_type <= 14 ? "reserved" :
           i_type == 15 ? "base layer" : "unknown";
}

static inline bool desc04_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC04_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc04_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<HIERARCHY_DESC type=\"%u\" type_txt=\"%s\" layer_index=\"%u\" embedded_layer_index=\"%u\" channel=\"%u\"/>",
                 desc04_get_type(p_desc),
                 desc04_get_type_txt(desc04_get_type(p_desc)),
                 desc04_get_layer_index(p_desc),
                 desc04_get_embedded_layer_index(p_desc),
                 desc04_get_channel(p_desc)
                );
        break;
    default:
        pf_print(opaque, "    - desc 04 hierarchy type=%u type_txt=\"%s\" layer_index=%u embedded_layer_index=%u channel=%u",
                 desc04_get_type(p_desc),
                 desc04_get_type_txt(desc04_get_type(p_desc)),
                 desc04_get_layer_index(p_desc),
                 desc04_get_embedded_layer_index(p_desc),
                 desc04_get_channel(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
