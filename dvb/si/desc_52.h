/*****************************************************************************
 * desc_52.h: ETSI EN 300 468 Descriptor 0x52: Stream identifier descriptor
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

#ifndef __BITSTREAM_DVB_DESC_52_H__
#define __BITSTREAM_DVB_DESC_52_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x52: Stream identifier descriptor
 *****************************************************************************/
#define DESC52_HEADER_SIZE      (DESC_HEADER_SIZE + 1)

static inline void desc52_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x52);
    desc_set_length(p_desc, DESC52_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline void desc52_set_component_tag(uint8_t *p_desc, uint8_t component_tag)
{
    p_desc[2] = component_tag;
}

static inline uint8_t desc52_get_component_tag(const uint8_t *p_desc)
{
    return p_desc[2];
}

static inline bool desc52_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC52_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc52_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<STREAM_IDENTIFIER_DESC component_tag=\"%u\"/>",
                 desc52_get_component_tag(p_desc));
        break;
    default:
        pf_print(opaque, "    - desc 52 stream_identifier component_tag=%u",
                 desc52_get_component_tag(p_desc));
    }
}

#ifdef __cplusplus
}
#endif

#endif
