/*****************************************************************************
 * cat.h: ISO/IEC 13818-1 Conditional Access Table (CAT)
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

#ifndef __BITSTREAM_MPEG_CAT_H__
#define __BITSTREAM_MPEG_CAT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

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
    psi_set_tableidext(p_cat, 0xffff);
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

static inline void cat_set_desclength(uint8_t *p_cat, uint16_t i_desc_len)
{
    cat_set_length(p_cat, i_desc_len);
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

#ifdef __cplusplus
}
#endif

#endif
