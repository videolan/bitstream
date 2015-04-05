/*****************************************************************************
 * desc_24.h: ISO/IEC 13818-1 Descriptor 0x24 (Content labeling descriptor)
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
 *  - ISO/IEC 13818-1:2007(E) (MPEG-2 Systems)
 */

#ifndef __BITSTREAM_MPEG_DESC_24_H__
#define __BITSTREAM_MPEG_DESC_24_H__

#include <inttypes.h> /* PRIu64 */

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x24 (Content labeling descriptor)
 *****************************************************************************/
#define DESC24_HEADER_SIZE      (DESC_HEADER_SIZE + 3)

static inline void desc24_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x24);
}

static inline uint16_t desc24_get_metadata_application_format(const uint8_t *p_desc)
{
    return (p_desc[2] << 8) | p_desc[3];
}

static inline void desc24_set_metadata_application_format(uint8_t *p_desc, uint16_t i_format)
{
    p_desc[2] = (i_format >> 8) & 0xff;
    p_desc[3] =  i_format       & 0xff;
}

static inline uint32_t desc24_get_metadata_application_format_identifier(const uint8_t *p_desc)
{
    if (desc24_get_metadata_application_format(p_desc) == 0xffff)
        return (p_desc[4] << 24) | (p_desc[5] << 16) | (p_desc[6] << 8) | p_desc[7];
    else
        return 0;
}

static inline void desc24_set_metadata_application_format_identifier(uint8_t *p_desc, uint32_t i_identifier)
{
    if (desc24_get_metadata_application_format(p_desc) != 0xffff)
        return;
    p_desc[4] = (i_identifier >> 24) & 0xff;
    p_desc[5] = (i_identifier >> 16) & 0xff;
    p_desc[6] = (i_identifier >>  8) & 0xff;
    p_desc[7] =  i_identifier        & 0xff;
}

#define __ofs1 \
    (4 + (desc24_get_metadata_application_format(p_desc) == 0xffff ? 4 : 0))

static inline bool desc24_get_content_reference_id_record_flag(const uint8_t *p_desc)
{
    return (p_desc[__ofs1] & 0x80) == 0x80;
}

static inline void desc24_set_content_reference_id_record_flag(uint8_t *p_desc, bool b_flag)
{
    uint8_t ofs = __ofs1;
    p_desc[ofs] = (b_flag ? (p_desc[ofs] | 0x80) : (p_desc[ofs] &~ 0x80)) | 0x07;
}

static inline uint8_t desc24_get_content_time_base_indicator(const uint8_t *p_desc)
{
    return (p_desc[__ofs1] >> 3) & 0x0f;
}

static inline void desc24_set_content_time_base_indicator(uint8_t *p_desc, uint8_t i_indicator)
{
    uint8_t ofs = __ofs1;
    p_desc[ofs] = (p_desc[ofs] & 0x80) | ((i_indicator & 0x0f) << 3) | 0x07;
}

static inline uint8_t desc24_get_content_reference_id_length(const uint8_t *p_desc)
{
    return desc24_get_content_reference_id_record_flag(p_desc) ? p_desc[__ofs1 + 1] : 0;
}

static inline const uint8_t *desc24_get_content_reference_id_data(const uint8_t *p_desc)
{
    return desc24_get_content_reference_id_record_flag(p_desc) ? p_desc + __ofs1 + 2 : NULL;
}

static inline const uint8_t *desc24_get_content_reference_id(const uint8_t *p_desc, uint8_t *i_length)
{
    *i_length = desc24_get_content_reference_id_length(p_desc);
    return desc24_get_content_reference_id_data(p_desc);
}

static inline void desc24_set_content_reference_id_record(uint8_t *p_desc, uint8_t i_length, uint8_t *p_data)
{
    if (desc24_get_content_reference_id_record_flag(p_desc)) {
        uint8_t ofs = __ofs1;
        p_desc[ofs + 1] = i_length;
        memcpy(p_desc + ofs + 2, p_data, i_length);
    }
}


#define __ofs2 \
    (__ofs1 + 1 + \
        (desc24_get_content_reference_id_record_flag(p_desc) \
            ? 1 + desc24_get_content_reference_id_length(p_desc) \
            : 0 \
        ) \
    )

