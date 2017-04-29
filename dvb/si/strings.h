/*****************************************************************************
 * strings.h: ETSI EN 300 468 Strings
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
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

/*
 * Normative references:
 *  - ETSI EN 300 468 V1.11.1 (2010-04) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_STRINGS_H__
#define __BITSTREAM_DVB_STRINGS_H__

#include <stdlib.h>   /* malloc, free */

#include <bitstream/common.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * DVB string
 *****************************************************************************/
static const char *ppsz_dvb_encodings[] = {
    /* 0x00 - 0x0f */
    "", "ISO-8859-5", "ISO-8859-6", "ISO-8859-7", "ISO-8859-8",
    "ISO-8859-9", "ISO-8859-10", "ISO-8859-11", "", "ISO-8859-13",
    "ISO-8859-14", "ISO-8859-15", "", "", "", "",

    /* 0x10 - 0x1f */
    "", "UCS-2BE", "EUC-KR", "GB2312", "UCS-2BE", "UTF-8",
    "", "", "", "", "", "", "", "", "", "", NULL
};
static const char *ppsz_dvb_encodings10[] = {
    "", "ISO-8859-1", "ISO-8859-2", "ISO-8859-3", "ISO-8859-4",
    "ISO-8859-5", "ISO-8859-6", "ISO-8859-7", "ISO-8859-8", "ISO-8859-9",
    "ISO-8859-10", "ISO-8859-11", "", "ISO-8859-13", "ISO-8859-14",
    "ISO-8859-15", NULL
};

static inline const char *dvb_string_get_encoding(const uint8_t **pp_string,
        size_t *pi_length, const char *psz_default_encoding)
{
    uint8_t i_first;

    if (!*pi_length)
        return NULL;
    i_first = (*pp_string)[0];

    if (!i_first)
        return NULL;
    if (i_first >= 0x20)
        return psz_default_encoding;
    (*pp_string)++;
    (*pi_length)--;

    if (i_first == 0x10 && *pi_length >= 2) {
        uint8_t i_second = (*pp_string)[0];
        uint8_t i_third = (*pp_string)[1];
        (*pp_string) += 2;
        (*pi_length) -= 2;

        if (i_second != 0x0 || i_third == 0 || i_third >= 0x10)
            return NULL;
        return ppsz_dvb_encodings10[i_third];
    }

    if (i_first == 0x1f && *pi_length >= 1) {
        /* no info on these encodings, skip */
        (*pp_string)++;
        (*pi_length)--;
        return NULL;
    }

    return ppsz_dvb_encodings[i_first];
}

static inline uint8_t *dvb_string_set_inner(const uint8_t *p_string,
        size_t i_length, const char *psz_encoding, size_t *pi_out_length,
        const char *psz_default_encoding)
{
    int i;

    if (!strcmp(psz_encoding, psz_default_encoding)) {
        uint8_t *p_encoded = (uint8_t *)malloc(i_length);
        *pi_out_length = i_length;
        memcpy(p_encoded, p_string, i_length);
        return p_encoded;
    }

    for (i = 0; ppsz_dvb_encodings[i] != NULL; i++) {
        if (!strcasecmp(psz_encoding, ppsz_dvb_encodings[i])) {
            uint8_t *p_encoded = (uint8_t *)malloc(i_length + 1);
            *pi_out_length = i_length + 1;
            p_encoded[0] = i;
            memcpy(p_encoded + 1, p_string, i_length);
            return p_encoded;
        }
    }

    for (i = 0; ppsz_dvb_encodings10[i] != NULL; i++) {
        if (!strcasecmp(psz_encoding, ppsz_dvb_encodings10[i])) {
            uint8_t *p_encoded = (uint8_t *)malloc(i_length + 3);
            *pi_out_length = i_length + 3;
            p_encoded[0] = 0x10;
            p_encoded[1] = 0x0;
            p_encoded[2] = i;
            memcpy(p_encoded + 3, p_string, i_length);
            return p_encoded;
        }
    }

    *pi_out_length = 0;
    return NULL;
}

