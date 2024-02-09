/*****************************************************************************
 * Haivision SRT
 *****************************************************************************
 * Copyright (C) 2023 Open Broadcast Systems Ltd.
 *
 * Authors: Kieran Kunhya <kierank@obe.tv>
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
 * References:
 *  - SRT Technical Overview
 */

#ifndef __BITSTREAM_HAIVISION_SRT__
#define __BITSTREAM_HAIVISION_SRT__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define SRT_MAGIC_CODE 0x4a17

#define SRT_HEADER_SIZE 16

/*

   SRT Data Packet

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+- SRT Header +-+-+-+-+-+-+-+-+-+-+-+-+-+
   |0|                    Packet Sequence Number                   |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |P P|O|K K|R|                   Message Number                  |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           Timestamp                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                     Destination Socket ID                     |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                                                               |
   +                              Data                             +
   |                                                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

*/

#define SRT_DATA_POSITION_MIDDLE    0x0
#define SRT_DATA_POSITION_LAST      0x1
#define SRT_DATA_POSITION_FIRST     0x2
#define SRT_DATA_POSITION_ONLY      0x3

#define SRT_DATA_ENCRYPTION_CLEAR   0x0
#define SRT_DATA_ENCRYPTION_EVEN    0x1
#define SRT_DATA_ENCRYPTION_ODD     0x2
#define SRT_DATA_ENCRYPTION_CONTROL 0x3

/*
   SRT Control Packet

       0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+- SRT Header +-+-+-+-+-+-+-+-+-+-+-+-+-+
   |1|         Control Type        |            Subtype            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                   Type-specific Information                   |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           Timestamp                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                     Destination Socket ID                     |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+- CIF -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                                                               |
   +                   Control Information Field                   +
   |                                                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

*/

#define SRT_CONTROL_TYPE_HANDSHAKE 0x0
#define SRT_CONTROL_TYPE_KEEPALIVE 0x1
#define SRT_CONTROL_TYPE_ACK       0x2
#define SRT_CONTROL_TYPE_NAK       0x3
#define SRT_CONTROL_TYPE_SHUTDOWN  0x5
#define SRT_CONTROL_TYPE_ACKACK    0x6
#define SRT_CONTROL_TYPE_DROPREQ   0x7
#define SRT_CONTROL_TYPE_PEERERROR 0x8
#define SRT_CONTROL_TYPE_USER      0x7fff

static inline void srt_set_packet_control(uint8_t *buf, bool control)
{
    buf[0] = !!control << 7;
}

static inline bool srt_get_packet_control(const uint8_t *buf)
{
    return !!(buf[0] & (1 << 7));
}

static inline void srt_set_data_packet_seq(uint8_t *buf, uint32_t seq)
{
    buf[0] |= (seq >> 24) & 0x7f;
    buf[1] = (seq >> 16) & 0xff;
    buf[2] = (seq >>  8) & 0xff;
    buf[3] =  seq & 0xff;
}

