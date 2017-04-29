/*****************************************************************************
 * dvb_print_si.c: Prints tables from a TS file
 *****************************************************************************
 * Copyright (C) 2010-2015 VideoLAN
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
#include <unistd.h>
#include <stdarg.h>
#include <getopt.h>
#include <iconv.h>

#include <bitstream/mpeg/ts.h>
#include <bitstream/mpeg/psi.h>
#include <bitstream/dvb/si.h>
#include <bitstream/dvb/si_print.h>
#include <bitstream/mpeg/psi_print.h>
#include <bitstream/scte/35.h>
#include <bitstream/scte/35_print.h>

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

static ts_pid_t p_pids[MAX_PIDS];

typedef struct sid_t {
    uint16_t i_sid, i_pmt_pid;
    uint8_t *p_current_pmt;
    PSI_TABLE_DECLARE(pp_eit_sections);
} sid_t;

static sid_t **pp_sids = NULL;
static int i_nb_sids = 0;

typedef struct bouquet_t {
    uint16_t i_bouquet;
    PSI_TABLE_DECLARE(pp_current_bat_sections);
    PSI_TABLE_DECLARE(pp_next_bat_sections);
} bouquet_t;

static bouquet_t **pp_bouquets = NULL;
static int i_nb_bouquets = 0;

static PSI_TABLE_DECLARE(pp_current_pat_sections);
static PSI_TABLE_DECLARE(pp_next_pat_sections);
static PSI_TABLE_DECLARE(pp_current_cat_sections);
static PSI_TABLE_DECLARE(pp_next_cat_sections);
static PSI_TABLE_DECLARE(pp_current_tsdt_sections);
static PSI_TABLE_DECLARE(pp_next_tsdt_sections);
static PSI_TABLE_DECLARE(pp_current_nit_sections);
static PSI_TABLE_DECLARE(pp_next_nit_sections);
static PSI_TABLE_DECLARE(pp_current_sdt_sections);
static PSI_TABLE_DECLARE(pp_next_sdt_sections);

static const char *psz_native_encoding = "UTF-8";
static const char *psz_current_encoding = "";
static iconv_t iconv_handle = (iconv_t)-1;
static print_type_t i_print_type = PRINT_TEXT;

/* Please keep those two in sync */
enum tables_t {
    TABLE_PAT = 0,
    TABLE_CAT,
    TABLE_TSDT,
    TABLE_NIT,
    TABLE_BAT,
    TABLE_SDT,
    TABLE_EIT,
    TABLE_TOT,
    TABLE_TDT,
    TABLE_RST,
    TABLE_DIT,
    TABLE_SIT,
    TABLE_PMT,
    TABLE_SCTE35,
    TABLE_END
};
static const char * const ppsz_all_tables[TABLE_END] = {
    "pat", "cat", "tsdt", "nit", "bat", "sdt", "eit", "tot", "tdt", "rst",
    "dit", "sit", "pmt", "scte35"
};
static bool pb_print_table[TABLE_END];

static void handle_pmt_es(uint8_t *p_pmt, bool b_select);

/*****************************************************************************
 * print_wrapper
 *****************************************************************************/
static void print_wrapper(void *_unused, const char *psz_format, ...)
{
    char psz_fmt[strlen(psz_format) + 2];
    va_list args;
    va_start(args, psz_format);
    strcpy(psz_fmt, psz_format);
    strcat(psz_fmt, "\n");
    vprintf(psz_fmt, args);
    va_end(args);
}

/*****************************************************************************
 * iconv_wrapper
 *****************************************************************************/
static char *iconv_append_null(const char *p_string, size_t i_length)
{
    char *psz_string = malloc(i_length + 1);
    memcpy(psz_string, p_string, i_length);
    psz_string[i_length] = '\0';
    return psz_string;
}

