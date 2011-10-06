/*****************************************************************************
 * tsdt.h: ISO/IEC 13818-1 Transport stream descriptor table (TSDT)
 *****************************************************************************
 * Copyright (C) 2011 Unix Solutions Ltd.
 *
 * Authors: Georgi Chorbadzhiyski <georgi@unixsol.org>
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

#ifndef __BITSTREAM_MPEG_TSDT_H__
#define __BITSTREAM_MPEG_TSDT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/mpeg/psi/cat.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Transport stream descriptor table
 *****************************************************************************/
#define TSDT_PID                 0x02
#define TSDT_TABLE_ID            0x03
#define TSDT_HEADER_SIZE         PSI_HEADER_SIZE_SYNTAX1

static inline void tsdt_init(uint8_t *p_tsdt)
{
    psi_init(p_tsdt, true);
    psi_set_tableid(p_tsdt, TSDT_TABLE_ID);
    p_tsdt[1] &= ~0x40;
    psi_set_tableidext(p_tsdt, 0xffff);
    psi_set_section(p_tsdt, 0);
    psi_set_lastsection(p_tsdt, 0);
}

#define tsdt_set_length         cat_set_length
#define tsdt_get_desclength     cat_get_desclength
#define tsdt_set_desclength     cat_set_desclength
#define tsdt_get_descl          cat_get_descl
#define tsdt_get_descl_const    cat_get_descl_const

static inline bool tsdt_validate(const uint8_t *p_tsdt)
{
    uint16_t i_section_size = psi_get_length(p_tsdt) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;

    if (!psi_get_syntax(p_tsdt) || psi_get_section(p_tsdt)
         || psi_get_lastsection(p_tsdt)
         || psi_get_tableid(p_tsdt) != TSDT_TABLE_ID)
        return false;

    if (i_section_size < TSDT_HEADER_SIZE
         || i_section_size < TSDT_HEADER_SIZE + tsdt_get_desclength(p_tsdt))
        return false;

    if (!descl_validate(tsdt_get_descl_const(p_tsdt), tsdt_get_desclength(p_tsdt)))
        return false;

    return true;
}

#define tsdt_table_validate cat_table_validate

#ifdef __cplusplus
}
#endif

#endif
