/*****************************************************************************
 * desc_03.h: ISO/IEC 13818-1 Descriptor 0x03 (Audio stream descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_03_H__
#define __BITSTREAM_MPEG_DESC_03_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x03: Audio stream descriptor
 *****************************************************************************/
#define DESC03_HEADER_SIZE      (DESC_HEADER_SIZE + 1)

static inline void desc03_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x03);
    desc_set_length(p_desc, DESC03_HEADER_SIZE - DESC_HEADER_SIZE);
    p_desc[2] = 0x07;
}

static inline bool desc03_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC03_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline bool desc03_get_free_format_flag(const uint8_t *p_desc) {
    return (p_desc[2] & 0x80) == 0x80;
}

static inline void desc03_set_free_format_flag(uint8_t *p_desc, bool b_free_format_flag) {
    p_desc[2] = (b_free_format_flag ? (p_desc[2] | 0x80) : (p_desc[2] &~ 0x80));
}

static inline bool desc03_get_ID(const uint8_t *p_desc) {
    return (p_desc[2] & 0x40) == 0x40;
}

static inline void desc03_set_ID(uint8_t *p_desc, bool b_id) {
    p_desc[2] = (b_id ? (p_desc[2] | 0x40) : (p_desc[2] &~ 0x40));
}

static inline const char *desc03_get_ID_txt(const uint8_t ID) {
    return ID ? "MPEG Audio" : "Other";
}

static inline uint8_t desc03_get_layer(const uint8_t *p_desc) {
    return (p_desc[2] >> 4) & 0x03; /* xx11xxxx */
}

static inline void desc03_set_layer(uint8_t *p_desc, uint8_t i_layer) {
    p_desc[2] = (p_desc[2] & 0xcf) | ((i_layer & 0x03) << 4);
}

static inline const char *desc03_get_layer_txt(const uint8_t layer) {
    return layer == 0 ? "reserved" :
           layer == 1 ? "Layer III" :
           layer == 2 ? "Layer II" :
           layer == 3 ? "Layer I" : "reserved";
}

static inline bool desc03_get_vbr_flag(const uint8_t *p_desc) {
    return (p_desc[2] & 0x08) == 0x08;
}

static inline void desc03_set_vbr_flag(uint8_t *p_desc, bool b_free_format_flag) {
    p_desc[2] = (b_free_format_flag ? (p_desc[2] | 0x08) : (p_desc[2] &~ 0x08));
}

static inline void desc03_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
            "<AUDIO_STREAM_DESC free_format_tag=\"%u\" ID=\"%u\" layer=\"%u\""
            " ID_txt=\"%s\" layer_txt=\"%s\" vbr_audio_flag=\"%u\"/>",
                 desc03_get_free_format_flag(p_desc),
                 desc03_get_ID(p_desc),
                 desc03_get_layer(p_desc),
                 desc03_get_ID_txt(desc03_get_ID(p_desc)),
                 desc03_get_layer_txt(desc03_get_layer(p_desc)),
                 desc03_get_vbr_flag(p_desc)
        );
        break;
    default:
        pf_print(opaque,
            "    - desc 03 audio_stream free_format_tag=%u ID=%u layer=%u"
            " ID_txt=\"%s\" layer_txt=\"%s\" vbr_audio_flag=%u",
                 desc03_get_free_format_flag(p_desc),
                 desc03_get_ID(p_desc),
                 desc03_get_layer(p_desc),
                 desc03_get_ID_txt(desc03_get_ID(p_desc)),
                 desc03_get_layer_txt(desc03_get_layer(p_desc)),
                 desc03_get_vbr_flag(p_desc)
        );
    }
}

#ifdef __cplusplus
}
#endif

#endif
