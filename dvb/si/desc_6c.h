/*****************************************************************************
 * desc_6c.h: ETSI EN 300 468 Descriptor 0x6c: Cell list descriptor
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

#ifndef __BITSTREAM_DVB_DESC_6C_H__
#define __BITSTREAM_DVB_DESC_6C_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x6c: Cell list descriptors
 *****************************************************************************/
#define DESC6C_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC6C_DATA_SIZE        10
#define DESC6C_SUBCELL_SIZE     8

static inline void desc6c_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x6c);
}

static inline uint16_t desc6cn_get_cell_id(const uint8_t *p_desc_n)
{
    return (p_desc_n[0] << 8) | p_desc_n[1];
}

static inline void desc6cn_set_cell_id(uint8_t *p_desc_n, uint16_t i_cell_id)
{
    p_desc_n[0] = (i_cell_id >> 8) & 0xff;
    p_desc_n[1] =  i_cell_id       & 0xff;
}

static inline uint16_t desc6cn_get_cell_latitude(const uint8_t *p_desc_n)
{
    return (p_desc_n[2] << 8) | p_desc_n[3];
}

static inline void desc6cn_set_cell_latitude(uint8_t *p_desc_n, uint16_t i_cell_latitude)
{
    p_desc_n[2] = (i_cell_latitude >> 8) & 0xff;
    p_desc_n[3] =  i_cell_latitude       & 0xff;
}

static inline uint16_t desc6cn_get_cell_longtitude(const uint8_t *p_desc_n)
{
    return (p_desc_n[4] << 8) | p_desc_n[5];
}

static inline void desc6cn_set_cell_longtitude(uint8_t *p_desc_n, uint16_t i_cell_longtitude)
{
    p_desc_n[4] = (i_cell_longtitude >> 8) & 0xff;
    p_desc_n[5] =  i_cell_longtitude       & 0xff;
}

static inline uint16_t desc6cn_get_cell_extend_of_latitude(const uint8_t *p_desc_n)
{
    return (p_desc_n[6] << 4) | ((p_desc_n[7] & 0xf0) >> 4);
}

static inline void desc6cn_set_cell_extend_of_latitude(uint8_t *p_desc_n, uint16_t i_cell_extend_of_latitude)
{
    p_desc_n[6] = (i_cell_extend_of_latitude >> 4) & 0xff;
    p_desc_n[7] = ((i_cell_extend_of_latitude & 0x0f) << 4) | (p_desc_n[7] & 0x0f);
}

static inline uint16_t desc6cn_get_cell_extend_of_longtitude(const uint8_t *p_desc_n)
{
    return ((p_desc_n[7] & 0x0f) << 8) | p_desc_n[8];
}

static inline void desc6cn_set_cell_extend_of_longtitude(uint8_t *p_desc_n, uint16_t i_cell_extend_of_longtitude)
{
    p_desc_n[7] = (p_desc_n[7] & 0xf0) | ((i_cell_extend_of_longtitude >> 8) & 0x0f);
    p_desc_n[8] = i_cell_extend_of_longtitude & 0xff;
}

static inline uint8_t desc6cn_get_subcell_info_loop_length(const uint8_t *p_desc_n)
{
    return p_desc_n[9];
}


static inline void desc6cn_set_subcell_info_loop_length(uint8_t *p_desc_n, uint8_t i_subcell_info_loop_length)
{
    p_desc_n[9] = i_subcell_info_loop_length;
}


/* Subcell loop */

static inline uint8_t desc6ck_get_cell_id_extension(const uint8_t *p_desc_k)
{
    return p_desc_k[0];
}

static inline void desc6ck_set_cell_id_extension(uint8_t *p_desc_k, uint8_t i_cell_id_extension)
{
    p_desc_k[0] = i_cell_id_extension;
}

static inline uint16_t desc6ck_get_subcell_latitude(const uint8_t *p_desc_k)
{
    return (p_desc_k[1] << 8) | p_desc_k[2];
}

static inline void desc6ck_set_subcell_latitude(uint8_t *p_desc_k, uint16_t i_cell_latitude)
{
    p_desc_k[1] = (i_cell_latitude >> 8) & 0xff;
    p_desc_k[2] =  i_cell_latitude       & 0xff;
}

static inline uint16_t desc6ck_get_subcell_longtitude(const uint8_t *p_desc_k)
{
    return (p_desc_k[3] << 8) | p_desc_k[4];
}

static inline void desc6ck_set_subcell_longtitude(uint8_t *p_desc_k, uint16_t i_cell_longtitude)
{
    p_desc_k[3] = (i_cell_longtitude >> 8) & 0xff;
    p_desc_k[4] =  i_cell_longtitude       & 0xff;
}

static inline uint16_t desc6ck_get_subcell_extend_of_latitude(const uint8_t *p_desc_k)
{
    return (p_desc_k[5] << 4) | ((p_desc_k[6] & 0xf0) >> 4);
}

static inline void desc6ck_set_subcell_extend_of_latitude(uint8_t *p_desc_k, uint16_t i_cell_extend_of_latitude)
{
    p_desc_k[5] = (i_cell_extend_of_latitude >> 4) & 0xff;
    p_desc_k[6] = ((i_cell_extend_of_latitude & 0x0f) << 4) | (p_desc_k[6] & 0x0f);
}

