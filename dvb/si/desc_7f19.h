/*****************************************************************************
 * desc_7f19.h: ETSI EN 300 468 Extension descriptor 0x19: Audio preselection
 * descriptor
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

#ifndef __BITSTREAM_DVB_DESC_7F19_H__
#define __BITSTREAM_DVB_DESC_7F19_H__

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
#define DESC7F19_HEADER_SIZE            (DESC7F_HEADER_SIZE + 1)
#define DESC7F19_PRESEL_SIZE            2
#define DESC7F19_PRESEL_LANG_SIZE       3
#define DESC7F19_PRESEL_TEXT_SIZE       1
#define DESC7F19_PRESEL_MSI_SIZE        1
#define DESC7F19_PRESEL_MSI_COMP_SIZE   1
#define DESC7F19_PRESEL_EXT_SIZE        1
#define DESC7F19_PRESEL_EXT_BYTE_SIZE   1

static inline void desc7f19_init(uint8_t *p_desc)
{
    desc7f_init(p_desc);
    desc_set_length(p_desc, (DESC7F19_HEADER_SIZE - DESC_HEADER_SIZE));
    desc7f_set_tag_extension(p_desc, DESC7F_TAG_AUDIO_PRESELECTION);
}

BITSTREAM_GET_SET(desc7f19, num_preselections, uint8, 24, 5);

BITSTREAM_GET_SET(desc7f19_presel, preselection_id, uint8, 0, 5);
BITSTREAM_GET_SET(desc7f19_presel, audio_rendering_indication, uint8, 5, 3);
BITSTREAM_GET_SET(desc7f19_presel, audio_description, uint8, 8, 1);
BITSTREAM_GET_SET(desc7f19_presel, spoken_subtitles, uint8, 9, 1);
BITSTREAM_GET_SET(desc7f19_presel, dialogue_enhancement, uint8, 10, 1);
BITSTREAM_GET_SET(desc7f19_presel, interactivity_enabled, uint8, 11, 1);
BITSTREAM_GET_SET(desc7f19_presel, language_code_present, uint8, 12, 1);
BITSTREAM_GET_SET(desc7f19_presel, text_label_present, uint8, 13, 1);
BITSTREAM_GET_SET(desc7f19_presel, multi_stream_info_present, uint8, 14, 1);
BITSTREAM_GET_SET(desc7f19_presel, future_extension, uint8, 15, 1);

static inline uint8_t *
desc7f19_presel_get_language_code_ptr(const uint8_t *p_desc_n)
{
    return desc7f19_presel_get_language_code_present(p_desc_n) ?
        (uint8_t *)(p_desc_n + DESC7F19_PRESEL_SIZE) : NULL;
}

static inline uint8_t *
desc7f19_presel_get_text_label_ptr(const uint8_t *p_desc_n)
{
    if (!desc7f19_presel_get_text_label_present(p_desc_n))
        return NULL;

    uint8_t *p;
    if ((p = desc7f19_presel_get_language_code_ptr(p_desc_n)))
        return p + DESC7F19_PRESEL_LANG_SIZE;
    return (uint8_t *)(p_desc_n + DESC7F19_PRESEL_SIZE);
}

BITSTREAM_GET_SET(desc7f19_presel_text, message_id, uint8, 0, 8);

static inline uint8_t *
desc7f19_presel_get_multi_stream_info_ptr(const uint8_t *p_desc_n)
{
    if (!desc7f19_presel_get_multi_stream_info_present(p_desc_n))
        return NULL;

    uint8_t *p;
    if ((p = desc7f19_presel_get_text_label_ptr(p_desc_n)))
        return p + DESC7F19_PRESEL_TEXT_SIZE;
    if ((p = desc7f19_presel_get_language_code_ptr(p_desc_n)))
        return p + DESC7F19_PRESEL_LANG_SIZE;
    return (uint8_t *)(p_desc_n + DESC7F19_PRESEL_SIZE);
}

BITSTREAM_GET_SET(desc7f19_presel_msi, num_aux_components, uint8, 0, 3);

static inline uint8_t *
desc7f19_presel_get_future_extension_ptr(const uint8_t *p_desc_n)
{
    if (!desc7f19_presel_get_future_extension(p_desc_n))
        return NULL;

    uint8_t *p;
    if ((p = desc7f19_presel_get_multi_stream_info_ptr(p_desc_n)))
        return p + DESC7F19_PRESEL_MSI_SIZE +
            desc7f19_presel_msi_get_num_aux_components(p) *
            DESC7F19_PRESEL_MSI_COMP_SIZE;
    if ((p = desc7f19_presel_get_text_label_ptr(p_desc_n)))
        return p + DESC7F19_PRESEL_TEXT_SIZE;
    if ((p = desc7f19_presel_get_language_code_ptr(p_desc_n)))
        return p + DESC7F19_PRESEL_LANG_SIZE;
    return (uint8_t *)(p_desc_n + DESC7F19_PRESEL_SIZE);
}

BITSTREAM_GET_SET(desc7f19_presel_ext, length, uint8, 3, 5);

static inline uint8_t desc7f19_presel_get_header_length(const uint8_t *p_desc_n)
{
    uint8_t length = DESC7F19_PRESEL_SIZE;
    if (desc7f19_presel_get_language_code_present(p_desc_n))
        length += DESC7F19_PRESEL_LANG_SIZE;
    if (desc7f19_presel_get_text_label_present(p_desc_n))
        length += DESC7F19_PRESEL_TEXT_SIZE;
    if (desc7f19_presel_get_multi_stream_info_present(p_desc_n))
        length += DESC7F19_PRESEL_MSI_SIZE;
    if (desc7f19_presel_get_future_extension(p_desc_n))
        length += DESC7F19_PRESEL_EXT_SIZE;
    return length;
}

static inline uint8_t desc7f19_presel_get_length(const uint8_t *p_desc_n)
{
    uint8_t length = desc7f19_presel_get_header_length(p_desc_n);

    uint8_t *p;
    if ((p = desc7f19_presel_get_multi_stream_info_ptr(p_desc_n)))
        length +=
            desc7f19_presel_msi_get_num_aux_components(p) *
            DESC7F19_PRESEL_MSI_COMP_SIZE;
    if ((p = desc7f19_presel_get_future_extension_ptr(p_desc_n)))
        length +=
            desc7f19_presel_ext_get_length(p) * DESC7F19_PRESEL_EXT_BYTE_SIZE;

    return length;
}

static inline uint8_t *desc7f19_next_presel(const uint8_t *p_desc,
                                            const uint8_t *p_desc_n)
{
    if (!p_desc_n)
        p_desc_n = p_desc + DESC7F19_HEADER_SIZE;
    else
        p_desc_n = p_desc_n + desc7f19_presel_get_length(p_desc_n);
    return desc_check(p_desc, p_desc_n, DESC7F19_PRESEL_SIZE);
}

#define desc7f19_each_presel(DESC, DESC_N) \
    desc_each(DESC, DESC_N, desc7f19_next_presel)
#define desc7f19_get_presel(DESC, N) \
    desc_get_at(DESC, N, desc7f19_next_presel)

static inline bool desc7f19_validate(const uint8_t *p_desc)
{
    uint8_t size = DESC7F19_HEADER_SIZE - DESC_HEADER_SIZE;
    uint8_t desc_length = desc_get_length(p_desc);
    if (size > desc_length)
        return false;
    desc7f19_each_presel(p_desc, p_desc_n)
        size += desc7f19_presel_get_length(p_desc_n);
    return size == desc_length;
}

static inline void desc7f19_print(const uint8_t *p_desc, f_print pf_print,
                                  void *opaque, print_type_t i_print_type)
{
    if (i_print_type == PRINT_XML)
        pf_print(opaque, "<AUDIO_PRESELECTION_DESC>");

    uint8_t *p;
    desc7f19_each_presel(p_desc, p_desc_n) {
        switch (i_print_type) {
            case PRINT_XML:
                pf_print(opaque,
                         "<AUDIO_PRESELECTION"
                         " id=\"%u\""
                         " audio_rendering_indication=\"%u\""
                         " audio_description=\"%u\""
                         " spoken_subtitles=\"%u\""
                         " dialogue_enhancement=\"%u\""
                         " interactivity_enabled=\"%u\""
                         ">",
                         desc7f19_presel_get_preselection_id(p_desc_n),
                         desc7f19_presel_get_audio_rendering_indication(p_desc_n),
                         desc7f19_presel_get_audio_description(p_desc_n),
                         desc7f19_presel_get_spoken_subtitles(p_desc_n),
                         desc7f19_presel_get_dialogue_enhancement(p_desc_n),
                         desc7f19_presel_get_interactivity_enabled(p_desc_n)
                        );
                if ((p = desc7f19_presel_get_language_code_ptr(p_desc_n))) {
                    char code[4];
                    memcpy(code, p, 3);
                    code[3] = '\0';
                    pf_print(opaque, "<AUDIO_PRESELECTION_LANG code=\"%s\"/>",
                             code);
                }
                if ((p = desc7f19_presel_get_text_label_ptr(p_desc_n))) {
                    pf_print(opaque, "<AUDIO_PRESELECTION_TEXT"
                             " message_id=\"%u\"/>",
                             desc7f19_presel_text_get_message_id(p));
                }
                if ((p = desc7f19_presel_get_multi_stream_info_ptr(p_desc_n))) {
                    uint8_t nb = desc7f19_presel_msi_get_num_aux_components(p);
                    char comp_tags[nb * 2 + 1];
                    for (uint8_t i = 0; i < nb; i++)
                        sprintf(comp_tags + i * 2, "%02x", p[i + 1]);
                    pf_print(opaque, "<AUDIO_PRESELECTION_MSI "
                             "components=\"%s\"/>",
                             comp_tags);
                }
                pf_print(opaque, "</AUDIO_PRESELECTION>");
                break;
            default:
                pf_print(opaque,
                         "    - desc 7f19 audio_preselection"
                         " id=%u"
                         " audio_rendering_indication=%u"
                         "%s"
                         "%s"
                         "%s"
                         "%s"
                         "",
                         desc7f19_presel_get_preselection_id(p_desc_n),
                         desc7f19_presel_get_audio_rendering_indication(p_desc_n),
                         desc7f19_presel_get_audio_description(p_desc_n) ?
                         " audio_description" : "",
                         desc7f19_presel_get_spoken_subtitles(p_desc_n) ?
                         " spoken_subtitles" : "",
                         desc7f19_presel_get_dialogue_enhancement(p_desc_n) ?
                         " dialogue_enhancement" : "",
                         desc7f19_presel_get_interactivity_enabled(p_desc_n) ?
                         " interactivity_enabled" : ""
                        );
                if ((p = desc7f19_presel_get_language_code_ptr(p_desc_n))) {
                    char code[4];
                    memcpy(code, p, 3);
                    code[3] = '\0';
                    pf_print(opaque,
                             "        - language code=\"%s\"", code);
                }
                if ((p = desc7f19_presel_get_text_label_ptr(p_desc_n))) {
                    pf_print(opaque, "        - text label message_id=%u",
                             desc7f19_presel_text_get_message_id(p));
                }
                if ((p = desc7f19_presel_get_multi_stream_info_ptr(p_desc_n))) {
                    uint8_t nb = desc7f19_presel_msi_get_num_aux_components(p);
                    char comp_tags[nb * 2 + 1];
                    for (uint8_t i = 0; i < nb; i++)
                        sprintf(comp_tags + i * 2, "%02x", p[i + 1]);
                    pf_print(opaque, "        - multi_stream_info %s",
                             comp_tags);
                }
        }
    }

    if (i_print_type == PRINT_XML)
        pf_print(opaque, "</AUDIO_PRESELECTION_DESC>");
}

#ifdef __cplusplus
}
#endif

#endif
