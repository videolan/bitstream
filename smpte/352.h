/*****************************************************************************
 * 352.h
 *****************************************************************************
 * Copyright (C) 2018 Open Broadcast Systems Ltd
 *
 * Authors: Kieran Kunhya <kierank@obe.tv>
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
 *  - SMPTE 352
 */

#ifndef __BITSTREAM_SMPTE_352_H__
#define __BITSTREAM_SMPTE_352_H__

/* Payload and Digital Device Interface Identification */
#define S352_PAYLOAD_SD                                      0x81
#define S352_PAYLOAD_720_INTERFACE_1_POINT_5_GBPS            0x84
#define S352_PAYLOAD_1080_INTERFACE_1_POINT_5_GBPS           0x85
#define S352_PAYLOAD_1080_INTERFACE_1_POINT_5_GBPS_DUAL_LINK 0x87
#define S352_PAYLOAD_720_INTERFACE_3_GBPS_LEVEL_A            0x88
#define S352_PAYLOAD_1080_INTERFACE_3_GBPS_LEVEL_A           0x89
#define S352_PAYLOAD_1080_INTERFACE_3_GBPS_LEVEL_B           0x8a

/* Scanning Method */
#define S352_TRANSPORT_INTERLACED  0
#define S352_TRANSPORT_PROGRESSIVE 1

#define S352_STRUCTURE_INTERLACED  0
#define S352_STRUCTURE_PROGRESSIVE 1

/* Picture Rate */
#define S352_PICTURE_RATE_UNDEFINED  0
#define S352_PICTURE_RATE_24000_1001 2
#define S352_PICTURE_RATE_24         3
#define S352_PICTURE_RATE_48000_1001 4
#define S352_PICTURE_RATE_25         5
#define S352_PICTURE_RATE_30000_1001 6
#define S352_PICTURE_RATE_30         7
#define S352_PICTURE_RATE_48         8
#define S352_PICTURE_RATE_50         9
#define S352_PICTURE_RATE_60000_1001 10
#define S352_PICTURE_RATE_60         11

/* Sampling Structure Identification */
#define S352_ASPECT_RATIO_4_3     0
#define S352_ASPECT_RATIO_16_9    1

#define S352_SAMPLING_422_YCBCR   0
#define S352_SAMPLING_444_YCBCR   1
#define S352_SAMPLING_444_GBR     2
#define S352_SAMPLING_420         3
#define S352_SAMPLING_4224_YCBCRA 4
#define S352_SAMPLING_4444_YCBCRA 5
#define S352_SAMPLING_4444_GBRA   6
#define S352_SAMPLING_2048_2_FS   7
#define S352_SAMPLING_4224_YCBCRD 8 /* D = data channel */
#define S352_SAMPLING_4444_YCBCRD 9
#define S352_SAMPLING_4444_GBRD   10
#define S352_SAMPLING_444_XYZ     14 /* X'Y'Z' */

/* Bit Depth */
#define S352_BIT_DEPTH_8  0
#define S352_BIT_DEPTH_10 1
#define S352_BIT_DEPTH_12 2 /* mapped into 10-bit interface */

#endif
