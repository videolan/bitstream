/*****************************************************************************
 * desc_1f.h: ISO/IEC 13818-1 Descriptor 0x1f (FMC descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_1F_H__
#define __BITSTREAM_MPEG_DESC_1F_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x1f: FMC descriptor
 *****************************************************************************/
#define DESC1F_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC1F_ENTRY_SIZE       3

static inline void desc1f_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x1f);
}

static inline uint8_t *desc1f_get_entry(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC1F_HEADER_SIZE + n * DESC1F_ENTRY_SIZE;
    if (p_desc_n + DESC1F_ENTRY_SIZE - p_desc
         > desc_get_length(p_desc) + DESC1F_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

static inline uint16_t desc1fn_get_es_id(const uint8_t *p_desc)
{
    return (p_desc[0] << 8) | p_desc[1];
}

static inline void desc1fn_set_es_id(uint8_t *p_desc_n, uint16_t i_es_id)
{
    p_desc_n[0] = (i_es_id >> 8) & 0xff;
    p_desc_n[1] = i_es_id        & 0xff;
}

static inline uint8_t desc1fn_get_flexmux_channel(const uint8_t *p_desc_n)
{
    return p_desc_n[2];
}

static inline void desc1fn_set_flexmux_channel(uint8_t *p_desc_n, uint8_t i_flexmux_channel)
{
    p_desc_n[2] = i_flexmux_channel;
}

static inline bool desc1f_validate(const uint8_t *p_desc)
{
    return !(desc_get_length(p_desc) % DESC1F_ENTRY_SIZE);
}

static inline void desc1f_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc1f_get_entry(p_desc, j)) != NULL) {
        j++;
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque, "<FMC_DESC es_id=\"0x%04x\" flexmux_channel=\"0x%02x\"/>",
                     desc1fn_get_es_id(p_desc_n),
                     desc1fn_get_flexmux_channel(p_desc_n));
            break;
        default:
            pf_print(opaque, "    - desc 1f fmc es_id=0x%04x flexmux_channel=0x%02x",
                     desc1fn_get_es_id(p_desc_n),
                     desc1fn_get_flexmux_channel(p_desc_n));
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
