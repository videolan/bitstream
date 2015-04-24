/*****************************************************************************
 * udp.h: User Datagram Protocol (UDP)
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
 *  - IETF RFC 768 User Datagram Protocol (August 1980)
 */

#ifndef __BITSTREAM_IETF_UDP_H__
#define __BITSTREAM_IETF_UDP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Reminder: UDP header
     0      7 8     15 16    23 24    31  
    +--------+--------+--------+--------+ 
    |     Source      |   Destination   | 
    |      Port       |      Port       | 
    +--------+--------+--------+--------+ 
    |                 |                 | 
    |     Length      |    Checksum     | 
    +--------+--------+--------+--------+ 
 */

#define UDP_HEADER_SIZE 8

static inline void udp_set_srcport(uint8_t *p_udp, uint16_t port)
{
    p_udp[0] = (port & 0xff00) >> 8;
    p_udp[1] = (port & 0xff);
}

static inline uint16_t udp_get_srcport(const uint8_t *p_udp)
{
    return (p_udp[0] << 8) | p_udp[1];
}

static inline void udp_set_dstport(uint8_t *p_udp, uint16_t port)
{
    p_udp[2] = (port & 0xff00) >> 8;
    p_udp[3] = (port & 0xff);
}

static inline uint16_t udp_get_dstport(const uint8_t *p_udp)
{
    return (p_udp[2] << 8) | p_udp[3];
}

static inline void udp_set_len(uint8_t *p_udp, uint16_t len)
{
    p_udp[4] = (len & 0xff00) >> 8;
    p_udp[5] = (len & 0xff);
}

static inline uint16_t udp_get_len(const uint8_t *p_udp)
{
    return (p_udp[4] << 8) | p_udp[5];
}

static inline void udp_set_cksum(uint8_t *p_udp, uint16_t cksum)
{
    p_udp[6] = (cksum & 0xff00) >> 8;
    p_udp[7] = (cksum & 0xff);
}

static inline uint16_t udp_get_cksum(const uint8_t *p_udp)
{
    return (p_udp[6] << 8) | p_udp[7];
}

static inline uint8_t *udp_payload(uint8_t *p_udp)
{
    return p_udp + UDP_HEADER_SIZE;
}

#ifdef __cplusplus
}
#endif

#endif
