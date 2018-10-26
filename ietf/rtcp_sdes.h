#ifndef __BITSTREAM_IETF_RTCP_SDES_H__
# define __BITSTREAM_IETF_RTCP_SDES_H__

# include <inttypes.h>
# include <bitstream/ietf/rtcp.h>

# define RTCP_SDES_SIZE 10

# define RTCP_PT_SDES 202

static inline void rtcp_sdes_set_pt(uint8_t *p_rtcp_rr)
{
    rtcp_set_pt(p_rtcp_rr, RTCP_PT_SDES);
}

static inline uint8_t rtcp_sdes_get_cname(const uint8_t *p_rtcp_sdes)
{
    return p_rtcp_sdes[8];
}

static inline void rtcp_sdes_set_cname(uint8_t *p_rtcp_sdes, uint8_t cname)
{
    p_rtcp_sdes[8] = cname;
}

static inline int8_t rtcp_sdes_get_name_length(const uint8_t *p_rtcp_sdes)
{
    return p_rtcp_sdes[9];
}

static inline void rtcp_sdes_set_name_length(uint8_t *p_rtcp_sdes,
        int8_t name_length)
{
    p_rtcp_sdes[9] = name_length;
}

#endif /* !__BITSTREAM_IETF_RTCP_SDES_H__ */
