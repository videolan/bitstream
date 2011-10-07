/*****************************************************************************
 * desc_50.h: ETSI EN 300 468 Descriptor 0x50: Component descriptor
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

#ifndef __BITSTREAM_DVB_DESC_50_H__
#define __BITSTREAM_DVB_DESC_50_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/dvb/si/strings.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x50: Component descriptor
 *****************************************************************************/
#define DESC50_HEADER_SIZE      (DESC_HEADER_SIZE + 6)

static inline void desc50_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x50);
    p_desc[2] = 0xf0;
    desc_set_length(p_desc, DESC50_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline uint8_t desc50_get_stream_content(const uint8_t *p_desc)
{
    return p_desc[2] & 0x0f;
}

static inline void desc50_set_stream_content(uint8_t *p_desc, uint8_t i_stream_content)
{
    p_desc[2] = 0xf0 | (i_stream_content & 0x0f);
}

static inline uint8_t desc50_get_component_type(const uint8_t *p_desc)
{
    return p_desc[3];
}

static inline void desc50_set_component_type(uint8_t *p_desc, uint8_t i_component_type)
{
    p_desc[3] = i_component_type;
}

static inline uint8_t desc50_get_component_tag(const uint8_t *p_desc)
{
    return p_desc[4];
}

static inline void desc50_set_component_tag(uint8_t *p_desc, uint8_t i_component_tag)
{
    p_desc[4] = i_component_tag;
}

static inline void desc50_set_language(uint8_t *p_desc, const uint8_t p_language[3])
{
    p_desc[5] = p_language[0];
    p_desc[6] = p_language[1];
    p_desc[7] = p_language[2];
}

static inline const uint8_t *desc50_get_language(const uint8_t *p_desc)
{
    return p_desc + 5;
}

static inline const uint8_t *desc50_get_text(const uint8_t *p_desc, uint8_t *pi_length)
{
    *pi_length = desc_get_length(p_desc) - (DESC50_HEADER_SIZE - DESC_HEADER_SIZE);
    return p_desc + DESC50_HEADER_SIZE;
}

static inline void desc50_set_text(uint8_t *p_desc, const uint8_t *p_text, uint8_t i_length)
{
    desc_set_length(p_desc, DESC50_HEADER_SIZE - DESC_HEADER_SIZE + i_length);
    memcpy(p_desc + DESC50_HEADER_SIZE, p_text, i_length);
}

static inline bool desc50_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC50_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc50_print(const uint8_t *p_desc,
                                f_print pf_print, void *print_opaque,
                                f_iconv pf_iconv, void *iconv_opaque,
                                print_type_t i_print_type)
{
    uint8_t i_text_len;
    const uint8_t *p_text = desc50_get_text(p_desc, &i_text_len);
    char *psz_text = dvb_string_get(p_text, i_text_len, pf_iconv, iconv_opaque);

    switch (i_print_type) {
    case PRINT_XML:
        psz_text = dvb_string_xml_escape(psz_text);
        pf_print(print_opaque,
                 "<COMPONENT_DESC stream_content=\"0x%02x\" component_type=\"0x%02x\""
                 " component_tag=\"%u\" language=\"%3.3s\" text=\"%s\"/>",
                 desc50_get_stream_content(p_desc),
                 desc50_get_component_type(p_desc),
                 desc50_get_component_tag(p_desc),
                 (char *)desc50_get_language(p_desc),
                 psz_text
                );
        break;
    default:
        pf_print(print_opaque,
                 "    - desc 50 component stream_content=0x%02x component_type=0x%02x"
                 " component_tag=%u language=%3.3s text=\"%s\"/>",
                 desc50_get_stream_content(p_desc),
                 desc50_get_component_type(p_desc),
                 desc50_get_component_tag(p_desc),
                 (char *)desc50_get_language(p_desc),
                 psz_text
                );
    }

    free(psz_text);
}

#ifdef __cplusplus
}
#endif

#endif
