/*****************************************************************************
 * desc_65.h: ETSI EN 300 468 Descriptor 0x65: Scrambling descriptor
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

#ifndef __BITSTREAM_DVB_DESC_65_H__
#define __BITSTREAM_DVB_DESC_65_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x65: Scrambling descriptor
 *****************************************************************************/
#define DESC65_HEADER_SIZE      (DESC_HEADER_SIZE + 1)

static inline void desc65_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x65);
    desc_set_length(p_desc, DESC65_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline uint8_t desc65_get_scrambling_mode(const uint8_t *p_desc)
{
    return p_desc[2];
}

static inline void desc65_set_scrambling_mode(uint8_t *p_desc, uint8_t i_scrambling_mode)
{
    p_desc[2] = i_scrambling_mode;
}

static inline const char *desc65_get_scrambling_mode_txt(uint8_t i_mode)
{
    return i_mode == 0x00                   ? "Reserved" :
           i_mode == 0x01                   ? "DVB-CSA1" :
           i_mode == 0x02                   ? "DVB-CSA2" :
           i_mode == 0x03                   ? "DVB-CSA3 in standart mode" :
           i_mode == 0x04                   ? "DVB-CSA3 in minimally enhanced mode" :
           i_mode == 0x05                   ? "DVB-CSA3 in fully enhanced mode" :
           i_mode >= 0x06 && i_mode <= 0x6f ? "Reserved" :
           i_mode >= 0x70 && i_mode <= 0x7f ? "ATIS defined" :
           i_mode >= 0x80 && i_mode <= 0xfe ? "User defined" : "Reserved";
}

static inline bool desc65_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC65_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc65_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<SCRAMBLING_DESC scrambling_mode=\"0x%02x\" scrambling_mode_txt=\"%s\"/>",
                 desc65_get_scrambling_mode(p_desc),
                 desc65_get_scrambling_mode_txt(desc65_get_scrambling_mode(p_desc))
                );
        break;
    default:
        pf_print(opaque, "    - desc 65 scrambling scrambling_mode=0x%02x scrambling_mode_txt=\"%s\"",
                 desc65_get_scrambling_mode(p_desc),
                 desc65_get_scrambling_mode_txt(desc65_get_scrambling_mode(p_desc))
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