static char *iconv_wrapper(void *_unused, const char *psz_encoding,
                           char *p_string, size_t i_length)
{
    char *psz_string, *p;
    size_t i_out_length;

    if (!strcmp(psz_encoding, psz_native_encoding))
        return iconv_append_null(p_string, i_length);

    if (iconv_handle != (iconv_t)-1 &&
        strcmp(psz_encoding, psz_current_encoding)) {
        iconv_close(iconv_handle);
        iconv_handle = (iconv_t)-1;
    }

    if (iconv_handle == (iconv_t)-1)
        iconv_handle = iconv_open(psz_native_encoding, psz_encoding);
    if (iconv_handle == (iconv_t)-1) {
        fprintf(stderr, "couldn't initiate conversion from %s to %s (%m)\n",
                psz_encoding, psz_native_encoding);
        return iconv_append_null(p_string, i_length);
    }
    psz_current_encoding = psz_encoding;

    /* converted strings can be up to six times larger */
    i_out_length = i_length * 6;
    p = psz_string = malloc(i_out_length);
    if (iconv(iconv_handle, &p_string, &i_length, &p, &i_out_length) == -1) {
        fprintf(stderr, "couldn't convert from %s to %s (%m)\n", psz_encoding,
                psz_native_encoding);
        free(psz_string);
        return iconv_append_null(p_string, i_length);
    }
    if (i_length)
        fprintf(stderr, "partial conversion from %s to %s\n", psz_encoding,
                psz_native_encoding);

    *p = '\0';
    return psz_string;
}

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
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_pat\"/>\n");
            break;
        default:
            printf("invalid PAT received\n");
        }
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

            if (i_sid == 0) {
                if (i_pid != NIT_PID)
                    fprintf(stderr,
                        "NIT is carried on PID %hu which isn't DVB compliant\n",
                        i_pid);
                continue; /* NIT */
            }

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
                    psi_table_init(p_sid->pp_eit_sections);
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
                            if (pp_sids[i_pmt]->p_current_pmt != NULL)
                                handle_pmt_es(pp_sids[i_pmt]->p_current_pmt,
                                              false);
                            free(pp_sids[i_pmt]->p_current_pmt);
                            pp_sids[i_pmt]->p_current_pmt = NULL;
                            psi_table_free(pp_sids[i]->pp_eit_sections);
                            psi_table_init(pp_sids[i]->pp_eit_sections);
                            break;
                        }
                }
            }
        }

        psi_table_free(pp_old_pat_sections);
    }

    if (pb_print_table[TABLE_PAT])
        pat_table_print(pp_current_pat_sections, print_wrapper, NULL,
                        i_print_type);
}

static void handle_pat_section(uint16_t i_pid, uint8_t *p_section)
{
    if (i_pid != PAT_PID || !pat_validate(p_section)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_pat_section\"/>\n");
            break;
        default:
            printf("invalid PAT section received on PID %hu\n", i_pid);
        }
        free(p_section);
        return;
    }

    if (!psi_table_section(pp_next_pat_sections, p_section))
        return;

    handle_pat();
}

/*****************************************************************************
 * handle_cat
 *****************************************************************************/
static void handle_cat(void)
{
    PSI_TABLE_DECLARE(pp_old_cat_sections);

    if (psi_table_validate(pp_current_cat_sections) &&
        psi_table_compare(pp_current_cat_sections, pp_next_cat_sections)) {
        /* Identical CAT. Shortcut. */
        psi_table_free(pp_next_cat_sections);
        psi_table_init(pp_next_cat_sections);
        return;
    }

    if (!cat_table_validate(pp_next_cat_sections)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_cat\"/>\n");
            break;
        default:
            printf("invalid CAT received\n");
        }
        psi_table_free(pp_next_cat_sections);
        psi_table_init(pp_next_cat_sections);
        return;
    }

    /* Switch tables. */
    psi_table_copy(pp_old_cat_sections, pp_current_cat_sections);
    psi_table_copy(pp_current_cat_sections, pp_next_cat_sections);
    psi_table_init(pp_next_cat_sections);

    if (psi_table_validate(pp_old_cat_sections))
        psi_table_free(pp_old_cat_sections);

    if (pb_print_table[TABLE_CAT])
        cat_table_print(pp_current_cat_sections, print_wrapper, NULL,
                        i_print_type);
}

static void handle_cat_section(uint16_t i_pid, uint8_t *p_section)
{
    if (i_pid != CAT_PID || !cat_validate(p_section)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_cat_section\"/>\n");
            break;
        default:
            printf("invalid CAT section received on PID %hu\n", i_pid);
        }
        free(p_section);
        return;
    }

    if (!psi_table_section(pp_next_cat_sections, p_section))
        return;

    handle_cat();
}

