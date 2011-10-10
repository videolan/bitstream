/*****************************************************************************
 * rst.h: ETSI EN 300 468 Running Status Table (RST)
 *****************************************************************************
 * Copyright (C) 2010 VideoLAN
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *          Georgi Chorbadzhiyski <georgi@unixsol.org>
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

#ifndef __BITSTREAM_DVB_RST_H__
#define __BITSTREAM_DVB_RST_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Running Status Table
 *****************************************************************************/
#define RST_PID                 0x13
#define RST_TABLE_ID            0x71
#define RST_HEADER_SIZE         PSI_HEADER_SIZE
#define RST_STATUS_SIZE         9

static inline void rst_init(uint8_t *p_rst)
{
    psi_set_tableid(p_rst, RST_TABLE_ID);
    psi_init(p_rst, false);
}

static inline void rst_set_length(uint8_t *p_rst, uint16_t i_rst_length)
{
    psi_set_length(p_rst, i_rst_length & 0x3fff);
}

static inline void rstn_init(uint8_t *p_rst_n)
{
    p_rst_n[8] = 0xf8;
}

static inline uint16_t rstn_get_tsid(const uint8_t *p_rst_n)
{
    return (p_rst_n[0] << 8) | p_rst_n[1];
}

static inline void rstn_set_tsid(uint8_t *p_rst_n, uint16_t i_tsid)
{
    p_rst_n[0] = i_tsid >> 8;
    p_rst_n[1] = i_tsid & 0xff;
}

static inline uint16_t rstn_get_onid(const uint8_t *p_rst_n)
{
    return (p_rst_n[2] << 8) | p_rst_n[3];
}

static inline void rstn_set_onid(uint8_t *p_rst_n, uint16_t i_onid)
{
    p_rst_n[2] = i_onid >> 8;
    p_rst_n[3] = i_onid & 0xff;
}

static inline uint16_t rstn_get_service_id(const uint8_t *p_rst_n)
{
    return (p_rst_n[4] << 8) | p_rst_n[5];
}

static inline void rstn_set_service_id(uint8_t *p_rst_n, uint16_t i_service_id)
{
    p_rst_n[4] = i_service_id >> 8;
    p_rst_n[5] = i_service_id & 0xff;
}

static inline uint16_t rstn_get_event_id(const uint8_t *p_rst_n)
{
    return (p_rst_n[6] << 8) | p_rst_n[7];
}

static inline void rstn_set_event_id(uint8_t *p_rst_n, uint16_t i_event_id)
{
    p_rst_n[6] = i_event_id >> 8;
    p_rst_n[7] = i_event_id & 0xff;
}

static inline uint8_t rstn_get_running_status(const uint8_t *p_rst_n)
{
    return p_rst_n[8] & 0x07;
}

static inline void rstn_set_running_status(uint8_t *p_rst_n, uint8_t i_running_status)
{
    p_rst_n[8] = 0xf8 | (i_running_status & 0x07);
}

static inline uint8_t *rst_get_status(uint8_t *p_rst, uint8_t n)
{
    uint8_t *p_rst_n = p_rst + RST_HEADER_SIZE + n * RST_STATUS_SIZE;
    if (p_rst_n + RST_STATUS_SIZE - p_rst
         > psi_get_length(p_rst) + PSI_HEADER_SIZE)
        return NULL;
    return p_rst_n;
}

static inline bool rst_validate(const uint8_t *p_rst)
{
    if (psi_get_syntax(p_rst) || psi_get_tableid(p_rst) != RST_TABLE_ID)
        return false;
    if (psi_get_length(p_rst) % RST_STATUS_SIZE)
        return false;

    return true;
}

#ifdef __cplusplus
}
#endif

#endif
