/*****************************************************************************
 * desc_1d.h: ISO/IEC 13818-1 Descriptor 0x1d (IOD descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_1D_H__
#define __BITSTREAM_MPEG_DESC_1D_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x1d (IOD descriptor)
 *****************************************************************************/
#define DESC1D_HEADER_SIZE      (DESC_HEADER_SIZE + 3)

static inline void desc1d_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x1d);
    desc_set_length(p_desc, DESC1D_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline bool desc1d_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC1D_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline uint8_t desc1d_get_scope(const uint8_t *p_desc) {
    return p_desc[2];
}

static inline void desc1d_set_scope(uint8_t *p_desc, uint8_t i_scope) {
    p_desc[2] = i_scope;
}

static inline uint8_t desc1d_get_iod_label(const uint8_t *p_desc) {
    return p_desc[3];
}

static inline void desc1d_set_iod_label(uint8_t *p_desc, uint8_t i_iod_label) {
    p_desc[3] = i_iod_label;
}

static inline uint8_t desc1d_get_iod(const uint8_t *p_desc) {
    return p_desc[4];
}

static inline void desc1d_set_iod(uint8_t *p_desc, uint8_t i_iod) {
    p_desc[4] = i_iod;
}

static inline void desc1d_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<IOD_DESC scope=\"0x%02x\" iod_label=\"0x%02x\" iod=\"0x%02x\"/>",
                 desc1d_get_scope(p_desc),
                 desc1d_get_iod_label(p_desc),
                 desc1d_get_iod(p_desc));
        break;
    default:
        pf_print(opaque,"    - desc 1d iod scope=0x%02x iod_label=0x%02x iod=0x%02x",
                 desc1d_get_scope(p_desc),
                 desc1d_get_iod_label(p_desc),
                 desc1d_get_iod(p_desc));
    }
}

#ifdef __cplusplus
}
#endif

#endif
