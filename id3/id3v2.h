/*****************************************************************************
 * id3v2.h: ID3 tag version 2.4.0
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
 * from @url http://id3.org/id3v2.4.0-structure
 *
 * @section {ID3 overview}
 * structure:
 * @code R
 * +-----------------------------+
 * |      Header (10 bytes)      |
 * +-----------------------------+
 * |       Extended Header       |
 * | (variable length, OPTIONAL) |
 * +-----------------------------+
 * |   Frames (variable length)  |
 * +-----------------------------+
 * |           Padding           |
 * | (variable length, OPTIONAL) |
 * +-----------------------------+
 * | Footer (10 bytes, OPTIONAL) |
 * +-----------------------------+
 * @end code
 * @end section
 *
 * @section {ID3 header}
 * structure:
 * @code R
 * +-----------------------------+---------+
 * | File identifier "ID3"       | 3 bytes |
 * +-----------------------------+---------+
 * | Version                     | 2 bytes |
 * |+----------------------------+--------+|
 * || First byte: version        | 1 byte ||
 * || Second byte: revision      | 1 byte ||
 * |+----------------------------+--------+|
 * +-----------------------------+---------+
 * | Flags                       | 1 byte  |
 * |+----------------------------+--------+|
 * || Unsynchronisation          | 1 bit  ||
 * || Extended header            | 1 bit  ||
 * || Experimental               | 1 bit  ||
 * || Footer                     | 1 bit  ||
 * || reserved (0)               | 4 bit  ||
 * |+----------------------------+--------+|
 * +-----------------------------+---------+
 * | Size                        | 4 bytes |
 * +-----------------------------+---------+
 * @end code
 * The first 3 bytes @strong must be "ID3".
 *
 * The defined flags are:
 * @list
 * @item Unsynchronisation:
 *   Bit 7 in the indicates whether or not
 *   unsynchronisation is applied on all frames; a set bit indicates usage.
 * @item Extended header:
 *   Bit 6 indicates whether or not the header is
 *   followed by an extended header. A set bit indicates the presence of an
 *   extended header.
 * @item Experimental:
 *   Bit 5 is used as an 'experimental indicator'. This
 *   flag SHALL always be set when the tag is in an experimental stage.
 * @item Footer present:
 *   Bit 4 indicates that a footer is present at the very
 *   end of the tag. A set bit indicates the presence of a footer.
 * @end list
 *
 * @end section
 *
 * @section {ID3 frame}
 * structure:
 * @code R
 * +-----------------------------+---------+
 * | Frame ID                    | 4 bytes |
 * +-----------------------------+---------+
 * | Size                        | 4 bytes |
 * +-----------------------------+---------+
 * | Flags                       | 2 byte  |
 * +-----------------------------+---------+
 * @end code
 * @end section
 *
 * @section {Syncsafe integer}
 *
 * In some parts of the tag it is inconvenient to use the
 * unsychronisation scheme because the size of unsynchronised data is
 * not known in advance, which is particularly problematic with size
 * descriptors. The solution in ID3v2 is to use synchsafe integers, in
 * which there can never be any false synchs. Synchsafe integers are
 * integers that keep its highest bit (bit 7) zeroed, making seven bits
 * out of eight available. Thus a 32 bit synchsafe integer can store 28
 * bits of information.
 *
 * Example encoding 255 as a 16 bit syncsafe integer:
 * @code R
 * 255 (0000 0000 1111 1111) -> 383 (0000 0001 0111 1111)
 * @end code
 *
 * @end section
 *
 */

#ifndef __BITSTREAM_ID3_ID3V2_H__
# define __BITSTREAM_ID3_ID3V2_H__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include <bitstream/common.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define ID3V2_HEADER_SIZE       10
#define ID3V2_FOOTER_SIZE       10
#define ID3V2_FRAME_HEADER_SIZE 10

static inline uint32_t id3v2_unsynchsafe(const uint8_t *p)
{
    uint32_t v = 0;
    for (unsigned i = 0; i < 4; i++)
        v += p[i] << ((3 - i) * 7);
    return v;
}

