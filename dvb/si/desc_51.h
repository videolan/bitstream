/*****************************************************************************
 * desc_51.h: ETSI EN 300 518 Descriptor 0x51: Mosaic descriptor
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
 *  - ETSI EN 300 518 V1.11.1 (2010-04) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_DESC_51_H__
#define __BITSTREAM_DVB_DESC_51_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x51: Mosaic descriptor
 *****************************************************************************/
#define DESC51_HEADER_SIZE      (DESC_HEADER_SIZE + 1)
#define DESC51_DATA_SIZE        4

static inline void desc51_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x51);
    desc_set_length(p_desc, 1);
    p_desc[2] = 0x08;
}

static inline bool desc51_get_mosaic_entry_point(const uint8_t *p_desc)
{
    return (p_desc[2] & 0x80) == 0x80;
}

static inline void desc51_set_mosaic_entry_point(uint8_t *p_desc, bool b_entry_point)
{
    p_desc[2] = b_entry_point ? (p_desc[2] | 0x80) : (p_desc[2] &~ 0x80);
}

static inline uint8_t desc51_get_horizontal_cells(const uint8_t *p_desc)
{
    return (p_desc[2] & 0x70) >> 4;
}

static inline void desc51_set_horizontal_cells(uint8_t *p_desc, uint8_t i_hcells)
{
    p_desc[2] = (p_desc[2] & 0x8f) | ((i_hcells & 0x07) << 4);
}

static inline uint8_t desc51_get_vertical_cells(const uint8_t *p_desc)
{
    return (p_desc[2] & 0x07);
}

static inline void desc51_set_vertical_cells(uint8_t *p_desc, uint8_t i_vcells)
{
    p_desc[2] = (p_desc[2] & 0xf8) | (i_vcells & 0x07);
}


static inline uint8_t desc51n_get_logical_cell_id(const uint8_t *p_desc_n)
{
    return p_desc_n[0] >> 2;
}

static inline void desc51n_set_logical_cell_id(uint8_t *p_desc_n, uint8_t i_logical_cell_id)
{
    p_desc_n[0] = (i_logical_cell_id << 2) | 0x03;
}

static inline uint8_t desc51n_get_logical_cell_presentation_info(const uint8_t *p_desc_n)
{
    return p_desc_n[1] & 0x07;
}

static inline void desc51n_set_logical_cell_presentation_info(uint8_t *p_desc_n, uint8_t i_lc_presentation_info)
{
    p_desc_n[1] = (i_lc_presentation_info & 0x07) | 0xf8;
}

static inline char *desc51_get_logical_cell_presentation_info_txt(uint8_t i_lcp_info)
{
    return i_lcp_info == 0x00 ? "undefined" :
           i_lcp_info == 0x01 ? "video" :
           i_lcp_info == 0x02 ? "still picture" :
           i_lcp_info == 0x03 ? "graphics/text" : "reserved";
}

static inline uint8_t desc51n_get_elementary_cell_field_length(const uint8_t *p_desc_n)
{
    return p_desc_n[2];
}

static inline void desc51n_set_elementary_cell_field_length(uint8_t *p_desc_n, uint8_t i_length)
{
    p_desc_n[2] = i_length;
}

static inline uint8_t desc51n_get_elementary_cell_id(const uint8_t *p_desc_n, uint8_t idx)
{
    return p_desc_n[3 + idx] & 0x3f;
}

static inline void desc51n_set_elementary_cell_id(uint8_t *p_desc_n, uint8_t idx, uint8_t i_elementary_cell_id)
{
    p_desc_n[3 + idx] = 0xc0 | (i_elementary_cell_id & 0x3f);
}

static inline uint8_t desc51n_get_cell_linkage_info(const uint8_t *p_desc_n)
{
    return p_desc_n[3 + desc51n_get_elementary_cell_field_length(p_desc_n)];
}

static inline void desc51n_set_cell_linkage_info(uint8_t *p_desc_n, uint8_t i_cell_linkage_info)
{
    p_desc_n[3 + desc51n_get_elementary_cell_field_length(p_desc_n)] = i_cell_linkage_info;
}

static inline char *desc51_get_cell_linkage_info_txt(uint8_t i_icl_info)
{
    return i_icl_info == 0x00 ? "undefined" :
           i_icl_info == 0x01 ? "bouquet related" :
           i_icl_info == 0x02 ? "service related" :
           i_icl_info == 0x03 ? "other mosaic related" :
           i_icl_info == 0x04 ? "event related" : "reserved";
}

