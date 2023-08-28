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

static inline const char *bitstream_xml_escape_char(char c)
{
    switch (c) {
        case '<': return "&lt;";
        case '>': return "&gt;";
        case '"': return "&quot;";
        case '\'': return "&apos;";
        case '&': return "&amp;";
    }
    return NULL;
}

static inline size_t bitstream_xml_escape_len(const char *str)
{
    size_t len = str ? strlen(str) : 0;
    size_t out_len = 0;
    for (unsigned i = 0; i < len; i++) {
        const char *esc = bitstream_xml_escape_char(str[i]);
        out_len += esc ? strlen(esc) : 1;
    }
    return out_len;
}

static inline char *bitstream_xml_escape(const char *str)
{
    if (!str)
        return NULL;

    size_t len = strlen(str);
    size_t out_len = bitstream_xml_escape_len(str);
    char *out = malloc(out_len + 1);
    if (!out)
        return NULL;

    char *tmp = out;
    for (unsigned i = 0; i < len; i++) {
        const char *esc = bitstream_xml_escape_char(str[i]);
        if (esc) {
            size_t esc_len = strlen(esc);
            memcpy(tmp, esc, esc_len);
            tmp += esc_len;
        }
        else
            *tmp++ = str[i];
    }
    *tmp = '\0';
    return out;
}

#ifdef __cplusplus
}
#endif

#endif
