/*****************************************************************************
 * desc_7f00.h: ETSI EN 300 468 Extension descriptor 0x00: Image icon
 * descriptor
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
 *  - ETSI EN 300 468 V1.12.1 (2011-06) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_DESC_7F00_H__
#define __BITSTREAM_DVB_DESC_7F00_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/dvb/si/desc_7f.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Extension descriptor 0x00: Image icon descriptor
 *****************************************************************************/
#define DESC7F00_HEADER_SIZE      (DESC7F_HEADER_SIZE + 2)

static inline void desc7f00_init(uint8_t *p_desc)
{
    desc7f_init(p_desc);
    desc_set_length(p_desc, (DESC7F00_HEADER_SIZE - DESC_HEADER_SIZE));
    desc7f_set_tag_extension(p_desc, DESC7F_TAG_IMAGE_ICON);
}

BITSTREAM_GET_SET(desc7f00, number, uint8, 24, 4);
BITSTREAM_GET_SET(desc7f00, last_descriptor_number, uint8, 28, 4);
BITSTREAM_GET_SET(desc7f00, icon_id, uint8, 37, 3);

static inline bool desc7f00_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC7F00_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc7f00_print(const uint8_t *p_desc, f_print pf_print,
                                  void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 "<IMAGE_ICON_DESC"
                 " number=\"%u\""
                 " last_descriptor_number=\"%u\""
                 " icon_id=\"%u\""
                 "/>",
                 desc7f00_get_number(p_desc),
                 desc7f00_get_last_descriptor_number(p_desc),
                 desc7f00_get_icon_id(p_desc)
                );
        break;
    default:
        pf_print(opaque,
                 "    - desc 7f00 image_icon"
                 " number=%u"
                 " last_descriptor_number=%u"
                 " icon_id=%u",
                 desc7f00_get_number(p_desc),
                 desc7f00_get_last_descriptor_number(p_desc),
                 desc7f00_get_icon_id(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
