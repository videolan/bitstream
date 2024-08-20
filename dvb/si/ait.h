/*****************************************************************************
 * ait.h: ETSI TS 102 809 Application Information Table (AIT)
 *****************************************************************************
 * Copyright (C) 2024 EasyTools
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
 *  - ETSI TS 102 809 V1.1.1 (2010-01) (Signalling and carriage of interactive
 *  applications and services)
 */

#ifndef __BITSTREAM_DVB_AIT_H__
#define __BITSTREAM_DVB_AIT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Application Information Table descriptors
 *****************************************************************************/
#define AIT_DESC_APP                0x00

#define AIT_DESC_APP_SIZE           3
#define AIT_DESC_APP_PROFILE_SIZE   5

static inline uint8_t ait_desc_app_get_profilelength(const uint8_t *p_desc)
{
    return p_desc[2];
}

static inline uint8_t *ait_desc_app_next_profile(const uint8_t *p_desc,
                                                 uint8_t *p_profile)
{
    uint8_t profilelength = ait_desc_app_get_profilelength(p_desc);

    if (!p_profile)
        p_profile = (uint8_t *)p_desc + 3;
    else
        p_profile += AIT_DESC_APP_PROFILE_SIZE;
    return p_profile - p_desc - 3 + AIT_DESC_APP_PROFILE_SIZE > profilelength ?
        NULL : p_profile;
}

#define ait_desc_app_each_profile(DESC, PROFILE) \
    for (uint8_t *PROFILE = ait_desc_app_next_profile(DESC, NULL); \
         PROFILE != NULL; \
         PROFILE = ait_desc_app_next_profile(DESC, PROFILE))

static inline uint16_t ait_desc_app_profile_get_type(const uint8_t *p_profile)
{
    return p_profile[0] << 8 | p_profile[1];
}

static inline uint8_t ait_desc_app_profile_get_major(const uint8_t *p_profile)
{
    return p_profile[2];
}

static inline uint8_t ait_desc_app_profile_get_minor(const uint8_t *p_profile)
{
    return p_profile[3];
}

static inline uint8_t ait_desc_app_profile_get_micro(const uint8_t *p_profile)
{
    return p_profile[4];
}

static inline uint8_t ait_desc_app_get_service_bound(const uint8_t *p_desc)
{
    uint8_t profilelength = ait_desc_app_get_profilelength(p_desc);
    return (p_desc[3 + profilelength] >> 7) & 0x1;
}

static inline uint8_t ait_desc_app_get_visibily(const uint8_t *p_desc)
{
    uint8_t profilelength = ait_desc_app_get_profilelength(p_desc);
    return (p_desc[3 + profilelength] >> 5) & 0x3;
}

static inline uint8_t ait_desc_app_get_priority(const uint8_t *p_desc)
{
    uint8_t profilelength = ait_desc_app_get_profilelength(p_desc);
    return p_desc[3 + profilelength + 1];
}

static inline uint8_t *ait_desc_app_next_transport_proto(const uint8_t *p_desc,
                                                         uint8_t *p_proto)
{
    const uint8_t *p_end = p_desc + DESC_HEADER_SIZE + desc_get_length(p_desc);
    uint8_t profilelength = ait_desc_app_get_profilelength(p_desc);
    if (!p_proto)
        p_proto = (uint8_t *)p_desc + 5 + profilelength;
    else
        p_proto++;
    return p_proto >= p_end ? NULL : p_proto;
}

#define ait_desc_app_each_transport_proto(DESC, PROTO) \
    for (uint8_t *PROTO = ait_desc_app_next_transport_proto(DESC, NULL); \
         PROTO != NULL; \
         PROTO = ait_desc_app_next_transport_proto(DESC, PROTO))

static inline bool ait_desc_app_validate(const uint8_t *p_desc)
{
    uint8_t tag = desc_get_tag(p_desc);
    uint8_t length = desc_get_length(p_desc);

    if (tag != AIT_DESC_APP || length < AIT_DESC_APP_SIZE)
        return false;

    uint8_t profilelength = ait_desc_app_get_profilelength(p_desc);
    if (length < AIT_DESC_APP + profilelength)
        return false;
    return true;
}

#define AIT_DESC_APP_NAME           0x01

static inline uint8_t *ait_desc_app_name_get_code(const uint8_t *p_name)
{
    return (uint8_t *)p_name;
}

static inline uint8_t ait_desc_app_name_get_textlength(const uint8_t *p_name)
{
    return p_name[3];
}

