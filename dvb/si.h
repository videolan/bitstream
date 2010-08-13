/*****************************************************************************
 * si.h: ETSI EN 300 468 Service Information
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
 * $Id$
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 *****************************************************************************/

/*
 * Normative references:
 *  - ISO/IEC 13818-1:2007(E) (MPEG-2 Systems)
 *  - ETSI EN 300 468 V1.11.1 (2010-04) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_SI_H__
#define __BITSTREAM_DVB_SI_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x40: Network name descriptor
 *****************************************************************************/
#define DESC40_HEADER_SIZE      DESC_HEADER_SIZE

static inline void desc40_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x40);
}

static inline void desc40_set_networkname(uint8_t *p_desc,
                                          const char *psz_network_name)
{
    uint8_t i_length = strlen(psz_network_name);
    desc_set_length(p_desc, i_length);
    memcpy(p_desc + 2, psz_network_name, i_length);
}

static inline void desc40_get_networkname(const uint8_t *p_desc,
                                          char *psz_network_name)
{
    uint8_t i_length = desc_get_length(p_desc);
    memcpy(psz_network_name, p_desc + 2, i_length);
    psz_network_name[i_length] = '\0';
}

static inline void desc40_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque)
{
    DESC_DECLARE_STRING1(psz_network_name);
    desc40_get_networkname(p_desc, psz_network_name);
    pf_print(opaque, "    - desc 40 networkname=%s", psz_network_name);
}

/*****************************************************************************
 * Descriptor 0x48: Service descriptor
 *****************************************************************************/
#define DESC48_HEADER_SIZE      3

static inline uint8_t desc48_get_type(const uint8_t *p_desc)
{
    return p_desc[2];
}

static inline void desc48_get_provider(const uint8_t *p_desc,
                                       char *psz_provider)
{
    const uint8_t *p = p_desc + 3;
    memcpy(psz_provider, p + 1, *p);
    psz_provider[*p] = '\0';
}

static inline void desc48_get_service(const uint8_t *p_desc,
                                      char *psz_service)
{
    const uint8_t *p = p_desc + 4 + p_desc[3];
    memcpy(psz_service, p + 1, *p);
    psz_service[*p] = '\0';
}

static inline void desc48_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque)
{
    DESC_DECLARE_STRING1(psz_provider);
    DESC_DECLARE_STRING1(psz_service);
    desc48_get_provider(p_desc, psz_provider);
    desc48_get_service(p_desc, psz_service);
    pf_print(opaque, "    - desc 48 provider=%s service=%s", psz_provider,
             psz_service);
}

/*****************************************************************************
 * Descriptor 0x56: Teletext descriptor
 *****************************************************************************/
#define DESC56_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC56_LANGUAGE_SIZE    5

static inline void desc56_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x56);
}

static inline uint8_t *desc56_get_language(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC56_HEADER_SIZE + n * DESC56_LANGUAGE_SIZE;
    if (p_desc_n + DESC56_LANGUAGE_SIZE - p_desc
         > desc_get_length(p_desc) + DESC56_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

#define desc56n_set_code desc0an_set_code
#define desc56n_get_code desc0an_get_code

static inline void desc56n_set_teletexttype(uint8_t *p_desc_n, uint8_t i_type)
{
    p_desc_n[3] &= ~0xfc;
    p_desc_n[3] |= (i_type << 3) & 0xfc;
}

static inline uint8_t desc56n_get_teletexttype(const uint8_t *p_desc_n)
{
    return p_desc_n[3] >> 3;
}

static inline void desc56n_set_teletextmagazine(uint8_t *p_desc_n,
                                                uint8_t i_magazine)
{
    p_desc_n[3] &= ~0x3;
    p_desc_n[3] |= (i_magazine & 0x3);
}

static inline uint8_t desc56n_get_teletextmagazine(const uint8_t *p_desc_n)
{
    return p_desc_n[3] & 0x3;
}

static inline void desc56n_set_teletextpage(uint8_t *p_desc_n, uint8_t i_page)
{
    p_desc_n[4] = i_page;
}

static inline uint8_t desc56n_get_teletextpage(const uint8_t *p_desc_n)
{
    return p_desc_n[4];
}

static inline void desc56_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc56_get_language(p_desc, j)) != NULL) {
        j++;
        pf_print(opaque,
             "    - desc 56 telx language=%3.3s type=%hhu mag=%hhu page=%hhu",
             (const char *)desc56n_get_code(p_desc_n),
             desc56n_get_teletexttype(p_desc_n),
             desc56n_get_teletextmagazine(p_desc_n),
             desc56n_get_teletextpage(p_desc_n));
    }
}

