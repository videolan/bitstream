/*****************************************************************************
 * desc_63.h: ETSI EN 300 468 Descriptor 0x63: Partial Transport Stream
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
 *  - ETSI EN 300 468 V1.11.1 (2010-04) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_DESC_63_H__
#define __BITSTREAM_DVB_DESC_63_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x63: Partial Transport Stream
 *****************************************************************************/
#define DESC63_HEADER_SIZE      (DESC_HEADER_SIZE + 8)

static inline void desc63_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x63);
    desc_set_length(p_desc, DESC63_HEADER_SIZE - DESC_HEADER_SIZE);
    p_desc[2] = 0xc0;
    p_desc[5] = 0xc0;
    p_desc[8] = 0xc0;
}

static inline uint32_t desc63_get_peak_rate(const uint8_t *p_desc)
{
    return ((p_desc[2] & 0x3f) << 16) | (p_desc[3] << 8) | p_desc[4];
}

static inline void desc63_set_peak_rate(uint8_t *p_desc, uint32_t i_peak_rate)
{
    p_desc[2] = ((i_peak_rate >> 16) & 0xff) | 0xc0;
    p_desc[3] =  (i_peak_rate >>  8) & 0xff;
    p_desc[4] =   i_peak_rate        & 0xff;
}

static inline uint32_t desc63_get_min_overall_smoothing_rate(const uint8_t *p_desc)
{
    return ((p_desc[5] & 0x3f) << 16) | (p_desc[6] << 8) | p_desc[7];
}

static inline void desc63_set_min_overall_smoothing_rate(uint8_t *p_desc, uint32_t i_min_overall_smoothing_rate)
{
    p_desc[5] = ((i_min_overall_smoothing_rate >> 16) & 0xff) | 0xc0;
    p_desc[6] =  (i_min_overall_smoothing_rate >>  8) & 0xff;
    p_desc[7] =   i_min_overall_smoothing_rate        & 0xff;
}

static inline uint32_t desc63_get_max_overall_smoothing_buffer(const uint8_t *p_desc)
{
    return ((p_desc[8] & 0x3f) << 8) | p_desc[9];
}

static inline void desc63_set_max_overall_smoothing_buffer(uint8_t *p_desc, uint32_t i_max_overall_smoothing_buffer)
{
    p_desc[8] = ((i_max_overall_smoothing_buffer >> 8) & 0xff) | 0xc0;
    p_desc[9] =   i_max_overall_smoothing_buffer       & 0xff;
}

static inline bool desc63_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC63_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc63_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 "<PARTIAL_TRANSPORT_STREAM_DESC peak_rate=\"%u\""
                 " min_overall_smoothing_rate=\"%u\""
                 " max_overall_smoothing_buffer=\"%u\"/>",
                 desc63_get_peak_rate(p_desc),
                 desc63_get_min_overall_smoothing_rate(p_desc),
                 desc63_get_max_overall_smoothing_buffer(p_desc)
                );
        break;
    default:
        pf_print(opaque,
                 "    - desc 63 partial_transport_stream peak_rate=%u"
                 " min_overall_smoothing_rate=%u"
                 " max_overall_smoothing_buffer=%u",
                 desc63_get_peak_rate(p_desc),
                 desc63_get_min_overall_smoothing_rate(p_desc),
                 desc63_get_max_overall_smoothing_buffer(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
