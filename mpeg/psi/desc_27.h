/*****************************************************************************
 * desc_27.h: ISO/IEC 13818-1 Descriptor 0x27 (Metadata STD descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_27_H__
#define __BITSTREAM_MPEG_DESC_27_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x27 (Metadata STD descriptor)
 *****************************************************************************/
#define DESC27_HEADER_SIZE      (DESC_HEADER_SIZE + 9)

static inline void desc27_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x27);
    desc_set_length(p_desc, DESC27_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline bool desc27_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC27_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline uint32_t desc27_get_input_leak_rate(const uint8_t *p_desc)
{
    return ((p_desc[2] & 0x3f) << 16) | (p_desc[3] << 8) | p_desc[4];
}

static inline void desc27_set_input_leak_rate(uint8_t *p_desc, uint32_t i_input_leak_rate)
{
    p_desc[2] = ((i_input_leak_rate >> 16) & 0xff) | 0xc0;
    p_desc[3] =  (i_input_leak_rate >>  8) & 0xff;
    p_desc[4] =   i_input_leak_rate        & 0xff;
}

static inline uint32_t desc27_get_buffer_size(const uint8_t *p_desc)
{
    return ((p_desc[5] & 0x3f) << 16) | (p_desc[6] << 8) | p_desc[7];
}

static inline void desc27_set_buffer_size(uint8_t *p_desc, uint32_t i_buffer_size)
{
    p_desc[5] = ((i_buffer_size >> 16) & 0xff) | 0xc0;
    p_desc[6] =  (i_buffer_size >>  8) & 0xff;
    p_desc[7] =   i_buffer_size        & 0xff;
}

static inline uint32_t desc27_get_output_leak_rate(const uint8_t *p_desc)
{
    return ((p_desc[8] & 0x3f) << 16) | (p_desc[9] << 8) | p_desc[10];
}

static inline void desc27_set_output_leak_rate(uint8_t *p_desc, uint32_t i_output_leak_rate)
{
    p_desc[ 8] = ((i_output_leak_rate >> 16) & 0xff) | 0xc0;
    p_desc[ 9] =  (i_output_leak_rate >>  8) & 0xff;
    p_desc[10] =   i_output_leak_rate        & 0xff;
}

static inline void desc27_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<METADATA_STD_DESC input_leak_rate=\"%u\" buffer_size=\"%u\" output_leak_rate=\"%u\"/>",
                 desc27_get_input_leak_rate(p_desc),
                 desc27_get_buffer_size(p_desc),
                 desc27_get_output_leak_rate(p_desc));
        break;
    default:
        pf_print(opaque,"    - desc 27 metadata_std input_leak_rate=%u buffer_size=%u output_leak_rate=%u",
                 desc27_get_input_leak_rate(p_desc),
                 desc27_get_buffer_size(p_desc),
                 desc27_get_output_leak_rate(p_desc));
    }
}

#ifdef __cplusplus
}
#endif

#endif
