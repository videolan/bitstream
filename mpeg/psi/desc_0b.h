/*****************************************************************************
 * desc_0b.h: ISO/IEC 13818-1 Descriptor 0x0b (System clock descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_0B_H__
#define __BITSTREAM_MPEG_DESC_0B_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x0b: System clock descriptor
 *****************************************************************************/
#define DESC0B_HEADER_SIZE      (DESC_HEADER_SIZE + 2)

static inline void desc0b_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x0b);
    desc_set_length(p_desc, DESC0B_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline bool desc0b_get_external_clock_reference_indicator(const uint8_t *p_desc)
{
    return (p_desc[2] & 0x80) == 0x80;
}

static inline void desc0b_set_external_clock_reference_indicator(uint8_t *p_desc, bool b_external_clock_reference_indicator)
{
    p_desc[2] = (b_external_clock_reference_indicator ? (p_desc[2] | 0x80) : (p_desc[2] &~ 0x80)) | 0x40;
}

static inline uint8_t desc0b_get_clock_accuracy_integer(const uint8_t *p_desc)
{
    return p_desc[2] & 0x3f;
}

static inline void desc0b_set_clock_accuracy_integer(uint8_t *p_desc, uint8_t i_clock_accuracy_integer) {
    p_desc[2] = (p_desc[2] & 0x80) | 0x40 | (i_clock_accuracy_integer & 0x3f);
}

static inline uint8_t desc0b_get_clock_accuracy_exponent(const uint8_t *p_desc)
{
    return p_desc[3] >> 5;
}

static inline void desc0b_set_clock_accuracy_exponent(uint8_t *p_desc, uint8_t i_clock_accuracy_exponent)
{
    p_desc[3] = (i_clock_accuracy_exponent << 5) | 0x1F;
}

static inline bool desc0b_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC0B_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc0b_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<SYSTEM_CLOCK_DESC external_clock_reference_indicator=\"%u\" clock_accuracy_integer=\"%u\" clock_accuracy_exponent=\"%u\"/>",
            desc0b_get_external_clock_reference_indicator(p_desc),
            desc0b_get_clock_accuracy_integer(p_desc),
            desc0b_get_clock_accuracy_exponent(p_desc));
        break;
    default:
        pf_print(opaque, "    - desc 0b system_clock external_clock_reference_indicator=%u clock_accuracy_integer=%u clock_accuracy_exponent=%u",
            desc0b_get_external_clock_reference_indicator(p_desc),
            desc0b_get_clock_accuracy_integer(p_desc),
            desc0b_get_clock_accuracy_exponent(p_desc));
    }
}

#ifdef __cplusplus
}
#endif

#endif