static inline void id3v2_synchsafe(uint8_t *p, uint32_t v)
{
    p[0] = (v >> 21) & 0x7f;
    p[1] = (v >> 14) & 0x7f;
    p[2] = (v >>  7) & 0x7f;
    p[3] = (v >>  0) & 0x7f;
}

static inline bool id3v2_check_tag(const uint8_t *p_id3v2)
{
    return p_id3v2[0] == 'I' && p_id3v2[1] == 'D' && p_id3v2[2] == '3';
}

static inline bool id3v2_check_footer_tag(const uint8_t *p_id3v2)
{
    return p_id3v2[0] == '3' && p_id3v2[1] == 'D' && p_id3v2[2] == 'I';
}

static inline void id3v2_set_tag(uint8_t *p_id3v2)
{
    p_id3v2[0] = 'I';
    p_id3v2[1] = 'D';
    p_id3v2[2] = '3';
}

static inline void id3v2_set_footer_tag(uint8_t *p_id3v2)
{
    p_id3v2[0] = '3';
    p_id3v2[1] = 'D';
    p_id3v2[2] = 'I';
}

static inline uint8_t id3v2_get_version_major(const uint8_t *p_id3v2)
{
    return p_id3v2[3];
}

static inline void id3v2_set_version_major(uint8_t *p_id3v2, uint8_t major)
{
    p_id3v2[3] = major;
}

static inline uint8_t id3v2_get_version_rev(const uint8_t *p_id3v2)
{
    return p_id3v2[4];
}

static inline void id3v2_set_version_rev(uint8_t *p_id3v2, uint8_t rev)
{
    p_id3v2[4] = rev;
}

static inline uint32_t id3v2_get_size(const uint8_t *p_id3v2)
{
    return id3v2_unsynchsafe(p_id3v2 + 6);
}

static inline void id3v2_set_size(uint8_t *p_id3v2, uint32_t size)
{
    id3v2_synchsafe(p_id3v2 + 6, size);
}

#define ID3V2_FLAG_UNSYNCHRONISATION   (1 << 7)
#define ID3V2_FLAG_EXTENDED_HEADER     (1 << 6)
#define ID3V2_FLAG_EXPERIMENTAL        (1 << 5)
#define ID3V2_FLAG_FOOTER              (1 << 4)

static inline void id3v2_unset_flags(uint8_t *p_id3v2)
{
    p_id3v2[5] = 0;
}

static inline bool id3v2_has_flag(const uint8_t *p_id3v2, uint8_t flag)
{
    return !!(p_id3v2[5] & flag);
}

static inline void id3v2_set_flag(uint8_t *p_id3v2, uint8_t flag)
{
    p_id3v2[5] |= flag;
}

static inline void id3v2_unset_flag(uint8_t *p_id3v2, uint8_t flag)
{
    p_id3v2[5] &= ~flag;
}

#define ID3V2_FLAG(Name, Flag)                                              \
                                                                            \
