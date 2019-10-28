/*****************************************************************************
 * desc_3f.h: ISO/IEC 13818-1 Descriptor 0x2a (HEVC timing and HRD descriptor)
 *****************************************************************************/

#ifndef __BITSTREAM_MPEG_DESC_3F_H__
#define __BITSTREAM_MPEG_DESC_3F_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x3f (HEVC timing and HRD descriptor)
 *****************************************************************************/
#define DESC3F_HEADER_SIZE      (DESC_HEADER_SIZE + 1 + 1)
#define DESC3F_HEADER_SIZE2     (DESC3F_HEADER_SIZE + 5)
#define DESC3F_HEADER_SIZE3     (DESC3F_HEADER_SIZE + 5 + 8)

static inline void desc3f_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x3f);
    p_desc[2] = 0x03;   // Extention descriptor tag
    desc_set_length(p_desc, DESC3F_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline bool desc3f_get_hrd_management_valid_flag(const uint8_t *p_desc)
{
    return (p_desc[3] & 0x80) == 0x80;
}

static inline void desc3f_set_hrd_management_valid_flag(uint8_t *p_desc, bool hrd_management_valid_flag)
{
    p_desc[3] = hrd_management_valid_flag ? (p_desc[3] | 0x80) : (p_desc[3] & ~0x80);
}

static inline bool desc3f_get_target_schedule_idx_not_present_flag(const uint8_t *p_desc)
{
    return (p_desc[3] & 0x40) == 0x40;
}

static inline void desc3f_set_target_schedule_idx_not_present_flag(uint8_t *p_desc, bool target_schedule_idx_not_present_flag)
{
    p_desc[3] = target_schedule_idx_not_present_flag ? p_desc[3] | 0x40 : p_desc[3] & ~0x40;
}

static inline uint8_t desc3f_get_target_schedule_idx(const uint8_t *p_desc)
{
    return (p_desc[3] >> 1) & 0x1f;
}

static inline void desc3f_set_target_schedule_idx(uint8_t *p_desc, uint8_t target_schedule_idx)
{
    p_desc[3] |= (target_schedule_idx << 1);
}

static inline bool desc3f_get_picture_and_timing_info_present_flag(const uint8_t *p_desc)
{
    return (p_desc[3] & 0x01) == 0x01;
}

static inline void desc3f_set_picture_and_timing_info_present_flag(uint8_t *p_desc, bool picture_and_timing_info_present_flag)
{
    if (picture_and_timing_info_present_flag)
    {
        desc_set_length(p_desc, DESC3F_HEADER_SIZE2 - DESC_HEADER_SIZE);
        p_desc[3] |= 0x01;
    }
    else
    {
        desc_set_length(p_desc, DESC3F_HEADER_SIZE - DESC_HEADER_SIZE);
        p_desc[3] &= ~0x01;
    }
}

static inline bool desc3f_get_90khz_flag(const uint8_t *p_desc)
{
    if (desc3f_get_picture_and_timing_info_present_flag(p_desc))
        return (p_desc[4] & 0x80) == 0x80;
    return false;
}

static inline void desc3f_set_90khz_flag(uint8_t *p_desc, bool b_90kHz_flag)
{
    if (!desc3f_get_picture_and_timing_info_present_flag(p_desc))
        return;
    if (b_90kHz_flag)
    {
        p_desc[4] = 0xff;
        desc_set_length(p_desc, DESC3F_HEADER_SIZE2 - DESC_HEADER_SIZE);
    }
    else
    {
        p_desc[4] = 0x7f;
        desc_set_length(p_desc, DESC3F_HEADER_SIZE3 - DESC_HEADER_SIZE);
    }
}

static inline uint32_t desc3f_get_N(const uint8_t *p_desc)
{
    if (!desc3f_get_picture_and_timing_info_present_flag(p_desc))
        return 0;
    if (desc3f_get_90khz_flag(p_desc))
        return 0;
   return (p_desc[5] << 24) | (p_desc[6] << 16) | (p_desc[7] << 8) | p_desc[8];
}

static inline void desc3f_set_N(uint8_t *p_desc, uint32_t N)
{
    if (!desc3f_get_picture_and_timing_info_present_flag(p_desc))
        return;
    if (desc3f_get_90khz_flag(p_desc))
        return;
    p_desc[5] = (N >> 24) & 0xff;
    p_desc[6] = (N >> 16) & 0xff;
    p_desc[7] = (N >>  8) & 0xff;
    p_desc[8] =  N        & 0xff;
}

static inline uint32_t desc3f_get_K(const uint8_t *p_desc)
{
    if (!desc3f_get_picture_and_timing_info_present_flag(p_desc))
        return UINT32_MAX;
    if (desc3f_get_90khz_flag(p_desc))
        return UINT32_MAX;
   return (p_desc[9] << 24) | (p_desc[10] << 16) | (p_desc[11] << 8) | p_desc[12];
}

static inline void desc3f_set_K(uint8_t *p_desc, uint32_t K)
{
    if (!desc3f_get_picture_and_timing_info_present_flag(p_desc))
        return;
    if (desc3f_get_90khz_flag(p_desc))
        return;
    p_desc[ 9] = (K >> 24) & 0xff;
    p_desc[10] = (K >> 16) & 0xff;
    p_desc[11] = (K >>  8) & 0xff;
    p_desc[12] =  K        & 0xff;
}

static inline uint32_t desc3f_get_num_units_in_tick(const uint8_t *p_desc)
{
    if (!desc3f_get_picture_and_timing_info_present_flag(p_desc))
        return 0;
    uint8_t ofs = desc3f_get_90khz_flag(p_desc) == 0 ? 13 : 5;
    return (p_desc[ofs+0] << 24) | (p_desc[ofs+1] << 16) | (p_desc[ofs+2] << 8) | p_desc[ofs+3];
}

static inline void desc3f_set_num_units_in_tick(uint8_t *p_desc, uint32_t num_units_in_tick)
{
    if (!desc3f_get_picture_and_timing_info_present_flag(p_desc))
        return;
    uint8_t ofs = desc3f_get_90khz_flag(p_desc) == 0 ? 13 : 5;
    p_desc[ofs+0] = (num_units_in_tick >> 24) & 0xff;
    p_desc[ofs+1] = (num_units_in_tick >> 16) & 0xff;
    p_desc[ofs+2] = (num_units_in_tick >>  8) & 0xff;
    p_desc[ofs+3] =  num_units_in_tick        & 0xff;
}

static inline bool desc3f_validate(const uint8_t *p_desc)
{
    uint8_t desc3f_header_size = DESC3F_HEADER_SIZE;
    if (desc3f_get_picture_and_timing_info_present_flag(p_desc))
    {
        if (desc3f_get_90khz_flag(p_desc))
            desc3f_header_size = DESC3F_HEADER_SIZE3;
        else
            desc3f_header_size = DESC3F_HEADER_SIZE2;

    }
    return desc_get_length(p_desc) >= (desc3f_header_size - DESC_HEADER_SIZE);
}

static inline void desc3f_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t print_type)
{
    switch (print_type)
    {
        case PRINT_XML:
            pf_print(opaque,"<HEVC_TIMING_AND_HRD_DESC hrd_management_valid_flag=\"%u\""
                    " target_schedule_idx_not_present_flag=\"%u\""
                    " picture_and_timing_info_present_flag=\"%u\""
                    " flag_90khz=\"%u\" N=\"%u\" K=\"%u\" num_units_in_tick=\"%u\"/>"
                    ,
                    desc3f_get_hrd_management_valid_flag(p_desc),
                    desc3f_get_target_schedule_idx_not_present_flag(p_desc),
                    desc3f_get_picture_and_timing_info_present_flag(p_desc),
                    desc3f_get_90khz_flag(p_desc),
                    desc3f_get_N(p_desc),
                    desc3f_get_K(p_desc),
                    desc3f_get_num_units_in_tick(p_desc)
                    );
            break;
        default:
            pf_print(opaque,"    - desc 2a hevc_timing_and_hrd hrd_management_valid_flag=%u"
                    "target_schedule_idx_not_present_flag=%u"
                    " picture_and_timing_info_present_flag=%u"
                    " flag_90khz=%u N=%u K=%u num_units_in_tick=%u"
                    ,
                    desc3f_get_hrd_management_valid_flag(p_desc),
                    desc3f_get_target_schedule_idx_not_present_flag(p_desc),
                    desc3f_get_picture_and_timing_info_present_flag(p_desc),
                    desc3f_get_90khz_flag(p_desc),
                    desc3f_get_N(p_desc),
                    desc3f_get_K(p_desc),
                    desc3f_get_num_units_in_tick(p_desc)
                    );
    }
}

#ifdef __cplusplus
}
#endif

#endif
