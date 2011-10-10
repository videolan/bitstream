/*****************************************************************************
 * desc_05.h: ISO/IEC 13818-1 Descriptor 0x05 (Registration descriptor)
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
 *  - ISO/IEC 13818-1:2007(E) (MPEG-2 Systems)
 */

#ifndef __BITSTREAM_MPEG_DESC_05_H__
#define __BITSTREAM_MPEG_DESC_05_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x05: Registration descriptor
 *****************************************************************************/
#define DESC05_HEADER_SIZE      (DESC_HEADER_SIZE + 4)

static inline void desc05_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x05);
    desc_set_length(p_desc, DESC05_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline void desc05_set_identifier(uint8_t *p_desc, uint8_t p_id[4])
{
    p_desc[2] = p_id[0];
    p_desc[3] = p_id[1];
    p_desc[4] = p_id[2];
    p_desc[5] = p_id[3];
}

static inline const uint8_t *desc05_get_identifier(const uint8_t *p_desc)
{
    return p_desc + 2;
}

static inline bool desc05_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC05_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc05_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<REGISTRATION_DESC identifier=\"%4.4s\"/>",
                 desc05_get_identifier(p_desc));
        break;
    default:
        pf_print(opaque, "    - desc 05 registration identifier=%4.4s",
                 desc05_get_identifier(p_desc));
    }
}

#ifdef __cplusplus
}
#endif

#endif
