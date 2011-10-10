/*****************************************************************************
 * dit.h: ETSI EN 300 468 Discontinuity Information Table (DIT)
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
 *  - ETSI EN 300 468 V1.11.1 (2010-04) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_DIT_H__
#define __BITSTREAM_DVB_DIT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Discontinuity Information Table
 *****************************************************************************/
#define DIT_PID                 0x1E
#define DIT_TABLE_ID            0x7E
#define DIT_HEADER_SIZE         PSI_HEADER_SIZE

static inline void dit_init(uint8_t *p_dit)
{
    psi_init(p_dit, false);
    psi_set_tableid(p_dit, DIT_TABLE_ID);
    psi_set_length(p_dit, 1);
}

static inline void dit_set_transition_flag(uint8_t *p_dit, bool b_transition_flag)
{
    p_dit[3] = b_transition_flag ? 0xff : 0x7f;
}

static inline bool dit_get_transition_flag(const uint8_t *p_dit)
{
    return p_dit[3] >> 7;
}

static inline bool dit_validate(const uint8_t *p_dit)
{
    uint16_t i_section_size = psi_get_length(p_dit) + PSI_HEADER_SIZE;
    uint8_t i_tid = psi_get_tableid(p_dit);

    if (psi_get_syntax(p_dit) || i_tid != DIT_TABLE_ID
         || i_section_size < DIT_HEADER_SIZE)
        return false;

    return true;
}

#ifdef __cplusplus
}
#endif

#endif
