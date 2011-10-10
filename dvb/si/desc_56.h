/*****************************************************************************
 * desc_56.h: ETSI EN 300 468 Descriptor 0x56: Teletext descriptor
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
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

/*
 * Normative references:
 *  - ETSI EN 300 468 V1.11.1 (2010-04) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_DESC_56_H__
#define __BITSTREAM_DVB_DESC_56_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/dvb/si/desc_46.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x56: Teletext descriptor
 *****************************************************************************/
#define DESC56_HEADER_SIZE      DESC46_HEADER_SIZE
#define DESC56_LANGUAGE_SIZE    DESC46_LANGUAGE_SIZE

static inline void desc56_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x56);
}

#define desc56_get_language desc46_get_language
#define desc56n_set_code desc46n_set_code
#define desc56n_get_code desc46n_get_code
#define desc56n_set_teletexttype desc46n_set_teletexttype
#define desc56n_get_teletexttype desc46n_get_teletexttype
#define desc56n_set_teletextmagazine desc46n_set_teletextmagazine
#define desc56n_get_teletextmagazine desc46n_get_teletextmagazine
#define desc56n_set_teletextpage desc46n_set_teletextpage
#define desc56n_get_teletextpage desc46n_get_teletextpage
#define desc56_validate desc46_validate
#define desc56_print desc46_print

#ifdef __cplusplus
}
#endif

#endif
