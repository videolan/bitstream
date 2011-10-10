/*****************************************************************************
 * desc_4e.h: ETSI EN 300 468 Descriptor 0x4e: Extended event descriptor
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

#ifndef __BITSTREAM_DVB_DESC_4E_H__
#define __BITSTREAM_DVB_DESC_4E_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/dvb/si/strings.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x4e: Extended event descriptor
 *****************************************************************************/
#define DESC4E_HEADER_SIZE      (DESC_HEADER_SIZE + 4)

static inline void desc4e_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x4e);
}

static inline uint8_t desc4e_get_desc_number(uint8_t *p_desc)
{
    return p_desc[2] >> 4;
}

static inline void desc4e_set_desc_number(uint8_t *p_desc, uint8_t i_desc_number)
{
    p_desc[2] = (i_desc_number << 4) | (p_desc[2] & 0x0f);
}

static inline uint8_t desc4e_get_last_desc_number(uint8_t *p_desc)
{
    return p_desc[2] & 0x0f;
}

static inline void desc4e_set_last_desc_number(uint8_t *p_desc, uint8_t i_last_desc_number)
{
    p_desc[2] = (p_desc[2] & 0xf0) | (i_last_desc_number & 0x0f);
}

static inline const uint8_t *desc4e_get_lang(const uint8_t *p_desc)
{
    return p_desc + 3;
}

static inline void desc4e_set_lang(uint8_t *p_desc_n, const uint8_t p_code[3])
{
    p_desc_n[3] = p_code[0];
    p_desc_n[4] = p_code[1];
    p_desc_n[5] = p_code[2];
}

static inline uint8_t desc4e_get_items_length(const uint8_t *p_desc)
{
    return p_desc[6];
}

static inline void desc4e_set_items_length(uint8_t *p_desc, uint8_t i_length)
{
    p_desc[6] = i_length;
}

static inline uint8_t *desc4e_get_item(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p = p_desc + DESC4E_HEADER_SIZE;
    uint8_t *p_item, *p_items, *p_items_end;

    uint8_t i_items_len = *p;
    if (!i_items_len)
        return NULL;

    p_items = p_item = p + 1;
    p_items_end = p_items + i_items_len;

    if (n == 0)
        return p_item;

    while ( p_item < p_items_end ) {
        p_item += 1 + p_item[0]; /* Skip item description */
        p_item += 1 + p_item[0]; /* Skip item text */
        if ( --n == 0 && p_item < p_items_end )
            return p_item;
    }

    return NULL;
}

static inline const uint8_t *desc4en_get_item_description(const uint8_t *p_desc_n, uint8_t *i_length)
{
    *i_length = p_desc_n[0];
    return p_desc_n + 1;
}

static inline void desc4en_set_item_description(uint8_t *p_desc_n, uint8_t *item_description, uint8_t i_length)
{
    p_desc_n[0] = i_length;
    memcpy(p_desc_n + 1, item_description, i_length);
}

static inline const uint8_t *desc4en_get_item_text(const uint8_t *p_desc_n, uint8_t *i_length)
{
    const uint8_t *p_ret = p_desc_n + 1 + p_desc_n[0];
    *i_length = p_ret[0];
    return p_ret + 1;
}

static inline void desc4en_set_item_text(uint8_t *p_desc_n, uint8_t *item_text, uint8_t i_length)
{
    uint8_t *p = p_desc_n + 1 + p_desc_n[0];
    p[0] = i_length;
    memcpy(p + 1, item_text, i_length);
}

static inline uint8_t *desc4e_get_text(uint8_t *p_desc, uint8_t *i_length)
{
    uint8_t *p = p_desc + DESC4E_HEADER_SIZE + 1 + desc4e_get_items_length(p_desc);
    *i_length = p[0];
    return p + 1;
}

static inline void desc4e_set_text(uint8_t *p_desc, uint8_t *p_text, uint8_t i_length)
{
    uint8_t *p = p_desc + DESC4E_HEADER_SIZE + 1 + desc4e_get_items_length(p_desc);
    p[0] = i_length;
    memcpy(p + 1, p_text, i_length);
}

