/*****************************************************************************
 * desc_40.h: ETSI EN 300 468 Descriptor 0x40: Network name descriptor
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

#ifndef __BITSTREAM_DVB_DESC_40_H__
#define __BITSTREAM_DVB_DESC_40_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/dvb/si/strings.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x40: Network name descriptor
 *****************************************************************************/
#define DESC40_HEADER_SIZE      DESC_HEADER_SIZE

static inline void desc40_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x40);
}

static inline void desc40_set_networkname(uint8_t *p_desc,
                                          const uint8_t *p_network_name,
                                          uint8_t i_length)
{
    desc_set_length(p_desc, i_length);
    memcpy(p_desc + 2, p_network_name, i_length);
}

static inline const uint8_t *desc40_get_networkname(const uint8_t *p_desc,
                                                    uint8_t *pi_length)
{
    *pi_length = desc_get_length(p_desc);
    return p_desc + 2;
}

static inline bool desc40_validate(const uint8_t *p_desc)
{
    return true;
}

static inline void desc40_print(const uint8_t *p_desc,
                                f_print pf_print, void *print_opaque,
                                f_iconv pf_iconv, void *iconv_opaque,
                                print_type_t i_print_type)
{
    uint8_t i_network_name_length;
    bool b_bouquet_name = desc_get_tag(p_desc) == 0x47;
    const uint8_t *p_network_name = desc40_get_networkname(p_desc,
                                                     &i_network_name_length);
    char *psz_network_name = dvb_string_get(p_network_name,
                                            i_network_name_length,
                                            pf_iconv, iconv_opaque);

    switch (i_print_type) {
    case PRINT_XML:
        psz_network_name = dvb_string_xml_escape(psz_network_name);
        if ( !b_bouquet_name )
            pf_print(print_opaque, "<NETWORK_NAME_DESC networkname=\"%s\"/>",
                     psz_network_name);
        else
            pf_print(print_opaque, "<BOUQUET_NAME_DESC bouquetname=\"%s\"/>",
                     psz_network_name);
        break;
    default:
        if ( !b_bouquet_name )
            pf_print(print_opaque, "    - desc 40 networkname=\"%s\"",
                     psz_network_name);
        else
            pf_print(print_opaque, "    - desc 47 bouquetname=\"%s\"",
                     psz_network_name);
    }
    free(psz_network_name);
}

#ifdef __cplusplus
}
#endif

#endif
