/*****************************************************************************
 * 2010.h: SMPTE 2010 VANC mapping of SCTE 104
 *****************************************************************************
 * Copyright (C) 2016 OpenHeadend
 *
 * Authors: Christophe Massiot <cmassiot@openheadend.tv>
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
 *  - SMPTE 2010
 */

#ifndef __BITSTREAM_SMPTE_2010_H__
#define __BITSTREAM_SMPTE_2010_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * SMPTE 2010 ancillary data
 *****************************************************************************/
#define S2010_HEADER_SIZE       1

#define S2010_VERSION           1

static inline uint8_t s2010_get_version(const uint16_t *p_s2010)
{
    return (p_s2010[0] & 0x18) >> 3;
}

static inline bool s2010_get_continued(const uint16_t *p_s2010)
{
    return p_s2010[0] & 0x04;
}

static inline bool s2010_get_following(const uint16_t *p_s2010)
{
    return p_s2010[0] & 0x02;
}

static inline bool s2010_get_duplicate(const uint16_t *p_s2010)
{
    return p_s2010[0] & 0x01;
}

#ifdef __cplusplus
}
#endif

#endif
