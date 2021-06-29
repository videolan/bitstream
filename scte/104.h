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

static inline void scte104t_set_type(uint8_t *p, uint8_t i_type)
{
    p[0] = i_type;
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

static inline void scte104t_set_utc_seconds(uint8_t *p, uint32_t i_seconds)
{
    p[1] = (i_seconds >> 24) & 0xff;
    p[2] = (i_seconds >> 16) & 0xff;
    p[3] = (i_seconds >>  8) & 0xff;
    p[4] = (i_seconds >>  0) & 0xff;
}

static inline uint16_t scte104t_get_utc_microseconds(const uint8_t *p)
{
    return ((uint16_t)p[5] << 8) |
           (uint16_t)p[6];
}

static inline void scte104t_set_utc_microseconds(uint8_t *p, uint16_t i_microseconds)
{
    p[5] = (i_microseconds >> 8) & 0xff;
    p[6] = (i_microseconds >> 0) & 0xff;
}

static inline uint8_t scte104t_get_hours(const uint8_t *p)
{
    return p[1];
}

static inline void scte104t_set_hours(uint8_t *p, uint8_t i_hours)
{
    p[1] = i_hours;
}

static inline uint8_t scte104t_get_minutes(const uint8_t *p)
{
    return p[2];
}

static inline void scte104t_set_minutes(uint8_t *p, uint8_t i_minutes)
{
    p[2] = i_minutes;
}

static inline uint8_t scte104t_get_seconds(const uint8_t *p)
{
    return p[3];
}

static inline void scte104t_set_seconds(uint8_t *p, uint8_t i_seconds)
{
    p[3] = i_seconds;
}

static inline uint8_t scte104t_get_frames(const uint8_t *p)
{
    return p[4];
}

static inline void scte104t_set_frames(uint8_t *p, uint8_t i_frames)
{
    p[4] = i_frames;
}

static inline uint8_t scte104t_get_gpi_number(const uint8_t *p)
{
    return p[1];
}

static inline void scte104t_set_gpi_number(uint8_t *p, uint8_t i_gpi_number)
{
    p[1] = i_gpi_number;
}

static inline uint8_t scte104t_get_gpi_edge(const uint8_t *p)
{
    return p[2];
}

static inline void scte104t_set_gpi_edge(uint8_t *p, uint8_t i_gpi_edge)
{
    p[2] = i_gpi_edge;
}

/*****************************************************************************
 * Single and multiple operation message
 *****************************************************************************/
#define SCTE104_HEADER_SIZE             4

#define SCTE104_OPID_INIT_REQUEST_DATA              0x0001
#define SCTE104_OPID_INIT_RESPONSE_DATA             0x0002
#define SCTE104_OPID_ALIVE_REQUEST_DATA             0x0003
#define SCTE104_OPID_ALIVE_RESPONSE_DATA            0x0004
#define SCTE104_OPID_INJECT_SECTION                 0x0100
#define SCTE104_OPID_SPLICE                         0x0101
#define SCTE104_OPID_SPLICE_NULL                    0x0102
#define SCTE104_OPID_TIME_SIGNAL                    0x0104
#define SCTE104_OPID_INSERT_DESCRIPTOR              0x0108
#define SCTE104_OPID_INSERT_SEGMENTATION_DESCRIPTOR 0x010B
#define SCTE104_OPID_MULTIPLE                       0xffff

static inline uint16_t scte104_get_opid(const uint8_t *p)
{
    return (p[0] << 8) | p[1];
}

static inline void scte104_set_opid(uint8_t *p, uint16_t i_opid)
{
    p[0] = (i_opid >> 8) & 0xff;
    p[1] = (i_opid >> 0) & 0xff;
}

static inline uint16_t scte104_get_size(const uint8_t *p)
{
    return (p[2] << 8) | p[3];
}

static inline void scte104_set_size(uint8_t *p, uint16_t i_size)
{
    p[2] = (i_size >> 8) & 0xff;
    p[3] = (i_size >> 0) & 0xff;
}

/*****************************************************************************
 * Single operation message
 *****************************************************************************/
#define SCTE104S_HEADER_SIZE            13

static inline uint16_t scte104s_get_result(const uint8_t *p)
{
    return (p[4] << 8) | p[5];
}

static inline void scte104s_set_result(uint8_t *p, uint16_t i_result)
{
    p[4] = (i_result >> 8) & 0xff;
    p[5] = (i_result >> 0) & 0xff;
}

static inline uint16_t scte104s_get_result_extension(const uint8_t *p)
{
    return (p[6] << 8) | p[7];
}

static inline void scte104s_set_result_extension(uint8_t *p, uint16_t i_result_ext)
{
    p[6] = (i_result_ext >> 8) & 0xff;
    p[7] = (i_result_ext >> 0) & 0xff;
}

static inline uint8_t scte104s_get_protocol(const uint8_t *p)
{
    return p[8];
}

static inline void scte104s_set_protocol(uint8_t *p, uint8_t i_protocol)
{
    p[8] = i_protocol;
}

static inline uint8_t scte104s_get_as_index(const uint8_t *p)
{
    return p[9];
}

static inline void scte104s_set_as_index(uint8_t *p, uint8_t i_as_index)
{
    p[9] = i_as_index;
}

static inline uint8_t scte104s_get_message_number(const uint8_t *p)
{
    return p[10];
}

static inline void scte104s_set_message_num(uint8_t *p, uint8_t i_message_num)
{
    p[10] = i_message_num;
}

static inline uint16_t scte104s_get_dpi_pid_index(const uint8_t *p)
{
    return (p[11] << 8) | p[12];
}

static inline void scte104s_set_dpi_pid_index(uint8_t *p, uint16_t i_dpi_pid_idx)
{
    p[11] = (i_dpi_pid_idx >> 8) & 0xff;
    p[12] = (i_dpi_pid_idx >> 0) & 0xff;
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

static inline void scte104o_set_opid(uint8_t *p, uint16_t i_opid)
{
    p[0] = (i_opid >> 8) & 0xff;
    p[1] = (i_opid >> 0) & 0xff;
}

static inline uint16_t scte104o_get_data_length(const uint8_t *p)
{
    return (p[2] << 8) | p[3];
}

static inline void scte104o_set_data_length(uint8_t *p, uint16_t i_data_len)
{
    p[2] = (i_data_len >> 8) & 0xff;
    p[3] = (i_data_len >> 0) & 0xff;
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

static inline void scte104m_set_protocol(uint8_t *p, uint8_t i_protocol)
{
    p[4] = i_protocol;
}

static inline uint8_t scte104m_get_as_index(const uint8_t *p)
{
    return p[5];
}

static inline void scte104m_set_as_index(uint8_t *p, uint8_t i_as_idx)
{
    p[5] = i_as_idx;
}

static inline uint8_t scte104m_get_message_number(const uint8_t *p)
{
    return p[6];
}

static inline void scte104m_set_message_number(uint8_t *p, uint8_t i_message_num)
{
    p[6] = i_message_num;
}

static inline uint16_t scte104m_get_dpi_pid_index(const uint8_t *p)
{
    return (p[7] << 8) | p[8];
}

static inline void scte104m_set_dpi_pid_index(uint8_t *p, uint16_t i_dpi_pid_idx)
{
    p[7] = (i_dpi_pid_idx >> 8) & 0xff;
    p[8] = (i_dpi_pid_idx >> 0) & 0xff;
}

static inline uint8_t scte104m_get_scte35_protocol(const uint8_t *p)
{
    return p[9];
}

static inline void scte104m_set_scte35_protocol(uint8_t *p, uint8_t i_scte_protocol)
{
    p[9] = i_scte_protocol;
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

static inline void scte104m_set_num_ops(uint8_t *p, uint8_t i_num_ops)
{
    p = scte104m_get_timestamp(p);
    p[scte104t_get_size(p)] = i_num_ops;
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

#define SCTE104SRD_RESERVED             0
#define SCTE104SRD_START_NORMAL         1
#define SCTE104SRD_START_IMMEDIATE      2
#define SCTE104SRD_END_NORMAL           3
#define SCTE104SRD_END_IMMEDIATE        4
#define SCTE104SRD_CANCEL               5

static inline uint8_t scte104srd_get_insert_type(const uint8_t *p)
{
    return p[0];
}

static inline void scte104srd_set_insert_type(uint8_t *p, uint8_t i_insert_type)
{
    p[0] = i_insert_type;
}

static inline uint32_t scte104srd_get_event_id(const uint8_t *p)
{
    return ((uint32_t)p[1] << 24) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 8) |
           (uint32_t)p[4];
}

static inline void scte104srd_set_event_id(uint8_t *p, uint32_t i_event_id)
{
    p[1] = (i_event_id >> 24) & 0xff;
    p[2] = (i_event_id >> 16) & 0xff;
    p[3] = (i_event_id >>  8) & 0xff;
    p[4] = (i_event_id >>  0) & 0xff;
}

static inline uint16_t scte104srd_get_unique_program_id(const uint8_t *p)
{
    return ((uint16_t)p[5] << 8) |
           (uint16_t)p[6];
}

static inline void scte104srd_set_unique_program_id(uint8_t *p, uint16_t i_unique_prog_id)
{
    p[5] = (i_unique_prog_id >> 8) & 0xff;
    p[6] = (i_unique_prog_id >> 0) & 0xff;
}

static inline uint16_t scte104srd_get_pre_roll_time(const uint8_t *p)
{
    return ((uint16_t)p[7] << 8) |
           (uint16_t)p[8];
}

static inline void scte104srd_set_pre_roll_time(uint8_t *p, uint16_t i_pre_roll_time)
{
    p[7] = (i_pre_roll_time >> 8) & 0xff;
    p[8] = (i_pre_roll_time >> 0) & 0xff;
}

static inline uint16_t scte104srd_get_break_duration(const uint8_t *p)
{
    return ((uint16_t)p[9] << 8) |
           (uint16_t)p[10];
}

static inline void scte104srd_set_break_duration(uint8_t *p, uint16_t i_break_duration)
{
    p[ 9] = (i_break_duration >> 8) & 0xff;
    p[10] = (i_break_duration >> 0) & 0xff;
}

static inline uint8_t scte104srd_get_avail_num(const uint8_t *p)
{
    return p[11];
}

static inline void scte104srd_set_avail_num(uint8_t *p, uint8_t i_avail_num)
{
    p[11] = i_avail_num;
}

static inline uint8_t scte104srd_get_avails_expected(const uint8_t *p)
{
    return p[12];
}

static inline void scte104srd_set_avails_expected(uint8_t *p, uint8_t i_avails_expected)
{
    p[12] = i_avails_expected;
}

static inline uint8_t scte104srd_get_auto_return(const uint8_t *p)
{
    return p[13];
}

static inline void scte104srd_set_auto_return(uint8_t *p, uint8_t i_auto_return)
{
    p[13] = i_auto_return;
}

/*****************************************************************************
 * SCTE-104 : time_signal_request_data
 *****************************************************************************/
#define SCTE104TSRD_HEADER_SIZE     2

static inline uint16_t scte104tsrd_get_pre_roll_time(const uint8_t *p)
{
    return ((uint16_t)p[0] << 8) | ((uint16_t)p[1]);
}

/*****************************************************************************
 * SCTE-104 : insert_descriptor_request_data
 *****************************************************************************/
#define SCTE104IDRD_HEADER_SIZE     1

static inline uint8_t scte104idrd_get_count(const uint8_t *p)
{
    return p[0];
}

static inline uint8_t *scte104idrd_get_image(const uint8_t *p)
{
    return (uint8_t *)(p + 1);
}

/*****************************************************************************
 * SCTE-104 : insert_segmentation_descriptor_request_data
 *****************************************************************************/
#define SCTE104ISDRD_HEADER_SIZE    18

static inline uint32_t scte104isdrd_get_event_id(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
        ((uint32_t)p[2] << 8) | ((uint32_t)p[3]);
}

static inline uint8_t scte104isdrd_get_cancel_indicator(const uint8_t *p)
{
    return p[4];
}

static inline uint16_t scte104isdrd_get_duration(const uint8_t *p)
{
    return ((uint16_t)p[5] << 8) | ((uint16_t)p[6]);
}

static inline uint8_t scte104isdrd_get_upid_type(const uint8_t *p)
{
    return p[7];
}

static inline uint8_t scte104isdrd_get_upid_length(const uint8_t *p)
{
    return p[8];
}

static inline uint8_t *scte104isdrd_get_upid(const uint8_t *p)
{
    return (uint8_t *)p + 9;
}

static inline uint8_t scte104isdrd_get_type_id(const uint8_t *p)
{
    uint8_t upid_length = scte104isdrd_get_upid_length(p);
    p += 9 + upid_length;
    return p[0];
}

static inline uint8_t scte104isdrd_get_num(const uint8_t *p)
{
    uint8_t upid_length = scte104isdrd_get_upid_length(p);
    p += 9 + upid_length;
    return p[1];
}

static inline uint8_t scte104isdrd_get_expected(const uint8_t *p)
{
    uint8_t upid_length = scte104isdrd_get_upid_length(p);
    p += 9 + upid_length;
    return p[2];
}

static inline uint8_t
scte104isdrd_get_duration_extension_frames(const uint8_t *p)
{
    uint8_t upid_length = scte104isdrd_get_upid_length(p);
    p += 9 + upid_length;
    return p[3];
}

static inline uint8_t scte104isdrd_get_delivery_not_restricted(const uint8_t *p)
{
    uint8_t upid_length = scte104isdrd_get_upid_length(p);
    p += 9 + upid_length;
    return p[4];
}

static inline uint8_t scte104isdrd_get_web_delivery_allowed(const uint8_t *p)
{
    uint8_t upid_length = scte104isdrd_get_upid_length(p);
    p += 9 + upid_length;
    return p[5];
}

static inline uint8_t scte104isdrd_get_no_regional_blackout(const uint8_t *p)
{
    uint8_t upid_length = scte104isdrd_get_upid_length(p);
    p += 9 + upid_length;
    return p[6];
}

static inline uint8_t scte104isdrd_get_archive_allowed(const uint8_t *p)
{
    uint8_t upid_length = scte104isdrd_get_upid_length(p);
    p += 9 + upid_length;
    return p[7];
}

static inline uint8_t scte104isdrd_get_device_restrictions(const uint8_t *p)
{
    uint8_t upid_length = scte104isdrd_get_upid_length(p);
    p += 9 + upid_length;
    return p[8];
}

static inline uint8_t scte104isdrd_get_insert_sub_info(const uint8_t *p)
{
    uint8_t upid_length = scte104isdrd_get_upid_length(p);
    p += 9 + upid_length;
    return p[9];
}

static inline uint8_t scte104isdrd_get_sub_num(const uint8_t *p)
{
    uint8_t upid_length = scte104isdrd_get_upid_length(p);
    p += 9 + upid_length;
    return p[10];
}

static inline uint8_t scte104isdrd_get_sub_expected(const uint8_t *p)
{
    uint8_t upid_length = scte104isdrd_get_upid_length(p);
    p += 9 + upid_length;
    return p[11];
}

#ifdef __cplusplus
}
#endif

#endif
