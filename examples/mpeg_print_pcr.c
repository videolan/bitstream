/*****************************************************************************
 * mpeg_print_pcr.c: Prints PCR PID from the first program of the stream
 *****************************************************************************
 * Copyright (C) 2011 VideoLAN
 * $Id$
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

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#include <bitstream/mpeg/ts.h>
#include <bitstream/mpeg/psi.h>

/*****************************************************************************
 * Local declarations
 *****************************************************************************/
#define MAX_PIDS    8192
#define READ_ONCE   7

typedef struct ts_pid_t {
    int i_psi_refcount;
    int8_t i_last_cc;

    /* biTStream PSI section gathering */
    uint8_t *p_psi_buffer;
    uint16_t i_psi_buffer_used;
} ts_pid_t;

typedef struct sid_t {
    uint16_t i_sid, i_pmt_pid;
    uint8_t *p_current_pmt;
} sid_t;

ts_pid_t p_pids[MAX_PIDS];
static sid_t **pp_sids = NULL;
static int i_nb_sids = 0;

static PSI_TABLE_DECLARE(pp_current_pat_sections);
static PSI_TABLE_DECLARE(pp_next_pat_sections);

/*****************************************************************************
 * handle_pat
 *****************************************************************************/
static void handle_pat(void)
{
    PSI_TABLE_DECLARE(pp_old_pat_sections);
    uint8_t i_last_section = psi_table_get_lastsection(pp_next_pat_sections);
    uint8_t i;

    if (psi_table_validate(pp_current_pat_sections) &&
        psi_table_compare(pp_current_pat_sections, pp_next_pat_sections)) {
        /* Identical PAT. Shortcut. */
        psi_table_free(pp_next_pat_sections);
        psi_table_init(pp_next_pat_sections);
        return;
    }

    if (!pat_table_validate(pp_next_pat_sections)) {
        psi_table_free(pp_next_pat_sections);
        psi_table_init(pp_next_pat_sections);
        return;
    }

    /* Switch tables. */
    psi_table_copy(pp_old_pat_sections, pp_current_pat_sections);
    psi_table_copy(pp_current_pat_sections, pp_next_pat_sections);
    psi_table_init(pp_next_pat_sections);

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_current_pat_sections, i);
        const uint8_t *p_program;
        int j = 0;

        while ((p_program = pat_get_program(p_section, j)) != NULL) {
            const uint8_t *p_old_program = NULL;
            uint16_t i_sid = patn_get_program(p_program);
            uint16_t i_pid = patn_get_pid(p_program);
            j++;

            if (i_sid == 0)
                continue; /* NIT */

            if (!psi_table_validate(pp_old_pat_sections)
                  || (p_old_program =
                      pat_table_find_program(pp_old_pat_sections, i_sid))
                       == NULL
                  || patn_get_pid(p_old_program) != i_pid) {
                sid_t *p_sid;
                int i_pmt;
                if (p_old_program != NULL)
                    p_pids[patn_get_pid(p_old_program)].i_psi_refcount--;
                p_pids[i_pid].i_psi_refcount++;

                for (i_pmt = 0; i_pmt < i_nb_sids; i_pmt++)
                    if (pp_sids[i_pmt]->i_sid == i_sid ||
                        pp_sids[i_pmt]->i_sid == 0)
                        break;

                if (i_pmt == i_nb_sids) {
                    p_sid = malloc(sizeof(sid_t));
                    pp_sids = realloc(pp_sids, ++i_nb_sids * sizeof(sid_t *));
                    pp_sids[i_pmt] = p_sid;
                    p_sid->p_current_pmt = NULL;
                }
                else
                    p_sid = pp_sids[i_pmt];

                p_sid->i_sid = i_sid;
                p_sid->i_pmt_pid = i_pid;
            }
        }
    }

    if (psi_table_validate(pp_old_pat_sections)) {
        i_last_section = psi_table_get_lastsection( pp_old_pat_sections );
        for (i = 0; i <= i_last_section; i++) {
            uint8_t *p_section = psi_table_get_section(pp_old_pat_sections, i);
            const uint8_t *p_program;
            int j = 0;

            while ((p_program = pat_get_program(p_section, j)) != NULL) {
                uint16_t i_sid = patn_get_program(p_program);
                j++;

                if (i_sid == 0)
                    continue; /* NIT */

                if (pat_table_find_program(pp_current_pat_sections, i_sid)
                      == NULL) {
                    int i_pmt;
                    for (i_pmt = 0; i_pmt < i_nb_sids; i_pmt++)
                        if (pp_sids[i_pmt]->i_sid == i_sid) {
                            pp_sids[i_pmt]->i_sid = 0;
                            free(pp_sids[i_pmt]->p_current_pmt);
                            pp_sids[i_pmt]->p_current_pmt = NULL;
                            break;
                        }
                }
            }
        }

        psi_table_free(pp_old_pat_sections);
    }
}

