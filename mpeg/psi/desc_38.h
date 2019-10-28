/*****************************************************************************
 * desc_38.h: Descriptor 0x38 (HEVC video descriptor)
 *****************************************************************************/

#ifndef __BITSTREAM_MPEG_DESC_38_H__
#define __BITSTREAM_MPEG_DESC_38_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x38 (HEVC video descriptor)
 *****************************************************************************/

#define DESC38_HEADER_SIZE (DESC_HEADER_SIZE + 13)
#define DESC38_HEADER_SIZE2 (DESC_HEADER_SIZE + 13 + 2)

static inline void desc38_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x38);
    desc_set_length(p_desc, DESC38_HEADER_SIZE - DESC_HEADER_SIZE);
    p_desc[7] = p_desc[8] = p_desc[9] = p_desc[10] = p_desc[11] = p_desc[12] = 0;
    p_desc[14] = 0;
}

static inline uint8_t desc38_get_profile_space(const uint8_t *p_desc)
{
    return (p_desc[2] >> 6);
}

static inline void desc38_set_profile_space(uint8_t *p_desc, uint8_t profile_space)
{
    p_desc[2] = (profile_space << 6) | (p_desc[2] & 0x3f);
}

static inline uint8_t desc38_get_tier_flag(const uint8_t *p_desc)
{
    return (p_desc[2] & 0x20) == 0x20;
}

static inline void desc38_set_tier_flag(uint8_t *p_desc, bool tier_flag)
{
    p_desc[2] = tier_flag ? (p_desc[2] | 0x20) : (p_desc[2] & ~0x20);
}

static inline uint8_t desc38_get_profile_idc(const uint8_t *p_desc)
{
    return (p_desc[2] & 0x1f);
}

static inline void desc38_set_profile_idc(uint8_t *p_desc, uint8_t profile_idc)
{
    p_desc[2] = (p_desc[2] & 0xe0) | profile_idc;
}

static inline uint32_t desc38_get_profile_compatibility_indication(const uint8_t *p_desc)
{
#if BYTE_ORDER == BIG_ENDIAN
    return (p_desc[3] | (p_desc[4] << 8) | (p_desc[5] << 16) | (p_desc[6] << 24));
#elif BYTE_ORDER == LITTLE_ENDIAN
    return (p_desc[3] << 24) | (p_desc[4] << 16) | (p_desc[5] << 8) | p_desc[6];
#else
    # error "What kind of system is this?"
#endif
}

static inline void desc38_set_profile_compatibility_indication(uint8_t *p_desc, uint32_t profile_compatibility_indication)
{
#if BYTE_ORDER == BIG_ENDIAN
    p_desc[3] = profile_compatibility_indication & 0xff;
    p_desc[4] = (profile_compatibility_indication >> 8) & 0xff;
    p_desc[5] = (profile_compatibility_indication >> 16) & 0xff;
    p_desc[6] = (profile_compatibility_indication >> 24) & 0xff;
#elif BYTE_ORDER == LITTLE_ENDIAN
    p_desc[3] = (profile_compatibility_indication >> 24) & 0xff;
    p_desc[4] = (profile_compatibility_indication >> 16) & 0xff;
    p_desc[5] = (profile_compatibility_indication >> 8) & 0xff;
    p_desc[6] = profile_compatibility_indication & 0xff;
#else
    # error "What kind of system is this?"
#endif
}

static inline bool desc38_get_progressive_source_flag(const uint8_t *p_desc)
{
    return (p_desc[7] & 0x80) == 0x80;
}

static inline void desc38_set_progressive_source_flag(uint8_t *p_desc, bool progressive_source_flag)
{
    p_desc[7] = progressive_source_flag ? (p_desc[7] | 0x80) : (p_desc[7] & ~0x80);
}

static inline bool desc38_get_interlaced_source_flag(const uint8_t *p_desc)
{
    return (p_desc[7] & 0x40) == 0x40;
}

