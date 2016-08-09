/*****************************************************************************
 * vbi.h: ETSI EN 301 775 Carriage of VBI in DVB
 *****************************************************************************
 * Copyright (C) 2016 OpenHeadend
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
 *  - ETSI EN 300 775 V1.2.1 (2003-05) (carriage of VBI in DVB)
 */

#ifndef __BITSTREAM_DVB_VBI_H__
#define __BITSTREAM_DVB_VBI_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * PES data field
 *****************************************************************************/
#define DVBVBI_HEADER_SIZE      1
#define DVBVBI_UNIT_HEADER_SIZE 2
#define DVBVBI_DATA_IDENTIFIER  0x10

#define DVBVBI_ID_TTX_NONSUB    0x02
#define DVBVBI_ID_TTX_SUB       0x03
#define DVBVBI_ID_TTX_INVERTED  0xc0
#define DVBVBI_ID_VPS           0xc3
#define DVBVBI_ID_WSS           0xc4
#define DVBVBI_ID_CEA608        0xc5
#define DVBVBI_ID_RAWVBI        0xc6
#define DVBVBI_ID_STUFFING      0xff

#define DVBVBI_LENGTH           0x2c

/*****************************************************************************
 * Teletext data field
 *****************************************************************************/
#define DVBVBITTX_FRAMING_CODE  0xe4

static inline void dvbvbittx_set_field(uint8_t *p, bool field)
{
    p[0] |= 0xc0;
    if (field)
        p[0] |= 0x20;
    else
        p[0] &= ~0x20;
}

static inline bool dvbvbittx_get_field(const uint8_t *p)
{
    return p[0] & 0x20;
}

static inline void dvbvbittx_set_line(uint8_t *p, uint8_t line)
{
    p[0] &= ~0x1f;
    p[0] |= 0xc0 | (line & 0x1f);
}

static inline uint8_t dvbvbittx_get_line(const uint8_t *p)
{
    return p[0] & 0x1f;
}

#ifdef __cplusplus
}
#endif

#endif
