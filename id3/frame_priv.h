/*****************************************************************************
 * frame_priv.h: ID3 PRIV frame
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
 * Private frame.
 */

#ifndef __BITSTREAM_ID3_FRAME_PRIV_H__
# define __BITSTREAM_ID3_FRAME_PRIV_H__

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

/* Private frame */
#define ID3V2_FRAME_ID_PRIV     ID3V2_FRAME_ID('P','R','I','V')
#define ID3V2_FRAME_PRIV_NAME   "private"
#define ID3V2_FRAME_PRIV_DESC   "Private"

#define ID3V2_FRAME_PRIV_APPLE_TS_TIMESTAMP \
    "com.apple.streaming.transportStreamTimestamp"

static inline const char *id3v2_frame_priv_get_owner(const uint8_t *p_frame)
{
    if (id3v2_frame_get_id(p_frame) != ID3V2_FRAME_ID_PRIV)
        return NULL;
    return (const char *)id3v2_frame_get_data(p_frame);
}

static inline uint8_t *id3v2_frame_priv_get_data(const uint8_t *p_frame)
{
    const char *owner = id3v2_frame_priv_get_owner(p_frame);
    return owner ? (uint8_t *)owner + strlen(owner) + 1 : NULL;
}

static inline bool
id3v2_frame_validate_priv(const uint8_t *p_frame)
{
    if (!p_frame || id3v2_frame_get_id(p_frame) != ID3V2_FRAME_ID_PRIV)
        return false;

    uint32_t size = id3v2_frame_get_size(p_frame);
    const uint8_t *data = id3v2_frame_get_data(p_frame);
    if (size < 1)
        return false;

    bool valid = false;
    for (uint32_t i = 0; i < size; i++)
        if (data[i] == '\0')
            valid = true;
    if (!valid)
        return false;

    const char *owner = id3v2_frame_priv_get_owner(p_frame);
    if (!strcmp(owner, ID3V2_FRAME_PRIV_APPLE_TS_TIMESTAMP))
        return size >= strlen(owner) + 1 + 8;
    return true;
}

static inline void id3v2_frame_print_priv(const uint8_t *p_frame,
                                          f_print pf_print, void *print_opaque,
                                          f_iconv pf_iconv, void *iconv_opaque,
                                          print_type_t i_print_type)
{
    const uint8_t *data = id3v2_frame_get_data(p_frame);
    uint32_t size = id3v2_frame_get_size(p_frame);
    const char *owner = (const char *)data;
    const uint8_t *priv_data = data + strlen(owner) + 1;

    char id[5] = { 0 };
    id3v2_frame_get_id_txt(p_frame, id);

    size -= strlen(owner) + 1;

    if (!strcmp(owner, ID3V2_FRAME_PRIV_APPLE_TS_TIMESTAMP)) {
        uint64_t timestamp =
            ((uint64_t)priv_data[0] << 56) +
            ((uint64_t)priv_data[1] << 48) +
            ((uint64_t)priv_data[2] << 40) +
            ((uint64_t)priv_data[3] << 32) +
            ((uint64_t)priv_data[4] << 24) +
            ((uint64_t)priv_data[5] << 16) +
            ((uint64_t)priv_data[6] << 8) +
            (uint64_t)priv_data[7];
        switch (i_print_type) {
            case PRINT_XML: {
                char *tmp_owner = bitstream_xml_escape(owner);
                pf_print(print_opaque,
                         "<%s name=\"%s\" long_name=\"%s\"  owner=\"%s\">"
                         "%" PRIu64
                         "</%s>",
                         id, ID3V2_FRAME_PRIV_NAME, ID3V2_FRAME_PRIV_DESC,
                         tmp_owner ?: "", timestamp, id);
                free(tmp_owner);
                break;
            }
            default:
                pf_print(print_opaque, "  * %s (%s): %" PRIu64,
                         id, owner, timestamp);
        }
    } else {
    char value[size * 2 + 1];
        for (uint32_t i = 0; i < size; i++)
            sprintf(value + i * 2, "%02x", priv_data[i]);
        value[size * 2] = '\0';

        switch (i_print_type) {
            case PRINT_XML: {
                char *tmp_owner = bitstream_xml_escape(owner);
                pf_print(print_opaque,
                         "<%s name=\"%s\" long_name=\"%s\"  owner=\"%s\">"
                         "%s"
                         "</%s>",
                         id, ID3V2_FRAME_PRIV_NAME, ID3V2_FRAME_PRIV_DESC,
                         tmp_owner ?: "", value, id);
                free(tmp_owner);
                break;
            }
            default:
                pf_print(print_opaque, "  * %s (%s): %s",
                         ID3V2_FRAME_PRIV_NAME, owner, value);
        }
    }
}


#ifdef __cplusplus
}
#endif

#endif /* !__BITSTREAM_ID3_FRAME_PRIV_H__ */
