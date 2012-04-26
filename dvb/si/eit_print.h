/*****************************************************************************
 * eit_print.h: ETSI EN 300 468 Event Information Table (EIT) (printing)
 *****************************************************************************
 * Copyright (C) 2011 Unix Solutions Ltd.
 *
 * Authors: Georgi Chorbadzhiyski <georgi@unixsol.org>
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

#ifndef __BITSTREAM_DVB_EIT_PRINT_H__
#define __BITSTREAM_DVB_EIT_PRINT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descs_print.h>
#include <bitstream/dvb/si/datetime.h>
#include <bitstream/dvb/si/eit.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Event Information Table
 *****************************************************************************/
static inline void eit_print(uint8_t *p_eit,
                             f_print pf_print, void *print_opaque,
                             f_iconv pf_iconv, void *iconv_opaque,
                             print_type_t i_print_type)
{
    uint8_t *p_event;
    uint8_t j = 0;
    uint8_t i_tid = psi_get_tableid(p_eit);
    char *psz_tid = "unknown";

    if (i_tid == EIT_TABLE_ID_PF_ACTUAL)
        psz_tid = "actual_pf";
    else if (i_tid == EIT_TABLE_ID_PF_OTHER)
        psz_tid = "other_pf";
    else if (i_tid >= EIT_TABLE_ID_SCHED_ACTUAL_FIRST && i_tid <= EIT_TABLE_ID_SCHED_ACTUAL_LAST)
        psz_tid = "actual_schedule";
    else if (i_tid >= EIT_TABLE_ID_SCHED_OTHER_FIRST && i_tid <= EIT_TABLE_ID_SCHED_OTHER_LAST)
        psz_tid = "other_schedule";

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque,
                "<EIT tableid=\"0x%02x\" type=\"%s\" service_id=\"%u\" version=\"%u\""
                " current_next=\"%u\" tsid=\"%u\" onid=\"%u\""
                " seg_last_sec_number=\"%u\" last_table_id=\"0x%02x\">",
                 i_tid, psz_tid,
                 eit_get_sid(p_eit),
                 psi_get_version(p_eit),
                 !psi_get_current(p_eit) ? 0 : 1,
                 eit_get_tsid(p_eit),
                 eit_get_onid(p_eit),
                 eit_get_segment_last_sec_number(p_eit),
                 eit_get_last_table_id(p_eit)
                );
        break;
    default:
        pf_print(print_opaque,
                 "new EIT tableid=0x%02x type=%s service_id=%u version=%u%s tsid=%u"
                 " onid=%u seg_last_sec_number=%u last_table_id=0x%02x",
                 i_tid, psz_tid,
                 eit_get_sid(p_eit),
                 psi_get_version(p_eit),
                 !psi_get_current(p_eit) ? " (next)" : "",
                 eit_get_tsid(p_eit),
                 eit_get_onid(p_eit),
                 eit_get_segment_last_sec_number(p_eit),
                 eit_get_last_table_id(p_eit)
                );
    }

    while ((p_event = eit_get_event(p_eit, j)) != NULL) {
        j++;
        char start_str[24], duration_str[10];
        int duration, hour, min, sec;
        time_t start_ts;

        start_ts = dvb_time_format_UTC(eitn_get_start_time(p_event), NULL, start_str);

        dvb_time_decode_bcd(eitn_get_duration_bcd(p_event), &duration, &hour, &min, &sec);
        sprintf(duration_str, "%02d:%02d:%02d", hour, min, sec);

        switch (i_print_type) {
        case PRINT_XML:
            pf_print(print_opaque, "<EVENT id=\"%u\" start_time=\"%ld\" start_time_dec=\"%s\""
                                   " duration=\"%u\" duration_dec=\"%s\""
                                   " running_status=\"%d\" free_CA_mode=\"%d\">",
                     eitn_get_event_id(p_event),
                     start_ts, start_str,
                     duration, duration_str,
                     eitn_get_running_status(p_event),
                     eitn_get_free_CA_mode(p_event)
                    );
            break;
        default:
            pf_print(print_opaque, "  * EVENT id=%u start_time=%ld start_time_dec=\"%s\" duration=%u duration_dec=%s running_status=%d free_CA_mode=%d",
                     eitn_get_event_id(p_event),
                     start_ts, start_str,
                     duration, duration_str,
                     eitn_get_running_status(p_event),
                     eitn_get_free_CA_mode(p_event)
                    );
        }

        descs_print(eitn_get_descs(p_event), pf_print, print_opaque,
                    pf_iconv, iconv_opaque, i_print_type);

        switch (i_print_type) {
        case PRINT_XML:
            pf_print(print_opaque, "</EVENT>");
            break;
        default:
            break;
        }
    }

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque, "</EIT>");
        break;
    default:
        pf_print(print_opaque, "end EIT");
    }
}

#ifdef __cplusplus
}
#endif

#endif