#define init_link_n(__const) \
    __const uint8_t *link_n; \
    uint8_t i_cell_linkage_info = desc51n_get_cell_linkage_info(p_desc_n); \
    link_n = p_desc_n + 4 + desc51n_get_elementary_cell_field_length(p_desc_n)

static inline uint16_t desc51n_get_bouquet_id(const uint8_t *p_desc_n)
{
    init_link_n(const);
    if (i_cell_linkage_info != 1) return 0;
    return (link_n[0] << 8) | link_n[1];
}

static inline void desc51n_set_bouquet_id(uint8_t *p_desc_n, uint16_t i_bouquet_id)
{
    init_link_n();
    if (i_cell_linkage_info != 1) return;
    link_n[0] = (i_bouquet_id >> 8) & 0xff;
    link_n[1] =  i_bouquet_id       & 0xff;
}


static inline uint16_t desc51n_get_onid(const uint8_t *p_desc_n)
{
    init_link_n(const);
    if (i_cell_linkage_info < 2 || i_cell_linkage_info > 4) return 0;
    return (link_n[0] << 8) | link_n[1];
}

static inline void desc51n_set_onid(uint8_t *p_desc_n, uint16_t i_onid)
{
    init_link_n();
    if (i_cell_linkage_info < 2 || i_cell_linkage_info > 4) return;
    link_n[0] = (i_onid >> 8) & 0xff;
    link_n[1] =  i_onid       & 0xff;
}


static inline uint16_t desc51n_get_tsid(const uint8_t *p_desc_n)
{
    init_link_n(const);
    if (i_cell_linkage_info < 2 || i_cell_linkage_info > 4) return 0;
    return (link_n[2] << 8) | link_n[3];
}

static inline void desc51n_set_tsid(uint8_t *p_desc_n, uint16_t i_tsid)
{
    init_link_n();
    if (i_cell_linkage_info < 2 || i_cell_linkage_info > 4) return;
    link_n[2] = (i_tsid >> 8) & 0xff;
    link_n[3] =  i_tsid       & 0xff;
}


static inline uint16_t desc51n_get_sid(const uint8_t *p_desc_n)
{
    init_link_n(const);
    if (i_cell_linkage_info < 2 || i_cell_linkage_info > 4) return 0;
    return (link_n[4] << 8) | link_n[5];
}

static inline void desc51n_set_sid(uint8_t *p_desc_n, uint16_t i_sid)
{
    init_link_n();
    if (i_cell_linkage_info < 2 || i_cell_linkage_info > 4) return;
    link_n[4] = (i_sid >> 8) & 0xff;
    link_n[5] =  i_sid       & 0xff;
}


static inline uint16_t desc51n_get_event_id(const uint8_t *p_desc_n)
{
    init_link_n(const);
    if (i_cell_linkage_info != 4) return 0;
    return (link_n[6] << 8) | link_n[7];
}

static inline void desc51n_set_event_id(uint8_t *p_desc_n, uint16_t i_event_id)
{
    init_link_n();
    if (i_cell_linkage_info != 4) return;
    link_n[6] = (i_event_id >> 8) & 0xff;
    link_n[7] =  i_event_id       & 0xff;
}

#undef init_link_n


static inline uint8_t *desc51_get_logical_cell(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC51_HEADER_SIZE;
    uint8_t i_desc_size = desc_get_length(p_desc);

    while (n) {
        uint8_t i_desc51_data_size = DESC51_DATA_SIZE + desc51n_get_elementary_cell_field_length(p_desc_n);
        switch (desc51n_get_cell_linkage_info(p_desc_n)) {
            case 1: i_desc51_data_size += 2; break;
            case 2: i_desc51_data_size += 6; break;
            case 3: i_desc51_data_size += 6; break;
            case 4: i_desc51_data_size += 8; break;
        }
        if (p_desc_n + i_desc51_data_size - p_desc > i_desc_size)
            return NULL;
        p_desc_n += i_desc51_data_size;
        n--;
    }
    if (p_desc_n - p_desc > i_desc_size)
        return NULL;
    return p_desc_n;
}

