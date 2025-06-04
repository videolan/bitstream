/*****************************************************************************
 * common.h: (Rare) common declarations for all submodules
 *****************************************************************************
 * Copyright (C) 2010 VideoLAN
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
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

#ifndef __BITSTREAM_COMMON_H__
#define __BITSTREAM_COMMON_H__

#include <stdint.h>   /* uint8_t, uint16_t, etc... */
#include <stdbool.h>  /* bool */
#include <string.h>   /* memset, memcpy */
#include <stdlib.h>   /* malloc, free */
#include <stdio.h>    /* sprintf */

#define BITSTREAM_VERSION_MAJOR 1
#define BITSTREAM_VERSION_MINOR 0
#define BITSTREAM_VERSION_REVISION 0

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum print_type_t {
    PRINT_TEXT,
    PRINT_XML
} print_type_t;

typedef void (*f_print)(void *, const char *, ...) __attribute__ ((format(printf, 2, 3)));
typedef char * (*f_iconv)(void *, const char *, char *, size_t);

static inline size_t bitstream_xml_escape_chars(const char *str, char *out)
{
    size_t len = 1;

    while (*str) {
        uint32_t codepoint = 0xFFFD;
        const char *p = str;
        int size = 1;

        unsigned char c = *str++;
        if (c < 0x80) {
            codepoint = c;
            switch (c) {
                case '<': p = "&lt;"; size = 4; break;
                case '>': p = "&gt;"; size = 4; break;
                case '"': p = "&quot;"; size = 6; break;
                case '\'': p = "&apos;"; size = 6; break;
                case '&': p = "&amp;"; size = 5; break;
            }
        } else if ((c & 0xE0) == 0xC0) {
            if ((str[0] & 0xC0) == 0x80) {
                codepoint = ((c & 0x1F) << 6) |
                    (str[0] & 0x3F);
                size = 2;
                str += 1;
            }
        } else if ((c & 0xF0) == 0xE0) {
            if ((str[0] & 0xC0) == 0x80 &&
                (str[1] & 0xC0) == 0x80) {
                codepoint = ((c & 0x0F) << 12) |
                    ((str[0] & 0x3F) << 6) |
                    (str[1] & 0x3F);
                size = 3;
                str += 2;
            }
        } else if ((c & 0xF8) == 0xF0) {
            if ((str[0] & 0xC0) == 0x80 &&
                (str[1] & 0xC0) == 0x80 &&
                (str[2] & 0xC0) == 0x80) {
                codepoint = ((c & 0x07) << 18) |
                    ((str[0] & 0x3F) << 12) |
                    ((str[1] & 0x3F) << 6) |
                    (str[2] & 0x3F);
                size = 4;
                str += 3;
            }
        }

        // #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF]
        if (codepoint == 0xFFFD ||
            !(codepoint == 0x9 ||
              codepoint == 0xA ||
              codepoint == 0xD ||
              (codepoint >= 0x20 && codepoint <= 0xD7FF) ||
              (codepoint >= 0xE000 && codepoint <= 0xFFFD) ||
              (codepoint >= 0x10000 && codepoint <= 0x10FFFF))) {
            p = "\uFFFD";
            size = 3;
        }

        if (out) {
            memcpy(out, p, size);
            out += size;
        }
        len += size;
    }

    if (out)
        *out = '\0';
    return len;
}

static inline char *bitstream_xml_escape(const char *str)
{
    if (!str)
        return NULL;
    char *out = (char *)malloc(bitstream_xml_escape_chars(str, NULL));
    if (!out)
        return NULL;
    bitstream_xml_escape_chars(str, out);
    return out;
}

#define BITSTREAM_GET_TYPE(Type)                                            \
static inline Type##_t bitstream_get_##Type(const uint8_t *p,               \
                                            unsigned position,              \
                                            unsigned nb_bits)               \
{                                                                           \
    Type##_t v = 0;                                                         \
    int bits = nb_bits;                                                     \
    uint8_t nbits = 8 - (position % 8);                                     \
    for (p += position / 8; bits > 0; p++, bits -= nbits, nbits = 8) {      \
        uint8_t size = nbits > bits ? bits : nbits;                         \
        uint8_t mask = ~(0xff << size) << (nbits - size);                   \
        v = (v << size) | ((*p & mask) >> (nbits - size));                  \
    }                                                                       \
    return v;                                                               \
}

BITSTREAM_GET_TYPE(uint8);
BITSTREAM_GET_TYPE(uint16);
BITSTREAM_GET_TYPE(uint32);
BITSTREAM_GET_TYPE(uint64);

#define BITSTREAM_SET_TYPE(Type)                                            \
static inline void bitstream_set_##Type(uint8_t *p,                         \
                                        unsigned position,                  \
                                        unsigned nb_bits,                   \
                                        Type##_t v)                         \
{                                                                           \
    int bits = nb_bits;                                                     \
    uint8_t vbits, mask, nbits = 8 - (position % 8);                        \
    for (p += position / 8; bits > 0; p++, bits -= nbits, nbits = 8) {      \
        mask = (uint8_t)(0xff << (bits >= 8 ? 0 : 8 - bits)) >> (8 - nbits);\
        vbits = bits >= nbits ? v >> (bits - nbits) : v << (nbits - bits);  \
        *p = (*p & ~mask) | (vbits & mask);                                 \
    }                                                                       \
}

BITSTREAM_SET_TYPE(uint8);
BITSTREAM_SET_TYPE(uint16);
BITSTREAM_SET_TYPE(uint32);
BITSTREAM_SET_TYPE(uint64);

#define BITSTREAM_GET(Name, Field, Type, Position, Bits)                    \
static inline Type##_t Name##_get_##Field(const uint8_t *p)                 \
{                                                                           \
    return bitstream_get_##Type(p, Position, Bits);                         \
}

#define BITSTREAM_SET(Name, Field, Type, Position, Bits)                    \
static inline void Name##_set_##Field(uint8_t *p, Type##_t v)               \
{                                                                           \
    bitstream_set_##Type(p, Position, Bits, v);                             \
}

#define BITSTREAM_GET_SET(Name, Field, Type, Position, Bits)                \
    BITSTREAM_GET(Name, Field, Type, Position, Bits)                        \
    BITSTREAM_SET(Name, Field, Type, Position, Bits)

#ifdef __cplusplus
}
#endif

#endif
