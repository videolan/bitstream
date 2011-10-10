/*****************************************************************************
 * eit.h: ETSI EN 300 468 Event Information Table (EIT)
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
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

#ifndef __BITSTREAM_DVB_EIT_H__
#define __BITSTREAM_DVB_EIT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Event Information Table
 *****************************************************************************/
#define EIT_PID                         0x12
#define EIT_TABLE_ID_PF_ACTUAL          0x4e
#define EIT_TABLE_ID_PF_OTHER           0x4f
#define EIT_TABLE_ID_SCHED_ACTUAL_FIRST 0x50
#define EIT_TABLE_ID_SCHED_ACTUAL_LAST  0x5f
#define EIT_TABLE_ID_SCHED_OTHER_FIRST  0x60
#define EIT_TABLE_ID_SCHED_OTHER_LAST   0x6f
#define EIT_HEADER_SIZE                 (PSI_HEADER_SIZE_SYNTAX1 + 6)
#define EIT_EVENT_SIZE                  12

#define eit_set_sid psi_set_tableidext
#define eit_get_sid psi_get_tableidext

static inline void eit_init(uint8_t *p_eit, bool b_actual)
{
    psi_init(p_eit, true);
    psi_set_tableid(p_eit, b_actual ? EIT_TABLE_ID_PF_ACTUAL :
                    EIT_TABLE_ID_PF_OTHER);
}

static inline void eit_set_length(uint8_t *p_eit, uint16_t i_eit_length)
{
    psi_set_length(p_eit, EIT_HEADER_SIZE + PSI_CRC_SIZE - PSI_HEADER_SIZE
                    + i_eit_length);
}

static inline void eit_set_tsid(uint8_t *p_eit, uint16_t i_tsid)
{
    p_eit[8] = i_tsid >> 8;
    p_eit[9] = i_tsid & 0xff;
}

static inline uint16_t eit_get_tsid(const uint8_t *p_eit)
{
    return (p_eit[8] << 8) | p_eit[9];
}

static inline void eit_set_onid(uint8_t *p_eit, uint16_t i_onid)
{
    p_eit[10] = i_onid >> 8;
    p_eit[11] = i_onid & 0xff;
}

static inline uint16_t eit_get_onid(const uint8_t *p_eit)
{
    return (p_eit[10] << 8) | p_eit[11];
}

static inline void eit_set_segment_last_sec_number(uint8_t *p_eit, uint8_t i_segment_last_sec_number)
{
    p_eit[12] = i_segment_last_sec_number;
}

static inline uint8_t eit_get_segment_last_sec_number(const uint8_t *p_eit)
{
    return p_eit[12];
}

static inline void eit_set_last_table_id(uint8_t *p_eit, uint8_t i_last_table_id)
{
    p_eit[13] = i_last_table_id;
}

static inline uint8_t eit_get_last_table_id(const uint8_t *p_eit)
{
    return p_eit[13];
}

static inline uint16_t eitn_get_event_id(const uint8_t *p_eit_n)
{
    return (p_eit_n[0] << 8) | p_eit_n[1];
}

static inline void eitn_set_event_id(uint8_t *p_eit_n, uint16_t i_event_id)
{
    p_eit_n[0] = i_event_id >> 8;
    p_eit_n[1] = i_event_id & 0xff;
}

static inline uint64_t eitn_get_start_time(const uint8_t *p_eit_n)
{
    return (uint64_t)(((uint64_t)p_eit_n[2] << 32) | ((uint64_t)p_eit_n[3] << 24) |
                      ((uint64_t)p_eit_n[4] << 16) | ((uint64_t)p_eit_n[5] << 8) | p_eit_n[6]);
}

static inline void eitn_set_start_time(uint8_t *p_eit_n, uint64_t i_start_time)
{
    p_eit_n[2] = (i_start_time >> 32) & 0xff;
    p_eit_n[3] = (i_start_time >> 24) & 0xff;
    p_eit_n[4] = (i_start_time >> 16) & 0xff;
    p_eit_n[5] = (i_start_time >>  8) & 0xff;
    p_eit_n[6] = i_start_time         & 0xff;
}

static inline uint32_t eitn_get_duration_bcd(const uint8_t *p_eit_n)
{
    return ((p_eit_n[7] << 16) | (p_eit_n[8] << 8)) | p_eit_n[9];
}

