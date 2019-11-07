/*****************************************************************************
 * au_info.h: ISO/IEC 13818-1 Access Unit Information
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
 *
 * Authors: Roberto Corno <corno.roberto@gmail.com>
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
 *  - ISO/IEC 13818-1:2007(E) (MPEG-2 systems)
 */

#ifndef __BITSTREAM_MPEG_AU_INFO_H__
#define __BITSTREAM_MPEG_AU_INFO_H__

#include <stdlib.h>   /* malloc */
#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */
#include <string.h>   /* memset */

#ifdef __cplusplus
extern "C"
{
#endif

#define ANNOUNCEMENT_SWITCHING  0x1
#define AU_INFORMATION          0x2
#define PVR_ASSIST_INFO         0x3

static inline void au_info_init(uint8_t *p_au_info) {
    p_au_info[3] = 0x0;
}

static inline void au_info_set_data_field_tag(uint8_t *p_au_info, uint8_t i_data_field_tag)
{
    p_au_info[0] = i_data_field_tag;
}

static inline uint8_t au_info_get_data_field_tag(const uint8_t *p_au_info)
{
    return p_au_info[0];
}

static inline void au_info_set_data_field_length(uint8_t *p_au_info, uint8_t i_data_field_length)
{
    p_au_info[1] = i_data_field_length;
}

static inline uint8_t au_info_get_data_field_length(const uint8_t *p_au_info)
{
    return p_au_info[1];
}

#define CF_UNDEFINED 0x0
#define CF_H262 0x1
#define CF_H264 0x2
#define CF_VC1  0x3
static inline void au_info_set_au_coding_format(uint8_t *p_au_info, uint8_t i_au_coding_format)
{
    p_au_info[2] &= ~0xF0;
    p_au_info[2] |= (i_au_coding_format & 0x0F) << 4;
}

static inline uint8_t au_info_get_au_coding_format(const uint8_t *p_au_info)
{
    return (p_au_info[2] & 0xF0) >> 4;
}

static inline void au_info_set_au_coding_type_info(uint8_t *p_au_info, uint8_t i_au_coding_type_info)
{
    p_au_info[2] &= ~0x0F;
    p_au_info[2] |= (i_au_coding_type_info & 0x0F);
}

static inline uint8_t au_info_get_au_coding_type_info(const uint8_t *p_au_info)
{
    return (p_au_info[2] & 0x0F);
}

static inline void au_info_set_au_ref_pic_idc(uint8_t *p_au_info, uint8_t i_au_ref_pic_idc)
{
    p_au_info[3] &= ~0xC0;
    p_au_info[3] |= (i_au_ref_pic_idc & 0x3) << 6;
}

static inline uint8_t au_info_get_au_ref_pic_idc(const uint8_t *p_au_info)
{
    return (p_au_info[3] & 0xC0) >> 6;
}

static inline void au_info_set_au_pic_struct(uint8_t *p_au_info, uint8_t i_au_pic_struct)
{
    p_au_info[3] &= ~0x30;
    p_au_info[3] |= (i_au_pic_struct & 0x3) << 4;
}

static inline uint8_t au_info_get_au_pic_struct(const uint8_t *p_au_info)
{
    return (p_au_info[3] & 0x30) >> 4;
}

static inline void au_info_set_pts_present(uint8_t *p_au_info)
{
    p_au_info[3] |= 0x8;
}

static inline bool au_info_get_pts_present(const uint8_t *p_au_info)
{
    return !!(p_au_info[3] & 0x8);
}

static inline void au_info_set_profile_info_present(uint8_t *p_au_info)
{
    p_au_info[3] |= 0x4;
}

static inline bool au_info_get_profile_info_present(const uint8_t *p_au_info)
{
    return !!(p_au_info[3] & 0x4);
}

static inline void au_info_set_stream_info_present(uint8_t *p_au_info)
{
    p_au_info[3] |= 0x2;
}

static inline bool au_info_get_stream_info_present(const uint8_t *p_au_info)
{
    return !!(p_au_info[3] & 0x2);
}

static inline void au_info_set_trickmode_info_present(uint8_t *p_au_info)
{
    p_au_info[3] |= 0x1;
}

static inline bool au_info_get_trickmode_info_present(const uint8_t *p_au_info)
{
    return !!(p_au_info[3] & 0x1);
}

static inline void au_info_set_pts(uint8_t *p_au_info, uint32_t i_pts)
{
    p_au_info[4] = (i_pts >> 24) & 0xff;
    p_au_info[5] = (i_pts >> 16) & 0xff;
    p_au_info[6] = (i_pts >>  8) & 0xff;
    p_au_info[7] =  i_pts        & 0xff;
}

static inline uint32_t au_info_get_pts(const uint8_t *p_au_info)
{
    return (p_au_info[4] << 24) | (p_au_info[5] << 16) | (p_au_info[6] << 8) | p_au_info[7];
}

static inline uint8_t* au_stream_info(uint8_t *p_au_info) {
    return p_au_info + 4 + (au_info_get_pts_present(p_au_info) ? 4 : 0);
}

static inline void au_info_set_frame_rate(uint8_t *p_au_info, uint8_t i_frame_rate)
{
    uint8_t *p_payload = au_stream_info(p_au_info);
    *p_payload = i_frame_rate & 0x0F;
}

static inline uint8_t au_info_get_frame_rate(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_stream_info(p_au_info);
    return *p_payload;
}

static inline uint8_t* au_profile_info(uint8_t *p_au_info) {
    return au_stream_info(p_au_info) + (au_info_get_stream_info_present(p_au_info) ? 1 : 0);
}

static inline void au_info_set_au_profile(uint8_t *p_au_info, uint8_t i_profile)
{
    uint8_t *p_payload = au_profile_info(p_au_info);
    *p_payload = i_profile;
}

static inline uint8_t au_info_get_au_profile(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_profile_info(p_au_info);
    return *p_payload;
}

static inline void au_info_set_au_contsraint_set0(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_profile_info(p_au_info);
    p_payload[1] |= 0x80;
}

static inline bool au_info_get_au_contsraint_set0(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_profile_info(p_au_info);
    return !!(p_payload[1] & 0x80);
}

static inline void au_info_set_au_contsraint_set1(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_profile_info(p_au_info);
    p_payload[1] |= 0x40;
}

static inline bool au_info_get_au_contsraint_set1(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_profile_info(p_au_info);
    return !!(p_payload[1] & 0x40);
}

static inline void au_info_set_au_contsraint_set2(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_profile_info(p_au_info);
    p_payload[1] |= 0x20;
}

static inline bool au_info_get_au_contsraint_set2(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_profile_info(p_au_info);
    return !!(p_payload[1] & 0x20);
}

static inline void au_info_set_au_avc_compatible_flags(uint8_t *p_au_info, uint8_t i_avc_flags)
{
    uint8_t *p_payload = au_profile_info(p_au_info);
    p_payload[1] |= i_avc_flags & 0x1F;
}

static inline uint8_t au_info_get_au_avc_compatible_flags(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_profile_info(p_au_info);
    return (p_payload[1] & 0x1F);
}

static inline void au_info_set_au_level(uint8_t *p_au_info, uint8_t i_au_level)
{
    uint8_t *p_payload = au_profile_info(p_au_info);
    p_payload[2] = i_au_level;
}

static inline uint8_t au_info_get_au_level(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_profile_info(p_au_info);
    return p_payload[2];
}

static inline uint8_t* au_trickmode_info(uint8_t *p_au_info) {
    return au_profile_info(p_au_info) + (au_info_get_profile_info_present(p_au_info) ? 3 : 0);
}

static inline void au_info_set_max_i_pic_size(uint8_t *p_au_info, uint16_t i_pic_size)
{
    uint8_t *p_payload = au_trickmode_info(p_au_info);
    p_payload[0] = (i_pic_size & 0xFFF) >> 4;
    p_payload[1] |= (i_pic_size & 0x0F);
}

static inline uint16_t au_info_get_max_i_pic_size(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_trickmode_info(p_au_info);
    return p_payload[0] << 4 | (p_payload[1] & 0xF0) >> 4;
}

static inline void au_info_set_nominal_i_period(uint8_t *p_au_info, uint8_t i_period)
{
    uint8_t *p_payload = au_trickmode_info(p_au_info);
    p_payload[1] |= (i_period & 0xF0) >> 4;
    p_payload[2] |= (i_period & 0x0F) << 4;
}

static inline uint8_t au_info_get_nominal_i_period(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_trickmode_info(p_au_info);
    return (p_payload[1] & 0X0F) << 4 | (p_payload[2] & 0xF0) >> 4;
}

static inline void au_info_set_max_i_period(uint8_t *p_au_info, uint8_t i_period)
{
    uint8_t *p_payload = au_trickmode_info(p_au_info);
    p_payload[2] |= (i_period & 0xF0) >> 4;
    p_payload[3] |= (i_period & 0x0F) << 4;
}

static inline uint8_t au_info_get_max_i_period(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_trickmode_info(p_au_info);
    return (p_payload[2] & 0X0F) << 4 | (p_payload[3] & 0xF0) >> 4;
}

static inline uint8_t* au_pulldown_info(uint8_t *p_au_info) {
    return au_trickmode_info(p_au_info) + (au_info_get_trickmode_info_present(p_au_info) ? 4 : 0);
}

static inline void au_info_set_pulldown_info_present(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_pulldown_info(p_au_info);
    p_payload[0] |= 0x80;
}

static inline bool au_info_get_au_pulldown_info_present(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_pulldown_info(p_au_info);
    return !!(p_payload[0] & 0x01);
}

static inline void au_info_set_extension_1_present(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_pulldown_info(p_au_info);
    p_payload[0] |= 0x01;
}

static inline bool au_info_get_au_extension_1_present(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_pulldown_info(p_au_info);
    return !!(p_payload[0] & 0x80);
}

static inline uint8_t* au_pulldown_payload_info(uint8_t *p_au_info) {
    return au_pulldown_info(p_au_info) + (au_info_get_au_pulldown_info_present(p_au_info) ? 1 : 0);
}

static inline void au_info_set_pulldown_info(uint8_t *p_au_info, uint8_t i_pulldown_info)
{
    uint8_t *p_payload = au_pulldown_payload_info(p_au_info);
    p_payload[0] = i_pulldown_info & 0x0F;
}

static inline uint8_t au_info_get_au_pulldown_info(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_pulldown_payload_info(p_au_info);
    return p_payload[0];
}

static inline uint8_t* au_extension_1(uint8_t *p_au_info) {
    return au_pulldown_payload_info(p_au_info) + (au_info_get_au_pulldown_info(p_au_info) ? 1 : 0);
}

static inline void au_info_set_au_reserved(uint8_t *p_au_info, uint8_t i_reserved)
{
    uint8_t *p_payload = au_extension_1(p_au_info);
    p_payload[0] = i_reserved;
}

static inline uint8_t au_info_get_au_reserved(uint8_t *p_au_info)
{
    uint8_t *p_payload = au_extension_1(p_au_info);
    return p_payload[0];
}

#ifdef __cplusplus
}
#endif

#endif
