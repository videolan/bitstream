/*****************************************************************************
 * desc_6e.h: ETSI EN 300 468 Descriptor 0x6e: Announcement support descriptor
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

#ifndef __BITSTREAM_DVB_DESC_6E_H__
#define __BITSTREAM_DVB_DESC_6E_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x6e: Announcement support descriptor
 *****************************************************************************/
#define DESC6E_HEADER_SIZE      (DESC_HEADER_SIZE + 2)
#define DESC6E_DATA_SIZE        1
#define DESC6E_EXT_DATA_SIZE    7

static inline void desc6e_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x6e);
    desc_set_length(p_desc, DESC6E_HEADER_SIZE - DESC_HEADER_SIZE);
    p_desc[2] = 0xff;
}

static inline uint16_t desc6e_get_announcement_support_indicator(const uint8_t *p_desc)
{
    return (p_desc[2] << 8) | p_desc[3];
}

static inline void desc6e_set_announcement_support_indicator(uint8_t *p_desc, uint16_t i_announcement_support_indicator)
{
    p_desc[2] = (i_announcement_support_indicator >> 8) & 0xff;
    p_desc[3] =  i_announcement_support_indicator       & 0xff;
}

#define __DEFINE_FLAG(FLAGNAME, bit) \
    static inline bool desc6e_get_##FLAGNAME##_flag(const uint8_t *p_desc) \
    { \
        return (p_desc[3] & bit) == bit; \
    } \
    \
    static inline void desc6e_set_##FLAGNAME##_flag(uint8_t *p_desc, bool b_##FLAGNAME) \
    { \
        p_desc[3] = b_##FLAGNAME ? (p_desc[3] | bit) : (p_desc[3] &~ bit); \
    }

__DEFINE_FLAG (emergency_alarm       , 0x01)
__DEFINE_FLAG (road_traffic_flash    , 0x02)
__DEFINE_FLAG (public_transport_flash, 0x04)
__DEFINE_FLAG (warning_message       , 0x08)
__DEFINE_FLAG (news_flash            , 0x10)
__DEFINE_FLAG (weather_flash         , 0x20)
__DEFINE_FLAG (event_announcement    , 0x40)
__DEFINE_FLAG (personal_call         , 0x80)

#undef __DEFINE_FLAG

static inline uint8_t desc6en_get_announcement_type(const uint8_t *p_desc_n)
{
    return (p_desc_n[0] & 0xf0) >> 4;
}

static inline void desc6en_set_announcement_type(uint8_t *p_desc_n, uint8_t i_announcement_type)
{
    p_desc_n[0] = ((i_announcement_type & 0x0f) << 4) | 0x08 | (p_desc_n[0] & 0x07);
}

static inline const char *desc6e_get_announcement_type_txt(uint8_t i_announcement_type)
{
    return i_announcement_type == 0x00 ? "Emergency alarm" :
           i_announcement_type == 0x01 ? "Road Traffic flash" :
           i_announcement_type == 0x02 ? "Public Transport flash" :
           i_announcement_type == 0x03 ? "Warning message" :
           i_announcement_type == 0x04 ? "News flash" :
           i_announcement_type == 0x05 ? "Weather flash" :
           i_announcement_type == 0x06 ? "Event announcement" :
           i_announcement_type == 0x07 ? "Personal call" : "Reserved";
}

static inline uint8_t desc6en_get_reference_type(const uint8_t *p_desc_n)
{
    return p_desc_n[0] & 0x07;
}

static inline void desc6en_set_reference_type(uint8_t *p_desc_n, uint8_t i_reference_type)
{
    p_desc_n[0] = (p_desc_n[0] & 0xf0) | 0x08 | (i_reference_type & 0x07);
}

static inline uint16_t desc6en_get_onid(const uint8_t *p_desc_n)
{
    uint8_t i_ref_type = desc6en_get_reference_type(p_desc_n);
    if (!(i_ref_type == 0x01 || i_ref_type == 0x02 || i_ref_type == 0x03))
        return 0;
    return (p_desc_n[1] << 8) | p_desc_n[2];
}

static inline void desc6en_set_onid(uint8_t *p_desc_n, uint16_t i_onid)
{
    uint8_t i_ref_type = desc6en_get_reference_type(p_desc_n);
    if (!(i_ref_type == 0x01 || i_ref_type == 0x02 || i_ref_type == 0x03))
        return;
    p_desc_n[1] = (i_onid >> 8) & 0xff;
    p_desc_n[2] =  i_onid       & 0xff;
}

