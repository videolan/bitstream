/*****************************************************************************
 * desc_7f20.h: ETSI EN 303 560 Extension descriptor 0x20: TTML subtitling
 * descriptor
 *****************************************************************************
 * Copyright (C) 2026 Open Broadcast Systems Ltd
 *
 * Authors: Kieran Kunhya <kierank@obe.tv>
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
 *  - ETSI EN 303 560 V1.1.1 (2018-05) (TTML subtitling systems)
 */

#ifndef __BITSTREAM_DVB_DESC_7F20_H__
#define __BITSTREAM_DVB_DESC_7F20_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/dvb/si/desc_7f.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Extension descriptor 0x20: TTML subtitling descriptor
 *****************************************************************************/
#define DESC7F20_HEADER_SIZE            (DESC7F_HEADER_SIZE + 5)
#define DESC7F20_PROFILE_SIZE           1
#define DESC7F20_QUALIFIER_SIZE         4
#define DESC7F20_FONT_COUNT_SIZE        1
#define DESC7F20_FONT_SIZE              1
#define DESC7F20_TEXT_LENGTH_SIZE       1

/* subtitle_purpose, EN 303 560 table 2 */
#define DESC7F20_PURPOSE_SAME_LANG_DIALOGUE         0x00
#define DESC7F20_PURPOSE_OTHER_LANG_DIALOGUE        0x01
#define DESC7F20_PURPOSE_ALL_DIALOGUE               0x02
#define DESC7F20_PURPOSE_HARD_OF_HEARING            0x10
#define DESC7F20_PURPOSE_OTHER_LANG_DIALOGUE_HOH    0x11
#define DESC7F20_PURPOSE_ALL_DIALOGUE_HOH           0x12
#define DESC7F20_PURPOSE_AUDIO_DESCRIPTION          0x30
#define DESC7F20_PURPOSE_CONTENT_RELATED_COMMENTARY 0x31

/* TTS_suitability, EN 303 560 table 3 */
#define DESC7F20_TTS_UNKNOWN                        0x0
#define DESC7F20_TTS_SUITABLE                       0x1
#define DESC7F20_TTS_NOT_SUITABLE                   0x2

/* dvb_ttml_profile, EN 303 560 table 5 */
#define DESC7F20_PROFILE_DEFAULT                    0x00
#define DESC7F20_PROFILE_IMSC1_TEXT                 0x01
#define DESC7F20_PROFILE_EBUTTD                     0x02

static inline void desc7f20_init(uint8_t *p_desc)
{
    desc7f_init(p_desc);
    desc_set_length(p_desc, (DESC7F20_HEADER_SIZE - DESC_HEADER_SIZE));
    desc7f_set_tag_extension(p_desc, DESC7F_TAG_TTML_SUBTITLING);
    p_desc[7] = 0x30; /* reserved_zero_future_use */
}

static inline const uint8_t *desc7f20_get_code(const uint8_t *p_desc)
{
    return p_desc + DESC7F_HEADER_SIZE;
}

static inline void desc7f20_set_code(uint8_t *p_desc, const uint8_t p_code[3])
{
    memcpy(p_desc + DESC7F_HEADER_SIZE, p_code, 3);
}

BITSTREAM_GET_SET(desc7f20, subtitle_purpose, uint8, 48, 6)
BITSTREAM_GET_SET(desc7f20, tts_suitability, uint8, 54, 2)
BITSTREAM_GET_SET(desc7f20, essential_font_usage_flag, uint8, 56, 1)
BITSTREAM_GET_SET(desc7f20, qualifier_present_flag, uint8, 57, 1)
BITSTREAM_GET_SET(desc7f20, profile_count, uint8, 60, 4)

/* Only the profile list is at a fixed offset; every field after it moves with
 * counts read out of the descriptor itself. */
static inline uint8_t *desc7f20_get_profile(const uint8_t *p_desc, uint8_t n)
{
    if (n >= desc7f20_get_profile_count(p_desc))
        return NULL;
    return (uint8_t *)p_desc + DESC7F20_HEADER_SIZE + n * DESC7F20_PROFILE_SIZE;
}

static inline uint8_t *desc7f20_get_qualifier(const uint8_t *p_desc)
{
    if (!desc7f20_get_qualifier_present_flag(p_desc))
        return NULL;
    return (uint8_t *)p_desc + DESC7F20_HEADER_SIZE +
        desc7f20_get_profile_count(p_desc) * DESC7F20_PROFILE_SIZE;
}

BITSTREAM_GET_SET(desc7f20_qualifier, size, uint8, 0, 4)
BITSTREAM_GET_SET(desc7f20_qualifier, cadence, uint8, 4, 4)
BITSTREAM_GET_SET(desc7f20_qualifier, monochrome_flag, uint8, 8, 1)
BITSTREAM_GET_SET(desc7f20_qualifier, enhanced_accessibility_contrast_flag,
                  uint8, 9, 1);
BITSTREAM_GET_SET(desc7f20_qualifier, position, uint8, 10, 4)

