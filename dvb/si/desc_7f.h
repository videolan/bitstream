/*****************************************************************************
 * desc_7f.h: ETSI EN 300 468 Descriptor 0x7f: Extension descriptor
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
 *  - ETSI EN 300 468 V1.12.1 (2011-06) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_DESC_7F_H__
#define __BITSTREAM_DVB_DESC_7F_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x7f: Extension descriptor
 *****************************************************************************/
#define DESC7F_HEADER_SIZE      (DESC_HEADER_SIZE + 1)

static inline void desc7f_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x7f);
    desc_set_length(p_desc, (DESC7F_HEADER_SIZE - DESC_HEADER_SIZE));
}

#define DESC7F_TAG_IMAGE_ICON                       0x00
#define DESC7F_TAG_CPCM_DELIVERY_SIGNALING          0x01
#define DESC7F_TAG_CP                               0x02
#define DESC7F_TAG_CP_IDENTIFIER                    0x03
#define DESC7F_TAG_T2_DELIVERY_SYSTEM               0x04
#define DESC7F_TAG_SH_DELIVERY_SYSTEM               0x05
#define DESC7F_TAG_SUPPLEMENTARY_AUDIO              0x06
#define DESC7F_TAG_NETWORK_CHANGE_NOTIFY            0x07
#define DESC7F_TAG_MESSAGE                          0x08
#define DESC7F_TAG_TARGET_REGION                    0x09
#define DESC7F_TAG_TARGET_REGION_NAME               0x0A
#define DESC7F_TAG_SERVICE_RELOCATED                0x0B
#define DESC7F_TAG_XAIT_PID                         0x0C
#define DESC7F_TAG_C2_DELIVERY_SYSTEM               0x0D
#define DESC7F_TAG_DTS_HD                           0x0E
#define DESC7F_TAG_DTS_NEURAL                       0x0F
#define DESC7F_TAG_VIDEO_DEPTH_RANGE                0x10
#define DESC7F_TAG_T2MI                             0x11
#define DESC7F_TAG_URI_LINKAGE                      0x13
#define DESC7F_TAG_CI_ANCILLARY_DATA                0x14
#define DESC7F_TAG_AC4                              0x15
#define DESC7F_TAG_C2_BUNDLE_DELIVERY_SYSTEM        0x16
#define DESC7F_TAG_S2X_SATELLITE_DELIVERY_SYSTEM    0x17
#define DESC7F_TAG_PROTECTION_MESSAGE               0x18
#define DESC7F_TAG_AUDIO_PRESELECTION               0x19
#define DESC7F_TAG_TTML_SUBTITLING                  0x20

static inline const char *desc7f_tag_extension_str(uint8_t tag)
{
    switch (tag) {
        case DESC7F_TAG_IMAGE_ICON:
            return "image_icon_descriptor";
        case DESC7F_TAG_CPCM_DELIVERY_SIGNALING:
            return "cpcm_delivery_signaling_descriptor";
        case DESC7F_TAG_CP:
            return "CP_descriptor";
        case DESC7F_TAG_CP_IDENTIFIER:
            return "CP_identifier_descriptor";
        case DESC7F_TAG_T2_DELIVERY_SYSTEM:
            return "T2_delivery_system_descriptor";
        case DESC7F_TAG_SH_DELIVERY_SYSTEM:
            return "SH_delivery_system_descriptor";
        case DESC7F_TAG_SUPPLEMENTARY_AUDIO:
            return "supplementary_audio_descriptor";
        case DESC7F_TAG_NETWORK_CHANGE_NOTIFY:
            return "network_change_notify_descriptor";
        case DESC7F_TAG_MESSAGE:
            return "message_descriptor";
        case DESC7F_TAG_TARGET_REGION:
            return "target_region_descriptor";
        case DESC7F_TAG_TARGET_REGION_NAME:
            return "target_region_name_descriptor";
        case DESC7F_TAG_SERVICE_RELOCATED:
            return "service_relocated_descriptor";
        case DESC7F_TAG_XAIT_PID:
            return "XAIT_PID_descriptor";
        case DESC7F_TAG_C2_DELIVERY_SYSTEM:
            return "C2_delivery_system_descriptor";
        case DESC7F_TAG_DTS_HD:
            return "DTS-HD_descriptor";
        case DESC7F_TAG_DTS_NEURAL:
            return "DTS_Neural_descriptor";
        case DESC7F_TAG_VIDEO_DEPTH_RANGE:
            return "video_depth_range_descriptor";
        case DESC7F_TAG_T2MI:
            return "T2MI_descriptor";
        case DESC7F_TAG_URI_LINKAGE:
            return "URI_linkage_descriptor";
        case DESC7F_TAG_CI_ANCILLARY_DATA:
            return "CI_ancillary_data_descriptor";
        case DESC7F_TAG_AC4:
            return "AC-4_descriptor";
        case DESC7F_TAG_C2_BUNDLE_DELIVERY_SYSTEM:
            return "C2_bundle_delivery_system_descriptor";
        case DESC7F_TAG_S2X_SATELLITE_DELIVERY_SYSTEM:
            return "S2X_satellite_delivery_system_descriptor";
        case DESC7F_TAG_PROTECTION_MESSAGE:
            return "protection_message_descriptor";
        case DESC7F_TAG_AUDIO_PRESELECTION:
            return "audio_preselection_descriptor";
        case DESC7F_TAG_TTML_SUBTITLING:
            return "TTML_subtitling_descriptor";
    }
    return "Unknown";
}

BITSTREAM_GET_SET(desc7f, tag_extension, uint8, 16, 8);

static inline bool desc7f_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC7F_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc7f_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t tag_extension = desc7f_get_tag_extension(p_desc);
    const char *tag_str = desc7f_tag_extension_str(tag_extension);

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<EXT_DESC tag=\"%04x\" name=\"%s\"/>",
                 0x7f00 + tag_extension, tag_str);
        break;
    default:
        pf_print(opaque, "    - desc %04x %s", 0x7f00 + tag_extension, tag_str);
    }
}

#ifdef __cplusplus
}
#endif

#endif