static inline uint32_t srt_get_data_packet_seq(const uint8_t *buf)
{
    return ((buf[0] & 0x7f) << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

static inline void srt_set_data_packet_position(uint8_t *buf, uint8_t position)
{
    buf[4] = (buf[4] & 0x3f) | ((position & 0x3) << 6);
}

static inline uint8_t srt_get_data_packet_position(const uint8_t *buf)
{
    return (buf[4] >> 6) & 0x3;
}

static inline void srt_set_data_packet_order(uint8_t *buf, bool order)
{
    buf[4] = (buf[4] & 0xdf) | (!!order << 5);
}

static inline bool srt_get_data_packet_order(const uint8_t *buf)
{
    return !!(buf[4] & 0x20);
}

static inline void srt_set_data_packet_encryption(uint8_t *buf, uint8_t encryption)
{
    buf[4] = (buf[4] & 0xe7) | ((encryption & 0x3) << 3);
}

static inline uint8_t srt_get_data_packet_encryption(const uint8_t *buf)
{
    return (buf[4] >> 3) & 0x3;
}

static inline void srt_set_data_packet_retransmit(uint8_t *buf, bool retransmit)
{
    buf[4] = (buf[4] & 0xfb) | (!!retransmit << 2);
}

static inline bool srt_get_data_packet_retransmit(const uint8_t *buf)
{
    return !!(buf[4] & 0x4);
}

static inline void srt_set_data_packet_message_number(uint8_t *buf, uint32_t message_number)
{
    buf[4] |= (message_number >> 24) & 0x3;
    buf[5] = (message_number >> 16) & 0xff;
    buf[6] = (message_number >>  8) & 0xff;
    buf[7] =  message_number & 0xff;
}

static inline uint32_t srt_get_data_packet_message_number(const uint8_t *buf)
{
    return ((buf[4] & 0x3) << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];
}

static inline void srt_set_packet_timestamp(uint8_t *buf, uint32_t timestamp)
{
    buf[ 8] = (timestamp >> 24) & 0xff;
    buf[ 9] = (timestamp >> 16) & 0xff;
    buf[10] = (timestamp >>  8) & 0xff;
    buf[11] =  timestamp & 0xff;
}

static inline uint32_t srt_get_packet_timestamp(const uint8_t *buf)
{
    return (buf[8] << 24) | (buf[9] << 16) | (buf[10] << 8) | buf[11];
}

static inline void srt_set_packet_dst_socket_id(uint8_t *buf, uint32_t dst_socket_id)
{
    buf[12] = (dst_socket_id >> 24) & 0xff;
    buf[13] = (dst_socket_id >> 16) & 0xff;
    buf[14] = (dst_socket_id >>  8) & 0xff;
    buf[15] =  dst_socket_id & 0xff;
}

static inline uint32_t srt_get_packet_dst_socket_id(const uint8_t *buf)
{
    return (buf[12] << 24) | (buf[13] << 16) | (buf[14] << 8) | buf[15];
}

static inline void srt_set_control_packet_type(uint8_t *buf, uint16_t type)
{
    type &= ~(1 << 15);
    buf[0] |= type >> 8;
    buf[1] = type & 0xff;
}

static inline uint16_t srt_get_control_packet_type(const uint8_t *buf)
{
    return ((buf[0] & 0x7f) << 8) | buf[1];
}

static inline void srt_set_control_packet_subtype(uint8_t *buf, uint16_t subtype)
{
    buf[2] = subtype >> 8;
    buf[3] = subtype & 0xff;
}

static inline uint16_t srt_get_control_packet_subtype(const uint8_t *buf)
{
    return (buf[2] << 8) | buf[3];
}

static inline void srt_set_control_packet_type_specific(uint8_t *buf, uint32_t type_specific)
{
    buf[4] = (type_specific >> 24) & 0xff;
    buf[5] = (type_specific >> 16) & 0xff;
    buf[6] = (type_specific >>  8) & 0xff;
    buf[7] =  type_specific & 0xff;
}

static inline uint32_t srt_get_control_packet_type_specific(const uint8_t *buf)
{
    return (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];
}

static inline const uint8_t *srt_get_control_packet_cif(const uint8_t *buf)
{
    return &buf[SRT_HEADER_SIZE];
}

/*
   SRT Handshake Control Information Field

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                            Version                            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |        Encryption Field       |        Extension Field        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                 Initial Packet Sequence Number                |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                 Maximum Transmission Unit Size                |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Maximum Flow Window Size                   |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                         Handshake Type                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                         SRT Socket ID                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           SYN Cookie                          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                                                               |
   +                                                               +
   |                                                               |
   +                        Peer IP Address                        +
   |                                                               |
   +                                                               +
   |                                                               |
   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
   |         Extension Type        |        Extension Length       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                                                               |
   +                       Extension Contents                      +
   |                                                               |
   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

*/

#define SRT_HANDSHAKE_VERSION_MIN   4
#define SRT_HANDSHAKE_VERSION       5

#define SRT_HANDSHAKE_CIF_SIZE          48
#define SRT_HANDSHAKE_CIF_EXTENSION_MIN_SIZE    4

#define SRT_HANDSHAKE_CIPHER_NONE       0
#define SRT_HANDSHAKE_CIPHER_AES_128    2
#define SRT_HANDSHAKE_CIPHER_AES_192    3
#define SRT_HANDSHAKE_CIPHER_AES_256    4

#define SRT_HANDSHAKE_TYPE_DONE 0xFFFFFFFD
#define SRT_HANDSHAKE_TYPE_AGREEMENT 0xFFFFFFFE
#define SRT_HANDSHAKE_TYPE_CONCLUSION 0xFFFFFFFF
#define SRT_HANDSHAKE_TYPE_WAVEHAND 0
#define SRT_HANDSHAKE_TYPE_INDUCTION 1
#define SRT_HANDSHAKE_TYPE_REJ_UNKNOWN 1000
#define SRT_HANDSHAKE_TYPE_REJ_SYSTEM 1001
#define SRT_HANDSHAKE_TYPE_REJ_PEER 1002
#define SRT_HANDSHAKE_TYPE_REJ_RESOURCE 1003
#define SRT_HANDSHAKE_TYPE_REJ_ROGUE 1004
#define SRT_HANDSHAKE_TYPE_REJ_BACKLOG 1005
#define SRT_HANDSHAKE_TYPE_REJ_IPE 1006
#define SRT_HANDSHAKE_TYPE_REJ_CLOSE 1007
#define SRT_HANDSHAKE_TYPE_REJ_VERSION 1008
#define SRT_HANDSHAKE_TYPE_REJ_RDVCOOKIE 1009
#define SRT_HANDSHAKE_TYPE_REJ_BADSECRET 1010
#define SRT_HANDSHAKE_TYPE_REJ_UNSECURE 1011
#define SRT_HANDSHAKE_TYPE_REJ_MESSAGEAPI 1012
#define SRT_HANDSHAKE_TYPE_REJ_CONGESTION 1013
#define SRT_HANDSHAKE_TYPE_REJ_FILTER 1014
#define SRT_HANDSHAKE_TYPE_REJ_GROUP 1015

static inline void srt_set_handshake_version(uint8_t *cif, uint32_t version)
{
    cif[0] = (version >> 24) & 0xff;
    cif[1] = (version >> 16) & 0xff;
    cif[2] = (version >>  8) & 0xff;
    cif[3] =  version & 0xff;
}

static inline uint32_t srt_get_handshake_version(const uint8_t *cif)
{
    return (cif[0] << 24) | (cif[1] << 16) | (cif[2] << 8) | cif[3];
}

static inline void srt_set_handshake_encryption(uint8_t *cif, uint16_t encryption)
{
    cif[4] = (encryption >>  8) & 0xff;
    cif[5] =  encryption & 0xff;
}

static inline uint16_t srt_get_handshake_encryption(const uint8_t *cif)
{
    return (cif[4] << 8) | cif[5];
}

static inline void srt_set_handshake_extension(uint8_t *cif, uint16_t extension)
{
    cif[6] = (extension >>  8) & 0xff;
    cif[7] =  extension & 0xff;
}

static inline uint16_t srt_get_handshake_extension(const uint8_t *cif)
{
    return (cif[6] << 8) | cif[7];
}

static inline void srt_set_handshake_isn(uint8_t *cif, uint32_t isn)
{
    cif[8] = (isn >> 24) & 0xff;
    cif[9] = (isn >> 16) & 0xff;
    cif[10] = (isn >>  8) & 0xff;
    cif[11] =  isn & 0xff;
}

static inline uint32_t srt_get_handshake_isn(const uint8_t *cif)
{
    return (cif[8] << 24) | (cif[9] << 16) | (cif[10] << 8) | cif[11];
}

static inline void srt_set_handshake_mtu(uint8_t *cif, uint32_t mtu)
{
    cif[12] = (mtu >> 24) & 0xff;
    cif[13] = (mtu >> 16) & 0xff;
    cif[14] = (mtu >>  8) & 0xff;
    cif[15] =  mtu & 0xff;
}

static inline uint32_t srt_get_handshake_mtu(const uint8_t *cif)
{
    return (cif[12] << 24) | (cif[13] << 16) | (cif[14] << 8) | cif[15];
}

static inline void srt_set_handshake_mfw(uint8_t *cif, uint32_t mfw)
{
    cif[16] = (mfw >> 24) & 0xff;
    cif[17] = (mfw >> 16) & 0xff;
    cif[18] = (mfw >>  8) & 0xff;
    cif[19] =  mfw & 0xff;
}

static inline uint32_t srt_get_handshake_mfw(const uint8_t *cif)
{
    return (cif[16] << 24) | (cif[17] << 16) | (cif[18] << 8) | cif[19];
}

static inline void srt_set_handshake_type(uint8_t *cif, uint32_t type)
{
    cif[20] = (type >> 24) & 0xff;
    cif[21] = (type >> 16) & 0xff;
    cif[22] = (type >>  8) & 0xff;
    cif[23] =  type & 0xff;
}

static inline uint32_t srt_get_handshake_type(const uint8_t *cif)
{
    return (cif[20] << 24) | (cif[21] << 16) | (cif[22] << 8) | cif[23];
}

static inline void srt_set_handshake_socket_id(uint8_t *cif, uint32_t socket_id)
{
    cif[24] = (socket_id >> 24) & 0xff;
    cif[25] = (socket_id >> 16) & 0xff;
    cif[26] = (socket_id >>  8) & 0xff;
    cif[27] =  socket_id & 0xff;
}

static inline uint32_t srt_get_handshake_socket_id(const uint8_t *cif)
{
    return (cif[24] << 24) | (cif[25] << 16) | (cif[26] << 8) | cif[27];
}

static inline void srt_set_handshake_syn_cookie(uint8_t *cif, uint32_t syn_cookie)
{
    cif[28] = (syn_cookie >> 24) & 0xff;
    cif[29] = (syn_cookie >> 16) & 0xff;
    cif[30] = (syn_cookie >>  8) & 0xff;
    cif[31] =  syn_cookie & 0xff;
}

static inline uint32_t srt_get_handshake_syn_cookie(const uint8_t *cif)
{
    return (cif[28] << 24) | (cif[29] << 16) | (cif[30] << 8) | cif[31];
}

static inline void srt_set_handshake_ip(uint8_t *cif, const struct sockaddr *addr)
{
    uint32_t ip;

    switch (addr->sa_family) {
        case AF_INET: {
            const struct sockaddr_in *in = (const struct sockaddr_in *)addr;
            memcpy(&ip, &in->sin_addr.s_addr, 4);
            cif[32] = (ip >> 24) & 0xff;
            cif[33] = (ip >> 16) & 0xff;
            cif[34] = (ip >>  8) & 0xff;
            cif[35] =  ip        & 0xff;
            memset(&cif[36], 0, 12);
            break;
        }
        case AF_INET6: {
            const struct sockaddr_in6 *in6 = (const struct sockaddr_in6 *)addr;
            for (int i = 0; i < 4; i++) {
                memcpy(&ip, &in6->sin6_addr.s6_addr32[i], 4);
                cif[32+4*i] = (ip >> 24) & 0xff;
                cif[33+4*i] = (ip >> 16) & 0xff;
                cif[34+4*i] = (ip >>  8) & 0xff;
                cif[35+4*i] =  ip        & 0xff;
            }
            break;
        }
    }
}

static inline void srt_get_handshake_ip(const uint8_t *cif, struct sockaddr *addr)
{
    uint32_t ip0 = (cif[32] << 24) | (cif[33] << 16) | (cif[34] << 8) | (cif[35]);
    uint32_t ip1 = (cif[36] << 24) | (cif[37] << 16) | (cif[38] << 8) | (cif[39]);
    uint32_t ip2 = (cif[40] << 24) | (cif[41] << 16) | (cif[42] << 8) | (cif[43]);
    uint32_t ip3 = (cif[44] << 24) | (cif[45] << 16) | (cif[46] << 8) | (cif[47]);

    if (!ip1 && !ip2 && !ip3) {
        struct sockaddr_in *in = (struct sockaddr_in *)addr;
        in->sin_family = AF_INET;
        memcpy(&in->sin_addr.s_addr, &ip0, 4); /* why ? */
    } else {
        struct sockaddr_in6 *in6 = (struct sockaddr_in6 *)addr;
        in6->sin6_family = AF_INET6;
        memcpy(&in6->sin6_addr.s6_addr32[0], &ip0, 4); /* why ? */
        memcpy(&in6->sin6_addr.s6_addr32[1], &ip1, 4); /* why ? */
        memcpy(&in6->sin6_addr.s6_addr32[2], &ip2, 4); /* why ? */
        memcpy(&in6->sin6_addr.s6_addr32[3], &ip3, 4); /* why ? */
    }
}

static inline void srt_set_handshake_extension_type(uint8_t *ext, uint16_t extension_type)
{
    ext[0] = (extension_type >>  8) & 0xff;
    ext[1] =  extension_type & 0xff;
}

static inline uint16_t srt_get_handshake_extension_type(const uint8_t *ext)
{
    return (ext[0] << 8) | ext[1];
}

static inline void srt_set_handshake_extension_len(uint8_t *ext, uint16_t extension_len)
{
    ext[2] = (extension_len >>  8) & 0xff;
    ext[3] =  extension_len & 0xff;
}

static inline uint16_t srt_get_handshake_extension_len(const uint8_t *ext)
{
    return (ext[2] << 8) | ext[3];
}

static inline uint8_t *srt_get_handshake_extension_buf(uint8_t *cif)
{
    return &cif[SRT_HANDSHAKE_CIF_SIZE];
}

static inline bool srt_check_handshake(const uint8_t *cif, size_t n)
{
    if (n < SRT_HANDSHAKE_CIF_SIZE)
        return false;

    uint32_t v = srt_get_handshake_version(cif);

    if (v < SRT_HANDSHAKE_VERSION_MIN)
        return false;

    if (v > SRT_HANDSHAKE_VERSION)
        return false;

    n -= SRT_HANDSHAKE_CIF_SIZE;

    uint16_t ext = srt_get_handshake_extension(cif);

    if (v == SRT_HANDSHAKE_VERSION && ext && ext != SRT_MAGIC_CODE) {
        const uint8_t *ext = srt_get_handshake_extension_buf((uint8_t*)cif);
        while (n) {
            if (n < SRT_HANDSHAKE_CIF_EXTENSION_MIN_SIZE)
                return false;
            n -= SRT_HANDSHAKE_CIF_EXTENSION_MIN_SIZE;
            uint16_t extension_len = 4 * srt_get_handshake_extension_len(ext);
            if (n < extension_len)
                return false;
            n -= extension_len;
            ext += SRT_HANDSHAKE_CIF_EXTENSION_MIN_SIZE + extension_len;
        }
    }

    return true;
}

#define SRT_HANDSHAKE_EXT_HSREQ     (1 << 0)
#define SRT_HANDSHAKE_EXT_KMREQ     (1 << 1)
#define SRT_HANDSHAKE_EXT_CONFIG    (1 << 2)

#define SRT_HANDSHAKE_EXT_TYPE_HSREQ        1
#define SRT_HANDSHAKE_EXT_TYPE_HSRSP        2
#define SRT_HANDSHAKE_EXT_TYPE_KMREQ        3
#define SRT_HANDSHAKE_EXT_TYPE_KMRSP        4
#define SRT_HANDSHAKE_EXT_TYPE_SID          5
#define SRT_HANDSHAKE_EXT_TYPE_CONGESTION   6
#define SRT_HANDSHAKE_EXT_TYPE_FILTER       7
#define SRT_HANDSHAKE_EXT_TYPE_GROUP        8

/* HSREQ */

#define SRT_HANDSHAKE_HSREQ_SIZE    12

#define SRT_HANDSHAKE_EXT_FLAG_TSBPDSND         (1<<0)
#define SRT_HANDSHAKE_EXT_FLAG_TSBPDRCV         (1<<1)
#define SRT_HANDSHAKE_EXT_FLAG_CRYPT            (1<<2)
#define SRT_HANDSHAKE_EXT_FLAG_TLPKTDROP        (1<<3)
#define SRT_HANDSHAKE_EXT_FLAG_PERIODICNAK      (1<<4)
#define SRT_HANDSHAKE_EXT_FLAG_REXMITFLG        (1<<5)
#define SRT_HANDSHAKE_EXT_FLAG_STREAM           (1<<6)
#define SRT_HANDSHAKE_EXT_FLAG_PACKET_FILTER    (1<<7)

static inline void srt_set_handshake_extension_srt_version(uint8_t *ext, uint16_t major, uint8_t minor, uint8_t patch)
{
    uint32_t srt_version = (major << 16) | (minor << 8) | patch;
    ext[0] = (srt_version >> 24) & 0xff;
    ext[1] = (srt_version >> 16) & 0xff;
    ext[2] = (srt_version >>  8) & 0xff;
    ext[3] =  srt_version & 0xff;
}

static inline void srt_get_handshake_extension_srt_version(const uint8_t *ext, uint16_t *major, uint8_t *minor, uint8_t *patch)
{
    uint32_t version = (ext[0] << 24) | (ext[1] << 16) | (ext[2] << 8) | ext[3];
    *major = version >> 16;
    *minor = (version >> 8) & 0xff;
    *patch = version & 0xff;
}

static inline void srt_set_handshake_extension_srt_flags(uint8_t *ext, uint32_t srt_flags)
{
    ext[4] = (srt_flags >> 24) & 0xff;
    ext[5] = (srt_flags >> 16) & 0xff;
    ext[6] = (srt_flags >>  8) & 0xff;
    ext[7] =  srt_flags & 0xff;
}

static inline uint32_t srt_get_handshake_extension_srt_flags(const uint8_t *ext)
{
    return (ext[4] << 24) | (ext[5] << 16) | (ext[6] << 8) | ext[7];
}

static inline void srt_set_handshake_extension_receiver_tsbpd_delay(uint8_t *ext, uint16_t receiver_tsbpd_delay)
{
    ext[8] = (receiver_tsbpd_delay >>  8) & 0xff;
    ext[9] =  receiver_tsbpd_delay & 0xff;
}

static inline uint16_t srt_get_handshake_extension_receiver_tsbpd_delay(const uint8_t *ext)
{
    return (ext[8] << 8) | ext[9];
}

static inline void srt_set_handshake_extension_sender_tsbpd_delay(uint8_t *ext, uint16_t sender_tsbpd_delay)
{
    ext[10] = (sender_tsbpd_delay >>  8) & 0xff;
    ext[11] =  sender_tsbpd_delay & 0xff;
}

static inline uint16_t srt_get_handshake_extension_sender_tsbpd_delay(const uint8_t *ext)
{
    return (ext[10] << 8) | ext[11];
}

/* KMREQ */

#define SRT_KMREQ_COMMON_SIZE   32

#define SRT_KMREQ_CIPHER_NONE   0
#define SRT_KMREQ_CIPHER_AES    2

static inline uint8_t srt_km_get_kk(const uint8_t *km)
{
    return km[3] & 3;
}

static inline void srt_km_set_kk(uint8_t *km, const uint8_t kk)
{
    km[3] = (km[3] & ~3) | kk;
}

static inline uint8_t srt_km_get_cipher(const uint8_t *km)
{
    return km[8];
}

static inline void srt_km_set_cipher(uint8_t *km, const uint8_t cipher)
{
    km[8] = cipher;
}

static inline uint8_t srt_km_get_klen(const uint8_t *km)
{
    return km[15];
}

static inline void srt_km_set_klen(uint8_t *km, const uint8_t klen)
{
    km[15] = klen;
}

static inline const uint8_t *srt_km_get_salt(const uint8_t *km)
{
    return &km[16];
}

static inline void srt_km_set_salt(uint8_t *km, const uint8_t *salt)
{
    memcpy(&km[16], salt, 128/8);
}

static inline uint8_t *srt_km_get_wrap(uint8_t *km)
{
    return &km[32];
}

static inline bool srt_check_km(const uint8_t *km, size_t n)
{
    if (n < SRT_KMREQ_COMMON_SIZE)
        return false;

    if (km[0] & (1 << 7)) // S
        return false;

    if (((km[0] >> 4) & 7) != 1) // V
        return false;

    if ((km[0] & 0xf) != 2) // PT == KMmsg
        return false;

    if ((km[1] != 0x20) || (km[2] != 0x29)) // Sign
        return false;

    if (km[3] & 0xfc) // Resv1
        return false;

    uint8_t kk = srt_km_get_kk(km);
    if (kk == 0) // KK == 00b is invalid
        return false;

    if (km[4] || km[5] || km[6] || km[7]) // KEKI == 0
        return false;

    uint8_t cipher = srt_km_get_cipher(km);
    if (cipher != SRT_KMREQ_CIPHER_NONE && cipher != SRT_KMREQ_CIPHER_AES)
        return false;

    if (km[9]) // Auth == 0
        return false;

    if (km[10] != 2) // SE
        return false;

    if (km[11]) // Resv2
        return false;

    if (km[12] || km[13]) // Resv3
        return false;

    if (km[14] != 4) // SLen
        return false;

    uint8_t klen = srt_km_get_klen(km);
    if (klen != 4 && klen != 6 && klen != 8)
        return false;
    klen *= 4;

    if (n != SRT_KMREQ_COMMON_SIZE + 8 + klen * ((kk == 3) ? 2 : 1))
        return false;

    return true;
}

/*
   SRT ACK Control Information Field

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |            Last Acknowledged Packet Sequence Number           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                              RTT                              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                          RTT Variance                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                     Available Buffer Size                     |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                     Packets Receiving Rate                    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                     Estimated Link Capacity                   |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                         Receiving Rate                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

#define SRT_ACK_CIF_SIZE_1 16   /* last ack, rtt, rtt variance, available buffer size*/
#define SRT_ACK_CIF_SIZE_2 24   /* packets receiving rate, estimated link capacity */
#define SRT_ACK_CIF_SIZE_3 28   /* receiving rate */
#define SRT_ACK_CIF_SIZE_4 32   /* 1.0.2 only, ? */

static inline bool srt_check_ack(const uint8_t *cif, size_t n)
{
    if (n != SRT_ACK_CIF_SIZE_1 && n != SRT_ACK_CIF_SIZE_2 &&
            n != SRT_ACK_CIF_SIZE_3 && n != SRT_ACK_CIF_SIZE_4)
        return false;

    return true;
}

static inline void srt_set_ack_last_ack_seq(uint8_t *cif, uint32_t last_ack_seq)
{
    cif[0] = (last_ack_seq >> 24) & 0xff;
    cif[1] = (last_ack_seq >> 16) & 0xff;
    cif[2] = (last_ack_seq >>  8) & 0xff;
    cif[3] =  last_ack_seq & 0xff;
}

static inline uint32_t srt_get_ack_last_ack_seq(const uint8_t *cif)
{
    return (cif[0] << 24) | (cif[1] << 16) | (cif[2] << 8) | cif[3];
}

static inline void srt_set_ack_rtt(uint8_t *cif, uint32_t rtt)
{
    cif[4] = (rtt >> 24) & 0xff;
    cif[5] = (rtt >> 16) & 0xff;
    cif[6] = (rtt >>  8) & 0xff;
    cif[7] =  rtt & 0xff;
}

static inline uint32_t srt_get_ack_rtt(const uint8_t *cif)
{
    return (cif[4] << 24) | (cif[5] << 16) | (cif[6] << 8) | cif[7];
}

static inline void srt_set_ack_rtt_variance(uint8_t *cif, uint32_t rtt_variance)
{
    cif[8] = (rtt_variance >> 24) & 0xff;
    cif[9] = (rtt_variance >> 16) & 0xff;
    cif[10] = (rtt_variance >>  8) & 0xff;
    cif[11] =  rtt_variance & 0xff;
}

static inline uint32_t srt_get_ack_rtt_variance(const uint8_t *cif)
{
    return (cif[8] << 24) | (cif[9] << 16) | (cif[10] << 8) | cif[11];
}

static inline void srt_set_ack_avail_bufsize(uint8_t *cif, uint32_t avail_bufsize)
{
    cif[12] = (avail_bufsize >> 24) & 0xff;
    cif[13] = (avail_bufsize >> 16) & 0xff;
    cif[14] = (avail_bufsize >>  8) & 0xff;
    cif[15] =  avail_bufsize & 0xff;
}

static inline uint32_t srt_get_ack_avail_bufsize(const uint8_t *cif)
{
    return (cif[12] << 24) | (cif[13] << 16) | (cif[14] << 8) | cif[15];
}

static inline void srt_set_ack_packets_receiving_rate(uint8_t *cif, uint32_t packets_receiving_rate)
{
    cif[16] = (packets_receiving_rate >> 24) & 0xff;
    cif[17] = (packets_receiving_rate >> 16) & 0xff;
    cif[18] = (packets_receiving_rate >>  8) & 0xff;
    cif[19] =  packets_receiving_rate & 0xff;
}

static inline uint32_t srt_get_ack_packets_receiving_rate(const uint8_t *cif)
{
    return (cif[16] << 24) | (cif[17] << 16) | (cif[18] << 8) | cif[19];
}

static inline void srt_set_ack_estimated_link_capacity(uint8_t *cif, uint32_t estimated_link_capacity)
{
    cif[20] = (estimated_link_capacity >> 24) & 0xff;
    cif[21] = (estimated_link_capacity >> 16) & 0xff;
    cif[22] = (estimated_link_capacity >>  8) & 0xff;
    cif[23] =  estimated_link_capacity & 0xff;
}

static inline uint32_t srt_get_ack_estimated_link_capacity(const uint8_t *cif)
{
    return (cif[20] << 24) | (cif[21] << 16) | (cif[22] << 8) | cif[23];
}

static inline void srt_set_ack_receiving_rate(uint8_t *cif, uint32_t receiving_rate)
{
    cif[24] = (receiving_rate >> 24) & 0xff;
    cif[25] = (receiving_rate >> 16) & 0xff;
    cif[26] = (receiving_rate >>  8) & 0xff;
    cif[27] =  receiving_rate & 0xff;
}

static inline uint32_t srt_get_ack_receiving_rate(const uint8_t *cif)
{
    return (cif[24] << 24) | (cif[25] << 16) | (cif[26] << 8) | cif[27];
}

/*
   SRT NAK Control Information Field

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |0|                 Lost packet sequence number                 |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |1|         Range of lost packets from sequence number          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |0|                    Up to sequence number                    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |0|                 Lost packet sequence number                 |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

static inline bool srt_get_nak_range(const uint8_t **p_cif, size_t *p_cif_size, uint32_t *start_seq, uint32_t *packets)
{
    const uint8_t *cif = *p_cif;
    if (*p_cif_size < 4)
        return false;

    uint32_t first_seq = (cif[0] << 24) | (cif[1] << 16) | (cif[2] << 8) | cif[3];
    if (first_seq & (1 << 31)) {
        if (*p_cif_size < 8)
            return false;

        first_seq &= ~(1<<31);

        uint32_t last_seq = (cif[4] << 24) | (cif[5] << 16) | (cif[6] << 8) | cif[7];
        if (last_seq & (1 << 31))
            return false;

        *p_cif_size -= 8;
        *p_cif += 8;

        *start_seq = first_seq;
        *packets = (last_seq - first_seq + 1) & ~(1<<31);
    } else {
        *p_cif_size -= 4;
        *p_cif += 4;
        *start_seq = first_seq;
        *packets = 1;
    }

    return true;
}


/*
   SRT DROPREQ Control Information Field

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                 First Packet Sequence Number                  |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  Last Packet Sequence Number                  |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

#define SRT_DROPREQ_CIF_SIZE 8

static inline bool srt_check_dropreq(const uint8_t *cif, size_t n)
{
    if (n != SRT_DROPREQ_CIF_SIZE)
        return false;

    return true;
}

static inline void srt_set_dropreq_first_seq(uint8_t *cif, uint32_t first_seq)
{
    cif[0] = (first_seq >> 24) & 0x7f;
    cif[1] = (first_seq >> 16) & 0xff;
    cif[2] = (first_seq >>  8) & 0xff;
    cif[3] =  first_seq & 0xff;
}

static inline uint32_t srt_get_dropreq_first_seq(const uint8_t *cif)
{
    return ((cif[0] << 24) & 0x7f) | (cif[1] << 16) | (cif[2] << 8) | cif[3];
}

static inline void srt_set_dropreq_last_seq(uint8_t *cif, uint32_t last_seq)
{
    cif[4] = (last_seq >> 24) & 0x7f;
    cif[5] = (last_seq >> 16) & 0xff;
    cif[6] = (last_seq >>  8) & 0xff;
    cif[7] =  last_seq & 0xff;
}

static inline uint32_t srt_get_dropreq_last_seq(const uint8_t *cif)
{
    return ((cif[4] << 24) & 0x7f) | (cif[5] << 16) | (cif[6] << 8) | cif[7];
}


#ifdef __cplusplus
}
#endif

#endif
