/*****************************************************************************
 * 35_print.h: SCTE 35 Digital Program Insertion Cueing Message for Cable
 * (printing)
 *****************************************************************************
 * Copyright (C) 2015 VideoLAN
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

#ifndef __BITSTREAM_SCTE_35_PRINT_H__
#define __BITSTREAM_SCTE_35_PRINT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descs_print.h>
#include <bitstream/scte/35.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Splice Information Table
 *****************************************************************************/
static inline const char *scte35_get_command_type_txt(uint8_t i_type) {
    switch (i_type) {
        case SCTE35_NULL_COMMAND: return "null";
        case SCTE35_SCHEDULE_COMMAND: return "schedule";
        case SCTE35_INSERT_COMMAND: return "insert";
        case SCTE35_TIME_SIGNAL_COMMAND: return "time_signal";
        case SCTE35_BANDWIDTH_RESERVATION_COMMAND: return "bandwidth_reservation";
        case SCTE35_PRIVATE_COMMAND: return "private";
        default: return "reserved";
    }
}

static inline void scte35_null_print(const uint8_t *p_scte35,
        f_print pf_print, void *print_opaque, print_type_t i_print_type)
{
    uint64_t i_pts_adjustment = scte35_get_pts_adjustment(p_scte35);

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque,
                 "<SCTE35 command=\"%" PRIu8 "\" command_str=\"null\" pts_adjustment=\"%" PRIu64 "\">",
                 SCTE35_NULL_COMMAND, i_pts_adjustment);
        break;
    default:
        pf_print(print_opaque,
                 "new SCTE35 command=%" PRIu8 " command_str=null pts_adjustment=%" PRIu64,
                 SCTE35_NULL_COMMAND, i_pts_adjustment);
    }
}

static inline void scte35_insert_print(const uint8_t *p_scte35,
        f_print pf_print, void *print_opaque, print_type_t i_print_type)
{
    uint64_t i_pts_adjustment = scte35_get_pts_adjustment(p_scte35);
    uint32_t i_event_id = scte35_insert_get_event_id(p_scte35);

    if (scte35_insert_has_cancel(p_scte35)) {
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(print_opaque,
                     "<SCTE35 command=\"%" PRIu8 "\" command_str=\"insert\" pts_adjustment=\"%" PRIu64 "\" event_id=\"%" PRIu32 "\" cancel=\"1\">",
                     SCTE35_INSERT_COMMAND, i_pts_adjustment, i_event_id);
            break;
        default:
            pf_print(print_opaque,
                     "new SCTE35 command=%" PRIu8 " command_str=insert pts_adjustment=%" PRIu64 " event_id=%" PRIu32 " cancel=true",
                     SCTE35_INSERT_COMMAND, i_pts_adjustment, i_event_id);
        }
        return;
    }

    bool b_out_of_network = scte35_insert_has_out_of_network(p_scte35);
    bool b_program_splice = scte35_insert_has_program_splice(p_scte35);
    bool b_duration = scte35_insert_has_duration(p_scte35);
    bool b_splice_immediate = scte35_insert_has_splice_immediate(p_scte35);
    uint16_t i_unique_program_id = scte35_insert_get_unique_program_id(p_scte35);

    char psz_duration[256];
    psz_duration[0] = '\0';
    psz_duration[255] = '\0';
    if (b_duration) {
        const uint8_t *p_break_duration =
            scte35_insert_get_break_duration(p_scte35);
        bool b_auto_return =
            scte35_break_duration_has_auto_return(p_break_duration);
        uint64_t i_duration =
            scte35_break_duration_get_duration(p_break_duration);

        switch (i_print_type) {
        case PRINT_XML:
            snprintf(psz_duration, 255,
                     " auto_return=\"%d\" duration=\"%" PRIu64 "\"",
                     b_auto_return ? 1 : 0, i_duration);
            break;
        default:
            snprintf(psz_duration, 255,
                     " auto_return=%s duration=%" PRIu64 "",
                     b_auto_return ? "true" : "false", i_duration);
        }
    }

    char psz_splice_time[256];
    psz_splice_time[0] = '\0';
    psz_splice_time[255] = '\0';
    if (b_splice_immediate) {
        switch (i_print_type) {
        case PRINT_XML:
            snprintf(psz_splice_time, 255, " splice_time=\"immediate\"");
            break;
        default:
            snprintf(psz_splice_time, 255, " splice_time=immediate");
        }
    } else {
        const uint8_t *p_splice_time = NULL;
        if (b_program_splice)
            p_splice_time = scte35_insert_get_splice_time(p_scte35);
        else if (scte35_insert_get_component_count(p_scte35))
            p_splice_time = scte35_insert_component_get_splice_time(
                    scte35_insert_get_component(p_scte35, 0));

        if (p_splice_time == NULL ||
            !scte35_splice_time_has_time_specified(p_splice_time)) {
            switch (i_print_type) {
            case PRINT_XML:
                snprintf(psz_splice_time, 255, " splice_time=\"undefined\"");
                break;
            default:
                snprintf(psz_splice_time, 255, " splice_time=undefined");
            }
        } else {
            uint64_t i_pts_time =
                scte35_splice_time_get_pts_time(p_splice_time);

            switch (i_print_type) {
            case PRINT_XML:
                snprintf(psz_splice_time, 255, " splice_time=\"%" PRIu64 "\"",
                         i_pts_time);
                break;
            default:
                snprintf(psz_splice_time, 255, " splice_time=%" PRIu64 "",
                         i_pts_time);
            }
        }
    }

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque,
                 "<SCTE35 command=\"%" PRIu8 "\" command_str=\"insert\" pts_adjustment=\"%" PRIu64 "\" event_id=\"%" PRIu32 "\" cancel=\"0\" out_of_network=\"%d\" program_splice=\"%d\"%s%s unique_program_id=\"%" PRIu16 "\">",
                 SCTE35_INSERT_COMMAND, i_pts_adjustment, i_event_id,
                 b_out_of_network ? 1 : 0, b_program_splice ? 1 : 0,
                 psz_splice_time, psz_duration, i_unique_program_id);
        break;
    default:
        pf_print(print_opaque,
                 "new SCTE35 command=%" PRIu8 " command_str=insert pts_adjustment=%" PRIu64 " event_id=%" PRIu32 " cancel=false out_of_network=%s program_splice=%s%s%s unique_program_id=%" PRIu16,
                 SCTE35_INSERT_COMMAND, i_pts_adjustment, i_event_id,
                 b_out_of_network ? "true" : "false",
                 b_program_splice ? "true" : "false",
                 psz_splice_time, psz_duration, i_unique_program_id);
    }
}

