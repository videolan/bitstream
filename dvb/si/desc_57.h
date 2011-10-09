/*****************************************************************************
 * desc_57.h: ETSI EN 300 468 Descriptor 0x57: Telephone descriptor
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

#ifndef __BITSTREAM_DVB_DESC_57_H__
#define __BITSTREAM_DVB_DESC_57_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x57: Telephone descriptor
 *****************************************************************************/
#define DESC57_HEADER_SIZE      (DESC_HEADER_SIZE + 3)

static inline void desc57_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x57);
    desc_set_length(p_desc, DESC57_HEADER_SIZE - DESC_HEADER_SIZE);
    p_desc[2] = 0xc0;
    p_desc[3] = 0x80;
    p_desc[4] = 0x80;
}

static inline bool desc57_get_foreign_availability(const uint8_t *p_desc)
{
    return (p_desc[2] & 0x20) == 0x20;
}


static inline void desc57_set_foreign_availability(uint8_t *p_desc, bool b_foreign_availability)
{
    p_desc[2] = b_foreign_availability ? (p_desc[2] | 0x20) : (p_desc[2] &~ 0x20);
}

static inline uint8_t desc57_get_connection_type(const uint8_t *p_desc)
{
    return p_desc[2] & 0x1f;
}

static inline void desc57_set_connection_type(uint8_t *p_desc, uint8_t i_connection_type)
{
    p_desc[2] = (p_desc[2] & 0xe0) | (i_connection_type & 0x1f);
}


static inline uint8_t desc57_get_country_prefix_length(const uint8_t *p_desc)
{
    return (p_desc[3] & 0x60) >> 5; // x11xxxxx
}

static inline const char *desc57_get_country_prefix(const uint8_t *p_desc, uint8_t *i_length)
{
    *i_length = desc57_get_country_prefix_length(p_desc);
    if (*i_length == 0) return NULL;
    return (const char *)p_desc + DESC57_HEADER_SIZE;
}

static inline void desc57_set_country_prefix(uint8_t *p_desc, const char *p_country_prefix, uint8_t i_length)
{
    p_desc[3] = (p_desc[3] &~ 0x60) | ((i_length & 0x03) << 5); // x11xxxxx
    memcpy(p_desc + DESC57_HEADER_SIZE, p_country_prefix, i_length);
}


static inline uint8_t desc57_get_international_area_code_length(const uint8_t *p_desc)
{
    return (p_desc[3] & 0x1c) >> 2; // xxx111xx
}

static inline const char *desc57_get_international_area_code(const uint8_t *p_desc, uint8_t *i_length)
{
    *i_length = desc57_get_international_area_code_length(p_desc);
    if (*i_length == 0) return NULL;
    return (const char *)p_desc + DESC57_HEADER_SIZE
            + desc57_get_country_prefix_length(p_desc);
}

static inline void desc57_set_international_area_code(uint8_t *p_desc, const char *p_international_area_code, uint8_t i_length)
{
    p_desc[3] = (p_desc[3] &~ 0x1c) | ((i_length & 0x07) << 2); // xxx111xx
    memcpy(p_desc + DESC57_HEADER_SIZE
            + desc57_get_country_prefix_length(p_desc),
           p_international_area_code, i_length);
}


static inline uint8_t desc57_get_operator_code_length(const uint8_t *p_desc)
{
    return (p_desc[3] & 0x03); // xxxxxx11
}

static inline const char *desc57_get_operator_code(const uint8_t *p_desc, uint8_t *i_length)
{
    *i_length = desc57_get_operator_code_length(p_desc);
    if (*i_length == 0) return NULL;
    return (const char *)p_desc + DESC57_HEADER_SIZE
            + desc57_get_country_prefix_length(p_desc)
            + desc57_get_international_area_code_length(p_desc);
}

static inline void desc57_set_operator_code(uint8_t *p_desc, const char *p_operator_code, uint8_t i_length)
{
    p_desc[3] = (p_desc[3] &~ 0x03) | (i_length & 0x03); // xxxxxx11
    memcpy(p_desc + DESC57_HEADER_SIZE
            + desc57_get_country_prefix_length(p_desc)
            + desc57_get_international_area_code_length(p_desc),
           p_operator_code, i_length);
}


static inline uint8_t desc57_get_national_area_code_length(const uint8_t *p_desc)
{
    return (p_desc[4] & 0x70) >> 4; // x111xxxx
}

static inline const char *desc57_get_national_area_code(const uint8_t *p_desc, uint8_t *i_length)
{
    *i_length = desc57_get_national_area_code_length(p_desc);
    if (*i_length == 0) return NULL;
    return (const char *)p_desc + DESC57_HEADER_SIZE
            + desc57_get_country_prefix_length(p_desc)
            + desc57_get_international_area_code_length(p_desc)
            + desc57_get_operator_code_length(p_desc);
}

static inline void desc57_set_national_area_code(uint8_t *p_desc, const char *p_national_area_code, uint8_t i_length)
{
    p_desc[4] = (p_desc[4] &~ 0x70) | ((i_length & 0x07) << 4); // x111xxxx
    memcpy(p_desc + DESC57_HEADER_SIZE
            + desc57_get_country_prefix_length(p_desc)
            + desc57_get_international_area_code_length(p_desc)
            + desc57_get_operator_code_length(p_desc),
           p_national_area_code, i_length);
}


