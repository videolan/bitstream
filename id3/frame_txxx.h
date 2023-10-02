/*****************************************************************************
 * frame_txxx.h: ID3 TXXX frame
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
 * User defined text information frame.
 */

#ifndef __BITSTREAM_ID3_FRAME_TXXX_H__
# define __BITSTREAM_ID3_FRAME_TXXX_H__

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

/* User defined text information frame */
#define ID3V2_FRAME_ID_TXXX     ID3V2_FRAME_ID('T','X','X','X')
#define ID3V2_FRAME_TXXX_NAME   "user text"
#define ID3V2_FRAME_TXXX_DESC   "User defined text information"

static inline uint8_t
id3v2_frame_txxx_get_encoding(const uint8_t *p_frame)
{
    const uint8_t *data = id3v2_frame_get_data(p_frame);
    return data[0];
}

static inline const char *
id3v2_frame_txxx_get_description(const uint8_t *p_frame)
{
    return (const char *)(id3v2_frame_get_data(p_frame) + 1);
}

static inline const char *
id3v2_frame_txxx_get_value(const uint8_t *p_frame)
{
    const char *description = id3v2_frame_txxx_get_description(p_frame);
    return description + strlen(description) + 1;
}

static inline uint32_t
id3v2_frame_txxx_get_value_size(const uint8_t *p_frame)
{
    uint32_t size = id3v2_frame_get_size(p_frame);
    const char *description = id3v2_frame_txxx_get_description(p_frame);
    return size - 1 - strlen(description) - 1;
}

static inline bool
id3v2_frame_validate_txxx(const uint8_t *p_frame)
{
    if (!p_frame || id3v2_frame_get_id(p_frame) != ID3V2_FRAME_ID_TXXX)
        return false;

    uint32_t size = id3v2_frame_get_size(p_frame);
    const uint8_t *data = id3v2_frame_get_data(p_frame);
    if (size < 2)
        return false;

    bool valid = false;
    for (uint32_t i = 1; i < size; i++)
        if (data[i] == '\0')
            valid = true;

    if (!valid)
        return false;

    return true;
}

static inline void
id3v2_frame_print_txxx(const uint8_t *p_frame,
                       f_print pf_print, void *print_opaque,
                       f_iconv pf_iconv, void *iconv_opaque,
                       print_type_t i_print_type)
{
    uint8_t encoding = id3v2_frame_txxx_get_encoding(p_frame);
    const char *description = id3v2_frame_txxx_get_description(p_frame);
    const char *value = id3v2_frame_txxx_get_value(p_frame);
    uint32_t value_size = id3v2_frame_txxx_get_value_size(p_frame);

    char id[5] = { 0 };
    id3v2_frame_get_id_txt(p_frame, id);

    char value_str[value_size + 1];
    if (value_size > 0)
        memcpy(value_str, value, value_size);
    value_str[value_size] = '\0';

    switch (i_print_type) {
        case PRINT_XML: {
            char *tmp_desc = bitstream_xml_escape(description);
            char *tmp_value = bitstream_xml_escape(value_str);
            pf_print(print_opaque, "<%s name=\"%s\" long_name=\"%s\" "
                     "encoding=\"%s\" desc=\"%s\">%s</%s>",
                     id, ID3V2_FRAME_TXXX_NAME, ID3V2_FRAME_TXXX_DESC,
                     id3v2_frame_text_encoding_str(encoding),
                     tmp_desc ?: "", tmp_value ?: "", id);
            free(tmp_desc);
            free(tmp_value);
            break;
        }
        default:
            pf_print(print_opaque, "  * %s (%s): %s",
                     ID3V2_FRAME_TXXX_NAME, description, value_str);
            break;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* !__BITSTREAM_ID3_FRAME_TXXX_H__ */