static inline void desc4e_set_length(uint8_t *p_desc)
{
    uint8_t i_text_len;
    desc4e_get_text(p_desc, &i_text_len);
    desc_set_length(p_desc, (DESC4E_HEADER_SIZE - DESC_HEADER_SIZE)
                            + 1 + desc4e_get_items_length(p_desc)
                            + 1 + i_text_len);
}

static inline bool desc4e_validate(const uint8_t *p_desc)
{
    uint8_t i_length = desc_get_length(p_desc);
    int i_items_len;
    const uint8_t *p = p_desc + DESC4E_HEADER_SIZE;
    const uint8_t *p_items, *p_items_end, *p_item;

    if (DESC4E_HEADER_SIZE + 2 > i_length + DESC_HEADER_SIZE)
        return false;

    i_items_len = *p;
    p_items = p_item = p + 1;
    p_items_end = p_items + i_items_len;

    p += 1 + p[0]; /* Check item loop */
    if (p + 1 - p_desc > i_length + DESC_HEADER_SIZE)
        return false;

    p += 1 + p[0]; /* Check text */
    if (p - p_desc > i_length + DESC_HEADER_SIZE)
        return false;

    while ( p_item < p_items_end ) {
        p_item += 1 + p_item[0]; /* Check item description */
        if ( p_item > p_items_end )
            return false;
        p_item += 1 + p_item[0]; /* Check item text */
        if ( p_item > p_items_end )
            return false;
    }

    return true;
}

static inline void desc4e_print(uint8_t *p_desc,
                                f_print pf_print, void *print_opaque,
                                f_iconv pf_iconv, void *iconv_opaque,
                                print_type_t i_print_type)
{
    uint8_t i_text_length;
    uint8_t *p_text = desc4e_get_text(p_desc, &i_text_length);

    char *psz_text = dvb_string_get(p_text, i_text_length,
                                    pf_iconv, iconv_opaque);

    switch (i_print_type) {
    case PRINT_XML:
        psz_text = dvb_string_xml_escape(psz_text);
        pf_print(print_opaque,
                 "<EXTENDED_EVENT_DESC desc_number=\"%u\" last_desc_number=\"%u\""
                 " lang=\"%3.3s\" text=\"%s\">",
                 desc4e_get_desc_number(p_desc),
                 desc4e_get_last_desc_number(p_desc),
                 (char *)desc4e_get_lang(p_desc),
                 psz_text
                );
        break;
    default:
        pf_print(print_opaque,
                 "    - desc 4e extended_event desc_number=%u"
                 " last_desc_number=%u lang=%3.3s text=\"%s\"",
                 desc4e_get_desc_number(p_desc),
                 desc4e_get_last_desc_number(p_desc),
                 (char *)desc4e_get_lang(p_desc),
                 psz_text
                );
    }
    free( psz_text );

    uint8_t j = 0;
    const uint8_t *p_desc_n;
    while ((p_desc_n = desc4e_get_item(p_desc, j++)) != NULL) {
        uint8_t i_desc_length, i_item_length;
        const uint8_t *p_desc = desc4en_get_item_description(p_desc_n, &i_desc_length);
        const uint8_t *p_item = desc4en_get_item_text(p_desc_n, &i_item_length);
        char *psz_desc = dvb_string_get(p_desc, i_desc_length, pf_iconv, iconv_opaque);
        char *psz_item = dvb_string_get(p_item, i_item_length, pf_iconv, iconv_opaque);

        switch (i_print_type) {
        case PRINT_XML:
            psz_desc = dvb_string_xml_escape(psz_desc);
            psz_item = dvb_string_xml_escape(psz_item);
            pf_print(print_opaque, "<EXTENDED_EVENT_ITEM description=\"%s\" text=\"%s\"/>",
                     psz_desc, psz_item);
            break;
        default:
            pf_print(print_opaque,"       - extended_event_item description=\"%s\" text=\"%s\"",
                     psz_desc, psz_item);
        }

        free(psz_desc);
        free(psz_item);
    }

    if (i_print_type == PRINT_XML)
        pf_print(print_opaque, "</EXTENDED_EVENT_DESC>");
}

#ifdef __cplusplus
}
#endif

#endif
