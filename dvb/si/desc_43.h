/*****************************************************************************
 * desc_43.h: ETSI EN 300 468 Descriptor 0x43: Satellite delivery system
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

#ifndef __BITSTREAM_DVB_DESC_43_H__
#define __BITSTREAM_DVB_DESC_43_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/dvb/si/numbers.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x43: Satellite delivery system descriptor
 *****************************************************************************/
#define DESC43_HEADER_SIZE      (DESC_HEADER_SIZE + 11)

static inline void desc43_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x43);
    desc_set_length(p_desc, DESC43_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline uint32_t desc43_get_frequency(const uint8_t *p_desc)
{
    return dvb_bcd_get(p_desc + 2, 32) * 10; /* kHz */
}

static inline void desc43_set_frequency_bcd(uint8_t *p_desc, uint32_t i_freq_bcd)
{
    p_desc[2] = (i_freq_bcd >> 24) & 0xff;
    p_desc[3] = (i_freq_bcd >> 16) & 0xff;
    p_desc[4] = (i_freq_bcd >>  8) & 0xff;
    p_desc[5] = i_freq_bcd         & 0xff;
}

static inline unsigned int desc43_get_position(const uint8_t *p_desc)
{
    return dvb_bcd_get(p_desc + 6, 16); /* 10th degree */
}

static inline void desc43_set_position_bcd(uint8_t *p_desc, uint16_t i_pos_bcd)
{
    p_desc[6] = (i_pos_bcd >> 8) & 0xff;
    p_desc[7] = i_pos_bcd        & 0xff;
}

static inline bool desc43_get_east(const uint8_t *p_desc)
{
    return !!(p_desc[8] & 0x80);
}

static inline void desc43_set_east(uint8_t *p_desc, bool b_east)
{
    p_desc[8] = b_east ? (p_desc[8] | 0x80) : (p_desc[8] &~ 0x80); // 1xxxxxxx
}

static inline uint8_t desc43_get_polarization(const uint8_t *p_desc)
{
    return (p_desc[8] & 0x60) >> 5;
}

static inline void desc43_set_polarization(uint8_t *p_desc, uint8_t i_polarization)
{
    p_desc[8] = ((i_polarization & 0x03) << 5) | (p_desc[8] &~ 0x60); // x11xxxxx
}

static inline uint8_t desc43_get_rolloff(const uint8_t *p_desc)
{
    return (p_desc[8] & 0x18) >> 3;
}

static inline void desc43_set_rolloff(uint8_t *p_desc, uint8_t i_rolloff)
{
    p_desc[8] = (p_desc[8] &~ 0x18) | (i_rolloff & 0x03 << 3); // xxx11xxx
}

static inline bool desc43_get_dvbs2(const uint8_t *p_desc)
{
    return !!(p_desc[8] & 0x4);
}

static inline void desc43_set_dvbs2(uint8_t *p_desc, bool b_dvbs2)
{
    p_desc[8] = b_dvbs2 ? (p_desc[8] | 0x04) : (p_desc[8] &~ 0x04); // xxxxx1xx
}

static inline uint8_t desc43_get_modulation(const uint8_t *p_desc)
{
    return p_desc[8] & 0x3;
}

static inline void desc43_set_modulation(uint8_t *p_desc, uint8_t i_modulation)
{
    p_desc[8] = (p_desc[8] &~ 0x03) | (i_modulation & 0x03); // xxxxxx11
}

static inline unsigned int desc43_get_symbolrate(const uint8_t *p_desc)
{
    return dvb_bcd_get(p_desc + 9, 28) * 100; /* sy/s */
}

static inline void desc43_set_symbolrate_bcd(uint8_t *p_desc, uint32_t i_symbolrate_bcd)
{
    p_desc[ 9] = (i_symbolrate_bcd >> 20) & 0xff;
    p_desc[10] = (i_symbolrate_bcd >> 12) & 0xff;
    p_desc[11] = (i_symbolrate_bcd >>  4) & 0xff;
    p_desc[12] = ((i_symbolrate_bcd & 0x0f) << 4) | (p_desc[12] & 0x0f);
}

static inline uint8_t desc43_get_fecinner(const uint8_t *p_desc)
{
    return p_desc[12] & 0xf;
}

static inline void desc43_set_fecinner(uint8_t *p_desc, uint8_t i_fecinner)
{
    p_desc[12] = (p_desc[12] & 0xf0) | (i_fecinner & 0x0f);
}

static inline const char *dvb_delivery_get_fec(uint8_t i_fec)
{
    switch (i_fec) {
        case 0x0: return "undefined";
        case 0x1: return "1/2";
        case 0x2: return "2/3";
        case 0x3: return "3/4";
        case 0x4: return "5/6";
        case 0x5: return "7/8";
        case 0x6: return "8/9";
        case 0x7: return "3/5";
        case 0x8: return "4/5";
        case 0x9: return "9/10";
        case 0xf: return "none";
        default: return "reserved";
    }
}

static inline bool desc43_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC43_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc43_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    unsigned int i_pos = desc43_get_position(p_desc);
    uint8_t i_polarization = desc43_get_polarization(p_desc);
    const char *psz_polarization = "";;
    uint8_t i_rolloff = desc43_get_rolloff(p_desc);
    uint8_t i_modulation = desc43_get_modulation(p_desc);
    const char *psz_modulation = "";

    switch (i_polarization) {
        case 0x0: psz_polarization = "H"; break;
        case 0x1: psz_polarization = "V"; break;
        case 0x2: psz_polarization = "L"; break;
        case 0x3: psz_polarization = "R"; break;
    }

    switch (i_rolloff) {
        case 0x0: i_rolloff = 35; break;
        case 0x1: i_rolloff = 25; break;
        case 0x2: i_rolloff = 20; break;
    }

    switch (i_modulation) {
        case 0x0: psz_modulation = "auto"; break;
        case 0x1: psz_modulation = "qpsk"; break;
        case 0x2: psz_modulation = "8psk"; break;
        case 0x3: psz_modulation = "16-qam"; break;
    }

    switch (i_print_type) {
    case PRINT_XML:
        if (desc43_get_dvbs2(p_desc))
            pf_print(opaque,
             "<SATELLITE_DESC s2=\"1\" frequency=\"%u%s\" pos=\"%u.%u%c\" rolloff=\"0.%hhu\" modulation=\"%s\" symbolrate=\"%u\" fecinner=\"%s\"/>",
             desc43_get_frequency(p_desc), psz_polarization,
             i_pos / 10, i_pos % 10, desc43_get_east(p_desc) ? 'E' : 'W',
             i_rolloff, psz_modulation, desc43_get_symbolrate(p_desc),
             dvb_delivery_get_fec(desc43_get_fecinner(p_desc)));
        else
            pf_print(opaque,
             "<SATELLITE_DESC s2=\"0\" frequency=\"%u%s\" pos=\"%u.%u%c\" modulation=\"%s\" symbolrate=\"%u\" fecinner=\"%s\"/>",
             desc43_get_frequency(p_desc), psz_polarization,
             i_pos / 10, i_pos % 10, desc43_get_east(p_desc) ? 'E' : 'W',
             psz_modulation, desc43_get_symbolrate(p_desc),
             dvb_delivery_get_fec(desc43_get_fecinner(p_desc)));
        break;
    default:
        if (desc43_get_dvbs2(p_desc))
            pf_print(opaque,
             "    - desc 43 dvb-s2 frequency=%u%s pos=%u.%u%c rolloff=0.%hhu modulation=%s symbolrate=%u fecinner=%s",
             desc43_get_frequency(p_desc), psz_polarization,
             i_pos / 10, i_pos % 10, desc43_get_east(p_desc) ? 'E' : 'W',
             i_rolloff, psz_modulation, desc43_get_symbolrate(p_desc),
             dvb_delivery_get_fec(desc43_get_fecinner(p_desc)));
        else
            pf_print(opaque,
             "    - desc 43 dvb-s frequency=%u%s pos=%u.%u%c modulation=%s symbolrate=%u fecinner=%s",
             desc43_get_frequency(p_desc), psz_polarization,
             i_pos / 10, i_pos % 10, desc43_get_east(p_desc) ? 'E' : 'W',
             psz_modulation, desc43_get_symbolrate(p_desc),
             dvb_delivery_get_fec(desc43_get_fecinner(p_desc)));
    }
}

#ifdef __cplusplus
}
#endif

#endif
