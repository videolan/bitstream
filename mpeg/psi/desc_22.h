/*****************************************************************************
 * desc_22.h: ISO/IEC 13818-1 Descriptor 0x22 (FmxBufferSize descriptor)
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
 *  - ISO/IEC 14496-1:2001(E) (MPEG-4 Systems)
 */

#ifndef __BITSTREAM_MPEG_DESC_22_H__
#define __BITSTREAM_MPEG_DESC_22_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x22: FmxBufferSize descriptor
 *****************************************************************************/
#define DESC22_HEADER_SIZE      (DESC_HEADER_SIZE + 3)
#define DESC22_ENTRY_SIZE       4

static inline void desc22_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x22);
    desc_set_length(p_desc, DESC22_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline uint32_t desc22_get_default_buffer_size(const uint8_t *p_desc)
{
    return (p_desc[2] << 16) | (p_desc[3] << 8) | p_desc[4];
}

static inline void desc22_set_default_buffer_size(uint8_t *p_desc, uint32_t i_default_buffer_size)
{
    p_desc[2] = (i_default_buffer_size >> 16) & 0xff;
    p_desc[3] = (i_default_buffer_size >>  8) & 0xff;
    p_desc[4] =  i_default_buffer_size        & 0xff;
}

static inline uint8_t *desc22_get_entry(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC22_HEADER_SIZE + n * DESC22_ENTRY_SIZE;
    if (p_desc_n + DESC22_ENTRY_SIZE - p_desc
         > desc_get_length(p_desc) + DESC22_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

static inline uint8_t desc22n_get_flexmux_channel(const uint8_t *p_desc_n)
{
    return p_desc_n[0];
}

static inline void desc22n_set_flexmux_channel(uint8_t *p_desc_n, uint8_t i_flexmux_channel)
{
    p_desc_n[0] = i_flexmux_channel;
}

static inline uint32_t desc22n_get_flexmux_buffer_size(const uint8_t *p_desc_n)
{
    return (p_desc_n[1] << 16) | (p_desc_n[2] << 8) | p_desc_n[3];
}

static inline void desc22n_set_flexmux_buffer_size(uint8_t *p_desc_n, uint32_t i_buffer_size)
{
    p_desc_n[1] = (i_buffer_size >> 16) & 0xff;
    p_desc_n[2] = (i_buffer_size >>  8) & 0xff;
    p_desc_n[3] =  i_buffer_size        & 0xff;
}


static inline bool desc22_validate(const uint8_t *p_desc)
{
    return ((desc_get_length(p_desc) - (DESC22_HEADER_SIZE - DESC_HEADER_SIZE))
            % DESC22_ENTRY_SIZE) == 0;
}

static inline void desc22_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t n = 0;
    uint8_t *p_desc_n;

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<FMX_BUFFER_SIZE_DESC default_buffer_size=\"%u\">",
                 desc22_get_default_buffer_size(p_desc));
        break;
    default:
        pf_print(opaque, "    - desc 22 fmx_buffer_size default_buffer_size=%u",
                 desc22_get_default_buffer_size(p_desc));
    }

    while ((p_desc_n = desc22_get_entry(p_desc, n++)) != NULL) {
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque, "<FLEXMUX_BUFFER_DESC flexmux_channel=\"0x%02x\" buffer_size=\"%u\"/>",
                     desc22n_get_flexmux_channel(p_desc_n),
                     desc22n_get_flexmux_buffer_size(p_desc_n));
            break;
        default:
            pf_print(opaque, "        - flexmux_channel=0x%02x buffer_size=%u",
                     desc22n_get_flexmux_channel(p_desc_n),
                     desc22n_get_flexmux_buffer_size(p_desc_n));
        }
    }

    if (i_print_type == PRINT_XML)
        pf_print(opaque, "</FMX_BUFFER_SIZE_DESC>");
}

#ifdef __cplusplus
}
#endif

#endif