static inline void scte35_time_signal_print(const uint8_t *p_scte35,
        f_print pf_print, void *print_opaque, print_type_t i_print_type)
{
    uint64_t i_pts_adjustment = scte35_get_pts_adjustment(p_scte35);
    const uint8_t *p_splice_time = scte35_time_signal_get_splice_time(p_scte35);

    switch (i_print_type) {
    case PRINT_XML:
        if (scte35_splice_time_has_time_specified(p_splice_time))
            pf_print(print_opaque,
                     "<SCTE35 command=\"%" PRIu8 "\" command_str=\"time_signal\" pts_adjustment=\"%" PRIu64 "\" splice_time=\"%" PRIu64 "\">",
                     SCTE35_TIME_SIGNAL_COMMAND, i_pts_adjustment,
                     scte35_splice_time_get_pts_time(p_splice_time));
        else
            pf_print(print_opaque,
                     "<SCTE35 command=\"%" PRIu8 "\" command_str=\"time_signal\" pts_adjustment=\"%" PRIu64 "\" splice_time=\"undefined\">",
                     SCTE35_TIME_SIGNAL_COMMAND, i_pts_adjustment);
        break;
    default:
        if (scte35_splice_time_has_time_specified(p_splice_time))
            pf_print(print_opaque,
                     "new SCTE35 command=%" PRIu8 " command_str=time_signal pts_adjustment=%" PRIu64 " splice_time=%" PRIu64,
                     SCTE35_TIME_SIGNAL_COMMAND, i_pts_adjustment,
                     scte35_splice_time_get_pts_time(p_splice_time));
        else
            pf_print(print_opaque,
                     "new SCTE35 command=%" PRIu8 " command_str=time_signal pts_adjustment=%" PRIu64 " splice_time=undefined",
                     SCTE35_TIME_SIGNAL_COMMAND, i_pts_adjustment);
    }
}

static inline void scte35_private_print(const uint8_t *p_scte35,
        f_print pf_print, void *print_opaque, print_type_t i_print_type)
{
    uint64_t i_pts_adjustment = scte35_get_pts_adjustment(p_scte35);
    uint32_t i_identifier = scte35_private_get_identifier(p_scte35);

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque,
                 "<SCTE35 command=\"%" PRIu8 "\" command_str=\"private\" pts_adjustment=\"%" PRIu64 "\" identifier=\"%" PRIu32 "\" />",
                 SCTE35_PRIVATE_COMMAND, i_pts_adjustment, i_identifier);
        break;
    default:
        pf_print(print_opaque,
                 "new SCTE35 command=%" PRIu8 " command_str=private pts_adjustment=%" PRIu64 " identifier=%" PRIu32,
                 SCTE35_PRIVATE_COMMAND, i_pts_adjustment, i_identifier);
    }
}

static inline void scte35_print(const uint8_t *p_scte35,
        f_print pf_print, void *print_opaque, print_type_t i_print_type)
{
    uint8_t i_type = scte35_get_command_type(p_scte35);
    bool done = false;

    switch (i_type) {
    case SCTE35_NULL_COMMAND:
        scte35_null_print(p_scte35, pf_print, print_opaque, i_print_type);
        done = true;
        break;
    case SCTE35_INSERT_COMMAND:
        scte35_insert_print(p_scte35, pf_print, print_opaque, i_print_type);
        done = true;
        break;
    case SCTE35_TIME_SIGNAL_COMMAND:
        scte35_time_signal_print(p_scte35, pf_print, print_opaque, i_print_type);
        done = true;
        break;
    case SCTE35_PRIVATE_COMMAND:
        scte35_private_print(p_scte35, pf_print, print_opaque, i_print_type);
        done = true;
        break;
    default:
        break;
    }

    if (!done) {
        uint64_t i_pts_adjustment = scte35_get_pts_adjustment(p_scte35);

        switch (i_print_type) {
        case PRINT_XML:
            pf_print(print_opaque,
                     "<SCTE35 command=\"%" PRIu8 "\" command_str=\"%s\" pts_adjustment=\"%" PRIu64 "\">",
                     i_type, scte35_get_command_type_txt(i_type), i_pts_adjustment);
            break;
        default:
            pf_print(print_opaque,
                     "new SCTE35 command=%" PRIu8 " command_str=%s pts_adjustment=%" PRIu64,
                     i_type, scte35_get_command_type_txt(i_type), i_pts_adjustment);
        }
    }

    /* TODO: print descriptors */

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque, "</SCTE35>");
        break;
    default:
        pf_print(print_opaque, "end SCTE35");
    }
}

#ifdef __cplusplus
}
#endif

#endif
