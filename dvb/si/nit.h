/*****************************************************************************
 * nit.h: ETSI EN 300 468 Network Information Table (NIT)
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

#ifndef __BITSTREAM_DVB_NIT_H__
#define __BITSTREAM_DVB_NIT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

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

static inline uint16_t nitn_get_tsid(const uint8_t *p_nit_n)
{
    return (p_nit_n[0] << 8) | p_nit_n[1];
}

static inline void nitn_set_onid(uint8_t *p_nit_n, uint16_t i_onid)
{
    p_nit_n[2] = i_onid >> 8;
    p_nit_n[3] = i_onid & 0xff;
}

static inline uint16_t nitn_get_onid(const uint8_t *p_nit_n)
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

static inline bool nit_validate_ts(const uint8_t *p_nit, const uint8_t *p_nit_n,
                                   uint16_t i_desclength)
{
    uint16_t i_section_size = psi_get_length(p_nit) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    return (p_nit_n + NIT_TS_SIZE + i_desclength
             <= p_nit + i_section_size);
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
            /* check that the TS is not already in the table */
            if (nit_table_find_ts(pp_sections, nitn_get_tsid(p_ts),
                                  nitn_get_onid(p_ts)) != p_ts)
                return false;
        }
    }

    return true;
}

#ifdef __cplusplus
}
#endif

#endif