/*****************************************************************************
 * Descriptor 0x59: Subtitling descriptor
 *****************************************************************************/
#define DESC59_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC59_LANGUAGE_SIZE    8

static inline void desc59_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x59);
}

static inline uint8_t *desc59_get_language(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC59_HEADER_SIZE + n * DESC59_LANGUAGE_SIZE;
    if (p_desc_n + DESC59_LANGUAGE_SIZE - p_desc
         > desc_get_length(p_desc) + DESC59_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

#define desc59n_set_code desc0an_set_code
#define desc59n_get_code desc0an_get_code

static inline void desc59n_set_subtitlingtype(uint8_t *p_desc_n, uint8_t i_type)
{
    p_desc_n[3] = i_type;
}

static inline uint8_t desc59n_get_subtitlingtype(const uint8_t *p_desc_n)
{
    return p_desc_n[3];
}

static inline void desc59n_set_compositionpage(uint8_t *p_desc_n,
                                               uint16_t i_page)
{
    p_desc_n[4] = i_page >> 8;
    p_desc_n[5] = i_page & 0xff;
}

static inline uint16_t desc59n_get_compositionpage(const uint8_t *p_desc_n)
{
    return (p_desc_n[4] << 8) | p_desc_n[5];
}

static inline void desc59n_set_ancillarypage(uint8_t *p_desc_n, uint16_t i_page)
{
    p_desc_n[6] = i_page >> 8;
    p_desc_n[7] = i_page & 0xff;
}

static inline uint16_t desc59n_get_ancillarypage(const uint8_t *p_desc_n)
{
    return (p_desc_n[6] << 8) | p_desc_n[7];
}

static inline void desc59_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc59_get_language(p_desc, j)) != NULL) {
        j++;
        pf_print(opaque,
        "    - desc 59 dvbs language=%3.3s type=%hhu composition=%hu ancillary=%hu",
        (const char *)desc59n_get_code(p_desc_n),
        desc59n_get_subtitlingtype(p_desc_n),
        desc59n_get_compositionpage(p_desc_n),
        desc59n_get_ancillarypage(p_desc_n));
    }
}

/*****************************************************************************
 * Descriptor 0x6a: AC-3 descriptor
 *****************************************************************************/
#define DESC6A_HEADER_SIZE      3

static inline void desc6a_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x6a);
}

static inline void desc6a_clear_flags(uint8_t *p_desc)
{
    p_desc[2] = 0;
}

static inline void desc6a_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque)
{
    pf_print(opaque, "    - desc 6a ac3");
}

/*****************************************************************************
 * Network Information Table
 *****************************************************************************/
#define NIT_PID                 0x10
#define NIT_TABLE_ID_ACTUAL     0x40
#define NIT_TABLE_ID_OTHER      0x41
#define NIT_HEADER_SIZE         (PSI_HEADER_SIZE_SYNTAX1 + 2)
#define NIT_HEADER2_SIZE        2
#define NIT_TS_SIZE             6

#define nit_set_nid psi_set_tableidext
#define nit_get_nid psi_get_tableidext

