/*****************************************************************************
 * desc_13.h: ETSI EN 300 468 Descriptor 0x13: Carousel identifier descriptor
 *****************************************************************************
 * Copyright (C) 2022 EasyTools
 *
 * Authors: Arnaud de Turckheim <adeturckheim@easytools.tv>
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
 *  - ISO/IEC 13818-6:1998
 */

#ifndef __BITSTREAM_MPEG_DESC_13_H__
#define __BITSTREAM_MPEG_DESC_13_H__

#include <inttypes.h>

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x13: Carousel identifier
 *****************************************************************************/
#define DESC13_HEADER_SIZE              (DESC_HEADER_SIZE + 4)

static inline void desc13_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x13);
    desc_set_length(p_desc, (DESC13_HEADER_SIZE - DESC_HEADER_SIZE));
}

static inline bool desc13_validate(const uint8_t *p_desc)
{
    uint8_t desc_length = desc_get_length(p_desc);
    uint8_t size = DESC13_HEADER_SIZE - DESC_HEADER_SIZE;
    return size <= desc_length;
}

BITSTREAM_GET_SET(desc13, id, uint8, 16, 32);

static inline void desc13_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 "<CAROUSEL_IDENTIFIER_DESC id=\"%u\"/>",
                 desc13_get_id(p_desc));
        break;

    default:
        pf_print(opaque,
                 "    - desc 13 carousel_identifier id=%u",
                 desc13_get_id(p_desc));
    }
}

#ifdef __cplusplus
}
#endif

#endif