static inline void desc38_set_interlaced_source_flag(uint8_t *p_desc, bool interlaced_source_flag)
{
    p_desc[7] = interlaced_source_flag ? (p_desc[7] | 0x40) : (p_desc[7] & ~0x40);
}

static inline bool desc38_get_non_packed_constraint_flag(const uint8_t *p_desc)
{
    return (p_desc[7] & 0x20) == 0x20;
}

static inline void desc38_set_non_packed_constraint_flag(uint8_t *p_desc, bool non_packed_constraint_flag)
{
    p_desc[7] = non_packed_constraint_flag ? (p_desc[7] | 0x20) : (p_desc[7] & ~0x20);
}

static inline bool desc38_get_frame_only_constraint_flag(const uint8_t *p_desc)
{
    return (p_desc[7] & 0x10) == 0x10;
}

static inline void desc38_set_frame_only_constraint_flag(uint8_t *p_desc, bool frame_only_constraint_flag)
{
    p_desc[7] = frame_only_constraint_flag ? (p_desc[7] | 0x10) : (p_desc[7] & ~0x10);
}

static inline uint8_t desc38_get_level_idc(const uint8_t *p_desc)
{
    return p_desc[13];
}

static inline void desc38_set_level_idc(uint8_t *p_desc, uint8_t level_idc)
{
    p_desc[13] = level_idc;
}

static inline bool desc38_get_temporal_layer_subset_flag(const uint8_t *p_desc)
{
    return (p_desc[14] & 0x80) == 0x80;
}

static inline void desc38_set_temporal_layer_subset_flag(uint8_t *p_desc, bool temporal_layer_subset_flag)
{
    if (temporal_layer_subset_flag)
    {
        desc_set_length(p_desc, DESC38_HEADER_SIZE2 - DESC_HEADER_SIZE);
        p_desc[14] |= 0x80;
    }
    else {
        desc_set_length(p_desc, DESC38_HEADER_SIZE - DESC_HEADER_SIZE);
        p_desc[14] &= ~0x80;
    }
}

static inline bool desc38_get_HEVC_still_present_flag(const uint8_t *p_desc)
{
    return (p_desc[14] & 0x40) == 0x40;
}

static inline void desc38_set_HEVC_still_present_flag(uint8_t *p_desc, bool HEVC_still_present_flag)
{
    p_desc[14] = HEVC_still_present_flag ? (p_desc[14] | 0x40) : (p_desc[14] & ~0x40);
}

static inline bool desc38_get_HEVC_24hr_picture_present_flag(const uint8_t *p_desc)
{
    return (p_desc[14] & 0x20) == 0x20;
}

static inline void desc38_set_HEVC_24hr_picture_present_flag(uint8_t *p_desc, bool HEVC_24hr_picture_present_flag)
{
    p_desc[14] = HEVC_24hr_picture_present_flag ? (p_desc[14] | 0x20) : (p_desc[14] & ~0x20);
}

static inline bool desc38_get_sub_pic_hrd_params_not_present_flag(const uint8_t *p_desc)
{
    return (p_desc[14] & 0x10) == 0x10;
}

static inline void desc38_set_sub_pic_hrd_params_not_present_flag(uint8_t *p_desc, bool sub_pic_hrd_params_not_present_flag)
{
    p_desc[14] = sub_pic_hrd_params_not_present_flag ? (p_desc[14] | 0x10) : (p_desc[14] & ~0x10);
}

static inline uint8_t desc38_get_temporal_id_min(const uint8_t *p_desc)
{
    if (desc38_get_temporal_layer_subset_flag(p_desc))
        return (p_desc[15] & 0xe0);
    return 0; 
}

static inline void desc38_set_temporal_id_min(uint8_t *p_desc, uint8_t temporal_id_min)
{
    desc38_set_temporal_layer_subset_flag(p_desc, true);
    p_desc[15] = (p_desc[15] & 0xe0) | temporal_id_min;
}

static inline uint8_t desc38_get_temporal_id_max(const uint8_t *p_desc)
{
    if (desc38_get_temporal_layer_subset_flag(p_desc))
        return (p_desc[16] & 0xe0);
    return 0; 
}