static inline void nit_init(uint8_t *p_nit, bool b_actual)
{
    psi_init(p_nit, true);
    psi_set_tableid(p_nit, b_actual ? NIT_TABLE_ID_ACTUAL : NIT_TABLE_ID_OTHER);
    p_nit[8] = 0xf0;
}

static inline void nit_set_length(uint8_t *p_nit, uint16_t i_nit_length)
{
    psi_set_length(p_nit, NIT_HEADER_SIZE + PSI_CRC_SIZE - PSI_HEADER_SIZE
                    + i_nit_length);
}

static inline void nit_set_desclength(uint8_t *p_nit, uint16_t i_length)
{
    p_nit[8] &= ~0xf;
    p_nit[8] |= i_length >> 8;
    p_nit[9] = i_length & 0xff;
}

static inline uint16_t nit_get_desclength(const uint8_t *p_nit)
{
    return ((p_nit[8] & 0xf) << 8) | p_nit[9];
}

static inline uint8_t *nit_get_descs(uint8_t *p_nit)
{
    return &p_nit[8];
}

static inline void nith_init(uint8_t *p_nit_h)
{
    p_nit_h[0] = 0xf0;
}

static inline void nith_set_tslength(uint8_t *p_nit_h, uint16_t i_length)
{
    p_nit_h[0] &= ~0xf;
    p_nit_h[0] |= i_length >> 8;
    p_nit_h[1] = i_length & 0xff;
}

static inline uint16_t nith_get_tslength(const uint8_t *p_nit_h)
{
    return ((p_nit_h[0] & 0xf) << 8) | p_nit_h[1];
}

static inline void nitn_init(uint8_t *p_nit_n)
{
    p_nit_n[4] = 0xf0;
}

static inline void nitn_set_tsid(uint8_t *p_nit_n, uint16_t i_tsid)
{
    p_nit_n[0] = i_tsid >> 8;
    p_nit_n[1] = i_tsid & 0xff;
}

static inline uint8_t nitn_get_tsid(const uint8_t *p_nit_n)
{
    return (p_nit_n[0] << 8) | p_nit_n[1];
}

static inline void nitn_set_onid(uint8_t *p_nit_n, uint16_t i_onid)
{
    p_nit_n[2] = i_onid >> 8;
    p_nit_n[3] = i_onid & 0xff;
}

static inline uint8_t nitn_get_onid(const uint8_t *p_nit_n)
{
    return (p_nit_n[2] << 8) | p_nit_n[3];
}

static inline void nitn_set_desclength(uint8_t *p_nit_n, uint16_t i_length)
{
    p_nit_n[4] &= ~0xf;
    p_nit_n[4] |= i_length >> 8;
    p_nit_n[5] = i_length & 0xff;
}

static inline uint16_t nitn_get_desclength(const uint8_t *p_nit_n)
{
    return ((p_nit_n[4] & 0xf) << 8) | p_nit_n[5];
}

static inline uint8_t *nitn_get_descs(uint8_t *p_nit_n)
{
    return &p_nit_n[4];
}

static inline uint8_t *nit_get_header2(uint8_t *p_nit)
{
    return p_nit + NIT_HEADER_SIZE + nit_get_desclength(p_nit);
}

static inline uint8_t *nit_get_ts(uint8_t *p_nit, uint8_t n)
{
    uint16_t i_section_size = psi_get_length(p_nit) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    uint8_t *p_nit_n = p_nit + NIT_HEADER_SIZE + nit_get_desclength(p_nit)
                        + NIT_HEADER2_SIZE;
    if (p_nit_n - p_nit > i_section_size) return NULL;

    while (n) {
        if (p_nit_n + NIT_TS_SIZE - p_nit > i_section_size) return NULL;
        p_nit_n += NIT_TS_SIZE + nitn_get_desclength(p_nit_n);
        n--;
    }
    if (p_nit_n - p_nit >= i_section_size) return NULL;
    return p_nit_n;
}