/*****************************************************************************
 * handle_tsdt
 *****************************************************************************/
static void handle_tsdt(void)
{
    PSI_TABLE_DECLARE(pp_old_tsdt_sections);

    if (psi_table_validate(pp_current_tsdt_sections) &&
        psi_table_compare(pp_current_tsdt_sections, pp_next_tsdt_sections)) {
        /* Identical TSDT. Shortcut. */
        psi_table_free(pp_next_tsdt_sections);
        psi_table_init(pp_next_tsdt_sections);
        return;
    }

    if (!tsdt_table_validate(pp_next_tsdt_sections)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_tsdt\"/>\n");
            break;
        default:
            printf("invalid TSDT received\n");
        }
        psi_table_free(pp_next_tsdt_sections);
        psi_table_init(pp_next_tsdt_sections);
        return;
    }

    /* Switch tables. */
    psi_table_copy(pp_old_tsdt_sections, pp_current_tsdt_sections);
    psi_table_copy(pp_current_tsdt_sections, pp_next_tsdt_sections);
    psi_table_init(pp_next_tsdt_sections);

    if (psi_table_validate(pp_old_tsdt_sections))
        psi_table_free(pp_old_tsdt_sections);

    if (pb_print_table[TABLE_TSDT])
        tsdt_table_print(pp_current_tsdt_sections, print_wrapper, NULL,
                         i_print_type);
}

static void handle_tsdt_section(uint16_t i_pid, uint8_t *p_section)
{
    if (i_pid != TSDT_PID || !tsdt_validate(p_section)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_tsdt_section\"/>\n");
            break;
        default:
            printf("invalid TSDT section received on PID %hu\n", i_pid);
        }
        free(p_section);
        return;
    }

    if (!psi_table_section(pp_next_tsdt_sections, p_section))
        return;

    handle_tsdt();
}

/*****************************************************************************
 * handle_pmt
 *****************************************************************************/
static void handle_pmt_es(uint8_t *p_pmt, bool b_select)
{
    int j = 0;
    uint8_t *p_es;
    while ((p_es = pmt_get_es(p_pmt, j)) != NULL) {
        uint16_t i_pid = pmtn_get_pid(p_es);
        uint8_t i_type = pmtn_get_streamtype(p_es);
        j++;

        switch (i_type) {
            case PMT_STREAMTYPE_SCTE_35:
                if (b_select)
                    p_pids[i_pid].i_psi_refcount++;
                else
                    p_pids[i_pid].i_psi_refcount--;
                break;
            default:
                break;
        }
    }
}

static void handle_pmt(uint16_t i_pid, uint8_t *p_pmt)
{
    uint16_t i_sid = pmt_get_program(p_pmt);
    sid_t *p_sid;
    int i;

    /* we do this before checking the service ID */
    if (!pmt_validate(p_pmt)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_pmt_section\" pid=\"%hu\"/>\n",
                   i_pid);
            break;
        default:
            printf("invalid PMT section received on PID %hu\n", i_pid);
        }
        free(p_pmt);
        return;
    }

    for (i = 0; i < i_nb_sids; i++)
        if (pp_sids[i]->i_sid && pp_sids[i]->i_sid == i_sid)
            break;

    if (i == i_nb_sids) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"ghost_pmt\" program=\"%hu\" pid=\"%hu\"/>\n",
                   i_sid, i_pid);
            break;
        default:
            printf("ghost PMT for service %hu carried on PID %hu\n", i_sid,
                   i_pid);
        }
        p_sid = malloc(sizeof(sid_t));
        pp_sids = realloc(pp_sids, ++i_nb_sids * sizeof(sid_t *));
        pp_sids[i] = p_sid;
        p_sid->i_sid = i_sid;
        p_sid->i_pmt_pid = i_pid;
        p_sid->p_current_pmt = NULL;
        psi_table_init(p_sid->pp_eit_sections);
    } else {
        p_sid = pp_sids[i];
        if (i_pid != p_sid->i_pmt_pid) {
            switch (i_print_type) {
            case PRINT_XML:
                printf("<ERROR type=\"ghost_pmt\" program=\"%hu\" pid=\"%hu\"/>\n",
                       i_sid, i_pid);
                break;
            default:
                printf("ghost PMT for service %hu carried on PID %hu\n", i_sid,
                       i_pid);
            }
        }
    }

    if (p_sid->p_current_pmt != NULL &&
        psi_compare(p_sid->p_current_pmt, p_pmt)) {
        /* Identical PMT. Shortcut. */
        free(p_pmt);
        return;
    }

    if (p_sid->p_current_pmt)
        handle_pmt_es(p_sid->p_current_pmt, false);

    free(p_sid->p_current_pmt);
    p_sid->p_current_pmt = p_pmt;
    handle_pmt_es(p_pmt, true);

    if (pb_print_table[TABLE_PMT])
        pmt_print(p_pmt, print_wrapper, NULL, iconv_wrapper, NULL,
                  i_print_type);
}

