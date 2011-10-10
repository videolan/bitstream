/*****************************************************************************
 * desc_48.h: ETSI EN 300 468 Descriptor 0x48: Service descriptor
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *          Georgi Chorbadzhiyski <georgi@unixsol.org>
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

#ifndef __BITSTREAM_DVB_DESC_48_H__
#define __BITSTREAM_DVB_DESC_48_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/dvb/si/strings.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x48: Service descriptor
 *****************************************************************************/
#define DESC48_HEADER_SIZE      (DESC_HEADER_SIZE + 1)

static inline void desc48_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x48);
}

static inline void desc48_set_type(uint8_t *p_desc, uint8_t i_type)
{
    p_desc[2] = i_type;
}

static inline uint8_t desc48_get_type(const uint8_t *p_desc)
{
    return p_desc[2];
}

static inline void desc48_set_provider(uint8_t *p_desc,
                                       const uint8_t *p_provider,
                                       uint8_t i_length)
{
    uint8_t *p = p_desc + DESC48_HEADER_SIZE;
    p[0] = i_length;
    memcpy(p + 1, p_provider, i_length);
}

static inline const uint8_t *desc48_get_provider(const uint8_t *p_desc,
                                                 uint8_t *pi_length)
{
    const uint8_t *p = p_desc + DESC48_HEADER_SIZE;
    *pi_length = p[0];
    return p + 1;
}

static inline void desc48_set_service(uint8_t *p_desc,
                                      const uint8_t *p_service,
                                      uint8_t i_length)
{
    uint8_t *p = p_desc + DESC48_HEADER_SIZE + 1 + p_desc[3];
    p[0] = i_length;
    memcpy(p + 1, p_service, i_length);
}

static inline uint8_t *desc48_get_service(const uint8_t *p_desc,
                                          uint8_t *pi_length)
{
    uint8_t *p = (uint8_t *)p_desc + DESC48_HEADER_SIZE + 1 + p_desc[3];
    *pi_length = p[0];
    return p + 1;
}

static inline void desc48_set_length(uint8_t *p_desc)
{
    uint8_t i_provider_len, i_service_len;
    desc48_get_provider(p_desc, &i_provider_len);
    desc48_get_service(p_desc, &i_service_len);
    desc_set_length(p_desc, DESC48_HEADER_SIZE - DESC_HEADER_SIZE
                            + 1 + i_provider_len
                            + 1 + i_service_len);
}

static inline bool desc48_validate(const uint8_t *p_desc)
{
    uint8_t i_length = desc_get_length(p_desc);
    const uint8_t *p = p_desc + DESC48_HEADER_SIZE;

    p += *p + 1;
    if (DESC48_HEADER_SIZE + 2 > i_length + DESC_HEADER_SIZE ||
        p + 1 - p_desc > i_length + DESC_HEADER_SIZE)
        return false;

    p += *p + 1;
    if (p - p_desc > i_length + DESC_HEADER_SIZE)
        return false;

    return true;
}

static inline void desc48_print(const uint8_t *p_desc,
                                f_print pf_print, void *print_opaque,
                                f_iconv pf_iconv, void *iconv_opaque,
                                print_type_t i_print_type)
{
    uint8_t i_provider_length, i_service_length;
    const uint8_t *p_provider = desc48_get_provider(p_desc, &i_provider_length);
    const uint8_t *p_service = desc48_get_service(p_desc, &i_service_length);
    char *psz_provider = dvb_string_get(p_provider, i_provider_length,
                                        pf_iconv, iconv_opaque);
    char *psz_service = dvb_string_get(p_service, i_service_length,
                                       pf_iconv, iconv_opaque);
    switch (i_print_type) {
    case PRINT_XML:
        psz_provider = dvb_string_xml_escape(psz_provider);
        psz_service = dvb_string_xml_escape(psz_service);
        pf_print(print_opaque,
                 "<SERVICE_DESC type=\"0x%hhx\" provider=\"%s\" service=\"%s\"/>",
                 desc48_get_type(p_desc), psz_provider, psz_service);
        break;
    default:
        pf_print(print_opaque,
                 "    - desc 48 service type=0x%hhx provider=\"%s\" service=\"%s\"",
                 desc48_get_type(p_desc), psz_provider, psz_service);
    }
    free(psz_provider);
    free(psz_service);
}

#ifdef __cplusplus
}
#endif

#endif
