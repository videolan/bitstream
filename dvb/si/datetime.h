/*****************************************************************************
 * datetime.h: ETSI EN 300 468 Date and time functions
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

#include <bitstream/common.h>

/*
 * Normative references:
 *  - ETSI EN 300 468 V1.11.1 (2010-04) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_DATETIME_H__
#define __BITSTREAM_DVB_DATETIME_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * DVB Date and time functions
 *****************************************************************************/

/* EN 300 468 Annex C, year is from 1900 */
static inline uint16_t dvb_mjd_set(int y, int m, int d)
{
    int l = (m == 1 || m == 2) ? 1 : 0;
    int mjd = 14956 + d + (y - l) * 1461 / 4 + (m + 1 + l * 12) * 306001 / 10000;
    return (uint16_t)mjd;
}

static inline void dvb_mjd_get(uint16_t mjd, int *y, int *m, int *d)
{
    int k;
    int yp = (mjd * 20 - 301564) / 7305;
    int mp = (mjd * 10000 - 149561000 - yp * 3652500) / 306001;
    *d = mjd - 14956 - yp * 1461 / 4 - mp * 306001 / 10000;
    k = (mp == 14 || mp == 15) ? 1 : 0;
    *y = yp + k;
    *m = mp - 1 - k * 12;
}

#ifdef __cplusplus
}
#endif

#endif
