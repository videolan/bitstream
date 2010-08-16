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
#include <bitstream/dvb/si.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptors list
 *****************************************************************************/
static inline void descs_print(uint8_t *p_descs, f_print pf_print, void *opaque)
{
    uint16_t j = 0;
    uint8_t *p_desc;

    while ((p_desc = descs_get_desc(p_descs, j)) != NULL) {
        j++;

        /* I am not proud of this */
        switch (desc_get_tag(p_desc)) {
        case 0x05:
            desc05_print(p_desc, pf_print, opaque);
            break;
        case 0x09:
            desc09_print(p_desc, pf_print, opaque);
            break;
        case 0x0a:
            desc0a_print(p_desc, pf_print, opaque);
            break;
        case 0x40:
            desc40_print(p_desc, pf_print, opaque);
            break;
        case 0x48:
            desc48_print(p_desc, pf_print, opaque);
            break;
        case 0x56:
            desc56_print(p_desc, pf_print, opaque);
            break;
        case 0x59:
            desc59_print(p_desc, pf_print, opaque);
            break;
        case 0x6a:
            desc6a_print(p_desc, pf_print, opaque);
            break;
        default:
            desc_print(p_desc, pf_print, opaque);
            break;
        }
    }
}

/*****************************************************************************
 * Program Map Table
 *****************************************************************************/
static inline void pmt_print(uint8_t *p_pmt, f_print pf_print, void *opaque)
{
    uint8_t *p_es;
    uint8_t j = 0;

    pf_print(opaque, "new PMT program=%hu version=%hhu%s pcrpid=%hu",
             pmt_get_program(p_pmt), psi_get_version(p_pmt),
             !psi_get_current(p_pmt) ? " (next)" : "",
             pmt_get_pcrpid(p_pmt));
    descs_print(pmt_get_descs(p_pmt), pf_print, opaque);

    while ((p_es = pmt_get_es(p_pmt, j)) != NULL) {
        j++;
        pf_print(opaque, "  * ES pid=%hu streamtype=0x%hx", pmtn_get_pid(p_es),
                 pmtn_get_streamtype(p_es));
        descs_print(pmtn_get_descs(p_es), pf_print, opaque);
    }

    pf_print(opaque, "end PMT");
}

/*****************************************************************************
 * Network Information Table
 *****************************************************************************/
static inline void nit_table_print(uint8_t **pp_sections, f_print pf_print,
                                   void *opaque)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    pf_print(opaque, "new NIT %s networkid=%hu version=%hhu%s",
             psi_table_get_tableid(pp_sections) == NIT_TABLE_ID_ACTUAL ?
             "actual" : "other",
             psi_table_get_tableidext(pp_sections),
             psi_table_get_version(pp_sections),
             !psi_table_get_current(pp_sections) ? " (next)" : "");
    descs_print(nit_get_descs(psi_table_get_section(pp_sections, 0)),
                pf_print, opaque);

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);
        uint8_t *p_ts;
        int j = 0;

        while ((p_ts = nit_get_ts(p_section, j)) != NULL) {
            j++;
            pf_print(opaque, "  * ts tsid=%hu onid=%hu",
                     nitn_get_tsid(p_ts), nitn_get_onid(p_ts));
            descs_print(nitn_get_descs(p_ts), pf_print, opaque);
        }
    }

    pf_print(opaque, "end NIT");
}

/*****************************************************************************
 * Service Description Table
 *****************************************************************************/
static inline void sdt_table_print(uint8_t **pp_sections, f_print pf_print,
                                   void *opaque)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    pf_print(opaque, "new SDT %s tsid=%hu version=%hhu%s onid=%hu",
             psi_table_get_tableid(pp_sections) == SDT_TABLE_ID_ACTUAL ?
             "actual" : "other",
             psi_table_get_tableidext(pp_sections),
             psi_table_get_version(pp_sections),
             !psi_table_get_current(pp_sections) ? " (next)" : "",
             sdt_get_onid(psi_table_get_section(pp_sections, 0)));

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);
        uint8_t *p_service;
        int j = 0;

        while ((p_service = sdt_get_service(p_section, j)) != NULL) {
            j++;
            pf_print(opaque, "  * service sid=%hu eit%s%s running=%hhu%s",
                     sdtn_get_sid(p_service),
                     sdtn_get_eitschedule(p_service) ? " schedule" : "",
                     sdtn_get_eitpresent(p_service) ? " present" : "",
                     sdtn_get_running(p_service),
                     sdtn_get_ca(p_service) ? " scrambled" : "");
            descs_print(sdtn_get_descs(p_service), pf_print, opaque);
        }
    }

    pf_print(opaque, "end SDT");
}

#ifdef __cplusplus
}
#endif

#endif
