/*****************************************************************************
 * desc_62.h: ETSI EN 300 468 Descriptor 0x62: Frequency list descriptor
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

#ifndef __BITSTREAM_DVB_DESC_62_H__
#define __BITSTREAM_DVB_DESC_62_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x62: Frequency list descriptor
 *****************************************************************************/
#define DESC62_HEADER_SIZE      (DESC_HEADER_SIZE + 1)
#define DESC62_FREQ_SIZE        4

static inline void desc62_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x62);
    desc_set_length(p_desc, DESC62_HEADER_SIZE - DESC_HEADER_SIZE);
    p_desc[2] = 0xfc;
}

static inline uint8_t desc62_get_coding_type(const uint8_t *p_desc)
{
    return p_desc[2] & 0x03;
}

static inline void desc62_set_coding_type(uint8_t *p_desc, uint8_t b_coding_type)
{
    p_desc[2] = (p_desc[2] & 0xfc) | (b_coding_type & 0x03);
}

static inline const char *desc62_get_coding_type_txt(uint8_t i_coding_type)
{
    return i_coding_type == 0 ? "not_defined" :
           i_coding_type == 1 ? "satellite" :
           i_coding_type == 2 ? "cable" :
           i_coding_type == 3 ? "terrestrial" : "reserved";
}

static inline uint32_t desc62n_get_freq(const uint8_t *p_desc_n)
{
    return (p_desc_n[0] << 24) | (p_desc_n[1] << 16) | (p_desc_n[2] << 8) | p_desc_n[3];
}

static inline void desc62n_set_freq(uint8_t *p_desc_n, uint32_t i_freq)
{
    p_desc_n[0] = (i_freq >> 24) & 0xff;
    p_desc_n[1] = (i_freq >> 16) & 0xff;
    p_desc_n[2] = (i_freq >>  8) & 0xff;
    p_desc_n[3] =  i_freq        & 0xff;
}

static inline uint8_t *desc62_get_frequency(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC62_HEADER_SIZE + n * DESC62_FREQ_SIZE;
    if (p_desc_n + DESC62_FREQ_SIZE - p_desc
         > desc_get_length(p_desc) + DESC62_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

static inline bool desc62_validate(const uint8_t *p_desc)
{
    return ((desc_get_length(p_desc) - 1) % DESC62_FREQ_SIZE) == 0;
}

static inline void desc62_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc62_get_frequency(p_desc, j)) != NULL) {
        j++;
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque,
                "<FREQUENCY_LIST_DESC coding_type=\"%u\" coding_type_txt=\"%s\" frequency=\"%x\"/>",
                desc62_get_coding_type(p_desc),
                desc62_get_coding_type_txt(desc62_get_coding_type(p_desc)),
                desc62n_get_freq(p_desc_n)
            );
            break;
        default:
            pf_print(opaque,
                "    - desc 62 frequency_list coding_type=%u coding_type_txt=%s frequency=%x",
                desc62_get_coding_type(p_desc),
                desc62_get_coding_type_txt(desc62_get_coding_type(p_desc)),
                desc62n_get_freq(p_desc_n)
            );
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