static inline bool nit_validate(const uint8_t *p_nit)
{
    uint16_t i_section_size = psi_get_length(p_nit) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    const uint8_t *p_nit_n;

    if (!psi_get_syntax(p_nit)
         || (psi_get_tableid(p_nit) != NIT_TABLE_ID_ACTUAL
              && psi_get_tableid(p_nit) != NIT_TABLE_ID_OTHER))
        return false;

    if (i_section_size < NIT_HEADER_SIZE
         || i_section_size < NIT_HEADER_SIZE + nit_get_desclength(p_nit))
        return false;

    if (!descs_validate(p_nit + 8))
        return false;

    p_nit_n = p_nit + NIT_HEADER_SIZE + nit_get_desclength(p_nit);

    if (nith_get_tslength(p_nit_n) != p_nit + i_section_size - p_nit_n
         - NIT_HEADER2_SIZE)
        return false;
    p_nit_n += NIT_HEADER2_SIZE;

    while (p_nit_n + NIT_TS_SIZE - p_nit <= i_section_size
            && p_nit_n + NIT_TS_SIZE + nitn_get_desclength(p_nit_n) - p_nit
                <= i_section_size) {
        if (!descs_validate(p_nit_n + 4))
            return false;

        p_nit_n += NIT_TS_SIZE + nitn_get_desclength(p_nit_n);
    }

    return (p_nit_n - p_nit == i_section_size);
}


static inline uint8_t *nit_table_find_ts(uint8_t **pp_sections,
                                         uint16_t i_tsid, uint16_t i_onid)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);
        uint8_t *p_ts;
        int j = 0;

        while ((p_ts = nit_get_ts(p_section, j)) != NULL) {
            j++;
            if (nitn_get_tsid(p_ts) == i_tsid && nitn_get_onid(p_ts) == i_onid)
                return p_ts;
        }
    }

    return NULL;
}

static inline bool nit_table_validate(uint8_t **pp_sections)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);
        uint8_t *p_ts;
        int j = 0;

        if (!psi_check_crc(p_section))
            return false;

        while ((p_ts = nit_get_ts(p_section, j)) != NULL) {
            j++;
            /* check that the program number if not already in the table */
            if (nit_table_find_ts(pp_sections, nitn_get_tsid(p_ts),
                                  nitn_get_onid(p_ts)) != p_ts)
                return false;
        }
    }

    return true;
}

/*****************************************************************************
 * Service Description Table
 *****************************************************************************/
#define SDT_PID                 0x11
#define SDT_TABLE_ID_ACTUAL     0x42
#define SDT_TABLE_ID_OTHER      0x46
#define SDT_HEADER_SIZE         (PSI_HEADER_SIZE_SYNTAX1 + 3)
#define SDT_SERVICE_SIZE        5

#define sdt_set_tsid psi_set_tableidext
#define sdt_get_tsid psi_get_tableidext

static inline void sdt_init(uint8_t *p_sdt, bool b_actual)
{
    psi_init(p_sdt, true);
    psi_set_tableid(p_sdt, b_actual ? SDT_TABLE_ID_ACTUAL : SDT_TABLE_ID_OTHER);
    p_sdt[10] = 0xff;
}

static inline void sdt_set_length(uint8_t *p_sdt, uint16_t i_sdt_length)
{
    psi_set_length(p_sdt, SDT_HEADER_SIZE + PSI_CRC_SIZE - PSI_HEADER_SIZE
                    + i_sdt_length);
}

static inline void sdt_set_onid(uint8_t *p_sdt, uint16_t i_onid)
{
    p_sdt[8] = i_onid >> 8;
    p_sdt[9] = i_onid & 0xff;
}

static inline uint16_t sdt_get_onid(const uint8_t *p_sdt)
{
    return (p_sdt[8] << 8) | p_sdt[9];
}

