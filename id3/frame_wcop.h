/*****************************************************************************
 * frame_wcop.h: ID3 WCOP frame
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
 * Copyright or Legal information frame.
 */

#ifndef __BITSTREAM_ID3_FRAME_WCOP_H__
# define __BITSTREAM_ID3_FRAME_WCOP_H__

#include <bitstream/id3/id3v2.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define ID3V2_FRAME_ID_WCOP     ID3V2_FRAME_ID('W','C','O','P')
#define ID3V2_FRAME_WCOP_NAME   "copyright info"
#define ID3V2_FRAME_WCOP_DESC   "Copyright or Legal information"

static inline bool
id3v2_frame_validate_wcop(const uint8_t *p_frame)
{
    return p_frame &&
        id3v2_frame_get_id(p_frame) == ID3V2_FRAME_ID_WCOP;
}

#define id3v2_frame_print_wcop  id3v2_frame_print_w

#ifdef __cplusplus
}
#endif

#endif /* !__BITSTREAM_ID3_FRAME_WCOP_H__ */
