/*****************************************************************************
 * psi_print.h: ISO/IEC 13818-1 Program Stream Information (printing)
 *****************************************************************************
 * Copyright (C) 2010 VideoLAN
 * $Id: psi_print.h -1   $
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 *****************************************************************************/

/*
 * Placed here for dependancy reasons
 */

#ifndef __BITSTREAM_MPEG_PSI_PRINT_H__
#define __BITSTREAM_MPEG_PSI_PRINT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi.h>

/* here you must manually include <bitstream/dvb/si.h> or the ATSC equivalent
 * if/when it is available */

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Conditional Access Table
 *****************************************************************************/
static inline void cat_table_print(uint8_t **pp_sections, f_print pf_print,
                                   void *opaque, print_type_t i_print_type)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<CAT version=\"%hhu\" current_next=\"%d\">",
                 psi_table_get_version(pp_sections),
                 !psi_table_get_current(pp_sections) ? 0 : 1);
        break;
    default:
        pf_print(opaque, "new CAT version=%hhu%s",
                 psi_table_get_version(pp_sections),
                 !psi_table_get_current(pp_sections) ? " (next)" : "");
    }

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);
        uint8_t *p_descs = cat_alloc_descs(p_section);

        descs_print(p_descs, pf_print, opaque, NULL, NULL, i_print_type);

        cat_free_descs(p_descs);
    }

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "</CAT>");
        break;
    default:
        pf_print(opaque, "end CAT");
    }
}

/*****************************************************************************
 * Program Map Table
 *****************************************************************************/
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
            pf_print(print_opaque, "<ES pid=\"%hu\" streamtype=\"0x%hx\">", pmtn_get_pid(p_es),
                     pmtn_get_streamtype(p_es));
            break;
        default:
            pf_print(print_opaque, "  * ES pid=%hu streamtype=0x%hx", pmtn_get_pid(p_es),
                     pmtn_get_streamtype(p_es));
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