static inline bool desc51_validate(const uint8_t *p_desc)
{
    const uint8_t *p_desc_n = p_desc + DESC51_HEADER_SIZE;
    int i_desc_size = desc_get_length(p_desc);
    while (i_desc_size > (DESC51_HEADER_SIZE - DESC_HEADER_SIZE)) {
        uint8_t i_desc51_data_size = DESC51_DATA_SIZE + desc51n_get_elementary_cell_field_length(p_desc_n);
        switch (desc51n_get_cell_linkage_info(p_desc_n)) {
            case 1: i_desc51_data_size += 2; break;
            case 2: i_desc51_data_size += 6; break;
            case 3: i_desc51_data_size += 6; break;
            case 4: i_desc51_data_size += 8; break;
        }
        i_desc_size -= i_desc51_data_size;
        p_desc_n    += i_desc51_data_size;
    }
    return i_desc_size == (DESC51_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline void desc51_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    if (i_print_type == PRINT_XML)
        pf_print(opaque,
                 "<MOSAIC_DESC entry_point=\"%u\" horizontal_elementary_cells=\"%u\" vertical_elementary_cells=\"%u\">",
                 desc51_get_mosaic_entry_point(p_desc),
                 desc51_get_horizontal_cells(p_desc),
                 desc51_get_vertical_cells(p_desc)
                );
    else
        pf_print(opaque,
                 "    - desc 51 mosaic entry_point=%u horizontal_elementary_cells=%u vertical_elementary_cells=%u",
                 desc51_get_mosaic_entry_point(p_desc),
                 desc51_get_horizontal_cells(p_desc),
                 desc51_get_vertical_cells(p_desc)
                );

    while ((p_desc_n = desc51_get_logical_cell(p_desc, j++)) != NULL) {
        uint8_t k;
        uint8_t i_present_info = desc51n_get_logical_cell_presentation_info(p_desc_n);
        uint8_t i_el_cell_len  = desc51n_get_elementary_cell_field_length(p_desc_n);
        uint8_t i_cell_linkage = desc51n_get_cell_linkage_info(p_desc_n);
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque,
                "<MOSAIC_LOGICAL_CELL cell_id=\"%u\" presentation_info=\"%u\""
                " presentation_info_txt=\"%s\" cell_linkage=\"%u\""
                " cell_linkage_txt=\"%s\" bouquet_id=\"%u\" onid=\"%u\""
                " tsid=\"%u\" sid=\"%u\" event_id=\"%u\">",
                desc51n_get_logical_cell_id(p_desc_n),
                i_present_info,
                desc51_get_logical_cell_presentation_info_txt(i_present_info),
                i_cell_linkage,
                desc51_get_cell_linkage_info_txt(i_cell_linkage),
                desc51n_get_bouquet_id(p_desc_n),
                desc51n_get_onid(p_desc_n),
                desc51n_get_tsid(p_desc_n),
                desc51n_get_sid(p_desc_n),
                desc51n_get_event_id(p_desc_n)
            );
            for (k = 0; k < i_el_cell_len; k++) {
                pf_print(opaque,
                    "<MOSAIC_ELEMENTARY_CELL cell_id=\"%u\"/>",
                    desc51n_get_elementary_cell_id(p_desc_n, k));
            }
            pf_print(opaque, "</MOSAIC_LOGICAL_CELL>");
            break;
        default:
            pf_print(opaque,
                "        - logical_cell cell_id=%u presentation_info=%u"
                " presentation_info_txt=\"%s\" cell_linkage=%u"
                " cell_linkage_txt=\"%s\" bouquet_id=%u onid=%u"
                " tsid=%u sid=%u event_id=%u",
                desc51n_get_logical_cell_id(p_desc_n),
                i_present_info,
                desc51_get_logical_cell_presentation_info_txt(i_present_info),
                i_cell_linkage,
                desc51_get_cell_linkage_info_txt(i_cell_linkage),
                desc51n_get_bouquet_id(p_desc_n),
                desc51n_get_onid(p_desc_n),
                desc51n_get_tsid(p_desc_n),
                desc51n_get_sid(p_desc_n),
                desc51n_get_event_id(p_desc_n)
            );
            for (k = 0; k < i_el_cell_len; k++) {
                pf_print(opaque,
                    "            - elementary_cell cell_id=%u",
                    desc51n_get_elementary_cell_id(p_desc_n, k));
            }
        }
    }

    if (i_print_type == PRINT_XML)
        pf_print(opaque, "</MOSAIC_DESC>");
}

#ifdef __cplusplus
}
#endif

#endif