/*****************************************************************************
 * handle_nit
 *****************************************************************************/
static void handle_nit(void)
{
    if (psi_table_validate(pp_current_nit_sections) &&
        psi_table_compare(pp_current_nit_sections, pp_next_nit_sections)) {
        /* Same version NIT. Shortcut. */
        psi_table_free(pp_next_nit_sections);
        psi_table_init(pp_next_nit_sections);
        return;
    }

    if (!nit_table_validate(pp_next_nit_sections)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_nit\"/>\n");
            break;
        default:
            printf("invalid NIT received\n");
        }
        psi_table_free(pp_next_nit_sections);
        psi_table_init(pp_next_nit_sections);
        return;
    }

    /* Switch tables. */
    psi_table_free(pp_current_nit_sections);
    psi_table_copy(pp_current_nit_sections, pp_next_nit_sections);
    psi_table_init(pp_next_nit_sections);

    if (pb_print_table[TABLE_NIT])
        nit_table_print(pp_current_nit_sections, print_wrapper, NULL,
                        iconv_wrapper, NULL, i_print_type);
}

static void handle_nit_section(uint16_t i_pid, uint8_t *p_section)
{
    if (i_pid != NIT_PID || !nit_validate(p_section)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_nit_section\" pid=\"%hu\"/>\n",
                   i_pid);
            break;
        default:
            printf("invalid NIT section received on PID %hu\n", i_pid);
        }
        free(p_section);
        return;
    }

    if (!psi_table_section(pp_next_nit_sections, p_section))
        return;

    handle_nit();
}

/*****************************************************************************
 * handle_bat
 *****************************************************************************/
static void handle_bat(bouquet_t *p_bouquet)
{
    if (psi_table_validate(p_bouquet->pp_current_bat_sections) &&
        psi_table_compare(p_bouquet->pp_current_bat_sections,
                          p_bouquet->pp_next_bat_sections)) {
        /* Same version BAT. Shortcut. */
        psi_table_free(p_bouquet->pp_next_bat_sections);
        psi_table_init(p_bouquet->pp_next_bat_sections);
        return;
    }

    if (!bat_table_validate(p_bouquet->pp_next_bat_sections)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_bat\"/>\n");
            break;
        default:
            printf("invalid BAT received\n");
        }
        psi_table_free(p_bouquet->pp_next_bat_sections);
        psi_table_init(p_bouquet->pp_next_bat_sections);
        return;
    }

    /* Switch tables. */
    psi_table_free(p_bouquet->pp_current_bat_sections);
    psi_table_copy(p_bouquet->pp_current_bat_sections,
                   p_bouquet->pp_next_bat_sections);
    psi_table_init(p_bouquet->pp_next_bat_sections);

    if (pb_print_table[TABLE_BAT])
        bat_table_print(p_bouquet->pp_current_bat_sections, print_wrapper, NULL,
                        iconv_wrapper, NULL, i_print_type);
}