static inline uint16_t desc6en_get_tsid(const uint8_t *p_desc_n)
{
    uint8_t i_ref_type = desc6en_get_reference_type(p_desc_n);
    if (!(i_ref_type == 0x01 || i_ref_type == 0x02 || i_ref_type == 0x03))
        return 0;
    return (p_desc_n[3] << 8) | p_desc_n[4];
}

static inline void desc6en_set_tsid(uint8_t *p_desc_n, uint16_t i_tsid)
{
    uint8_t i_ref_type = desc6en_get_reference_type(p_desc_n);
    if (!(i_ref_type == 0x01 || i_ref_type == 0x02 || i_ref_type == 0x03))
        return;
    p_desc_n[3] = (i_tsid >> 8) & 0xff;
    p_desc_n[4] =  i_tsid       & 0xff;
}

static inline uint16_t desc6en_get_service_id(const uint8_t *p_desc_n)
{
    uint8_t i_ref_type = desc6en_get_reference_type(p_desc_n);
    if (!(i_ref_type == 0x01 || i_ref_type == 0x02 || i_ref_type == 0x03))
        return 0;
    return (p_desc_n[5] << 8) | p_desc_n[6];
}

static inline void desc6en_set_service_id(uint8_t *p_desc_n, uint16_t i_service_id)
{
    uint8_t i_ref_type = desc6en_get_reference_type(p_desc_n);
    if (!(i_ref_type == 0x01 || i_ref_type == 0x02 || i_ref_type == 0x03))
        return;
    p_desc_n[5] = (i_service_id >> 8) & 0xff;
    p_desc_n[6] =  i_service_id       & 0xff;
}

static inline uint8_t desc6en_get_component_tag(const uint8_t *p_desc_n)
{
    uint8_t i_ref_type = desc6en_get_reference_type(p_desc_n);
    if (!(i_ref_type == 0x01 || i_ref_type == 0x02 || i_ref_type == 0x03))
        return 0;
    return p_desc_n[7];
}

static inline void desc6en_set_component_tag(uint8_t *p_desc_n, uint8_t i_component_tag)
{
    uint8_t i_ref_type = desc6en_get_reference_type(p_desc_n);
    if (!(i_ref_type == 0x01 || i_ref_type == 0x02 || i_ref_type == 0x03))
        return;
    p_desc_n[7] = i_component_tag;
}

static inline uint8_t *desc6e_get_announcement(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC6E_HEADER_SIZE;
    int i_loop_len = desc_get_length(p_desc) - (DESC6E_HEADER_SIZE - DESC_HEADER_SIZE);

    if (i_loop_len <= 0)
        return NULL;

    while (n) {
        uint8_t i_desc6e_data_size = DESC6E_DATA_SIZE;

        uint8_t i_ref_type = desc6en_get_reference_type(p_desc_n);
        if (i_ref_type == 0x01 || i_ref_type == 0x02 || i_ref_type == 0x03)
            i_desc6e_data_size += DESC6E_EXT_DATA_SIZE;

        p_desc_n   += i_desc6e_data_size;
        i_loop_len -= i_desc6e_data_size;
        if (i_loop_len <= 0)
            return NULL;
        n--;
    }

    return p_desc_n;
}

