/*****************************************************************************
 * desc_83p28.h: ETSI EN 300 468 Descriptor 0x83: Logical channel descriptor
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
 *  - IEC/CENELEC 62 216
 */

#ifndef __BITSTREAM_DVB_DESC_83P28_H__
#define __BITSTREAM_DVB_DESC_83P28_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x83: Logical channel descriptor (IEC/CENELEC 62 216)
 * Only valid if a private data specifier descriptor 28 is present before.
 *****************************************************************************/
#define DESC83P28_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC83P28_SERVICE_SIZE     4

static inline void desc83p28_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x83);
}

static inline uint8_t *desc83p28_get_service(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC83P28_HEADER_SIZE
                         + n * DESC83P28_SERVICE_SIZE;
    if (p_desc_n + DESC83P28_SERVICE_SIZE - p_desc
         > desc_get_length(p_desc) + DESC83P28_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

static inline uint16_t desc83p28n_get_sid(const uint8_t *p_desc_n)
{
    return (p_desc_n[0] << 8) | p_desc_n[1];
}

static inline bool desc83p28n_get_visible(const uint8_t *p_desc_n)
{
    return !!(p_desc_n[2] & 0x80);
}

static inline uint16_t desc83p28n_get_lcn(const uint8_t *p_desc_n)
{
    return ((p_desc_n[2] & 0x3) << 8) | p_desc_n[3];
}

static inline bool desc83p28_validate(const uint8_t *p_desc)
{
    return !(desc_get_length(p_desc) % DESC83P28_SERVICE_SIZE);
}

static inline void desc83p28_print(uint8_t *p_desc, f_print pf_print,
                                   void *opaque, print_type_t i_print_type)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc83p28_get_service(p_desc, j)) != NULL) {
        j++;
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque,
            "<%s sid=\"%hu\" visible=\"%d\" lcn=\"%hu\" />",
            desc_get_tag(p_desc) == 0x88 ? "HD_SIMULCAST_LCN_DESC" : "LCN_DESC",
            desc83p28n_get_sid(p_desc_n),
            desc83p28n_get_visible(p_desc_n) ? 1 : 0,
            desc83p28n_get_lcn(p_desc_n));
            break;
        default:
            pf_print(opaque,
            "    - desc %hhu lcn sid=%hu%s lcn=%hu", desc_get_tag(p_desc),
            desc83p28n_get_sid(p_desc_n),
            desc83p28n_get_visible(p_desc_n) ? " visible" : "",
            desc83p28n_get_lcn(p_desc_n));
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
