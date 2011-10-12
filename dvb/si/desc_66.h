/*****************************************************************************
 * desc_66.h: ETSI EN 300 468 Descriptor 0x66: Data broadcast id descriptor
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

#ifndef __BITSTREAM_DVB_DESC_66_H__
#define __BITSTREAM_DVB_DESC_66_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x66: Data broadcast id descriptor
 *****************************************************************************/
#define DESC66_HEADER_SIZE      (DESC_HEADER_SIZE + 2)

static inline void desc66_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x66);
    desc_set_length(p_desc, DESC66_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline uint16_t desc66_get_broadcast_id(const uint8_t *p_desc)
{
    return (p_desc[2] << 8) | p_desc[3];
}

static inline void desc66_set_broadcast_id(uint8_t *p_desc, uint16_t i_broadcast_id)
{
    p_desc[2] = (i_broadcast_id >> 8) & 0xff;
    p_desc[3] =  i_broadcast_id       & 0xff;
}

static inline uint8_t desc66_get_selector_byte_length(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) - (DESC66_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline const uint8_t *desc66_get_selector_byte(const uint8_t *p_desc, uint8_t *pi_length)
{
    *pi_length = desc66_get_selector_byte_length(p_desc);
    return p_desc + 4;
}

static inline void desc66_set_selector_byte(uint8_t *p_desc, const uint8_t *p_selector_byte, uint8_t i_length)
{
    desc_set_length(p_desc, i_length + (DESC66_HEADER_SIZE - DESC_HEADER_SIZE));
    memcpy(p_desc + 4, p_selector_byte, i_length);
}

static inline bool desc66_validate(const uint8_t *p_desc)
{
    int i_length = desc_get_length(p_desc) - (DESC66_HEADER_SIZE - DESC_HEADER_SIZE);
    i_length -= desc66_get_selector_byte_length(p_desc);
    return i_length == 0;
}

static inline void desc66_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t i_selector_byte_length, i;
    const uint8_t *p_selector_byte = desc66_get_selector_byte(p_desc, &i_selector_byte_length);
    char psz_selector_byte[2 * 255 + 1];
    char psz_selector_byte_txt[255 + 1];

    for (i = 0; i < i_selector_byte_length; i++) {
        sprintf(psz_selector_byte + 2 * i, "%02x", p_selector_byte[i]);
        if (p_selector_byte[i] >= 32 && p_selector_byte[i] <= 127 && p_selector_byte[i] != '"')
            psz_selector_byte_txt[i] = p_selector_byte[i];
        else
            psz_selector_byte_txt[i] = '.';
    }
    psz_selector_byte[2 * i] = '\0';
    psz_selector_byte_txt[i] = '\0';

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 "<DATA_BROADCAST_ID_DESC broadcast_id=\"0x%04x\""
                 " selector_byte=\"%s\" selector_byte_txt=\"%s\"/>",
                 desc66_get_broadcast_id(p_desc),
                 psz_selector_byte,
                 psz_selector_byte_txt
                );
        break;
    default:
        pf_print(opaque,
                 "    - desc 66 data_broadcast broadcast_id=0x%04x"
                 " selector_byte=\"%s\" selector_byte_txt=\"%s\"",
                 desc66_get_broadcast_id(p_desc),
                 psz_selector_byte,
                 psz_selector_byte_txt
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
