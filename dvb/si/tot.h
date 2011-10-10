/*****************************************************************************
 * tot.h: ETSI EN 300 468 Time Offset Table (TOT)
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
 *  - ETSI EN 300 468 V1.11.1 (2010-04) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_TOT_H__
#define __BITSTREAM_DVB_TOT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/dvb/si/tdt.h>

#ifdef __cplusplus
extern "C"
{
#endif

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

static inline void tot_set_length(uint8_t *p_tot, uint16_t i_tot_length)
{
    psi_set_length(p_tot, TOT_HEADER_SIZE + PSI_CRC_SIZE - PSI_HEADER_SIZE
                    + i_tot_length);
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

static inline uint8_t *tot_get_descs(uint8_t *p_tot)
{
    return &p_tot[8];
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
