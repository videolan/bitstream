/*****************************************************************************
 * bat_print.h: ETSI EN 300 468 Bouquet Association Table (BAT) (printing)
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

#ifndef __BITSTREAM_DVB_BAT_PRINT_H__
#define __BITSTREAM_DVB_BAT_PRINT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descs_print.h>
#include <bitstream/dvb/si/bat.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Bouquet Association Table
 *****************************************************************************/
static inline void bat_table_print(uint8_t **pp_sections,
                                   f_print pf_print, void *print_opaque,
                                   f_iconv pf_iconv, void *iconv_opaque,
                                   print_type_t i_print_type)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque, "<BAT tid=\"%hhu\" networkid=\"%hu\" version=\"%hhu\" current_next=\"%d\">",
                 psi_table_get_tableid(pp_sections),
                 psi_table_get_tableidext(pp_sections),
                 psi_table_get_version(pp_sections),
                 !psi_table_get_current(pp_sections) ? 0 : 1);
        break;
    default:
        pf_print(print_opaque, "new BAT networkid=%hu version=%hhu%s",
                 psi_table_get_tableidext(pp_sections),
                 psi_table_get_version(pp_sections),
                 !psi_table_get_current(pp_sections) ? " (next)" : "");
    }

    descs_print(bat_get_descs(psi_table_get_section(pp_sections, 0)),
                pf_print, print_opaque, pf_iconv, iconv_opaque, i_print_type);

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);
        uint8_t *p_ts;
        int j = 0;

        while ((p_ts = bat_get_ts(p_section, j)) != NULL) {
            j++;
            switch (i_print_type) {
            case PRINT_XML:
                pf_print(print_opaque, "<TS tsid=\"%hu\" onid=\"%hu\">",
                         batn_get_tsid(p_ts), batn_get_onid(p_ts));
                break;
            default:
                pf_print(print_opaque, "  * ts tsid=%hu onid=%hu",
                         batn_get_tsid(p_ts), batn_get_onid(p_ts));
            }

            descs_print(batn_get_descs(p_ts), pf_print, print_opaque,
                        pf_iconv, iconv_opaque, i_print_type);

            switch (i_print_type) {
            case PRINT_XML:
                pf_print(print_opaque, "</TS>");
                break;
            default:
                break;
            }
        }
    }

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque, "</BAT>");
        break;
    default:
        pf_print(print_opaque, "end BAT");
    }
}

#ifdef __cplusplus
}
#endif

#endif
