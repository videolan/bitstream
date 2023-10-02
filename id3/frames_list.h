/*****************************************************************************
 * frames_list.h: List of all supported frames
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
 * Includes all supported frames.
 */

#ifndef __BITSTREAM_ID3_FRAMES_LIST_H__
# define __BITSTREAM_ID3_FRAMES_LIST_H__

#include <bitstream/id3/frame_talb.h>
#include <bitstream/id3/frame_tbpm.h>
#include <bitstream/id3/frame_tcom.h>
#include <bitstream/id3/frame_tcon.h>
#include <bitstream/id3/frame_tcop.h>
#include <bitstream/id3/frame_tden.h>
#include <bitstream/id3/frame_tdly.h>
#include <bitstream/id3/frame_tdor.h>
#include <bitstream/id3/frame_tdrc.h>
#include <bitstream/id3/frame_tdrl.h>
#include <bitstream/id3/frame_tdtg.h>
#include <bitstream/id3/frame_tenc.h>
#include <bitstream/id3/frame_text.h>
#include <bitstream/id3/frame_tflt.h>
#include <bitstream/id3/frame_tipl.h>
#include <bitstream/id3/frame_tit1.h>
#include <bitstream/id3/frame_tit2.h>
#include <bitstream/id3/frame_tit3.h>
#include <bitstream/id3/frame_tkey.h>
#include <bitstream/id3/frame_tlan.h>
#include <bitstream/id3/frame_tlen.h>
#include <bitstream/id3/frame_tmcl.h>
#include <bitstream/id3/frame_tmed.h>
#include <bitstream/id3/frame_tmoo.h>
#include <bitstream/id3/frame_toal.h>
#include <bitstream/id3/frame_tofn.h>
#include <bitstream/id3/frame_toly.h>
#include <bitstream/id3/frame_tope.h>
#include <bitstream/id3/frame_town.h>
#include <bitstream/id3/frame_tpe1.h>
#include <bitstream/id3/frame_tpe2.h>
#include <bitstream/id3/frame_tpe3.h>
#include <bitstream/id3/frame_tpe4.h>
#include <bitstream/id3/frame_tpos.h>
#include <bitstream/id3/frame_tpro.h>
#include <bitstream/id3/frame_tpub.h>
#include <bitstream/id3/frame_trck.h>
#include <bitstream/id3/frame_trsn.h>
#include <bitstream/id3/frame_trso.h>
#include <bitstream/id3/frame_tsoa.h>
#include <bitstream/id3/frame_tsop.h>
#include <bitstream/id3/frame_tsot.h>
#include <bitstream/id3/frame_tsrc.h>
#include <bitstream/id3/frame_tsse.h>
#include <bitstream/id3/frame_tsst.h>
#include <bitstream/id3/frame_txxx.h>
#include <bitstream/id3/frame_wcom.h>
#include <bitstream/id3/frame_wcop.h>
#include <bitstream/id3/frame_woaf.h>
#include <bitstream/id3/frame_woar.h>
#include <bitstream/id3/frame_woas.h>
#include <bitstream/id3/frame_wors.h>
#include <bitstream/id3/frame_wpay.h>
#include <bitstream/id3/frame_wpub.h>
#include <bitstream/id3/frame_wxxx.h>
#include <bitstream/id3/frame_comm.h>
#include <bitstream/id3/frame_priv.h>
#include <bitstream/id3/frame_ufid.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define ID3V2_EACH_FRAME(Do, ...)       \
    Do(COMM, comm, ## __VA_ARGS__)      \
    Do(PRIV, priv, ## __VA_ARGS__)      \
    Do(TALB, talb, ## __VA_ARGS__)      \
    Do(TBPM, tbpm, ## __VA_ARGS__)      \
    Do(TCOM, tcom, ## __VA_ARGS__)      \
    Do(TCON, tcon, ## __VA_ARGS__)      \
    Do(TCOP, tcop, ## __VA_ARGS__)      \
    Do(TDEN, tden, ## __VA_ARGS__)      \
    Do(TDLY, tdly, ## __VA_ARGS__)      \
    Do(TDOR, tdor, ## __VA_ARGS__)      \
    Do(TDRC, tdrc, ## __VA_ARGS__)      \
    Do(TDRL, tdrl, ## __VA_ARGS__)      \
    Do(TDTG, tdtg, ## __VA_ARGS__)      \
    Do(TENC, tenc, ## __VA_ARGS__)      \
    Do(TEXT, text, ## __VA_ARGS__)      \
    Do(TFLT, tflt, ## __VA_ARGS__)      \
    Do(TIPL, tipl, ## __VA_ARGS__)      \
    Do(TIT1, tit1, ## __VA_ARGS__)      \
    Do(TIT2, tit2, ## __VA_ARGS__)      \
    Do(TIT3, tit3, ## __VA_ARGS__)      \
    Do(TKEY, tkey, ## __VA_ARGS__)      \
    Do(TLAN, tlan, ## __VA_ARGS__)      \
    Do(TLEN, tlen, ## __VA_ARGS__)      \
    Do(TMCL, tmcl, ## __VA_ARGS__)      \
    Do(TMED, tmed, ## __VA_ARGS__)      \
    Do(TMOO, tmoo, ## __VA_ARGS__)      \
    Do(TOAL, toal, ## __VA_ARGS__)      \
    Do(TOFN, tofn, ## __VA_ARGS__)      \
    Do(TOLY, toly, ## __VA_ARGS__)      \
    Do(TOPE, tope, ## __VA_ARGS__)      \
    Do(TOWN, town, ## __VA_ARGS__)      \
    Do(TPE1, tpe1, ## __VA_ARGS__)      \
    Do(TPE2, tpe2, ## __VA_ARGS__)      \
    Do(TPE3, tpe3, ## __VA_ARGS__)      \
    Do(TPE4, tpe4, ## __VA_ARGS__)      \
    Do(TPOS, tpos, ## __VA_ARGS__)      \
    Do(TPRO, tpro, ## __VA_ARGS__)      \
    Do(TPUB, tpub, ## __VA_ARGS__)      \
    Do(TRCK, trck, ## __VA_ARGS__)      \
    Do(TRSN, trsn, ## __VA_ARGS__)      \
    Do(TRSO, trso, ## __VA_ARGS__)      \
    Do(TSOA, tsoa, ## __VA_ARGS__)      \
    Do(TSOP, tsop, ## __VA_ARGS__)      \
    Do(TSOT, tsot, ## __VA_ARGS__)      \
    Do(TSRC, tsrc, ## __VA_ARGS__)      \
    Do(TSSE, tsse, ## __VA_ARGS__)      \
    Do(TSST, tsst, ## __VA_ARGS__)      \
    Do(TXXX, txxx, ## __VA_ARGS__)      \
    Do(UFID, ufid, ## __VA_ARGS__)      \
    Do(WCOM, wcom, ## __VA_ARGS__)      \
    Do(WCOP, wcop, ## __VA_ARGS__)      \
    Do(WOAF, woaf, ## __VA_ARGS__)      \
    Do(WOAR, woar, ## __VA_ARGS__)      \
    Do(WOAS, woas, ## __VA_ARGS__)      \
    Do(WORS, wors, ## __VA_ARGS__)      \
    Do(WPAY, wpay, ## __VA_ARGS__)      \
    Do(WPUB, wpub, ## __VA_ARGS__)      \
    Do(WXXX, wxxx, ## __VA_ARGS__)


static inline const char *id3v2_frame_name(const uint8_t *p_frame)
{
    switch (id3v2_frame_get_id(p_frame)) {
#define CASE(ID, Name) case ID3V2_FRAME_ID_##ID: return ID3V2_FRAME_##ID##_NAME;
        ID3V2_EACH_FRAME(CASE)
#undef CASE
    }
    return NULL;
}

static inline const char *id3v2_frame_desc(const uint8_t *p_frame)
{
    switch (id3v2_frame_get_id(p_frame)) {
#define CASE(ID, Name) case ID3V2_FRAME_ID_##ID: return ID3V2_FRAME_##ID##_DESC;
        ID3V2_EACH_FRAME(CASE)
#undef CASE
    }
    return NULL;
}

#ifdef __cplusplus
}
#endif

#endif /* !__BITSTREAM_ID3_FRAMES_LIST_H__ */
