/*****************************************************************************
 * ethernet.h: Ethernet frames
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
 *  - IEEE Std 802.3-2012 (December 2012)
 *  - IETF RFC 7042 IANA Considerations and IETF Protocol
 *    and Documentation Usage for IEEE 802 Parameters (October 2013)
 */

#ifndef __BITSTREAM_IEEE_ETHERNET_H__
#define __BITSTREAM_IEEE_ETHERNET_H__

#include <stdint.h>
#include <string.h> /* memcpy */

#ifdef __cplusplus
extern "C"
{
#endif

#define ETHERNET_ADDR_LEN 6
#define ETHERNET_HEADER_LEN (2*ETHERNET_ADDR_LEN + 2)

#define ETHERNET_TYPE_IP            0x0800
#define ETHERNET_TYPE_ARP           0x0806
#define ETHERNET_TYPE_RARP          0x8035
#define ETHERNET_TYPE_VLAN          0x8100
#define ETHERNET_TYPE_IPV6          0x86DD
#define ETHERNET_TYPE_PPP           0x880B
#define ETHERNET_TYPE_MPLS          0x8847
#define ETHERNET_TYPE_PPPOE_DISC    0x8863
#define ETHERNET_TYPE_PPPOE_SESSION 0x8864
#define ETHERNET_TYPE_LLDP          0x88CC

static inline uint8_t *ethernet_dstaddr(uint8_t *p_ethernet)
{
    return p_ethernet;
}

static inline void ethernet_get_dstaddr(const uint8_t *p_ethernet, uint8_t *p_addr)
{
    memcpy(p_addr, p_ethernet, ETHERNET_ADDR_LEN);
}

static inline void ethernet_set_dstaddr(uint8_t *p_ethernet, const uint8_t *p_addr)
{
    memcpy(ethernet_dstaddr(p_ethernet), p_addr, ETHERNET_ADDR_LEN);
}

static inline uint8_t *ethernet_srcaddr(uint8_t *p_ethernet)
{
    return p_ethernet + ETHERNET_ADDR_LEN;
}

static inline void ethernet_get_srcaddr(const uint8_t *p_ethernet, uint8_t *p_addr)
{
    memcpy(p_addr, p_ethernet + ETHERNET_ADDR_LEN, ETHERNET_ADDR_LEN);
}

static inline void ethernet_set_srcaddr(uint8_t *p_ethernet, const uint8_t *p_addr)
{
    memcpy(ethernet_srcaddr(p_ethernet), p_addr, ETHERNET_ADDR_LEN);
}

static inline uint16_t ethernet_get_lentype(const uint8_t *p_ethernet)
{
    return (p_ethernet[12] << 8) | p_ethernet[13];
}

static inline void ethernet_set_lentype(uint8_t *p_ethernet, uint16_t lentype)
{
    p_ethernet[12] = (lentype & 0xff00) >> 8;
    p_ethernet[13] = (lentype & 0xff);
}

static inline uint8_t *ethernet_payload(uint8_t *p_ethernet)
{
    return p_ethernet + ETHERNET_HEADER_LEN;
}

#ifdef __cplusplus
}
#endif

#endif