static inline bool id3v2_has_##Name(const uint8_t *p_id3v2)                 \
{                                                                           \
    return id3v2_has_flag(p_id3v2, ID3V2_FLAG_##Flag);                      \
}                                                                           \
                                                                            \
static inline void id3v2_set_##Name(uint8_t *p_id3v2)                       \
{                                                                           \
    id3v2_set_flag(p_id3v2, ID3V2_FLAG_##Flag);                             \
}                                                                           \
                                                                            \
static inline void id3v2_unset_##Name(uint8_t *p_id3v2)                     \
{                                                                           \
    id3v2_unset_flag(p_id3v2, ID3V2_FLAG_##Flag);                           \
}

ID3V2_FLAG(unsynchronisation, UNSYNCHRONISATION);
ID3V2_FLAG(extended_header, EXTENDED_HEADER);
ID3V2_FLAG(experimental, EXPERIMENTAL);
ID3V2_FLAG(footer, FOOTER);

static inline void id3v2_init(uint8_t *p_id3v2)
{
    id3v2_set_tag(p_id3v2);
    id3v2_set_version_major(p_id3v2, 4);
    id3v2_set_version_rev(p_id3v2, 0);
    id3v2_unset_flags(p_id3v2);
    id3v2_set_size(p_id3v2, 0);
}

static inline uint32_t id3v2_footer_get_size(const uint8_t *p_id3v2)
{
    return id3v2_has_footer(p_id3v2) ? ID3V2_FOOTER_SIZE : 0;
}

static inline uint8_t *id3v2_get_footer(const uint8_t *p_id3v2)
{
    return id3v2_has_footer(p_id3v2) ?
        (uint8_t *)p_id3v2 + ID3V2_HEADER_SIZE + id3v2_get_size(p_id3v2) : NULL;
}

static inline uint8_t *id3v2_get_extented_header(const uint8_t *p_id3v2)
{
    return id3v2_has_extended_header(p_id3v2) ?
        (uint8_t *)p_id3v2 + ID3V2_HEADER_SIZE : NULL;
}

static inline uint32_t id3v2_get_extented_header_size(const uint8_t *p_id3v2)
{
    if (!id3v2_has_extended_header(p_id3v2))
        return 0;
    return id3v2_unsynchsafe(p_id3v2 + ID3V2_HEADER_SIZE);
}

static inline uint32_t id3v2_get_header_size(const uint8_t *p_id3v2)
{
    return ID3V2_HEADER_SIZE + id3v2_get_extented_header_size(p_id3v2);
}

static inline uint32_t id3v2_get_total_size(const uint8_t *p_id3v2)
{
    return ID3V2_HEADER_SIZE + id3v2_get_size(p_id3v2) +
        id3v2_footer_get_size(p_id3v2);
}

static inline uint8_t *id3v2_get_frames(const uint8_t *p_id3v2)
{
    return (uint8_t *)p_id3v2 + id3v2_get_header_size(p_id3v2);
}

#define ID3V2_FRAME_ID(a, b, c, d)  \
    (((uint32_t)a) << 24 |          \
     ((uint32_t)b) << 16 |          \
     ((uint32_t)c) <<  8 |          \
     ((uint32_t)d) <<  0)

#define ID3V2_FRAME_TEXT_ENCODING_ISO_8859_1    0
#define ID3V2_FRAME_TEXT_ENCODING_UTF_16        1
#define ID3V2_FRAME_TEXT_ENCODING_UTF_16BE      2
#define ID3V2_FRAME_TEXT_ENCODING_UTF_8         3

static inline const char *id3v2_frame_text_encoding_str(uint8_t encoding)
{
    switch (encoding) {
        case ID3V2_FRAME_TEXT_ENCODING_ISO_8859_1: return "ISO-8859-1";
        case ID3V2_FRAME_TEXT_ENCODING_UTF_16: return "UTF-16";
        case ID3V2_FRAME_TEXT_ENCODING_UTF_16BE: return "UTF-16BE";
        case ID3V2_FRAME_TEXT_ENCODING_UTF_8: return "UTF-8";
    }
    return "Unknown";
}

static inline void id3v2_set_txt_enc(uint8_t *p_txt, uint8_t encoding,
                                     const char *str)
{
    p_txt[0] = encoding;
    memcpy(p_txt + 1, str, strlen(str));
}

static inline bool id3v2_frame_validate_id(const uint8_t *p_frame)
{
    for (unsigned i = 0; i < 4; i++)
        if ((p_frame[i] < 'A' || p_frame[i] > 'Z') &&
            (p_frame[i] < '0' || p_frame[i] > '9'))
            return false;
    return true;
}

static inline uint32_t id3v2_frame_get_id(const uint8_t *p_frame)
{
    return ID3V2_FRAME_ID(p_frame[0], p_frame[1], p_frame[2], p_frame[3]);
}

static inline void id3v2_frame_set_id(uint8_t *p_frame, uint32_t id)
{
    p_frame[0] = (id >> 24) & 0xff;
    p_frame[1] = (id >> 16) & 0xff;
    p_frame[2] = (id >>  8) & 0xff;
    p_frame[3] = (id >>  0) & 0xff;
}

static inline void id3v2_frame_get_id_txt(const uint8_t *p_frame, char id[5])
{
    memcpy(id, p_frame, 4);
    id[4] = '\0';
}

static inline uint32_t id3v2_frame_get_size(const uint8_t *p_frame)
{
    return id3v2_unsynchsafe(p_frame + 4);
}

static inline void id3v2_frame_set_size(uint8_t *p_frame, uint32_t size)
{
    return id3v2_synchsafe(p_frame + 4, size);
}

#define ID3V2_FRAME_FLAG_TAG_ALTER_PRESERVATION     (1 << 14)
#define ID3V2_FRAME_FLAG_FILE_ALTER_PRESERVATION    (1 << 13)
#define ID3V2_FRAME_FLAG_REAL_ONLY                  (1 << 12)
#define ID3V2_FRAME_FLAG_GROUP_IDENTITY             (1 <<  6)
#define ID3V2_FRAME_FLAG_COMPRESSION                (1 <<  3)
#define ID3V2_FRAME_FLAG_ENCRYPTION                 (1 <<  2)
#define ID3V2_FRAME_FLAG_UNSYNCHRONISATION          (1 <<  1)
#define ID3V2_FRAME_FLAG_DATA_LENGTH_INDICATOR      (1 <<  0)

static inline void id3v2_frame_unset_flags(uint8_t *p_frame)
{
    p_frame[8] = 0;
    p_frame[9] = 0;
}

static inline uint16_t id3v2_frame_get_flags(const uint8_t *p_frame)
{
    return (uint16_t)p_frame[8] << 8 | p_frame[9];
}

static inline bool id3v2_frame_has_flag(const uint8_t *p_frame, uint16_t flag)
{
    return !!(id3v2_frame_get_flags(p_frame) & flag);
}

static inline void id3v2_frame_set_flag(uint8_t *p_frame, uint16_t flag)
{
    p_frame[8] |= (flag >> 8) & 0xff;
    p_frame[9] |= flag & 0xff;
}

static inline void id3v2_frame_unset_flag(uint8_t *p_frame, uint16_t flag)
{
    p_frame[8] &= ~((flag >> 8) & 0xff);
    p_frame[9] &= ~(flag & 0xff);
}

#define ID3V2_FRAME_FLAG(Name, Flag)                                        \
                                                                            \
static inline bool id3v2_frame_has_##Name(const uint8_t *p_frame)           \
{                                                                           \
    return id3v2_frame_has_flag(p_frame, ID3V2_FRAME_FLAG_##Flag);          \
}                                                                           \
                                                                            \
static inline void id3v2_frame_set_##Name(uint8_t *p_frame)                 \
{                                                                           \
    id3v2_frame_set_flag(p_frame, ID3V2_FRAME_FLAG_##Flag);                 \
}                                                                           \
                                                                            \
static inline void id3v2_frame_unset_##Name(uint8_t *p_frame)               \
{                                                                           \
    id3v2_frame_unset_flag(p_frame, ID3V2_FRAME_FLAG_##Flag);               \
}

ID3V2_FRAME_FLAG(tag_alter_preservation, TAG_ALTER_PRESERVATION);
ID3V2_FRAME_FLAG(file_alter_preservation, FILE_ALTER_PRESERVATION);
ID3V2_FRAME_FLAG(read_only, REAL_ONLY);
ID3V2_FRAME_FLAG(group_identity, GROUP_IDENTITY);
ID3V2_FRAME_FLAG(compression, COMPRESSION);
ID3V2_FRAME_FLAG(encryption, ENCRYPTION);
ID3V2_FRAME_FLAG(unsynchronisation, UNSYNCHRONISATION);
ID3V2_FRAME_FLAG(data_length_indicator, DATA_LENGTH_INDICATOR);

static inline uint8_t *id3v2_frame_get_data(const uint8_t *p_frame)
{
    return (uint8_t *)p_frame + ID3V2_FRAME_HEADER_SIZE;
}

static inline uint8_t *id3v2_next_frame(const uint8_t *p_id3v2,
                                        const uint8_t *p_frame)
{
    const uint8_t *frames = id3v2_get_frames(p_id3v2);
    const uint8_t *end = p_id3v2 + ID3V2_HEADER_SIZE + id3v2_get_size(p_id3v2);
    const uint8_t *next = frames;
    if (p_frame)
        next = id3v2_frame_get_data(p_frame) + id3v2_frame_get_size(p_frame);
    if (next + ID3V2_FRAME_HEADER_SIZE >= end)
        return NULL;
    if (next[0] == 0) /* padding */
        return NULL;
    return (uint8_t *)next;
}

#define id3v2_each_frame(p_id3v2, frame)                                    \
    for (uint8_t *frame = id3v2_next_frame(p_id3v2, NULL);                  \
         frame != NULL;                                                     \
         frame = id3v2_next_frame(p_id3v2, frame))

static inline bool id3v2_frame_is(const uint8_t *p_frame, uint32_t id)
{
    return id3v2_frame_get_id(p_frame) == id;
}

static inline bool id3v2_frame_validate(const uint8_t *p_frame)
{
    if (!id3v2_frame_validate_id(p_frame))
        return false;
    return id3v2_frame_get_size(p_frame) >= 1;
}

static inline void id3v2_frame_unsynchronise(const uint8_t *p_frame,
                                             uint8_t *p_frame_out,
                                             uint32_t *p_size)
{
    uint32_t size = *p_size;
    uint32_t frame_size = id3v2_frame_get_size(p_frame);
    uint32_t out_size = ID3V2_FRAME_HEADER_SIZE + frame_size;
    const uint8_t *frame_data = id3v2_frame_get_data(p_frame);
    uint8_t *tmp = p_frame_out;
    bool has_header = false;
    bool altered = false;

    if (id3v2_frame_has_unsynchronisation(p_frame)) {
        if (size >= out_size)
            memcpy(p_frame_out, p_frame, out_size);
        *p_size = out_size;
        return;
    }

    if (size >= ID3V2_FRAME_HEADER_SIZE) {
        memcpy(tmp, p_frame, ID3V2_FRAME_HEADER_SIZE);
        size -= ID3V2_FRAME_HEADER_SIZE;
        tmp += ID3V2_FRAME_HEADER_SIZE;
        has_header = true;
    } else
        size = 0;

    for (uint32_t i = 0; i < frame_size; i++) {
        if (size) {
            *tmp++ = frame_data[i];
            size--;
        }

        if (i + 1 < frame_size && frame_data[i] == 0xff &&
            ((frame_data[i + 1] & 0xe0) == 0xe0 || frame_data[i + 1] == 0)) {
            if (size) {
                *tmp++ = 0;
                size--;
            }
            out_size++;
            altered = true;
        }
    }

    if (has_header) {
        if (altered)
            id3v2_frame_set_unsynchronisation(p_frame_out);
        id3v2_frame_set_size(p_frame_out, out_size - ID3V2_FRAME_HEADER_SIZE);
    }
    *p_size = out_size;
}

static inline void id3v2_unsynchronise(const uint8_t *p_id3v2,
                                       uint8_t *p_id3v2_out,
                                       uint32_t *p_size)
{
    uint32_t size = *p_size;
    uint32_t out_size = 0;
    uint32_t extended_header_size = id3v2_get_extented_header_size(p_id3v2);
    uint8_t *tmp = p_id3v2_out;
    bool has_header = false;

    if (id3v2_has_unsynchronisation(p_id3v2)) {
        uint32_t total_size = id3v2_get_total_size(p_id3v2);
        if (size >= total_size)
            memcpy(tmp, p_id3v2, total_size);
        *p_size = total_size;
        return;
    }

    if (size >= ID3V2_HEADER_SIZE) {
        memcpy(tmp, p_id3v2, ID3V2_HEADER_SIZE);
        size -= ID3V2_HEADER_SIZE;
        tmp += ID3V2_HEADER_SIZE;
        has_header = true;
    } else
        size = 0;
    out_size += ID3V2_HEADER_SIZE;

    if (extended_header_size) {
        if (size >= extended_header_size) {
            const uint8_t *extended_header = id3v2_get_extented_header(p_id3v2);
            memcpy(tmp, extended_header, extended_header_size);
        } else
            size = 0;
        out_size += extended_header_size;
    }

    const uint8_t *end_frames = NULL;
    id3v2_each_frame(p_id3v2, frame) {
        uint32_t frame_size = size;
        id3v2_frame_unsynchronise(frame, tmp, &frame_size);
        if (size >= frame_size) {
            size -= frame_size;
            tmp += frame_size;
        }
        out_size += frame_size;
        end_frames =
            frame + ID3V2_FRAME_HEADER_SIZE + id3v2_frame_get_size(frame);
    }

    const uint8_t *footer = id3v2_get_footer(p_id3v2);
    if (footer) {
        out_size += ID3V2_HEADER_SIZE;
        if (size >= ID3V2_HEADER_SIZE) {
            memcpy(tmp, footer, ID3V2_HEADER_SIZE);
            id3v2_set_unsynchronisation(tmp);
            id3v2_set_size(tmp, out_size - 2 * ID3V2_HEADER_SIZE);
        }
    } else {
        const uint8_t *end =
            p_id3v2 + ID3V2_HEADER_SIZE + id3v2_get_size(p_id3v2);
        uint32_t padding = end - end_frames;
        /* make sure the tag doesn't finish with 0xff */
        if (padding) {
            out_size += padding;
            if (size >= padding)
                memcpy(tmp, end, padding);
        } else {
            out_size++;
            if (size)
                *tmp = 0;
        }
    }

    if (has_header) {
        id3v2_set_unsynchronisation(p_id3v2_out);
        id3v2_set_size(p_id3v2_out, out_size - ID3V2_HEADER_SIZE -
                       (footer ? ID3V2_HEADER_SIZE : 0));
    }

    *p_size = out_size;
}

static inline void id3v2_frame_undo_unsynchronise(const uint8_t *p_frame,
                                                  uint8_t *p_frame_out,
                                                  uint32_t *p_size)
{
    uint32_t size = *p_size;
    uint32_t frame_size = id3v2_frame_get_size(p_frame);
    uint32_t out_size = ID3V2_FRAME_HEADER_SIZE + frame_size;
    uint8_t *tmp = p_frame_out;
    bool has_header = false;

    if (!id3v2_frame_has_unsynchronisation(p_frame)) {
        if (size >= out_size)
            memcpy(tmp, p_frame, out_size);
        *p_size = out_size;
        return;
    }

    if (size >= ID3V2_FRAME_HEADER_SIZE) {
        memcpy(tmp, p_frame, ID3V2_FRAME_HEADER_SIZE);
        size -= ID3V2_FRAME_HEADER_SIZE;
        tmp += ID3V2_FRAME_HEADER_SIZE;
        has_header = true;
    }

    const uint8_t *frame_data = id3v2_frame_get_data(p_frame);
    for (uint32_t i = 0; i < frame_size; i++) {
        if (size) {
            *tmp++ = frame_data[i];
            size--;
        }
        if (i + 1 < frame_size && frame_data[i] == 0xff &&
            frame_data[i + 1] == 0x00) {
            i++;
            out_size--;
        }
    }

    if (has_header)
        id3v2_frame_set_size(p_frame_out, out_size - ID3V2_FRAME_HEADER_SIZE);
    *p_size = out_size;
}

static inline void id3v2_undo_unsynchronise(const uint8_t *p_id3v2,
                                            uint8_t *p_id3v2_out,
                                            uint32_t *p_size)
{
    uint32_t size = *p_size;
    uint32_t out_size = 0;
    uint32_t extended_header_size = id3v2_get_extented_header_size(p_id3v2);
    uint8_t *tmp = p_id3v2_out;
    bool has_header = false;

    if (!id3v2_has_unsynchronisation(p_id3v2)) {
        uint32_t total_size = id3v2_get_total_size(p_id3v2);
        if (size >= total_size)
            memcpy(tmp, p_id3v2, total_size);
        *p_size = total_size;
        return;
    }

    if (size >= ID3V2_HEADER_SIZE) {
        memcpy(tmp, p_id3v2, ID3V2_HEADER_SIZE);
        size -= ID3V2_HEADER_SIZE;
        tmp += ID3V2_HEADER_SIZE;
        has_header = true;
    } else
        size = 0;
    out_size += ID3V2_HEADER_SIZE;

    if (extended_header_size) {
        if (size >= extended_header_size) {
            const uint8_t *extended_header = id3v2_get_extented_header(p_id3v2);
            memcpy(tmp, extended_header, extended_header_size);
        } else
            size = 0;
        out_size += extended_header_size;
    }

    id3v2_each_frame(p_id3v2, frame) {
        uint32_t frame_size = size;
        id3v2_frame_undo_unsynchronise(frame, tmp, &frame_size);
        if (size >= frame_size) {
            size -= frame_size;
            tmp += frame_size;
        }
        out_size += frame_size;
    }

    const uint8_t *footer = id3v2_get_footer(p_id3v2);
    if (footer) {
        out_size += ID3V2_HEADER_SIZE;
        if (size >= ID3V2_HEADER_SIZE) {
            memcpy(tmp, footer, ID3V2_HEADER_SIZE);
            id3v2_unset_unsynchronisation(tmp);
            id3v2_set_size(tmp, out_size - 2 * ID3V2_HEADER_SIZE);
        }
    }

    if (has_header) {
        id3v2_unset_unsynchronisation(p_id3v2_out);
        id3v2_set_size(p_id3v2_out, out_size - ID3V2_HEADER_SIZE -
                       (footer ? ID3V2_HEADER_SIZE : 0));
    }

    *p_size = out_size;
}

static inline bool id3v2_validate_footer(const uint8_t *p_id3v2,
                                         const uint8_t *p_footer)
{
    return
        id3v2_check_footer_tag(p_footer) &&
        id3v2_get_version_major(p_id3v2) == id3v2_get_version_major(p_footer) &&
        id3v2_get_version_rev(p_id3v2) == id3v2_get_version_rev(p_footer) &&
        id3v2_has_unsynchronisation(p_id3v2) ==
        id3v2_has_unsynchronisation(p_footer) &&
        id3v2_has_extended_header(p_id3v2) ==
        id3v2_has_extended_header(p_footer) &&
        id3v2_has_experimental(p_id3v2) == id3v2_has_experimental(p_footer) &&
        id3v2_has_footer(p_id3v2) == id3v2_has_footer(p_footer) &&
        id3v2_get_size(p_id3v2) == id3v2_get_size(p_footer);
}

static inline bool id3v2_validate(const uint8_t *p_id3v2, size_t size)
{
    if (!p_id3v2 || size < ID3V2_HEADER_SIZE)
        return false;

    if (!id3v2_check_tag(p_id3v2) ||
        id3v2_get_version_major(p_id3v2) != 4 ||
        id3v2_get_version_rev(p_id3v2) != 0)
        return false;

    uint32_t total_size = id3v2_get_total_size(p_id3v2);
    if (size < total_size)
        return false;

    const uint8_t *end = p_id3v2 + ID3V2_HEADER_SIZE + id3v2_get_size(p_id3v2);
    const uint8_t *last = NULL;
    id3v2_each_frame(p_id3v2, frame) {
        last = frame + ID3V2_FRAME_HEADER_SIZE + id3v2_frame_get_size(frame);
        if (last > end || !id3v2_frame_validate(frame))
            return false;
    }
    if (!last)
        /* at least one frame is expected */
        return false;

    const uint8_t *footer = id3v2_get_footer(p_id3v2);
    if (footer)
        return last == footer && id3v2_validate_footer(p_id3v2, footer);
    else {
        while (last < end)
            /* skip padding */
            if (*last++ != 0)
                return false;
    }

    return true;
}

#ifdef __cplusplus
}
#endif

#endif /* !__BITSTREAM_ID3_V2_H__ */
