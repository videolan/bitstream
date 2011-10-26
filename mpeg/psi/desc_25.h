/*****************************************************************************
 * desc_25.h: ISO/IEC 13818-1 Descriptor 0x25 (Metadata pointer descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_25_H__
#define __BITSTREAM_MPEG_DESC_25_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/mpeg/psi/desc_24.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x25 (Metadata pointer descriptor)
 *****************************************************************************/
#define DESC25_HEADER_SIZE      (DESC_HEADER_SIZE + 5)

static inline void desc25_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x25);
}

#define desc25_get_metadata_application_format desc24_get_metadata_application_format
#define desc25_set_metadata_application_format desc24_set_metadata_application_format
#define desc25_get_metadata_application_format_identifier desc24_get_metadata_application_format_identifier
#define desc25_set_metadata_application_format_identifier desc24_set_metadata_application_format_identifier

#define __ofs1 \
    (4 + (desc25_get_metadata_application_format(p_desc) == 0xffff ? 4 : 0))

static inline uint8_t desc25_get_metadata_format(const uint8_t *p_desc)
{
    return p_desc[__ofs1];
}

static inline void desc25_set_metadata_format(uint8_t *p_desc, uint8_t i_format)
{
    p_desc[__ofs1] = i_format;
}

static inline uint32_t desc25_get_metadata_format_identifier(const uint8_t *p_desc)
{
    if (desc25_get_metadata_format(p_desc) == 0xff) {
        uint8_t ofs = __ofs1;
        return (p_desc[ofs + 1] << 24) | (p_desc[ofs + 2] << 16) | (p_desc[ofs + 3] << 8) | p_desc[ofs + 4];
    } else {
        return 0;
    }
}

static inline void desc25_set_metadata_format_identifier(uint8_t *p_desc, uint32_t i_identifier)
{
    if (desc25_get_metadata_format(p_desc) == 0xff) {
        uint8_t ofs = __ofs1;
        p_desc[ofs + 1] = (i_identifier >> 24) & 0xff;
        p_desc[ofs + 2] = (i_identifier >> 16) & 0xff;
        p_desc[ofs + 3] = (i_identifier >>  8) & 0xff;
        p_desc[ofs + 4] =  i_identifier        & 0xff;
    }
}

#define __ofs2 \
    (__ofs1 + 1 + (desc25_get_metadata_format(p_desc) == 0xff ? 4 : 0))

static inline uint8_t desc25_get_metadata_service_id(const uint8_t *p_desc)
{
    return p_desc[__ofs2];
}

static inline void desc25_set_metadata_service_id(uint8_t *p_desc, uint8_t i_service_id)
{
    p_desc[__ofs2] = i_service_id;
}

static inline bool desc25_get_metadata_locator_record_flag(const uint8_t *p_desc)
{
    return (p_desc[__ofs2 + 1] & 0x80) == 0x80;
}

static inline void desc25_set_metadata_locator_record_flag(uint8_t *p_desc, bool b_flag)
{
    uint8_t ofs = __ofs2;
    p_desc[ofs + 1] = (b_flag ? (p_desc[ofs + 1] | 0x80) : (p_desc[ofs + 1] &~ 0x80)) | 0x1f;
}

static inline uint8_t desc25_get_mpeg_carriage_flags(const uint8_t *p_desc)
{
    return (p_desc[__ofs2 + 1] & 0x60) >> 5;
}

static inline void desc25_set_mpeg_carriage_flags(uint8_t *p_desc, uint8_t i_flags)
{
    uint8_t ofs = __ofs2;
    p_desc[ofs + 1] = ((p_desc[ofs + 1] & 0x80) | ((i_flags & 0x03) << 5)) | 0x1f;
}


static inline uint8_t desc25_get_metadata_locator_record_length(const uint8_t *p_desc)
{
    return desc25_get_metadata_locator_record_flag(p_desc) ? p_desc[__ofs2 + 2] : 0;
}

