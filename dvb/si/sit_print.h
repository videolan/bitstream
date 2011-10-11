/*****************************************************************************
 * sit_print.h: ISO/IEC 13818-1 Selection Information Table (printing)
 *****************************************************************************
 * Copyright (C) 2010 VideoLAN
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

#ifndef __BITSTREAM_DVB_SIT_PRINT_H__
#define __BITSTREAM_DVB_SIT_PRINT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descs_print.h>
#include <bitstream/dvb/si/sit.h>

#ifdef __cplusplus
extern "C"
{
#endif

static inline void sit_print(uint8_t *p_sit,
                             f_print pf_print, void *print_opaque,
                             f_iconv pf_iconv, void *iconv_opaque,
                             print_type_t i_print_type)
{
    uint8_t *p_service;
    uint8_t j = 0;

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque, "<SIT version=\"%hhu\" current_next=\"%d\">",
                 psi_get_version(p_sit),
                 !psi_get_current(p_sit) ? 0 : 1);
        break;
    default:
        pf_print(print_opaque, "new SIT version=%hhu%s",
                 psi_get_version(p_sit),
                 !psi_get_current(p_sit) ? " (next)" : "");
    }

    descs_print(sit_get_descs(p_sit), pf_print, print_opaque,
                pf_iconv, iconv_opaque, i_print_type);

    while ((p_service = sit_get_service(p_sit, j)) != NULL) {
        j++;
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(print_opaque, "<SERVICE sid=\"%hu\" running_status=\"%u\">",
                     sitn_get_sid(p_service),
                     sitn_get_running_status(p_service)
                    );
            break;
        default:
            pf_print(print_opaque, "  * SERVICE sid=%hu running_status=%u",
                     sitn_get_sid(p_service),
                     sitn_get_running_status(p_service)
                    );
        }

        descs_print(sitn_get_descs(p_service), pf_print, print_opaque,
                    pf_iconv, iconv_opaque, i_print_type);

        switch (i_print_type) {
        case PRINT_XML:
            pf_print(print_opaque, "</SERVICE>");
            break;
        default:
            break;
        }
    }

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque, "</SIT>");
        break;
    default:
        pf_print(print_opaque, "end SIT");
    }
}

#ifdef __cplusplus
}
#endif

#endif
