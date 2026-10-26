/*****************************************************************************
 * 2108.h: SMPTE ST 2108-1 HDR/WCG Metadata Packing in VANC
 *****************************************************************************
 * Copyright (C) 2026 Open Broadcast Systems Ltd
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
 *  - SMPTE ST 2108-1:2018 HDR/WCG Metadata Packing and Signaling in the
 *    Vertical Ancillary Data Space
 */

#ifndef __BITSTREAM_SMPTE_2108_H__
#define __BITSTREAM_SMPTE_2108_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */
#include <stddef.h>   /* NULL */

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * SMPTE ST 2108-1 HDR/WCG Metadata Ancillary Data Packet
 *
 * Carried as a SMPTE ST 291-1 Type 2 ANC packet in VANC with:
 *   DID  = 0x41
 *   SDID = 0x0C
 *
 * The UDW payload consists of one or more HDR/WCG Metadata Frames,
 * each with the structure:
 *
 *   Byte 0 : HDR/WCG Metadata Frame Type  (uint8)
 *   Byte 1 : HDR/WCG Metadata Frame Length j (uint8, number of data bytes)
 *   Bytes 2..j+1 : HDR/WCG Metadata Frame Data
 *****************************************************************************/

/* ANC packet identifiers (Table 1) */
#define S2108_DID                     0x41
#define S2108_SDID                    0x0C

/* HDR/WCG Metadata Frame Type values (Table 3) */
#define S2108_FRAME_TYPE_STATIC1      0x00  /* Static Metadata Type 1 –
                                                 Mastering Display Colour Volume
                                                 (ITU-T H.265 SEI type 137) */
#define S2108_FRAME_TYPE_STATIC2      0x01  /* Static Metadata Type 2 –
                                                 Content Light Level
                                                 (ITU-T H.265 SEI type 144) */
#define S2108_FRAME_TYPE_DYNAMIC1     0x02  /* Dynamic Metadata Type 1 –
                                                 ATSC A/341 ST 2094-10 */
#define S2108_FRAME_TYPE_DYNAMIC5     0x06  /* Dynamic Metadata Type 5 –
                                                 ETSI TS 103 433-1 SL-HDR1 */

/* Fixed frame lengths for the two static frame types (Table 2 constraints) */
#define S2108_STATIC1_FRAME_LENGTH    0x1A  /* 26 bytes */
#define S2108_STATIC2_FRAME_LENGTH    0x06  /*  6 bytes */

/* SEI payloadType values embedded in Static frame data */
#define S2108_STATIC1_SEI_TYPE        0x89  /* 137 – mastering_display_colour_volume */
#define S2108_STATIC1_SEI_SIZE        0x18  /* 24 bytes of SEI payload */
#define S2108_STATIC2_SEI_TYPE        0x90  /* 144 – content_light_level_info */
#define S2108_STATIC2_SEI_SIZE        0x04  /*  4 bytes of SEI payload */

/* Minimum size of a metadata frame header (type + length fields) */
#define S2108_FRAME_HEADER_SIZE       2

/* Header bytes for Dynamic Metadata Type 1 (Table 4 / ANSI/SCTE 128-1) */
#define S2108_DYNAMIC1_COUNTRY_CODE   0xB5  /* itu_t_t35_country_code (USA) */
#define S2108_DYNAMIC1_PROVIDER_MSB   0x00  /* itu_t_t35_provider_code MSB (ATSC) */
#define S2108_DYNAMIC1_PROVIDER_LSB   0x31  /* itu_t_t35_provider_code LSB (ATSC) */
#define S2108_DYNAMIC1_USER_ID_0      0x47  /* user_identifier byte 0 */
#define S2108_DYNAMIC1_USER_ID_1      0x41  /* user_identifier byte 1 */
#define S2108_DYNAMIC1_USER_ID_2      0x39  /* user_identifier byte 2 */
#define S2108_DYNAMIC1_USER_ID_3      0x34  /* user_identifier byte 3 */
#define S2108_DYNAMIC1_TYPE_CODE      0x09  /* user_data_type_code for ST2094-10 */
#define S2108_DYNAMIC1_MARKER_BITS    0xFF  /* marker_bits concluding ATSC1_data() */