static inline const uint8_t *desc25_get_metadata_locator_record_data(const uint8_t *p_desc)
{
    return desc25_get_metadata_locator_record_flag(p_desc) ? p_desc + __ofs2 + 3 : NULL;
}

static inline const uint8_t *desc25_get_metadata_locator_record(const uint8_t *p_desc, uint8_t *i_length)
{
    *i_length = desc25_get_metadata_locator_record_length(p_desc);
    return desc25_get_metadata_locator_record_data(p_desc);
}

static inline void desc25_set_metadata_locator_record(uint8_t *p_desc, uint8_t i_length, uint8_t *p_data)
{
    if (desc25_get_metadata_locator_record_flag(p_desc)) {
        uint8_t ofs = __ofs2;
        p_desc[ofs + 2] = i_length;
        memcpy(p_desc + ofs + 3, p_data, i_length);
    }
}


#define __ofs3 \
    (__ofs2 + 1 + \
        (desc25_get_metadata_locator_record_flag(p_desc) \
            ? 1 + desc25_get_metadata_locator_record_length(p_desc) \
            : 0 \
        ) + 1 \
    )

static inline uint16_t desc25_get_program_number(const uint8_t *p_desc)
{
    uint8_t i_flags = desc25_get_mpeg_carriage_flags(p_desc);
    if (i_flags == 0 || i_flags == 1 || i_flags == 2) {
        uint8_t ofs = __ofs3;
        return (p_desc[ofs + 0] << 8) | p_desc[ofs + 1];
    } else {
        return 0;
    }
}

static inline void desc25_set_program_number(uint8_t *p_desc, uint16_t i_program)
{
    uint8_t i_flags = desc25_get_mpeg_carriage_flags(p_desc);
    if (i_flags == 0 || i_flags == 1 || i_flags == 2) {
        uint8_t ofs = __ofs3;
        p_desc[ofs + 0] = (i_program >> 8) & 0xff;
        p_desc[ofs + 1] =  i_program       & 0xff;
    }
}

static inline uint16_t desc25_get_ts_location(const uint8_t *p_desc)
{
    if (desc25_get_mpeg_carriage_flags(p_desc) == 1) {
        uint8_t ofs = __ofs3;
        return (p_desc[ofs + 2] << 8) | p_desc[ofs + 3];
    } else {
        return 0;
    }
}

static inline void desc25_set_ts_location(uint8_t *p_desc, uint16_t i_ts_location)
{
    if (desc25_get_mpeg_carriage_flags(p_desc) == 1) {
        uint8_t ofs = __ofs3;
        p_desc[ofs + 2] = (i_ts_location >> 8) & 0xff;
        p_desc[ofs + 3] =  i_ts_location       & 0xff;
    }
}

static inline uint16_t desc25_get_ts_id(const uint8_t *p_desc)
{
    if (desc25_get_mpeg_carriage_flags(p_desc) == 1) {
        uint8_t ofs = __ofs3;
        return (p_desc[ofs + 4] << 8) | p_desc[ofs + 5];
    } else {
        return 0;
    }
}

static inline void desc25_set_ts_id(uint8_t *p_desc, uint16_t i_ts_id)
{
    if (desc25_get_mpeg_carriage_flags(p_desc) == 1) {
        uint8_t ofs = __ofs3;
        p_desc[ofs + 4] = (i_ts_id >> 8) & 0xff;
        p_desc[ofs + 5] =  i_ts_id       & 0xff;
    }
}

#undef __ofs1
#undef __ofs2
#undef __ofs3

