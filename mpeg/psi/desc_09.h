/*****************************************************************************
 * desc_09.h: ISO/IEC 13818-1 Descriptor 0x09 (Conditional access descriptor)
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *          Georgi Chorbadzhiyski <georgi@unixsol.org>
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

#ifndef __BITSTREAM_MPEG_DESC_09_H__
#define __BITSTREAM_MPEG_DESC_09_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x09: Conditional access descriptor
 *****************************************************************************/
#define DESC09_HEADER_SIZE      (DESC_HEADER_SIZE + 4)

static inline void desc09_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x09);
    desc_set_length(p_desc, DESC09_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline uint16_t desc09_get_sysid(const uint8_t *p_desc)
{
    return (p_desc[2] << 8) | p_desc[3];
}

static inline void desc09_set_sysid(uint8_t *p_desc, uint16_t i_sysid)
{
    p_desc[2] = i_sysid >> 8;
    p_desc[3] = i_sysid & 0xff;
}

static inline uint16_t desc09_get_pid(const uint8_t *p_desc)
{
    return ((p_desc[4] & 0x1f) << 8) | p_desc[5];
}

static inline void desc09_set_pid(uint8_t *p_desc, uint16_t i_pid)
{
    p_desc[4] = ((i_pid >> 8) & 0xff) | 0xE0;
    p_desc[5] = i_pid & 0xff;
}

static inline bool desc09_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC09_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc09_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<CA_DESC sysid=\"0x%hx\" pid=\"%hu\"/>",
                 desc09_get_sysid(p_desc), desc09_get_pid(p_desc));
        break;
    default:
        pf_print(opaque, "    - desc 09 ca sysid=0x%hx pid=%hu",
                 desc09_get_sysid(p_desc), desc09_get_pid(p_desc));
    }
}

#ifdef __cplusplus
}
#endif

#endif
