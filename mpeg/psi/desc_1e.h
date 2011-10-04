/*****************************************************************************
 * desc_1e.h: ISO/IEC 13818-1 Descriptor 0x1e (SL descriptor)
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
 *  - ISO/IEC 13818-1:2007(E) (MPEG-2 Systems)
 */

#ifndef __BITSTREAM_MPEG_DESC_1e_H__
#define __BITSTREAM_MPEG_DESC_1e_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x1e (SL descriptor)
 *****************************************************************************/
#define DESC1E_HEADER_SIZE      (DESC_HEADER_SIZE + 2)

static inline void desc1e_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x1e);
    desc_set_length(p_desc, DESC1E_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline bool desc1e_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC1E_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline uint16_t desc1e_get_es_id(const uint8_t *p_desc) {
    return (p_desc[2] << 8) | p_desc[3];
}

static inline void desc1e_set_es_id(uint8_t *p_desc, uint16_t i_es_id) {
    p_desc[2] = (i_es_id >> 8) & 0xff;
    p_desc[3] = i_es_id        & 0xff;
}

static inline void desc1e_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<SL_DESC es_id=\"0x%04x\"/>",
                 desc1e_get_es_id(p_desc));
        break;
    default:
        pf_print(opaque,"    - desc 1e sl es_id=0x%04x",
                 desc1e_get_es_id(p_desc));
    }
}

#ifdef __cplusplus
}
#endif

#endif
