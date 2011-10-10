/*****************************************************************************
 * sdt_print.h: ETSI EN 300 468 Service Definition Table (SDT) (printing)
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

#ifndef __BITSTREAM_DVB_SDT_PRINT_H__
#define __BITSTREAM_DVB_SDT_PRINT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descs_print.h>
#include <bitstream/dvb/si/sdt.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Service Description Table
 *****************************************************************************/
static inline void sdt_table_print(uint8_t **pp_sections,
                                   f_print pf_print, void *print_opaque,
                                   f_iconv pf_iconv, void *iconv_opaque,
                                   print_type_t i_print_type)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque, "<SDT tid=\"%hhu\" tsid=\"%hu\" version=\"%hhu\" current_next=\"%d\" onid=\"%hu\">",
                 psi_table_get_tableid(pp_sections),
                 psi_table_get_tableidext(pp_sections),
                 psi_table_get_version(pp_sections),
                 !psi_table_get_current(pp_sections) ? 0 : 1,
                 sdt_get_onid(psi_table_get_section(pp_sections, 0)));
        break;
    default:
        pf_print(print_opaque, "new SDT %s tsid=%hu version=%hhu%s onid=%hu",
                 psi_table_get_tableid(pp_sections) == SDT_TABLE_ID_ACTUAL ?
                 "actual" : "other",
                 psi_table_get_tableidext(pp_sections),
                 psi_table_get_version(pp_sections),
                 !psi_table_get_current(pp_sections) ? " (next)" : "",
                 sdt_get_onid(psi_table_get_section(pp_sections, 0)));
    }

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);
        uint8_t *p_service;
        int j = 0;

        while ((p_service = sdt_get_service(p_section, j)) != NULL) {
            j++;
            switch (i_print_type) {
            case PRINT_XML:
                pf_print(print_opaque, "<SERVICE sid=\"%hu\" eit_schedule=\"%d\" eit_pf=\"%d\" running=\"%hhu\" free_CA=\"%d\">",
                         sdtn_get_sid(p_service),
                         sdtn_get_eitschedule(p_service) ? 1 : 0,
                         sdtn_get_eitpresent(p_service) ? 1 : 0,
                         sdtn_get_running(p_service),
                         sdtn_get_ca(p_service) ? 1 : 0);
                break;
            default:
                pf_print(print_opaque, "  * service sid=%hu%s%s running=%hhu%s",
                         sdtn_get_sid(p_service),
                         sdtn_get_eitschedule(p_service) ? " eit_schedule" : "",
                         sdtn_get_eitpresent(p_service) ? " eit_pf" : "",
                         sdtn_get_running(p_service),
                         sdtn_get_ca(p_service) ? " scrambled" : "");
            }

            descs_print(sdtn_get_descs(p_service), pf_print, print_opaque,
                        pf_iconv, iconv_opaque, i_print_type);

            switch (i_print_type) {
            case PRINT_XML:
                pf_print(print_opaque, "</SERVICE>");
                break;
            default:
                break;
            }
        }
    }

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque, "</SDT>");
        break;
    default:
        pf_print(print_opaque, "end SDT");
    }
}

#ifdef __cplusplus
}
#endif

#endif
