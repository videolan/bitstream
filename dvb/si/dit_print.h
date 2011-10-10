/*****************************************************************************
 * dit_print.h: ETSI EN 300 468 Discontinuity Information Table (DIT) (printing)
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

#ifndef __BITSTREAM_DVB_DIT_PRINT_H__
#define __BITSTREAM_DVB_DIT_PRINT_H__

#include <bitstream/common.h>
#include <bitstream/dvb/si/dit.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Discontinuity Information Table
 *****************************************************************************/
static inline void dit_print(uint8_t *p_dit,
                             f_print pf_print, void *print_opaque,
                             f_iconv pf_iconv, void *iconv_opaque,
                             print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque, "<DIT transition_flag=\"%u\"/>",
                 dit_get_transition_flag(p_dit));
        break;
    default:
        pf_print(print_opaque, "new DIT transition_flag=%u",
                 dit_get_transition_flag(p_dit));
        pf_print(print_opaque, "end DIT");
    }
}

#ifdef __cplusplus
}
#endif

#endif
