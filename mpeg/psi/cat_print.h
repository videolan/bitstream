/*****************************************************************************
 * cat_print.h: ISO/IEC 13818-1 Conditional Access Table (printing)
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

#ifndef __BITSTREAM_MPEG_CAT_PRINT_H__
#define __BITSTREAM_MPEG_CAT_PRINT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/mpeg/psi/descs_print.h>
#include <bitstream/mpeg/psi/cat.h>
#include <bitstream/mpeg/psi/tsdt.h>

#ifdef __cplusplus
extern "C"
{
#endif

static inline void cat_table_print(uint8_t **pp_sections, f_print pf_print,
                                   void *opaque, print_type_t i_print_type)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;
    char *psz_table_name = "CAT";

    if (psi_get_tableid(psi_table_get_section(pp_sections, 0)) == TSDT_TABLE_ID)
        psz_table_name = "TSDT";

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<%s version=\"%hhu\" current_next=\"%d\">",
                 psz_table_name,
                 psi_table_get_version(pp_sections),
                 !psi_table_get_current(pp_sections) ? 0 : 1);
        break;
    default:
        pf_print(opaque, "new %s version=%hhu%s",
                 psz_table_name,
                 psi_table_get_version(pp_sections),
                 !psi_table_get_current(pp_sections) ? " (next)" : "");
    }

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);

        descl_print(cat_get_descl(p_section), cat_get_desclength(p_section),
                    pf_print, opaque, NULL, NULL, i_print_type);
    }

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "</%s>", psz_table_name);
        break;
    default:
        pf_print(opaque, "end %s", psz_table_name);
    }
}

#ifdef __cplusplus
}
#endif

#endif