/* Overhead bytes added around the raw ST2094-10 payload in a Dynamic Type 1 frame:
 *   SEI payloadType (1) + SEI payloadSize (1) + 7 SCTE header bytes +
 *   user_data_type_code (1) = 10 bytes before the data, then 1 marker byte after.
 *   Frame Length = 11 + sizeof(ST2094-10_data). */
#define S2108_DYNAMIC1_OVERHEAD       11

/*****************************************************************************
 * HDR/WCG Metadata Frame accessors
 *
 * All functions operate on a pointer to the first byte of an HDR/WCG Metadata
 * Frame within the UDW payload (i.e. after ST 291-1 ANC framing).
 *****************************************************************************/

/* --- Frame Type (byte 0) -------------------------------------------------- */

static inline uint8_t s2108_get_frame_type(const uint8_t *p_frame)
{
    return p_frame[0];
}

static inline void s2108_set_frame_type(uint8_t *p_frame, uint8_t type)
{
    p_frame[0] = type;
}

/* --- Frame Length (byte 1) ------------------------------------------------ */

static inline uint8_t s2108_get_frame_length(const uint8_t *p_frame)
{
    return p_frame[1];
}

static inline void s2108_set_frame_length(uint8_t *p_frame, uint8_t length)
{
    p_frame[1] = length;
}

/* --- Frame Data pointer (byte 2 onwards) ---------------------------------- */

static inline uint8_t *s2108_get_frame_data(uint8_t *p_frame)
{
    return p_frame + S2108_FRAME_HEADER_SIZE;
}

static inline const uint8_t *s2108_get_frame_data_const(const uint8_t *p_frame)
{
    return p_frame + S2108_FRAME_HEADER_SIZE;
}

/* Total wire size of one frame (header + data bytes) */
static inline uint16_t s2108_frame_size(const uint8_t *p_frame)
{
    return (uint16_t)S2108_FRAME_HEADER_SIZE + p_frame[1];
}

/*****************************************************************************
 * Static Metadata Type 1 — Mastering Display Colour Volume (§5.3.2)
 *
 * Frame layout (26 data bytes, total frame = 28 bytes):
 *   data[0]      = 0x89  (SEI payloadType = 137)
 *   data[1]      = 0x18  (SEI payloadSize = 24)
 *   data[2..25]  = mastering_display_colour_volume() from ITU-T H.265
 *
 * mastering_display_colour_volume() contains (all big-endian uint16):
 *   display_primaries_x[0..2]  (chromaticity coords × 50000, CIE 1931)
 *   display_primaries_y[0..2]
 *   white_point_x
 *   white_point_y
 *   max_display_mastering_luminance  (0.0001 cd/m² units)
 *   min_display_mastering_luminance
 *****************************************************************************/

/* Offset of the mastering_display_colour_volume() payload within frame data */
#define S2108_STATIC1_MDC_OFFSET  2   /* skip payloadType + payloadSize */

static inline void s2108_static1_init(uint8_t *p_frame)
{
    s2108_set_frame_type(p_frame, S2108_FRAME_TYPE_STATIC1);
    s2108_set_frame_length(p_frame, S2108_STATIC1_FRAME_LENGTH);
    p_frame[2] = S2108_STATIC1_SEI_TYPE;
    p_frame[3] = S2108_STATIC1_SEI_SIZE;
}

/* Display primaries — index 0=green, 1=blue, 2=red per H.265 */
static inline uint16_t s2108_static1_get_primary_x(const uint8_t *p_frame,
                                                       unsigned int i)
{
    const uint8_t *d = p_frame + 2 + S2108_STATIC1_MDC_OFFSET + i * 4;
    return (d[0] << 8) | d[1];
}

