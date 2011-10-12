/*****************************************************************************
 * desc_67.h: ETSI EN 300 468 Descriptor 0x67: Transport stream descriptor
 *****************************************************************************
 * Copyright (C) 2011 Unix Solutions Ltd.
 *
 * Authors: Georgi Chorbadzhiyski <georgi@unixsol.org>
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
 *  - ETSI EN 300 468 V1.11.1 (2010-04) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_DESC_67_H__
#define __BITSTREAM_DVB_DESC_67_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x67: Transport stream descriptor
 *****************************************************************************/
#define DESC67_HEADER_SIZE      DESC_HEADER_SIZE

static inline void desc67_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x67);
    desc_set_length(p_desc, 0);
}

static inline uint8_t desc67_get_bytes_length(const uint8_t *p_desc)
{
    return desc_get_length(p_desc);
}

static inline const uint8_t *desc67_get_bytes(const uint8_t *p_desc, uint8_t *pi_length)
{
    *pi_length = desc67_get_bytes_length(p_desc);
    return p_desc + 2;
}

static inline void desc67_set_bytes(uint8_t *p_desc, const uint8_t *p_bytes, uint8_t i_length)
{
    desc_set_length(p_desc, i_length);
    memcpy(p_desc + 2, p_bytes, i_length);
}

static inline bool desc67_validate(const uint8_t *p_desc)
{
    return true;
}

static inline void desc67_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t i_bytes_length, i;
    bool b_dsng_desc = desc_get_tag(p_desc) == 0x68;
    const uint8_t *p_bytes = desc67_get_bytes(p_desc, &i_bytes_length);
    char psz_bytes[2 * 255 + 1];
    char psz_bytes_txt[255 + 1];

    for (i = 0; i < i_bytes_length; i++) {
        sprintf(psz_bytes + 2 * i, "%02x", p_bytes[i]);
        if (p_bytes[i] >= 32 && p_bytes[i] <= 127 && p_bytes[i] != '"')
            psz_bytes_txt[i] = p_bytes[i];
        else
            psz_bytes_txt[i] = '.';
    }
    psz_bytes[2 * i] = '\0';
    psz_bytes_txt[i] = '\0';

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 !b_dsng_desc
                  ? "<TRANSPORT_STREAM_DESC bytes=\"%s\" bytes_txt=\"%s\"/>"
                  : "<DSNG_DESC bytes=\"%s\" bytes_txt=\"%s\"/>",
                 psz_bytes,
                 psz_bytes_txt
                );
        break;
    default:
        pf_print(opaque,
                 !b_dsng_desc
                  ? "    - desc 67 transport_stream bytes=\"%s\" bytes_txt=\"%s\""
                  : "    - desc 68 dsng bytes=\"%s\" bytes_txt=\"%s\"",
                 psz_bytes,
                 psz_bytes_txt
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
