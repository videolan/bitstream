/*****************************************************************************
 * desc_53.h: ETSI EN 300 468 Descriptor 0x53: CA identifier descriptor
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

#ifndef __BITSTREAM_DVB_DESC_53_H__
#define __BITSTREAM_DVB_DESC_53_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x53: CA identifier descriptor
 *****************************************************************************/
#define DESC53_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC53_CA_SIZE          2

static inline void desc53_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x53);
}

static inline void desc53n_set_ca_sysid(uint8_t *p_desc_n, uint16_t i_ca_sysid)
{
    p_desc_n[0] = i_ca_sysid >> 8;
    p_desc_n[1] = i_ca_sysid & 0xff;
}

static inline uint16_t desc53n_get_ca_sysid(const uint8_t *p_desc_n)
{
    return (p_desc_n[0] << 8) | p_desc_n[1];
}

static inline uint8_t *desc53_get_ca(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC53_HEADER_SIZE + n * DESC53_CA_SIZE;
    if (p_desc_n + DESC53_CA_SIZE - p_desc
         > desc_get_length(p_desc) + DESC53_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

static inline bool desc53_validate(const uint8_t *p_desc)
{
    return (desc_get_length(p_desc) % DESC53_CA_SIZE) == 0;
}

static inline void desc53_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc53_get_ca(p_desc, j)) != NULL) {
        j++;
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque, "<CA_IDENTIFIER_DESC ca_sysid=\"0x%04x\"/>",
                     desc53n_get_ca_sysid(p_desc_n));
            break;
        default:
            pf_print(opaque,"    - desc 53 ca_identifier ca_sysid=0x%04x",
                     desc53n_get_ca_sysid(p_desc_n));
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