static inline void sdtn_init(uint8_t *p_sdt_n)
{
    p_sdt_n[2] = 0xfc;
}

static inline void sdtn_set_sid(uint8_t *p_sdt_n, uint16_t i_sid)
{
    p_sdt_n[0] = i_sid >> 8;
    p_sdt_n[1] = i_sid & 0xff;
}

static inline uint16_t sdtn_get_sid(const uint8_t *p_sdt_n)
{
    return (p_sdt_n[0] << 8) | p_sdt_n[1];
}

static inline void sdtn_set_eitschedule(uint8_t *p_sdt_n)
{
    p_sdt_n[2] |= 0x2;
}

static inline bool sdtn_get_eitschedule(const uint8_t *p_sdt_n)
{
    return p_sdt_n[2] & 0x2;
}

static inline void sdtn_set_eitpresent(uint8_t *p_sdt_n)
{
    p_sdt_n[2] |= 0x1;
}

static inline bool sdtn_get_eitpresent(const uint8_t *p_sdt_n)
{
    return p_sdt_n[2] & 0x1;
}

static inline void sdtn_set_running(uint8_t *p_sdt_n, uint8_t i_running)
{
    p_sdt_n[3] &= 0x1f;
    p_sdt_n[3] |= i_running << 5;
}

static inline uint8_t sdtn_get_running(const uint8_t *p_sdt_n)
{
    return p_sdt_n[3] >> 5;
}

static inline void sdtn_set_ca(uint8_t *p_sdt_n)
{
    p_sdt_n[3] |= 0x10;
}

static inline bool sdtn_get_ca(const uint8_t *p_sdt_n)
{
    return p_sdt_n[3] & 0x10;
}

static inline void sdtn_set_desclength(uint8_t *p_sdt_n, uint16_t i_length)
{
    p_sdt_n[3] &= ~0xf;
    p_sdt_n[3] |= (i_length >> 8) & 0xf;
    p_sdt_n[4] = i_length & 0xff;
}

static inline uint16_t sdtn_get_desclength(const uint8_t *p_sdt_n)
{
    return ((p_sdt_n[3] & 0xf) << 8) | p_sdt_n[4];
}

static inline uint8_t *sdtn_get_descs(uint8_t *p_sdt_n)
{
    return &p_sdt_n[3];
}

static inline uint8_t *sdt_get_service(uint8_t *p_sdt, uint8_t n)
{
    uint16_t i_section_size = psi_get_length(p_sdt) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    uint8_t *p_sdt_n = p_sdt + SDT_HEADER_SIZE;

    while (n) {
        if (p_sdt_n + SDT_SERVICE_SIZE - p_sdt > i_section_size) return NULL;
        p_sdt_n += SDT_SERVICE_SIZE + sdtn_get_desclength(p_sdt_n);
        n--;
    }
    if (p_sdt_n - p_sdt >= i_section_size) return NULL;
    return p_sdt_n;
}

static inline bool sdt_validate(const uint8_t *p_sdt)
{
    uint16_t i_section_size = psi_get_length(p_sdt) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    const uint8_t *p_sdt_n;

    if (!psi_get_syntax(p_sdt)
         || (psi_get_tableid(p_sdt) != SDT_TABLE_ID_ACTUAL
              && psi_get_tableid(p_sdt) != SDT_TABLE_ID_OTHER))
        return false;

    p_sdt_n = p_sdt + SDT_HEADER_SIZE;

    while (p_sdt_n + SDT_SERVICE_SIZE - p_sdt <= i_section_size
            && p_sdt_n + SDT_SERVICE_SIZE + sdtn_get_desclength(p_sdt_n) - p_sdt
                <= i_section_size) {
        if (!descs_validate(p_sdt_n + 3))
            return false;

        p_sdt_n += SDT_SERVICE_SIZE + sdtn_get_desclength(p_sdt_n);
    }

    return (p_sdt_n - p_sdt == i_section_size);
}