static inline uint64_t desc24_get_content_time_base_value(const uint8_t *p_desc)
{
    uint8_t i_indicator = desc24_get_content_time_base_indicator(p_desc);
    if (i_indicator == 1 || i_indicator == 2) {
        uint8_t ofs = __ofs2;
        return (uint64_t)(
                (((uint64_t)p_desc[ofs] & 0x01ull) << 32) |
                 ((uint64_t)p_desc[ofs + 1] << 24) |
                 ((uint64_t)p_desc[ofs + 2] << 16) |
                 ((uint64_t)p_desc[ofs + 3] <<  8) |
                  (uint64_t)p_desc[ofs + 4]
               );
    } else {
        return 0;
    }
}

static inline void desc24_set_content_time_base_value(uint8_t *p_desc, uint64_t i_value)
{
    uint8_t i_indicator = desc24_get_content_time_base_indicator(p_desc);
    if (i_indicator == 1 || i_indicator == 2) {
        uint8_t ofs = __ofs2;
        p_desc[ofs + 0] = 0xfe | ((i_value >> 32) & 0x01);
        p_desc[ofs + 1] = (i_value >> 24) & 0xff;
        p_desc[ofs + 2] = (i_value >> 16) & 0xff;
        p_desc[ofs + 3] = (i_value >>  8) & 0xff;
        p_desc[ofs + 4] =  i_value        & 0xff;
    }
}

static inline uint64_t desc24_get_metadata_time_base_value(const uint8_t *p_desc)
{
    uint8_t i_indicator = desc24_get_content_time_base_indicator(p_desc);
    if (i_indicator == 1 || i_indicator == 2) {
        uint8_t ofs = __ofs2 + 5;
        return (uint64_t)(
                (((uint64_t)p_desc[ofs] & 0x01ull) << 32) |
                 ((uint64_t)p_desc[ofs + 1] << 24) |
                 ((uint64_t)p_desc[ofs + 2] << 16) |
                 ((uint64_t)p_desc[ofs + 3] <<  8) |
                  (uint64_t)p_desc[ofs + 4]
               );
    } else {
        return 0;
    }
}

static inline void desc24_set_metadata_time_base_value(uint8_t *p_desc, uint64_t i_value)
{
    uint8_t i_indicator = desc24_get_content_time_base_indicator(p_desc);
    if (i_indicator == 1 || i_indicator == 2) {
        uint8_t ofs = __ofs2 + 5;
        p_desc[ofs + 0] = 0xfe | ((i_value >> 32) & 0x01);
        p_desc[ofs + 1] = (i_value >> 24) & 0xff;
        p_desc[ofs + 2] = (i_value >> 16) & 0xff;
        p_desc[ofs + 3] = (i_value >>  8) & 0xff;
        p_desc[ofs + 4] =  i_value        & 0xff;
    }
}


static inline uint8_t desc24_get_content_id(const uint8_t *p_desc)
{
    uint8_t i_indicator = desc24_get_content_time_base_indicator(p_desc);
    if (i_indicator == 2) {
        uint8_t ofs = __ofs2 + 10;
        return p_desc[ofs] & 0x7f;
    } else {
        return 0;
    }
}

static inline void desc24_set_content_id(uint8_t *p_desc, uint8_t i_content_id)
{
    uint8_t i_indicator = desc24_get_content_time_base_indicator(p_desc);
    if (i_indicator == 2) {
        uint8_t ofs = __ofs2 + 10;
        p_desc[ofs + 0] = 0x80 | i_content_id;
    }
}


static inline uint8_t desc24_get_time_base_association_data_length(const uint8_t *p_desc)
{
    uint8_t i_indicator = desc24_get_content_time_base_indicator(p_desc);
    return (i_indicator >= 3 && i_indicator <= 7) ? p_desc[__ofs2] : 0;
}

static inline const uint8_t *desc24_get_time_base_association_data(const uint8_t *p_desc)
{
    uint8_t i_indicator = desc24_get_content_time_base_indicator(p_desc);
    return (i_indicator >= 3 && i_indicator <= 7) ? p_desc + 1 + __ofs2 : NULL;
}

static inline const uint8_t *desc24_get_time_base_association(const uint8_t *p_desc, uint8_t *i_length)
{
    *i_length = desc24_get_time_base_association_data_length(p_desc);
    return desc24_get_time_base_association_data(p_desc);
}

static inline void desc24_set_time_base_association(uint8_t *p_desc, uint8_t i_length, uint8_t *p_data)
{
    uint8_t i_indicator = desc24_get_content_time_base_indicator(p_desc);
    if (i_indicator >= 3 && i_indicator <= 7) {
        uint8_t ofs = __ofs2;
        p_desc[ofs] = i_length;
        memcpy(p_desc + ofs + 1, p_data, i_length);
    }
}

#undef __ofs1
#undef __ofs2

