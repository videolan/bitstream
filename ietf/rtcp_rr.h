#ifndef __BITSTREAM_IETF_RTCP_RR_H__
# define __BITSTREAM_IETF_RTCP_RR_H__

# include <inttypes.h>
# include <bitstream/ietf/rtcp.h>

# define RTCP_RR_SIZE 32

# define RTCP_PT_RR 201

static inline void rtcp_rr_set_pt(uint8_t *p_rtcp_rr)
{
    rtcp_set_pt(p_rtcp_rr, RTCP_PT_RR);
}

static inline uint8_t rtcp_rr_get_fraction_lost(const uint8_t *p_rtcp_rr)
{
    return p_rtcp_rr[12];
}

static inline void rtcp_rr_set_fraction_lost(uint8_t *p_rtcp_rr, uint8_t fraction_lost)
{
    p_rtcp_rr[12] = fraction_lost;
}

static inline int32_t rtcp_rr_get_cumulative_packets_lost(const uint8_t *p_rtcp_rr)
{
    uint32_t u = (p_rtcp_rr[13] << 16) | (p_rtcp_rr[14] << 8) | p_rtcp_rr[15];
    if (u & 0x800000)
        return -(u & 0x7fffff);
    else
        return u;
}

static inline void rtcp_rr_set_cumulative_packets_lost(uint8_t *p_rtcp_rr,
        int32_t packets_lost)
{
    if (packets_lost > 0x7fffff)
        packets_lost = 0x7fffff;
    else if (packets_lost < -0x800000)
        packets_lost = -0x800000;

    int neg = packets_lost < 0;
    uint32_t u = neg ? -packets_lost : packets_lost;

    p_rtcp_rr[13] = (neg << 7) | ((u >> 16) & 0x7f);
    p_rtcp_rr[14] = (u >>  8) & 0xff;
    p_rtcp_rr[15] =  u        & 0xff;
}

static inline uint32_t rtcp_rr_get_highest_seqnum(const uint8_t *p_rtcp_rr)
{
    return (p_rtcp_rr[16] << 24) | (p_rtcp_rr[17] << 16) |
           (p_rtcp_rr[18] << 8) | p_rtcp_rr[19];
}

static inline void rtcp_rr_set_highest_seqnum(uint8_t *p_rtcp_rr,
        uint32_t highest_seqnum)
{
    p_rtcp_rr[16] = (highest_seqnum >> 24) & 0xff;
    p_rtcp_rr[17] = (highest_seqnum >> 16) & 0xff;
    p_rtcp_rr[18] = (highest_seqnum >> 8) & 0xff;
    p_rtcp_rr[19] = highest_seqnum & 0xff;
}

static inline uint32_t rtcp_rr_get_inter_arrival_jitter(const uint8_t *p_rtcp_rr)
{
    return (p_rtcp_rr[20] << 24) | (p_rtcp_rr[21] << 16) |
           (p_rtcp_rr[22] << 8) | p_rtcp_rr[23];
}

static inline void rtcp_rr_set_inter_arrival_jitter(uint8_t *p_rtcp_rr,
        uint32_t inter_arrival_jitter)
{
    p_rtcp_rr[20] = (inter_arrival_jitter >> 24) & 0xff;
    p_rtcp_rr[21] = (inter_arrival_jitter >> 16) & 0xff;
    p_rtcp_rr[22] = (inter_arrival_jitter >> 8) & 0xff;
    p_rtcp_rr[23] = inter_arrival_jitter & 0xff;
}

static inline uint32_t rtcp_rr_get_last_sr(const uint8_t *p_rtcp_rr)
{
    return (p_rtcp_rr[24] << 24) | (p_rtcp_rr[25] << 16) |
           (p_rtcp_rr[26] << 8) | p_rtcp_rr[27];
}

static inline void rtcp_rr_set_last_sr(uint8_t *p_rtcp_rr,
        uint32_t last_sr)
{
    p_rtcp_rr[24] = (last_sr >> 24) & 0xff;
    p_rtcp_rr[25] = (last_sr >> 16) & 0xff;
    p_rtcp_rr[26] = (last_sr >> 8) & 0xff;
    p_rtcp_rr[27] = last_sr & 0xff;
}

static inline uint32_t rtcp_rr_get_delay_since_last_sr(const uint8_t *p_rtcp_rr)
{
    return (p_rtcp_rr[28] << 24) | (p_rtcp_rr[29] << 16) |
           (p_rtcp_rr[30] << 8) | p_rtcp_rr[31];
}

static inline void rtcp_rr_set_delay_since_last_sr(uint8_t *p_rtcp_rr,
        uint32_t delay_since_last_sr)
{
    p_rtcp_rr[28] = (delay_since_last_sr >> 24) & 0xff;
    p_rtcp_rr[29] = (delay_since_last_sr >> 16) & 0xff;
    p_rtcp_rr[30] = (delay_since_last_sr >> 8) & 0xff;
    p_rtcp_rr[31] = delay_since_last_sr & 0xff;
}
#endif /* !__BITSTREAM_IETF_RTCP_RR_H__ */
