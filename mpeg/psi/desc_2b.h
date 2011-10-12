/*****************************************************************************
 * desc_2b.h: ISO/IEC 13818-1 Descriptor 0x2b (MPEG2 AAC audio descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_2B_H__
#define __BITSTREAM_MPEG_DESC_2B_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x2b (MPEG2 AAC audio descriptor)
 *****************************************************************************/
#define DESC2B_HEADER_SIZE      (DESC_HEADER_SIZE + 3)

static inline void desc2b_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x2b);
    desc_set_length(p_desc, DESC2B_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline bool desc2b_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC2B_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline uint8_t desc2b_get_aac_profile(const uint8_t *p_desc)
{
    return p_desc[2];
}

static inline void desc2b_set_aac_profile(uint8_t *p_desc, uint8_t i_profile)
{
    p_desc[2] = i_profile;
}

static inline uint8_t desc2b_get_aac_channel_config(const uint8_t *p_desc)
{
    return p_desc[3];
}

static inline void desc2b_set_aac_channel_config(uint8_t *p_desc, uint8_t i_channel_config)
{
    p_desc[3] = i_channel_config;
}

static inline uint8_t desc2b_get_aac_additional_info(const uint8_t *p_desc)
{
    return p_desc[4];
}

static inline void desc2b_set_aac_additional_info(uint8_t *p_desc, uint8_t i_additional_info)
{
    p_desc[4] = i_additional_info;
}

static inline void desc2b_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<MPEG2_AAC_AUDIO_DESC profile=\"0x%02x\""
                " channel_config=\"0x%02x\" additional_info=\"0x%02x\"/>",
                 desc2b_get_aac_profile(p_desc),
                 desc2b_get_aac_channel_config(p_desc),
                 desc2b_get_aac_additional_info(p_desc)
                );
        break;
    default:
        pf_print(opaque,"    - desc 2b mpeg2_aac_audio profile=0x%02x"
                " channel_config=0x%02x additional_info=0x%02x",
                 desc2b_get_aac_profile(p_desc),
                 desc2b_get_aac_channel_config(p_desc),
                 desc2b_get_aac_additional_info(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
