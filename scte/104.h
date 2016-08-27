/*****************************************************************************
 * 104.h: SCTE 104 Automation to Compression Communications API
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
 *  - SCTE 104 2012 (Automation to Compression Communications API)
 */

#ifndef __BITSTREAM_SCTE_104_H__
#define __BITSTREAM_SCTE_104_H__

#include <bitstream/common.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * SCTE 104 - timestamp structure
 *****************************************************************************/
#define SCTE104T_HEADER_SIZE        1

#define SCTE104T_TYPE_NONE          0
#define SCTE104T_TYPE_UTC           1
#define SCTE104T_TYPE_VITC          2
#define SCTE104T_TYPE_GPI           3

static inline uint8_t scte104t_get_type(const uint8_t *p)
{
    return p[0];
}

static inline uint8_t scte104t_get_size(const uint8_t *p)
{
    switch (scte104t_get_type(p)) {
        case SCTE104T_TYPE_NONE:
        default:
            return 1;

        case SCTE104T_TYPE_UTC:
            return 7;

        case SCTE104T_TYPE_VITC:
            return 5;

        case SCTE104T_TYPE_GPI:
            return 3;
    }
}

static inline uint32_t scte104t_get_utc_seconds(const uint8_t *p)
{
    return ((uint32_t)p[1] << 24) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 8) |
           (uint32_t)p[4];
}

static inline uint16_t scte104t_get_utc_microseconds(const uint8_t *p)
{
    return ((uint16_t)p[5] << 8) |
           (uint16_t)p[6];
}

static inline uint8_t scte104t_get_hours(const uint8_t *p)
{
    return p[1];
}

static inline uint8_t scte104t_get_minutes(const uint8_t *p)
{
    return p[2];
}

static inline uint8_t scte104t_get_seconds(const uint8_t *p)
{
    return p[3];
}

static inline uint8_t scte104t_get_frames(const uint8_t *p)
{
    return p[4];
}

static inline uint8_t scte104t_get_gpi_number(const uint8_t *p)
{
    return p[1];
}

static inline uint8_t scte104t_get_gpi_edge(const uint8_t *p)
{
    return p[2];
}

/*****************************************************************************
 * Single and multiple operation message
 *****************************************************************************/
#define SCTE104_HEADER_SIZE             4

#define SCTE104_OPID_INJECT_SECTION     0x0100
#define SCTE104_OPID_SPLICE             0x0101
#define SCTE104_OPID_SPLICE_NULL        0x0102
#define SCTE104_OPID_TIME_SIGNAL        0x0104
#define SCTE104_OPID_MULTIPLE           0xffff

static inline uint16_t scte104_get_opid(const uint8_t *p)
{
    return (p[0] << 8) | p[1];
}

static inline uint16_t scte104_get_size(const uint8_t *p)
{
    return (p[2] << 8) | p[3];
}

/*****************************************************************************
 * Single operation message
 *****************************************************************************/
#define SCTE104S_HEADER_SIZE            13

static inline uint16_t scte104s_get_result(const uint8_t *p)
{
    return (p[4] << 8) | p[5];
}

static inline uint16_t scte104s_get_result_extension(const uint8_t *p)
{
    return (p[6] << 8) | p[7];
}

static inline uint8_t scte104s_get_protocol(const uint8_t *p)
{
    return p[8];
}

static inline uint8_t scte104s_get_as_index(const uint8_t *p)
{
    return p[9];
}

static inline uint8_t scte104s_get_message_number(const uint8_t *p)
{
    return p[10];
}

static inline uint16_t scte104s_get_dpi_pid_index(const uint8_t *p)
{
    return (p[11] << 8) | p[12];
}

static inline uint8_t *scte104s_get_data(const uint8_t *p, uint16_t *pi_size)
{
    *pi_size = scte104_get_size(p) - SCTE104_HEADER_SIZE;
    return (uint8_t *)&p[13];
}

static inline bool scte104s_validate(const uint8_t *p)
{
    uint16_t i_size = scte104_get_size(p);
    if (i_size < SCTE104S_HEADER_SIZE)
        return false;

    return true;
}

/*****************************************************************************
 * Multiple operation message : operations
 *****************************************************************************/
#define SCTE104O_HEADER_SIZE            4

