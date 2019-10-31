/*****************************************************************************
 * mpeg_print_au_info.c: Prints adaptation field AU_INFO
 *****************************************************************************
 * Copyright (C) 2019 VideoLAN
 *
 * Authors: Roberto Corno <corno.roberto@gmail.com>
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
#include <bitstream/mpeg/au_info.h>

#include <fcntl.h>
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

    uint64_t pnum = 0;
    while (!feof(stdin) && !ferror(stdin)) {
        uint8_t p_ts[TS_SIZE];
        size_t i_ret = fread(p_ts, sizeof(p_ts), 1, stdin);
        if (i_ret != 1) continue;
        if (ts_validate(p_ts)) {
            if (ts_has_adaptation(p_ts)) {
                if (tsaf_has_transport_private_data(p_ts)) {
                    uint8_t tag = tsaf_get_private_data_tag(p_ts);
                    uint8_t len = tsaf_get_private_data_length(p_ts);
                    fprintf(stderr, "packet %lu ts private data [len %d] found [%d]\n", pnum, len, tag);
                    if (tag == 0x02) {                        
                        uint8_t *p_au = ts_private_data(p_ts);
                        p_au++;
                        uint8_t df_len = au_info_get_data_field_length(p_au);
                        fprintf(stderr, "  AU_information len %d\n", df_len);
                        uint8_t code_fmt = au_info_get_au_coding_format(p_au);
                        uint8_t ctype_info = au_info_get_au_coding_type_info(p_au);
                        uint8_t ref_pic_icd = au_info_get_au_ref_pic_idc(p_au);
                        uint8_t pic_struct = au_info_get_au_pic_struct(p_au);
                        bool pts = au_info_get_pts_present(p_au);
                        bool prof_info = au_info_get_profile_info_present(p_au);
                        bool stream_info = au_info_get_stream_info_present(p_au);
                        fprintf(stderr, "  AU_coding_format:%d  AU_coding_type_info:%d\n",code_fmt, ctype_info);
                        fprintf(stderr, "  AU_ref_pic_idc:%d  AU_pic_struct:%d\n",ref_pic_icd, pic_struct);
                        fprintf(stderr, "  PTS present:%d ProfileInfo present:%d Stream Info present:%d\n",pts, prof_info, stream_info);
                        if (pts) {
                            uint32_t pts32 = au_info_get_pts(p_au);
                            fprintf(stderr, "  PTS :%u\n",pts32);
                        }
                    }
                    exit(0);
                }
            }
            uint16_t i_pid = ts_get_pid(p_ts);
            ts_pid_t *p_pid = &p_pids[i_pid];
            p_pid->i_last_cc = ts_get_cc(p_ts);
            pnum++;
        }
    }

    return EXIT_FAILURE;
}
