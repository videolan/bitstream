/*****************************************************************************
 * desc_3f.h: ETSI EN 300 468 Descriptor 0x3f: Extension descriptor
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
 *  - ISO/IEC 13818-1:2021 (MPEG-2 Systems)
 */

#ifndef __BITSTREAM_MPEG_DESC_3F_H__
#define __BITSTREAM_MPEG_DESC_3F_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x3f: Extension descriptor
 *****************************************************************************/
#define DESC3F_HEADER_SIZE              (DESC_HEADER_SIZE + 1)

static inline void desc3f_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x3f);
    desc_set_length(p_desc, (DESC3F_HEADER_SIZE - DESC_HEADER_SIZE));
}

#define DESC3F_TAG_OBJECT_UPDATE                0x02
#define DESC3F_TAG_HEVC_TIMING_HRD              0x03
#define DESC3F_TAG_AF_EXTENSIONS                0x04
#define DESC3F_TAG_HEVC_OPERATION_POINT         0x05
#define DESC3F_TAG_HEVC_HIERARCHY               0x06
#define DESC3F_TAG_GREEN                        0x07
#define DESC3F_TAG_MPEG_H_3DAUDIO               0x08
#define DESC3F_TAG_MPEG_H_3DAUDIO_CONFIG        0x09
#define DESC3F_TAG_MPEG_H_3DAUDIO_SCENE         0x0A
#define DESC3F_TAG_MPEG_H_3DAUDIO_TEXT          0x0B
#define DESC3F_TAG_MPEG_H_3DAUDIO_MULTI_STREAM  0x0C
#define DESC3F_TAG_MPEG_H_3DAUDIO_DRC_LOUDNESS  0x0D
#define DESC3F_TAG_MPEG_H_3DAUDIO_COMMAND       0x0E
#define DESC3F_TAG_QUALITY                      0x0F
#define DESC3F_TAG_VIRTUAL_SEGMENTATION         0x10
#define DESC3F_TAG_TIMED_METADATA               0x11
#define DESC3F_TAG_HEVC_TILE_SUBSTREAM          0x12
#define DESC3F_TAG_HEVC_SUBREGION               0x13
#define DESC3F_TAG_JXS_VIDEO                    0x14
#define DESC3F_TAG_VVC_TIMING_HRD               0x15
#define DESC3F_TAG_EVC_TIMING_HRD               0x16

static inline const char *desc3f_tag_extension_str(uint8_t tag)
{
    switch (tag) {
        case DESC3F_TAG_OBJECT_UPDATE: return "object_update";
        case DESC3F_TAG_HEVC_TIMING_HRD: return "HEVC_timing_and_HRD";
        case DESC3F_TAG_AF_EXTENSIONS: return "af_extensions";
        case DESC3F_TAG_HEVC_OPERATION_POINT: return "HEVC_operation_point";
        case DESC3F_TAG_HEVC_HIERARCHY: return "HEVC_hierarchy";
        case DESC3F_TAG_GREEN: return "Green_extension";
        case DESC3F_TAG_MPEG_H_3DAUDIO: return "MPEG-H_3dAudio";
        case DESC3F_TAG_MPEG_H_3DAUDIO_CONFIG: return "MPEG-H_3dAudio_config";
        case DESC3F_TAG_MPEG_H_3DAUDIO_SCENE: return "MPEG-H_3dAudio_scene";
        case DESC3F_TAG_MPEG_H_3DAUDIO_TEXT: return "MPEG-H_3dAudio_text";
        case DESC3F_TAG_MPEG_H_3DAUDIO_MULTI_STREAM:
            return "MPEG-H_3dAudio_multi_stream";
        case DESC3F_TAG_MPEG_H_3DAUDIO_DRC_LOUDNESS:
            return "MPEG-H_3dAudio_drc_loudness";
        case DESC3F_TAG_MPEG_H_3DAUDIO_COMMAND: return "MPEG-H_3dAudio_command";
        case DESC3F_TAG_QUALITY: return "quality";
        case DESC3F_TAG_VIRTUAL_SEGMENTATION: return "virtual_segmentation";
        case DESC3F_TAG_TIMED_METADATA: return "timed_metadata";
        case DESC3F_TAG_HEVC_TILE_SUBSTREAM: return "HEVC_tile_substream";
        case DESC3F_TAG_HEVC_SUBREGION: return "HEVC_subregion";
        case DESC3F_TAG_JXS_VIDEO: return "JXS_video";
        case DESC3F_TAG_VVC_TIMING_HRD: return "VVC_timing_hrd";
        case DESC3F_TAG_EVC_TIMING_HRD: return "EVC_timing_hrd";
    }
    return "Unknown";
}

BITSTREAM_GET_SET(desc3f, tag_extension, uint8, 16, 8);

static inline bool desc3f_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC3F_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc3f_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t tag_extension = desc3f_get_tag_extension(p_desc);
    const char *tag_str = desc3f_tag_extension_str(tag_extension);

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<EXT_DESC tag=\"%04x\" name=\"%s\"/>",
                 0x3f00 + tag_extension, tag_str);
        break;

    default:
        pf_print(opaque, "    - desc %04x %s", 0x3f00 + tag_extension, tag_str);
    }
}

#ifdef __cplusplus
}
#endif

#endif
