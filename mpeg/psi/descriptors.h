/*****************************************************************************
 * descriptors.h: ISO/IEC 13818-1 descriptors
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
 *  - ISO/IEC 13818-1:2007(E) (MPEG-2 Systems)
 */

#ifndef __BITSTREAM_MPEG_DESCRIPTORS_H__
#define __BITSTREAM_MPEG_DESCRIPTORS_H__

#include <bitstream/common.h>

#include <stdio.h> /* sprintf */

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor
 *****************************************************************************/
#define DESC_HEADER_SIZE        2

static inline void desc_set_tag(uint8_t *p_desc, uint8_t i_tag)
{
    p_desc[0] = i_tag;
}

static inline uint8_t desc_get_tag(const uint8_t *p_desc)
{
    return p_desc[0];
}

static inline void desc_set_length(uint8_t *p_desc, uint8_t i_length)
{
    p_desc[1] = i_length;
}

static inline uint8_t desc_get_length(const uint8_t *p_desc)
{
    return p_desc[1];
}

static inline void desc_print_begin(const uint8_t *p_desc, f_print pf_print,
                                    void *opaque, print_type_t i_print_type)
{

    switch (i_print_type) {
    case PRINT_XML:
    {
        uint8_t i, i_length = desc_get_length(p_desc);
        char psz_value[2 * i_length + 1];

        for (i = 0; i < i_length; i++)
            sprintf(psz_value + 2 * i, "%2.2hhx", p_desc[2 + i]);
        psz_value[2 * i] = '\0';

        pf_print(opaque, "<DESC id=\"0x%02x\" length=\"%u\" value=\"%s\">",
                 desc_get_tag(p_desc), i_length, psz_value);
        break;
    }
    default:
        break;
    }
}

static inline void desc_print_end(const uint8_t *p_desc, f_print pf_print,
                                  void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "</DESC>");
        break;
    default:
        break;
    }
}

static inline void desc_print_error(const uint8_t *p_desc, f_print pf_print,
                                    void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<INVALID_DESC />");
        break;
    default:
         pf_print(opaque, "desc %2.2hhx invalid", desc_get_tag(p_desc));
    }
}

static inline void desc_print(const uint8_t *p_desc, f_print pf_print,
                              void *opaque, print_type_t i_print_type)
{
    uint8_t i, i_length;
    char psz_value[2 * 255 + 1];

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<UNKNOWN_DESC />");
        break;
    default:
        i_length = desc_get_length(p_desc);

        for (i = 0; i < i_length; i++)
            sprintf(psz_value + 2 * i, "%02x", p_desc[2 + i]);
        psz_value[2 * i] = '\0';

        pf_print(opaque, "    - desc %02x unknown length=%u value=%s",
            desc_get_tag(p_desc), i_length, psz_value);
    }
}

/*****************************************************************************
 * Descriptors list
 *****************************************************************************/
static inline uint8_t *descl_get_desc(uint8_t *p_descl, uint16_t i_length,
                                      uint16_t n)
{
    uint8_t *p_desc = p_descl;

    while (n) {
        if (p_desc + DESC_HEADER_SIZE - p_descl > i_length) return NULL;
        p_desc += DESC_HEADER_SIZE + desc_get_length(p_desc);
        n--;
    }
    if (p_desc - p_descl >= i_length) return NULL;
    return p_desc;
}

static inline bool descl_validate(const uint8_t *p_descl, uint16_t i_length)
{
    const uint8_t *p_desc = p_descl;

    while (p_desc + DESC_HEADER_SIZE - p_descl <= i_length)
        p_desc += DESC_HEADER_SIZE + desc_get_length(p_desc);

    return (p_desc - p_descl == i_length);
}

/*****************************************************************************
 * Descriptors structure
 *****************************************************************************/
#define DESCS_HEADER_SIZE       2
#define DESCS_MAX_SIZE          4095

static inline void descs_set_length(uint8_t *p_descs, uint16_t i_length)
{
    p_descs[0] &= 0xf0;
    p_descs[0] |= (i_length >> 8) & 0xf;
    p_descs[1] = i_length & 0xff;
}

static inline uint16_t descs_get_length(const uint8_t *p_descs)
{
    return ((p_descs[0] & 0xf) << 8) | p_descs[1];
}

static inline uint8_t *descs_get_desc(uint8_t *p_descs, uint16_t n)
{
    return descl_get_desc(p_descs + DESCS_HEADER_SIZE,
                          descs_get_length(p_descs), n);
}

static inline bool descs_validate(const uint8_t *p_descs)
{
    return descl_validate(p_descs + DESCS_HEADER_SIZE,
                          descs_get_length(p_descs));
}

#ifdef __cplusplus
}
#endif

#endif
