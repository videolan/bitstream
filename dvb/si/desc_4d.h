/*****************************************************************************
 * desc_4d.h: ETSI EN 300 468 Descriptor 0x4d: Short event descriptor
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

#ifndef __BITSTREAM_DVB_DESC_4D_H__
#define __BITSTREAM_DVB_DESC_4D_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/dvb/si/numbers.h>
#include <bitstream/dvb/si/strings.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x4d: Short event descriptor
 *****************************************************************************/
#define DESC4D_HEADER_SIZE      (DESC_HEADER_SIZE + 3)

static inline void desc4d_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x4d);
}

static inline const uint8_t *desc4d_get_lang(const uint8_t *p_desc)
{
    return p_desc + DESC_HEADER_SIZE;
}

static inline void desc4d_set_lang(uint8_t *p_desc, const uint8_t p_lang[3])
{
    p_desc[2] = p_lang[0];
    p_desc[3] = p_lang[1];
    p_desc[4] = p_lang[2];
}

static inline const uint8_t *desc4d_get_event_name(const uint8_t *p_desc,
                                                 uint8_t *pi_length)
{
    const uint8_t *p = p_desc + DESC4D_HEADER_SIZE;
    *pi_length = p[0];
    return p + 1;
}

static inline void desc4d_set_event_name(uint8_t *p_desc,
                                       const uint8_t *p_event_name,
                                       uint8_t i_length)
{
    uint8_t *p = p_desc + DESC4D_HEADER_SIZE;
    p[0] = i_length;
    memcpy(p + 1, p_event_name, i_length);
}

static inline uint8_t *desc4d_get_text(const uint8_t *p_desc,
                                          uint8_t *pi_length)
{
    const uint8_t *p = (uint8_t *)p_desc + DESC4D_HEADER_SIZE + 1 + p_desc[5];
    *pi_length = p[0];
    return (uint8_t *)p + 1;
}

static inline void desc4d_set_text(uint8_t *p_desc,
                                   const uint8_t *p_text,
                                   uint8_t i_length)
{
    uint8_t *p = p_desc + DESC4D_HEADER_SIZE + 1 + p_desc[5];
    p[0] = i_length;
    memcpy(p + 1, p_text, i_length);
}

static inline void desc4d_set_length(uint8_t *p_desc)
{
    uint8_t i_event_name_len, i_text_len;
    desc4d_get_event_name(p_desc, &i_event_name_len);
    desc4d_get_text(p_desc, &i_text_len);
    desc_set_length(p_desc, DESC4D_HEADER_SIZE - DESC_HEADER_SIZE
                            + 1 + i_event_name_len
                            + 1 + i_text_len);
}

static inline bool desc4d_validate(const uint8_t *p_desc)
{
    uint8_t i_length = desc_get_length(p_desc);
    const uint8_t *p = p_desc + DESC4D_HEADER_SIZE;

    p += *p + 1;
    if (DESC4D_HEADER_SIZE + 2 > i_length + DESC_HEADER_SIZE ||
        p + 1 - p_desc > i_length + DESC_HEADER_SIZE)
        return false;

    p += *p + 1;
    if (p - p_desc > i_length + DESC_HEADER_SIZE)
        return false;

    return true;
}

static inline void desc4d_print(const uint8_t *p_desc,
                                f_print pf_print, void *print_opaque,
                                f_iconv pf_iconv, void *iconv_opaque,
                                print_type_t i_print_type)
{
    uint8_t i_event_name_length, i_text_length;
    const uint8_t *p_event_name = desc4d_get_event_name(p_desc, &i_event_name_length);
    const uint8_t *p_text = desc4d_get_text(p_desc, &i_text_length);
    char *psz_event_name = dvb_string_get(p_event_name, i_event_name_length,
                                        pf_iconv, iconv_opaque);
    char *psz_text = dvb_string_get(p_text, i_text_length,
                                       pf_iconv, iconv_opaque);
    switch (i_print_type) {
    case PRINT_XML:
        psz_event_name = dvb_string_xml_escape(psz_event_name);
        psz_text = dvb_string_xml_escape(psz_text);
        pf_print(print_opaque,
                 "<SHORT_EVENT_DESC lang=\"%3.3s\" event_name=\"%s\" text=\"%s\"/>",
                 (char *)desc4d_get_lang(p_desc), psz_event_name, psz_text);
        break;
    default:
        pf_print(print_opaque,
                 "    - desc 4d short_event lang=%3.3s event_name=\"%s\" text=\"%s\"",
                 (char *)desc4d_get_lang(p_desc), psz_event_name, psz_text);
    }
    free(psz_event_name);
    free(psz_text);
}

#ifdef __cplusplus
}
#endif

#endif
