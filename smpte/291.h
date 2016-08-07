/*****************************************************************************
 * 291.h: SMPTE 291 Ancillary Data Packet and Space Formatting
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
 *  - SMPTE 291
 *  - SMPTE RP-291
 */

#ifndef __BITSTREAM_SMPTE_291_H__
#define __BITSTREAM_SMPTE_291_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * SMPTE 291 ancillary data (8-bit mode only)
 *****************************************************************************/
#define S291_HEADER_SIZE            6
#define S291_FOOTER_SIZE            1

#define S291_ADF1                   0x00
#define S291_ADF2                   0xff
#define S291_ADF3                   0xff

/* SMPTE 2016 */
#define S291_AFD_DID                0x41
#define S291_AFD_SDID               0x05
#define S291_PAN_SCAN_DID           0x41
#define S291_PAN_SCAN_SDID          0x06

/* SMPTE 2010 */
#define S291_SCTE104_DID            0x41
#define S291_SCTE104_SDID           0x07

/* OP-47 / SMPTE RDD-8 */
#define S291_OP47SDP_DID            0x43
#define S291_OP47SDP_SDID           0x02
#define S291_OP47MP_DID             0x43
#define S291_OP47MP_SDID            0x03

/* SMPTE 12M */
#define S291_ATC_DID                0x60
#define S291_ATC_SDID               0x60

/* SMPTE 334 */
#define S291_CEA708_DID             0x61
#define S291_CEA708_SDID            0x01
#define S291_CEA608_DID             0x61
#define S291_CEA608_SDID            0x02

/* SMPTE RP-207 */
#define S291_PROGDESC_DID           0x62
#define S291_PROGDESC_SDID          0x01

/* SMPTE 334 */
#define S291_DATABROADCAST_DID      0x62
#define S291_DATABROADCAST_SDID     0x02

/* SMPTE RP-208 (legacy) */
#define S291_SMPTEVBI_DID           0x62
#define S291_SMPTEVBI_SDID          0x03

static inline void s291_set_did(uint8_t *p_s291, uint8_t i_did)
{
    p_s291[3] = i_did;
}

static inline uint8_t s291_get_did(const uint8_t *p_s291)
{
    return p_s291[3];
}

static inline void s291_set_sdid(uint8_t *p_s291, uint8_t i_sdid)
{
    p_s291[4] = i_sdid;
}

static inline uint8_t s291_get_sdid(const uint8_t *p_s291)
{
    return p_s291[4];
}

static inline void s291_set_data_mode(uint8_t *p_s291, uint8_t i_data_mode)
{
    p_s291[5] &= ~0x60;
    p_s291[5] |= (i_data_mode << 5) & 0x60;
}

#define s291_set_dbn s291_set_sdid
#define s291_get_dbn s291_get_sdid

static inline void s291_set_dc(uint8_t *p_s291, uint8_t i_dc)
{
    p_s291[5] = i_dc;
}

static inline uint8_t s291_get_dc(const uint8_t *p_s291)
{
    return p_s291[5];
}

static inline uint8_t *s291_get_udw(const uint8_t *p_s291)
{
    return (uint8_t *)&p_s291[6];
}

static inline bool s291_check_crc(const uint8_t *p_s291)
{
    uint8_t i_crc = 0;
    uint8_t i_dc = s291_get_dc(p_s291);
    int i;
    for (i = 3; i < i_dc + S291_HEADER_SIZE; i++)
        i_crc += p_s291[i];
    return i_crc == p_s291[i];
}

#ifdef __cplusplus
}
#endif

#endif
