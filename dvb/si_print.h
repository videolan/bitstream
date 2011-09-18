/*****************************************************************************
 * si_print.h: ETSI EN 300 468 Service Information (printing)
 *****************************************************************************
 * Copyright (C) 2010 VideoLAN
 * $Id: psi_print.h -1   $
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

/*
 * Placed here for dependancy reasons
 */

#ifndef __BITSTREAM_DVB_SI_PRINT_H__
#define __BITSTREAM_DVB_SI_PRINT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi.h>
#include <bitstream/dvb/si.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptors list
 *****************************************************************************/
static inline void descl_print(uint8_t *p_descl, uint16_t i_length,
                               f_print pf_print, void *print_opaque,
                               f_iconv pf_iconv, void *iconv_opaque,
                               print_type_t i_print_type)
{
    uint16_t j = 0;
    uint8_t *p_desc;
    uint32_t i_private_data_specifier = 0;

    while ((p_desc = descl_get_desc(p_descl, i_length, j)) != NULL) {
        uint8_t i_tag = desc_get_tag(p_desc);
        j++;

        desc_print_begin(p_desc, pf_print, print_opaque, i_print_type);

        if (i_private_data_specifier == 0x28) {
            /* EICTA */
            switch (i_tag) {
#define CASE_DESC(id)                                                       \
            case 0x##id:                                                    \
                if (desc##id##p28_validate(p_desc))                         \
                    desc##id##p28_print(p_desc, pf_print, print_opaque,     \
                                        i_print_type);                      \
                else                                                        \
                    desc_print_error(p_desc, pf_print, print_opaque,        \
                                     i_print_type);                         \
                goto print_end;

            CASE_DESC(83)
            CASE_DESC(88)

#undef CASE_DESC

            default:
                break;
            }
        }

        switch (i_tag) {
#define CASE_DESC(id)                                                       \
        case 0x##id:                                                        \
            if (desc##id##_validate(p_desc))                                \
                desc##id##_print(p_desc, pf_print, print_opaque,            \
                                 i_print_type);                             \
            else                                                            \
                desc_print_error(p_desc, pf_print, print_opaque,            \
                                 i_print_type);                             \
            break;

#define CASE_DESC_ICONV(id)                                                 \
        case 0x##id:                                                        \
            if (desc##id##_validate(p_desc))                                \
                desc##id##_print(p_desc, pf_print, print_opaque,            \
                                 pf_iconv, iconv_opaque, i_print_type);     \
            else                                                            \
                desc_print_error(p_desc, pf_print, print_opaque,            \
                                 i_print_type);                             \
            break;

        CASE_DESC(05)
        CASE_DESC(09)
        CASE_DESC(0a)
        CASE_DESC_ICONV(40)
        CASE_DESC(41)
        CASE_DESC(43)
        CASE_DESC(44)
        CASE_DESC(46)
        CASE_DESC_ICONV(48)
        CASE_DESC(4a)
        CASE_DESC(56)
        CASE_DESC(59)
        CASE_DESC(5a)
        CASE_DESC(6a)

#undef CASE_DESC
#undef CASE_DESC_ICONV

        case 0x5f:
            if (desc5f_validate(p_desc)) {
                desc5f_print(p_desc, pf_print, print_opaque, i_print_type);
                i_private_data_specifier = desc5f_get_specifier(p_desc);
            } else
                desc_print_error(p_desc, pf_print, print_opaque, i_print_type);
            break;

        default:
            desc_print(p_desc, pf_print, print_opaque, i_print_type);
            break;
        }

print_end:
        desc_print_end(p_desc, pf_print, print_opaque, i_print_type);
    }
}

static inline void descs_print(uint8_t *p_descs,
                               f_print pf_print, void *print_opaque,
                               f_iconv pf_iconv, void *iconv_opaque,
                               print_type_t i_print_type)
{
    descl_print(p_descs + DESCS_HEADER_SIZE, descs_get_length(p_descs),
                pf_print, print_opaque, pf_iconv, iconv_opaque, i_print_type);
}

/*****************************************************************************
 * Network Information Table
 *****************************************************************************/
static inline void nit_table_print(uint8_t **pp_sections,
                                   f_print pf_print, void *print_opaque,
                                   f_iconv pf_iconv, void *iconv_opaque,
                                   print_type_t i_print_type)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque, "<NIT tid=\"%hhu\" networkid=\"%hu\" version=\"%hhu\" current_next=\"%d\">",
                 psi_table_get_tableid(pp_sections),
                 psi_table_get_tableidext(pp_sections),
                 psi_table_get_version(pp_sections),
                 !psi_table_get_current(pp_sections) ? 0 : 1);
        break;
    default:
        pf_print(print_opaque, "new NIT %s networkid=%hu version=%hhu%s",
                 psi_table_get_tableid(pp_sections) == NIT_TABLE_ID_ACTUAL ?
                 "actual" : "other",
                 psi_table_get_tableidext(pp_sections),
                 psi_table_get_version(pp_sections),
                 !psi_table_get_current(pp_sections) ? " (next)" : "");
    }

    descs_print(nit_get_descs(psi_table_get_section(pp_sections, 0)),
                pf_print, print_opaque, pf_iconv, iconv_opaque, i_print_type);

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);
        uint8_t *p_ts;
        int j = 0;

        while ((p_ts = nit_get_ts(p_section, j)) != NULL) {
            j++;
            switch (i_print_type) {
            case PRINT_XML:
                pf_print(print_opaque, "<TS tsid=\"%hu\" onid=\"%hu\">",
                         nitn_get_tsid(p_ts), nitn_get_onid(p_ts));
                break;
            default:
                pf_print(print_opaque, "  * ts tsid=%hu onid=%hu",
                         nitn_get_tsid(p_ts), nitn_get_onid(p_ts));
            }

            descs_print(nitn_get_descs(p_ts), pf_print, print_opaque,
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
        pf_print(print_opaque, "</NIT>");
        break;
    default:
        pf_print(print_opaque, "end NIT");
    }
}

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
                pf_print(print_opaque, "  * service sid=%hu eit%s%s running=%hhu%s",
                         sdtn_get_sid(p_service),
                         sdtn_get_eitschedule(p_service) ? " schedule" : "",
                         sdtn_get_eitpresent(p_service) ? " present" : "",
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
