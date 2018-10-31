#ifndef __BITSTREAM_IETF_RTCP_FB_H__
# define __BITSTREAM_IETF_RTCP_FB_H__

# include <inttypes.h>

# define RTCP_PT_RTPFB             205
# define RTCP_PT_PSFB              206

# define RTCP_PT_RTPFB_GENERIC_NACK 1

# define RTCP_FB_HEADER_SIZE 12
# define RTCP_FB_FCI_GENERIC_NACK_SIZE 4

static inline void rtcp_fb_set_fmt(uint8_t *p_rtcp, uint8_t fmt)
{
    p_rtcp[0] |= fmt & 0x1f;
}

static inline uint8_t rtcp_fb_get_fmt(const uint8_t *p_rtcp)
{
    return p_rtcp[0] & 0x1f;
}

static inline void rtcp_fb_set_ssrc_pkt_sender(uint8_t *p_rtcp_fb,
                                               const uint8_t pi_ssrc[4])
{
    p_rtcp_fb[4] = pi_ssrc[0];
    p_rtcp_fb[5] = pi_ssrc[1];
    p_rtcp_fb[6] = pi_ssrc[2];
    p_rtcp_fb[7] = pi_ssrc[3];
}

static inline void rtcp_fb_set_int_ssrc_pkt_sender(uint8_t *p_rtcp_fb, uint32_t i_ssrc)
{
    p_rtcp_fb[4] = (i_ssrc >> 24) & 0xff;
    p_rtcp_fb[5] = (i_ssrc >> 16) & 0xff;
    p_rtcp_fb[6] = (i_ssrc >> 8) & 0xff;
    p_rtcp_fb[7] = i_ssrc & 0xff;
}

static inline uint32_t rtcp_fb_get_int_ssrc_pkt_sender(const uint8_t *p_rtcp_fb)
{
    return (p_rtcp_fb[4] << 24) | (p_rtcp_fb[5] << 16) | (p_rtcp_fb[6] << 8) | p_rtcp_fb[7];
}

static inline void rtcp_fb_set_ssrc_media_src(uint8_t *p_rtcp_fb,
                                              const uint8_t pi_ssrc[4])
{
    p_rtcp_fb[8] = pi_ssrc[0];
    p_rtcp_fb[9] = pi_ssrc[1];
    p_rtcp_fb[10] = pi_ssrc[2];
    p_rtcp_fb[11] = pi_ssrc[3];
}

static inline void rtcp_fb_get_ssrc_pkt_sender(const uint8_t *p_rtcp_fb,
                                               uint8_t pi_ssrc[4])
{
    pi_ssrc[0] = p_rtcp_fb[4];
    pi_ssrc[1] = p_rtcp_fb[5];
    pi_ssrc[2] = p_rtcp_fb[6];
    pi_ssrc[3] = p_rtcp_fb[7];
}

static inline void rtcp_fb_get_ssrc_media_src(const uint8_t *p_rtcp_fb,
                                              uint8_t pi_ssrc[4])
{
    pi_ssrc[0] = p_rtcp_fb[8];
    pi_ssrc[1] = p_rtcp_fb[9];
    pi_ssrc[2] = p_rtcp_fb[10];
    pi_ssrc[3] = p_rtcp_fb[11];
}

static inline void rtcp_fb_nack_set_packet_id(uint8_t *p_rtcp_fb_nack,
                                              uint16_t packet_id)
{
    p_rtcp_fb_nack[0] = (packet_id >> 8) & 0xff;
    p_rtcp_fb_nack[1] = packet_id & 0xff;
}

static inline uint16_t rtcp_fb_nack_get_packet_id(const uint8_t *p_rtcp_fb_nack)
{
    return (p_rtcp_fb_nack[0] << 8) | p_rtcp_fb_nack[1];
}

static inline uint16_t rtcp_fb_nack_get_bitmask_lost(const uint8_t *p_rtcp_fb_nack)
{
    return (p_rtcp_fb_nack[2] << 8) | p_rtcp_fb_nack[3];
}

static inline void rtcp_fb_nack_set_bitmask_lost(uint8_t *p_rtcp_fb_nack,
                                                 uint16_t bitmask)
{
    p_rtcp_fb_nack[2] = (bitmask >> 8) & 0xff;
    p_rtcp_fb_nack[3] = bitmask & 0xff;
}

#endif /* !__BITSTREAM_IETF_RTCP_FB_H__ */
