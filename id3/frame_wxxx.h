/*****************************************************************************
 * frame_wxxx.h: ID3 WXXX frame
 *****************************************************************************
 * Copyright (C) 2023 EasyTools
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
 * User defined URL link frame.
 */

#ifndef __BITSTREAM_ID3_FRAME_WXXX_H__
# define __BITSTREAM_ID3_FRAME_WXXX_H__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include <bitstream/common.h>
#include <bitstream/id3/id3v2.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* User defined URL link frame */
#define ID3V2_FRAME_ID_WXXX     ID3V2_FRAME_ID('W','X','X','X')
#define ID3V2_FRAME_WXXX_NAME   "user URL"
#define ID3V2_FRAME_WXXX_DESC   "User defined URL link"

static inline uint8_t
id3v2_frame_wxxx_get_encoding(const uint8_t *p_frame)
{
    const uint8_t *data = id3v2_frame_get_data(p_frame);
    return data[0];
}

static inline const char *
id3v2_frame_wxxx_get_description(const uint8_t *p_frame)
{
    return (const char *)(id3v2_frame_get_data(p_frame) + 1);
}

static inline const char *
id3v2_frame_wxxx_get_url(const uint8_t *p_frame)
{
    const char *description = id3v2_frame_wxxx_get_description(p_frame);
    return description + strlen(description) + 1;
}

static inline uint32_t
id3v2_frame_wxxx_get_url_size(const uint8_t *p_frame)
{
    uint32_t size = id3v2_frame_get_size(p_frame);
    const char *description = id3v2_frame_wxxx_get_description(p_frame);
    return size - 1 - strlen(description) - 1;
}

static inline bool
id3v2_frame_validate_wxxx(const uint8_t *p_frame)
{
    if (!p_frame || id3v2_frame_get_id(p_frame) != ID3V2_FRAME_ID_WXXX)
        return false;

    uint32_t size = id3v2_frame_get_size(p_frame);
    const uint8_t *data = id3v2_frame_get_data(p_frame);
    if (size < 2)
        return false;

    bool valid = false;
    for (uint32_t i = 1; i < size; i++)
        if (data[i] == '\0')
            valid = true;

    return valid;
}

static inline void id3v2_frame_print_wxxx(const uint8_t *p_frame,
                                          f_print pf_print, void *print_opaque,
                                          f_iconv pf_iconv, void *iconv_opaque,
                                          print_type_t i_print_type)
{
    uint8_t encoding = id3v2_frame_wxxx_get_encoding(p_frame);
    const char *description = id3v2_frame_wxxx_get_description(p_frame);
    const char *url = id3v2_frame_wxxx_get_url(p_frame);
    uint32_t url_size = id3v2_frame_wxxx_get_url_size(p_frame);

    char id[5] = { 0 };
    id3v2_frame_get_id_txt(p_frame, id);

    char url_str[url_size + 1];
    if (url_size)
        memcpy(url_str, url, url_size);
    url_str[url_size] = '\0';

    switch (i_print_type) {
        case PRINT_XML: {
            char *tmp_desc = bitstream_xml_escape(description);
            char *tmp_url = bitstream_xml_escape(url_str);
            pf_print(print_opaque, "<%s name=\"%s\" long_name=\"%s\" "
                     "encoding=\"%s\" desc=\"%s\">%s</%s>",
                     id, ID3V2_FRAME_WXXX_NAME, ID3V2_FRAME_WXXX_DESC,
                     id3v2_frame_text_encoding_str(encoding),
                     tmp_desc ?: "", tmp_url ?: "", id);
            free(tmp_url);
            free(tmp_desc);
            break;
        }
        default:
            pf_print(print_opaque, "  * %s (%s): %s",
                     ID3V2_FRAME_WXXX_NAME, description, url_str);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* !__BITSTREAM_ID3_FRAME_WXXX_H__ */