static void handle_pat_section(uint16_t i_pid, uint8_t *p_section)
{
    if (i_pid != PAT_PID || !pat_validate(p_section)) {
        free(p_section);
        return;
    }

    if (!psi_table_section(pp_next_pat_sections, p_section))
        return;

    handle_pat();
}

/*****************************************************************************
 * handle_pmt
 *****************************************************************************/
static void handle_pmt(uint16_t i_pid, uint8_t *p_pmt)
{
    uint16_t i_sid = pmt_get_program(p_pmt);
    sid_t *p_sid;
    int i;

    /* we do this before checking the service ID */
    if (!pmt_validate(p_pmt)) {
        free(p_pmt);
        return;
    }

    for (i = 0; i < i_nb_sids; i++)
        if (pp_sids[i]->i_sid && pp_sids[i]->i_sid == i_sid)
            break;

    if (i != i_nb_sids) {
        p_sid = pp_sids[i];
        if (i_pid == p_sid->i_pmt_pid) {
            printf("%"PRIu16"\n", pmt_get_pcrpid(p_pmt));
            exit(EXIT_SUCCESS);
        }
    }

    free(p_pmt);
}

/*****************************************************************************
 * handle_section
 *****************************************************************************/
static void handle_section(uint16_t i_pid, uint8_t *p_section)
{
    uint8_t i_table_id = psi_get_tableid(p_section);

    if (!psi_validate(p_section)) {
        free(p_section);
        return;
    }

    switch (i_table_id) {
    case PAT_TABLE_ID:
        handle_pat_section(i_pid, p_section);
        break;

    case PMT_TABLE_ID:
        handle_pmt(i_pid, p_section);
        break;

    default:
        free( p_section );
        break;
    }
}

/*****************************************************************************
 * handle_psi_packet
 *****************************************************************************/
static void handle_psi_packet(uint8_t *p_ts)
{
    uint16_t i_pid = ts_get_pid(p_ts);
    ts_pid_t *p_pid = &p_pids[i_pid];
    uint8_t i_cc = ts_get_cc(p_ts);
    const uint8_t *p_payload;
    uint8_t i_length;

    if (ts_check_duplicate(i_cc, p_pid->i_last_cc) || !ts_has_payload(p_ts))
        return;

    if (p_pid->i_last_cc != -1
          && ts_check_discontinuity(i_cc, p_pid->i_last_cc))
        psi_assemble_reset(&p_pid->p_psi_buffer, &p_pid->i_psi_buffer_used);

    p_payload = ts_section(p_ts);
    i_length = p_ts + TS_SIZE - p_payload;

    if (!psi_assemble_empty(&p_pid->p_psi_buffer, &p_pid->i_psi_buffer_used)) {
        uint8_t *p_section = psi_assemble_payload(&p_pid->p_psi_buffer,
                                                  &p_pid->i_psi_buffer_used,
                                                  &p_payload, &i_length);
        if (p_section != NULL)
            handle_section(i_pid, p_section);
    }

    p_payload = ts_next_section( p_ts );
    i_length = p_ts + TS_SIZE - p_payload;

    while (i_length) {
        uint8_t *p_section = psi_assemble_payload(&p_pid->p_psi_buffer,
                                                  &p_pid->i_psi_buffer_used,
                                                  &p_payload, &i_length);
        if (p_section != NULL)
            handle_section(i_pid, p_section);
    }
}

/*****************************************************************************
 * Main loop
 *****************************************************************************/
static void usage(const char *psz)
{
    fprintf(stderr, "usage: %s < <input file> [> <output>]\n", psz);
    exit(EXIT_FAILURE);
}

int main(int i_argc, char **ppsz_argv)
{
    int i;

    if (ppsz_argv[1] != NULL &&
        (!strcmp(ppsz_argv[1], "-h") || !strcmp(ppsz_argv[1], "--help")))
        usage(ppsz_argv[0]);

    setvbuf(stdout, NULL, _IOLBF, 0);

    memset(p_pids, 0, sizeof(p_pids));

    for (i = 0; i < 8192; i++) {
        p_pids[i].i_last_cc = -1;
        psi_assemble_init( &p_pids[i].p_psi_buffer,
                           &p_pids[i].i_psi_buffer_used );
    }

    p_pids[PAT_PID].i_psi_refcount++;

    while (!feof(stdin) && !ferror(stdin)) {
        uint8_t p_ts[TS_SIZE];
        size_t i_ret = fread(p_ts, sizeof(p_ts), 1, stdin);
        if (i_ret != 1) continue;
        if (ts_validate(p_ts)) {
            uint16_t i_pid = ts_get_pid(p_ts);
            ts_pid_t *p_pid = &p_pids[i_pid];
            if (p_pid->i_psi_refcount)
                handle_psi_packet(p_ts);
            p_pid->i_last_cc = ts_get_cc(p_ts);
        }
    }

    return EXIT_FAILURE;
}
