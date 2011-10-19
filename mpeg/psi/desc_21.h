/*****************************************************************************
 * desc_21.h: ISO/IEC 13818-1 Descriptor 0x21 (Muxcode descriptor)
 *****************************************************************************
 * Copyright (C) 2021 Unix Solutions Ltd.
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
 *  - ISO/IEC 13818-1:2007(E) (MPEG-2 Systems)
 *  - ISO/IEC 14496-1:2001(E) (MPEG-4 Systems)
 */

#ifndef __BITSTREAM_MPEG_DESC_21_H__
#define __BITSTREAM_MPEG_DESC_21_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x21: Muxcode descriptor
 *****************************************************************************/
#define DESC21_HEADER_SIZE              DESC_HEADER_SIZE
#define DESC21_ENTRY_HEADER_SIZE        3
#define DESC21_SUBSTRUCT_HEADER_SIZE    1

static inline void desc21_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x21);
}

static inline uint8_t desc21n_get_length(const uint8_t *p_desc_n)
{
    return p_desc_n[0];
}

static inline void desc21n_set_length(uint8_t *p_desc_n, uint8_t i_length)
{
    p_desc_n[0] = i_length;
}

static inline uint8_t desc21n_get_muxcode(const uint8_t *p_desc_n)
{
    return (p_desc_n[1] & 0xf0) >> 4;
}

static inline void desc21n_set_muxcode(uint8_t *p_desc_n, uint8_t i_muxcode)
{
    p_desc_n[1] = (i_muxcode << 4) | (p_desc_n[1] & 0x0f);
}

static inline uint8_t desc21n_get_version(const uint8_t *p_desc_n)
{
    return p_desc_n[1] & 0x0f;
}

static inline void desc21n_set_version(uint8_t *p_desc_n, uint8_t i_version)
{
    p_desc_n[1] = (p_desc_n[1] & 0xf0) | (i_version & 0x0f);
}

static inline uint8_t desc21n_get_substruct_count(const uint8_t *p_desc_n)
{
    return p_desc_n[2];
}

static inline void desc21n_set_substruct_count(uint8_t *p_desc_n, uint8_t i_substruct_count)
{
    p_desc_n[2] = i_substruct_count;
}

static inline uint8_t *desc21_get_entry(const uint8_t *p_desc, uint8_t n)
{
    const uint8_t *p_desc_n = p_desc + DESC21_HEADER_SIZE;
    uint8_t i_desc_size = desc_get_length(p_desc);

    while (n) {
        if (p_desc_n + desc21n_get_length(p_desc_n) - p_desc > i_desc_size)
            return NULL;
        p_desc_n += 1 + desc21n_get_length(p_desc_n);
        n--;
    }
    if (p_desc_n - p_desc > i_desc_size)
        return NULL;
    return (uint8_t *)p_desc_n;
}

static inline uint8_t desc21k_get_slot_count(const uint8_t *p_desc_k)
{
    return p_desc_k[0] >> 3;
}

static inline void desc21k_set_slot_count(uint8_t *p_desc_k, uint8_t i_slot_count)
{
    p_desc_k[0] = (i_slot_count << 3) | (p_desc_k[0] & 0x07);
}

static inline uint8_t desc21k_get_repetition_count(const uint8_t *p_desc_k)
{
    return p_desc_k[0] & 0x07;
}

static inline void desc21k_set_repetition_count(uint8_t *p_desc_k, uint8_t i_repetition_count)
{
    p_desc_k[0] = (p_desc_k[0] & 0xf8) | (i_repetition_count & 0x07);
}

static inline uint8_t desc21k_get_flex_mux_channel(const uint8_t *p_desc_k, uint8_t i_index)
{
    return p_desc_k[DESC21_SUBSTRUCT_HEADER_SIZE + i_index * 2];
}

static inline void desc21k_set_flex_mux_channel(uint8_t *p_desc_k, uint8_t i_index, uint8_t i_flex_mux_channel)
{
    p_desc_k[DESC21_SUBSTRUCT_HEADER_SIZE + i_index * 2] = i_flex_mux_channel;
}

static inline uint8_t desc21k_get_number_of_bytes(const uint8_t *p_desc_k, uint8_t i_index)
{
    return p_desc_k[DESC21_SUBSTRUCT_HEADER_SIZE + i_index * 2 + 1];
}

static inline void desc21k_set_number_of_bytes(uint8_t *p_desc_k, uint8_t i_index, uint8_t i_number_of_bytes)
{
    p_desc_k[DESC21_SUBSTRUCT_HEADER_SIZE + i_index * 2 + 1] = i_number_of_bytes;
}