static inline void eitn_set_duration_bcd(uint8_t *p_eit_n, uint32_t i_duration_bcd)
{
    p_eit_n[7] = (i_duration_bcd >> 16) & 0xff;
    p_eit_n[8] = (i_duration_bcd >>  8) & 0xff;
    p_eit_n[9] = i_duration_bcd         & 0xff;
}

static inline uint8_t eitn_get_running_status(const uint8_t *p_eit_n)
{
    return p_eit_n[10] >> 5;
}

static inline void eitn_set_running_status(uint8_t *p_eit_n, uint8_t i_running_status)
{
    p_eit_n[10] = (p_eit_n[10] & 0x1f) | (i_running_status << 5);
}

static inline bool eitn_get_free_CA_mode(const uint8_t *p_eit_n)
{
    return (p_eit_n[10] & 0x10) == 0x10;
}

static inline void eitn_set_free_CA_mode(uint8_t *p_eit_n, bool b_free_CA_mode)
{
    p_eit_n[10] = b_free_CA_mode ? (p_eit_n[10] | 0x10) : (p_eit_n[10] &~ 0x10);
}

static inline uint16_t eitn_get_desclength(const uint8_t *p_eit_n)
{
    return ((p_eit_n[10] & 0xf) << 8) | p_eit_n[11];
}

static inline void eitn_set_desclength(uint8_t *p_eit_n, uint16_t i_length)
{
    p_eit_n[10] &= ~0xf;
    p_eit_n[10] |= (i_length >> 8) & 0xf;
    p_eit_n[11] = i_length & 0xff;
}

static inline uint8_t *eitn_get_descs(uint8_t *p_eit_n)
{
    return &p_eit_n[10];
}

static inline uint8_t *eit_get_event(uint8_t *p_eit, uint8_t n)
{
    uint16_t i_section_size = psi_get_length(p_eit) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    uint8_t *p_eit_n = p_eit + EIT_HEADER_SIZE;
    if (p_eit_n - p_eit > i_section_size) return NULL;

    while (n) {
        if (p_eit_n + EIT_EVENT_SIZE - p_eit > i_section_size) return NULL;
        p_eit_n += EIT_EVENT_SIZE + eitn_get_desclength(p_eit_n);
        n--;
    }
    if (p_eit_n - p_eit >= i_section_size) return NULL;

    return p_eit_n;
}

static inline bool eit_validate_event(const uint8_t *p_eit,
                                      const uint8_t *p_eit_n,
                                      uint16_t i_desclength)
{
    uint16_t i_section_size = psi_get_length(p_eit) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    return (p_eit_n + EIT_EVENT_SIZE + i_desclength
             <= p_eit + i_section_size);
}

static inline bool eit_validate(const uint8_t *p_eit)
{
    uint16_t i_section_size = psi_get_length(p_eit) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    uint8_t i_tid = psi_get_tableid(p_eit);
    const uint8_t *p_eit_n;

    if (!psi_get_syntax(p_eit)
         || (i_tid != EIT_TABLE_ID_PF_ACTUAL
              && i_tid != EIT_TABLE_ID_PF_OTHER
              && !(i_tid >= EIT_TABLE_ID_SCHED_ACTUAL_FIRST
                    && i_tid <= EIT_TABLE_ID_SCHED_ACTUAL_LAST)
              && !(i_tid >= EIT_TABLE_ID_SCHED_OTHER_FIRST
                    && i_tid <= EIT_TABLE_ID_SCHED_OTHER_LAST)))
        return false;

    if (!psi_check_crc(p_eit))
        return false;

    p_eit_n = p_eit + EIT_HEADER_SIZE;

    while (p_eit_n + EIT_EVENT_SIZE - p_eit <= i_section_size
            && p_eit_n + EIT_EVENT_SIZE + eitn_get_desclength(p_eit_n) - p_eit
                <= i_section_size) {
        if (!descs_validate(p_eit_n + 10))
            return false;

        p_eit_n += EIT_EVENT_SIZE + eitn_get_desclength(p_eit_n);
    }

    return (p_eit_n - p_eit == i_section_size);
}

#ifdef __cplusplus
}
#endif

#endif