static inline uint8_t *sdt_table_find_service(uint8_t **pp_sections,
                                              uint16_t i_sid)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);
        uint8_t *p_service;
        int j = 0;

        while ((p_service = sdt_get_service(p_section, j)) != NULL) {
            if (sdtn_get_sid(p_service) == i_sid)
                return p_service;
            j++;
        }
    }

    return NULL;
}

static inline bool sdt_table_validate(uint8_t **pp_sections)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);
        uint8_t *p_ts;
        int j = 0;

        if (!psi_check_crc(p_section))
            return false;
    }

    return true;
}

/*****************************************************************************
 * Event Information Table
 *****************************************************************************/
#define EIT_PID                         0x12
#define EIT_TABLE_ID_PF_ACTUAL          0x4e
#define EIT_TABLE_ID_PF_OTHER           0x4f
#define EIT_TABLE_ID_SCHED_ACTUAL_FIRST 0x50
#define EIT_TABLE_ID_SCHED_ACTUAL_LAST  0x5f
#define EIT_TABLE_ID_SCHED_OTHER_FIRST  0x60
#define EIT_TABLE_ID_SCHED_OTHER_LAST   0x6f
#define EIT_HEADER_SIZE                 (PSI_HEADER_SIZE_SYNTAX1 + 6)
#define EIT_EVENT_SIZE                  12

#define eit_set_sid psi_set_tableidext
#define eit_get_sid psi_get_tableidext

static inline void eit_init(uint8_t *p_eit, bool b_actual)
{
    psi_init(p_eit, true);
    psi_set_tableid(p_eit, b_actual ? EIT_TABLE_ID_PF_ACTUAL :
                    EIT_TABLE_ID_PF_OTHER);
}

static inline void eit_set_length(uint8_t *p_eit, uint16_t i_eit_length)
{
    psi_set_length(p_eit, EIT_HEADER_SIZE + PSI_CRC_SIZE - PSI_HEADER_SIZE
                    + i_eit_length);
}

static inline void eit_set_tsid(uint8_t *p_eit, uint16_t i_tsid)
{
    p_eit[8] = i_tsid >> 8;
    p_eit[9] = i_tsid & 0xff;
}

static inline uint16_t eit_get_tsid(const uint8_t *p_eit)
{
    return (p_eit[8] << 8) | p_eit[9];
}

static inline uint16_t eitn_get_desclength(const uint8_t *p_eit_n)
{
    return ((p_eit_n[10] & 0xf) << 8) | p_eit_n[11];
}

static inline bool eit_validate(const uint8_t *p_eit)
{
    uint16_t i_section_size = psi_get_length(p_eit) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    uint8_t i_tid = psi_get_tableid(p_eit);
    const uint8_t *p_eit_n;

    if (!psi_get_syntax(p_eit)
         || (i_tid != EIT_TABLE_ID_PF_ACTUAL
              && i_tid != EIT_TABLE_ID_PF_OTHER
              && !(i_tid >= EIT_TABLE_ID_SCHED_ACTUAL_FIRST
                    && i_tid <= EIT_TABLE_ID_SCHED_ACTUAL_LAST)
              && !(i_tid >= EIT_TABLE_ID_SCHED_OTHER_FIRST
                    && i_tid <= EIT_TABLE_ID_SCHED_OTHER_LAST)))
        return false;

    if (!psi_check_crc(p_eit))
        return false;

    p_eit_n = p_eit + EIT_HEADER_SIZE;

    while (p_eit_n + EIT_EVENT_SIZE - p_eit <= i_section_size
            && p_eit_n + EIT_EVENT_SIZE + eitn_get_desclength(p_eit_n) - p_eit
                <= i_section_size) {
        if (!descs_validate(p_eit_n + 10))
            return false;

        p_eit_n += EIT_EVENT_SIZE + eitn_get_desclength(p_eit_n);
    }

    return (p_eit_n - p_eit == i_section_size);
}