static void handle_bat_section(uint16_t i_pid, uint8_t *p_section)
{
    uint16_t i_bouquet;
    bouquet_t *p_bouquet;
    int i;

    if (i_pid != BAT_PID || !bat_validate(p_section)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_bat_section\" pid=\"%hu\"/>\n",
                   i_pid);
            break;
        default:
            printf("invalid BAT section received on PID %hu\n", i_pid);
        }
        free(p_section);
        return;
    }

    i_bouquet = psi_get_tableidext(p_section);
    for (i = 0; i < i_nb_bouquets; i++)
        if (pp_bouquets[i]->i_bouquet && pp_bouquets[i]->i_bouquet == i_bouquet)
            break;
    if (i == i_nb_bouquets) {
        p_bouquet = malloc(sizeof(bouquet_t));
        pp_bouquets = realloc(pp_bouquets, ++i_nb_bouquets * sizeof(bouquet_t *));
        pp_bouquets[i] = p_bouquet;
        p_bouquet->i_bouquet = i_bouquet;
        psi_table_init(p_bouquet->pp_current_bat_sections);
        psi_table_init(p_bouquet->pp_next_bat_sections);
    } else
        p_bouquet = pp_bouquets[i];

    if (!psi_table_section(p_bouquet->pp_next_bat_sections, p_section))
        return;

    handle_bat(p_bouquet);
}

/*****************************************************************************
 * handle_sdt
 *****************************************************************************/
static void handle_sdt(void)
{
    if (psi_table_validate(pp_current_sdt_sections) &&
        psi_table_compare(pp_current_sdt_sections, pp_next_sdt_sections)) {
        /* Identical SDT. Shortcut. */
        psi_table_free(pp_next_sdt_sections);
        psi_table_init(pp_next_sdt_sections);
        return;
    }

    if (!sdt_table_validate(pp_next_sdt_sections)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_sdt\"/>\n");
            break;
        default:
            printf("invalid SDT received\n");
        }
        psi_table_free(pp_next_sdt_sections);
        psi_table_init(pp_next_sdt_sections);
        return;
    }

    /* Switch tables. */
    psi_table_free(pp_current_sdt_sections);
    psi_table_copy(pp_current_sdt_sections, pp_next_sdt_sections);
    psi_table_init(pp_next_sdt_sections);

    if (pb_print_table[TABLE_SDT])
        sdt_table_print(pp_current_sdt_sections, print_wrapper, NULL,
                        iconv_wrapper, NULL, i_print_type);
}

static void handle_sdt_section(uint16_t i_pid, uint8_t *p_section)
{
    if (i_pid != SDT_PID || !sdt_validate(p_section)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_sdt_section\" pid=\"%hu\"/>\n",
                   i_pid);
            break;
        default:
            printf("invalid SDT section received on PID %hu\n", i_pid);
        }
        free(p_section);
        return;
    }

    if (!psi_table_section(pp_next_sdt_sections, p_section))
        return;

    handle_sdt();
}

/*****************************************************************************
 * handle_eit
 *****************************************************************************/
static void handle_eit_section(uint16_t i_pid, uint8_t *p_section)
{
    uint16_t i_sid;
    uint8_t i_section;
    sid_t *p_sid;
    int i;

    if (i_pid != EIT_PID || !eit_validate(p_section)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_eit_section\" pid=\"%hu\"/>\n",
                   i_pid);
            break;
        default:
            printf("invalid EIT section received on PID %hu\n", i_pid);
        }
        free(p_section);
        return;
    }

    i_sid = psi_get_tableidext(p_section);
    for (i = 0; i < i_nb_sids; i++)
        if (pp_sids[i]->i_sid && pp_sids[i]->i_sid == i_sid)
            break;
    if (i == i_nb_sids) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"ghost_eit\" sid=\"%hu\"/>\n", i_sid);
            break;
        default:
            printf("ghost EIT for service %hu\n", i_sid);
        }
        p_sid = malloc(sizeof(sid_t));
        pp_sids = realloc(pp_sids, ++i_nb_sids * sizeof(sid_t *));
        pp_sids[i] = p_sid;
        p_sid->i_sid = i_sid;
        p_sid->i_pmt_pid = 0;
        p_sid->p_current_pmt = NULL;
        psi_table_init(p_sid->pp_eit_sections);
    } else
        p_sid = pp_sids[i];

    /* We do not use psi_table_* primitives as the spec allows for holes in
     * section numbering, and there is no sure way to know whether you have
     * gathered all sections. */
    i_section = psi_get_section(p_section);
    if (p_sid->pp_eit_sections[i_section] != NULL &&
        psi_compare(p_sid->pp_eit_sections[i_section], p_section)) {
        /* Identical section. Shortcut. */
        free(p_section);
        return;
    }

    free(p_sid->pp_eit_sections[i_section]);
    p_sid->pp_eit_sections[i_section] = p_section;

    if (pb_print_table[TABLE_EIT])
        eit_print(p_section, print_wrapper, NULL, iconv_wrapper, NULL,
                  i_print_type);
}

