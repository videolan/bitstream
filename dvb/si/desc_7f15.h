/*****************************************************************************
 * desc_7f15.h: ETSI EN 300 468 Extension descriptor 0x15: AC-4 descriptor
 *****************************************************************************
 * Copyright (C) 2022 EasyTools
 *
 * Authors: Arnaud de Turckheim <adeturckheim@easytools.tv>
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
 *  - ETSI EN 300 468 V1.12.1 (2011-06) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_DESC_7F15_H__
#define __BITSTREAM_DVB_DESC_7F15_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/dvb/si/desc_7f.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Extension descriptor 0x06: Supplementary audio descriptor
 *****************************************************************************/
#define DESC7F15_HEADER_SIZE      (DESC7F_HEADER_SIZE + 1)

static inline void desc7f15_init(uint8_t *p_desc)
{
    desc7f_init(p_desc);
    desc_set_length(p_desc, (DESC7F15_HEADER_SIZE - DESC_HEADER_SIZE));
    desc7f_set_tag_extension(p_desc, DESC7F_TAG_AC4);
}

BITSTREAM_GET_SET(desc7f15, ac4_config_flag, uint8, 24, 1);
BITSTREAM_GET_SET(desc7f15, ac4_toc_flag, uint8, 25, 1);
BITSTREAM_GET_SET(desc7f15_opt, ac4_dialog_enhancement_enabled, uint8, 32, 1);
BITSTREAM_GET_SET(desc7f15_opt, ac4_channel_mode, uint8, 33, 2);

static inline uint8_t
desc7f15_get_ac4_dialog_enhancement_enabled(const uint8_t *p_desc)
{
    return desc7f15_get_ac4_config_flag(p_desc) ?
        desc7f15_opt_get_ac4_dialog_enhancement_enabled(p_desc) : 0;
}

#define DESC7F15_AC4_CHANNEL_MODE_MONO      0x0
#define DESC7F15_AC4_CHANNEL_MODE_STEREO    0x1
#define DESC7F15_AC4_CHANNEL_MODE_MULTI     0x2

static inline const char *desc7f15_ac4_channel_mode_str(uint8_t mode)
{
    switch (mode) {
        case DESC7F15_AC4_CHANNEL_MODE_MONO: return "mono";
        case DESC7F15_AC4_CHANNEL_MODE_STEREO: return "stereo";
        case DESC7F15_AC4_CHANNEL_MODE_MULTI: return "multichannel";
    }
    return "unknown";
}

static inline uint8_t desc7f15_get_ac4_channel_mode(const uint8_t *p_desc)
{
    return desc7f15_get_ac4_config_flag(p_desc) ?
        desc7f15_opt_get_ac4_channel_mode(p_desc) : 0;
}

static inline uint8_t desc7f15_get_ac4_toc_len(const uint8_t *p_desc)
{
    const uint8_t *p = p_desc + DESC7F15_HEADER_SIZE;
    if (!desc7f15_get_ac4_toc_flag(p_desc))
        return 0;
    if (desc7f15_get_ac4_config_flag(p_desc))
        p++;
    return *p;
}

static inline bool desc7f15_validate(const uint8_t *p_desc)
{
    uint8_t size = DESC7F15_HEADER_SIZE - DESC_HEADER_SIZE;
    if (desc_get_length(p_desc) < size)
        return false;
    if (desc7f15_get_ac4_config_flag(p_desc))
        size += 1;
    if (desc7f15_get_ac4_toc_flag(p_desc))
        size += 1;
    if (desc_get_length(p_desc) < size)
        return false;
    size += desc7f15_get_ac4_toc_len(p_desc);
    return desc_get_length(p_desc) >= size;
}

static inline void desc7f15_print(const uint8_t *p_desc, f_print pf_print,
                                  void *opaque, print_type_t i_print_type)
{
    uint8_t config = desc7f15_get_ac4_config_flag(p_desc);

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<AC4_DESC ac4_toc=\"%u\">",
                 desc7f15_get_ac4_toc_flag(p_desc));
        if (config)
            pf_print(opaque,
                     "<AC4_CONFIG"
                     " ac4_dialog_enhancement_enabled=\"%u\""
                     " ac4_channel_mode=\"%s\"/>",
                     desc7f15_get_ac4_dialog_enhancement_enabled(p_desc),
                     desc7f15_ac4_channel_mode_str(
                         desc7f15_get_ac4_channel_mode(p_desc))
                    );
        pf_print(opaque, "</AC4_DESC>");
        break;
    default:
        pf_print(opaque, "    - desc 7f15 ac-4%s",
                 desc7f15_get_ac4_toc_flag(p_desc) ?  " ac4_toc" : "");
        if (config)
            pf_print(opaque, "        - config%s ac4_channel_mode=\"%s\"",
                     desc7f15_get_ac4_dialog_enhancement_enabled(p_desc) ?
                     " ac4_dialog_enhancement_enabled" : "",
                     desc7f15_ac4_channel_mode_str(
                         desc7f15_get_ac4_channel_mode(p_desc)));
    }
}

#ifdef __cplusplus
}
#endif

#endif
