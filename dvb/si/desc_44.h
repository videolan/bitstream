/*****************************************************************************
 * desc_44.h: ETSI EN 300 468 Descriptor 0x44: Cable delivery system
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

#ifndef __BITSTREAM_DVB_DESC_44_H__
#define __BITSTREAM_DVB_DESC_44_H__

#include <inttypes.h> /* PRIu64 */

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/dvb/si/desc_43.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x44: Cable delivery system descriptor
 *****************************************************************************/
#define DESC44_HEADER_SIZE      (DESC_HEADER_SIZE + 11)

static inline void desc44_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x44);
    desc_set_length(p_desc, DESC44_HEADER_SIZE - DESC_HEADER_SIZE);
    p_desc[6]  = 0xff;
    p_desc[7] |= 0xf0;
}


static inline uint64_t desc44_get_frequency(const uint8_t *p_desc)
{
    return (uint64_t)dvb_bcd_get(p_desc + 2, 32) * 100; /* Hz */
}

static inline void desc44_set_frequency_bcd(uint8_t *p_desc, uint32_t i_freq_bcd)
{
    p_desc[2] = (i_freq_bcd >> 24) & 0xff;
    p_desc[3] = (i_freq_bcd >> 16) & 0xff;
    p_desc[4] = (i_freq_bcd >>  8) & 0xff;
    p_desc[5] = i_freq_bcd         & 0xff;
}

static inline uint8_t desc44_get_fecouter(const uint8_t *p_desc)
{
    return p_desc[7] & 0xf;
}

static inline void desc44_set_fecouter(uint8_t *p_desc, uint8_t i_fecouter)
{
    p_desc[7] = 0xf0 | (i_fecouter & 0x0f);
}

static inline uint8_t desc44_get_modulation(const uint8_t *p_desc)
{
    return p_desc[8];
}

static inline void desc44_set_modulation(uint8_t *p_desc, uint8_t i_modulation)
{
    p_desc[8] = i_modulation;
}

#define desc44_get_symbolrate desc43_get_symbolrate
#define desc44_set_symbolrate_bcd desc43_set_symbolrate_bcd
#define desc44_get_fecinner desc43_get_fecinner
#define desc44_set_fecinner desc43_set_fecinner

static inline bool desc44_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC44_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc44_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t i_fecouter = desc44_get_fecouter(p_desc);
    const char *psz_fecouter = "reserved";
    uint8_t i_modulation = desc44_get_modulation(p_desc);
    const char *psz_modulation = "reserved";

    switch (i_fecouter) {
        case 0x0: psz_fecouter = "undefined"; break;
        case 0x1: psz_fecouter = "none"; break;
        case 0x2: psz_fecouter = "rs(204/188)"; break;
    }

    switch (i_modulation) {
        case 0x0: psz_modulation = "undefined"; break;
        case 0x1: psz_modulation = "16-qam"; break;
        case 0x2: psz_modulation = "32-qam"; break;
        case 0x3: psz_modulation = "64-qam"; break;
        case 0x4: psz_modulation = "128-qam"; break;
        case 0x5: psz_modulation = "256-qam"; break;
    }

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
             "<CABLE_DESC frequency=\"%"PRIu64"\" fecouter=\"%s\" modulation=\"%s\" symbolrate=\"%u\" fecinner=\"%s\"/>",
             desc44_get_frequency(p_desc), psz_fecouter, psz_modulation,
             desc44_get_symbolrate(p_desc),
             dvb_delivery_get_fec(desc43_get_fecinner(p_desc)));
        break;
    default:
        pf_print(opaque,
             "    - desc 44 dvb-c frequency=%"PRIu64" Hz fecouter=%s modulation=%s symbolrate=%u fecinner=%s",
             desc44_get_frequency(p_desc), psz_fecouter, psz_modulation,
             desc44_get_symbolrate(p_desc),
             dvb_delivery_get_fec(desc43_get_fecinner(p_desc)));
    }
}

#ifdef __cplusplus
}
#endif

#endif
