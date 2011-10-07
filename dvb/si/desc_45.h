/*****************************************************************************
 * desc_45.h: ETSI EN 300 458 Descriptor 0x45: VBI data descriptor
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
 *  - ETSI EN 300 458 V1.11.1 (2010-04) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_DESC_45_H__
#define __BITSTREAM_DVB_DESC_45_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x45: VBI data descriptor
 *****************************************************************************/
#define DESC45_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC45_DATA_SIZE        2

static inline void desc45_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x45);
}

static inline void desc45n_set_service_id(uint8_t *p_desc_n, uint8_t i_service_id)
{
    p_desc_n[0] = i_service_id;
}

static inline uint8_t desc45n_get_service_id(const uint8_t *p_desc_n)
{
    return p_desc_n[0];
}

static inline void desc45n_set_data_length(uint8_t *p_desc_n, uint8_t i_length)
{
    p_desc_n[1] = i_length;
}

static inline uint8_t desc45n_get_data_length(const uint8_t *p_desc_n)
{
    return p_desc_n[1];
}

static inline char *desc45_get_service_id_txt(uint8_t i_service_id)
{
    return i_service_id == 0x00 ? "reserved" :
           i_service_id == 0x01 ? "EBU teletext" :
           i_service_id == 0x02 ? "inverted teletext" :
           i_service_id == 0x03 ? "reserved" :
           i_service_id == 0x04 ? "VPS" :
           i_service_id == 0x05 ? "WSS" :
           i_service_id == 0x06 ? "Closed Captioning" :
           i_service_id == 0x07 ? "Mono 4:2:2 samples" : "reserved";
}

static inline uint8_t desc45n_get_byte(uint8_t *p_desc_n, uint8_t idx)
{
    return p_desc_n[idx + 2];
}

static inline void desc45n_set_byte(uint8_t *p_desc_n, uint8_t idx, uint8_t i_byte)
{
    p_desc_n[idx + 2] = i_byte;
}

static inline bool desc45n_get_field_parity(const uint8_t *p_desc_n, uint8_t idx)
{
    return (p_desc_n[idx + 2] & 0x20) == 0x20;
}

static inline void desc45n_set_field_parity(uint8_t *p_desc_n, uint8_t idx, bool b_parity)
{
    p_desc_n[idx + 2] = 0xc0 | (b_parity ? (p_desc_n[idx + 2] | 0x20) : (p_desc_n[idx + 2] &~ 0x20));
}

static inline uint8_t desc45n_get_line_offset(const uint8_t *p_desc_n, uint8_t idx)
{
    return p_desc_n[idx + 2] & 0x1f;
}

static inline void desc45n_set_line_offset(uint8_t *p_desc_n, uint8_t idx, uint8_t i_line_offset)
{
    p_desc_n[idx + 2] = 0xc0 | (p_desc_n[idx + 2] & 0x20) | (i_line_offset & 0x1f);
}

static inline uint8_t *desc45_get_data(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC45_HEADER_SIZE;
    uint8_t i_desc_size = desc_get_length(p_desc);

    while (n) {
        if (p_desc_n + DESC45_DATA_SIZE - p_desc > i_desc_size)
            return NULL;
        p_desc_n += DESC45_DATA_SIZE + desc45n_get_data_length(p_desc_n);
        n--;
    }
    if (p_desc_n - p_desc > i_desc_size)
        return NULL;
    return p_desc_n;
}

static inline bool desc45_validate(const uint8_t *p_desc)
{
    const uint8_t *p_desc_n = p_desc + DESC45_HEADER_SIZE;
    int i_desc_size = desc_get_length(p_desc);
    while (i_desc_size > 0)
    {
        uint8_t i_data_sz = DESC45_DATA_SIZE + desc45n_get_data_length(p_desc_n);
        i_desc_size -= i_data_sz;
        p_desc_n    += i_data_sz;
    }
    return i_desc_size == 0;
}

static inline void desc45_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    if (i_print_type == PRINT_XML)
        pf_print(opaque, "<VBI_DATA_DESC>");

    while ((p_desc_n = desc45_get_data(p_desc, j++)) != NULL) {
        uint8_t k;
        uint8_t i_service_id  = desc45n_get_service_id(p_desc_n);
        uint8_t i_service_len = desc45n_get_data_length(p_desc_n);
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque,
                "<VBI_DATA_SERVICE service_id=\"0x%02x\" length=\"%u\" service_txt=\"%s\">",
                i_service_id, i_service_len, desc45_get_service_id_txt(i_service_id)
            );
            for (k = 0; k < i_service_len; k++) {
                if (i_service_id >= 1 && i_service_id <= 7 && i_service_id != 3)
                {
                    pf_print(opaque,
                        "<VBI_DATA field_parity=\"%u\" line_offset=\"%u\"/>",
                        desc45n_get_field_parity(p_desc_n, k),
                        desc45n_get_line_offset(p_desc_n, k));
                } else {
                    pf_print(opaque,
                        "<VBI_DATA reserved_byte=\"0x%02x\"/>",
                        desc45n_get_byte(p_desc_n, k));
                }
            }
            pf_print(opaque, "</VBI_DATA_SERVICE>");
            break;
        default:
            pf_print(opaque,
                "    - desc 45 vbi_data service_id=0x%02x length=%u service_txt=\"%s\"",
                     i_service_id, i_service_len, desc45_get_service_id_txt(i_service_id));
            for (k = 0; k < i_service_len; k++) {
                if (i_service_id >= 1 && i_service_id <= 7 && i_service_id != 3)
                {
                    pf_print(opaque,
                        "        - vbi_data field_parity=%u line_offset=%u",
                        desc45n_get_field_parity(p_desc_n, k),
                        desc45n_get_line_offset(p_desc_n, k));
                } else {
                    pf_print(opaque,
                        "        - vbi_data reserved_byte=0x%02x",
                        desc45n_get_byte(p_desc_n, k));
                }
            }
        }
    }

    if (i_print_type == PRINT_XML)
        pf_print(opaque, "</VBI_DATA_DESC>");
}

#ifdef __cplusplus
}
#endif

#endif