static inline bool desc6e_validate(const uint8_t *p_desc)
{
    const uint8_t *p_desc_n = p_desc + DESC6E_HEADER_SIZE;
    int i_desc_size = desc_get_length(p_desc);
    while (i_desc_size > (DESC6E_HEADER_SIZE - DESC_HEADER_SIZE)) {
        uint8_t i_desc6e_data_size = DESC6E_DATA_SIZE;
        uint8_t i_ref_type = desc6en_get_reference_type(p_desc_n);
        if (i_ref_type == 0x01 || i_ref_type == 0x02 || i_ref_type == 0x03)
            i_desc6e_data_size += DESC6E_EXT_DATA_SIZE;
        i_desc_size -= i_desc6e_data_size;
        p_desc_n    += i_desc6e_data_size;
    }
    return i_desc_size == (DESC6E_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline void desc6e_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t n = 0;
    uint8_t *p_desc_n;

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
            "<ANNOUNCEMENT_SUPPORT_DESC announcement_support_indicator=\"0x%04x\""
            " emergency_alarm_flag=\"%u\""
            " road_traffic_flash_flag=\"%u\""
            " public_transport_flash_flag=\"%u\""
            " warning_message_flag=\"%u\""
            " news_flash_flag=\"%u\""
            " weather_flash_flag=\"%u\""
            " event_announcement_flag=\"%u\""
            " personal_call_flag=\"%u\">",
            desc6e_get_announcement_support_indicator(p_desc),
            desc6e_get_emergency_alarm_flag(p_desc),
            desc6e_get_road_traffic_flash_flag(p_desc),
            desc6e_get_public_transport_flash_flag(p_desc),
            desc6e_get_warning_message_flag(p_desc),
            desc6e_get_news_flash_flag(p_desc),
            desc6e_get_weather_flash_flag(p_desc),
            desc6e_get_event_announcement_flag(p_desc),
            desc6e_get_personal_call_flag(p_desc)
        );
        break;
    default:
        pf_print(opaque,
            "    - desc 6e announcement_support announcement_support_indicator=0x%04x"
            " emergency_alarm_flag=%u"
            " road_traffic_flash_flag=%u"
            " public_transport_flash_flag=%u"
            " warning_message_flag=%u"
            " news_flash_flag=%u"
            " weather_flash_flag=%u"
            " event_announcement_flag=%u"
            " personal_call_flag=%u",
            desc6e_get_announcement_support_indicator(p_desc),
            desc6e_get_emergency_alarm_flag(p_desc),
            desc6e_get_road_traffic_flash_flag(p_desc),
            desc6e_get_public_transport_flash_flag(p_desc),
            desc6e_get_warning_message_flag(p_desc),
            desc6e_get_news_flash_flag(p_desc),
            desc6e_get_weather_flash_flag(p_desc),
            desc6e_get_event_announcement_flag(p_desc),
            desc6e_get_personal_call_flag(p_desc)
        );
    }

    while ((p_desc_n = desc6e_get_announcement(p_desc, n++)) != NULL) {
        uint8_t i_announcement_type = desc6en_get_announcement_type(p_desc_n);
        uint8_t i_reference_type = desc6en_get_reference_type(p_desc_n);
        bool b_extra_data = (i_reference_type == 0x01 || i_reference_type == 0x02 || i_reference_type == 0x03);
        switch (i_print_type) {
        case PRINT_XML:
            if (!b_extra_data)
                pf_print(opaque,
                    "<ANNOUNCEMENT type=\"%u\" type_txt=\"%s\" reference_type=\"%u\"/>",
                    i_announcement_type,
                    desc6e_get_announcement_type_txt(i_announcement_type),
                    i_reference_type
                );
            else
                pf_print(opaque,
                    "<ANNOUNCEMENT type=\"%u\" type_txt=\"%s\" reference_type=\"%u\""
                    " onid=\"%u\" tsid=\"%u\" service_id=\"%u\" component_tag=\"%u\"/>",
                    i_announcement_type,
                    desc6e_get_announcement_type_txt(i_announcement_type),
                    i_reference_type,
                    desc6en_get_onid(p_desc_n),
                    desc6en_get_tsid(p_desc_n),
                    desc6en_get_service_id(p_desc_n),
                    desc6en_get_component_tag(p_desc_n)
                );
            break;
        default:
            if (!b_extra_data)
                pf_print(opaque,
                    "        - announcement type=%u type_txt=\"%s\" reference_type=%u",
                    i_announcement_type,
                    desc6e_get_announcement_type_txt(i_announcement_type),
                    i_reference_type
                );
            else
                pf_print(opaque,
                    "        - announcement type=%u type_txt=\"%s\" reference_type=%u"
                    " onid=%u tsid=%u service_id=%u component_tag=%u",
                    i_announcement_type,
                    desc6e_get_announcement_type_txt(i_announcement_type),
                    i_reference_type,
                    desc6en_get_onid(p_desc_n),
                    desc6en_get_tsid(p_desc_n),
                    desc6en_get_service_id(p_desc_n),
                    desc6en_get_component_tag(p_desc_n)
                );
        }
    }

    if (i_print_type == PRINT_XML)
        pf_print(opaque, "</ANNOUNCEMENT_SUPPORT_DESC>");
}

#ifdef __cplusplus
}
#endif

#endif
