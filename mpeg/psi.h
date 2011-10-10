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

/*****************************************************************************
 * Conditional Access Table
 *****************************************************************************/
#define CAT_PID                 0x01
#define CAT_TABLE_ID            0x01
#define CAT_HEADER_SIZE         PSI_HEADER_SIZE_SYNTAX1

static inline void cat_init(uint8_t *p_cat)
{
    psi_init(p_cat, true);
    psi_set_tableid(p_cat, CAT_TABLE_ID);
    p_cat[1] &= ~0x40;
    psi_set_section(p_cat, 0);
    psi_set_lastsection(p_cat, 0);
}

static inline void cat_set_length(uint8_t *p_cat, uint16_t i_cat_length)
{
    psi_set_length(p_cat, CAT_HEADER_SIZE + PSI_CRC_SIZE - PSI_HEADER_SIZE
                    + i_cat_length);
}

static inline uint16_t cat_get_desclength(const uint8_t *p_cat)
{
    return psi_get_length(p_cat) - (CAT_HEADER_SIZE + PSI_CRC_SIZE - PSI_HEADER_SIZE);
}

static inline uint8_t *cat_get_descl(uint8_t *p_cat)
{
    return p_cat + CAT_HEADER_SIZE;
}

static inline const uint8_t *cat_get_descl_const(const uint8_t *p_cat)
{
    return p_cat + CAT_HEADER_SIZE;
}

static inline bool cat_validate(const uint8_t *p_cat)
{
    uint16_t i_section_size = psi_get_length(p_cat) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;

    if (!psi_get_syntax(p_cat) || psi_get_section(p_cat)
         || psi_get_lastsection(p_cat)
         || psi_get_tableid(p_cat) != CAT_TABLE_ID)
        return false;

    if (i_section_size < CAT_HEADER_SIZE
         || i_section_size < CAT_HEADER_SIZE + cat_get_desclength(p_cat))
        return false;

    if (!descl_validate(cat_get_descl_const(p_cat), cat_get_desclength(p_cat)))
        return false;

    return true;
}

static inline bool cat_table_validate(uint8_t **pp_sections)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);

        if (!psi_check_crc(p_section))
            return false;
    }

    return true;
}

/*****************************************************************************
 * Program Map Table
 *****************************************************************************/
#define PMT_TABLE_ID            0x2
#define PMT_HEADER_SIZE         (PSI_HEADER_SIZE_SYNTAX1 + 4)
#define PMT_ES_SIZE             5

#define pmt_set_program psi_set_tableidext
#define pmt_get_program psi_get_tableidext

static inline void pmt_init(uint8_t *p_pmt)
{
    psi_init(p_pmt, true);
    psi_set_tableid(p_pmt, PMT_TABLE_ID);
    p_pmt[1] &= ~0x40;
    psi_set_section(p_pmt, 0);
    psi_set_lastsection(p_pmt, 0);
    p_pmt[8] = 0xe0;
    p_pmt[10] = 0xf0;
}

static inline void pmt_set_length(uint8_t *p_pmt, uint16_t i_pmt_length)
{
    psi_set_length(p_pmt, PMT_HEADER_SIZE + PSI_CRC_SIZE - PSI_HEADER_SIZE
                    + i_pmt_length);
}

static inline void pmt_set_pcrpid(uint8_t *p_pmt, uint16_t i_pcr_pid)
{
    p_pmt[8] &= ~0x1f;
    p_pmt[8] |= i_pcr_pid >> 8;
    p_pmt[9] = i_pcr_pid & 0xff;
}

static inline uint16_t pmt_get_pcrpid(const uint8_t *p_pmt)
{
    return ((p_pmt[8] & 0x1f) << 8) | p_pmt[9];
}

static inline void pmt_set_desclength(uint8_t *p_pmt, uint16_t i_length)
{
    p_pmt[10] &= ~0xf;
    p_pmt[10] |= i_length >> 8;
    p_pmt[11] = i_length & 0xff;
}

static inline uint16_t pmt_get_desclength(const uint8_t *p_pmt)
{
    return ((p_pmt[10] & 0xf) << 8) | p_pmt[11];
}

static inline uint8_t *pmt_get_descs(uint8_t *p_pmt)
{
    return &p_pmt[10];
}