static inline int desc24_calc_length(const uint8_t *p_desc)
{
    int i_length = DESC24_HEADER_SIZE - DESC_HEADER_SIZE;
    uint8_t i_indicator = desc24_get_content_time_base_indicator(p_desc);
    if (desc24_get_metadata_application_format(p_desc) == 0xffff)
        i_length += 4;
    if (desc24_get_content_reference_id_record_flag(p_desc))
        i_length += 1 + desc24_get_content_reference_id_length(p_desc);
    if (i_indicator == 1 || i_indicator == 2)
        i_length += 10;
    if (i_indicator == 2)
        i_length += 1;
    if (i_indicator >= 3 && i_indicator <= 7)
        i_length += 1 + desc24_get_time_base_association_data_length(p_desc);
    return i_length;
}

static inline bool desc24_validate(const uint8_t *p_desc)
{
    return desc24_calc_length(p_desc) <= desc_get_length(p_desc);
}

static inline void desc24_set_length(uint8_t *p_desc)
{
    desc_set_length(p_desc, desc24_calc_length(p_desc));
}

static inline void desc24_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t i;
    uint8_t i_content_reference_id_length;
    uint8_t i_time_base_association_data_length;
    const uint8_t *p_content_reference_id = desc24_get_content_reference_id(p_desc, &i_content_reference_id_length);
    const uint8_t *p_time_base_association_data = desc24_get_time_base_association(p_desc, &i_time_base_association_data_length);
    char psz_content[2 * 255 + 1];
    char psz_time_base[2 * 255 + 1];

    for (i = 0; i < i_content_reference_id_length; i++)
        sprintf(psz_content + 2 * i, "%02x", p_content_reference_id[i]);
    psz_content[2 * i] = '\0';

    for (i = 0; i < i_time_base_association_data_length; i++)
        sprintf(psz_time_base + 2 * i, "%02x", p_time_base_association_data[i]);
    psz_time_base[2 * i] = '\0';


    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 "<CONTENT_LABELING_DESC metadata_application_format=\"0x%04x\""
                 " metadata_application_format_identifier=\"0x%08x\""
                 " content_reference_id_record_flag=\"%u\""
                 " content_reference_id_record_length=\"%u\""
                 " content_reference_id_record=\"%s\""
                 " content_time_base_indicator=\"%u\""
                 " content_time_base_value=\"%" PRIu64 "\""
                 " metadata_time_base_value=\"%" PRIu64 "\""
                 " content_id=\"%u\""
                 " time_base_association_data_length=\"%u\""
                 " time_base_association_data=\"%s\"/>",
                 desc24_get_metadata_application_format(p_desc),
                 desc24_get_metadata_application_format_identifier(p_desc),
                 desc24_get_content_reference_id_record_flag(p_desc),
                 desc24_get_content_reference_id_length(p_desc),
                 psz_content, // desc24_get_content_reference_id_data(p_desc)
                 desc24_get_content_time_base_indicator(p_desc),
                 desc24_get_content_time_base_value(p_desc),
                 desc24_get_metadata_time_base_value(p_desc),
                 desc24_get_content_id(p_desc),
                 desc24_get_time_base_association_data_length(p_desc),
                 psz_time_base // desc24_get_time_base_association_data(p_desc)
                );
        break;
    default:
        pf_print(opaque,
                 "    - desc 24 content_labeling metadata_application_format=0x%04x"
                 " metadata_application_format_identifier=0x%08x"
                 " content_reference_id_record_flag=%u"
                 " content_reference_id_record_length=%u"
                 " content_reference_id_record=\"%s\""
                 " content_time_base_indicator=%u"
                 " content_time_base_value=\"%" PRIu64 "\""
                 " metadata_time_base_value=\"%" PRIu64 "\""
                 " content_id=%u"
                 " time_base_association_data_length=%u"
                 " time_base_association_data=\"%s\"",
                 desc24_get_metadata_application_format(p_desc),
                 desc24_get_metadata_application_format_identifier(p_desc),
                 desc24_get_content_reference_id_record_flag(p_desc),
                 desc24_get_content_reference_id_length(p_desc),
                 psz_content, // desc24_get_content_reference_id_data(p_desc)
                 desc24_get_content_time_base_indicator(p_desc),
                 desc24_get_content_time_base_value(p_desc),
                 desc24_get_metadata_time_base_value(p_desc),
                 desc24_get_content_id(p_desc),
                 desc24_get_time_base_association_data_length(p_desc),
                 psz_time_base // desc24_get_time_base_association_data(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
