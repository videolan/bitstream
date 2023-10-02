/*****************************************************************************
 * frame_comm.h: ID3 COMM frame
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
 * Comments frame.
 */

#ifndef __BITSTREAM_ID3_FRAME_COMM_H__
# define __BITSTREAM_ID3_FRAME_COMM_H__

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

/* Comments */
#define ID3V2_FRAME_ID_COMM     ID3V2_FRAME_ID('C','O','M','M')
#define ID3V2_FRAME_COMM_NAME   "comments"
#define ID3V2_FRAME_COMM_DESC   "Comments"

static inline bool
id3v2_frame_validate_comm(const uint8_t *p_frame)
{
    return p_frame &&
        id3v2_frame_get_id(p_frame) == ID3V2_FRAME_ID_COMM &&
        id3v2_frame_get_size(p_frame) >= 5;
}

static inline void
id3v2_frame_print_comm(const uint8_t *p_frame,
                       f_print pf_print, void *print_opaque,
                       f_iconv pf_iconv, void *iconv_opaque,
                       print_type_t i_print_type)
{
    const uint8_t *data = id3v2_frame_get_data(p_frame);
    uint8_t encoding = *data;
    const char *lang = (const char *)(data + 1);
    const char *desc = lang + 3;
    const char *value = desc + strlen(desc) + 1;
    uint32_t size = id3v2_frame_get_size(p_frame);
    size -= 1 + 3 + strlen(desc) + 1;

    char id[5] = { 0 };
    id3v2_frame_get_id_txt(p_frame, id);

    char tmp[size + 1];
    if (size)
        memcpy(tmp, value, size);
    tmp[size] = '\0';

    switch (i_print_type) {
        case PRINT_XML: {
            char *tmp_desc = bitstream_xml_escape(desc);
            char *tmp_value = bitstream_xml_escape(tmp);
            pf_print(print_opaque, "<%s name=\"%s\" long_name=\"%s\" "
                     "encoding=\"%s\" lang=\"%c%c%c\" "
                     "desc=\"%s\">%s</%s>",
                     id, ID3V2_FRAME_COMM_NAME, ID3V2_FRAME_COMM_DESC,
                     id3v2_frame_text_encoding_str(encoding),
                     lang[0], lang[1], lang[2],
                     tmp_desc ?: "", tmp_value ?: "", id);
            free(tmp_desc);
            free(tmp_value);
            break;
        }
        default:
            pf_print(print_opaque, "  * %s (%c%c%c - %s): %s",
                     ID3V2_FRAME_COMM_NAME,
                     lang[0], lang[1], lang[2], desc, tmp);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* !__BITSTREAM_ID3_FRAME_COMM_H__ */
