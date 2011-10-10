/*****************************************************************************
 * desc_5c.h: ETSI EN 300 468 Descriptor 0x5c: Multilingual bouquet name
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *          Georgi Chorbadzhiyski <georgi@unixsol.org>
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

#ifndef __BITSTREAM_DVB_DESC_5C_H__
#define __BITSTREAM_DVB_DESC_5C_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/mpeg/psi/desc_0a.h>
#include <bitstream/dvb/si/desc_5c.h>
#include <bitstream/dvb/si/strings.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x5c: Multilingual bouquet name
 *****************************************************************************/
#define DESC5C_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC5C_DATA_SIZE        4

static inline void desc5c_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x5c);
}

#define desc5cn_set_code                desc0an_set_code
#define desc5cn_get_code                desc0an_get_code
#define desc5cn_get bouquetname_length  desc5bn_get_networkname_length
#define desc5cn_get_bouquetname         desc5bn_get_networkname
#define desc5cn_set_bouquetname         desc5bn_set_networkname
#define desc5c_get_data                 desc5b_get_data
#define desc5c_validate                 desc5b_validate
#define desc5c_print                    desc5b_print

#ifdef __cplusplus
}
#endif

#endif