static inline void pmtn_init(uint8_t *p_pmt_n)
{
    p_pmt_n[1] = 0xe0;
    p_pmt_n[3] = 0xf0;
}

static inline void pmtn_set_streamtype(uint8_t *p_pmt_n, uint8_t i_stream_type)
{
    p_pmt_n[0] = i_stream_type;
}

static inline uint8_t pmtn_get_streamtype(const uint8_t *p_pmt_n)
{
    return p_pmt_n[0];
}

static inline void pmtn_set_pid(uint8_t *p_pmt_n, uint16_t i_pid)
{
    p_pmt_n[1] &= ~0x1f;
    p_pmt_n[1] |= i_pid >> 8;
    p_pmt_n[2] = i_pid & 0xff;
}

static inline uint16_t pmtn_get_pid(const uint8_t *p_pmt_n)
{
    return ((p_pmt_n[1] & 0x1f) << 8) | p_pmt_n[2];
}

static inline void pmtn_set_desclength(uint8_t *p_pmt_n, uint16_t i_length)
{
    p_pmt_n[3] &= ~0xf;
    p_pmt_n[3] |= i_length >> 8;
    p_pmt_n[4] = i_length & 0xff;
}

static inline uint16_t pmtn_get_desclength(const uint8_t *p_pmt_n)
{
    return ((p_pmt_n[3] & 0xf) << 8) | p_pmt_n[4];
}

static inline uint8_t *pmtn_get_descs(uint8_t *p_pmt_n)
{
    return &p_pmt_n[3];
}

static inline uint8_t *pmt_get_es(uint8_t *p_pmt, uint8_t n)
{
    uint16_t i_section_size = psi_get_length(p_pmt) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    uint8_t *p_pmt_n = p_pmt + PMT_HEADER_SIZE + pmt_get_desclength(p_pmt);
    if (p_pmt_n - p_pmt > i_section_size) return NULL;

    while (n) {
        if (p_pmt_n + PMT_ES_SIZE - p_pmt > i_section_size) return NULL;
        p_pmt_n += PMT_ES_SIZE + pmtn_get_desclength(p_pmt_n);
        n--;
    }
    if (p_pmt_n - p_pmt >= i_section_size) return NULL;
    return p_pmt_n;
}

static inline bool pmt_validate_es(const uint8_t *p_pmt, const uint8_t *p_pmt_n,
                                   uint16_t i_desclength)
{
    uint16_t i_section_size = psi_get_length(p_pmt) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    return (p_pmt_n + PMT_ES_SIZE + i_desclength
             <= p_pmt + i_section_size);
}

static inline bool pmt_validate(const uint8_t *p_pmt)
{
    uint16_t i_section_size = psi_get_length(p_pmt) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    const uint8_t *p_pmt_n;

    if (!psi_get_syntax(p_pmt) || psi_get_section(p_pmt)
         || psi_get_lastsection(p_pmt)
         || psi_get_tableid(p_pmt) != PMT_TABLE_ID)
        return false;

    if (!psi_check_crc(p_pmt))
        return false;

    if (i_section_size < PMT_HEADER_SIZE
         || i_section_size < PMT_HEADER_SIZE + pmt_get_desclength(p_pmt))
        return false;

    if (!descs_validate(p_pmt + 10))
        return false;

    p_pmt_n = p_pmt + PMT_HEADER_SIZE + pmt_get_desclength(p_pmt);

    while (p_pmt_n + PMT_ES_SIZE - p_pmt <= i_section_size
            && p_pmt_n + PMT_ES_SIZE + pmtn_get_desclength(p_pmt_n) - p_pmt
                <= i_section_size) {
        if (!descs_validate(p_pmt_n + 3))
            return false;

        p_pmt_n += PMT_ES_SIZE + pmtn_get_desclength(p_pmt_n);
    }

    return (p_pmt_n - p_pmt == i_section_size);
}

static inline uint8_t *pmt_find_es(uint8_t *p_pmt, uint16_t i_pid)
{
    uint8_t *p_es;
    uint8_t j = 0;

    while ((p_es = pmt_get_es(p_pmt, j)) != NULL) {
        j++;
        if (pmtn_get_pid(p_es) == i_pid)
            return p_es;
    }

    return NULL;
}

#ifdef __cplusplus
}
#endif

#endif
