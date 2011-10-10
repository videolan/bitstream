/*****************************************************************************
 * desc_46.h: ETSI EN 300 468 Descriptor 0x46: VBI teletext descriptor
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

#ifndef __BITSTREAM_DVB_DESC_46_H__
#define __BITSTREAM_DVB_DESC_46_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/mpeg/psi/desc_0a.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x46: VBI teletext descriptor
 *****************************************************************************/
#define DESC46_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC46_LANGUAGE_SIZE    5

static inline void desc46_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x46);
}

static inline uint8_t *desc46_get_language(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC46_HEADER_SIZE + n * DESC46_LANGUAGE_SIZE;
    if (p_desc_n + DESC46_LANGUAGE_SIZE - p_desc
         > desc_get_length(p_desc) + DESC46_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

#define desc46n_set_code desc0an_set_code
#define desc46n_get_code desc0an_get_code

static inline void desc46n_set_teletexttype(uint8_t *p_desc_n, uint8_t i_type)
{
    p_desc_n[3] &= ~0xf8;
    p_desc_n[3] |= (i_type << 3) & 0xf8;
}

static inline uint8_t desc46n_get_teletexttype(const uint8_t *p_desc_n)
{
    return p_desc_n[3] >> 3;
}

static inline char *desc46_get_teletexttype_txt(uint8_t i_type)
{
    return i_type == 0x00 ? "Reserved" :
           i_type == 0x01 ? "Initial teletext page" :
           i_type == 0x02 ? "Teletext subtitle page" :
           i_type == 0x03 ? "Additional information page" :
           i_type == 0x04 ? "Programme schedule page" :
           i_type == 0x05 ? "Teletext subtitle page for hearing impaired people schedule page" : "Reserved";
}

static inline void desc46n_set_teletextmagazine(uint8_t *p_desc_n,
                                                uint8_t i_magazine)
{
    p_desc_n[3] &= ~0x7;
    p_desc_n[3] |= (i_magazine & 0x7);
}

static inline uint8_t desc46n_get_teletextmagazine(const uint8_t *p_desc_n)
{
    return p_desc_n[3] & 0x7;
}

static inline void desc46n_set_teletextpage(uint8_t *p_desc_n, uint8_t i_page)
{
    p_desc_n[4] = i_page;
}

static inline uint8_t desc46n_get_teletextpage(const uint8_t *p_desc_n)
{
    return p_desc_n[4];
}

static inline bool desc46_validate(const uint8_t *p_desc)
{
    return !(desc_get_length(p_desc) % DESC46_LANGUAGE_SIZE);
}

static inline void desc46_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc46_get_language(p_desc, j)) != NULL) {
        j++;
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque,
                 "<%s language=\"%3.3s\" type=\"0x%hhx\" type_txt=\"%s\" mag=\"%hhu\" page=\"0x%hhux\"/>",
                 desc_get_tag(p_desc) == 0x46 ? "VBI_TELX_DESC" : "TELX_DESC",
                 (const char *)desc46n_get_code(p_desc_n),
                 desc46n_get_teletexttype(p_desc_n),
                 desc46_get_teletexttype_txt(desc46n_get_teletexttype(p_desc_n)),
                 desc46n_get_teletextmagazine(p_desc_n),
                 desc46n_get_teletextpage(p_desc_n));
            break;
        default:
            pf_print(opaque,
                 "    - desc %x %s language=%3.3s type=0x%hhx type_txt=\"%s\" mag=%hhu page=0x%hhux",
                 desc_get_tag(p_desc),
                 desc_get_tag(p_desc) == 0x46 ? "vbi_telx" : "telx",
                 (const char *)desc46n_get_code(p_desc_n),
                 desc46n_get_teletexttype(p_desc_n),
                 desc46_get_teletexttype_txt(desc46n_get_teletexttype(p_desc_n)),
                 desc46n_get_teletextmagazine(p_desc_n),
                 desc46n_get_teletextpage(p_desc_n));
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
