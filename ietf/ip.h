/*****************************************************************************
 * ip.h: Internet Protocol (IP)
 *****************************************************************************
 * Copyright (C) 2015 VideoLAN
 *
 * Authors: Benjamin Cohen <bencoh@notk.org>
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
 *  - IETF RFC 791 INTERNET PROTOCOL (September 1981)
 *  - IETF RFC 790 ASSIGNED NUMBERS (September 1981)
 */

#ifndef __BITSTREAM_IETF_IP_H__
#define __BITSTREAM_IETF_IP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Reminder: IP header
    0                   1                   2                   3   
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |Version|  IHL  |Type of Service|          Total Length         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |         Identification        |Flags|      Fragment Offset    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  Time to Live |    Protocol   |         Header Checksum       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                       Source Address                          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Destination Address                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Options                    |    Padding    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

#define IP_HEADER_MINSIZE   20

#define IP_PROTO_ICMP       1
#define IP_PROTO_TCP        6
#define IP_PROTO_UDP        17

static inline void ip_set_version(uint8_t *p_ip, uint8_t version)
{
    p_ip[0] &= ~0xf0;
    p_ip[0] |= (version & 0xf) << 4;
}

static inline uint8_t ip_get_version(const uint8_t *p_ip)
{
    return (p_ip[0] >> 4) & 0xf;
}

static inline void ip_set_ihl(uint8_t *p_ip, uint8_t ihl)
{
    p_ip[0] &= ~0xf;
    p_ip[0] |= (ihl & 0xf);
}

static inline uint8_t ip_get_ihl(const uint8_t *p_ip)
{
    return p_ip[0] & 0xf;
}

static inline void ip_set_tos(uint8_t *p_ip, uint8_t tos)
{
    p_ip[1] = tos;
}

static inline uint8_t ip_get_tos(const uint8_t *p_ip)
{
    return p_ip[1];
}

static inline void ip_set_len(uint8_t *p_ip, uint16_t len)
{
    p_ip[2] = (len & 0xff00) >> 8;
    p_ip[3] = (len & 0xff);
}

static inline uint16_t ip_get_len(const uint8_t *p_ip)
{
    return (p_ip[2] << 8) | p_ip[3];
}

static inline void ip_set_id(uint8_t *p_ip, uint16_t id)
{
    
    p_ip[4] = (id & 0xff00) >> 8;
    p_ip[5] = (id & 0xff);
}

static inline uint16_t ip_get_id(const uint8_t *p_ip)
{
    return (p_ip[4] << 8) | p_ip[5];
}

static inline void ip_set_flag_reserved(uint8_t *p_ip, uint8_t flag)
{
    p_ip[6] &= ~0x80;
    p_ip[6] |= (flag & 1) << 7;
}

static inline uint8_t ip_get_flag_reservered(const uint8_t *p_ip)
{
    return (p_ip[6] & 0x80);
}

static inline void ip_set_flag_df(uint8_t *p_ip, uint8_t flag)
{
    p_ip[6] &= ~0x40;
    p_ip[6] |= (flag & 1) << 6;
}

static inline uint8_t ip_get_flag_df(const uint8_t *p_ip)
{
    return (p_ip[6] & 0x40);
}

static inline void ip_set_flag_mf(uint8_t *p_ip, uint8_t flag)
{
    p_ip[6] &= ~0x20;
    p_ip[6] |= (flag & 1) << 5;
}

static inline uint8_t ip_get_flag_mf(const uint8_t *p_ip)
{
    return (p_ip[6] & 0x20);
}

static inline void ip_set_frag_offset(uint8_t *p_ip, uint16_t offset)
{
    p_ip[6] &= ~0x1f;
    p_ip[6] |= (offset & 0x1f00) >> 8;
    p_ip[7] = (offset & 0xff);
}

static inline uint8_t ip_get_frag_offset(const uint8_t *p_ip)
{
    return ((p_ip[6] & 0x1f) << 8 | p_ip[7]);
}

static inline void ip_set_ttl(uint8_t *p_ip, uint8_t ttl)
{
    p_ip[8] = ttl;
}

static inline uint8_t ip_get_ttl(const uint8_t *p_ip)
{
    return p_ip[8];
}

static inline void ip_set_proto(uint8_t *p_ip, uint8_t proto)
{
    p_ip[9] = proto;
}

static inline uint8_t ip_get_proto(const uint8_t *p_ip)
{
    return p_ip[9];
}

static inline void ip_set_cksum(uint8_t *p_ip, uint16_t cksum)
{
    p_ip[10] = (cksum & 0xff00) >> 8;
    p_ip[11] = (cksum & 0xff);
}

static inline uint16_t ip_get_cksum(const uint8_t *p_ip)
{
    return (p_ip[10] << 8) | p_ip[11];
}

static inline void ip_set_srcaddr(uint8_t *p_ip, uint32_t addr)
{
    p_ip[12] = (addr & 0xff000000) >> 24;
    p_ip[13] = (addr & 0x00ff0000) >> 16;
    p_ip[14] = (addr & 0x0000ff00) >>  8;
    p_ip[15] = (addr & 0x000000ff);
}

static inline uint32_t ip_get_srcaddr(const uint8_t *p_ip)
{
    return (p_ip[12] << 24) | (p_ip[13] << 16) | (p_ip[14] << 8) | p_ip[15];
}

static inline uint8_t *ip_srcaddr(uint8_t *p_ip)
{
    return p_ip + 12;
}

static inline void ip_set_dstaddr(uint8_t *p_ip, uint32_t addr)
{
    p_ip[16] = (addr & 0xff000000) >> 24;
    p_ip[17] = (addr & 0x00ff0000) >> 16;
    p_ip[18] = (addr & 0x0000ff00) >>  8;
    p_ip[19] = (addr & 0x000000ff);
}

static inline uint8_t *ip_dstaddr(uint8_t *p_ip)
{
    return p_ip + 16;
}

static inline uint32_t ip_get_dstaddr(const uint8_t *p_ip)
{
    return (p_ip[16] << 24) | (p_ip[17] << 16) | (p_ip[18] << 8) | p_ip[19];
}

static inline uint8_t *ip_payload(uint8_t *p_ip)
{
    return p_ip + 4 * ip_get_ihl(p_ip); /* ihl is in 32b words */
}

#ifdef __cplusplus
}
#endif

#endif