/*****************************************************************************
 * handle_tdt
 *****************************************************************************/
static void handle_tdt_section(uint16_t i_pid, uint8_t *p_tdt)
{
    if (i_pid != TDT_PID || !tdt_validate(p_tdt)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_tdt_section\" pid=\"%hu\"/>\n",
                   i_pid);
            break;
        default:
            printf("invalid TDT section received on PID %hu\n", i_pid);
        }
        free(p_tdt);
        return;
    }

    if (pb_print_table[TABLE_TDT])
        tdt_print(p_tdt, print_wrapper, NULL, iconv_wrapper, NULL,
                  i_print_type);

    free(p_tdt);
}

/*****************************************************************************
 * handle_tot
 *****************************************************************************/
static void handle_tot_section(uint16_t i_pid, uint8_t *p_tot)
{
    if (i_pid != TOT_PID || !tot_validate(p_tot)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_tot_section\" pid=\"%hu\"/>\n",
                   i_pid);
            break;
        default:
            printf("invalid TOT section received on PID %hu\n", i_pid);
        }
        free(p_tot);
        return;
    }

    if (pb_print_table[TABLE_TOT])
        tot_print(p_tot, print_wrapper, NULL, iconv_wrapper, NULL,
                  i_print_type);

    free(p_tot);
}

/*****************************************************************************
 * handle_dit
 *****************************************************************************/
static void handle_dit_section(uint16_t i_pid, uint8_t *p_dit)
{
    if (i_pid != DIT_PID || !dit_validate(p_dit)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_dit_section\" pid=\"%hu\"/>\n",
                   i_pid);
            break;
        default:
            printf("invalid DIT section received on PID %hu\n", i_pid);
        }
        free(p_dit);
        return;
    }

    if (pb_print_table[TABLE_DIT])
        dit_print(p_dit, print_wrapper, NULL, iconv_wrapper, NULL,
                  i_print_type);

    free(p_dit);
}

/*****************************************************************************
 * handle_rst
 *****************************************************************************/
static void handle_rst_section(uint16_t i_pid, uint8_t *p_rst)
{
    if (i_pid != RST_PID || !rst_validate(p_rst)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_rst_section\" pid=\"%hu\"/>\n",
                   i_pid);
            break;
        default:
            printf("invalid RST section received on PID %hu\n", i_pid);
        }
        free(p_rst);
        return;
    }

    if (pb_print_table[TABLE_RST])
        rst_print(p_rst, print_wrapper, NULL, iconv_wrapper, NULL,
                  i_print_type);

    free(p_rst);
}

/*****************************************************************************
 * handle_sit
 *****************************************************************************/
static void handle_sit_section(uint16_t i_pid, uint8_t *p_sit)
{
    if (i_pid != SIT_PID || !sit_validate(p_sit)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_sit_section\" pid=\"%hu\"/>\n",
                   i_pid);
            break;
        default:
            printf("invalid SIT section received on PID %hu\n", i_pid);
        }
        free(p_sit);
        return;
    }

    if (pb_print_table[TABLE_SIT])
        sit_print(p_sit, print_wrapper, NULL, iconv_wrapper, NULL,
                  i_print_type);

    free(p_sit);
}

/*****************************************************************************
 * handle_scte35
 *****************************************************************************/
static void handle_scte35_section(uint16_t i_pid, uint8_t *p_scte35)
{
    if (!scte35_validate(p_scte35)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_scte35_section\" pid=\"%hu\"/>\n",
                   i_pid);
            break;
        default:
            printf("invalid SCTE35 section received on PID %hu\n", i_pid);
        }
        free(p_scte35);
        return;
    }

    if (pb_print_table[TABLE_SCTE35] &&
        scte35_get_command_type(p_scte35) != SCTE35_NULL_COMMAND)
        scte35_print(p_scte35, print_wrapper, NULL, i_print_type);

    free(p_scte35);
}

/*****************************************************************************
 * handle_section
 *****************************************************************************/