static inline int desc25_calc_length(const uint8_t *p_desc)
{
    int i_length = DESC25_HEADER_SIZE - DESC_HEADER_SIZE;
    uint8_t i_flags = desc25_get_mpeg_carriage_flags(p_desc);
    if (desc25_get_metadata_application_format(p_desc) == 0xffff)
        i_length += 4;
    if (desc25_get_metadata_format(p_desc) == 0xff)
        i_length += 4;
    if (desc25_get_metadata_locator_record_flag(p_desc))
        i_length += 1 + desc25_get_metadata_locator_record_length(p_desc);
    if (i_flags == 0 || i_flags == 1 || i_flags == 2)
        i_length += 2;
    if (i_flags == 1)
        i_length += 4;
    return i_length;
}

static inline bool desc25_validate(const uint8_t *p_desc)
{
    return desc25_calc_length(p_desc) <= desc_get_length(p_desc);
}

static inline void desc25_set_length(uint8_t *p_desc)
{
    desc_set_length(p_desc, desc25_calc_length(p_desc));
}

static inline void desc25_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t i, i_metadata_locator_record_length;
    const uint8_t *p_metadata_locator_record = desc25_get_metadata_locator_record(p_desc, &i_metadata_locator_record_length);
    char psz_metadata_locator_record[2 * 255 + 1];

    for (i = 0; i < i_metadata_locator_record_length; i++)
        sprintf(psz_metadata_locator_record + 2 * i, "%02x", p_metadata_locator_record[i]);
    psz_metadata_locator_record[2 * i] = '\0';

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 "<METADATA_POINTER_DESC metadata_application_format=\"0x%04x\""
                 " metadata_application_format_identifier=\"0x%08x\""
                 " metadata_format=\"0x%02x\""
                 " metadata_format_identifier=\"0x%08x\""
                 " metadata_service_id=\"0x%02x\""
                 " metadata_locator_record_flag=\"%u\""
                 " metadata_locator_record_length=\"%u\""
                 " metadata_locator_record=\"%s\""
                 " mpeg_carriage_flags=\"%u\""
                 " program_number=\"%u\" ts_location=\"%u\" ts_id=\"%u\"/>",
                 desc25_get_metadata_application_format(p_desc),
                 desc25_get_metadata_application_format_identifier(p_desc),
                 desc25_get_metadata_format(p_desc),
                 desc25_get_metadata_format_identifier(p_desc),
                 desc25_get_metadata_service_id(p_desc),
                 desc25_get_metadata_locator_record_flag(p_desc),
                 desc25_get_metadata_locator_record_length(p_desc),
                 psz_metadata_locator_record, // desc25_get_metadata_locator_record_data(p_desc)
                 desc25_get_mpeg_carriage_flags(p_desc),
                 desc25_get_program_number(p_desc),
                 desc25_get_ts_location(p_desc),
                 desc25_get_ts_id(p_desc)
                );
        break;
    default:
        pf_print(opaque,
                 "    - desc 25 metadata_pointer_desc metadata_application_format=0x%04x"
                 " metadata_application_format_identifier=0x%08x"
                 " metadata_format=0x%02x"
                 " metadata_format_identifier=0x%08x"
                 " metadata_service_id=0x%02x"
                 " metadata_locator_record_flag=%u"
                 " metadata_locator_record_length=%u"
                 " metadata_locator_record=\"%s\""
                 " mpeg_carriage_flags=%u"
                 " program_number=%u ts_location=%u ts_id=%u",
                 desc25_get_metadata_application_format(p_desc),
                 desc25_get_metadata_application_format_identifier(p_desc),
                 desc25_get_metadata_format(p_desc),
                 desc25_get_metadata_format_identifier(p_desc),
                 desc25_get_metadata_service_id(p_desc),
                 desc25_get_metadata_locator_record_flag(p_desc),
                 desc25_get_metadata_locator_record_length(p_desc),
                 psz_metadata_locator_record, // desc25_get_metadata_locator_record_data(p_desc)
                 desc25_get_mpeg_carriage_flags(p_desc),
                 desc25_get_program_number(p_desc),
                 desc25_get_ts_location(p_desc),
                 desc25_get_ts_id(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