static inline void desc38_set_temporal_id_max(uint8_t *p_desc, uint8_t temporal_id_max)
{
    desc38_set_temporal_layer_subset_flag(p_desc, true);
    p_desc[16] = (p_desc[16] & 0xe0) | temporal_id_max;
}

static inline bool desc38_validate(const uint8_t *p_desc)
{
    uint8_t desc38_header_size = DESC38_HEADER_SIZE;
    if (desc38_get_temporal_layer_subset_flag(p_desc))
        desc38_header_size = DESC38_HEADER_SIZE2;
    return desc_get_length(p_desc) >= (desc38_header_size - DESC_HEADER_SIZE);
}

static inline void desc38_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<HEVC_VIDEO_DESC profile_space=\"0x%02x\" tier_flag=\"%u\" profile_idc=\"0x%02x\""
                         " profile_compatibility_indication=\"0x%02x\""
                         " progressive_source_flag=\"%u\" interlaced_source_flag=\"%u\""
                         " non_packed_constraint_flag=\"%u\" frame_only_constraint_flag=\"%u\""
                         " level_idc=\"0x%02x\" temporal_layer_subset_flag=\"%u\""
                         " HEVC_still_present_flag=\"%u\" HEVC_24hr_picture_present_flag=\"%u\""
                         " sub_pic_hrd_params_not_present_flag=\"%u\""
                         " temporal_id_min=\"0x%02x\" temporal_id_max=\"0x%02x\" "
                ,
                desc38_get_profile_space(p_desc),
                desc38_get_tier_flag(p_desc),
                desc38_get_profile_idc(p_desc),
                desc38_get_profile_compatibility_indication(p_desc),
                desc38_get_progressive_source_flag(p_desc),
                desc38_get_interlaced_source_flag(p_desc),
                desc38_get_non_packed_constraint_flag(p_desc),
                desc38_get_frame_only_constraint_flag(p_desc),
                desc38_get_level_idc(p_desc),
                desc38_get_temporal_layer_subset_flag(p_desc),
                desc38_get_HEVC_still_present_flag(p_desc),
                desc38_get_HEVC_24hr_picture_present_flag(p_desc),
                desc38_get_sub_pic_hrd_params_not_present_flag(p_desc),
                desc38_get_temporal_id_min(p_desc),
                desc38_get_temporal_id_max(p_desc)
                );
        break;
    default:
        pf_print(opaque,"    - desc 38 hevc_video profile_space=0x%02x tier_flag=%u profile_idc=0x%02x"
                " profile_compatibility_indication=0x%02x"
                " progressive_source_flag=%u interlaced_source_flag=%u"
                " non_packed_constraint_flag=%u frame_only_constraint_flag=%u"
                " level_idc=0x%02x temporal_layer_subset_flag=%u"
                " HEVC_still_present_flag=%u HEVC_24hr_picture_present_flag=%u"
                " sub_pic_hrd_params_not_present_flag=%u"
                " temporal_id_min=0x%02x temporal_id_max=0x%02x"
                ,
                desc38_get_profile_space(p_desc),
                desc38_get_tier_flag(p_desc),
                desc38_get_profile_idc(p_desc),
                desc38_get_profile_compatibility_indication(p_desc),
                desc38_get_progressive_source_flag(p_desc),
                desc38_get_interlaced_source_flag(p_desc),
                desc38_get_non_packed_constraint_flag(p_desc),
                desc38_get_frame_only_constraint_flag(p_desc),
                desc38_get_level_idc(p_desc),
                desc38_get_temporal_layer_subset_flag(p_desc),
                desc38_get_HEVC_still_present_flag(p_desc),
                desc38_get_HEVC_24hr_picture_present_flag(p_desc),
                desc38_get_sub_pic_hrd_params_not_present_flag(p_desc),
                desc38_get_temporal_id_min(p_desc),
                desc38_get_temporal_id_max(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
