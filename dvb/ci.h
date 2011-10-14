/*****************************************************************************
 * ci.h: ETSI EN 50 221 Common Interface Specification
 *****************************************************************************
 * Copyright (C) 2010 VideoLAN
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
 *  - ETSI EN 50 221 (1997) (Common Interface Specification)
 */

#ifndef __BITSTREAM_DVB_CI_H__
#define __BITSTREAM_DVB_CI_H__

#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Conditional Access Program Map Table
 *****************************************************************************
 * Implementation note: the first field here is ca_pmt_list_management,
 * since the previous length field is variable size and is generally put
 * afterwards.
 *****************************************************************************/
#define CAPMT_HEADER_SIZE       6
#define CAPMT_ES_SIZE           5
#define CAPMTI_HEADER_SIZE      3

/* Max theoritical size is PSI_MAX_SIZE + ~100 */
#define CAPMT_DECLARE PSI_PRIVATE_DECLARE
#define capmt_allocate psi_private_allocate

static inline void capmt_init(uint8_t *p_capmt)
{
    p_capmt[3] = 0xc1;
    p_capmt[4] = 0xf0;
}

static inline void capmt_set_listmanagement(uint8_t *p_capmt, uint8_t i_mgt)
{
    p_capmt[0] = i_mgt;
}

static inline void capmt_set_program(uint8_t *p_capmt, uint16_t i_program)
{
    p_capmt[1] = i_program >> 8;
    p_capmt[2] = i_program & 0xff;
}

static inline void capmt_set_version(uint8_t *p_capmt, uint8_t i_version)
{
    p_capmt[3] &= ~0x3e;
    p_capmt[3] |= i_version << 1;
}

static inline uint8_t *capmt_get_infos(uint8_t *p_capmt)
{
    return &p_capmt[4];
}

static inline uint16_t capmt_get_infoslength(const uint8_t *p_capmt)
{
    return ((p_capmt[4] & 0xf) << 8) | p_capmt[5];
}

static inline void capmtn_init(uint8_t *p_capmt_n)
{
    p_capmt_n[1] = 0xe0;
}

static inline void capmtn_set_streamtype(uint8_t *p_capmt_n,
                                         uint8_t i_stream_type)
{
    p_capmt_n[0] = i_stream_type;
}

static inline void capmtn_set_pid(uint8_t *p_capmt_n, uint16_t i_pid)
{
    p_capmt_n[1] &= ~0x1f;
    p_capmt_n[1] |= i_pid >> 8;
    p_capmt_n[2] = i_pid & 0xff;
}

static inline uint16_t capmtn_get_infoslength(const uint8_t *p_capmt_n)
{
    return ((p_capmt_n[3] & 0xf) << 8) | p_capmt_n[4];
}

static inline uint8_t *capmtn_get_infos(uint8_t *p_capmt_n)
{
    return &p_capmt_n[3];
}

static inline uint8_t *capmt_get_es(uint8_t *p_capmt, uint8_t n)
{
    uint8_t *p_capmt_n = p_capmt + CAPMT_HEADER_SIZE
                          + capmt_get_infoslength(p_capmt);

    while (n) {
        p_capmt_n += CAPMT_ES_SIZE + capmtn_get_infoslength(p_capmt_n);
        n--;
    }
    return p_capmt_n;
}

static inline void capmti_init(uint8_t *p_infos)
{
    p_infos[0] = 0xf0;
}

static inline void capmti_set_length(uint8_t *p_infos, uint16_t i_length)
{
    p_infos[0] &= ~0xf;
    p_infos[0] |= i_length >> 8;
    p_infos[1] = i_length & 0xff;
}

static inline uint16_t capmti_get_length(const uint8_t *p_infos)
{
    return ((p_infos[0] & 0xf) << 8) | p_infos[1];
}

static inline void capmti_set_cmd(uint8_t *p_infos, uint8_t i_cmd)
{
    p_infos[2] = i_cmd;
}

static inline uint8_t *capmti_get_info(uint8_t *p_infos, uint16_t n)
{
    uint8_t *p_info = p_infos + CAPMTI_HEADER_SIZE;
    uint16_t i_infos_size = capmti_get_length(p_infos) + DESCS_HEADER_SIZE;

    while (n) {
        if (p_info + DESC_HEADER_SIZE - p_infos > i_infos_size) return NULL;
        p_info += DESC_HEADER_SIZE + desc_get_length(p_info);
        n--;
    }
    if (p_info - p_infos >= i_infos_size) return NULL;
    return p_info;
}

#ifdef __cplusplus
}
#endif

#endif
