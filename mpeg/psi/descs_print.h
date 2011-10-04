/*****************************************************************************
 * descs_print.h: Print defined MPEG and DVB descriptors
 *****************************************************************************
 * Copyright (C) 2010 VideoLAN
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

#ifndef __BITSTREAM_MPEG_DESCS_PRINT_H__
#define __BITSTREAM_MPEG_DESCS_PRINT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/mpeg/psi/descs_list.h>
#include <bitstream/dvb/si/descs_list.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptors list
 *****************************************************************************/
static inline void descl_print(uint8_t *p_descl, uint16_t i_length,
                               f_print pf_print, void *print_opaque,
                               f_iconv pf_iconv, void *iconv_opaque,
                               print_type_t i_print_type)
{
    uint16_t j = 0;
    uint8_t *p_desc;
    uint32_t i_private_data_specifier = 0;

    while ((p_desc = descl_get_desc(p_descl, i_length, j)) != NULL) {
        uint8_t i_tag = desc_get_tag(p_desc);
        j++;

        desc_print_begin(p_desc, pf_print, print_opaque, i_print_type);

        if (i_private_data_specifier == 0x28) {
            /* EICTA */
            switch (i_tag) {
#define CASE_DESC(id)                                                       \
            case 0x##id:                                                    \
                if (desc##id##p28_validate(p_desc))                         \
                    desc##id##p28_print(p_desc, pf_print, print_opaque,     \
                                        i_print_type);                      \
                else                                                        \
                    desc_print_error(p_desc, pf_print, print_opaque,        \
                                     i_print_type);                         \
                goto print_end;

            CASE_DESC(83)
            CASE_DESC(88)

#undef CASE_DESC

            default:
                break;
            }
        }

        switch (i_tag) {
#define CASE_DESC(id)                                                       \
        case 0x##id:                                                        \
            if (desc##id##_validate(p_desc))                                \
                desc##id##_print(p_desc, pf_print, print_opaque,            \
                                 i_print_type);                             \
            else                                                            \
                desc_print_error(p_desc, pf_print, print_opaque,            \
                                 i_print_type);                             \
            break;

#define CASE_DESC_ICONV(id)                                                 \
        case 0x##id:                                                        \
            if (desc##id##_validate(p_desc))                                \
                desc##id##_print(p_desc, pf_print, print_opaque,            \
                                 pf_iconv, iconv_opaque, i_print_type);     \
            else                                                            \
                desc_print_error(p_desc, pf_print, print_opaque,            \
                                 i_print_type);                             \
            break;

        CASE_DESC(02)
        CASE_DESC(03)
        CASE_DESC(04)
        CASE_DESC(05)
        CASE_DESC(06)
        CASE_DESC(07)
        CASE_DESC(08)
        CASE_DESC(09)
        CASE_DESC(0a)
        CASE_DESC(0b)
        CASE_DESC(0c)
        CASE_DESC(0d)
        CASE_DESC(0e)
        CASE_DESC(0f)
        CASE_DESC(10)
        CASE_DESC(11)
        CASE_DESC(12)
        CASE_DESC(1b)
        CASE_DESC(1c)
        CASE_DESC_ICONV(40)
        CASE_DESC(41)
        CASE_DESC(43)
        CASE_DESC(44)
        CASE_DESC(46)
        CASE_DESC_ICONV(47)
        CASE_DESC_ICONV(48)
        CASE_DESC(4a)
        CASE_DESC_ICONV(4d)
        CASE_DESC_ICONV(4e)
        CASE_DESC(52)
        CASE_DESC(54)
        CASE_DESC(55)
        CASE_DESC(56)
        CASE_DESC(58)
        CASE_DESC(59)
        CASE_DESC(5a)
        CASE_DESC(6a)

#undef CASE_DESC
#undef CASE_DESC_ICONV

        case 0x5f:
            if (desc5f_validate(p_desc)) {
                desc5f_print(p_desc, pf_print, print_opaque, i_print_type);
                i_private_data_specifier = desc5f_get_specifier(p_desc);
            } else
                desc_print_error(p_desc, pf_print, print_opaque, i_print_type);
            break;

        default:
            desc_print(p_desc, pf_print, print_opaque, i_print_type);
            break;
        }

print_end:
        desc_print_end(p_desc, pf_print, print_opaque, i_print_type);
    }
}

static inline void descs_print(uint8_t *p_descs,
                               f_print pf_print, void *print_opaque,
                               f_iconv pf_iconv, void *iconv_opaque,
                               print_type_t i_print_type)
{
    descl_print(p_descs + DESCS_HEADER_SIZE, descs_get_length(p_descs),
                pf_print, print_opaque, pf_iconv, iconv_opaque, i_print_type);
}

#ifdef __cplusplus
}
#endif

#endif