static inline uint8_t *desc7f20_get_font_count(const uint8_t *p_desc)
{
    if (!desc7f20_get_essential_font_usage_flag(p_desc))
        return NULL;
    return (uint8_t *)p_desc + DESC7F20_HEADER_SIZE +
        desc7f20_get_profile_count(p_desc) * DESC7F20_PROFILE_SIZE +
        (desc7f20_get_qualifier_present_flag(p_desc) ?
         DESC7F20_QUALIFIER_SIZE : 0);
}

static inline uint8_t *desc7f20_get_font(const uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_font_count = desc7f20_get_font_count(p_desc);
    if (!p_font_count || n >= *p_font_count)
        return NULL;
    return p_font_count + DESC7F20_FONT_COUNT_SIZE + n * DESC7F20_FONT_SIZE;
}

BITSTREAM_GET_SET(desc7f20_font, font_id, uint8, 1, 7)

static inline uint8_t *desc7f20_get_text_length(const uint8_t *p_desc)
{
    uint8_t *p_font_count = desc7f20_get_font_count(p_desc);
    if (p_font_count)
        return p_font_count + DESC7F20_FONT_COUNT_SIZE +
            *p_font_count * DESC7F20_FONT_SIZE;
    return (uint8_t *)p_desc + DESC7F20_HEADER_SIZE +
        desc7f20_get_profile_count(p_desc) * DESC7F20_PROFILE_SIZE +
        (desc7f20_get_qualifier_present_flag(p_desc) ?
         DESC7F20_QUALIFIER_SIZE : 0);
}

static inline uint8_t *desc7f20_get_text(const uint8_t *p_desc,
                                         uint8_t *pi_length)
{
    uint8_t *p_text_length = desc7f20_get_text_length(p_desc);
    *pi_length = *p_text_length;
    return p_text_length + DESC7F20_TEXT_LENGTH_SIZE;
}

static inline bool desc7f20_validate(const uint8_t *p_desc)
{
    uint16_t i_desc_length = desc_get_length(p_desc);
    if (i_desc_length + DESC_HEADER_SIZE < DESC7F20_HEADER_SIZE)
        return false;
    /* dvb_ttml_profile_count shall not be set to zero */
    if (!desc7f20_get_profile_count(p_desc))
        return false;

    uint16_t i_size = DESC7F20_HEADER_SIZE - DESC_HEADER_SIZE +
        desc7f20_get_profile_count(p_desc) * DESC7F20_PROFILE_SIZE +
        (desc7f20_get_qualifier_present_flag(p_desc) ?
         DESC7F20_QUALIFIER_SIZE : 0);
    if (desc7f20_get_essential_font_usage_flag(p_desc)) {
        if (i_size + DESC7F20_FONT_COUNT_SIZE > i_desc_length)
            return false;
        i_size += DESC7F20_FONT_COUNT_SIZE +
            *desc7f20_get_font_count(p_desc) * DESC7F20_FONT_SIZE;
    }
    if (i_size + DESC7F20_TEXT_LENGTH_SIZE > i_desc_length)
        return false;
    i_size += DESC7F20_TEXT_LENGTH_SIZE + *desc7f20_get_text_length(p_desc);
    /* trailing reserved_zero_future_use bytes are allowed */
    return i_size <= i_desc_length;
}

static inline void desc7f20_print(const uint8_t *p_desc, f_print pf_print,
                                  void *opaque, print_type_t i_print_type)
{
    char psz_code[4];
    memcpy(psz_code, desc7f20_get_code(p_desc), 3);
    psz_code[3] = '\0';

    uint8_t i_profile_count = desc7f20_get_profile_count(p_desc);
    char psz_profiles[i_profile_count * 3 + 1];
    psz_profiles[0] = '\0';
    for (uint8_t i = 0; i < i_profile_count; i++)
        sprintf(psz_profiles + i * 3, i + 1 < i_profile_count ? "%02x " : "%02x",
                *desc7f20_get_profile(p_desc, i));

    uint8_t i_text_length;
    const uint8_t *p_text = desc7f20_get_text(p_desc, &i_text_length);
    char psz_text[i_text_length + 1];
    memcpy(psz_text, p_text, i_text_length);
    psz_text[i_text_length] = '\0';

    switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque,
                     "<TTML_SUBTITLING_DESC language=\"%s\""
                     " subtitle_purpose=\"%u\" tts_suitability=\"%u\""
                     " profiles=\"%s\" text=\"%s\"/>",
                     psz_code,
                     desc7f20_get_subtitle_purpose(p_desc),
                     desc7f20_get_tts_suitability(p_desc),
                     psz_profiles, psz_text);
            break;
        default:
            pf_print(opaque,
                     "    - desc 7f20 ttml_subtitling language=%s"
                     " subtitle_purpose=%u tts_suitability=%u"
                     " profiles=\"%s\" text=\"%s\"",
                     psz_code,
                     desc7f20_get_subtitle_purpose(p_desc),
                     desc7f20_get_tts_suitability(p_desc),
                     psz_profiles, psz_text);
    }
}

#ifdef __cplusplus
}
#endif

#endif
