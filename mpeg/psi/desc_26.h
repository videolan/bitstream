/*****************************************************************************
 * desc_26.h: ISO/IEC 13818-1 Descriptor 0x26 (Metadata descriptor)
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

#ifndef __BITSTREAM_MPEG_DESC_26_H__
#define __BITSTREAM_MPEG_DESC_26_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/mpeg/psi/desc_25.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x26 (Metadata descriptor)
 *****************************************************************************/
#define DESC26_HEADER_SIZE      (DESC_HEADER_SIZE + 5)

static inline void desc26_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x26);
}

#define desc26_get_metadata_application_format desc25_get_metadata_application_format
#define desc26_set_metadata_application_format desc25_set_metadata_application_format
#define desc26_get_metadata_application_format_identifier desc25_get_metadata_application_format_identifier
#define desc26_set_metadata_application_format_identifier desc25_set_metadata_application_format_identifier

#define __ofs1 \
    (4 + (desc26_get_metadata_application_format(p_desc) == 0xffff ? 4 : 0))

#define desc26_get_metadata_format desc25_get_metadata_format
#define desc26_set_metadata_format desc25_set_metadata_format
#define desc26_get_metadata_format_identifier desc25_get_metadata_format_identifier
#define desc26_set_metadata_format_identifier desc25_set_metadata_format_identifier

#define __ofs2 \
    (__ofs1 + 1 + (desc26_get_metadata_format(p_desc) == 0xff ? 4 : 0))

#define desc26_get_metadata_service_id desc25_get_metadata_service_id
#define desc26_set_metadata_service_id desc25_set_metadata_service_id

static inline uint8_t desc26_get_decoder_config_flags(const uint8_t *p_desc)
{
    return (p_desc[__ofs2 + 1] & 0xe0) >> 5;
}

static inline void desc26_set_decoder_config_flags(uint8_t *p_desc, uint8_t i_flags)
{
    uint8_t ofs = __ofs2;
    p_desc[ofs + 1] = ((i_flags & 0x07) << 5) | (p_desc[ofs + 1] & 0x1f) | 0x0f;
}

static inline bool desc26_get_dsm_cc_flag(const uint8_t *p_desc)
{
    return (p_desc[__ofs2 + 1] & 0x10) == 0x10;
}

static inline void desc26_set_dsm_cc_flag(uint8_t *p_desc, bool b_flag)
{
    uint8_t ofs = __ofs2;
    p_desc[ofs + 1] = (b_flag ? (p_desc[ofs + 1] | 0x10) : (p_desc[ofs + 1] &~ 0x10)) | 0x0f;
}



static inline uint8_t desc26_get_service_identification_record_length(const uint8_t *p_desc)
{
    return desc26_get_dsm_cc_flag(p_desc) ? p_desc[__ofs2 + 2] : 0;
}

static inline const uint8_t *desc26_get_service_identification_record_data(const uint8_t *p_desc)
{
    return desc26_get_dsm_cc_flag(p_desc) ? p_desc + __ofs2 + 3 : NULL;
}

static inline const uint8_t *desc26_get_service_identification_record(const uint8_t *p_desc, uint8_t *i_length)
{
    *i_length = desc26_get_service_identification_record_length(p_desc);
    return desc26_get_service_identification_record_data(p_desc);
}

static inline void desc26_set_service_identification_record(uint8_t *p_desc, uint8_t i_length, uint8_t *p_data)
{
    if (desc26_get_dsm_cc_flag(p_desc)) {
        uint8_t ofs = __ofs2;
        p_desc[ofs + 2] = i_length;
        memcpy(p_desc + ofs + 3, p_data, i_length);
    }
}


#define __ofs3 \
    (__ofs2 + 1 + \
        (desc26_get_dsm_cc_flag(p_desc) \
            ? 1 + desc26_get_service_identification_record_length(p_desc) \
            : 0 \
        ) + 1 \
    )

static inline uint8_t desc26_get_decoder_config_length(const uint8_t *p_desc)
{
    return (desc26_get_decoder_config_flags(p_desc) == 1) ? p_desc[__ofs3] : 0;
}

static inline const uint8_t *desc26_get_decoder_config_data(const uint8_t *p_desc)
{
    return (desc26_get_decoder_config_flags(p_desc) == 1) ? p_desc + __ofs3 + 1 : NULL;
}

static inline const uint8_t *desc26_get_decoder_config(const uint8_t *p_desc, uint8_t *i_length)
{
    *i_length = desc26_get_decoder_config_length(p_desc);
    return desc26_get_decoder_config_data(p_desc);
}

static inline void desc26_set_decoder_config(uint8_t *p_desc, uint8_t i_length, uint8_t *p_data)
{
    if (desc26_get_decoder_config_flags(p_desc) == 1) {
        uint8_t ofs = __ofs3;
        p_desc[ofs] = i_length;
        memcpy(p_desc + ofs + 1, p_data, i_length);
    }
}


static inline uint8_t desc26_get_dec_config_identification_record_length(const uint8_t *p_desc)
{
    return (desc26_get_decoder_config_flags(p_desc) == 3) ? p_desc[__ofs3] : 0;
}

static inline const uint8_t *desc26_get_dec_config_identification_record_data(const uint8_t *p_desc)
{
    return (desc26_get_decoder_config_flags(p_desc) == 3) ? p_desc + __ofs3 + 1 : NULL;
}

static inline const uint8_t *desc26_get_dec_config_identification_record(const uint8_t *p_desc, uint8_t *i_length)
{
    *i_length = desc26_get_dec_config_identification_record_length(p_desc);
    return desc26_get_dec_config_identification_record_data(p_desc);
}

