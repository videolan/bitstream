/*****************************************************************************
 * sim.h: ETSI TS 103 194 DVB Simulcrypt
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
 *  - ETSI TS 101 197 V1.2.1 (2002-02) (DVB Simulcrypt - part 1)
 *  - ETSI TR 102 035 V1.1.1 (2002-04) (Implementation guidelines of Simulcrypt)
 *  - ETSI TS 103 197 V1.5.1 (2008-10) (DVB Simulcrypt)
 */

#ifndef __BITSTREAM_DVB_SIM_H__
#define __BITSTREAM_DVB_SIM_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */
#include <string.h>   /* memcpy, NULL */

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Generic "Type, Length, Value" message
 *****************************************************************************/
#define TLVH_HEADER_SIZE        1
#define TLV_HEADER_SIZE         4

#define TLV_PARAM_EMPTY         0
#define TLV_PARAM_EMPTY_SIZE    2

typedef struct tlv_param_count_t {
    uint16_t i_type;
    uint16_t i_min;
    uint16_t i_max;
} tlv_param_count_t;

static inline void tlvh_set_version(uint8_t *p_tlv, uint8_t i_version)
{
    p_tlv[0] = i_version;
}

static inline uint8_t tlvh_get_version(const uint8_t *p_tlv)
{
    return p_tlv[0];
}

static inline uint8_t *tlvh_get_tlv(uint8_t *p_tlv)
{
    return p_tlv + 1;
}

static inline void tlv_set_type(uint8_t *p_tlv, uint16_t i_type)
{
    p_tlv[0] = i_type >> 8;
    p_tlv[1] = i_type & 0xff;
}

static inline uint16_t tlv_get_type(const uint8_t *p_tlv)
{
    return (p_tlv[0] << 8) | p_tlv[1];
}

static inline void tlv_set_length(uint8_t *p_tlv, uint16_t i_length)
{
    p_tlv[2] = i_length >> 8;
    p_tlv[3] = i_length & 0xff;
}

static inline uint16_t tlv_get_length(const uint8_t *p_tlv)
{
    return (p_tlv[2] << 8) | p_tlv[3];
}

static inline void tlv_empty(uint8_t *p_tlv)
{
    /* prepare a dummy parameter for later appending */
    p_tlv[TLV_HEADER_SIZE] = TLV_PARAM_EMPTY >> 8;
    p_tlv[TLV_HEADER_SIZE + 1] = TLV_PARAM_EMPTY & 0xff;
}

static inline void tlv_append_empty(uint8_t *p_tlv)
{
    uint16_t i_length = tlv_get_length(p_tlv);
    p_tlv[TLV_HEADER_SIZE + i_length] = TLV_PARAM_EMPTY >> 8;
    p_tlv[TLV_HEADER_SIZE + i_length + 1] = TLV_PARAM_EMPTY & 0xff;
}

static inline uint8_t *tlv_get_param(uint8_t *p_tlv, uint16_t n)
{
    uint16_t i_tlv_size = tlv_get_length(p_tlv);
    uint8_t *p_tlv_n = p_tlv + TLV_HEADER_SIZE;
    if (p_tlv_n - p_tlv - TLV_HEADER_SIZE > i_tlv_size) return NULL;

    while (n) {
        if (p_tlv_n - p_tlv > i_tlv_size)
            return NULL;
        p_tlv_n += TLV_HEADER_SIZE + tlv_get_length(p_tlv_n);
        n--;
    }
    if (p_tlv_n - p_tlv - TLV_HEADER_SIZE >= i_tlv_size) return NULL;
    return p_tlv_n;
}

static inline bool tlv_validate_param(const uint8_t *p_tlv,
                                      const uint8_t *p_tlv_n, uint16_t i_length)
{
    uint16_t i_tlv_size = tlv_get_length(p_tlv);
    return (p_tlv_n + i_length - p_tlv <= i_tlv_size);
}

static inline bool tlv_validate(const uint8_t *p_tlv)
{
    uint16_t i_tlv_size = tlv_get_length(p_tlv);
    const uint8_t *p_tlv_n = p_tlv + TLV_HEADER_SIZE;

    while (p_tlv_n - p_tlv <= i_tlv_size
            && p_tlv_n + tlv_get_length(p_tlv_n) - p_tlv <= i_tlv_size)
        p_tlv_n += TLV_HEADER_SIZE + tlv_get_length(p_tlv_n);

    return (p_tlv_n - p_tlv - TLV_HEADER_SIZE == i_tlv_size);
}