static inline uint16_t scte104o_get_opid(const uint8_t *p)
{
    return (p[0] << 8) | p[1];
}

static inline uint16_t scte104o_get_data_length(const uint8_t *p)
{
    return (p[2] << 8) | p[3];
}

static inline uint8_t *scte104o_get_data(const uint8_t *p)
{
    return (uint8_t *)&p[4];
}

/*****************************************************************************
 * Multiple operation message
 *****************************************************************************/
#define SCTE104M_HEADER_SIZE            10

static inline uint8_t scte104m_get_protocol(const uint8_t *p)
{
    return p[4];
}

static inline uint8_t scte104m_get_as_index(const uint8_t *p)
{
    return p[5];
}

static inline uint8_t scte104m_get_message_number(const uint8_t *p)
{
    return p[6];
}

static inline uint16_t scte104m_get_dpi_pid_index(const uint8_t *p)
{
    return (p[7] << 8) | p[8];
}

static inline uint8_t scte104m_get_scte35_protocol(const uint8_t *p)
{
    return p[9];
}

static inline uint8_t *scte104m_get_timestamp(const uint8_t *p)
{
    return (uint8_t *)&p[10];
}

static inline uint8_t scte104m_get_num_ops(const uint8_t *p)
{
    p = scte104m_get_timestamp(p);
    return p[scte104t_get_size(p)];
}

static inline uint8_t *scte104m_get_op(const uint8_t *p, uint8_t i_op)
{
    uint16_t i_size = scte104_get_size(p);
    const uint8_t *p_end = p + i_size;
    p = scte104m_get_timestamp(p);
    p += scte104t_get_size(p) + 1;

    while (i_op--) {
        if (p + SCTE104O_HEADER_SIZE > p_end ||
            p + scte104o_get_data_length(p) + SCTE104O_HEADER_SIZE > p_end)
            return NULL;
        p += scte104o_get_data_length(p) + SCTE104O_HEADER_SIZE;
    }
    return (uint8_t *)p;
}

static inline bool scte104m_validate(const uint8_t *p)
{
    uint16_t i_size = scte104_get_size(p);
    if (i_size < SCTE104M_HEADER_SIZE + SCTE104T_HEADER_SIZE)
        return false;

    const uint8_t *p_timestamp = scte104m_get_timestamp(p);
    if (i_size < SCTE104M_HEADER_SIZE + scte104t_get_size(p_timestamp) + 1)
        return false;

    uint8_t i_op = scte104m_get_num_ops(p);
    if (scte104m_get_op(p, i_op - 1) == NULL)
        return false;

    return true;
}

/*****************************************************************************
 * SCTE-104 : splice_request_data
 *****************************************************************************/
#define SCTE104SRD_HEADER_SIZE          14

#define SCTE104SRD_START_NORMAL         1
#define SCTE104SRD_START_IMMEDIATE      2
#define SCTE104SRD_END_NORMAL           3
#define SCTE104SRD_END_IMMEDIATE        4
#define SCTE104SRD_CANCEL               5

static inline uint8_t scte104srd_get_insert_type(const uint8_t *p)
{
    return p[0];
}

static inline uint32_t scte104srd_get_event_id(const uint8_t *p)
{
    return ((uint32_t)p[1] << 24) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 8) |
           (uint32_t)p[4];
}

static inline uint16_t scte104srd_get_unique_program_id(const uint8_t *p)
{
    return ((uint16_t)p[5] << 8) |
           (uint16_t)p[6];
}

static inline uint16_t scte104srd_get_pre_roll_time(const uint8_t *p)
{
    return ((uint16_t)p[7] << 8) |
           (uint16_t)p[8];
}

static inline uint16_t scte104srd_get_break_duration(const uint8_t *p)
{
    return ((uint16_t)p[9] << 8) |
           (uint16_t)p[10];
}

static inline uint8_t scte104srd_get_avail_num(const uint8_t *p)
{
    return p[11];
}

static inline uint8_t scte104srd_get_avails_expected(const uint8_t *p)
{
    return p[12];
}

static inline uint8_t scte104srd_get_auto_return(const uint8_t *p)
{
    return p[13];
}

#ifdef __cplusplus
}
#endif

#endif
