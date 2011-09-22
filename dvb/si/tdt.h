/*****************************************************************************
 * tdt.h: ETSI EN 300 468 Time and Date Table (TDT)
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

#ifndef __BITSTREAM_DVB_TDT_H__
#define __BITSTREAM_DVB_TDT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>

#ifdef __cplusplus
extern "C"
{
#endif

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
    return (uint64_t)(((uint64_t)p_tdt[3] << 32) | ((uint64_t)p_tdt[4] << 24) |
                      ((uint64_t)p_tdt[5] << 16) | ((uint64_t)p_tdt[6] << 8) | p_tdt[7]);
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

#ifdef __cplusplus
}
#endif

#endif