static inline void s2108_static1_set_primary_x(uint8_t *p_frame,
                                                   unsigned int i, uint16_t val)
{
    uint8_t *d = p_frame + 2 + S2108_STATIC1_MDC_OFFSET + i * 4;
    d[0] = val >> 8;
    d[1] = val & 0xff;
}

static inline uint16_t s2108_static1_get_primary_y(const uint8_t *p_frame,
                                                       unsigned int i)
{
    const uint8_t *d = p_frame + 2 + S2108_STATIC1_MDC_OFFSET + i * 4 + 2;
    return (d[0] << 8) | d[1];
}

static inline void s2108_static1_set_primary_y(uint8_t *p_frame,
                                                   unsigned int i, uint16_t val)
{
    uint8_t *d = p_frame + 2 + S2108_STATIC1_MDC_OFFSET + i * 4 + 2;
    d[0] = val >> 8;
    d[1] = val & 0xff;
}

static inline uint16_t s2108_static1_get_white_x(const uint8_t *p_frame)
{
    const uint8_t *d = p_frame + 2 + S2108_STATIC1_MDC_OFFSET + 12;
    return (d[0] << 8) | d[1];
}

static inline void s2108_static1_set_white_x(uint8_t *p_frame, uint16_t val)
{
    uint8_t *d = p_frame + 2 + S2108_STATIC1_MDC_OFFSET + 12;
    d[0] = val >> 8;
    d[1] = val & 0xff;
}

static inline uint16_t s2108_static1_get_white_y(const uint8_t *p_frame)
{
    const uint8_t *d = p_frame + 2 + S2108_STATIC1_MDC_OFFSET + 14;
    return (d[0] << 8) | d[1];
}

static inline void s2108_static1_set_white_y(uint8_t *p_frame, uint16_t val)
{
    uint8_t *d = p_frame + 2 + S2108_STATIC1_MDC_OFFSET + 14;
    d[0] = val >> 8;
    d[1] = val & 0xff;
}

/* max_display_mastering_luminance in units of 0.0001 cd/m² */
static inline uint32_t s2108_static1_get_max_luma(const uint8_t *p_frame)
{
    const uint8_t *d = p_frame + 2 + S2108_STATIC1_MDC_OFFSET + 16;
    return ((uint32_t)d[0] << 24) | ((uint32_t)d[1] << 16) |
           ((uint32_t)d[2] << 8)  |  (uint32_t)d[3];
}

static inline void s2108_static1_set_max_luma(uint8_t *p_frame, uint32_t val)
{
    uint8_t *d = p_frame + 2 + S2108_STATIC1_MDC_OFFSET + 16;
    d[0] = (val >> 24) & 0xff;
    d[1] = (val >> 16) & 0xff;
    d[2] = (val >>  8) & 0xff;
    d[3] =  val        & 0xff;
}

/* min_display_mastering_luminance in units of 0.0001 cd/m² */
static inline uint32_t s2108_static1_get_min_luma(const uint8_t *p_frame)
{
    const uint8_t *d = p_frame + 2 + S2108_STATIC1_MDC_OFFSET + 20;
    return ((uint32_t)d[0] << 24) | ((uint32_t)d[1] << 16) |
           ((uint32_t)d[2] << 8)  |  (uint32_t)d[3];
}

static inline void s2108_static1_set_min_luma(uint8_t *p_frame, uint32_t val)
{
    uint8_t *d = p_frame + 2 + S2108_STATIC1_MDC_OFFSET + 20;
    d[0] = (val >> 24) & 0xff;
    d[1] = (val >> 16) & 0xff;
    d[2] = (val >>  8) & 0xff;
    d[3] =  val        & 0xff;
}

/*****************************************************************************
 * Static Metadata Type 2 — Content Light Level (§5.3.3)
 *
 * Frame layout (6 data bytes, total frame = 8 bytes):
 *   data[0]    = 0x90  (SEI payloadType = 144)
 *   data[1]    = 0x04  (SEI payloadSize = 4)
 *   data[2..3] = max_content_light_level   (uint16 big-endian, cd/m²)
 *   data[4..5] = max_pic_average_light_level (uint16 big-endian, cd/m²)
 *****************************************************************************/

