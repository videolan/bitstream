/*****************************************************************************
 * desc_0d.h: ISO/IEC 13818-1 Descriptor 0x0d (Copyright descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_0D_H__
#define __BITSTREAM_MPEG_DESC_0D_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x0d: Copyright descriptor
 *****************************************************************************/
#define DESC0D_HEADER_SIZE      (DESC_HEADER_SIZE + 4)

static inline void desc0d_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x0d);
    desc_set_length(p_desc, DESC0D_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline uint32_t desc0d_get_copyright_identifier(const uint8_t *p_desc)
{
    return (p_desc[2] << 24) | (p_desc[3] << 16) | (p_desc[4] << 8) | p_desc[5];
}

static inline void desc0d_set_copyright_identifier(uint8_t *p_desc, uint32_t i_copyright_identifier)
{
    p_desc[2] = (i_copyright_identifier >> 24) & 0xff;
    p_desc[3] = (i_copyright_identifier >> 16) & 0xff;
    p_desc[4] = (i_copyright_identifier >>  8) & 0xff;
    p_desc[5] =  i_copyright_identifier        & 0xff;
}

static inline const uint8_t *desc0d_get_additional_copyright_info(const uint8_t *p_desc, uint8_t *i_length)
{
    *i_length = desc_get_length(p_desc) - (DESC0D_HEADER_SIZE - DESC_HEADER_SIZE);
    if (*i_length)
        return &p_desc[DESC0D_HEADER_SIZE];
    return NULL;
}

static inline void desc0d_set_additional_copyright_info(uint8_t *p_desc, uint8_t *p_additional_copyright_info, uint8_t i_length)
{
    if (i_length == 0 || !p_additional_copyright_info)
        desc_set_length(p_desc, DESC0D_HEADER_SIZE - DESC_HEADER_SIZE);
    else if (i_length < 252)
    {
        desc_set_length(p_desc, (DESC0D_HEADER_SIZE - DESC_HEADER_SIZE) + i_length);
        memcpy(p_desc + DESC0D_HEADER_SIZE, p_additional_copyright_info, i_length);
    }
}

static inline bool desc0d_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC0D_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc0d_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t i, i_length;
    const uint8_t *p_add_copy_info = desc0d_get_additional_copyright_info(p_desc, &i_length);
    char psz_value[2 * 255 + 1];

    for (i = 0; i < i_length; i++)
        sprintf(psz_value + 2 * i, "%2.2hhx", p_add_copy_info[i]);
    psz_value[2 * i] = '\0';

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<COPYRIGHT_DESC identifier=\"0x%08x\" additional_info_len=\"%u\" additional_info=\"%s\"/>",
                 desc0d_get_copyright_identifier(p_desc),
                 i_length, psz_value);
        break;
    default:
        pf_print(opaque, "    - desc 0d copyright identifier=0x%08x additional_info_len=%u additional_info=%s",
                 desc0d_get_copyright_identifier(p_desc),
                 i_length, psz_value);
    }
}

#ifdef __cplusplus
}
#endif

#endif
