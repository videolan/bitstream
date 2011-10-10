/*****************************************************************************
 * desc_41.h: ETSI EN 300 468 Descriptor 0x41: Service list descriptor
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

#ifndef __BITSTREAM_DVB_DESC_41_H__
#define __BITSTREAM_DVB_DESC_41_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x41: Service list descriptor
 *****************************************************************************/
#define DESC41_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC41_SERVICE_SIZE     3

static inline void desc41_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x41);
}

static inline uint8_t *desc41_get_service(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC41_HEADER_SIZE + n * DESC41_SERVICE_SIZE;
    if (p_desc_n + DESC41_SERVICE_SIZE - p_desc
         > desc_get_length(p_desc) + DESC41_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

static inline uint16_t desc41n_get_sid(const uint8_t *p_desc_n)
{
    return (p_desc_n[0] << 8) | p_desc_n[1];
}

static inline void desc41n_set_sid(uint8_t *p_desc_n, uint16_t i_sid)
{
    p_desc_n[0] = i_sid >> 8;
    p_desc_n[1] = i_sid & 0xff;
}

static inline uint8_t desc41n_get_type(const uint8_t *p_desc_n)
{
    return p_desc_n[2];
}

static inline void desc41n_set_type(uint8_t *p_desc_n, uint8_t i_type)
{
    p_desc_n[2] = i_type;
}

static inline bool desc41_validate(const uint8_t *p_desc)
{
    return !(desc_get_length(p_desc) % DESC41_SERVICE_SIZE);
}

static inline void desc41_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc41_get_service(p_desc, j)) != NULL) {
        j++;
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque,
            "<SERVICE_LIST_DESC sid=\"%hu\" type=\"0x%02x\" />",
            desc41n_get_sid(p_desc_n), desc41n_get_type(p_desc_n));
            break;
        default:
            pf_print(opaque,
            "    - desc 41 service_list sid=%hu type=0x%02x",
            desc41n_get_sid(p_desc_n), desc41n_get_type(p_desc_n));
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
