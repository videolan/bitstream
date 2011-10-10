/*****************************************************************************
 * desc_5d.h: ETSI EN 300 468 Descriptor 0x5d: Multilingual service name
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

#ifndef __BITSTREAM_DVB_DESC_5D_H__
#define __BITSTREAM_DVB_DESC_5D_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/mpeg/psi/desc_0a.h>
#include <bitstream/dvb/si/strings.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x5d: Multilingual service name
 *****************************************************************************/
#define DESC5D_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC5D_DATA_SIZE        5

static inline void desc5d_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x5d);
}

#define desc5dn_set_code desc0an_set_code
#define desc5dn_get_code desc0an_get_code

static inline uint8_t desc5dn_get_provider_name_length(const uint8_t *p_desc_n)
{
    return p_desc_n[3];
}

static inline const uint8_t *desc5dn_get_provider_name(const uint8_t *p_desc_n, uint8_t *pi_length)
{
    *pi_length = desc5dn_get_provider_name_length(p_desc_n);
    return p_desc_n + 4;
}

static inline void desc5dn_set_provider_name(uint8_t *p_desc_n, const uint8_t *p_network_name, uint8_t i_length)
{
    p_desc_n[3] = i_length;
    memcpy(p_desc_n + 4, p_network_name, i_length);
}

static inline uint8_t desc5dn_get_service_name_length(const uint8_t *p_desc_n)
{
    return p_desc_n[4 + desc5dn_get_provider_name_length(p_desc_n)];
}

static inline const uint8_t *desc5dn_get_service_name(const uint8_t *p_desc_n, uint8_t *pi_length)
{
    *pi_length = desc5dn_get_service_name_length(p_desc_n);
    return p_desc_n + 5 + desc5dn_get_provider_name_length(p_desc_n);
}

static inline void desc5dn_set_service_name(uint8_t *p_desc_n, const uint8_t *p_network_name, uint8_t i_length)
{
    p_desc_n[4 + desc5dn_get_provider_name_length(p_desc_n)] = i_length;
    memcpy(p_desc_n + 5 + desc5dn_get_provider_name_length(p_desc_n),
        p_network_name, i_length);
}

static inline uint8_t *desc5d_get_data(const uint8_t *p_desc, uint8_t n)
{
    const uint8_t *p_desc_n = p_desc + DESC5D_HEADER_SIZE;
    uint8_t i_desc_size = desc_get_length(p_desc);

    while (n) {
        if (p_desc_n + DESC5D_DATA_SIZE - p_desc > i_desc_size)
            return NULL;
        p_desc_n += DESC5D_DATA_SIZE
            + desc5dn_get_provider_name_length(p_desc_n)
            + desc5dn_get_service_name_length(p_desc_n);
        n--;
    }
    if (p_desc_n - p_desc > i_desc_size)
        return NULL;
    return (uint8_t *)p_desc_n;
}

static inline bool desc5d_validate(const uint8_t *p_desc)
{
    const uint8_t *p_desc_n = p_desc + DESC5D_HEADER_SIZE;
    int i_desc_size = desc_get_length(p_desc);
    while (i_desc_size > 0)
    {
        uint8_t i_data_sz = DESC5D_DATA_SIZE
            + desc5dn_get_provider_name_length(p_desc_n)
            + desc5dn_get_service_name_length(p_desc_n);
        i_desc_size -= i_data_sz;
        p_desc_n    += i_data_sz;
    }
    return i_desc_size == 0;
}

static inline void desc5d_print(const uint8_t *p_desc,
                                f_print pf_print, void *print_opaque,
                                f_iconv pf_iconv, void *iconv_opaque,
                                print_type_t i_print_type)
{
    const uint8_t *p_desc_n;
    uint8_t j = 0;

    while ((p_desc_n = desc5d_get_data(p_desc, j++)) != NULL) {
        uint8_t i_provider_name_length, i_service_name_length;
        const uint8_t *p_provider_name = desc5dn_get_provider_name(p_desc_n, &i_provider_name_length);
        const uint8_t *p_service_name  = desc5dn_get_service_name(p_desc_n, &i_service_name_length);
        char *psz_provider_name = dvb_string_get(p_provider_name, i_provider_name_length, pf_iconv, iconv_opaque);
        char *psz_service_name = dvb_string_get(p_service_name, i_service_name_length, pf_iconv, iconv_opaque);
        switch (i_print_type) {
        case PRINT_XML:
            psz_provider_name = dvb_string_xml_escape(psz_provider_name);
            pf_print(print_opaque,
                     "<MULTILINGUAL_SERVICE_NAME_DESC code=\"%3.3s\" provider=\"%s\" service=\"%s\"/>",
                     desc5dn_get_code(p_desc_n),
                     psz_provider_name,
                     psz_service_name
                    );
            break;
        default:
            pf_print(print_opaque,
                     "    - desc 5d multilingual_service_name code=\"%3.3s\" provider=\"%s\" service=\"%s\"",
                     desc5dn_get_code(p_desc_n),
                     psz_provider_name,
                     psz_service_name
                    );
        }
        free(psz_provider_name);
        free(psz_service_name);
    }
}

#ifdef __cplusplus
}
#endif

#endif
