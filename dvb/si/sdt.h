/*****************************************************************************
 * sdt.h: ETSI EN 300 468 Service Definition Table (SDT)
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
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
 *  - ETSI EN 300 468 V1.11.1 (2010-04) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_SDT_H__
#define __BITSTREAM_DVB_SDT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

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
    p_sdt_n[3] = 0;
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
    return !!(p_sdt_n[2] & 0x2);
}

static inline void sdtn_set_eitpresent(uint8_t *p_sdt_n)
{
    p_sdt_n[2] |= 0x1;
}

static inline bool sdtn_get_eitpresent(const uint8_t *p_sdt_n)
{
    return !!(p_sdt_n[2] & 0x1);
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
    return !!(p_sdt_n[3] & 0x10);
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

static inline bool sdt_validate_service(const uint8_t *p_sdt,
                                        const uint8_t *p_sdt_n,
                                        uint16_t i_desclength)
{
    uint16_t i_section_size = psi_get_length(p_sdt) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    return (p_sdt_n + SDT_SERVICE_SIZE + i_desclength
             <= p_sdt + i_section_size);
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
        uint8_t *p_service;
        int j = 0;

        if (!psi_check_crc(p_section))
            return false;

        while ((p_service = sdt_get_service(p_section, j)) != NULL) {
            j++;
            /* check that the service is not already in the table */
            if (sdt_table_find_service(pp_sections, sdtn_get_sid(p_service))
                 != p_service)
                return false;
        }
    }

    return true;
}

#ifdef __cplusplus
}
#endif

#endif
