/*****************************************************************************
 * desc_5e.h: ETSI EN 300 468 Descriptor 0x5e: Multilingual component descriptor
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *          Georgi Chorbadzhiyski <georgi@unixsol.org>
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

#ifndef __BITSTREAM_DVB_DESC_5E_H__
#define __BITSTREAM_DVB_DESC_5E_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/mpeg/psi/desc_0a.h>
#include <bitstream/dvb/si/strings.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x5e: Multilingual component descriptor
 *****************************************************************************/
#define DESC5E_HEADER_SIZE      (DESC_HEADER_SIZE + 1)
#define DESC5E_DATA_SIZE        4

static inline void desc5e_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x5e);
    desc_set_length(p_desc, DESC5E_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline uint8_t desc5e_get_component_tag(const uint8_t *p_desc)
{
    return p_desc[2];
}

static inline void desc5e_set_component_tag(uint8_t *p_desc, uint8_t i_component_tag)
{
    p_desc[2] = i_component_tag;
}

#define desc5en_set_code desc0an_set_code
#define desc5en_get_code desc0an_get_code

static inline uint8_t desc5en_get_text_length(const uint8_t *p_desc_n)
{
    return p_desc_n[3];
}

static inline const uint8_t *desc5en_get_text(const uint8_t *p_desc_n, uint8_t *pi_length)
{
    *pi_length = desc5en_get_text_length(p_desc_n);
    return p_desc_n + 4;
}

static inline void desc5en_set_text(uint8_t *p_desc_n, const uint8_t *p_text, uint8_t i_length)
{
    p_desc_n[3] = i_length;
    memcpy(p_desc_n + 4, p_text, i_length);
}

static inline uint8_t *desc5e_get_data(const uint8_t *p_desc, uint8_t n)
{
    const uint8_t *p_desc_n = p_desc + DESC5E_HEADER_SIZE;
    uint8_t i_desc_size = desc_get_length(p_desc);

    while (n) {
        if (p_desc_n + DESC5E_DATA_SIZE - p_desc > i_desc_size)
            return NULL;
        p_desc_n += DESC5E_DATA_SIZE + desc5en_get_text_length(p_desc_n);
        n--;
    }
    if (p_desc_n - p_desc > i_desc_size)
        return NULL;
    return (uint8_t *)p_desc_n;
}

static inline bool desc5e_validate(const uint8_t *p_desc)
{
    const uint8_t *p_desc_n = p_desc + DESC5E_HEADER_SIZE;
    int i_desc_size = desc_get_length(p_desc);
    while (i_desc_size > (DESC5E_HEADER_SIZE - DESC_HEADER_SIZE))
    {
        uint8_t i_data_sz = DESC5E_DATA_SIZE + desc5en_get_text_length(p_desc_n);
        i_desc_size -= i_data_sz;
        p_desc_n    += i_data_sz;
    }
    return i_desc_size == (DESC5E_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline void desc5e_print(const uint8_t *p_desc,
                                f_print pf_print, void *print_opaque,
                                f_iconv pf_iconv, void *iconv_opaque,
                                print_type_t i_print_type)
{
    const uint8_t *p_desc_n;
    uint8_t j = 0;

    while ((p_desc_n = desc5e_get_data(p_desc, j++)) != NULL) {
        uint8_t i_text_length;
        const uint8_t *p_text = desc5en_get_text(p_desc_n, &i_text_length);
        char *psz_text = dvb_string_get(p_text, i_text_length, pf_iconv, iconv_opaque);
        switch (i_print_type) {
        case PRINT_XML:
            psz_text = dvb_string_xml_escape(psz_text);
            pf_print(print_opaque,
                     "<MULTILINGUAL_COMPONENT_DESC code=\"%3.3s\" component_tag=\"%u\" text=\"%s\"/>",
                     desc5en_get_code(p_desc_n),
                     desc5e_get_component_tag(p_desc),
                     psz_text
                    );
            break;
        default:
            pf_print(print_opaque,
                     "    - desc 5e multilingual_component code=\"%3.3s\" component_tag=%u text=\"%s\"",
                     desc5en_get_code(p_desc_n),
                     desc5e_get_component_tag(p_desc),
                     psz_text
                    );
        }
        free(psz_text);
    }
}

#ifdef __cplusplus
}
#endif

#endif
