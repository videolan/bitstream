/*****************************************************************************
 * pmt.h: ISO/IEC 13818-1 Program Map Table (PMT)
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
 *  - ISO/IEC 13818-1:2007(E) (MPEG-2 Systems)
 */

#ifndef __BITSTREAM_MPEG_PMT_H__
#define __BITSTREAM_MPEG_PMT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

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
