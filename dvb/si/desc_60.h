/*****************************************************************************
 * desc_60.h: ETSI EN 300 468 Descriptor 0x60: Service move descriptor
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

/*
 * Normative references:
 *  - ETSI EN 300 468 V1.11.1 (2010-04) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_DESC_60_H__
#define __BITSTREAM_DVB_DESC_60_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x60: Service move descriptor
 *****************************************************************************/
#define DESC60_HEADER_SIZE      (DESC_HEADER_SIZE + 6)

static inline void desc60_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x60);
    desc_set_length(p_desc, DESC60_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline uint16_t desc60_get_new_onid(const uint8_t *p_desc)
{
    return (p_desc[2] << 8) | p_desc[3];
}

static inline void desc60_set_new_onid(uint8_t *p_desc, uint16_t i_new_onid)
{
    p_desc[2] = (i_new_onid >> 8) & 0xff;
    p_desc[3] =  i_new_onid       & 0xff;
}

static inline uint16_t desc60_get_new_tsid(const uint8_t *p_desc)
{
    return (p_desc[4] << 8) | p_desc[5];
}

static inline void desc60_set_new_tsid(uint8_t *p_desc, uint16_t i_new_tsid)
{
    p_desc[4] = (i_new_tsid >> 8) & 0xff;
    p_desc[5] =  i_new_tsid       & 0xff;
}

static inline uint16_t desc60_get_new_service_id(const uint8_t *p_desc)
{
    return (p_desc[6] << 8) | p_desc[7];
}

static inline void desc60_set_new_service_id(uint8_t *p_desc, uint16_t i_new_service_id)
{
    p_desc[6] = (i_new_service_id >> 8) & 0xff;
    p_desc[7] =  i_new_service_id       & 0xff;
}

static inline bool desc60_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC60_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc60_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<SERVICE_MOVE_DESC new_onid=\"%u\" new_tsid=\"%u\" new_service_id=\"%u\"/>",
                 desc60_get_new_onid(p_desc),
                 desc60_get_new_tsid(p_desc),
                 desc60_get_new_service_id(p_desc)
                );
        break;
    default:
        pf_print(opaque, "    - desc 60 service_move new_onid=%u new_tsid=%u new_service_id=%u",
                 desc60_get_new_onid(p_desc),
                 desc60_get_new_tsid(p_desc),
                 desc60_get_new_service_id(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
