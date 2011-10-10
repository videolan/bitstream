/*****************************************************************************
 * desc_55.h: ETSI EN 300 468 Descriptor 0x55: Parental rating descriptor
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

#ifndef __BITSTREAM_DVB_DESC_55_H__
#define __BITSTREAM_DVB_DESC_55_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/mpeg/psi/desc_0a.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x55: Parental rating descriptor
 *****************************************************************************/
#define DESC55_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC55_RATING_SIZE      4

static inline void desc55_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x55);
}

#define desc55n_get_country_code desc0an_get_code
#define desc55n_set_country_code desc0an_set_code

static inline uint8_t *desc55_get_rating(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC55_HEADER_SIZE + n * DESC55_RATING_SIZE;
    if (p_desc_n + DESC55_RATING_SIZE - p_desc
         > desc_get_length(p_desc) + DESC55_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

static inline uint8_t desc55n_get_rating(uint8_t *p_desc_n)
{
    return p_desc_n[3];
}

static inline void desc55n_set_rating(uint8_t *p_desc_n, uint8_t i_rating)
{
    p_desc_n[3] = i_rating;
}

static inline bool desc55_validate(const uint8_t *p_desc)
{
    return !(desc_get_length(p_desc) % DESC55_RATING_SIZE);
}

static inline void desc55_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc55_get_rating(p_desc, j++)) != NULL) {
        char rating_txt[16];
        uint8_t rating = desc55n_get_rating(p_desc_n);
        if (rating == 0)
            sprintf(rating_txt, "%s", "undefined");
        else if (rating >= 0x01 && rating <= 0x0f)
            sprintf(rating_txt, "min %d years", rating + 3);
        else
            sprintf(rating_txt, "%s", "unknown");
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque, "<PARENTAL_RATING_DESC country_code=\"%3.3s\" rating=\"%u\" rating_txt=\"%s\"/>",
                     (char *)desc55n_get_country_code(p_desc_n),
                     desc55n_get_rating(p_desc_n),
                     rating_txt
                    );
            break;
        default:
            pf_print(opaque,"    - desc 55 parental_rating country_code=%3.3s rating=%u rating_txt=\"%s\"",
                     (char *)desc55n_get_country_code(p_desc_n),
                     desc55n_get_rating(p_desc_n),
                     rating_txt
                    );
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
