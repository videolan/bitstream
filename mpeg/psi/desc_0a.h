/*****************************************************************************
 * desc_0a.h: ISO/IEC 13818-1 Descriptor 0x0A (ISO-639 language descriptor)
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *          Georgi Chorbadzhiyski <georgi@unixsol.org>
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

#ifndef __BITSTREAM_MPEG_DESC_0A_H__
#define __BITSTREAM_MPEG_DESC_0A_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x0a: ISO-639 language descriptor
 *****************************************************************************/
#define DESC0A_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC0A_LANGUAGE_SIZE    4

static inline void desc0a_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x0a);
}

static inline uint8_t *desc0a_get_language(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC0A_HEADER_SIZE + n * DESC0A_LANGUAGE_SIZE;
    if (p_desc_n + DESC0A_LANGUAGE_SIZE - p_desc
         > desc_get_length(p_desc) + DESC0A_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

static inline void desc0an_set_code(uint8_t *p_desc_n, const uint8_t p_code[3])
{
    p_desc_n[0] = p_code[0];
    p_desc_n[1] = p_code[1];
    p_desc_n[2] = p_code[2];
}

static inline const uint8_t *desc0an_get_code(const uint8_t *p_desc_n)
{
    return p_desc_n;
}

static inline void desc0an_set_audiotype(uint8_t *p_desc_n, uint8_t i_type)
{
    p_desc_n[3] = i_type;
}

static inline uint8_t desc0an_get_audiotype(const uint8_t *p_desc_n)
{
    return p_desc_n[3];
}

static inline const char *desc0a_get_audiotype_txt(uint8_t audio_type)
{
    return audio_type == 0x00 ? "undefined" :
           audio_type == 0x01 ? "clean effects" :
           audio_type == 0x02 ? "hearing impaired" :
           audio_type == 0x03 ? "visual impaired commentary" : "reserved";
}

static inline bool desc0a_validate(const uint8_t *p_desc)
{
    return !(desc_get_length(p_desc) % DESC0A_LANGUAGE_SIZE);
}

static inline void desc0a_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc0a_get_language(p_desc, j)) != NULL) {
        j++;
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque, "<AUDIO_LANGUAGE_DESC language=\"%3.3s\" audiotype=\"%u\" autiotype_txt=\"%s\"/>",
                     (const char *)desc0an_get_code(p_desc_n),
                     desc0an_get_audiotype(p_desc_n),
                     desc0a_get_audiotype_txt(desc0an_get_audiotype(p_desc_n))
                    );
            break;
        default:
            pf_print(opaque, "    - desc 0a audio_language language=%3.3s audiotype=%u audiotype_txt=\"%s\"",
                     (const char *)desc0an_get_code(p_desc_n),
                     desc0an_get_audiotype(p_desc_n),
                     desc0a_get_audiotype_txt(desc0an_get_audiotype(p_desc_n))
                    );
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
