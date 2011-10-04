/*****************************************************************************
 * desc_23.h: ISO/IEC 13818-1 Descriptor 0x23 (MultiplexBuffer descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_23_H__
#define __BITSTREAM_MPEG_DESC_23_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x23 (MultiplexBuffer descriptor)
 *****************************************************************************/
#define DESC23_HEADER_SIZE      (DESC_HEADER_SIZE + 6)

static inline void desc23_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x23);
    desc_set_length(p_desc, DESC23_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline bool desc23_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC23_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline uint32_t desc23_get_mb_buffer_size(const uint8_t *p_desc)
{
    return (p_desc[2] << 16) | (p_desc[3] << 8) | p_desc[4];
}

static inline void desc23_set_mb_buffer_size(uint8_t *p_desc, uint32_t i_mb_buffer_size)
{
    p_desc[2] = (i_mb_buffer_size >> 16) & 0xff;
    p_desc[3] = (i_mb_buffer_size >>  8) & 0xff;
    p_desc[4] = i_mb_buffer_size         & 0xff;
}

static inline uint32_t desc23_get_tb_leak_rate(const uint8_t *p_desc)
{
    return (p_desc[5] << 16) | (p_desc[6] << 8) | p_desc[7];
}

static inline void desc23_set_tb_leak_rate(uint8_t *p_desc, uint32_t i_tb_leak_rate)
{
    p_desc[5] = (i_tb_leak_rate >> 16) & 0xff;
    p_desc[6] = (i_tb_leak_rate >>  8) & 0xff;
    p_desc[7] = i_tb_leak_rate         & 0xff;
}

static inline void desc23_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<MULTIPLEX_BUFFER_DESC mb_buffer_size=\"%u\" tb_leak_rate=\"%u\"/>",
                 desc23_get_mb_buffer_size(p_desc),
                 desc23_get_tb_leak_rate(p_desc));
        break;
    default:
        pf_print(opaque,"    - desc 23 multiplex_buffer mb_buffer_size=%u tb_leak_rate=%u",
                 desc23_get_mb_buffer_size(p_desc),
                 desc23_get_tb_leak_rate(p_desc));
    }
}

#ifdef __cplusplus
}
#endif

#endif