static inline void s2108_static2_init(uint8_t *p_frame)
{
    s2108_set_frame_type(p_frame, S2108_FRAME_TYPE_STATIC2);
    s2108_set_frame_length(p_frame, S2108_STATIC2_FRAME_LENGTH);
    p_frame[2] = S2108_STATIC2_SEI_TYPE;
    p_frame[3] = S2108_STATIC2_SEI_SIZE;
}

/* MaxCLL — Maximum Content Light Level in cd/m² */
static inline uint16_t s2108_static2_get_maxcll(const uint8_t *p_frame)
{
    return ((uint16_t)p_frame[4] << 8) | p_frame[5];
}

static inline void s2108_static2_set_maxcll(uint8_t *p_frame, uint16_t val)
{
    p_frame[4] = val >> 8;
    p_frame[5] = val & 0xff;
}

/* MaxFALL — Maximum Frame Average Light Level in cd/m² */
static inline uint16_t s2108_static2_get_maxfall(const uint8_t *p_frame)
{
    return ((uint16_t)p_frame[6] << 8) | p_frame[7];
}

static inline void s2108_static2_set_maxfall(uint8_t *p_frame, uint16_t val)
{
    p_frame[6] = val >> 8;
    p_frame[7] = val & 0xff;
}

/*****************************************************************************
 * Dynamic Metadata Type 1 — ATSC A/341 ST 2094-10 (§5.3.4)
 *
 * Frame layout (variable length):
 *   data[0]      = 0x04  (SEI payloadType = 4, user_data_registered_itu_t_t35)
 *   data[1]      = 9 + sizeof(ST2094-10_data)  (SEI payloadSize)
 *   data[2]      = 0xB5  (itu_t_t35_country_code, USA)
 *   data[3]      = 0x00  (itu_t_t35_provider_code MSB, ATSC)
 *   data[4]      = 0x31  (itu_t_t35_provider_code LSB, ATSC)
 *   data[5]      = 0x47  (user_identifier MSB)
 *   data[6]      = 0x41
 *   data[7]      = 0x39
 *   data[8]      = 0x34  (user_identifier LSB)
 *   data[9]      = 0x09  (user_data_type_code for ST2094-10_data)
 *   data[10..]   = ST2094-10_data()
 *   data[last]   = 0xFF  (marker_bits)
 *
 * Frame Length = S2108_DYNAMIC1_OVERHEAD + sizeof(ST2094-10_data).
 *****************************************************************************/

/* Initialise the fixed-format header bytes of a Dynamic Type 1 frame.
 * st2094_10_size is the byte length of the raw ST2094-10_data() payload. */
static inline void s2108_dynamic1_init(uint8_t *p_frame,
                                          uint8_t st2094_10_size)
{
    s2108_set_frame_type(p_frame, S2108_FRAME_TYPE_DYNAMIC1);
    s2108_set_frame_length(p_frame,
                              (uint8_t)(S2108_DYNAMIC1_OVERHEAD + st2094_10_size));
    p_frame[2] = 0x04;                         /* SEI payloadType */
    p_frame[3] = (uint8_t)(9 + st2094_10_size); /* SEI payloadSize */
    p_frame[4] = S2108_DYNAMIC1_COUNTRY_CODE;
    p_frame[5] = S2108_DYNAMIC1_PROVIDER_MSB;
    p_frame[6] = S2108_DYNAMIC1_PROVIDER_LSB;
    p_frame[7] = S2108_DYNAMIC1_USER_ID_0;
    p_frame[8] = S2108_DYNAMIC1_USER_ID_1;
    p_frame[9] = S2108_DYNAMIC1_USER_ID_2;
    p_frame[10] = S2108_DYNAMIC1_USER_ID_3;
    p_frame[11] = S2108_DYNAMIC1_TYPE_CODE;
    /* marker byte lives at p_frame[12 + st2094_10_size] */
    p_frame[12 + st2094_10_size] = S2108_DYNAMIC1_MARKER_BITS;
}

