/*****************************************************************************
 * desc_7f06.h: ETSI EN 300 468 Extension descriptor 0x06: Supplementary audio
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

#ifndef __BITSTREAM_DVB_DESC_7F06_H__
#define __BITSTREAM_DVB_DESC_7F06_H__

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
#define DESC7F06_HEADER_SIZE      (DESC7F_HEADER_SIZE + 1)

static inline void desc7f06_init(uint8_t *p_desc)
{
    desc7f_init(p_desc);
    desc_set_length(p_desc, (DESC7F06_HEADER_SIZE - DESC_HEADER_SIZE));
    desc7f_set_tag_extension(p_desc, DESC7F_TAG_SUPPLEMENTARY_AUDIO);
}

BITSTREAM_GET_SET(desc7f06, mix_type, uint8, 24, 1);
BITSTREAM_GET_SET(desc7f06, editorial_classification, uint8, 25, 5);
BITSTREAM_GET_SET(desc7f06, language_code_present, uint8, 31, 1);
BITSTREAM_GET_SET(desc7f06_opt, language_code, uint32, 32, 24);

static inline uint32_t desc7f06_get_language_code(const uint8_t *p_desc)
{
    return desc7f06_get_language_code_present(p_desc) ?
        desc7f06_opt_get_language_code(p_desc) : 0;
}

static inline bool desc7f06_validate(const uint8_t *p_desc)
{
    if (desc_get_length(p_desc) < DESC7F06_HEADER_SIZE - DESC_HEADER_SIZE)
        return false;

    if (desc7f06_get_language_code_present(p_desc) &&
        desc_get_length(p_desc) < DESC7F06_HEADER_SIZE + 3 - DESC_HEADER_SIZE)
        return false;
    return true;
}

static inline void desc7f06_print(const uint8_t *p_desc, f_print pf_print,
                                  void *opaque, print_type_t i_print_type)
{
    uint8_t language_code_present = desc7f06_get_language_code_present(p_desc);
    char language_code[4];
    if (language_code_present) {
        uint32_t v = desc7f06_opt_get_language_code(p_desc);
        language_code[0] = v >> 16;
        language_code[1] = v >> 8;
        language_code[2] = v;
        language_code[3] = '\0';
    } else {
        language_code[0] = '\0';
    }

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 "<SUPPLEMENTARY_AUDIO_DESC"
                 " mix_type=\"%u\""
                 " editorial_classification=\"%u\""
                 ">",
                 desc7f06_get_mix_type(p_desc),
                 desc7f06_get_editorial_classification(p_desc)
                );
        if (language_code_present)
            pf_print(opaque,
                     "<SUPPLEMENTARY_AUDIO_LANG code=\"%s\"/>",
                     language_code
                    );
        pf_print(opaque, "</SUPPLEMENTARY_AUDIO_DESC>");
        break;
    default:
        pf_print(opaque,
                 "    - desc 7f06 supplementary_audio"
                 " mix_type=%u"
                 " editorial_classification=%u",
                 desc7f06_get_mix_type(p_desc),
                 desc7f06_get_editorial_classification(p_desc)
                );
        if (language_code_present)
            pf_print(opaque,
                     "        - language code=\"%s\"", language_code);
    }
}

#ifdef __cplusplus
}
#endif

#endif
