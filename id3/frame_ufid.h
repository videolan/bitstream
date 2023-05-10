/*****************************************************************************
 * frame_ufid.h: ID3 UFID frame
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
 * Unique file identifier frame.
 */

#ifndef __BITSTREAM_ID3_FRAME_UFID_H__
# define __BITSTREAM_ID3_FRAME_UFID_H__

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

/* Unique file idendifier */
#define ID3V2_FRAME_ID_UFID     ID3V2_FRAME_ID('U','F','I','D')
#define ID3V2_FRAME_UFID_NAME   "ufid"
#define ID3V2_FRAME_UFID_DESC   "Unique file idendifier"

static inline bool
id3v2_frame_validate_ufid(const uint8_t *p_frame)
{
    return p_frame &&
        id3v2_frame_get_id(p_frame) == ID3V2_FRAME_ID_UFID &&
        id3v2_frame_get_size(p_frame) >= 2;
}

static inline void
id3v2_frame_print_ufid(const uint8_t *p_frame,
                       f_print pf_print, void *print_opaque,
                       f_iconv pf_iconv, void *iconv_opaque,
                       print_type_t i_print_type)
{
    const uint8_t *data = id3v2_frame_get_data(p_frame);
    uint32_t size = id3v2_frame_get_size(p_frame);
    const char *owner = (const char *)data;
    const uint8_t *identifier = data + strlen(owner) + 1;
    size -= strlen(owner) - 1;

    char id[5] = { 0 };
    id3v2_frame_get_id_txt(p_frame, id);

    char identifer_str[size * 2 + 1];
    for (unsigned i = 0; i < size; i++)
        sprintf(identifer_str + i * 2, "%02x", identifier[i]);
    identifer_str[size * 2] = '\0';

    switch (i_print_type) {
        case PRINT_XML: {
            char *tmp_owner = bitstream_xml_escape(owner);
            pf_print(print_opaque, "<%s name=\"%s\" long_name=\"%s\" "
                     "owner=\"%s\">%s</%s>",
                     id, ID3V2_FRAME_UFID_NAME, ID3V2_FRAME_UFID_DESC,
                     tmp_owner ?: "", identifer_str, id);
            free(tmp_owner);
            break;
        }
        default:
            pf_print(print_opaque, "  * %s (%s): %s",
                     ID3V2_FRAME_UFID_NAME, owner, identifer_str);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* !__BITSTREAM_ID3_FRAME_UFID_H__ */
