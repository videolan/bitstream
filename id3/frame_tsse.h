/*****************************************************************************
 * frame_tsse.h: ID3 TSSE frame
 *****************************************************************************
 * Copyright (C) 2023 EasyTools
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
 * Software or Hardware and settings used for encoding frame.
 */

#ifndef __BITSTREAM_ID3_FRAME_TSSE_H__
# define __BITSTREAM_ID3_FRAME_TSSE_H__

#include <bitstream/id3/id3v2.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define ID3V2_FRAME_ID_TSSE     ID3V2_FRAME_ID('T','S','S','E')
#define ID3V2_FRAME_TSSE_NAME   "encoding settings"
#define ID3V2_FRAME_TSSE_DESC   "Software or Hardware and settings used for encoding"

static inline bool
id3v2_frame_validate_tsse(const uint8_t *p_frame)
{
    return p_frame &&
        id3v2_frame_get_id(p_frame) == ID3V2_FRAME_ID_TSSE &&
        id3v2_frame_get_size(p_frame) >= 1;
}

#define id3v2_frame_print_tsse  id3v2_frame_print_t

#ifdef __cplusplus
}
#endif

#endif /* !__BITSTREAM_ID3_FRAME_TSSE_H__ */
