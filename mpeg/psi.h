/*****************************************************************************
 * psi.h: ISO/IEC 13818-1 Program Stream Information
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
 * $Id$
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
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

#ifndef __BITSTREAM_MPEG_PSI_H__
#define __BITSTREAM_MPEG_PSI_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/ts.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/pat.h>
#include <bitstream/mpeg/psi/cat.h>
#include <bitstream/mpeg/psi/pmt.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x05: Registration descriptor
 *****************************************************************************/
#define DESC05_HEADER_SIZE      (DESC_HEADER_SIZE + 4)

static inline void desc05_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x05);
    desc_set_length(p_desc, DESC05_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline void desc05_set_identifier(uint8_t *p_desc, uint8_t p_id[4])
{
    p_desc[2] = p_id[0];
    p_desc[3] = p_id[1];
    p_desc[4] = p_id[2];
    p_desc[5] = p_id[3];
}

static inline const uint8_t *desc05_get_identifier(const uint8_t *p_desc)
{
    return p_desc + 2;
}

static inline bool desc05_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC05_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc05_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<REGISTRATION_DESC identifier=\"%4.4s\"/>",
                 desc05_get_identifier(p_desc));
        break;
    default:
        pf_print(opaque, "    - desc 05 identifier=%4.4s",
                 desc05_get_identifier(p_desc));
    }
}

/*****************************************************************************
 * Descriptor 0x09: Conditional access descriptor
 *****************************************************************************/
#define DESC09_HEADER_SIZE      (DESC_HEADER_SIZE + 4)

static inline uint16_t desc09_get_sysid(const uint8_t *p_desc)
{
    return (p_desc[2] << 8) | p_desc[3];
}

static inline uint16_t desc09_get_pid(const uint8_t *p_desc)
{
    return ((p_desc[4] & 0x1f) << 8) | p_desc[5];
}

static inline bool desc09_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC09_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc09_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<CA_DESC sysid=\"0x%hx\" pid=\"%hu\"/>",
                 desc09_get_sysid(p_desc), desc09_get_pid(p_desc));
        break;
    default:
        pf_print(opaque, "    - desc 09 sysid=0x%hx pid=%hu",
                 desc09_get_sysid(p_desc), desc09_get_pid(p_desc));
    }
}

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
            pf_print(opaque, "<AUDIO_LANGUAGE_DESC language=\"%3.3s\" audiotype=\"0x%hhx\"/>",
                     (const char *)desc0an_get_code(p_desc_n),
                     desc0an_get_audiotype(p_desc_n));
            break;
        default:
            pf_print(opaque, "    - desc 0a language=%3.3s audiotype=0x%hhx",
                     (const char *)desc0an_get_code(p_desc_n),
                     desc0an_get_audiotype(p_desc_n));
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
