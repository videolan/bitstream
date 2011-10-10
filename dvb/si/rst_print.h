/*****************************************************************************
 * rst_print.h: ETSI EN 300 468 Running Status Table (RST) (printing)
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

#ifndef __BITSTREAM_DVB_RST_PRINT_H__
#define __BITSTREAM_DVB_RST_PRINT_H__

#include <bitstream/common.h>
#include <bitstream/dvb/si/rst.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Running Status Table
 *****************************************************************************/
static inline void rst_print(uint8_t *p_rst,
                             f_print pf_print, void *print_opaque,
                             f_iconv pf_iconv, void *iconv_opaque,
                             print_type_t i_print_type)
{
    uint8_t j = 0;
    uint8_t *p_rst_n;

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque, "<RST>");
        break;
    default:
        pf_print(print_opaque, "new RST");
    }

    while ((p_rst_n = rst_get_status(p_rst, j++)) != NULL) {
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(print_opaque,
                     "<STATUS tsid=\"%hu\" onid=\"%hu\" service_id=\"%hu\""
                     " event_id=\"%hu\" running_status=\"%hu\"/>",
                     rstn_get_tsid(p_rst_n),
                     rstn_get_onid(p_rst_n),
                     rstn_get_service_id(p_rst_n),
                     rstn_get_event_id(p_rst_n),
                     rstn_get_running_status(p_rst_n)
                    );
            break;
        default:
            pf_print(print_opaque,
                     "  * status tsid=\"%hu\" onid=\"%hu\" service_id=\"%hu\""
                     " event_id=\"%hu\" running_status=\"%hu\"",
                     rstn_get_tsid(p_rst_n),
                     rstn_get_onid(p_rst_n),
                     rstn_get_service_id(p_rst_n),
                     rstn_get_event_id(p_rst_n),
                     rstn_get_running_status(p_rst_n)
                    );
        }
    }

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque, "</RST>");
        break;
    default:
        pf_print(print_opaque, "end RST");
    }
}

#ifdef __cplusplus
}
#endif

#endif