/* Pointer to the start of ST2094-10_data() within a Dynamic Type 1 frame */
static inline uint8_t *s2108_dynamic1_get_payload(uint8_t *p_frame)
{
    return p_frame + 12; /* skip 2-byte frame header + 10 fixed bytes */
}

static inline const uint8_t *s2108_dynamic1_get_payload_const(
        const uint8_t *p_frame)
{
    return p_frame + 12;
}

/*****************************************************************************
 * Dynamic Metadata Type 5 — ETSI TS 103 433-1 SL-HDR1 (§5.3.5)
 *
 * Frame layout (variable length, sl_hdr_info size ≤ 251 bytes):
 *   data[0]     = 0x04  (SEI payloadType = 4)
 *   data[1]     = ceil(sl_hdr_info_bits / 8)  (payload size in bytes)
 *   data[2..]   = sl_hdr_info() from ETSI TS 103 433-1 Annex A.2,
 *                 zero-padded to a byte boundary with a leading 1-bit
 *
 * Frame Length = 2 + ceil(sl_hdr_info_bits / 8).
 *****************************************************************************/

static inline void s2108_dynamic5_init(uint8_t *p_frame,
                                          uint8_t sl_hdr_payload_bytes)
{
    s2108_set_frame_type(p_frame, S2108_FRAME_TYPE_DYNAMIC5);
    s2108_set_frame_length(p_frame, (uint8_t)(2 + sl_hdr_payload_bytes));
    p_frame[2] = 0x04;                /* SEI payloadType */
    p_frame[3] = sl_hdr_payload_bytes; /* SEI payloadSize */
}

/* Pointer to the start of sl_hdr_info() within a Dynamic Type 5 frame */
static inline uint8_t *s2108_dynamic5_get_payload(uint8_t *p_frame)
{
    return p_frame + 4; /* skip 2-byte frame header + payloadType + payloadSize */
}

static inline const uint8_t *s2108_dynamic5_get_payload_const(
        const uint8_t *p_frame)
{
    return p_frame + 4;
}

/*****************************************************************************
 * Packet-level helpers
 *****************************************************************************/

/* Validate that p_frame points to a well-formed HDR/WCG Metadata Frame.
 * Returns false if the frame type is reserved or the fixed-length types have
 * an unexpected length. */
static inline bool s2108_validate_frame(const uint8_t *p_frame)
{
    uint8_t type   = s2108_get_frame_type(p_frame);
    uint8_t length = s2108_get_frame_length(p_frame);

    switch (type) {
    case S2108_FRAME_TYPE_STATIC1:
        return length == S2108_STATIC1_FRAME_LENGTH &&
               p_frame[2] == S2108_STATIC1_SEI_TYPE &&
               p_frame[3] == S2108_STATIC1_SEI_SIZE;
    case S2108_FRAME_TYPE_STATIC2:
        return length == S2108_STATIC2_FRAME_LENGTH &&
               p_frame[2] == S2108_STATIC2_SEI_TYPE &&
               p_frame[3] == S2108_STATIC2_SEI_SIZE;
    case S2108_FRAME_TYPE_DYNAMIC1:
        return length >= S2108_DYNAMIC1_OVERHEAD;
    case S2108_FRAME_TYPE_DYNAMIC5:
        return length >= 2 && length <= 253;
    default:
        return false; /* reserved */
    }
}

/* Advance p_frame to the next HDR/WCG Metadata Frame within the same UDW
 * payload.  p_end must point one byte past the last UDW byte.
 * Returns NULL when there are no more frames. */
static inline const uint8_t *s2108_next_frame(const uint8_t *p_frame,
                                                  const uint8_t *p_end)
{
    const uint8_t *p_next = p_frame + s2108_frame_size(p_frame);
    if (p_next + S2108_FRAME_HEADER_SIZE > p_end)
        return NULL;
    return p_next;
}

#ifdef __cplusplus
}
#endif

#endif /* __BITSTREAM_SMPTE_2108_H__ */
