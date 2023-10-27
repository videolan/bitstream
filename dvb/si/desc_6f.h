/*****************************************************************************
 * desc_6f.h: ETSI EN 300 468 Descriptor 0x7f: Application signalling
 * descriptor
 *****************************************************************************
 * Copyright (C) 2022 EasyTools
 *
 * Authors: Arnaud de Turckheim <adeturckheim@easytools.tv>
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
 * ETSI TS 102 809 V1.3.1 (2017-06)
 */

#ifndef __BITSTREAM_DVB_DESC_6F_H__
#define __BITSTREAM_DVB_DESC_6F_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x6f: Application signalling
 *****************************************************************************/
#define DESC6F_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC6F_APPLICATION_SIZE 3

static inline void desc6f_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x6f);
    desc_set_length(p_desc, (DESC6F_HEADER_SIZE - DESC_HEADER_SIZE));
}

static inline uint8_t *desc6f_next_application(const uint8_t *p_desc,
                                               const uint8_t *p_desc_n)
{
    if (!p_desc_n)
        p_desc_n = p_desc + DESC6F_HEADER_SIZE;
    else
        p_desc_n += DESC6F_APPLICATION_SIZE;
    return desc_check(p_desc, p_desc_n, DESC6F_APPLICATION_SIZE);
}

#define desc6f_each_application(DESC, DESC_N) \
    desc_each(DESC, DESC_N, desc6f_next_application)
#define desc6f_get_application(DESC, N) \
    desc_get_at(DESC, N, desc6f_next_application)

BITSTREAM_GET_SET(desc6f_app, type, uint16, 1, 15);
BITSTREAM_GET_SET(desc6f_app, AIT_version_number, uint8, 19, 5);

static inline bool desc6f_validate(const uint8_t *p_desc)
{
    uint8_t desc_length = desc_get_length(p_desc);
    uint8_t size = DESC6F_HEADER_SIZE - DESC_HEADER_SIZE;
    desc6f_each_application(p_desc, p_desc_n)
        size += DESC6F_APPLICATION_SIZE;
    return size == desc_length;
}

static inline void desc6f_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    if (i_print_type == PRINT_XML)
        pf_print(opaque, "<APPLICATION_SIGNALLING_DESC>");
    else
        pf_print(opaque, "    - desc 6f application_signalling");

    desc6f_each_application(p_desc, p_desc_n) {
        switch (i_print_type) {
            case PRINT_XML:
                pf_print(opaque, "<APPLICATION type=\"%u\""
                         " AIT_version_number=\"%u\"/>",
                         desc6f_app_get_type(p_desc_n),
                         desc6f_app_get_AIT_version_number(p_desc_n));
                break;
            default:
                pf_print(opaque, "        - application"
                         " type=%u AIT_version_number=%u",
                         desc6f_app_get_type(p_desc_n),
                         desc6f_app_get_AIT_version_number(p_desc_n));
        }
    }
    if (i_print_type == PRINT_XML)
        pf_print(opaque, "</APPLICATION_SIGNALLING_DESC>");
}

#ifdef __cplusplus
}
#endif

#endif
