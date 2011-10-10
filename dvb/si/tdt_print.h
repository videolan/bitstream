/*****************************************************************************
 * tdt_print.h: ETSI EN 300 468 Time and Date Table (TDT) (printing)
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

#ifndef __BITSTREAM_DVB_TDT_PRINT_H__
#define __BITSTREAM_DVB_TDT_PRINT_H__

#include <bitstream/common.h>
#include <bitstream/dvb/si/datetime.h>
#include <bitstream/dvb/si/tdt.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Time and Date Table
 *****************************************************************************/
static inline void tdt_print(uint8_t *p_tdt,
                             f_print pf_print, void *print_opaque,
                             f_iconv pf_iconv, void *iconv_opaque,
                             print_type_t i_print_type)
{
    time_t ts;
    char ts_str[24];

    ts = dvb_time_format_UTC(tdt_get_utc(p_tdt), NULL, ts_str);

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque, "<TDT time=\"%ld\" time_dec=\"%s\"/>",
                 ts, ts_str);
        break;
    default:
        pf_print(print_opaque, "new TDT time=%ld time_dec=\"%s\"",
                 ts, ts_str);
        pf_print(print_opaque, "end TDT");
    }
}

#ifdef __cplusplus
}
#endif

#endif