static inline uint8_t *ait_desc_app_name_get_text(const uint8_t *p_name,
                                                  uint8_t *text_len)
{
    if (text_len)
        *text_len = p_name[3];
    return (uint8_t *)p_name + 4;
}

static inline uint8_t *ait_desc_app_name_next_name(const uint8_t *p_desc,
                                                   uint8_t *p_name)
{
    const uint8_t *p_end = p_desc + DESC_HEADER_SIZE + desc_get_length(p_desc);
    if (!p_name)
        p_name = (uint8_t *)p_desc + DESC_HEADER_SIZE;
    else
        p_name += 3 + ait_desc_app_name_get_textlength(p_name);
    return p_name + 3 > p_end ? NULL : p_name;
}

#define ait_desc_app_name_each_name(DESC, NAME) \
    for (uint8_t *NAME = ait_desc_app_name_next_name(DESC, NULL); \
         NAME != NULL; \
         NAME = ait_desc_app_name_next_name(DESC, NAME))

static inline bool ait_desc_app_name_validate(const uint8_t *p_desc)
{
    return desc_get_tag(p_desc) == AIT_DESC_APP_NAME;
}

#define AIT_DESC_TRANSPORT_PROTO    0x02

#define AIT_DESC_TRANSPORT_PROTO_SIZE   3

static inline uint16_t ait_desc_transport_proto_get_id(const uint8_t *p_desc)
{
    return p_desc[2] << 8 | p_desc[3];
}

static inline uint8_t ait_desc_transport_proto_get_label(const uint8_t *p_desc)
{
    return p_desc[4];
}

static inline bool ait_desc_transport_proto_validate(const uint8_t *p_desc)
{
    uint8_t tag = desc_get_tag(p_desc);
    uint8_t length = desc_get_length(p_desc);

    return tag == AIT_DESC_TRANSPORT_PROTO &&
        length >= AIT_DESC_TRANSPORT_PROTO_SIZE;
}


#define AIT_DESC_EXTERNAL_APP_AUTH  0x05
#define AIT_DESC_APP_RECORD         0x06
#define AIT_DESC_APP_ICON           0x0B
#define AIT_DESC_APP_STORAGE        0x10
#define AIT_DESC_GRAPH_CONSTRAINS   0x14
#define AIT_DESC_APP_LOCATION       0x15

static inline uint8_t *
ait_desc_app_location_get_initial_path(const uint8_t *p_desc, uint8_t *length)
{
    if (length)
        *length = p_desc[1];
    return (uint8_t *)p_desc + 2;
}

static inline bool ait_desc_app_location_validate(const uint8_t *p_desc)
{
    return desc_get_tag(p_desc) == AIT_DESC_APP_LOCATION;
}

#define AIT_DESC_APP_USAGE          0x16
#define AIT_DESC_APP_BOUND          0x17
#define AIT_DESC_PRIVATE_DATA       0x5F


/*****************************************************************************
 * Application Information Table
 *****************************************************************************/
#define AIT_TABLE_ID            0x74
#define AIT_HEADER_SIZE         (PSI_HEADER_SIZE_SYNTAX1 + 4)
#define AIT_APP_IDENTIFIER_SIZE 6
#define AIT_APP_SIZE            (AIT_APP_IDENTIFIER_SIZE + 3)

static inline void ait_init(uint8_t *p_ait)
{
    psi_init(p_ait, true);
    psi_set_tableid(p_ait, AIT_TABLE_ID);
    p_ait[8] = 0xf0;
}

static inline void ait_set_length(uint8_t *p_ait, uint16_t i_ait_length)
{
    psi_set_length(p_ait, AIT_HEADER_SIZE + PSI_CRC_SIZE - PSI_HEADER_SIZE
                    + i_ait_length);
}

static inline void ait_set_desclength(uint8_t *p_ait, uint16_t i_length)
{
    p_ait[8] &= ~0xf;
    p_ait[8] |= i_length >> 8;
    p_ait[9] = i_length & 0xff;
}

static inline uint16_t ait_get_desclength(const uint8_t *p_ait)
{
    return ((p_ait[8] & 0xf) << 8) | p_ait[9];
}

static inline uint8_t *ait_get_descs(uint8_t *p_ait)
{
    return &p_ait[8];
}

static inline uint32_t aitn_get_organisation_id(const uint8_t *p_aitn)
{
    return p_aitn[0] << 24 | p_aitn[1] << 16 | p_aitn[2] << 8 | p_aitn[3];
}

static inline uint16_t aitn_get_application_id(const uint8_t *p_aitn)
{
    return p_aitn[4] << 8 | p_aitn[5];
}

