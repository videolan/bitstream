/*****************************************************************************
 * desc_88p28.h: ETSI EN 300 468 Descriptor 0x88: HD simulcast logical channel
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
 *  - IEC/CENELEC 62 216
 */

#ifndef __BITSTREAM_DVB_DESC_88P28_H__
#define __BITSTREAM_DVB_DESC_88P28_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/dvb/si/desc_83p28.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x88: HD simulcast logical channel descriptor (IEC/CENELEC 62 216)
 * Only valid if a private data specifier descriptor 28 is present before.
 *****************************************************************************/
#define DESC88P28_HEADER_SIZE      DESC83P28_HEADER_SIZE
#define DESC88P28_SERVICE_SIZE     DESC83P28_SERVICE_SIZE

static inline void desc88p28_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x88);
}

#define desc88p28_get_service desc83p28_get_service
#define desc88p28_get_sid desc83p28_get_sid
#define desc88p28_get_visible desc83p28_get_visible
#define desc88p28_get_lcn desc83p28_get_lcn
#define desc88p28_validate desc83p28_validate
#define desc88p28_print desc83p28_print

#ifdef __cplusplus
}
#endif

#endif
