/*****************************************************************************
 * desc_69.h: ETSI EN 300 468 Descriptor 0x69: PDC descriptor
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

#ifndef __BITSTREAM_DVB_DESC_69_H__
#define __BITSTREAM_DVB_DESC_69_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x69: PDC descriptor
 *****************************************************************************/
#define DESC69_HEADER_SIZE      (DESC_HEADER_SIZE + 3)

static inline void desc69_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x69);
    desc_set_length(p_desc, DESC69_HEADER_SIZE - DESC_HEADER_SIZE);
    p_desc[2] = 0xf0;
}

static inline uint32_t desc69_get_pil(const uint8_t *p_desc)
{
    return ((p_desc[2] & 0x0f) << 16) | (p_desc[3] << 8) | p_desc[4];
}

static inline void desc69_set_pil(uint8_t *p_desc, uint32_t i_pil)
{
    p_desc[2] = ((i_pil >> 16) & 0x0f) | 0xf0;
    p_desc[3] =  (i_pil >>  8) & 0xff;
    p_desc[4] =   i_pil        & 0x0f;
}

static inline uint8_t desc69_get_day(const uint8_t *p_desc)
{
    uint8_t i_day = ((p_desc[2] & 0x0f) << 1) | ((p_desc[3] & 0x7f) >> 7); // rrrr1111 1xxxxxxx
    return i_day + 1;
}

static inline void desc69_set_day(uint8_t *p_desc, uint8_t i_day)
{
    i_day &= 0x1f; // 5 bits
    p_desc[2] = 0xf0 | ((i_day >> 1) & 0x0f); // rrrr1111
    p_desc[3] = ((i_day & 0x01) << 7) | (p_desc[3] & 0x7f); // 1xxxxxxx
}

static inline uint8_t desc69_get_month(const uint8_t *p_desc)
{
    uint8_t i_month = ((p_desc[3] & 0x78) >> 3); // x1111xxx
    return i_month;
}

static inline void desc69_set_month(uint8_t *p_desc, uint8_t i_month)
{
    i_month &= 0x0f; // 4 bits
    p_desc[3] = (p_desc[3] & 0x87) | (i_month << 3); // x1111xxx
}


static inline uint8_t desc69_get_hour(const uint8_t *p_desc)
{
    uint8_t i_hour = ((p_desc[3] & 0x07) << 2) | ((p_desc[4] & 0xc0) >> 6); // xxxxx111 11xxxxxx
    return i_hour;
}

static inline void desc69_set_hour(uint8_t *p_desc, uint8_t i_hour)
{
    i_hour &= 0x1f; // 5 bits
    p_desc[3] = (p_desc[3] & 0xf8) | ((i_hour >> 3) & 0x07); // xxxxx111
    p_desc[4] = ((i_hour & 0x03) << 6) | (p_desc[4] & 0x3f); // 11xxxxxx
}

static inline uint8_t desc69_get_minute(const uint8_t *p_desc)
{
    uint8_t i_min = p_desc[4] & 0x3f; // xx111111
    return i_min;
}

static inline void desc69_set_minute(uint8_t *p_desc, uint8_t i_minute)
{
    i_minute &= 0x3f;
    p_desc[4] = (p_desc[4] & 0xc0) | i_minute;
}

static inline bool desc69_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= (DESC69_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline void desc69_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 "<PDC_DESC pil=\"0x%x\" day=\"%u\" month=\"%u\" hour=\"%u\" min=\"%u\"/>",
                 desc69_get_pil(p_desc),
                 desc69_get_day(p_desc),
                 desc69_get_month(p_desc),
                 desc69_get_hour(p_desc),
                 desc69_get_minute(p_desc)
                );
        break;
    default:
        pf_print(opaque,
                 "    - desc 69 pdc pil=\"0x%x\" day=\"%u\" month=\"%u\" hour=\"%u\" min=\"%u\"",
                 desc69_get_pil(p_desc),
                 desc69_get_day(p_desc),
                 desc69_get_month(p_desc),
                 desc69_get_hour(p_desc),
                 desc69_get_minute(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
