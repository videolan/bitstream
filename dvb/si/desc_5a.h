/*****************************************************************************
 * desc_5a.h: ETSI EN 300 468 Descriptor 0x5a: Terrestrial delivery system
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

#ifndef __BITSTREAM_DVB_DESC_5A_H__
#define __BITSTREAM_DVB_DESC_5A_H__

#include <inttypes.h> /* PRIu64 */

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/dvb/si/desc_43.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x5a: Terrestrial delivery system descriptor
 *****************************************************************************/
#define DESC5A_HEADER_SIZE      (DESC_HEADER_SIZE + 11)

static inline void desc5a_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x5a);
    desc_set_length(p_desc, DESC5A_HEADER_SIZE - DESC_HEADER_SIZE);
    p_desc[ 6] = 0x03;
    p_desc[ 9] = 0xff;
    p_desc[10] = 0xff;
    p_desc[11] = 0xff;
    p_desc[12] = 0xff;
}

static inline uint64_t desc5a_get_frequency(const uint8_t *p_desc)
{
    return (((uint64_t)p_desc[2] << 24) | (p_desc[3] << 16) | (p_desc[4] << 8)
             | p_desc[5]) * 10; /* Hz */
}

static inline void desc5a_set_frequency(uint8_t *p_desc, uint32_t i_freq)
{
    p_desc[2] = (i_freq >> 24) & 0xff;
    p_desc[3] = (i_freq >> 16) & 0xff;
    p_desc[4] = (i_freq >>  8) & 0xff;
    p_desc[5] = i_freq         & 0xff;
}

static inline uint8_t desc5a_get_bandwidth(const uint8_t *p_desc)
{
    return p_desc[6] >> 5;
}

static inline void desc5a_set_bandwidth(uint8_t *p_desc, uint8_t i_bandwidth)
{
    p_desc[6] = (p_desc[6] &~ 0xe0) | (i_bandwidth << 5); // 111xxxxx
}

static inline bool desc5a_get_priority(const uint8_t *p_desc)
{
    return !!((p_desc[6] >> 4) & 0x1);
}

static inline void desc5a_set_priority(uint8_t *p_desc, bool b_priority)
{
    p_desc[6] = b_priority ? (p_desc[6] | 0x10) : (p_desc[6] &~ 0x10); // xxx1xxxx
}

/* ! inverted logic ! */
static inline bool desc5a_get_timeslicing(const uint8_t *p_desc)
{
    return !((p_desc[6] >> 3) & 0x1);
}

static inline void desc5a_set_timeslicing(uint8_t *p_desc, bool b_timeslicing)
{
    p_desc[6] = !b_timeslicing ? (p_desc[6] | 0x08) : (p_desc[6] &~ 0x08); // xxxx1xxx
}

/* ! inverted logic ! */
static inline bool desc5a_get_mpefec(const uint8_t *p_desc)
{
    return !((p_desc[6] >> 2) & 0x1);
}

static inline void desc5a_set_mpefec(uint8_t *p_desc, bool b_mpefec)
{
    p_desc[6] = !b_mpefec ? (p_desc[6] | 0x04) : (p_desc[6] &~ 0x04); // xxxxx1xx
}

static inline uint8_t desc5a_get_constellation(const uint8_t *p_desc)
{
    return p_desc[7] >> 6;
}

static inline void desc5a_set_constellation(uint8_t *p_desc, uint8_t i_constellation)
{
    p_desc[7] = (p_desc[7] &~ 0xc0) | (i_constellation << 6); // 11xxxxxx
}

static inline uint8_t desc5a_get_hierarchy(const uint8_t *p_desc)
{
    return (p_desc[7] >> 3) & 0x7;
}

static inline void desc5a_set_hierarchy(uint8_t *p_desc, uint8_t i_hierarchy)
{
    p_desc[7] = (p_desc[7] &~ 0x38) | ((i_hierarchy & 0x07) << 3); // xx111xxx
}

static inline uint8_t desc5a_get_coderatehp(const uint8_t *p_desc)
{
    return p_desc[7] & 0x7;
}

static inline void desc5a_set_coderatehp(uint8_t *p_desc, uint8_t i_coderatehp)
{
    p_desc[7] = (p_desc[7] &~ 0x07) | (i_coderatehp & 0x07); // xxxxx111
}

static inline uint8_t desc5a_get_coderatelp(const uint8_t *p_desc)
{
    return p_desc[8] >> 5;
}

static inline void desc5a_set_coderatelp(uint8_t *p_desc, uint8_t i_coderatelp)
{
    p_desc[8] = (p_desc[8] &~ 0xE0) | (i_coderatelp << 5); // 111xxxxx
}

static inline uint8_t desc5a_get_guard(const uint8_t *p_desc)
{
    return (p_desc[8] >> 3) & 0x3;
}

static inline void desc5a_set_guard(uint8_t *p_desc, uint8_t i_guard)
{
    p_desc[8] = (p_desc[8] &~ 0x18) | ((i_guard & 0x03) << 3); // xxx11xxx
}