static inline void desc26_set_dec_config_identification_record(uint8_t *p_desc, uint8_t i_length, uint8_t *p_data)
{
    if (desc26_get_decoder_config_flags(p_desc) == 3) {
        uint8_t ofs = __ofs3;
        p_desc[ofs] = i_length;
        memcpy(p_desc + ofs + 1, p_data, i_length);
    }
}


static inline uint8_t desc26_get_decoder_config_metadata_service_id(const uint8_t *p_desc)
{
    if (desc26_get_decoder_config_flags(p_desc) == 4)
        return p_desc[__ofs3];
    else
        return 0;
}

static inline void desc26_set_decoder_config_metadata_service_id(uint8_t *p_desc, uint8_t i_service)
{
    if (desc26_get_decoder_config_flags(p_desc) == 4)
        p_desc[__ofs3] = i_service;
}

#undef __ofs1
#undef __ofs2
#undef __ofs3

static inline int desc26_calc_length(const uint8_t *p_desc)
{
    int i_length = DESC26_HEADER_SIZE - DESC_HEADER_SIZE;
    uint8_t i_flags = desc26_get_decoder_config_flags(p_desc);

    if (desc26_get_metadata_application_format(p_desc) == 0xffff)
        i_length += 4;
    if (desc26_get_metadata_format(p_desc) == 0xff)
        i_length += 4;
    if (desc26_get_dsm_cc_flag(p_desc))
        i_length += 1 + desc26_get_service_identification_record_length(p_desc);
    if (i_flags == 1)
        i_length += 1 + desc26_get_decoder_config_length(p_desc);
    if (i_flags == 3)
        i_length += 1 + desc26_get_dec_config_identification_record_length(p_desc);
    if (i_flags == 4)
        i_length += 1;
    return i_length;
}

static inline bool desc26_validate(const uint8_t *p_desc)
{
    return desc26_calc_length(p_desc) <= desc_get_length(p_desc);
}

static inline void desc26_set_length(uint8_t *p_desc)
{
    desc_set_length(p_desc, desc26_calc_length(p_desc));
}

static inline void desc26_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t i, i_service_identification_record_length;
    const uint8_t *p_service_identification_record = desc26_get_service_identification_record(p_desc, &i_service_identification_record_length);
    char psz_service_identification_record[2 * 256 + 1];
    const char *extra_str = "decoder_config";
    const uint8_t *p_extra = NULL;
    uint8_t i_extra_length = 0;
    char psz_extra[2 * 256 + 1];

    for (i = 0; i < i_service_identification_record_length; i++)
        sprintf(psz_service_identification_record + 2 * i, "%02x", p_service_identification_record[i]);
    psz_service_identification_record[2 * i] = '\0';

    if (desc26_get_decoder_config_flags(p_desc) == 1) {
        p_extra = desc26_get_decoder_config(p_desc, &i_extra_length);
    }
    if (desc26_get_decoder_config_flags(p_desc) == 3) {
        extra_str = "dec_config_identification_record";
        p_extra = desc26_get_dec_config_identification_record(p_desc, &i_extra_length);
    }
    for (i = 0; i < i_extra_length; i++)
        sprintf(psz_extra + 2 * i, "%02x", p_extra[i]);
    psz_extra[2 * i] = '\0';

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 "<METADATA_DESC metadata_application_format=\"0x%04x\""
                 " metadata_application_format_identifier=\"0x%08x\""
                 " metadata_format=\"0x%02x\""
                 " metadata_format_identifier=\"0x%08x\""
                 " metadata_service_id=\"0x%02x\""

                 " dsm_cc_flag=\"%u\""
                 " service_identification_record_length=\"%u\""
                 " service_identification_record=\"%s\""

                 " decoder_config_flags=\"%u\""
                 " %s_length=\"%u\""
                 " %s_data=\"%s\""
                 " decoder_config_metadata_service_id=\"%u\"/>",
                 desc26_get_metadata_application_format(p_desc),
                 desc26_get_metadata_application_format_identifier(p_desc),
                 desc26_get_metadata_format(p_desc),
                 desc26_get_metadata_format_identifier(p_desc),
                 desc26_get_metadata_service_id(p_desc),
                 desc26_get_dsm_cc_flag(p_desc),

                 desc26_get_service_identification_record_length(p_desc),
                 psz_service_identification_record,

                 desc26_get_decoder_config_flags(p_desc),
                 extra_str, i_extra_length,
                 extra_str, psz_extra,
                 desc26_get_decoder_config_metadata_service_id(p_desc)
                );
        break;
    default:
        pf_print(opaque,
                 "    - desc 26 metadata metadata_application_format=0x%04x"
                 " metadata_application_format_identifier=0x%08x"
                 " metadata_format=0x%02x"
                 " metadata_format_identifier=0x%08x"
                 " metadata_service_id=0x%02x"

                 " dsm_cc_flag=%u"
                 " service_identification_record_length=%u"
                 " service_identification_record=\"%s\""

                 " decoder_config_flags=%u"
                 " %s_length=%u"
                 " %s_data=\"%s\""
                 " decoder_config_metadata_service_id=%u",
                 desc26_get_metadata_application_format(p_desc),
                 desc26_get_metadata_application_format_identifier(p_desc),
                 desc26_get_metadata_format(p_desc),
                 desc26_get_metadata_format_identifier(p_desc),
                 desc26_get_metadata_service_id(p_desc),
                 desc26_get_dsm_cc_flag(p_desc),

                 desc26_get_service_identification_record_length(p_desc),
                 psz_service_identification_record,

                 desc26_get_decoder_config_flags(p_desc),
                 extra_str, i_extra_length,
                 extra_str, psz_extra,
                 desc26_get_decoder_config_metadata_service_id(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
