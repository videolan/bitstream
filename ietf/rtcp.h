/*
 * Copyright (C) 2018 OpenHeadend S.A.R.L.
 *
 * Authors: Arnaud de Turckheim
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
 */

#ifndef __BITSTREAM_IETF_RTCP_H__
# define __BITSTREAM_IETF_RTCP_H__

# include <inttypes.h>

# define RTCP_RTP_VERSION       2
# define RTCP_PT_APP            204

static inline void rtcp_set_rtp_version(uint8_t *p_rtcp)
{
    p_rtcp[0] = RTCP_RTP_VERSION << 6;
}

static inline uint8_t rtcp_get_rc(const uint8_t *p_rtcp)
{
    return p_rtcp[0] & 0x1f;
}

static inline void rtcp_set_rc(uint8_t *p_rtcp, uint8_t rc)
{
    p_rtcp[0] |= rc & 0x1f;
}

static inline uint8_t rtcp_get_pt(const uint8_t *p_rtcp)
{
    return p_rtcp[1];
}

static inline void rtcp_set_pt(uint8_t *p_rtcp, uint8_t pt)
{
    p_rtcp[1] = pt;
}

static inline uint16_t rtcp_get_length(const uint8_t *p_rtcp)
{
    return (p_rtcp[2] << 8) | p_rtcp[3];
}

static inline void rtcp_set_length(uint8_t *p_rtcp,
                                      uint16_t length)
{
    p_rtcp[2] = length >> 8;
    p_rtcp[3] = length & 0xff;
}

# include <bitstream/ietf/rtcp_sr.h>

#endif /* !__BITSTREAM_IETF_RTCP_H__ */