static void handle_section(uint16_t i_pid, uint8_t *p_section)
{
    uint8_t i_table_id = psi_get_tableid(p_section);

    if (!psi_validate(p_section)) {
        switch (i_print_type) {
        case PRINT_XML:
            printf("<ERROR type=\"invalid_section\" pid=\"%hu\"/>\n", i_pid);
            break;
        default:
            printf("invalid section on PID %hu\n", i_pid);
        }
        free(p_section);
        return;
    }

    switch (i_table_id) {
    case PAT_TABLE_ID:
        handle_pat_section(i_pid, p_section);
        break;

    case CAT_TABLE_ID:
        handle_cat_section(i_pid, p_section);
        break;

    case TSDT_TABLE_ID:
        handle_tsdt_section(i_pid, p_section);
        break;

    case PMT_TABLE_ID:
        handle_pmt(i_pid, p_section);
        break;

    case NIT_TABLE_ID_ACTUAL:
        handle_nit_section(i_pid, p_section);
        break;

    case BAT_TABLE_ID:
        handle_bat_section(i_pid, p_section);
        break;

    case SDT_TABLE_ID_ACTUAL:
        handle_sdt_section(i_pid, p_section);
        break;

    case TDT_TABLE_ID:
        handle_tdt_section(i_pid, p_section);
        break;

    case TOT_TABLE_ID:
        handle_tot_section(i_pid, p_section);
        break;

    case RST_TABLE_ID:
        handle_rst_section(i_pid, p_section);
        break;

    case DIT_TABLE_ID:
        handle_dit_section(i_pid, p_section);
        break;

    case SIT_TABLE_ID:
        handle_sit_section(i_pid, p_section);
        break;

    case EIT_TABLE_ID_PF_ACTUAL:
        handle_eit_section(i_pid, p_section);
        break;

    case SCTE35_TABLE_ID:
        handle_scte35_section(i_pid, p_section);
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
    fprintf(stderr, "usage: %s [-x xml] [-T <tables>] < <input file> [> <output>]\n", psz);
    exit(EXIT_FAILURE);
}

int main(int i_argc, char **ppsz_argv)
{
    int i, c;
    char *psz_tables = NULL;

    static const struct option long_options[] = {
        { "print",           required_argument, NULL, 'x' },
        { "help",            no_argument,       NULL, 'h' },
        { "version",         no_argument,       NULL, 'V' },
        { "tables",          no_argument,       NULL, 'T' },
        { 0, 0, 0, 0 }
    };

    while ((c = getopt_long(i_argc, ppsz_argv, "x:hVT:", long_options, NULL)) != -1)
    {
        switch (c) {
        case 'x':
            if (!strcmp(optarg, "text"))
                i_print_type = PRINT_TEXT;
            else if (!strcmp(optarg, "xml"))
                i_print_type = PRINT_XML;
            else
                fprintf(stderr, "unrecognized print type %s\n", optarg);
            break;

        case 'T':
            psz_tables = strdup(optarg);
            break;

        case 'V':
            fprintf(stderr, "biTStream %d.%d.%d\n", BITSTREAM_VERSION_MAJOR,
                    BITSTREAM_VERSION_MINOR, BITSTREAM_VERSION_REVISION);
            exit(0);
            break;


        case 'h':
        default:
            usage(ppsz_argv[0]);
        }
    }
    if (optind < i_argc)
        usage(ppsz_argv[0]);

    setvbuf(stdout, NULL, _IOLBF, 0);

    memset(p_pids, 0, sizeof(p_pids));

    for (i = 0; i < 8192; i++) {
        p_pids[i].i_last_cc = -1;
        psi_assemble_init(&p_pids[i].p_psi_buffer,
                          &p_pids[i].i_psi_buffer_used);
    }

    p_pids[PAT_PID].i_psi_refcount++;
    p_pids[CAT_PID].i_psi_refcount++;
    p_pids[TSDT_PID].i_psi_refcount++;
    p_pids[NIT_PID].i_psi_refcount++;
    p_pids[BAT_PID].i_psi_refcount++;
    p_pids[SDT_PID].i_psi_refcount++;
    p_pids[EIT_PID].i_psi_refcount++;
    p_pids[TDT_PID].i_psi_refcount++;
    p_pids[RST_PID].i_psi_refcount++;
    p_pids[DIT_PID].i_psi_refcount++;
    p_pids[SIT_PID].i_psi_refcount++;

    psi_table_init(pp_current_pat_sections);
    psi_table_init(pp_next_pat_sections);
    psi_table_init(pp_current_cat_sections);
    psi_table_init(pp_next_cat_sections);
    psi_table_init(pp_current_tsdt_sections);
    psi_table_init(pp_next_tsdt_sections);
    psi_table_init(pp_current_nit_sections);
    psi_table_init(pp_next_nit_sections);
    psi_table_init(pp_current_sdt_sections);
    psi_table_init(pp_next_sdt_sections);

    if (psz_tables != NULL) {
        char *psz_table = psz_tables;
        for (i = 0; i < TABLE_END; i++)
            pb_print_table[i] = false;

        do {
            char *psz_next = strpbrk(psz_table, ",");
            if (psz_next != NULL) *psz_next++ = '\0';

            for (i = 0; i < TABLE_END; i++)
                if (!strcasecmp(psz_table, ppsz_all_tables[i]))
                    pb_print_table[i] = true;

            psz_table = psz_next;
        } while (psz_table != NULL);

        free(psz_tables);
    } else
        for (i = 0; i < TABLE_END; i++)
            pb_print_table[i] = true;

    switch (i_print_type) {
    case PRINT_XML:
        printf("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
        printf("<TS>\n");
        break;
    default:
        break;
    }

    bool b_is_last_invalid = false;
    while (!feof(stdin) && !ferror(stdin)) {
        uint8_t p_ts[TS_SIZE];
        size_t i_ret = fread(p_ts, sizeof(p_ts), 1, stdin);
        if (i_ret != 1) continue;
        if (!ts_validate(p_ts)) {
            if (!b_is_last_invalid) {
                switch (i_print_type) {
                case PRINT_XML:
                    printf("<ERROR type=\"invalid_ts\"/>\n");
                    break;
                default:
                    printf("invalid TS packet\n");
                }
                b_is_last_invalid = true;
            }

            int i;
            for (i = 1; i < TS_SIZE; i++) {
                if (ts_validate(p_ts + i)) {
                    memmove(p_ts, p_ts + i, TS_SIZE - i);
                    i_ret = fread(p_ts + TS_SIZE - i, i, 1, stdin);
                    if (i_ret != 1) continue;
                    break;
                }
            }
            if (i == TS_SIZE)
                continue;
        }

        uint16_t i_pid = ts_get_pid(p_ts);
        ts_pid_t *p_pid = &p_pids[i_pid];
        if (p_pid->i_psi_refcount)
            handle_psi_packet(p_ts);
        p_pid->i_last_cc = ts_get_cc(p_ts);
        b_is_last_invalid = false;
    }

    switch (i_print_type) {
    case PRINT_XML:
        printf("</TS>\n");
        break;
    default:
        break;
    }

    if (iconv_handle != (iconv_t)-1)
        iconv_close(iconv_handle);

    psi_table_free(pp_current_pat_sections);
    psi_table_free(pp_next_pat_sections);
    psi_table_free(pp_current_cat_sections);
    psi_table_free(pp_next_cat_sections);
    psi_table_free(pp_current_tsdt_sections);
    psi_table_free(pp_next_tsdt_sections);
    psi_table_free(pp_current_nit_sections);
    psi_table_free(pp_next_nit_sections);
    psi_table_free(pp_current_sdt_sections);
    psi_table_free(pp_next_sdt_sections);

    for (i = 0; i < i_nb_bouquets; i++) {
        psi_table_free(pp_bouquets[i]->pp_current_bat_sections);
        psi_table_free(pp_bouquets[i]->pp_next_bat_sections);
        free(pp_bouquets[i]);
    }
    free(pp_bouquets);

    for (i = 0; i < i_nb_sids; i++) {
        psi_table_free(pp_sids[i]->pp_eit_sections);
        free(pp_sids[i]->p_current_pmt);
        free(pp_sids[i]);
    }
    free(pp_sids);

    for (i = 0; i < 8192; i++)
        psi_assemble_reset(&p_pids[i].p_psi_buffer,
                           &p_pids[i].i_psi_buffer_used);

    return EXIT_SUCCESS;
}
