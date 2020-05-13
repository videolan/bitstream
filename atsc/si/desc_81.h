/*****************************************************************************
 * desc_81.h: ATSC A/52 Descriptor 0x81: AC-3 descriptor
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

#ifndef __BITSTREAM_ATSC_DESC_81_H__
#define __BITSTREAM_ATSC_DESC_81_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define DESC81_HEADER_SIZE      (DESC_HEADER_SIZE + 3)

static inline void desc81_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x81);
}

#define DESC81_SAMPLE_RATE_CODE_48                  0x00
#define DESC81_SAMPLE_RATE_CODE_44_1                0x01
#define DESC81_SAMPLE_RATE_CODE_32                  0x02
#define DESC81_SAMPLE_RATE_CODE_48_OR_44_1          0x04
#define DESC81_SAMPLE_RATE_CODE_48_OR_32            0x05
#define DESC81_SAMPLE_RATE_CODE_44_1_OR_32          0x06
#define DESC81_SAMPLE_RATE_CODE_48_OR_44_1_OR_32    0x07

static inline void desc81_set_sample_rate_code(uint8_t *p_desc,
                                               uint8_t sample_rate)
{
    p_desc[2] = (p_desc[2] & 0x3f) | (sample_rate << 5);
}

static inline uint8_t desc81_get_sample_rate_code(const uint8_t *p_desc)
{
    return p_desc[2] >> 5;
}

static inline void desc81_set_bsid(uint8_t *p_desc, uint8_t bsid)
{
    p_desc[2] = (p_desc[2] & 0xd0) | (bsid & 0x1f);
}

static inline uint8_t desc81_get_bsid(const uint8_t *p_desc)
{
    return p_desc[2] & 0x1f;
}

static inline uint8_t desc81_bit_rate_code_from_octetrate(uint64_t octetrate)
{
    unsigned table[19] = {
        32, 40, 48, 56, 64, 80,
        96, 112, 128, 160, 192, 224,
        256, 320, 384, 448, 512, 576,
        640,
    };
    for (unsigned i = 0; i < 19; i++) {
        if (octetrate * 8 == table[i] * 1000)
            return i;
        else if (octetrate * 8 < table[i] * 1000)
            return 0x20 | i;
    }
    return 0xff;
}

static inline void desc81_set_bit_rate_code(uint8_t *p_desc,
                                            uint8_t bit_rate_code)
{
    p_desc[3] = (p_desc[3] & 0x03) | (bit_rate_code << 2);
}

static inline uint8_t desc81_get_bit_rate_code(const uint8_t *p_desc)
{
    return p_desc[3] >> 2;
}

static inline void desc81_set_surround_mode(uint8_t *p_desc,
                                            uint8_t surround_mode)
{
    p_desc[3] = (p_desc[3] & 0xfc) | (surround_mode & 0x03);
}

static inline uint8_t desc81_get_surround_mode(const uint8_t *p_desc)
{
    return p_desc[3] & 0x03;
}

static inline void desc81_set_bsmod(uint8_t *p_desc, uint8_t bsmod)
{
    p_desc[4] = (p_desc[4] & 0x1f) | (bsmod << 5);
}

static inline uint8_t desc81_get_bsmod(const uint8_t *p_desc)
{
    return p_desc[4] >> 5;
}

static inline void desc81_set_num_channels(uint8_t *p_desc,
                                           uint8_t num_channels)
{
    p_desc[4] = (p_desc[4] & 0xe1) | ((num_channels & 0x0f) << 1);
}

static inline uint8_t desc81_get_num_channels(const uint8_t *p_desc)
{
    return (p_desc[4] & 0x1e) >> 1;
}

static inline void desc81_set_full_svc(uint8_t *p_desc, uint8_t full_svc)
{
    p_desc[4] = (p_desc[4] & 0xfe) | (full_svc & 0x01);
}

static inline uint8_t desc81_get_full_svc(const uint8_t *p_desc)
{
    return p_desc[4] & 0x1;
}

static inline void desc81_set_langcod(uint8_t *p_desc, uint8_t langcod)
{
    p_desc[5] = langcod;
}

static inline uint8_t desc81_get_langcod(const uint8_t *p_desc)
{
    return p_desc[5];
}

static inline void desc81_set_langcod2(uint8_t *p_desc, uint8_t langcod2)
{
    if (desc81_get_num_channels(p_desc) == 0)
        p_desc[6] = langcod2;
}

static inline uint8_t desc81_get_langcod2(const uint8_t *p_desc)
{
    return p_desc[6];
}

static inline void desc81_set_mainid(uint8_t *p_desc, uint8_t mainid)
{
    if (desc81_get_bsmod(p_desc) < 2)
        p_desc[7] = (p_desc[7] & 0x1f) | (mainid << 5);
}

static inline uint8_t desc81_get_mainid(const uint8_t *p_desc)
{
    return p_desc[7] >> 5;
}

static inline void desc81_set_priority(uint8_t *p_desc, uint8_t priority)
{
    p_desc[7] = (p_desc[7] & 0xe7) | ((priority & 0x3) << 3);
}

static inline uint8_t desc81_get_priority(const uint8_t *p_desc)
{
    return (p_desc[7] & 0x18) >> 3;
}

static inline void desc81_set_asvcflags(uint8_t *p_desc, uint8_t asvcflags)
{
    p_desc[7] = asvcflags;
}

static inline uint8_t desc81_get_asvcflags(const uint8_t *p_desc)
{
    return p_desc[7];
}

static inline bool desc81_validate(const uint8_t *p_desc)
{
    uint8_t i_size = (DESC81_HEADER_SIZE - DESC_HEADER_SIZE);
    return desc_get_length(p_desc) >= i_size;
}

static inline void desc81_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t sample_rate_code = desc81_get_sample_rate_code(p_desc);
    uint8_t bsid = desc81_get_bsid(p_desc);
    uint8_t bit_rate_code = desc81_get_bit_rate_code(p_desc);
    uint8_t surround_mode = desc81_get_surround_mode(p_desc);
    uint8_t bsmod = desc81_get_bsmod(p_desc);
    uint8_t num_channels = desc81_get_num_channels(p_desc);
    uint8_t full_svc = desc81_get_full_svc(p_desc);

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 "<AC3_DESC"
                 " sample_rate=\"0x%02x\""
                 " bsid=\"0x%02x\""
                 " bit_rate=\"0x%02x\""
                 " surround_mode=\"0x%02x\""
                 " bsmod=\"0x%02x\""
                 " num_channels=\"0x%02x\""
                 " full_svc=\"%u\"/>",
                 sample_rate_code,
                 bsid,
                 bit_rate_code,
                 surround_mode,
                 bsmod,
                 num_channels,
                 full_svc);
        break;
    default:
        pf_print(opaque,
                 "    - desc 81 ac-3"
                 " sample_rate=\"0x%02x\""
                 " bsid=\"0x%02x\""
                 " bit_rate=\"0x%02x\""
                 " surround_mode=\"0x%02x\""
                 " bsmod=\"0x%02x\""
                 " num_channels=\"0x%02x\""
                 " full_svc=\"%u\"",
                 sample_rate_code,
                 bsid,
                 bit_rate_code,
                 surround_mode,
                 bsmod,
                 num_channels,
                 full_svc);
        break;
    }
}

#ifdef __cplusplus
}
#endif

#endif
