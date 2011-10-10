/*****************************************************************************
 * desc_4a.h: ETSI EN 300 468 Descriptor 0x4a: Linkage descriptor (partially implemented)
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
 */

#ifndef __BITSTREAM_DVB_DESC_4A_H__
#define __BITSTREAM_DVB_DESC_4A_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x4a: Linkage descriptor (partially implemented)
 *****************************************************************************/
#define DESC4A_HEADER_SIZE      (DESC_HEADER_SIZE + 7)

static inline uint16_t desc4a_get_tsid(const uint8_t *p_desc)
{
    return (p_desc[2] << 8) | p_desc[3];
}

static inline uint16_t desc4a_get_onid(const uint8_t *p_desc)
{
    return (p_desc[4] << 8) | p_desc[5];
}

static inline uint16_t desc4a_get_sid(const uint8_t *p_desc)
{
    return (p_desc[6] << 8) | p_desc[7];
}

static inline uint8_t desc4a_get_linkage(const uint8_t *p_desc)
{
    return p_desc[8];
}

static inline bool desc4a_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC4A_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc4a_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
             "<LINKAGE_DESC tsid=\"%hu\" onid=\"%hu\" sid=\"%hu\" linkage=\"%hhu\" />",
             desc4a_get_tsid(p_desc), desc4a_get_onid(p_desc),
             desc4a_get_sid(p_desc), desc4a_get_linkage(p_desc));
        break;
    default:
        pf_print(opaque,
             "    - desc 4a linkage tsid=%hu onid=%hu sid=%hu linkage=%hhu",
             desc4a_get_tsid(p_desc), desc4a_get_onid(p_desc),
             desc4a_get_sid(p_desc), desc4a_get_linkage(p_desc));
    }
}

#ifdef __cplusplus
}
#endif

#endif