static inline uint8_t desc57_get_core_number_length(const uint8_t *p_desc)
{
    return (p_desc[4] & 0x0f); // xxxx1111
}

static inline const char *desc57_get_core_number(const uint8_t *p_desc, uint8_t *i_length)
{
    *i_length = desc57_get_core_number_length(p_desc);
    if (*i_length == 0) return NULL;
    return (const char *)p_desc + DESC57_HEADER_SIZE
            + desc57_get_country_prefix_length(p_desc)
            + desc57_get_international_area_code_length(p_desc)
            + desc57_get_operator_code_length(p_desc)
            + desc57_get_national_area_code_length(p_desc);
}

static inline void desc57_set_core_number(uint8_t *p_desc, const char *p_core_number, uint8_t i_length)
{
    p_desc[4] = (p_desc[4] &~ 0x0f) | (i_length & 0x0f);
    memcpy(p_desc + DESC57_HEADER_SIZE
            + desc57_get_country_prefix_length(p_desc)
            + desc57_get_international_area_code_length(p_desc)
            + desc57_get_operator_code_length(p_desc)
            + desc57_get_national_area_code_length(p_desc),
           p_core_number, i_length);
}


static inline uint8_t desc57_get_phone_length(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) - (DESC57_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline const char *desc57_get_phone(const uint8_t *p_desc, uint8_t *i_length)
{
    *i_length = desc57_get_phone_length(p_desc);
    if (*i_length == 0) return NULL;
    return (const char *)p_desc + DESC57_HEADER_SIZE;
}


static inline void desc57_set_length(uint8_t *p_desc)
{
    desc_set_length(p_desc, DESC57_HEADER_SIZE - DESC_HEADER_SIZE +
                            + desc57_get_country_prefix_length(p_desc)
                            + desc57_get_international_area_code_length(p_desc)
                            + desc57_get_operator_code_length(p_desc)
                            + desc57_get_national_area_code_length(p_desc)
                            + desc57_get_core_number_length(p_desc));
}

static inline bool desc57_validate(const uint8_t *p_desc)
{
    return (p_desc + DESC57_HEADER_SIZE
                   + desc57_get_country_prefix_length(p_desc)
                   + desc57_get_international_area_code_length(p_desc)
                   + desc57_get_operator_code_length(p_desc)
                   + desc57_get_national_area_code_length(p_desc)
                   + desc57_get_core_number_length(p_desc))
           == (p_desc + DESC_HEADER_SIZE + desc_get_length(p_desc));
}

static inline void desc57_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t i_country_prefix_len, i_international_area_code_len, i_operator_code_len;
    uint8_t i_national_area_code_len, i_core_number_len, i_phone;
    const char *empty = "";
    const char *p_country_prefix     = desc57_get_country_prefix(p_desc, &i_country_prefix_len);
    const char *p_international_area_code = desc57_get_international_area_code(p_desc, &i_international_area_code_len);
    const char *p_operator_code      = desc57_get_operator_code(p_desc, &i_operator_code_len);
    const char *p_national_area_code = desc57_get_national_area_code(p_desc, &i_national_area_code_len);
    const char *p_core_number        = desc57_get_core_number(p_desc, &i_core_number_len);
    const char *p_phone              = desc57_get_phone(p_desc, &i_phone);

    if (!p_country_prefix) p_country_prefix = empty;
    if (!p_international_area_code) p_international_area_code = empty;
    if (!p_operator_code) p_operator_code = empty;
    if (!p_national_area_code) p_national_area_code = empty;
    if (!p_core_number) p_core_number = empty;
    if (!p_phone) p_phone = empty;

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 "<TELEPHONE_DESC foreign_availability=\"%u\" connection_type=\"%u\""
                 " country_prefix=\"%.*s\" international_area_code=\"%.*s\""
                 " operator_code=\"%.*s\" national_area_code=\"%.*s\""
                 " core_number=\"%.*s\" full_telephone=\"%.*s\"/>",
                 desc57_get_foreign_availability(p_desc),
                 desc57_get_connection_type(p_desc),
                 i_country_prefix_len, p_country_prefix,
                 i_international_area_code_len, p_international_area_code,
                 i_operator_code_len, p_operator_code,
                 i_national_area_code_len, p_national_area_code,
                 i_core_number_len, p_core_number,
                 i_phone, p_phone
                );
        break;
    default:
        pf_print(opaque,
                 "    - desc 57 telephone foreign_availability=%u connection_type=%u"
                 " country_prefix=%.*s international_area_code=%.*s"
                 " operator_code=%.*s national_area_code=%.*s"
                 " core_number=%.*s phone=%.*s",
                 desc57_get_foreign_availability(p_desc),
                 desc57_get_connection_type(p_desc),
                 i_country_prefix_len, p_country_prefix,
                 i_international_area_code_len, p_international_area_code,
                 i_operator_code_len, p_operator_code,
                 i_national_area_code_len, p_national_area_code,
                 i_core_number_len, p_core_number,
                 i_phone, p_phone
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
