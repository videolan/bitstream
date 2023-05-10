/*****************************************************************************
 * id3v2.h: ID3 tag version 2.4.0 (printing)
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

#ifndef __BITSTREAM_ID3V2_PRINT_H__
# define __BITSTREAM_ID3V2_PRINT_H__

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include <bitstream/common.h>
#include <bitstream/id3/id3v2.h>
#include <bitstream/id3/frames_list.h>

#ifdef __cplusplus
extern "C"
{
#endif

static inline void
id3v2_frame_print_t(const uint8_t *p_frame,
                    f_print pf_print, void *print_opaque,
                    f_iconv pf_iconv, void *iconv_opaque,
                    print_type_t i_print_type)
{
    const char *name = id3v2_frame_name(p_frame);
    const char *desc = id3v2_frame_desc(p_frame);
    uint32_t size = id3v2_frame_get_size(p_frame);
    uint8_t *data = id3v2_frame_get_data(p_frame);
    uint8_t encoding = *data;

    char id[5] = { 0 };
    id3v2_frame_get_id_txt(p_frame, id);

    char content[size];
    memcpy(content, data + 1, size - 1);
    for (unsigned i = 0; i < size - 2; i++)
        if (content[i] == '\0')
            content[i] = ' ';
    content[size - 1] = '\0';

    switch (i_print_type) {
        case PRINT_XML: {
            char *tmp = bitstream_xml_escape(content);
            pf_print(print_opaque,
                     "<%s%s%s%s%s%s%s encoding=\"%s\">%s</%s>",
                     id,
                     name ? " name=\"" : "", name ?: "", name ? "\"" : "",
                     desc ? " long_name=\"" : "", desc ?: "", desc ? "\"" : "",
                     id3v2_frame_text_encoding_str(encoding),
                     tmp ?: "", id);
            free(tmp);
            break;
        }
        default:
            pf_print(print_opaque, "  * %s: %s", name ?: id, content);
            break;
    }
}

static inline void id3v2_frame_print_w(const uint8_t *p_frame,
                                       f_print pf_print, void *print_opaque,
                                       f_iconv pf_iconv, void *iconv_opaque,
                                       print_type_t i_print_type)
{
    const char *name = id3v2_frame_name(p_frame);
    const char *desc = id3v2_frame_desc(p_frame);
    const uint8_t *data = id3v2_frame_get_data(p_frame);
    const char *value = (const char *)data;
    uint32_t size = id3v2_frame_get_size(p_frame);

    char id[5] = { 0 };
    id3v2_frame_get_id_txt(p_frame, id);

    char value_str[size + 1];
    if (size)
        memcpy(value_str, value, size);
    value_str[size] = '\0';

    switch (i_print_type) {
        case PRINT_XML: {
            char *tmp = bitstream_xml_escape(value_str);
            pf_print(print_opaque,
                     "<%s%s%s%s%s%s%s>%s</%s>",
                     id,
                     name ? " name=\"" : "", name ?: "", name ? "\"" : "",
                     desc ? " long_name=\"" : "", desc ?: "", desc ? "\"" : "",
                     tmp ?: "", id);
            free(tmp);
            break;
        }
        default: {
            const char *name = id3v2_frame_name(p_frame);
            if (!name)
                name = id;
            pf_print(print_opaque, "  * %s: %s", name, value_str);
        }
    }
}

static inline void
id3v2_frame_print_binary(const uint8_t *p_frame,
                         f_print pf_print, void *print_opaque,
                         f_iconv pf_iconv, void *iconv_opaque,
                         print_type_t i_print_type)
{
    const uint8_t *data = id3v2_frame_get_data(p_frame);
    uint32_t size = id3v2_frame_get_size(p_frame);
    char id[5] = { 0 };
    id3v2_frame_get_id_txt(p_frame, id);

    char value[size * 2 + 1];
    for (uint32_t i = 0; i < size; i++)
        sprintf(value + i * 2, "%02x", data[i]);
    value[size * 2] = '\0';

    switch (i_print_type) {
        case PRINT_XML:
            pf_print(print_opaque, "<%s>%s</%s>", id, value, id);
            break;
        default:
            pf_print(print_opaque, "  * %s: %s", id, value);
    }
}

static inline void
id3v2_frame_print_error(const uint8_t *p_frame,
                        f_print pf_print, void *opaque,
                        f_iconv pf_iconv, void *iconv_opaque,
                        print_type_t i_print_type)
{
    char id[5] = { 0 };
    id3v2_frame_get_id_txt(p_frame, id);

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<INVALID_FRAME id=\"%s\"/>", id);
        break;
    default:
         pf_print(opaque, "  * %s invalid", id);
    }
}

static inline void id3v2_frame_print(const uint8_t *p_frame,
                                     f_print pf_print, void *opaque,
                                     f_iconv pf_iconv, void *iconv_opaque,
                                     print_type_t i_print_type)
{
    char id[5] = { 0 };
    id3v2_frame_get_id_txt(p_frame, id);

    void (*print_frame)(const uint8_t *p_frame,
                        f_print pf_print, void *print_opaque,
                        f_iconv pf_iconv, void *iconv_opaque,
                        print_type_t i_print_type)
        = id3v2_frame_print_binary;

    switch (id3v2_frame_get_id(p_frame)) {
#define CASE(ID, Name)                                  \
        case ID3V2_FRAME_ID_##ID:                       \
            if (id3v2_frame_validate_##Name(p_frame))   \
                print_frame = id3v2_frame_print_##Name; \
            else                                        \
                print_frame = id3v2_frame_print_error;  \
            break;
        ID3V2_EACH_FRAME(CASE)
#undef CASE
    }

    print_frame(p_frame, pf_print, opaque, pf_iconv, iconv_opaque,
                i_print_type);
}

static inline void id3v2_print(const uint8_t *p_id3v2,
                               f_print pf_print, void *opaque,
                               f_iconv pf_iconv, void *iconv_opaque,
                               print_type_t i_print_type)
{
    switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque, "<ID3V2 major=\"%u\" revision=\"%u\">",
                     id3v2_get_version_major(p_id3v2),
                     id3v2_get_version_rev(p_id3v2));
            break;
        default:
            pf_print(opaque, "id3v2.%u.%u",
                     id3v2_get_version_major(p_id3v2),
                     id3v2_get_version_rev(p_id3v2));
    }

    id3v2_each_frame(p_id3v2, frame) {
        id3v2_frame_print(frame, pf_print, opaque, pf_iconv, iconv_opaque,
                          i_print_type);
    }

    switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque, "</ID3V2>");
            break;
        default:
            break;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* !__BITSTREAM_ID3_V2_H__ */
