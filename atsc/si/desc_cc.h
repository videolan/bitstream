/*****************************************************************************
 * desc_cc.h: ATSC A/52 Descriptor 0xcc: Enhanced AC-3 descriptor
 *****************************************************************************
 * Copyright (C) 2019 OpenHeadend
 *
 * Authors: Arnaud de Turckheim <adeturckheim@openheadend.tv>
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

#ifndef __BITSTREAM_ATSC_DESC_CC_H__
#define __BITSTREAM_ATSC_DESC_CC_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define DESCCC_HEADER_SIZE      (DESC_HEADER_SIZE + 2)

static inline void desccc_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0xcc);
    desc_set_length(p_desc, DESCCC_HEADER_SIZE - DESC_HEADER_SIZE);
    p_desc[2] = 0x80;
    p_desc[3] = 0xc0;
}

static inline void desccc_clear_flags(uint8_t *p_desc)
{
    p_desc[2] = 0x80;
    p_desc[3] = 0xc0;
}

static inline void desccc_set_bsid_flag(uint8_t *p_desc, uint8_t bsid_flag)
{
    p_desc[2] = (p_desc[2] & 0xbf) | ((bsid_flag & 0x1) << 6);
}

static inline uint8_t desccc_get_bsid_flag(const uint8_t *p_desc)
{
    return (p_desc[2] & 0x40) >> 6;
}

static inline void desccc_set_mainid_flag(uint8_t *p_desc, uint8_t mainid_flag)
{
    p_desc[2] = (p_desc[2] & 0xdf) | ((mainid_flag & 0x1) << 5);
}

static inline uint8_t desccc_get_mainid_flag(const uint8_t *p_desc)
{
    return (p_desc[2] & 0x20) >> 5;
}

static inline void desccc_set_asvc_flag(uint8_t *p_desc, uint8_t asvc_flag)
{
    p_desc[2] = (p_desc[2] & 0xef) | ((asvc_flag & 0x1) << 4);
}

static inline uint8_t desccc_get_asvc_flag(const uint8_t *p_desc)
{
    return (p_desc[2] & 0x10) >> 4;
}

static inline void desccc_set_mixinfoexists(uint8_t *p_desc,
                                            uint8_t mixinfoexists)
{
    p_desc[2] = (p_desc[2] & 0xf7) | ((mixinfoexists & 0x1) << 3);
}

static inline uint8_t desccc_get_mixinfoexists(const uint8_t *p_desc)
{
    return (p_desc[2] & 0x08) >> 3;
}

static inline void desccc_set_substream1_flag(uint8_t *p_desc,
                                            uint8_t substream1_flag)
{
    p_desc[2] = (p_desc[2] & 0xfb) | ((substream1_flag & 0x1) << 2);
}

static inline uint8_t desccc_get_substream1_flag(const uint8_t *p_desc)
{
    return (p_desc[2] & 0x04) >> 2;
}

static inline void desccc_set_substream2_flag(uint8_t *p_desc,
                                            uint8_t substream2_flag)
{
    p_desc[2] = (p_desc[2] & 0xfd) | ((substream2_flag & 0x1) << 1);
}

static inline uint8_t desccc_get_substream2_flag(const uint8_t *p_desc)
{
    return (p_desc[2] & 0x02) >> 1;
}

static inline void desccc_set_substream3_flag(uint8_t *p_desc,
                                            uint8_t substream3_flag)
{
    p_desc[2] = (p_desc[2] & 0xfe) | (substream3_flag & 0x1);
}

static inline uint8_t desccc_get_substream3_flag(const uint8_t *p_desc)
{
    return p_desc[2] & 0x01;
}

static inline void desccc_set_full_service_flag(uint8_t *p_desc,
                                                uint8_t full_service_flag)
{
    p_desc[3] = (p_desc[3] & 0xbf) | ((full_service_flag & 0x01) << 6);
}

static inline uint8_t desccc_get_full_service_flag(const uint8_t *p_desc)
{
    return (p_desc[3] & 0x40) >> 6;
}

static inline void desccc_set_audio_service_type(uint8_t *p_desc,
                                                 uint8_t audio_service_type)
{
    p_desc[3] = (p_desc[3] & 0xc7) | ((audio_service_type & 0x07) << 3);
}

static inline uint8_t desccc_get_audio_service_type(const uint8_t *p_desc)
{
    return (p_desc[3] & 0x38) >> 3;
}

static inline void desccc_set_number_of_channels(uint8_t *p_desc,
                                                 uint8_t number_of_channels)
{
    p_desc[3] = (p_desc[3] & 0xf8) | number_of_channels;
}

static inline uint8_t desccc_get_number_of_channels(const uint8_t *p_desc)
{
    return p_desc[3] & 0x07;
}

static inline bool desccc_validate(const uint8_t *p_desc)
{
    uint8_t i_size = (DESCCC_HEADER_SIZE - DESC_HEADER_SIZE);
    return desc_get_length(p_desc) >= i_size;
}

static inline void desccc_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 "<EAC3_DESC "
                 " bsid_flag=\"%u\""
                 " mainid_flag=\"%u\""
                 " asvc_flag=\"%u\""
                 " mixinfoexists=\"%u\""
                 " substream1_flag=\"%u\""
                 " substream2_flag=\"%u\""
                 " substream3_flag=\"%u\""
                 " full_service_flag=\"%u\""
                 " audio_service_type=\"0x%02x\""
                 " number_of_channels=\"0x%02x\""
                 "/>",
                 desccc_get_bsid_flag(p_desc) ,
                 desccc_get_mainid_flag(p_desc) ,
                 desccc_get_asvc_flag(p_desc) ,
                 desccc_get_mixinfoexists(p_desc) ,
                 desccc_get_substream1_flag(p_desc) ,
                 desccc_get_substream2_flag(p_desc) ,
                 desccc_get_substream3_flag(p_desc) ,
                 desccc_get_full_service_flag(p_desc) ,
                 desccc_get_audio_service_type(p_desc),
                 desccc_get_number_of_channels(p_desc));
        break;

    default:
        pf_print(opaque,
                 "    - desc cc eac3"
                 " bsid_flag=\"%u\""
                 " mainid_flag=\"%u\""
                 " asvc_flag=\"%u\""
                 " mixinfoexists=\"%u\""
                 " substream1_flag=\"%u\""
                 " substream2_flag=\"%u\""
                 " substream3_flag=\"%u\""
                 " full_service_flag=\"%u\""
                 " audio_service_type=\"0x%02x\""
                 " number_of_channels=\"0x%02x\"",
                 desccc_get_bsid_flag(p_desc) ,
                 desccc_get_mainid_flag(p_desc) ,
                 desccc_get_asvc_flag(p_desc) ,
                 desccc_get_mixinfoexists(p_desc) ,
                 desccc_get_substream1_flag(p_desc) ,
                 desccc_get_substream2_flag(p_desc) ,
                 desccc_get_substream3_flag(p_desc) ,
                 desccc_get_full_service_flag(p_desc) ,
                 desccc_get_audio_service_type(p_desc),
                 desccc_get_number_of_channels(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
