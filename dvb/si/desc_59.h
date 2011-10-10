/*****************************************************************************
 * desc_59.h: ETSI EN 300 468 Descriptor 0x59: Subtitling descriptor
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
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

#ifndef __BITSTREAM_DVB_DESC_59_H__
#define __BITSTREAM_DVB_DESC_59_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/mpeg/psi/desc_0a.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x59: Subtitling descriptor
 *****************************************************************************/
#define DESC59_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC59_LANGUAGE_SIZE    8

static inline void desc59_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x59);
}

static inline uint8_t *desc59_get_language(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC59_HEADER_SIZE + n * DESC59_LANGUAGE_SIZE;
    if (p_desc_n + DESC59_LANGUAGE_SIZE - p_desc
         > desc_get_length(p_desc) + DESC59_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

#define desc59n_set_code desc0an_set_code
#define desc59n_get_code desc0an_get_code

static inline void desc59n_set_subtitlingtype(uint8_t *p_desc_n, uint8_t i_type)
{
    p_desc_n[3] = i_type;
}

static inline uint8_t desc59n_get_subtitlingtype(const uint8_t *p_desc_n)
{
    return p_desc_n[3];
}

static inline void desc59n_set_compositionpage(uint8_t *p_desc_n,
                                               uint16_t i_page)
{
    p_desc_n[4] = i_page >> 8;
    p_desc_n[5] = i_page & 0xff;
}

static inline uint16_t desc59n_get_compositionpage(const uint8_t *p_desc_n)
{
    return (p_desc_n[4] << 8) | p_desc_n[5];
}

static inline void desc59n_set_ancillarypage(uint8_t *p_desc_n, uint16_t i_page)
{
    p_desc_n[6] = i_page >> 8;
    p_desc_n[7] = i_page & 0xff;
}

static inline uint16_t desc59n_get_ancillarypage(const uint8_t *p_desc_n)
{
    return (p_desc_n[6] << 8) | p_desc_n[7];
}

static inline bool desc59_validate(const uint8_t *p_desc)
{
    return !(desc_get_length(p_desc) % DESC59_LANGUAGE_SIZE);
}

static inline void desc59_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc59_get_language(p_desc, j)) != NULL) {
        j++;
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque,
            "<SUBTITLING_DESC language=\"%3.3s\" type=\"0x%hhx\" composition=\"%hu\" ancillary=\"%hu\"/>",
            (const char *)desc59n_get_code(p_desc_n),
            desc59n_get_subtitlingtype(p_desc_n),
            desc59n_get_compositionpage(p_desc_n),
            desc59n_get_ancillarypage(p_desc_n));
            break;
        default:
            pf_print(opaque,
            "    - desc 59 subtitling language=%3.3s type=0x%hhx composition=%hu ancillary=%hu",
            (const char *)desc59n_get_code(p_desc_n),
            desc59n_get_subtitlingtype(p_desc_n),
            desc59n_get_compositionpage(p_desc_n),
            desc59n_get_ancillarypage(p_desc_n));
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
