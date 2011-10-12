/*****************************************************************************
 * desc_2c.h: ISO/IEC 13818-1 Descriptor 0x2c (FlexMuxTiming descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_2C_H__
#define __BITSTREAM_MPEG_DESC_2C_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x2c (FlexMuxTiming descriptor)
 *****************************************************************************/
#define DESC2C_HEADER_SIZE      (DESC_HEADER_SIZE + 8)

static inline void desc2c_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x2c);
    desc_set_length(p_desc, DESC2C_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline bool desc2c_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC2C_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline uint16_t desc2c_get_fcr_es_id(const uint8_t *p_desc)
{
    return (p_desc[2] << 8) | p_desc[3];
}

static inline void desc2c_set_fcr_es_id(uint8_t *p_desc, uint16_t i_fcr_es_id)
{
    p_desc[2] = (i_fcr_es_id >> 8) & 0xff;
    p_desc[3] =  i_fcr_es_id       & 0xff;
}

static inline uint32_t desc2c_get_fcr_resolution(const uint8_t *p_desc)
{
    return (p_desc[4] << 24) | (p_desc[5] << 16) | (p_desc[6] << 8) | p_desc[7];
}

static inline void desc2c_set_fcr_resolution(uint8_t *p_desc, uint32_t i_fcr_resolution)
{
    p_desc[4] = (i_fcr_resolution >> 24) & 0xff;
    p_desc[5] = (i_fcr_resolution >> 16) & 0xff;
    p_desc[6] = (i_fcr_resolution >>  8) & 0xff;
    p_desc[7] =  i_fcr_resolution        & 0xff;
}

static inline uint8_t desc2c_get_fcr_length(const uint8_t *p_desc)
{
    return p_desc[8];
}

static inline void desc2c_set_fcr_length(uint8_t *p_desc, uint8_t i_fcr_length)
{
    p_desc[8] = i_fcr_length;
}

static inline uint8_t desc2c_get_fmx_rate_length(const uint8_t *p_desc)
{
    return p_desc[9];
}

static inline void desc2c_set_fmx_rate_length(uint8_t *p_desc, uint8_t i_fmx_rate_length)
{
    p_desc[9] = i_fmx_rate_length;
}

static inline void desc2c_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<FLEXMUX_TIMING_DESC fcr_es_id=\"0x%04x\""
                " fcr_resolution=\"%u\" fcr_length=\"%u\" fmx_rate_length=\"%u\"/>",
                 desc2c_get_fcr_es_id(p_desc),
                 desc2c_get_fcr_resolution(p_desc),
                 desc2c_get_fcr_length(p_desc),
                 desc2c_get_fmx_rate_length(p_desc)
                );
        break;
    default:
        pf_print(opaque,"    - desc 2c flexmux_timing fcr_es_id=0x%04x"
                " fcr_resolution=%u fcr_length=%u fmx_rate_length=%u",
                 desc2c_get_fcr_es_id(p_desc),
                 desc2c_get_fcr_resolution(p_desc),
                 desc2c_get_fcr_length(p_desc),
                 desc2c_get_fmx_rate_length(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