/* TODO: unfinished support */

/*****************************************************************************
 * Running Status Table
 *****************************************************************************/
#define RST_PID                 0x13
#define RST_TABLE_ID            0x71
#define RST_HEADER_SIZE         PSI_HEADER_SIZE
#define RST_EVENT_SIZE          9

/* TODO: unfinished support */

/*****************************************************************************
 * Time and Date Table
 *****************************************************************************/
#define TDT_PID                 0x14
#define TDT_TABLE_ID            0x70
#define TDT_HEADER_SIZE         (PSI_HEADER_SIZE + 5)

static inline void tdt_init(uint8_t *p_tdt)
{
    psi_init(p_tdt, false);
    psi_set_tableid(p_tdt, TDT_TABLE_ID);
    psi_set_length(p_tdt, TDT_HEADER_SIZE - PSI_HEADER_SIZE);
}

static inline void tdt_set_utc(uint8_t *p_tdt, uint64_t i_utc)
{
    p_tdt[3] = (i_utc >> 32) & 0xff;
    p_tdt[4] = (i_utc >> 24) & 0xff;
    p_tdt[5] = (i_utc >> 16) & 0xff;
    p_tdt[6] = (i_utc >> 8) & 0xff;
    p_tdt[7] = i_utc & 0xff;
}

static inline uint64_t tdt_get_utc(const uint8_t *p_tdt)
{
    return ((uint64_t)p_tdt[3] << 32) | (p_tdt[4] << 24) | (p_tdt[5] << 16)
             | (p_tdt[6] << 8) | p_tdt[7];
}

static inline bool tdt_validate(const uint8_t *p_tdt)
{
    uint16_t i_section_size = psi_get_length(p_tdt) + PSI_HEADER_SIZE;
    uint8_t i_tid = psi_get_tableid(p_tdt);

    if (psi_get_syntax(p_tdt) || i_tid != TDT_TABLE_ID
         || i_section_size < TDT_HEADER_SIZE)
        return false;

    return true;
}

/*****************************************************************************
 * Time Offset Table
 *****************************************************************************/
#define TOT_PID                 0x14
#define TOT_TABLE_ID            0x73
#define TOT_HEADER_SIZE         (PSI_HEADER_SIZE + 7)

#define tot_set_utc tdt_set_utc
#define tot_get_utc tdt_get_utc

static inline void tot_init(uint8_t *p_tot)
{
    psi_init(p_tot, false);
    psi_set_tableid(p_tot, TOT_TABLE_ID);
    p_tot[8] = 0xf0;
}

static inline void tot_set_desclength(uint8_t *p_tot, uint16_t i_length)
{
    p_tot[8] &= ~0xf;
    p_tot[8] |= i_length >> 8;
    p_tot[9] = i_length & 0xff;
}

static inline uint16_t tot_get_desclength(const uint8_t *p_tot)
{
    return ((p_tot[8] & 0xf) << 8) | p_tot[9];
}

static inline bool tot_validate(const uint8_t *p_tot)
{
    uint16_t i_section_size = psi_get_length(p_tot) + PSI_HEADER_SIZE;
    uint8_t i_tid = psi_get_tableid(p_tot);

    if (psi_get_syntax(p_tot) || i_tid != TOT_TABLE_ID
         || i_section_size < TOT_HEADER_SIZE + PSI_CRC_SIZE)
        return false;

    /* TOT is a syntax0 table with CRC */
    if (!psi_check_crc(p_tot))
        return false;

    if (i_section_size < TOT_HEADER_SIZE
         || i_section_size < TOT_HEADER_SIZE + tot_get_desclength(p_tot))
        return false;

    if (!descs_validate(p_tot + 8))
        return false;

    return true;
}

#ifdef __cplusplus
}
#endif

#endif
