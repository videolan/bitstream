/*****************************************************************************
 * sub.h: ETSI EN 300 743 Subtitling Systems
 *****************************************************************************
 * Copyright (C) 2014 VideoLAN
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
 *  - ETSI EN 300 743 V1.5.1 (2014-01) (Subtitling Systems)
 */

#ifndef __BITSTREAM_DVB_SUB_H__
#define __BITSTREAM_DVB_SUB_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * PES data field
 *****************************************************************************/
#define DVBSUB_HEADER_SIZE      2
#define DVBSUB_DATA_IDENTIFIER  0x20

/*****************************************************************************
 * Subtitling segment
 *****************************************************************************/
#define DVBSUBS_HEADER_SIZE     6
#define DVBSUBS_SYNC            0xf

#define DVBSUBS_PAGE_COMPOSITION        0x10
#define DVBSUBS_REGION_COMPOSITION      0x11
#define DVBSUBS_CLUT_DEFINITION         0x12
#define DVBSUBS_OBJECT_DATA             0x13
#define DVBSUBS_DISPLAY_DEFINITION      0x14
#define DVBSUBS_DISPARITY_SIGNALLING    0x15
#define DVBSUBS_END_OF_DISPLAY_SET      0x80

static inline uint8_t dvbsubs_get_type(const uint8_t *p_dvbsubs)
{
    return p_dvbsubs[1];
}

static inline uint16_t dvbsubs_get_page(const uint8_t *p_dvbsubs)
{
    return (p_dvbsubs[2] << 8) | p_dvbsubs[3];
}

static inline uint16_t dvbsubs_get_length(const uint8_t *p_dvbsubs)
{
    return (p_dvbsubs[4] << 8) | p_dvbsubs[5];
}

#ifdef __cplusplus
}
#endif

#endif
