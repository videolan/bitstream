/*****************************************************************************
 * st.h: ETSI EN 300 468 Stuffing Table (ST)
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

#ifndef __BITSTREAM_DVB_ST_H__
#define __BITSTREAM_DVB_ST_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Stuffing Table
 *****************************************************************************/
#define ST_TABLE_ID            0x72
#define ST_HEADER_SIZE         PSI_HEADER_SIZE

static inline void st_init(uint8_t *p_st)
{
    psi_init(p_st, false);
    psi_set_tableid(p_st, ST_TABLE_ID);
    psi_set_length(p_st, 0);
}

static inline bool st_validate(const uint8_t *p_st)
{
    if (psi_get_tableid(p_st) != ST_TABLE_ID)
        return false;

    return true;
}

#ifdef __cplusplus
}
#endif

#endif
