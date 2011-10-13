/*****************************************************************************
 * desc_4a.h: ETSI EN 300 468 Descriptor 0x4a: Linkage descriptor
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

#ifndef __BITSTREAM_DVB_DESC_4A_H__
#define __BITSTREAM_DVB_DESC_4A_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x4a: Linkage descriptor
 *****************************************************************************/
#define DESC4A_HEADER_SIZE      (DESC_HEADER_SIZE + 7)

#define DESC4A_LINKAGE_MOBILE       0x08
#define DESC4A_LINKAGE_EVENT        0x0d
#define DESC4A_LINKAGE_EXT_EVENT    0x0e

static inline void desc4a_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x4a);
    desc_set_length(p_desc, DESC4A_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline uint16_t desc4a_get_tsid(const uint8_t *p_desc)
{
    return (p_desc[2] << 8) | p_desc[3];
}

static inline void desc4a_set_tsid(uint8_t *p_desc, uint16_t i_tsid)
{
    p_desc[2] = (i_tsid >> 8) & 0xff;
    p_desc[3] =  i_tsid       & 0xff;
}

static inline uint16_t desc4a_get_onid(const uint8_t *p_desc)
{
    return (p_desc[4] << 8) | p_desc[5];
}

static inline void desc4a_set_onid(uint8_t *p_desc, uint16_t i_onid)
{
    p_desc[4] = (i_onid >> 8) & 0xff;
    p_desc[5] =  i_onid       & 0xff;
}

static inline uint16_t desc4a_get_sid(const uint8_t *p_desc)
{
    return (p_desc[6] << 8) | p_desc[7];
}

static inline void desc4a_set_sid(uint8_t *p_desc, uint16_t i_sid)
{
    p_desc[6] = (i_sid >> 8) & 0xff;
    p_desc[7] =  i_sid       & 0xff;
}

static inline uint8_t desc4a_get_linkage(const uint8_t *p_desc)
{
    return p_desc[8];
}

static inline void desc4a_set_linkage(uint8_t *p_desc, uint8_t i_linkage)
{
    p_desc[8] = i_linkage;
}

static inline const char *desc4a_get_linkage_txt(uint8_t i_linkage)
{
    return i_linkage == 0x00 ? "reserved" :
           i_linkage == 0x01 ? "information service" :
           i_linkage == 0x02 ? "EPG service" :
           i_linkage == 0x03 ? "CA replacement service" :
           i_linkage == 0x04 ? "TS containing complete Network/Bouquet SI" :
           i_linkage == 0x05 ? "service replacement service" :
           i_linkage == 0x06 ? "data broadcast service" :
           i_linkage == 0x07 ? "RCS Map" :
           i_linkage == 0x08 ? "mobile hand-over" :
           i_linkage == 0x09 ? "System Software Update Service (TS 102 006 [11])" :
           i_linkage == 0x0a ? "TS containing SSU BAT or NIT (TS 102 006 [11])" :
           i_linkage == 0x0b ? "IP/MAC Notification Service (EN 301 192 [4])" :
           i_linkage == 0x0c ? "TS containing INT BAT or NIT (EN 301 192 [4])" :
           i_linkage == 0x0d ? "event linkage" :
           i_linkage == 0x0e ? "extended event linkage" :
           i_linkage >= 0x0f && i_linkage <= 0x7f ? "reserved" :
           i_linkage >= 0x80 && i_linkage <= 0xfe ? "user defined" : "reserved";
}

/* MOBILE HANDOVER linkage == 0x08 (DESC4A_LINKAGE_MOBILE) */

static inline uint8_t desc4a_get_mobile_handover_type(const uint8_t *p_desc)
{
    return (p_desc[9] & 0xf0) >> 4;
}

static inline void desc4a_set_mobile_handover_type(uint8_t *p_desc, uint8_t i_handover_type)
{
    p_desc[9] = ((i_handover_type & 0x0f) << 4) | 0x0e | (p_desc[9] & 0x01);
}

static inline bool desc4a_get_mobile_origin_type(const uint8_t *p_desc)
{
    return (p_desc[9] & 0x01) == 0x01;
}

static inline void desc4a_set_mobile_origin_type(uint8_t *p_desc, bool b_origin_type)
{
    p_desc[9] = b_origin_type ? (p_desc[9] | 0x01) : (p_desc[9] &~ 0x01);
}

static inline bool desc4a_have_mobile_nid(const uint8_t *p_desc)
{
    uint8_t i_handover_type = desc4a_get_mobile_handover_type(p_desc);
    return (i_handover_type >= 0x01 && i_handover_type <= 0x03);
}

static inline uint16_t desc4a_get_mobile_nid(const uint8_t *p_desc)
{
    if (desc4a_have_mobile_nid(p_desc))
        return (p_desc[10] << 8) | p_desc[11];
    else
        return 0;
}

static inline void desc4a_set_mobile_nid(uint8_t *p_desc, uint16_t i_nid)
{
    if (desc4a_have_mobile_nid(p_desc)) {
        p_desc[10] = (i_nid >> 8) & 0xff;
        p_desc[11] =  i_nid       & 0xff;
    }
}

static inline uint16_t desc4a_get_mobile_initial_sid(const uint8_t *p_desc)
{
    if (!desc4a_get_mobile_origin_type(p_desc)) {
        uint8_t ofs = 10;
        if (desc4a_have_mobile_nid(p_desc))
            ofs += 2;
        return (p_desc[ofs + 0] << 8) | p_desc[ofs + 1];
    }
    return 0;
}

static inline void desc4a_set_mobile_initial_sid(uint8_t *p_desc, uint16_t i_initial_sid)
{
    if (!desc4a_get_mobile_origin_type(p_desc)) {
        uint8_t ofs = 10;
        if (desc4a_have_mobile_nid(p_desc))
            ofs += 2;
        p_desc[ofs + 0] = (i_initial_sid >> 8) & 0xff;
        p_desc[ofs + 1] =  i_initial_sid       & 0xff;
    }
}

static inline uint8_t desc4a_get_mobile_handover_length(const uint8_t *p_desc)
{
    uint8_t i_length = 0;
    if (desc4a_get_linkage(p_desc) == DESC4A_LINKAGE_MOBILE)
    {
        i_length += 1;
        if (desc4a_have_mobile_nid(p_desc))
            i_length += 2;
        if (!desc4a_get_mobile_origin_type(p_desc))
            i_length += 2;
    }
    return i_length;
}

/* EVENT LINKAGE == 0x0d (DESC4A_LINKAGE_EVENT) */

static inline uint16_t desc4a_get_event_target_event_id(const uint8_t *p_desc)
{
    return (p_desc[9] << 8) | p_desc[10];
}

static inline void desc4a_set_event_target_event_id(uint8_t *p_desc, uint16_t i_target_event_id)
{
    p_desc[ 9] = (i_target_event_id >> 8) & 0xff;
    p_desc[10] =  i_target_event_id       & 0xff;
}

static inline bool desc4a_get_event_target_listed(const uint8_t *p_desc)
{
    return (p_desc[11] & 0x80) == 0x80;
}

static inline void desc4a_set_event_target_listed(uint8_t *p_desc, bool b_target_listed)
{
    p_desc[11] |= 0x3f;
    p_desc[11]  = b_target_listed ? (p_desc[11] | 0x80) : (p_desc[11] &~ 0x80);
}

static inline bool desc4a_get_event_simulcast(const uint8_t *p_desc)
{
    return (p_desc[11] & 0x40) == 0x40;
}

static inline void desc4a_set_event_simulcast(uint8_t *p_desc, bool b_event_simulcast)
{
    p_desc[11] |= 0x3f;
    p_desc[11]  = b_event_simulcast ? (p_desc[11] | 0x40) : (p_desc[11] &~ 0x40);
}

static inline uint8_t desc4a_get_event_length(const uint8_t *p_desc)
{
    if (desc4a_get_linkage(p_desc) == DESC4A_LINKAGE_EVENT)
        return 3;
    return 0;
}

/* EXTENDED EVENT LINKAGE == 0x0e (DESC4A_LINKAGE_EXT_EVENT) */

static inline uint16_t desc4an_get_ext_event_target_event_id(const uint8_t *p_desc_n)
{
    return (p_desc_n[0] << 8) | p_desc_n[1];
}

static inline void desc4an_set_ext_event_target_event_id(uint8_t *p_desc_n, uint16_t i_target_event_id)
{
    p_desc_n[0] = (i_target_event_id >> 8) & 0xff;
    p_desc_n[1] =  i_target_event_id       & 0xff;
}

static inline bool desc4an_get_ext_event_target_listed(const uint8_t *p_desc_n)
{
    return (p_desc_n[2] & 0x80) == 0x80;
}

static inline void desc4an_set_ext_event_target_listed(uint8_t *p_desc_n, bool b_target_listed)
{
    p_desc_n[2]  = b_target_listed ? (p_desc_n[2] | 0x80) : (p_desc_n[2] &~ 0x80);
}

static inline bool desc4an_get_ext_event_simulcast(const uint8_t *p_desc_n)
{
    return (p_desc_n[2] & 0x40) == 0x40;
}

static inline void desc4an_set_ext_event_simulcast(uint8_t *p_desc_n, bool b_event_simulcast)
{
    p_desc_n[2] = b_event_simulcast ? (p_desc_n[2] | 0x40) : (p_desc_n[2] &~ 0x40);
}

static inline uint8_t desc4an_get_ext_event_link_type(const uint8_t *p_desc_n)
{
    return (p_desc_n[2] & 0x30) >> 4;
}

static inline void desc4an_set_ext_event_link_type(uint8_t *p_desc_n, uint8_t i_link_type)
{
    p_desc_n[2] = (p_desc_n[2] & 0xcf) | ((i_link_type & 0x03) << 4);
}

static inline const char *desc4a_get_ext_event_link_type_txt(uint8_t i_link_type)
{
    return i_link_type == 0 ? "SD" :
           i_link_type == 1 ? "HD" :
           i_link_type == 2 ? "3D" : "reserved";
}

static inline uint8_t desc4an_get_ext_event_target_id_type(const uint8_t *p_desc_n)
{
    return (p_desc_n[2] & 0x0c) >> 2;
}

static inline void desc4an_set_ext_event_target_id_type(uint8_t *p_desc_n, uint8_t i_target_id_type)
{
    p_desc_n[2] = (p_desc_n[2] & 0xf3) | ((i_target_id_type & 0x03) << 2);
}

static inline const char *desc4a_get_ext_event_target_id_type_txt(uint8_t i_target_id_type)
{
    return i_target_id_type == 0 ? "use tsid" :
           i_target_id_type == 1 ? "use target_tsid" :
           i_target_id_type == 2 ? "match any tsid" : "use user_defined_id";
}

static inline bool desc4an_get_ext_event_onid_id_flag(const uint8_t *p_desc_n)
{
    return (p_desc_n[2] & 0x02) == 0x02;
}

static inline void desc4an_set_ext_event_onid_id_flag(uint8_t *p_desc_n, bool b_event_onid_id_flag)
{
    p_desc_n[2] = b_event_onid_id_flag ? (p_desc_n[2] | 0x02) : (p_desc_n[2] &~ 0x02);
}

static inline bool desc4an_get_ext_event_service_id_flag(const uint8_t *p_desc_n)
{
    return (p_desc_n[2] & 0x01) == 0x01;
}

static inline void desc4an_set_ext_event_service_id_flag(uint8_t *p_desc_n, bool b_event_service_id_flag)
{
    p_desc_n[2] = b_event_service_id_flag ? (p_desc_n[2] | 0x01) : (p_desc_n[2] &~ 0x01);
}


static inline bool desc4an_ext_event_have_user_defined_id(const uint8_t *p_desc_n)
{
    return desc4an_get_ext_event_target_id_type(p_desc_n) == 3;
}

static inline bool desc4an_ext_event_have_target_tsid(const uint8_t *p_desc_n)
{
    return desc4an_get_ext_event_target_id_type(p_desc_n) == 1;
}


static inline uint16_t desc4an_get_ext_event_user_defined_id(const uint8_t *p_desc_n)
{
    if (desc4an_ext_event_have_user_defined_id(p_desc_n))
        return (p_desc_n[3] << 8) | p_desc_n[4];
    else
        return 0;
}

static inline void desc4an_set_ext_event_user_defined_id(uint8_t *p_desc_n, uint16_t i_user_defined_id)
{
    if (desc4an_ext_event_have_user_defined_id(p_desc_n)) {
        p_desc_n[3] = (i_user_defined_id >> 8) & 0xff;
        p_desc_n[4] =  i_user_defined_id       & 0xff;
    }
}

static inline uint16_t desc4an_get_ext_event_target_tsid(const uint8_t *p_desc_n)
{
    if (desc4an_ext_event_have_target_tsid(p_desc_n))
        return (p_desc_n[3] << 8) | p_desc_n[4];
    else
        return 0;
}

static inline void desc4an_set_ext_event_target_tsid(uint8_t *p_desc_n, uint16_t i_target_tsid)
{
    if (desc4an_ext_event_have_target_tsid(p_desc_n)) {
        p_desc_n[3] = (i_target_tsid >> 8) & 0xff;
        p_desc_n[4] =  i_target_tsid       & 0xff;
    }
}

static inline uint16_t desc4an_get_ext_event_target_onid(const uint8_t *p_desc_n)
{
    if (desc4an_ext_event_have_user_defined_id(p_desc_n))
        return 0;
    uint8_t ofs = desc4an_ext_event_have_target_tsid(p_desc_n) ? 5 : 3;
    if (desc4an_get_ext_event_onid_id_flag(p_desc_n))
        return (p_desc_n[ofs + 0] << 8) | p_desc_n[ofs + 1];
    else
        return 0;
}

static inline void desc4an_set_ext_event_target_onid(uint8_t *p_desc_n, uint16_t i_target_onid)
{
    if (desc4an_ext_event_have_user_defined_id(p_desc_n))
        return;
    uint8_t ofs = desc4an_ext_event_have_target_tsid(p_desc_n) ? 5 : 3;
    if (desc4an_get_ext_event_onid_id_flag(p_desc_n)) {
        p_desc_n[ofs + 0] = (i_target_onid >> 8) & 0xff;
        p_desc_n[ofs + 1] =  i_target_onid       & 0xff;
    }
}

static inline uint16_t desc4an_get_ext_event_service_id(const uint8_t *p_desc_n)
{
    if (desc4an_ext_event_have_user_defined_id(p_desc_n))
        return 0;
    uint8_t ofs = desc4an_ext_event_have_target_tsid(p_desc_n) ? 5 : 3;
    if (desc4an_get_ext_event_onid_id_flag(p_desc_n))
        ofs += 2;
    if (desc4an_get_ext_event_service_id_flag(p_desc_n))
        return (p_desc_n[ofs + 0] << 8) | p_desc_n[ofs + 1];
    else
        return 0;
}

static inline void desc4an_set_ext_event_service_id(uint8_t *p_desc_n, uint16_t i_service_id)
{
    if (desc4an_ext_event_have_user_defined_id(p_desc_n))
        return;
    uint8_t ofs = desc4an_ext_event_have_target_tsid(p_desc_n) ? 5 : 3;
    if (desc4an_get_ext_event_onid_id_flag(p_desc_n))
        ofs += 2;
    if (desc4an_get_ext_event_service_id_flag(p_desc_n)) {
        p_desc_n[ofs + 0] = (i_service_id >> 8) & 0xff;
        p_desc_n[ofs + 1] =  i_service_id       & 0xff;
    }
}

static inline uint8_t desc4an_get_ext_event_length(const uint8_t *p_desc_n)
{
    uint8_t i_len = 3;
    if (desc4an_ext_event_have_user_defined_id(p_desc_n))
        return i_len + 2;
    if (desc4an_ext_event_have_target_tsid(p_desc_n))
        i_len += 2;
    if (desc4an_get_ext_event_onid_id_flag(p_desc_n))
        i_len += 2;
    if (desc4an_get_ext_event_service_id_flag(p_desc_n))
        i_len += 2;
    return i_len;
}

static inline uint8_t *desc4a_get_ext_event(const uint8_t *p_desc, uint8_t n)
{
    uint8_t *p     = (uint8_t *)p_desc + DESC4A_HEADER_SIZE;
    uint8_t *p_end = (uint8_t *)p_desc + DESC_HEADER_SIZE + desc_get_length(p_desc);

    if (p == p_end)
        return NULL;

    if (n == 0)
        return p;

    while (p < p_end && n) {
        p += desc4an_get_ext_event_length(p);
        if (--n == 0 && p < p_end )
            return p;
    }

    return NULL;
}

/* Generic desc4a continued... */

static inline void desc4a_set_length(uint8_t *p_desc)
{
    uint8_t i_length = DESC4A_HEADER_SIZE - DESC_HEADER_SIZE;
    i_length += desc4a_get_mobile_handover_length(p_desc);
    i_length += desc4a_get_event_length(p_desc);
    desc_set_length(p_desc, i_length);
}

static inline bool desc4a_validate(const uint8_t *p_desc)
{
    uint8_t i_length = DESC4A_HEADER_SIZE - DESC_HEADER_SIZE;
    i_length += desc4a_get_mobile_handover_length(p_desc);
    i_length += desc4a_get_event_length(p_desc);
    return desc_get_length(p_desc) >= i_length;
}

static inline void desc4a_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t j;
    uint8_t *p_desc_n;
    uint8_t i_linkage = desc4a_get_linkage(p_desc);

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
             "<LINKAGE_DESC tsid=\"%hu\" onid=\"%hu\" sid=\"%hu\" linkage=\"0x%02x\" linkage_txt=\"%s\">",
             desc4a_get_tsid(p_desc), desc4a_get_onid(p_desc),
             desc4a_get_sid(p_desc), i_linkage,
             desc4a_get_linkage_txt(i_linkage)
             );
        if (i_linkage == DESC4A_LINKAGE_MOBILE) {
            pf_print(opaque,
                 "<MOBILE_HANDOVER_INFO handover_type=\"%u\" origin_type=\"%u\" nid=\"%u\" initial_sid=\"%u\"/>",
                 desc4a_get_mobile_handover_type(p_desc),
                 desc4a_get_mobile_origin_type(p_desc),
                 desc4a_get_mobile_nid(p_desc),
                 desc4a_get_mobile_initial_sid(p_desc)
                 );
        }
        if (i_linkage == DESC4A_LINKAGE_EVENT) { // event linkage
            pf_print(opaque,
                 "<EVENT_LINKAGE target_event_id=\"%u\" target_listed=\"%u\" event_simulcast=\"%u\"/>",
                 desc4a_get_event_target_event_id(p_desc),
                 desc4a_get_event_target_listed(p_desc),
                 desc4a_get_event_simulcast(p_desc)
                 );
        }
        if (i_linkage == DESC4A_LINKAGE_EXT_EVENT) {
            j = 0;
            while ((p_desc_n = desc4a_get_ext_event(p_desc, j++)) != NULL) {
                pf_print(opaque,
                     "<EXTENDED_EVENT_LINKAGE target_event_id=\"%u\" target_listed=\"%u\" event_simulcast=\"%u\""
                     " link_type=\"%u\" link_type_txt=\"%s\""
                     " target_id_type=\"%u\" target_id_type_txt=\"%s\""
                     " onid_id_flag=\"%u\" service_id_flag=\"%u\""
                     " user_defined_id=\"%u\" target_tsid=\"%u\" target_onid=\"%u\" target_service_id=\"%u\"/>",
                     desc4an_get_ext_event_target_event_id(p_desc_n),
                     desc4an_get_ext_event_target_listed(p_desc_n),
                     desc4an_get_ext_event_simulcast(p_desc_n),
                     desc4an_get_ext_event_link_type(p_desc_n),
                     desc4a_get_ext_event_link_type_txt(desc4an_get_ext_event_link_type(p_desc_n)),
                     desc4an_get_ext_event_target_id_type(p_desc_n),
                     desc4a_get_ext_event_target_id_type_txt(desc4an_get_ext_event_target_id_type(p_desc_n)),
                     desc4an_get_ext_event_onid_id_flag(p_desc_n),
                     desc4an_get_ext_event_service_id_flag(p_desc_n),
                     desc4an_get_ext_event_user_defined_id(p_desc_n),
                     desc4an_get_ext_event_target_tsid(p_desc_n),
                     desc4an_get_ext_event_target_onid(p_desc_n),
                     desc4an_get_ext_event_service_id(p_desc_n)
                );
            }
        }
        pf_print(opaque,
             "</LINKAGE_DESC>");
        break;
    default:
        pf_print(opaque,
             "    - desc 4a linkage tsid=%hu onid=%hu sid=%hu linkage=0x%02x linkage_txt=\"%s\"",
             desc4a_get_tsid(p_desc), desc4a_get_onid(p_desc),
             desc4a_get_sid(p_desc), i_linkage,
             desc4a_get_linkage_txt(i_linkage)
             );
        if (i_linkage == DESC4A_LINKAGE_MOBILE) {
            pf_print(opaque,
                 "        - mobile_handover handover_type=%u origin_type=%u nid=%u initial_sid=%u",
                 desc4a_get_mobile_handover_type(p_desc),
                 desc4a_get_mobile_origin_type(p_desc),
                 desc4a_get_mobile_nid(p_desc),
                 desc4a_get_mobile_initial_sid(p_desc)
                 );
        }
        if (i_linkage == DESC4A_LINKAGE_EVENT) {
            pf_print(opaque,
                 "        - event_linkage target_event_id=%u target_listed=%u event_simulcast=%u",
                 desc4a_get_event_target_event_id(p_desc),
                 desc4a_get_event_target_listed(p_desc),
                 desc4a_get_event_simulcast(p_desc)
                 );
        }
        if (i_linkage == DESC4A_LINKAGE_EXT_EVENT) {
            j = 0;
            while ((p_desc_n = desc4a_get_ext_event(p_desc, j++)) != NULL) {
                pf_print(opaque,
                     "        - extended_event_linkage target_event_id=%u target_listed=%u event_simulcast=%u"
                     " link_type=%u link_type_txt=\"%s\""
                     " target_id_type=%u target_id_type_txt=\"%s\""
                     " onid_id_flag=%u service_id_flag=%u"
                     " user_defined_id=%u target_tsid=%u target_onid=%u target_service_id=%u",
                     desc4an_get_ext_event_target_event_id(p_desc_n),
                     desc4an_get_ext_event_target_listed(p_desc_n),
                     desc4an_get_ext_event_simulcast(p_desc_n),
                     desc4an_get_ext_event_link_type(p_desc_n),
                     desc4a_get_ext_event_link_type_txt(desc4an_get_ext_event_link_type(p_desc_n)),
                     desc4an_get_ext_event_target_id_type(p_desc_n),
                     desc4a_get_ext_event_target_id_type_txt(desc4an_get_ext_event_target_id_type(p_desc_n)),
                     desc4an_get_ext_event_onid_id_flag(p_desc_n),
                     desc4an_get_ext_event_service_id_flag(p_desc_n),
                     desc4an_get_ext_event_user_defined_id(p_desc_n),
                     desc4an_get_ext_event_target_tsid(p_desc_n),
                     desc4an_get_ext_event_target_onid(p_desc_n),
                     desc4an_get_ext_event_service_id(p_desc_n)
                );
            }
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