static inline uint8_t *dvb_string_set(const uint8_t *p_string, size_t i_length,
                                      const char *psz_encoding,
                                      size_t *pi_out_length)
{
    return dvb_string_set_inner(p_string, i_length, psz_encoding,
                                pi_out_length, "ISO6937");
}

static inline uint8_t *dvb_string_set_quirks(const uint8_t *p_string,
        size_t i_length, const char *psz_encoding, size_t *pi_out_length,
        const char *psz_provider)
{
    if (psz_provider != NULL && !strcmp(psz_provider, "CSAT"))
        return dvb_string_set_inner(p_string, i_length, psz_encoding,
                                    pi_out_length, "ISO-8859-1");
    return dvb_string_set_inner(p_string, i_length, psz_encoding,
                                pi_out_length, "ISO6937");
}

/* simpler API because this one doesn't output to multibyte charsets */
static inline char *dvb_string_get_inner(const uint8_t *p_string,
        size_t i_length, f_iconv pf_iconv, void *iconv_opaque,
        const char *psz_default_encoding)
{
    if (i_length) {
        const char *psz_encoding =
            dvb_string_get_encoding(&p_string, &i_length, psz_default_encoding);
        if (psz_encoding == NULL || !i_length) {
            /* try one-byte charset */
            char *psz_string = (char *)malloc(i_length + 1);
            memcpy(psz_string, p_string, i_length);
            psz_string[i_length] = '\0';
            return psz_string;
        }

        return pf_iconv(iconv_opaque, psz_encoding, (char *)p_string, i_length);
    }

    return strdup("");
}

static inline char *dvb_string_get(const uint8_t *p_string, size_t i_length,
                                   f_iconv pf_iconv, void *iconv_opaque)
{
    return dvb_string_get_inner(p_string, i_length, pf_iconv, iconv_opaque,
                                "ISO6937");
}

static inline char *dvb_string_get_quirks(const uint8_t *p_string,
        size_t i_length, f_iconv pf_iconv, void *iconv_opaque,
        const char *psz_provider)
{
    if (psz_provider != NULL && !strcmp(psz_provider, "CSAT"))
        return dvb_string_get_inner(p_string, i_length, pf_iconv, iconv_opaque,
                                    "ISO-8859-1");
    return dvb_string_get_inner(p_string, i_length, pf_iconv, iconv_opaque,
                                "ISO6937");
}

static inline char *dvb_string_xml_escape(char *psz_input)
{
    char *psz_output, *psz2;
    char *psz1 = psz_input;
    size_t i_output_size = 0;

    while (*psz1) {
        switch (*psz1) {
            case '<':
            case '>':
                i_output_size += strlen("&lt;");
                break;
            case '&':
                i_output_size += strlen("&amp;");
                break;
            case '"':
            case '\'':
                i_output_size += strlen("&quot;");
                break;
            default:
                i_output_size++;
        }
        psz1++;
    }

    psz2 = psz_output = (char *)malloc(i_output_size + 1);
    psz1 = psz_input;
    while (*psz1) {
        switch (*psz1) {
            case '<':
                memcpy(psz2, "&lt;", strlen("&lt;"));
                psz2 += strlen("&lt;");
                break;
            case '>':
                memcpy(psz2, "&gt;", strlen("&gt;"));
                psz2 += strlen("&gt;");
                break;
            case '&':
                memcpy(psz2, "&amp;", strlen("&amp;"));
                psz2 += strlen("&amp;");
                break;
            case '"':
                memcpy(psz2, "&quot;", strlen("&quot;"));
                psz2 += strlen("&quot;");
                break;
            case '\'':
                memcpy(psz2, "&apos;", strlen("&apos;"));
                psz2 += strlen("&apos;");
                break;
            default:
                *psz2++ = *psz1;
        }
        psz1++;
    }
    *psz2 = '\0';

    free(psz_input);
    return psz_output;
}

#ifdef __cplusplus
}
#endif

#endif