static inline uint8_t desc5a_get_transmission(const uint8_t *p_desc)
{
    return (p_desc[8] >> 1) & 0x3;
}

static inline void desc5a_set_transmission(uint8_t *p_desc, uint8_t i_transmission)
{
    p_desc[8] = (p_desc[8] &~ 0x06) | ((i_transmission & 0x03) << 1); // xxxxx11x
}

static inline bool desc5a_get_otherfrequency(const uint8_t *p_desc)
{
    return !!(p_desc[8] & 0x1);
}

static inline void desc5a_set_otherfrequency(uint8_t *p_desc, uint8_t b_otherfrequency)
{
    p_desc[8] = b_otherfrequency ? (p_desc[8] | 0x01) : (p_desc[8] &~ 0x01); // xxxxxxx1
}

static inline bool desc5a_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC5A_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc5a_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t i_bandwidth = desc5a_get_bandwidth(p_desc);
    uint8_t i_constellation = desc5a_get_constellation(p_desc);
    const char *psz_constellation = "reserved";
    uint8_t i_hierarchy = desc5a_get_hierarchy(p_desc);
    const char *psz_hierarchy = "";
    bool b_hierarchy = !!(i_hierarchy & 0x3);
    uint8_t i_guard = desc5a_get_guard(p_desc);
    const char *psz_guard = "";
    uint8_t i_transmission = desc5a_get_transmission(p_desc);
    const char *psz_transmission = "reserved";

    switch (i_bandwidth) {
        case 0x0: i_bandwidth = 8; break;
        case 0x1: i_bandwidth = 7; break;
        case 0x2: i_bandwidth = 6; break;
        case 0x3: i_bandwidth = 5; break;
        default: i_bandwidth = 0; break;
    }

    switch (i_constellation) {
        case 0x0: psz_constellation = "QPSK"; break;
        case 0x1: psz_constellation = "16-qam"; break;
        case 0x2: psz_constellation = "64-qam"; break;
    }

    switch (i_hierarchy) {
        case 0x0: psz_hierarchy = "none"; break;
        case 0x1: psz_hierarchy = "1"; break;
        case 0x2: psz_hierarchy = "2"; break;
        case 0x3: psz_hierarchy = "4"; break;
        case 0x4: psz_hierarchy = "none+in-depth"; break;
        case 0x5: psz_hierarchy = "1+in-depth"; break;
        case 0x6: psz_hierarchy = "2+in-depth"; break;
        case 0x7: psz_hierarchy = "4+in-depth"; break;
    }

    switch (i_guard) {
        case 0x0: psz_guard = "1/32"; break;
        case 0x1: psz_guard = "1/16"; break;
        case 0x2: psz_guard = "1/8"; break;
        case 0x3: psz_guard = "1/4"; break;
    }

    switch (i_transmission) {
        case 0x0: psz_transmission = "2k"; break;
        case 0x1: psz_transmission = "8k"; break;
        case 0x2: psz_transmission = "4k"; break;
    }

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
             "<TERRESTRIAL_DESC frequency=\"%"PRIu64"\" bandwidth=\"%u\" priority=\"%s\" timeslicing=\"%d\" mpefec=\"%d\" constellation=\"%s\" hierarchy=\"%s\" coderatehp=\"%s\" coderatelp=\"%s\" guard=\"%s\" transmission=\"%s\" otherfrequency=\"%d\"/>",
             desc5a_get_frequency(p_desc), i_bandwidth,
             desc5a_get_priority(p_desc) ? "HP" : "LP",
             desc5a_get_timeslicing(p_desc) ? 1 : 0,
             desc5a_get_mpefec(p_desc) ? 1 : 0, psz_constellation,
             psz_hierarchy,
             dvb_delivery_get_fec(desc5a_get_coderatehp(p_desc) + 1),
             b_hierarchy ? dvb_delivery_get_fec(desc5a_get_coderatelp(p_desc) + 1) : "not_applicable",
             psz_guard, psz_transmission,
             desc5a_get_otherfrequency(p_desc) ? 1 : 0);
        break;
    default:
        pf_print(opaque,
             "    - desc 5a dvb-t frequency=%"PRIu64" Hz bandwidth=%u MHz priority=%s timeslicing=%d mpefec=%d constellation=%s hierarchy=%s coderatehp=%s coderatelp=%s guard=%s transmission=%s otherfrequency=%d",
             desc5a_get_frequency(p_desc), i_bandwidth,
             desc5a_get_priority(p_desc) ? "HP" : "LP",
             desc5a_get_timeslicing(p_desc) ? 1 : 0,
             desc5a_get_mpefec(p_desc) ? 1 : 0, psz_constellation,
             psz_hierarchy,
             dvb_delivery_get_fec(desc5a_get_coderatehp(p_desc) + 1),
             b_hierarchy ? dvb_delivery_get_fec(desc5a_get_coderatelp(p_desc) + 1) : "not_applicable",
             psz_guard, psz_transmission,
             desc5a_get_otherfrequency(p_desc) ? 1 : 0);
    }
}

#ifdef __cplusplus
}
#endif

#endif
