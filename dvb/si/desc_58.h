/*****************************************************************************
 * desc_58.h: ETSI EN 300 468 Descriptor 0x58: Local time offset descriptor
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

#ifndef __BITSTREAM_DVB_DESC_58_H__
#define __BITSTREAM_DVB_DESC_58_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/mpeg/psi/desc_0a.h>
#include <bitstream/dvb/si/datetime.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x58: Local time offset descriptor
 *****************************************************************************/
#define DESC58_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC58_LTO_SIZE         13

static inline void desc58_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x58);
}

static inline uint8_t *desc58_get_lto(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC58_HEADER_SIZE + n * DESC58_LTO_SIZE;
    if (p_desc_n + DESC58_LTO_SIZE - p_desc
         > desc_get_length(p_desc) + DESC58_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

static inline const uint8_t *desc58n_get_country_code(const uint8_t *p_desc_n)
{
    return desc0an_get_code(p_desc_n);
}

static inline void desc58n_set_country_code(uint8_t *p_desc_n, const uint8_t p_code[3])
{
    desc0an_set_code(p_desc_n, p_code);
}

static inline uint8_t desc58n_get_country_region_id(const uint8_t *p_desc_n)
{
    return (p_desc_n[3] & 0xfc) >> 2;
}

static inline void desc58n_set_country_region_id(uint8_t *p_desc_n, uint8_t i_region_id)
{
    p_desc_n[3] = (i_region_id << 2) | 0x02 | (p_desc_n[3] & 0x01);
}

static inline uint8_t desc58n_get_lto_polarity(const uint8_t *p_desc_n)
{
    return p_desc_n[3] & 0x01;
}

static inline void desc58n_set_lto_polarity(uint8_t *p_desc_n, uint8_t i_lto_polarity)
{
    p_desc_n[3] = i_lto_polarity ? (p_desc_n[3] | 0x01) : (p_desc_n[3] &~ 0x01);
}

static inline uint16_t desc58n_get_lt_offset(const uint8_t *p_desc_n)
{
    return (p_desc_n[4] << 8) | p_desc_n[5];
}

static inline void desc58n_set_lt_offset(uint8_t *p_desc_n, uint16_t i_lt_offset)
{
    p_desc_n[4] = i_lt_offset >> 8;
    p_desc_n[5] = i_lt_offset & 0xff;
}

static inline uint64_t desc58n_get_time_of_change(const uint8_t *p_desc_n)
{
    return (uint64_t)(((uint64_t)p_desc_n[6] << 32) | ((uint64_t)p_desc_n[7] << 24) |
                      ((uint64_t)p_desc_n[8] << 16) | ((uint64_t)p_desc_n[9] << 8) |
                       p_desc_n[10]);
}

static inline void desc58n_set_time_of_change(uint8_t *p_desc_n, uint64_t i_time_of_change)
{
    p_desc_n[6] = (i_time_of_change >> 32) & 0xff;
    p_desc_n[7] = (i_time_of_change >> 24) & 0xff;
    p_desc_n[8] = (i_time_of_change >> 16) & 0xff;
    p_desc_n[9] = (i_time_of_change >> 8) & 0xff;
    p_desc_n[10] = i_time_of_change & 0xff;
}

static inline uint16_t desc58n_get_next_offset(const uint8_t *p_desc_n)
{
    return (p_desc_n[11] << 8) | p_desc_n[12];
}

static inline void desc58n_set_next_offset(uint8_t *p_desc_n, uint16_t i_next_offset)
{
    p_desc_n[11] = i_next_offset >> 8;
    p_desc_n[12] = i_next_offset & 0xff;
}

static inline bool desc58_validate(const uint8_t *p_desc)
{
    return !(desc_get_length(p_desc) % DESC58_LTO_SIZE);
}

static inline void desc58_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc58_get_lto(p_desc, j++)) != NULL) {
        char txt_time_of_change[24];
        time_t ts_time_of_change;

        ts_time_of_change = dvb_time_format_UTC(desc58n_get_time_of_change(p_desc_n),
                            NULL, txt_time_of_change);

        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque,
              "<LOCAL_TIME_OFFSET_DESC country_code=\"%3.3s\" country_region_id=\"%u\""
              " lto_polarity=\"%u\" lt_offset=\"%04x\" time_of_change=\"%ld\""
              " time_of_change_dec=\"%s\" next_time_offset=\"%04x\"/>",
              (const char *)desc58n_get_country_code(p_desc_n),
              desc58n_get_country_region_id(p_desc_n),
              desc58n_get_lto_polarity(p_desc_n),
              desc58n_get_lt_offset(p_desc_n),
              ts_time_of_change, txt_time_of_change,
              desc58n_get_next_offset(p_desc_n)
            );
            break;
        default:
            pf_print(opaque,
              "    - desc 58 local_time_offset country_code=%3.3s country_region_id=%u"
              " lto_polarity=%u lto_offset=%04x time_of_change=%ld"
              " time_of_change_dec=\"%s\" next_time_offset=%04x",
              (const char *)desc58n_get_country_code(p_desc_n),
              desc58n_get_country_region_id(p_desc_n),
              desc58n_get_lto_polarity(p_desc_n),
              desc58n_get_lt_offset(p_desc_n),
              ts_time_of_change, txt_time_of_change,
              desc58n_get_next_offset(p_desc_n)
             );
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