static inline uint16_t desc6ck_get_subcell_extend_of_longtitude(const uint8_t *p_desc_k)
{
    return ((p_desc_k[6] & 0x0f) << 8) | p_desc_k[7];
}

static inline void desc6ck_set_subcell_extend_of_longtitude(uint8_t *p_desc_k, uint16_t i_cell_extend_of_longtitude)
{
    p_desc_k[6] = (p_desc_k[6] & 0xf0) | ((i_cell_extend_of_longtitude >> 8) & 0x0f);
    p_desc_k[7] = i_cell_extend_of_longtitude & 0xff;
}

/* ----- */

static inline uint8_t *desc6c_get_cell(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC6C_HEADER_SIZE;
    uint8_t i_desc_size = desc_get_length(p_desc);

    while (n) {
        uint8_t i_desc6c_data_size = DESC6C_DATA_SIZE + desc6cn_get_subcell_info_loop_length(p_desc_n);
        if (p_desc_n + i_desc6c_data_size - p_desc > i_desc_size)
            return NULL;
        p_desc_n += i_desc6c_data_size;
        n--;
    }
    if (p_desc_n - p_desc > i_desc_size)
        return NULL;
    return p_desc_n;
}

static inline uint8_t *desc6cn_get_subcell(uint8_t *p_desc_n, uint8_t k)
{
    int ofs = k * DESC6C_SUBCELL_SIZE;
    if (ofs < desc6cn_get_subcell_info_loop_length(p_desc_n))
        return p_desc_n + DESC6C_DATA_SIZE + ofs;
    else
        return NULL;
}

static inline bool desc6c_validate(const uint8_t *p_desc)
{
    const uint8_t *p_desc_n = p_desc + DESC6C_HEADER_SIZE;
    int i_desc_size = desc_get_length(p_desc);
    while (i_desc_size > (DESC6C_HEADER_SIZE - DESC_HEADER_SIZE)) {
        uint8_t i_desc6c_data_size = DESC6C_DATA_SIZE + desc6cn_get_subcell_info_loop_length(p_desc_n);
        i_desc_size -= i_desc6c_data_size;
        p_desc_n    += i_desc6c_data_size;
    }
    return i_desc_size == (DESC6C_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline void desc6c_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t n = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc6c_get_cell(p_desc, n++)) != NULL) {
        uint8_t k = 0;
        uint8_t *p_desc_k;
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque,
                "<CELL_LIST_DESC cell_id=\"%u\" cell_latitude=\"%u\""
                " cell_longtitude=\"%u\" cell_extend_of_latitude=\"%u\""
                " cell_extend_of_longtitude=\"%u\">",
                desc6cn_get_cell_id(p_desc_n),
                desc6cn_get_cell_latitude(p_desc_n),
                desc6cn_get_cell_longtitude(p_desc_n),
                desc6cn_get_cell_extend_of_latitude(p_desc_n),
                desc6cn_get_cell_extend_of_longtitude(p_desc_n)
            );
            while ((p_desc_k = desc6cn_get_subcell(p_desc_n, k++)) != NULL) {
                pf_print(opaque,
                    "<CELL_LIST_SUBCELL_INFO cell_id_extension=\"%u\" subcell_latitude=\"%u\""
                    " subcell_longtitude=\"%u\" subcell_extend_of_latitude=\"%u\""
                    " subcell_extend_of_longtitude=\"%u\"/>",
                    desc6ck_get_cell_id_extension(p_desc_k),
                    desc6ck_get_subcell_latitude(p_desc_k),
                    desc6ck_get_subcell_longtitude(p_desc_k),
                    desc6ck_get_subcell_extend_of_latitude(p_desc_k),
                    desc6ck_get_subcell_extend_of_longtitude(p_desc_k)
                );
            }
            pf_print(opaque, "</CELL_LIST_DESC>");
            break;
        default:
            pf_print(opaque,
                "    - desc 6c cell_list cell_id=%u cell_latitude=%u"
                " cell_longtitude=%u cell_extend_of_latitude=%u"
                " cell_extend_of_longtitude=%u",
                desc6cn_get_cell_id(p_desc_n),
                desc6cn_get_cell_latitude(p_desc_n),
                desc6cn_get_cell_longtitude(p_desc_n),
                desc6cn_get_cell_extend_of_latitude(p_desc_n),
                desc6cn_get_cell_extend_of_longtitude(p_desc_n)
            );
            while ((p_desc_k = desc6cn_get_subcell(p_desc_n, k++)) != NULL) {
                pf_print(opaque,
                    "        - cell_list_subcell_info cell_id_extension=%u subcell_latitude=%u"
                    " subcell_longtitude=%u subcell_extend_of_latitude=%u"
                    " subcell_extend_of_longtitude=%u",
                    desc6ck_get_cell_id_extension(p_desc_k),
                    desc6ck_get_subcell_latitude(p_desc_k),
                    desc6ck_get_subcell_longtitude(p_desc_k),
                    desc6ck_get_subcell_extend_of_latitude(p_desc_k),
                    desc6ck_get_subcell_extend_of_longtitude(p_desc_k)
                );
            }
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
