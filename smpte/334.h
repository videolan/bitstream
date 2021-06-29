/*****************************************************************************
 * 334.h: Vertical Ancillary Data Mapping of Caption Data and Other Related Data /
          Caption Distribution Packet (CDP) Definition
 *****************************************************************************
 * Copyright (C) 2021 Open Broadcast Systems Ltd
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
 *  - SMPTE 334
 */

#ifndef __BITSTREAM_SMPTE_334_H__
#define __BITSTREAM_SMPTE_334_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */

#ifdef __cplusplus
extern "C"
{
#endif

#define S334_CDP_IDENTIFIER 0x9669
#define S334_CDP_HEADER_LEN 7
#define S334_CDP_CCDATA_HEADER_LEN 2
#define S334_CDP_CCDATA_ID 0x72
#define S334_CDP_TIMECODE_LEN 5
#define S334_CDP_FOOTER_LEN 4

#ifdef __cplusplus
}
#endif

#endif