static inline uint8_t *tlv_find_param(uint8_t *p_tlv, uint16_t i_type,
                                      uint16_t n)
{
    uint8_t *p_param;
    uint8_t j = 0;

    while ((p_param = tlv_get_param(p_tlv, j)) != NULL) {
        j++;
        if (tlv_get_type(p_param) == i_type) {
            if (!n) return p_param;
            n--;
        }
    }

    return NULL;
}

static inline uint16_t tlv_count_param(uint8_t *p_tlv, uint16_t i_type)
{
    uint16_t i_count = 0;
    uint8_t *p_tlv_n;
    uint16_t j = 0;

    while ((p_tlv_n = tlv_get_param(p_tlv, j)) != NULL) {
        j++;
        if (tlv_get_type(p_tlv_n) == i_type)
            i_count++;
    }
    return i_count;
}

static inline bool tlv_validate_count_param(uint8_t *p_tlv,
                                            const tlv_param_count_t *p_count)
{
    uint16_t i_count = tlv_count_param(p_tlv, p_count->i_type);
    return (i_count >= p_count->i_min) && (i_count <= p_count->i_max);
}

#define TLV_DECLARE_PARAM(intf, name, param, utype, type)                   \
static inline bool intf##_append_##name(uint8_t *p_tlv, type i_##name)      \
{                                                                           \
    uint8_t *p_tlv_n = tlv_find_param(p_tlv, TLV_PARAM_EMPTY, 0);           \
    int i;                                                                  \
    if (!tlv_validate_param(p_tlv, p_tlv_n,                                 \
                            sizeof(type) + TLV_PARAM_EMPTY_SIZE))           \
        return false;                                                       \
    tlv_set_type(p_tlv_n, param);                                           \
    tlv_set_length(p_tlv_n, sizeof(type));                                  \
    for (i = 0; i < sizeof(type); i++)                                      \
        p_tlv_n[4 + i] = ((utype)(i_##name) >> 8 * (sizeof(type) - i - 1))  \
                           & 0xff;                                          \
    tlv_append_empty(p_tlv_n);                                              \
    return true;                                                            \
}                                                                           \
                                                                            \
static inline type intf##_find_##name(uint8_t *p_tlv, uint16_t n)           \
{                                                                           \
    const uint8_t *p_tlv_n = tlv_find_param(p_tlv, param, n);               \
    type i_##name = (type)(p_tlv_n[4]) << 8 * (sizeof(type) - 1);           \
    int i;                                                                  \
    for (i = 1; i < sizeof(type); i++)                                      \
        i_##name |= (utype)(p_tlv_n[4 + i]) << 8 * (sizeof(type) - i - 1);  \
    return i_##name;                                                        \
}

static inline bool tlv_append_data(uint8_t *p_tlv, uint16_t i_type,
                                   const uint8_t *p_data, uint16_t i_length)
{
    uint8_t *p_tlv_n = tlv_find_param(p_tlv, TLV_PARAM_EMPTY, 0);
    if (!tlv_validate_param(p_tlv, p_tlv_n,
                            i_length + TLV_PARAM_EMPTY_SIZE))
        return false;
    tlv_set_type(p_tlv_n, i_type);
    tlv_set_length(p_tlv_n, i_length);
    memcpy(p_tlv_n + 4, p_data, i_length);
    tlv_append_empty(p_tlv_n);
    return true;
}

static inline uint8_t *tlv_find_data(uint8_t *p_tlv, uint16_t i_type,
                                     uint16_t n, uint16_t *pi_length)
{
    uint8_t *p_tlv_n = tlv_find_param(p_tlv, i_type, n);
    *pi_length = tlv_get_length(p_tlv_n);
    return p_tlv_n + 4;
}

/*****************************************************************************
 * ECMG
 *****************************************************************************/
/* message types */
#define ECMG_TYPE_CHANNEL_SETUP     0x1
#define ECMG_TYPE_CHANNEL_TEST      0x2
#define ECMG_TYPE_CHANNEL_STATUS    0x3
#define ECMG_TYPE_CHANNEL_CLOSE     0x4
#define ECMG_TYPE_CHANNEL_ERROR     0x5
#define ECMG_TYPE_STREAM_SETUP      0x101
#define ECMG_TYPE_STREAM_TEST       0x102
#define ECMG_TYPE_STREAM_STATUS     0x103
#define ECMG_TYPE_STREAM_CLOSEREQ   0x104
#define ECMG_TYPE_STREAM_CLOSERESP  0x105
#define ECMG_TYPE_STREAM_ERROR      0x106
#define ECMG_TYPE_CW                0x201
#define ECMG_TYPE_ECM               0x202

/* parameter types */
#define ECMG_PARAM_SUPERCASID       0x1
#define ECMG_PARAM_SECTIONTSPKT     0x2
#define ECMG_PARAM_DELAYSTART       0x3
#define ECMG_PARAM_DELAYSTOP        0x4
#define ECMG_PARAM_TRANSDELAYSTART  0x5
#define ECMG_PARAM_TRANSDELAYSTOP   0x6
#define ECMG_PARAM_REPPERIOD        0x7
#define ECMG_PARAM_MAXSTREAMS       0x8
#define ECMG_PARAM_MINCPDUR         0x9
#define ECMG_PARAM_LEADCW           0xa
#define ECMG_PARAM_CWPERMSG         0xb
#define ECMG_PARAM_MAXCOMPTIME      0xc
#define ECMG_PARAM_ACCESSCRIT       0xd
#define ECMG_PARAM_CHANNELID        0xe
#define ECMG_PARAM_STREAMID         0xf
#define ECMG_PARAM_NOMCPDUR         0x10
#define ECMG_PARAM_ACCESSCRITMODE   0x11
#define ECMG_PARAM_CPNUMBER         0x12
#define ECMG_PARAM_CPDUR            0x13
#define ECMG_PARAM_CPCWCOMB         0x14
#define ECMG_PARAM_ECM              0x15
#define ECMG_PARAM_ACDELAYSTART     0x16
#define ECMG_PARAM_ACDELAYSTOP      0x17
#define ECMG_PARAM_CWENCRYPT        0x18
#define ECMG_PARAM_ECMID            0x19
#define ECMG_PARAM_ERRORSTATUS      0x7000
#define ECMG_PARAM_ERRORINFO        0x7001

static inline void ecmg_init(uint8_t *p_tlv)
{
    tlv_empty(p_tlv);
}

TLV_DECLARE_PARAM(ecmg, supercasid, ECMG_PARAM_SUPERCASID, uint32_t, uint32_t)
TLV_DECLARE_PARAM(ecmg, sectiontspkt, ECMG_PARAM_SECTIONTSPKT, uint8_t, uint8_t)
TLV_DECLARE_PARAM(ecmg, delaystart, ECMG_PARAM_DELAYSTART, uint16_t, int16_t)
TLV_DECLARE_PARAM(ecmg, delaystop, ECMG_PARAM_DELAYSTOP, uint16_t, int16_t)
TLV_DECLARE_PARAM(ecmg, transdelaystart, ECMG_PARAM_TRANSDELAYSTART,
                  uint16_t, int16_t)
TLV_DECLARE_PARAM(ecmg, transdelaystop, ECMG_PARAM_TRANSDELAYSTOP,
                  uint16_t, int16_t)
TLV_DECLARE_PARAM(ecmg, repperiod, ECMG_PARAM_REPPERIOD, uint16_t, uint16_t)
TLV_DECLARE_PARAM(ecmg, maxstreams, ECMG_PARAM_MAXSTREAMS, uint16_t, uint16_t)
TLV_DECLARE_PARAM(ecmg, mincpdur, ECMG_PARAM_MINCPDUR, uint16_t, uint16_t)
TLV_DECLARE_PARAM(ecmg, leadcw, ECMG_PARAM_LEADCW, uint8_t, uint8_t)
TLV_DECLARE_PARAM(ecmg, cwpermsg, ECMG_PARAM_CWPERMSG, uint8_t, uint8_t)
TLV_DECLARE_PARAM(ecmg, maxcomptime, ECMG_PARAM_MAXCOMPTIME, uint16_t, uint16_t)
TLV_DECLARE_PARAM(ecmg, channelid, ECMG_PARAM_CHANNELID, uint16_t, uint16_t)
TLV_DECLARE_PARAM(ecmg, streamid, ECMG_PARAM_STREAMID, uint16_t, uint16_t)
TLV_DECLARE_PARAM(ecmg, nomcpdur, ECMG_PARAM_NOMCPDUR, uint16_t, uint16_t)
TLV_DECLARE_PARAM(ecmg, accesscritmode, ECMG_PARAM_ACCESSCRITMODE,
                  uint8_t, bool)
TLV_DECLARE_PARAM(ecmg, cpnumber, ECMG_PARAM_CPNUMBER, uint16_t, uint16_t)
TLV_DECLARE_PARAM(ecmg, cpdur, ECMG_PARAM_CPDUR, uint16_t, uint16_t)
TLV_DECLARE_PARAM(ecmg, acdelaystart, ECMG_PARAM_ACDELAYSTART,
                  uint16_t, int16_t)
TLV_DECLARE_PARAM(ecmg, acdelaystop, ECMG_PARAM_ACDELAYSTOP, uint16_t, int16_t)
TLV_DECLARE_PARAM(ecmg, ecmid, ECMG_PARAM_ECMID, uint16_t, uint16_t)
TLV_DECLARE_PARAM(ecmg, errorstatus, ECMG_PARAM_ERRORSTATUS, uint16_t, uint16_t)

static inline void ecmgcw_set_cpnum(uint8_t *p_param, uint16_t i_cpnum)
{
    p_param[0] = i_cpnum >> 8;
    p_param[1] = i_cpnum & 0xff;
}

static inline uint16_t ecmgcw_get_cpnum(uint8_t *p_param)
{
    return (p_param[0] << 8) | p_param[1];
}

static inline uint8_t *ecmgcw_get_cw(uint8_t *p_param)
{
    return p_param + 2;
}

static inline bool ecmg_validate_param(const uint8_t *p_tlv_n)
{
    static const uint16_t pi_ecmg_params_minlength[] = {
        /*  0x0 */ 0, 4, 1, 2, 2, 2, 2, 2,
        /*  0x8 */ 2, 2, 1, 1, 2, 0, 2, 2,
        /* 0x10 */ 2, 1, 2, 2, 0, 0, 2, 2,
        /* 0x18 */ 0, 2
    };
    uint16_t i_type = tlv_get_type(p_tlv_n);
    uint16_t i_length = tlv_get_length(p_tlv_n);

    if (i_type <= ECMG_PARAM_ECMID) {
        if (i_length < pi_ecmg_params_minlength[i_type]) return false;

    } else if (i_type == ECMG_PARAM_ERRORSTATUS) {
        if (i_length < 2) return false;
    }

    return true;
}

static inline bool ecmg_validate(uint8_t *p_tlv)
{
    static const tlv_param_count_t p_ecmg_params_channel_setup[] = {
        {ECMG_PARAM_CHANNELID, 1, 1}, {ECMG_PARAM_SUPERCASID, 1, 1}, {0, 0, 0}
    };
    static const tlv_param_count_t p_ecmg_params_channel_test[] = {
        {ECMG_PARAM_CHANNELID, 1, 1}, {0, 0, 0}
    };
    static const tlv_param_count_t p_ecmg_params_channel_status[] = {
        {ECMG_PARAM_CHANNELID, 1, 1}, {ECMG_PARAM_SECTIONTSPKT, 1, 1},
        {ECMG_PARAM_ACDELAYSTART, 0, 1}, {ECMG_PARAM_ACDELAYSTOP, 0, 1},
        {ECMG_PARAM_DELAYSTART, 1, 1}, {ECMG_PARAM_DELAYSTOP, 1, 1},
        {ECMG_PARAM_TRANSDELAYSTART, 0, 1}, {ECMG_PARAM_TRANSDELAYSTOP, 0, 1},
        {ECMG_PARAM_REPPERIOD, 1, 1}, {ECMG_PARAM_MAXSTREAMS, 1, 1},
        {ECMG_PARAM_MINCPDUR, 1, 1}, {ECMG_PARAM_LEADCW, 1, 1},
        {ECMG_PARAM_CWPERMSG, 1, 1}, {ECMG_PARAM_MAXCOMPTIME, 1, 1}, {0, 0, 0}
    };
    static const tlv_param_count_t p_ecmg_params_channel_close[] = {
        {ECMG_PARAM_CHANNELID, 1, 1}, {0, 0, 0}
    };
    static const tlv_param_count_t p_ecmg_params_channel_error[] = {
        {ECMG_PARAM_CHANNELID, 1, 1}, {ECMG_PARAM_ERRORSTATUS, 1, UINT16_MAX},
        {0, 0, 0}
    };

    static const tlv_param_count_t p_ecmg_params_stream_setup[] = {
        {ECMG_PARAM_CHANNELID, 1, 1}, {ECMG_PARAM_STREAMID, 1, 1},
        {ECMG_PARAM_ECMID, 0, 1}, {ECMG_PARAM_NOMCPDUR, 1, 1}, {0, 0, 0}
    };
    static const tlv_param_count_t p_ecmg_params_stream_test[] = {
        {ECMG_PARAM_CHANNELID, 1, 1}, {ECMG_PARAM_STREAMID, 1, 1}, {0, 0, 0}
    };
    static const tlv_param_count_t p_ecmg_params_stream_status[] = {
        {ECMG_PARAM_CHANNELID, 1, 1}, {ECMG_PARAM_STREAMID, 1, 1},
        {ECMG_PARAM_ECMID, 0, 1}, {ECMG_PARAM_ACCESSCRITMODE, 1, 1}, {0, 0, 0}
    };
    static const tlv_param_count_t p_ecmg_params_stream_close[] = {
        {ECMG_PARAM_CHANNELID, 1, 1}, {ECMG_PARAM_STREAMID, 1, 1}, {0, 0, 0}
    };
    static const tlv_param_count_t p_ecmg_params_stream_error[] = {
        {ECMG_PARAM_CHANNELID, 1, 1}, {ECMG_PARAM_STREAMID, 1, 1},
        {ECMG_PARAM_ERRORSTATUS, 1, UINT16_MAX}, {0, 0, 0}
    };
    static const tlv_param_count_t p_ecmg_params_cw[] = {
        {ECMG_PARAM_CHANNELID, 1, 1}, {ECMG_PARAM_STREAMID, 1, 1},
        {ECMG_PARAM_CPNUMBER, 1, 1}, {ECMG_PARAM_CWENCRYPT, 0, 1},
        {ECMG_PARAM_CPCWCOMB, 1, UINT16_MAX}, {ECMG_PARAM_CPDUR, 0, 1},
        {ECMG_PARAM_ACCESSCRIT, 0, 1}, {0, 0, 0}
    };
    static const tlv_param_count_t p_ecmg_params_ecm[] = {
        {ECMG_PARAM_CHANNELID, 1, 1}, {ECMG_PARAM_STREAMID, 1, 1},
        {ECMG_PARAM_CPNUMBER, 1, 1}, {ECMG_PARAM_ECM, 1, 1}, {0, 0, 0}
    };

    const tlv_param_count_t null_param = {0, 0, 0};
    const tlv_param_count_t *p_param = &null_param;
    uint8_t *p_tlv_n;
    int j = 0;

    switch (tlv_get_type(p_tlv)) {
        case ECMG_TYPE_CHANNEL_SETUP:
            p_param = p_ecmg_params_channel_setup; break;
        case ECMG_TYPE_CHANNEL_TEST:
            p_param = p_ecmg_params_channel_test; break;
        case ECMG_TYPE_CHANNEL_STATUS:
            p_param = p_ecmg_params_channel_status; break;
        case ECMG_TYPE_CHANNEL_CLOSE:
            p_param = p_ecmg_params_channel_close; break;
        case ECMG_TYPE_CHANNEL_ERROR:
            p_param = p_ecmg_params_channel_error; break;
        case ECMG_TYPE_STREAM_SETUP:
            p_param = p_ecmg_params_stream_setup; break;
        case ECMG_TYPE_STREAM_TEST:
            p_param = p_ecmg_params_stream_test; break;
        case ECMG_TYPE_STREAM_STATUS:
            p_param = p_ecmg_params_stream_status; break;
        case ECMG_TYPE_STREAM_CLOSEREQ:
        case ECMG_TYPE_STREAM_CLOSERESP:
            p_param = p_ecmg_params_stream_close; break;
        case ECMG_TYPE_STREAM_ERROR:
            p_param = p_ecmg_params_stream_error; break;
        case ECMG_TYPE_CW:
            p_param = p_ecmg_params_cw; break;
        case ECMG_TYPE_ECM:
            p_param = p_ecmg_params_ecm; break;
        default:
            break;
    }

    while (p_param->i_type)
        if (!tlv_validate_count_param(p_tlv, p_param++))
            return false;

    while ((p_tlv_n = tlv_get_param(p_tlv, j)) != NULL) {
        j++;
        if (!ecmg_validate_param(p_tlv_n)) return false;
    }

    return true;
}

#ifdef __cplusplus
}
#endif

#endif