#define AITN_CONTROL_CODE_AUTOSTART             0x01
#define AITN_CONTROL_CODE_PRESENT               0x02
#define AITN_CONTROL_CODE_DESTROY               0x03
#define AITN_CONTROL_CODE_KILL                  0x04
#define AITN_CONTROL_CODE_PREFETCH              0x05
#define AITN_CONTROL_CODE_REMOTE                0x06
#define AITN_CONTROL_CODE_DISABLED              0x07
#define AITN_CONTROL_CODE_PLAYBACK_AUTOSTART    0x08

static inline const char *aitn_control_code_txt(uint8_t i_control_code)
{
    switch (i_control_code) {
        case AITN_CONTROL_CODE_AUTOSTART: return "autostart";
        case AITN_CONTROL_CODE_PRESENT: return "present";
        case AITN_CONTROL_CODE_DESTROY: return "destroy";
        case AITN_CONTROL_CODE_KILL: return "kill";
        case AITN_CONTROL_CODE_PREFETCH: return "prefetch";
        case AITN_CONTROL_CODE_REMOTE: return "remote";
        case AITN_CONTROL_CODE_DISABLED: return "disabled";
        case AITN_CONTROL_CODE_PLAYBACK_AUTOSTART: return "playback autostart";
        default:
            return "Reserved";
    }
}

static inline uint8_t aitn_get_control_code(const uint8_t *p_aitn)
{
    return p_aitn[6];
}

static inline void aitn_set_desclength(uint8_t *p_aitn, uint16_t i_length)
{
    p_aitn[7] &= ~0xf;
    p_aitn[7] |= i_length >> 8;
    p_aitn[8] = i_length & 0xff;
}

static inline uint16_t aitn_get_desclength(const uint8_t *p_aitn)
{
    return ((p_aitn[7] & 0xf) << 8) | p_aitn[8];
}

static inline uint8_t *aitn_get_descs(uint8_t *p_aitn)
{
    return &p_aitn[7];
}

#define aitn_each_desc(AITN, DESC)  descs_each_desc(aitn_get_descs(AITN), DESC)

static inline uint8_t *ait_next_app(const uint8_t *p_ait, uint8_t *p_ait_n)
{
    uint16_t i_section_size =
        psi_get_length(p_ait) + PSI_HEADER_SIZE - PSI_CRC_SIZE;
    if (!p_ait_n)
        p_ait_n = (uint8_t *)p_ait + AIT_HEADER_SIZE +
            ait_get_desclength(p_ait);
    else {
        if (p_ait_n + AIT_APP_SIZE - p_ait > i_section_size) return NULL;
        p_ait_n += AIT_APP_SIZE + aitn_get_desclength(p_ait_n);
    }
    return (p_ait_n - p_ait < i_section_size) ? p_ait_n : NULL;
}

#define ait_each_app(AIT, APP) \
    for (uint8_t *APP = ait_next_app(AIT, NULL); \
         APP; \
         APP = ait_next_app(AIT, APP))

static inline uint8_t *ait_get_apps(uint8_t *p_ait)
{
    uint16_t desclength = ait_get_desclength(p_ait);
    return ait_get_descs(p_ait) + 2 + desclength;
}

static inline void ait_set_applength(uint8_t *p_ait, uint16_t i_length)
{
    uint8_t *p_apps = ait_get_apps(p_ait);
    p_apps[0] &= ~0xf;
    p_apps[0] |= i_length >> 8;
    p_apps[1] = i_length & 0xff;
}

static inline uint16_t ait_get_appslength(const uint8_t *p_ait)
{
    const uint8_t *p_apps = ait_get_apps((uint8_t *)p_ait);
    return ((p_apps[0] & 0xf) << 8) | p_apps[1];
}

static inline bool ait_validate(const uint8_t *p_ait)
{
    uint16_t i_section_size =
        psi_get_length(p_ait) + PSI_HEADER_SIZE - PSI_CRC_SIZE;

    if (!psi_get_syntax(p_ait) || psi_get_tableid(p_ait) != AIT_TABLE_ID)
        return false;

    if (i_section_size < AIT_HEADER_SIZE
         || i_section_size < AIT_HEADER_SIZE + ait_get_desclength(p_ait))
        return false;

    if (!descs_validate(p_ait + 8))
        return false;

    ait_each_app(p_ait, p_app) {
        if (!descs_validate(p_app + 7))
            return false;
    }
    return true;
}

static inline bool ait_table_validate(uint8_t **pp_sections)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);

        if (!psi_check_crc(p_section))
            return false;
    }

    return true;
}

#ifdef __cplusplus
}
#endif

#endif
