/*****************************************************************************
 * pmt_print.h: ISO/IEC 13818-1 Program Map Table (printing)
 *****************************************************************************
 * Copyright (C) 2010 VideoLAN
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
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

#ifndef __BITSTREAM_MPEG_PMT_PRINT_H__
#define __BITSTREAM_MPEG_PMT_PRINT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descs_print.h>
#include <bitstream/mpeg/psi/pmt.h>

#ifdef __cplusplus
extern "C"
{
#endif

static inline void pmt_print(uint8_t *p_pmt,
                             f_print pf_print, void *print_opaque,
                             f_iconv pf_iconv, void *iconv_opaque,
                             print_type_t i_print_type)
{
    uint8_t *p_es;
    uint8_t j = 0;

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque, "<PMT program=\"%hu\" version=\"%hhu\" current_next=\"%d\" pcrpid=\"%hu\">",
                 pmt_get_program(p_pmt), psi_get_version(p_pmt),
                 !psi_get_current(p_pmt) ? 0 : 1,
                 pmt_get_pcrpid(p_pmt));
        break;
    default:
        pf_print(print_opaque, "new PMT program=%hu version=%hhu%s pcrpid=%hu",
                 pmt_get_program(p_pmt), psi_get_version(p_pmt),
                 !psi_get_current(p_pmt) ? " (next)" : "",
                 pmt_get_pcrpid(p_pmt));
    }

    descs_print(pmt_get_descs(p_pmt), pf_print, print_opaque,
                pf_iconv, iconv_opaque, i_print_type);

    while ((p_es = pmt_get_es(p_pmt, j)) != NULL) {
        j++;
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(print_opaque, "<ES pid=\"%hu\" streamtype=\"0x%02x\" streamtype_txt=\"%s\">",
                     pmtn_get_pid(p_es),
                     pmtn_get_streamtype(p_es),
                     pmt_get_streamtype_txt(pmtn_get_streamtype(p_es))
                    );
            break;
        default:
            pf_print(print_opaque, "  * ES pid=%hu streamtype=0x%02x streamtype_txt=\"%s\"",
                     pmtn_get_pid(p_es),
                     pmtn_get_streamtype(p_es),
                     pmt_get_streamtype_txt(pmtn_get_streamtype(p_es))
                    );
        }

        descs_print(pmtn_get_descs(p_es), pf_print, print_opaque,
                    pf_iconv, iconv_opaque, i_print_type);

        switch (i_print_type) {
        case PRINT_XML:
            pf_print(print_opaque, "</ES>");
            break;
        default:
            break;
        }
    }

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque, "</PMT>");
        break;
    default:
        pf_print(print_opaque, "end PMT");
    }
}

#ifdef __cplusplus
}
#endif

#endif