static inline uint8_t *desc21n_get_substruct(const uint8_t *p_desc_n, uint8_t k)
{
    const uint8_t *p_desc_k = p_desc_n + DESC21_ENTRY_HEADER_SIZE;
    uint8_t i_entry_size = desc21n_get_length(p_desc_n);

    if (desc21n_get_substruct_count(p_desc_n) == 0)
        return NULL;

    if (k > desc21n_get_substruct_count(p_desc_n) - 1)
        return NULL;

    while (k) {
        if (p_desc_k - p_desc_n > i_entry_size)
            return NULL;
        p_desc_k += DESC21_SUBSTRUCT_HEADER_SIZE + (2 * desc21k_get_slot_count(p_desc_k));
        k--;
    }
    if (p_desc_n - p_desc_k > i_entry_size)
        return NULL;
    return (uint8_t *)p_desc_k;
}

static inline bool desc21_validate(const uint8_t *p_desc)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;
    int i_desc_length = desc_get_length(p_desc);
    while ((p_desc_n = desc21_get_entry(p_desc, j++)) != NULL) {
        int i_calc_entry_length = DESC21_ENTRY_HEADER_SIZE - 1;
        uint8_t *p_desc_k;
        uint8_t k = 0;
        while ((p_desc_k = desc21n_get_substruct(p_desc_n, k++)) != NULL) {
            i_calc_entry_length += DESC21_SUBSTRUCT_HEADER_SIZE + (2 * desc21k_get_slot_count(p_desc_k));
        }
        if (i_calc_entry_length != desc21n_get_length(p_desc_n))
            return false;
        i_desc_length -= desc21n_get_length(p_desc_n);
    }
    return i_desc_length >= 0;
}

static inline void desc21_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    const uint8_t *p_desc_n;
    uint8_t n = 0;

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<MUXCODE_DESC>");
        break;
    default:
        pf_print(opaque, "    - desc 21 muxcode");
    }

    while ((p_desc_n = desc21_get_entry(p_desc, n++)) != NULL) {
        const uint8_t *p_desc_k;
        uint8_t k = 0;

        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque,
                     "<MUXCODE_ENTRY length=\"%u\" muxcode=\"%u\""
                     " version=\"%u\" substruct_count=\"%u\">",
                     desc21n_get_length(p_desc_n),
                     desc21n_get_muxcode(p_desc_n),
                     desc21n_get_version(p_desc_n),
                     desc21n_get_substruct_count(p_desc_n)
                    );
            break;
        default:
            pf_print(opaque,
                     "        - muxcode_entry length=%u muxcode=%u"
                     " version=%u substruct_count=%u",
                     desc21n_get_length(p_desc_n),
                     desc21n_get_muxcode(p_desc_n),
                     desc21n_get_version(p_desc_n),
                     desc21n_get_substruct_count(p_desc_n)
                    );
        }

        while ((p_desc_k = desc21n_get_substruct(p_desc_n, k++)) != NULL) {
            uint8_t i_slot_count = desc21k_get_slot_count(p_desc_k);
            uint8_t i_repetition_count = desc21k_get_repetition_count(p_desc_k);
            uint8_t r;

            switch (i_print_type) {
            case PRINT_XML:
                pf_print(opaque,
                         "<MUXCODE_SUBSTRUCT slot_count=\"%u\" repetition_count=\"%u\">",
                         i_slot_count, i_repetition_count);
                break;
            default:
                pf_print(opaque,
                         "            - muxcode_substruct slot_count=%u repetition_count=%u",
                         i_slot_count, i_repetition_count);
            }

            for (r = 0; r < i_slot_count; r++) {
                uint8_t i_flex_mux_channel = desc21k_get_flex_mux_channel(p_desc_k, r);
                uint8_t i_number_of_bytes  = desc21k_get_number_of_bytes(p_desc_k, r);
                switch (i_print_type) {
                case PRINT_XML:
                    pf_print(opaque,
                             "<MUXCODE_SUBSTRUCT_ENTRY slot=\"%u\" flex_mux_channel=\"%u\" number_of_bytes=\"%u\"/>",
                             r, i_flex_mux_channel, i_number_of_bytes);
                    break;
                default:
                    pf_print(opaque,
                             "                - muxcode_substruct_entry slot=%u flex_mux_channel=%u number_of_bytes=%u",
                             r, i_flex_mux_channel, i_number_of_bytes);
                }
            }

            if (i_print_type == PRINT_XML)
                pf_print(opaque, "</MUXCODE_SUBSTRUCT>");
        }

        if (i_print_type == PRINT_XML)
            pf_print(opaque, "</MUXCODE_ENTRY>");
    }

    if (i_print_type == PRINT_XML)
        pf_print(opaque, "</MUXCODE_DESC>");
}

#ifdef __cplusplus
}
#endif

#endif
