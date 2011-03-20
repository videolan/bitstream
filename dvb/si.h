/*****************************************************************************
 * si.h: ETSI EN 300 468 Service Information
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
 * $Id$
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 *****************************************************************************/

/*
 * Normative references:
 *  - ISO/IEC 13818-1:2007(E) (MPEG-2 Systems)
 *  - ETSI EN 300 468 V1.11.1 (2010-04) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_SI_H__
#define __BITSTREAM_DVB_SI_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * DVB numbers
 *****************************************************************************/
static inline unsigned int dvb_bcd_get(const uint8_t *p_bcd, uint8_t i_width)
{
    unsigned int i_result = 0;

    while (i_width >= 8) {
        i_result *= 10;
        i_result += (*p_bcd) >> 4;
        i_result *= 10;
        i_result += (*p_bcd) & 0xf;
        i_width -= 8;
        p_bcd++;
    }

    if (i_width == 4) {
        i_result *= 10;
        i_result += (*p_bcd) >> 4;
    }

    return i_result;
}

static inline void dvb_bcd_set(uint8_t *p_bcd, unsigned int i_int,
                               uint8_t i_width)
{
    /* calculate 10^(i_width/4-1) */
    unsigned int i_factor = 1, i_exp = i_width / 4 - 1, i_base = 10;
    while (i_exp)
    {
        if (i_exp & 1)
            i_factor *= i_base;
        i_exp >>= 1;
        i_base *= i_base;
    }

    while (i_factor >= 10) {
        *p_bcd = (i_int / i_factor) << 4;
        i_int %= i_factor;
        i_factor /= 10;
        *p_bcd |= (i_int / i_factor);
        i_int %= i_factor;
        i_factor /= 10;
        p_bcd++;
    }

    if (i_factor == 1) {
        *p_bcd &= 0xf;
        *p_bcd |= i_int << 4;
    }
}

static inline uint8_t dvb_bcd_set8(unsigned int i_int)
{
    uint8_t i_result;
    dvb_bcd_set(&i_result, i_int, 8);
    return i_result;
}

/* EN 300 468 Annex C, year is from 1900 */
static inline uint16_t dvb_mjd_set(int y, int m, int d)
{
    int l = (m == 1 || m == 2) ? 1 : 0;
    int mjd = 14956 + d + (y - l) * 1461 / 4 + (m + 1 + l * 12) * 306001 / 10000;
    return (uint16_t)mjd;
}

static inline void dvb_mjd_get(uint16_t mjd, int *y, int *m, int *d)
{
    int k;
    int yp = (mjd * 20 - 301564) / 7305;
    int mp = (mjd * 10000 - 149561000 - yp * 3652500) / 306001;
    *d = mjd - 14956 - yp * 1461 / 4 - mp * 306001 / 10000;
    k = (mp == 14 || mp == 15) ? 1 : 0;
    *y = yp + k;
    *m = mp - 1 - k * 12;
}

/*****************************************************************************
 * DVB string
 *****************************************************************************/
static const char *ppsz_dvb_encodings[] = {
    /* 0x00 - 0x0f */
    "", "ISO_8859-5", "ISO_8859-6", "ISO_8859-7", "ISO_8859-8",
    "ISO_8859-9", "ISO_8859-10", "ISO_8859-11", "", "ISO_8859-13",
    "ISO_8859-14", "ISO_8859-15", "", "", "", "",

    /* 0x10 - 0x1f */
    "", "UTF-16", "KSC5601-1987", "GB2312", "BIG-5", "UTF-8",
    "", "", "", "", "", "", "", "", "", "", NULL
};
static const char *ppsz_dvb_encodings10[] = {
    "", "ISO_8859-1", "ISO_8859-2", "ISO_8859-3", "ISO_8859-4",
    "ISO_8859-5", "ISO_8859-6", "ISO_8859-7", "ISO_8859-8", "ISO_8859-9",
    "ISO_8859-10", "ISO_8859-11", "", "ISO_8859-13", "ISO_8859-14",
    "ISO_8859-15", NULL
};

static inline const char *dvb_string_get_encoding(const uint8_t **pp_string,
                                                  size_t *pi_length)
{
    uint8_t i_first;

    if (!*pi_length) return NULL;
    i_first = (*pp_string)[0];

    if (!i_first) return NULL;
    if (i_first >= 0x20) return "ISO_8859-1";
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

static inline uint8_t *dvb_string_set(const uint8_t *p_string, size_t i_length,
                                      const char *psz_encoding,
                                      size_t *pi_out_length)
{
    int i;

    if (!strcmp(psz_encoding, "ISO_8859-9")) {
        *pi_out_length = i_length;
        return strdup(p_string);
    }

    for (i = 0; ppsz_dvb_encodings[i] != NULL; i++) {
        if (!strcasecmp(psz_encoding, ppsz_dvb_encodings[i])) {
            uint8_t *p_encoded = malloc(i_length + 1);
            *pi_out_length = i_length + 1;
            p_encoded[0] = i;
            memcpy(p_encoded + 1, p_string, i_length);
            return p_encoded;
        }
    }

    for (i = 0; ppsz_dvb_encodings10[i] != NULL; i++) {
        if (!strcasecmp(psz_encoding, ppsz_dvb_encodings10[i])) {
            uint8_t *p_encoded = malloc(i_length + 3);
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

/* simpler API because this one doesn't output to multibyte charsets */
static inline char *dvb_string_get(const uint8_t *p_string, size_t i_length,
                                   f_iconv pf_iconv, void *iconv_opaque)
{
    if (i_length) {
        const char *psz_encoding = dvb_string_get_encoding(&p_string,
                                                           &i_length);
        if (psz_encoding == NULL || !i_length) {
            /* try one-byte charset */
            char *psz_string = malloc(i_length + 1);
            memcpy(psz_string, p_string, i_length);
            psz_string[i_length] = '\0';
            return psz_string;
        }

        return pf_iconv(iconv_opaque, psz_encoding,
                        p_string, i_length);
    }

    return strdup("");
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

    psz2 = psz_output = malloc(i_output_size + 1);
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

/*****************************************************************************
 * DVB delivery systems
 *****************************************************************************/
static inline const char *dvb_delivery_get_fec(uint8_t i_fec)
{
    switch (i_fec) {
        case 0x0: return "undefined";
        case 0x1: return "1/2";
        case 0x2: return "2/3";
        case 0x3: return "3/4";
        case 0x4: return "5/6";
        case 0x5: return "7/8";
        case 0x6: return "8/9";
        case 0x7: return "3/5";
        case 0x8: return "4/5";
        case 0x9: return "9/10";
        case 0xf: return "none";
        default: return "reserved";
    }
}

/*****************************************************************************
 * Descriptor 0x40: Network name descriptor
 *****************************************************************************/
#define DESC40_HEADER_SIZE      DESC_HEADER_SIZE

static inline void desc40_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x40);
}

static inline void desc40_set_networkname(uint8_t *p_desc,
                                          const uint8_t *p_network_name,
                                          uint8_t i_length)
{
    desc_set_length(p_desc, i_length);
    memcpy(p_desc + 2, p_network_name, i_length);
}

static inline const uint8_t *desc40_get_networkname(const uint8_t *p_desc,
                                                    uint8_t *pi_length)
{
    *pi_length = desc_get_length(p_desc);
    return p_desc + 2;
}

static inline bool desc40_validate(const uint8_t *p_desc)
{
    return true;
}

static inline void desc40_print(const uint8_t *p_desc,
                                f_print pf_print, void *print_opaque,
                                f_iconv pf_iconv, void *iconv_opaque,
                                print_type_t i_print_type)
{
    uint8_t i_network_name_length;
    const uint8_t *p_network_name = desc40_get_networkname(p_desc,
                                                     &i_network_name_length);
    char *psz_network_name = dvb_string_get(p_network_name,
                                            i_network_name_length,
                                            pf_iconv, iconv_opaque);

    switch (i_print_type) {
    case PRINT_XML:
        psz_network_name = dvb_string_xml_escape(psz_network_name);
        pf_print(print_opaque, "<NETWORK_NAME_DESC networkname=\"%s\"/>",
                 psz_network_name);
        break;
    default:
        pf_print(print_opaque, "    - desc 40 networkname=\"%s\"",
                 psz_network_name);
    }
    free(psz_network_name);
}

/*****************************************************************************
 * Descriptor 0x41: Service list descriptor
 *****************************************************************************/
#define DESC41_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC41_SERVICE_SIZE     3

static inline uint8_t *desc41_get_service(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC41_HEADER_SIZE + n * DESC41_SERVICE_SIZE;
    if (p_desc_n + DESC41_SERVICE_SIZE - p_desc
         > desc_get_length(p_desc) + DESC41_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

static inline uint16_t desc41n_get_sid(const uint8_t *p_desc_n)
{
    return (p_desc_n[0] << 8) | p_desc_n[1];
}

static inline uint8_t desc41n_get_type(const uint8_t *p_desc_n)
{
    return p_desc_n[2];
}

static inline bool desc41_validate(const uint8_t *p_desc)
{
    return !(desc_get_length(p_desc) % DESC41_SERVICE_SIZE);
}

static inline void desc41_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc41_get_service(p_desc, j)) != NULL) {
        j++;
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque,
            "<SERVICE_LIST_DESC sid=\"%hu\" type=\"%hhu\" />",
            desc41n_get_sid(p_desc_n), desc41n_get_type(p_desc_n));
            break;
        default:
            pf_print(opaque,
            "    - desc 41 service_list sid=%hu type=%hhu",
            desc41n_get_sid(p_desc_n), desc41n_get_type(p_desc_n));
        }
    }
}

/*****************************************************************************
 * Descriptor 0x43: Satellite delivery system descriptor
 *****************************************************************************/
#define DESC43_HEADER_SIZE      (DESC_HEADER_SIZE + 11)

static inline uint32_t desc43_get_frequency(const uint8_t *p_desc)
{
    return dvb_bcd_get(p_desc + 2, 32) * 10; /* kHz */
}

static inline unsigned int desc43_get_position(const uint8_t *p_desc)
{
    return dvb_bcd_get(p_desc + 6, 16); /* 10th degree */
}

static inline bool desc43_get_east(const uint8_t *p_desc)
{
    return !!(p_desc[8] & 0x80);
}

static inline uint8_t desc43_get_polarization(const uint8_t *p_desc)
{
    return (p_desc[8] & 0x60) >> 5;
}

static inline uint8_t desc43_get_rolloff(const uint8_t *p_desc)
{
    return (p_desc[8] & 0x18) >> 3;
}

static inline bool desc43_get_dvbs2(const uint8_t *p_desc)
{
    return !!(p_desc[8] & 0x4);
}

static inline bool desc43_get_modulation(const uint8_t *p_desc)
{
    return p_desc[8] & 0x3;
}

static inline unsigned int desc43_get_symbolrate(const uint8_t *p_desc)
{
    return dvb_bcd_get(p_desc + 9, 28) * 100; /* sy/s */
}

static inline uint8_t desc43_get_fecinner(const uint8_t *p_desc)
{
    return p_desc[12] & 0xf;
}

static inline bool desc43_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC43_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc43_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    unsigned int i_pos = desc43_get_position(p_desc);
    uint8_t i_polarization = desc43_get_polarization(p_desc);
    const char *psz_polarization = "";;
    uint8_t i_rolloff = desc43_get_rolloff(p_desc);
    uint8_t i_modulation = desc43_get_modulation(p_desc);
    const char *psz_modulation = "";

    switch (i_polarization) {
        case 0x0: psz_polarization = "H"; break;
        case 0x1: psz_polarization = "V"; break;
        case 0x2: psz_polarization = "L"; break;
        case 0x3: psz_polarization = "R"; break;
    }

    switch (i_rolloff) {
        case 0x0: i_rolloff = 35; break;
        case 0x1: i_rolloff = 25; break;
        case 0x2: i_rolloff = 20; break;
    }

    switch (i_modulation) {
        case 0x0: psz_modulation = "auto"; break;
        case 0x1: psz_modulation = "qpsk"; break;
        case 0x2: psz_modulation = "8psk"; break;
        case 0x3: psz_modulation = "16-qam"; break;
    }

    switch (i_print_type) {
    case PRINT_XML:
        if (desc43_get_dvbs2(p_desc))
            pf_print(opaque,
             "<SATELLITE_DESC s2=\"1\" frequency=\"%u%s\" pos=\"%u.%u%c\" rolloff=\"0.%hhu\" modulation=\"%s\" symbolrate=\"%u\" fecinner=\"%s\"/>",
             desc43_get_frequency(p_desc), psz_polarization,
             i_pos / 10, i_pos % 10, desc43_get_east(p_desc) ? 'E' : 'W',
             i_rolloff, psz_modulation, desc43_get_symbolrate(p_desc),
             dvb_delivery_get_fec(desc43_get_fecinner(p_desc)));
        else
            pf_print(opaque,
             "<SATELLITE_DESC s2=\"0\" frequency=\"%u%s\" pos=\"%u.%u%c\" modulation=\"%s\" symbolrate=\"%u\" fecinner=\"%s\"/>",
             desc43_get_frequency(p_desc), psz_polarization,
             i_pos / 10, i_pos % 10, desc43_get_east(p_desc) ? 'E' : 'W',
             psz_modulation, desc43_get_symbolrate(p_desc),
             dvb_delivery_get_fec(desc43_get_fecinner(p_desc)));
        break;
    default:
        if (desc43_get_dvbs2(p_desc))
            pf_print(opaque,
             "    - desc 43 dvb-s2 frequency=%u%s pos=%u.%u%c rolloff=0.%hhu modulation=%s symbolrate=%u fecinner=%s",
             desc43_get_frequency(p_desc), psz_polarization,
             i_pos / 10, i_pos % 10, desc43_get_east(p_desc) ? 'E' : 'W',
             i_rolloff, psz_modulation, desc43_get_symbolrate(p_desc),
             dvb_delivery_get_fec(desc43_get_fecinner(p_desc)));
        else
            pf_print(opaque,
             "    - desc 43 dvb-s frequency=%u%s pos=%u.%u%c modulation=%s symbolrate=%u fecinner=%s",
             desc43_get_frequency(p_desc), psz_polarization,
             i_pos / 10, i_pos % 10, desc43_get_east(p_desc) ? 'E' : 'W',
             psz_modulation, desc43_get_symbolrate(p_desc),
             dvb_delivery_get_fec(desc43_get_fecinner(p_desc)));
    }
}

/*****************************************************************************
 * Descriptor 0x44: Cable delivery system descriptor
 *****************************************************************************/
#define DESC44_HEADER_SIZE      (DESC_HEADER_SIZE + 11)

static inline uint64_t desc44_get_frequency(const uint8_t *p_desc)
{
    return (uint64_t)dvb_bcd_get(p_desc + 2, 32) * 100; /* Hz */
}

static inline uint8_t desc44_get_fecouter(const uint8_t *p_desc)
{
    return p_desc[7] & 0xf;
}

static inline uint8_t desc44_get_modulation(const uint8_t *p_desc)
{
    return p_desc[8];
}

#define desc44_get_symbolrate desc43_get_symbolrate
#define desc44_get_fecinner desc43_get_fecinner

static inline bool desc44_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC44_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc44_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t i_fecouter = desc44_get_fecouter(p_desc);
    const char *psz_fecouter = "reserved";
    uint8_t i_modulation = desc44_get_modulation(p_desc);
    const char *psz_modulation = "reserved";

    switch (i_fecouter) {
        case 0x0: psz_fecouter = "undefined"; break;
        case 0x1: psz_fecouter = "none"; break;
        case 0x2: psz_fecouter = "rs(204/188)"; break;
    }

    switch (i_modulation) {
        case 0x0: psz_modulation = "undefined"; break;
        case 0x1: psz_modulation = "16-qam"; break;
        case 0x2: psz_modulation = "32-qam"; break;
        case 0x3: psz_modulation = "64-qam"; break;
        case 0x4: psz_modulation = "128-qam"; break;
        case 0x5: psz_modulation = "256-qam"; break;
    }

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
             "<CABLE_DESC frequency=\"%"PRIu64"\" fecouter=\"0x%s\" modulation=\"0x%s\" symbolrate=\"%u\" fecinner=\"%s\"/>",
             desc44_get_frequency(p_desc), psz_fecouter, psz_modulation,
             desc44_get_symbolrate(p_desc),
             dvb_delivery_get_fec(desc43_get_fecinner(p_desc)));
        break;
    default:
        pf_print(opaque,
             "    - desc 44 dvb-c frequency=%"PRIu64" Hz fecouter=0x%s modulation=0x%s symbolrate=%u fecinner=%s",
             desc44_get_frequency(p_desc), psz_fecouter, psz_modulation,
             desc44_get_symbolrate(p_desc),
             dvb_delivery_get_fec(desc43_get_fecinner(p_desc)));
    }
}

/*****************************************************************************
 * Descriptor 0x46: VBI teletext descriptor
 *****************************************************************************/
#define DESC46_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC46_LANGUAGE_SIZE    5

static inline void desc46_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x46);
}

static inline uint8_t *desc46_get_language(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC46_HEADER_SIZE + n * DESC46_LANGUAGE_SIZE;
    if (p_desc_n + DESC46_LANGUAGE_SIZE - p_desc
         > desc_get_length(p_desc) + DESC46_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

#define desc46n_set_code desc0an_set_code
#define desc46n_get_code desc0an_get_code

static inline void desc46n_set_teletexttype(uint8_t *p_desc_n, uint8_t i_type)
{
    p_desc_n[3] &= ~0xfc;
    p_desc_n[3] |= (i_type << 3) & 0xfc;
}

static inline uint8_t desc46n_get_teletexttype(const uint8_t *p_desc_n)
{
    return p_desc_n[3] >> 3;
}

static inline void desc46n_set_teletextmagazine(uint8_t *p_desc_n,
                                                uint8_t i_magazine)
{
    p_desc_n[3] &= ~0x3;
    p_desc_n[3] |= (i_magazine & 0x3);
}

static inline uint8_t desc46n_get_teletextmagazine(const uint8_t *p_desc_n)
{
    return p_desc_n[3] & 0x3;
}

static inline void desc46n_set_teletextpage(uint8_t *p_desc_n, uint8_t i_page)
{
    p_desc_n[4] = i_page;
}

static inline uint8_t desc46n_get_teletextpage(const uint8_t *p_desc_n)
{
    return p_desc_n[4];
}

static inline bool desc46_validate(const uint8_t *p_desc)
{
    return !(desc_get_length(p_desc) % DESC46_LANGUAGE_SIZE);
}

static inline void desc46_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc46_get_language(p_desc, j)) != NULL) {
        j++;
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque,
                 "<%s language=\"%3.3s\" type=\"0x%hhx\" mag=\"%hhu\" page=\"%hhu\"/>",
                 desc_get_tag(p_desc) == 0x46 ? "VBI_TELX_DESC" : "TELX_DESC",
                 (const char *)desc46n_get_code(p_desc_n),
                 desc46n_get_teletexttype(p_desc_n),
                 desc46n_get_teletextmagazine(p_desc_n),
                 desc46n_get_teletextpage(p_desc_n));
            break;
        default:
            pf_print(opaque,
                 "    - desc %x telx language=%3.3s type=0x%hhx mag=%hhu page=%hhu",
                 desc_get_tag(p_desc), (const char *)desc46n_get_code(p_desc_n),
                 desc46n_get_teletexttype(p_desc_n),
                 desc46n_get_teletextmagazine(p_desc_n),
                 desc46n_get_teletextpage(p_desc_n));
        }
    }
}

/*****************************************************************************
 * Descriptor 0x48: Service descriptor
 *****************************************************************************/
#define DESC48_HEADER_SIZE      (DESC_HEADER_SIZE + 1)

static inline void desc48_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x48);
}

static inline void desc48_set_type(uint8_t *p_desc, uint8_t i_type)
{
    p_desc[2] = i_type;
}

static inline uint8_t desc48_get_type(const uint8_t *p_desc)
{
    return p_desc[2];
}

static inline void desc48_set_provider(uint8_t *p_desc,
                                       const uint8_t *p_provider,
                                       uint8_t i_length)
{
    uint8_t *p = p_desc + DESC48_HEADER_SIZE;
    p[0] = i_length;
    memcpy(p + 1, p_provider, i_length);
}

static inline const uint8_t *desc48_get_provider(const uint8_t *p_desc,
                                                 uint8_t *pi_length)
{
    const uint8_t *p = p_desc + DESC48_HEADER_SIZE;
    *pi_length = p[0];
    return p + 1;
}

static inline void desc48_set_service(uint8_t *p_desc,
                                      const uint8_t *p_service,
                                      uint8_t i_length)
{
    uint8_t *p = p_desc + DESC48_HEADER_SIZE + 1 + p_desc[3];
    p[0] = i_length;
    memcpy(p + 1, p_service, i_length);
}

static inline uint8_t *desc48_get_service(const uint8_t *p_desc,
                                          uint8_t *pi_length)
{
    const uint8_t *p = p_desc + DESC48_HEADER_SIZE + 1 + p_desc[3];
    *pi_length = p[0];
    return p + 1;
}

static inline bool desc48_validate(const uint8_t *p_desc)
{
    uint8_t i_length = desc_get_length(p_desc);
    const uint8_t *p = p_desc + DESC48_HEADER_SIZE;

    p += *p + 1;
    if (DESC48_HEADER_SIZE + 2 > i_length + DESC_HEADER_SIZE ||
        p + 1 - p_desc > i_length + DESC_HEADER_SIZE)
        return false;

    p += *p + 1;
    if (p - p_desc > i_length + DESC_HEADER_SIZE)
        return false;

    return true;
}

static inline void desc48_print(const uint8_t *p_desc,
                                f_print pf_print, void *print_opaque,
                                f_iconv pf_iconv, void *iconv_opaque,
                                print_type_t i_print_type)
{
    uint8_t i_provider_length, i_service_length;
    const uint8_t *p_provider = desc48_get_provider(p_desc, &i_provider_length);
    const uint8_t *p_service = desc48_get_service(p_desc, &i_service_length);
    char *psz_provider = dvb_string_get(p_provider, i_provider_length,
                                        pf_iconv, iconv_opaque);
    char *psz_service = dvb_string_get(p_service, i_service_length,
                                       pf_iconv, iconv_opaque);
    switch (i_print_type) {
    case PRINT_XML:
        psz_provider = dvb_string_xml_escape(psz_provider);
        psz_service = dvb_string_xml_escape(psz_service);
        pf_print(print_opaque,
                 "<SERVICE_DESC type=\"0x%hhx\" provider=\"%s\" service=\"%s\"/>",
                 desc48_get_type(p_desc), psz_provider, psz_service);
        break;
    default:
        pf_print(print_opaque,
                 "    - desc 48 type=0x%hhx provider=\"%s\" service=\"%s\"",
                 desc48_get_type(p_desc), psz_provider, psz_service);
    }
    free(psz_provider);
    free(psz_service);
}

/*****************************************************************************
 * Descriptor 0x4a: Linkage descriptor (partially implemented)
 *****************************************************************************/
#define DESC4A_HEADER_SIZE      (DESC_HEADER_SIZE + 7)

static inline uint16_t desc4a_get_tsid(const uint8_t *p_desc)
{
    return (p_desc[2] << 8) | p_desc[3];
}

static inline uint16_t desc4a_get_onid(const uint8_t *p_desc)
{
    return (p_desc[4] << 8) | p_desc[5];
}

static inline uint16_t desc4a_get_sid(const uint8_t *p_desc)
{
    return (p_desc[6] << 8) | p_desc[7];
}

static inline uint8_t desc4a_get_linkage(const uint8_t *p_desc)
{
    return p_desc[8];
}

static inline bool desc4a_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC4A_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc4a_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
             "<LINKAGE_DESC tsid=\"%hu\" onid=\"%hu\" sid=\"%hu\" linkage=\"%hhu\" />",
             desc4a_get_tsid(p_desc), desc4a_get_onid(p_desc),
             desc4a_get_sid(p_desc), desc4a_get_linkage(p_desc));
        break;
    default:
        pf_print(opaque,
             "    - desc 4a linkage tsid=%hu onid=%hu sid=%hu linkage=%hhu",
             desc4a_get_tsid(p_desc), desc4a_get_onid(p_desc),
             desc4a_get_sid(p_desc), desc4a_get_linkage(p_desc));
    }
}

/*****************************************************************************
 * Descriptor 0x56: Teletext descriptor
 *****************************************************************************/
#define DESC56_HEADER_SIZE      DESC46_HEADER_SIZE
#define DESC56_LANGUAGE_SIZE    DESC46_LANGUAGE_SIZE

static inline void desc56_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x56);
}

#define desc56_get_language desc46_get_language
#define desc56n_set_code desc46n_set_code
#define desc56n_get_code desc46n_get_code
#define desc56n_set_teletexttype desc46n_set_teletexttype
#define desc56n_get_teletexttype desc46n_get_teletexttype
#define desc56n_set_teletextmagazine desc46n_set_teletextmagazine
#define desc56n_get_teletextmagazine desc46n_get_teletextmagazine
#define desc56n_set_teletextpage desc46n_set_teletextpage
#define desc56n_get_teletextpage desc46n_get_teletextpage
#define desc56_validate desc46_validate
#define desc56_print desc46_print

/*****************************************************************************
 * Descriptor 0x59: Subtitling descriptor
 *****************************************************************************/
#define DESC59_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC59_LANGUAGE_SIZE    8

static inline void desc59_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x59);
}

static inline uint8_t *desc59_get_language(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC59_HEADER_SIZE + n * DESC59_LANGUAGE_SIZE;
    if (p_desc_n + DESC59_LANGUAGE_SIZE - p_desc
         > desc_get_length(p_desc) + DESC59_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

#define desc59n_set_code desc0an_set_code
#define desc59n_get_code desc0an_get_code

static inline void desc59n_set_subtitlingtype(uint8_t *p_desc_n, uint8_t i_type)
{
    p_desc_n[3] = i_type;
}

static inline uint8_t desc59n_get_subtitlingtype(const uint8_t *p_desc_n)
{
    return p_desc_n[3];
}

static inline void desc59n_set_compositionpage(uint8_t *p_desc_n,
                                               uint16_t i_page)
{
    p_desc_n[4] = i_page >> 8;
    p_desc_n[5] = i_page & 0xff;
}

static inline uint16_t desc59n_get_compositionpage(const uint8_t *p_desc_n)
{
    return (p_desc_n[4] << 8) | p_desc_n[5];
}

static inline void desc59n_set_ancillarypage(uint8_t *p_desc_n, uint16_t i_page)
{
    p_desc_n[6] = i_page >> 8;
    p_desc_n[7] = i_page & 0xff;
}

static inline uint16_t desc59n_get_ancillarypage(const uint8_t *p_desc_n)
{
    return (p_desc_n[6] << 8) | p_desc_n[7];
}

static inline bool desc59_validate(const uint8_t *p_desc)
{
    return !(desc_get_length(p_desc) % DESC59_LANGUAGE_SIZE);
}

static inline void desc59_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc59_get_language(p_desc, j)) != NULL) {
        j++;
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque,
            "<SUBTITLING_DESC language=\"%3.3s\" type=\"0x%hhx\" composition=\"%hu\" ancillary=\"%hu\"/>",
            (const char *)desc59n_get_code(p_desc_n),
            desc59n_get_subtitlingtype(p_desc_n),
            desc59n_get_compositionpage(p_desc_n),
            desc59n_get_ancillarypage(p_desc_n));
            break;
        default:
            pf_print(opaque,
            "    - desc 59 dvbs language=%3.3s type=0x%hhx composition=%hu ancillary=%hu",
            (const char *)desc59n_get_code(p_desc_n),
            desc59n_get_subtitlingtype(p_desc_n),
            desc59n_get_compositionpage(p_desc_n),
            desc59n_get_ancillarypage(p_desc_n));
        }
    }
}

/*****************************************************************************
 * Descriptor 0x5a: Terrestrial delivery system descriptor
 *****************************************************************************/
#define DESC5A_HEADER_SIZE      (DESC_HEADER_SIZE + 11)

static inline uint64_t desc5a_get_frequency(const uint8_t *p_desc)
{
    return (((uint64_t)p_desc[2] << 24) | (p_desc[3] << 16) | (p_desc[4] << 8)
             | p_desc[5]) * 10; /* Hz */
}

static inline uint8_t desc5a_get_bandwidth(const uint8_t *p_desc)
{
    return p_desc[6] >> 5;
}

static inline bool desc5a_get_priority(const uint8_t *p_desc)
{
    return !!((p_desc[6] >> 4) & 0x1);
}

/* ! inverted logic ! */
static inline bool desc5a_get_timeslicing(const uint8_t *p_desc)
{
    return !((p_desc[6] >> 3) & 0x1);
}

/* ! inverted logic ! */
static inline bool desc5a_get_mpefec(const uint8_t *p_desc)
{
    return !((p_desc[6] >> 2) & 0x1);
}

static inline uint8_t desc5a_get_constellation(const uint8_t *p_desc)
{
    return p_desc[7] >> 6;
}

static inline uint8_t desc5a_get_hierarchy(const uint8_t *p_desc)
{
    return (p_desc[7] >> 3) & 0x7;
}

static inline uint8_t desc5a_get_coderatehp(const uint8_t *p_desc)
{
    return p_desc[7] & 0x7;
}

static inline uint8_t desc5a_get_coderatelp(const uint8_t *p_desc)
{
    return p_desc[8] >> 5;
}

static inline uint8_t desc5a_get_guard(const uint8_t *p_desc)
{
    return (p_desc[8] >> 3) & 0x3;
}

static inline uint8_t desc5a_get_transmission(const uint8_t *p_desc)
{
    return (p_desc[8] >> 1) & 0x3;
}

static inline bool desc5a_get_otherfrequency(const uint8_t *p_desc)
{
    return !!(p_desc[8] & 0x1);
}

static inline bool desc5a_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC5A_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc5a_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    uint8_t i_bandwidth = desc5a_get_bandwidth(p_desc);
    uint8_t i_constellation = desc5a_get_constellation(p_desc);
    const char *psz_constellation = "reserved";
    uint8_t i_hierarchy = desc5a_get_hierarchy(p_desc);
    const char *psz_hierarchy = "";
    bool b_hierarchy = !!(i_hierarchy & 0x3);
    uint8_t i_guard = desc5a_get_guard(p_desc);
    const char *psz_guard = "";
    uint8_t i_transmission = desc5a_get_transmission(p_desc);
    const char *psz_transmission = "reserved";

    switch (i_bandwidth) {
        case 0x0: i_bandwidth = 8; break;
        case 0x1: i_bandwidth = 7; break;
        case 0x2: i_bandwidth = 6; break;
        case 0x3: i_bandwidth = 5; break;
        default: i_bandwidth = 0; break;
    }

    switch (i_constellation) {
        case 0x0: psz_constellation = "QPSK"; break;
        case 0x1: psz_constellation = "16-qam"; break;
        case 0x2: psz_constellation = "64-qam"; break;
    }

    switch (i_hierarchy) {
        case 0x0: psz_hierarchy = "none"; break;
        case 0x1: psz_hierarchy = "1"; break;
        case 0x2: psz_hierarchy = "2"; break;
        case 0x3: psz_hierarchy = "4"; break;
        case 0x4: psz_hierarchy = "none+in-depth"; break;
        case 0x5: psz_hierarchy = "1+in-depth"; break;
        case 0x6: psz_hierarchy = "2+in-depth"; break;
        case 0x7: psz_hierarchy = "4+in-depth"; break;
    }

    switch (i_guard) {
        case 0x0: psz_guard = "1/32"; break;
        case 0x1: psz_guard = "1/16"; break;
        case 0x2: psz_guard = "1/8"; break;
        case 0x3: psz_guard = "1/4"; break;
    }

    switch (i_transmission) {
        case 0x0: psz_transmission = "2k"; break;
        case 0x1: psz_transmission = "8k"; break;
        case 0x2: psz_transmission = "4k"; break;
    }

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
             "<TERRESTRIAL_DESC frequency=\"%"PRIu64"\" bandwidth=\"%u\" priority=\"%s\" timeslicing=\"%d\" mpefec=\"%d\" constellation=\"%s\" hierarchy=\"%s\" coderatehp=\"%s\" coderatelp=\"%s\" guard=\"%s\" transmission=\"%s\" otherfrequency=\"%d\"/>",
             desc5a_get_frequency(p_desc), i_bandwidth,
             desc5a_get_priority(p_desc) ? "HP" : "LP",
             desc5a_get_timeslicing(p_desc) ? 1 : 0,
             desc5a_get_mpefec(p_desc) ? 1 : 0, psz_constellation,
             psz_hierarchy,
             dvb_delivery_get_fec(desc5a_get_coderatehp(p_desc)),
             b_hierarchy ? dvb_delivery_get_fec(desc5a_get_coderatehp(p_desc)) : "not_applicable",
             psz_guard, psz_transmission,
             desc5a_get_otherfrequency(p_desc) ? 1 : 0);
        break;
    default:
        pf_print(opaque,
             "    - desc 5a dvb-t frequency=%"PRIu64" Hz bandwidth=%u MHz priority=%s%s%s constellation=%s hierarchy=%s coderatehp=%s%s%s guard=%s transmission=%s%s",
             desc5a_get_frequency(p_desc), i_bandwidth,
             desc5a_get_priority(p_desc) ? "HP" : "LP",
             desc5a_get_timeslicing(p_desc) ? " timeslicing" : "",
             desc5a_get_mpefec(p_desc) ? " mpefec" : "", psz_constellation,
             psz_hierarchy,
             dvb_delivery_get_fec(desc5a_get_coderatehp(p_desc)),
             b_hierarchy ? "coderatelp=" : "",
             b_hierarchy ? dvb_delivery_get_fec(desc5a_get_coderatehp(p_desc)) : "",
             psz_guard, psz_transmission,
             desc5a_get_otherfrequency(p_desc) ? " otherfrequency" : "");
    }
}

/*****************************************************************************
 * Descriptor 0x5f: Private data specifier descriptor
 *****************************************************************************/
#define DESC5F_HEADER_SIZE      (DESC_HEADER_SIZE + 4)

static inline void desc5f_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x5f);
}

static inline uint32_t desc5f_get_specifier(const uint8_t *p_desc)
{
    return (p_desc[2] << 24) | (p_desc[3] << 16) |
           (p_desc[4] << 8) | p_desc[5];
}

static inline bool desc5f_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC5F_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc5f_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<PRIVATE_DATA_SPECIFIER_DESC specifier=\"%u\" />",
                 desc5f_get_specifier(p_desc));
        break;
    default:
        pf_print(opaque, "    - desc 5f private data specifier=%u",
                 desc5f_get_specifier(p_desc));
    }
}

/*****************************************************************************
 * Descriptor 0x6a: AC-3 descriptor
 *****************************************************************************/
#define DESC6A_HEADER_SIZE      (DESC_HEADER_SIZE + 1)

static inline void desc6a_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x6a);
}

static inline void desc6a_clear_flags(uint8_t *p_desc)
{
    p_desc[2] = 0;
}

static inline bool desc6a_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC6A_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc6a_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<AC3_DESC />");
        break;
    default:
        pf_print(opaque, "    - desc 6a ac3");
    }
}

/*****************************************************************************
 * Descriptor 0x83: Logical channel descriptor (IEC/CENELEC 62 216)
 * Only valid if a private data specifier descriptor 28 is present before.
 *****************************************************************************/
#define DESC83P28_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC83P28_SERVICE_SIZE     4

static inline void desc83p28_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x83);
}

static inline uint8_t *desc83p28_get_service(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC83P28_HEADER_SIZE
                         + n * DESC83P28_SERVICE_SIZE;
    if (p_desc_n + DESC83P28_SERVICE_SIZE - p_desc
         > desc_get_length(p_desc) + DESC83P28_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

static inline uint16_t desc83p28n_get_sid(const uint8_t *p_desc_n)
{
    return (p_desc_n[0] << 8) | p_desc_n[1];
}

static inline bool desc83p28n_get_visible(const uint8_t *p_desc_n)
{
    return !!(p_desc_n[2] & 0x80);
}

static inline uint16_t desc83p28n_get_lcn(const uint8_t *p_desc_n)
{
    return ((p_desc_n[2] & 0x3) << 8) | p_desc_n[3];
}

static inline bool desc83p28_validate(const uint8_t *p_desc)
{
    return !(desc_get_length(p_desc) % DESC83P28_SERVICE_SIZE);
}

static inline void desc83p28_print(uint8_t *p_desc, f_print pf_print,
                                   void *opaque, print_type_t i_print_type)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc83p28_get_service(p_desc, j)) != NULL) {
        j++;
        switch (i_print_type) {
        case PRINT_XML:
            pf_print(opaque,
            "<%s sid=\"%hu\" visible=\"%d\" lcn=\"%hu\" />",
            desc_get_tag(p_desc) == 0x88 ? "HD_SIMULCAST_LCN_DESC" : "LCN_DESC",
            desc83p28n_get_sid(p_desc_n),
            desc83p28n_get_visible(p_desc_n) ? 1 : 0,
            desc83p28n_get_lcn(p_desc_n));
            break;
        default:
            pf_print(opaque,
            "    - desc %hhu lcn sid=%hu%s lcn=%hu", desc_get_tag(p_desc),
            desc83p28n_get_sid(p_desc_n),
            desc83p28n_get_visible(p_desc_n) ? " visible" : "",
            desc83p28n_get_lcn(p_desc_n));
        }
    }
}

/*****************************************************************************
 * Descriptor 0x88: HD simulcast logical channel descriptor (IEC/CENELEC 62 216)
 * Only valid if a private data specifier descriptor 28 is present before.
 *****************************************************************************/
#define DESC88P28_HEADER_SIZE      DESC83P28_HEADER_SIZE
#define DESC88P28_SERVICE_SIZE     DESC83P28_SERVICE_SIZE

static inline void desc88p28_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x88);
}

#define desc88p28_get_service desc83p28_get_service
#define desc88p28_get_sid desc83p28_get_sid
#define desc88p28_get_visible desc83p28_get_visible
#define desc88p28_get_lcn desc83p28_get_lcn
#define desc88p28_validate desc83p28_validate
#define desc88p28_print desc83p28_print

/*****************************************************************************
 * Network Information Table
 *****************************************************************************/
#define NIT_PID                 0x10
#define NIT_TABLE_ID_ACTUAL     0x40
#define NIT_TABLE_ID_OTHER      0x41
#define NIT_HEADER_SIZE         (PSI_HEADER_SIZE_SYNTAX1 + 2)
#define NIT_HEADER2_SIZE        2
#define NIT_TS_SIZE             6

#define nit_set_nid psi_set_tableidext
#define nit_get_nid psi_get_tableidext

static inline void nit_init(uint8_t *p_nit, bool b_actual)
{
    psi_init(p_nit, true);
    psi_set_tableid(p_nit, b_actual ? NIT_TABLE_ID_ACTUAL : NIT_TABLE_ID_OTHER);
    p_nit[8] = 0xf0;
}

static inline void nit_set_length(uint8_t *p_nit, uint16_t i_nit_length)
{
    psi_set_length(p_nit, NIT_HEADER_SIZE + PSI_CRC_SIZE - PSI_HEADER_SIZE
                    + i_nit_length);
}

static inline void nit_set_desclength(uint8_t *p_nit, uint16_t i_length)
{
    p_nit[8] &= ~0xf;
    p_nit[8] |= i_length >> 8;
    p_nit[9] = i_length & 0xff;
}

static inline uint16_t nit_get_desclength(const uint8_t *p_nit)
{
    return ((p_nit[8] & 0xf) << 8) | p_nit[9];
}

static inline uint8_t *nit_get_descs(uint8_t *p_nit)
{
    return &p_nit[8];
}

static inline void nith_init(uint8_t *p_nit_h)
{
    p_nit_h[0] = 0xf0;
}

static inline void nith_set_tslength(uint8_t *p_nit_h, uint16_t i_length)
{
    p_nit_h[0] &= ~0xf;
    p_nit_h[0] |= i_length >> 8;
    p_nit_h[1] = i_length & 0xff;
}

static inline uint16_t nith_get_tslength(const uint8_t *p_nit_h)
{
    return ((p_nit_h[0] & 0xf) << 8) | p_nit_h[1];
}

static inline void nitn_init(uint8_t *p_nit_n)
{
    p_nit_n[4] = 0xf0;
}

static inline void nitn_set_tsid(uint8_t *p_nit_n, uint16_t i_tsid)
{
    p_nit_n[0] = i_tsid >> 8;
    p_nit_n[1] = i_tsid & 0xff;
}

static inline uint16_t nitn_get_tsid(const uint8_t *p_nit_n)
{
    return (p_nit_n[0] << 8) | p_nit_n[1];
}

static inline void nitn_set_onid(uint8_t *p_nit_n, uint16_t i_onid)
{
    p_nit_n[2] = i_onid >> 8;
    p_nit_n[3] = i_onid & 0xff;
}

static inline uint16_t nitn_get_onid(const uint8_t *p_nit_n)
{
    return (p_nit_n[2] << 8) | p_nit_n[3];
}

static inline void nitn_set_desclength(uint8_t *p_nit_n, uint16_t i_length)
{
    p_nit_n[4] &= ~0xf;
    p_nit_n[4] |= i_length >> 8;
    p_nit_n[5] = i_length & 0xff;
}

static inline uint16_t nitn_get_desclength(const uint8_t *p_nit_n)
{
    return ((p_nit_n[4] & 0xf) << 8) | p_nit_n[5];
}

static inline uint8_t *nitn_get_descs(uint8_t *p_nit_n)
{
    return &p_nit_n[4];
}

static inline uint8_t *nit_get_header2(uint8_t *p_nit)
{
    return p_nit + NIT_HEADER_SIZE + nit_get_desclength(p_nit);
}

static inline uint8_t *nit_get_ts(uint8_t *p_nit, uint8_t n)
{
    uint16_t i_section_size = psi_get_length(p_nit) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    uint8_t *p_nit_n = p_nit + NIT_HEADER_SIZE + nit_get_desclength(p_nit)
                        + NIT_HEADER2_SIZE;
    if (p_nit_n - p_nit > i_section_size) return NULL;

    while (n) {
        if (p_nit_n + NIT_TS_SIZE - p_nit > i_section_size) return NULL;
        p_nit_n += NIT_TS_SIZE + nitn_get_desclength(p_nit_n);
        n--;
    }
    if (p_nit_n - p_nit >= i_section_size) return NULL;
    return p_nit_n;
}

static inline bool nit_validate_ts(const uint8_t *p_nit, const uint8_t *p_nit_n,
                                   uint16_t i_desclength)
{
    uint16_t i_section_size = psi_get_length(p_nit) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    return (p_nit_n + NIT_TS_SIZE + i_desclength
             <= p_nit + i_section_size);
}

static inline bool nit_validate(const uint8_t *p_nit)
{
    uint16_t i_section_size = psi_get_length(p_nit) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    const uint8_t *p_nit_n;

    if (!psi_get_syntax(p_nit)
         || (psi_get_tableid(p_nit) != NIT_TABLE_ID_ACTUAL
              && psi_get_tableid(p_nit) != NIT_TABLE_ID_OTHER))
        return false;

    if (i_section_size < NIT_HEADER_SIZE
         || i_section_size < NIT_HEADER_SIZE + nit_get_desclength(p_nit))
        return false;

    if (!descs_validate(p_nit + 8))
        return false;

    p_nit_n = p_nit + NIT_HEADER_SIZE + nit_get_desclength(p_nit);

    if (nith_get_tslength(p_nit_n) != p_nit + i_section_size - p_nit_n
         - NIT_HEADER2_SIZE)
        return false;
    p_nit_n += NIT_HEADER2_SIZE;

    while (p_nit_n + NIT_TS_SIZE - p_nit <= i_section_size
            && p_nit_n + NIT_TS_SIZE + nitn_get_desclength(p_nit_n) - p_nit
                <= i_section_size) {
        if (!descs_validate(p_nit_n + 4))
            return false;

        p_nit_n += NIT_TS_SIZE + nitn_get_desclength(p_nit_n);
    }

    return (p_nit_n - p_nit == i_section_size);
}


static inline uint8_t *nit_table_find_ts(uint8_t **pp_sections,
                                         uint16_t i_tsid, uint16_t i_onid)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);
        uint8_t *p_ts;
        int j = 0;

        while ((p_ts = nit_get_ts(p_section, j)) != NULL) {
            j++;
            if (nitn_get_tsid(p_ts) == i_tsid && nitn_get_onid(p_ts) == i_onid)
                return p_ts;
        }
    }

    return NULL;
}

static inline bool nit_table_validate(uint8_t **pp_sections)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);
        uint8_t *p_ts;
        int j = 0;

        if (!psi_check_crc(p_section))
            return false;

        while ((p_ts = nit_get_ts(p_section, j)) != NULL) {
            j++;
            /* check that the TS is not already in the table */
            if (nit_table_find_ts(pp_sections, nitn_get_tsid(p_ts),
                                  nitn_get_onid(p_ts)) != p_ts)
                return false;
        }
    }

    return true;
}

/*****************************************************************************
 * Service Description Table
 *****************************************************************************/
#define SDT_PID                 0x11
#define SDT_TABLE_ID_ACTUAL     0x42
#define SDT_TABLE_ID_OTHER      0x46
#define SDT_HEADER_SIZE         (PSI_HEADER_SIZE_SYNTAX1 + 3)
#define SDT_SERVICE_SIZE        5

#define sdt_set_tsid psi_set_tableidext
#define sdt_get_tsid psi_get_tableidext

static inline void sdt_init(uint8_t *p_sdt, bool b_actual)
{
    psi_init(p_sdt, true);
    psi_set_tableid(p_sdt, b_actual ? SDT_TABLE_ID_ACTUAL : SDT_TABLE_ID_OTHER);
    p_sdt[10] = 0xff;
}

static inline void sdt_set_length(uint8_t *p_sdt, uint16_t i_sdt_length)
{
    psi_set_length(p_sdt, SDT_HEADER_SIZE + PSI_CRC_SIZE - PSI_HEADER_SIZE
                    + i_sdt_length);
}

static inline void sdt_set_onid(uint8_t *p_sdt, uint16_t i_onid)
{
    p_sdt[8] = i_onid >> 8;
    p_sdt[9] = i_onid & 0xff;
}

static inline uint16_t sdt_get_onid(const uint8_t *p_sdt)
{
    return (p_sdt[8] << 8) | p_sdt[9];
}

static inline void sdtn_init(uint8_t *p_sdt_n)
{
    p_sdt_n[2] = 0xfc;
    p_sdt_n[3] = 0;
}

static inline void sdtn_set_sid(uint8_t *p_sdt_n, uint16_t i_sid)
{
    p_sdt_n[0] = i_sid >> 8;
    p_sdt_n[1] = i_sid & 0xff;
}

static inline uint16_t sdtn_get_sid(const uint8_t *p_sdt_n)
{
    return (p_sdt_n[0] << 8) | p_sdt_n[1];
}

static inline void sdtn_set_eitschedule(uint8_t *p_sdt_n)
{
    p_sdt_n[2] |= 0x2;
}

static inline bool sdtn_get_eitschedule(const uint8_t *p_sdt_n)
{
    return !!(p_sdt_n[2] & 0x2);
}

static inline void sdtn_set_eitpresent(uint8_t *p_sdt_n)
{
    p_sdt_n[2] |= 0x1;
}

static inline bool sdtn_get_eitpresent(const uint8_t *p_sdt_n)
{
    return !!(p_sdt_n[2] & 0x1);
}

static inline void sdtn_set_running(uint8_t *p_sdt_n, uint8_t i_running)
{
    p_sdt_n[3] &= 0x1f;
    p_sdt_n[3] |= i_running << 5;
}

static inline uint8_t sdtn_get_running(const uint8_t *p_sdt_n)
{
    return p_sdt_n[3] >> 5;
}

static inline void sdtn_set_ca(uint8_t *p_sdt_n)
{
    p_sdt_n[3] |= 0x10;
}

static inline bool sdtn_get_ca(const uint8_t *p_sdt_n)
{
    return !!(p_sdt_n[3] & 0x10);
}

static inline void sdtn_set_desclength(uint8_t *p_sdt_n, uint16_t i_length)
{
    p_sdt_n[3] &= ~0xf;
    p_sdt_n[3] |= (i_length >> 8) & 0xf;
    p_sdt_n[4] = i_length & 0xff;
}

static inline uint16_t sdtn_get_desclength(const uint8_t *p_sdt_n)
{
    return ((p_sdt_n[3] & 0xf) << 8) | p_sdt_n[4];
}

static inline uint8_t *sdtn_get_descs(uint8_t *p_sdt_n)
{
    return &p_sdt_n[3];
}

static inline uint8_t *sdt_get_service(uint8_t *p_sdt, uint8_t n)
{
    uint16_t i_section_size = psi_get_length(p_sdt) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    uint8_t *p_sdt_n = p_sdt + SDT_HEADER_SIZE;

    while (n) {
        if (p_sdt_n + SDT_SERVICE_SIZE - p_sdt > i_section_size) return NULL;
        p_sdt_n += SDT_SERVICE_SIZE + sdtn_get_desclength(p_sdt_n);
        n--;
    }
    if (p_sdt_n - p_sdt >= i_section_size) return NULL;
    return p_sdt_n;
}

static inline bool sdt_validate_service(const uint8_t *p_sdt,
                                        const uint8_t *p_sdt_n,
                                        uint16_t i_desclength)
{
    uint16_t i_section_size = psi_get_length(p_sdt) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    return (p_sdt_n + SDT_SERVICE_SIZE + i_desclength
             <= p_sdt + i_section_size);
}

static inline bool sdt_validate(const uint8_t *p_sdt)
{
    uint16_t i_section_size = psi_get_length(p_sdt) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    const uint8_t *p_sdt_n;

    if (!psi_get_syntax(p_sdt)
         || (psi_get_tableid(p_sdt) != SDT_TABLE_ID_ACTUAL
              && psi_get_tableid(p_sdt) != SDT_TABLE_ID_OTHER))
        return false;

    p_sdt_n = p_sdt + SDT_HEADER_SIZE;

    while (p_sdt_n + SDT_SERVICE_SIZE - p_sdt <= i_section_size
            && p_sdt_n + SDT_SERVICE_SIZE + sdtn_get_desclength(p_sdt_n) - p_sdt
                <= i_section_size) {
        if (!descs_validate(p_sdt_n + 3))
            return false;

        p_sdt_n += SDT_SERVICE_SIZE + sdtn_get_desclength(p_sdt_n);
    }

    return (p_sdt_n - p_sdt == i_section_size);
}

static inline uint8_t *sdt_table_find_service(uint8_t **pp_sections,
                                              uint16_t i_sid)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);
        uint8_t *p_service;
        int j = 0;

        while ((p_service = sdt_get_service(p_section, j)) != NULL) {
            if (sdtn_get_sid(p_service) == i_sid)
                return p_service;
            j++;
        }
    }

    return NULL;
}

static inline bool sdt_table_validate(uint8_t **pp_sections)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);
        uint8_t *p_service;
        int j = 0;

        if (!psi_check_crc(p_section))
            return false;

        while ((p_service = sdt_get_service(p_section, j)) != NULL) {
            j++;
            /* check that the service is not already in the table */
            if (sdt_table_find_service(pp_sections, sdtn_get_sid(p_service))
                 != p_service)
                return false;
        }
    }

    return true;
}

/*****************************************************************************
 * Event Information Table
 *****************************************************************************/
#define EIT_PID                         0x12
#define EIT_TABLE_ID_PF_ACTUAL          0x4e
#define EIT_TABLE_ID_PF_OTHER           0x4f
#define EIT_TABLE_ID_SCHED_ACTUAL_FIRST 0x50
#define EIT_TABLE_ID_SCHED_ACTUAL_LAST  0x5f
#define EIT_TABLE_ID_SCHED_OTHER_FIRST  0x60
#define EIT_TABLE_ID_SCHED_OTHER_LAST   0x6f
#define EIT_HEADER_SIZE                 (PSI_HEADER_SIZE_SYNTAX1 + 6)
#define EIT_EVENT_SIZE                  12

#define eit_set_sid psi_set_tableidext
#define eit_get_sid psi_get_tableidext

static inline void eit_init(uint8_t *p_eit, bool b_actual)
{
    psi_init(p_eit, true);
    psi_set_tableid(p_eit, b_actual ? EIT_TABLE_ID_PF_ACTUAL :
                    EIT_TABLE_ID_PF_OTHER);
}

static inline void eit_set_length(uint8_t *p_eit, uint16_t i_eit_length)
{
    psi_set_length(p_eit, EIT_HEADER_SIZE + PSI_CRC_SIZE - PSI_HEADER_SIZE
                    + i_eit_length);
}

static inline void eit_set_tsid(uint8_t *p_eit, uint16_t i_tsid)
{
    p_eit[8] = i_tsid >> 8;
    p_eit[9] = i_tsid & 0xff;
}

static inline uint16_t eit_get_tsid(const uint8_t *p_eit)
{
    return (p_eit[8] << 8) | p_eit[9];
}

static inline uint16_t eitn_get_desclength(const uint8_t *p_eit_n)
{
    return ((p_eit_n[10] & 0xf) << 8) | p_eit_n[11];
}

static inline bool eit_validate_event(const uint8_t *p_eit,
                                      const uint8_t *p_eit_n,
                                      uint16_t i_desclength)
{
    uint16_t i_section_size = psi_get_length(p_eit) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    return (p_eit_n + EIT_EVENT_SIZE + i_desclength
             <= p_eit + i_section_size);
}

static inline bool eit_validate(const uint8_t *p_eit)
{
    uint16_t i_section_size = psi_get_length(p_eit) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    uint8_t i_tid = psi_get_tableid(p_eit);
    const uint8_t *p_eit_n;

    if (!psi_get_syntax(p_eit)
         || (i_tid != EIT_TABLE_ID_PF_ACTUAL
              && i_tid != EIT_TABLE_ID_PF_OTHER
              && !(i_tid >= EIT_TABLE_ID_SCHED_ACTUAL_FIRST
                    && i_tid <= EIT_TABLE_ID_SCHED_ACTUAL_LAST)
              && !(i_tid >= EIT_TABLE_ID_SCHED_OTHER_FIRST
                    && i_tid <= EIT_TABLE_ID_SCHED_OTHER_LAST)))
        return false;

    if (!psi_check_crc(p_eit))
        return false;

    p_eit_n = p_eit + EIT_HEADER_SIZE;

    while (p_eit_n + EIT_EVENT_SIZE - p_eit <= i_section_size
            && p_eit_n + EIT_EVENT_SIZE + eitn_get_desclength(p_eit_n) - p_eit
                <= i_section_size) {
        if (!descs_validate(p_eit_n + 10))
            return false;

        p_eit_n += EIT_EVENT_SIZE + eitn_get_desclength(p_eit_n);
    }

    return (p_eit_n - p_eit == i_section_size);
}

/* TODO: unfinished support */

/*****************************************************************************
 * Running Status Table
 *****************************************************************************/
#define RST_PID                 0x13
#define RST_TABLE_ID            0x71
#define RST_HEADER_SIZE         PSI_HEADER_SIZE
#define RST_EVENT_SIZE          9

/* TODO: unfinished support */

/*****************************************************************************
 * Time and Date Table
 *****************************************************************************/
#define TDT_PID                 0x14
#define TDT_TABLE_ID            0x70
#define TDT_HEADER_SIZE         (PSI_HEADER_SIZE + 5)

static inline void tdt_init(uint8_t *p_tdt)
{
    psi_init(p_tdt, false);
    psi_set_tableid(p_tdt, TDT_TABLE_ID);
    psi_set_length(p_tdt, TDT_HEADER_SIZE - PSI_HEADER_SIZE);
}

static inline void tdt_set_utc(uint8_t *p_tdt, uint64_t i_utc)
{
    p_tdt[3] = (i_utc >> 32) & 0xff;
    p_tdt[4] = (i_utc >> 24) & 0xff;
    p_tdt[5] = (i_utc >> 16) & 0xff;
    p_tdt[6] = (i_utc >> 8) & 0xff;
    p_tdt[7] = i_utc & 0xff;
}

static inline uint64_t tdt_get_utc(const uint8_t *p_tdt)
{
    return ((uint64_t)p_tdt[3] << 32) | (p_tdt[4] << 24) | (p_tdt[5] << 16)
             | (p_tdt[6] << 8) | p_tdt[7];
}

static inline bool tdt_validate(const uint8_t *p_tdt)
{
    uint16_t i_section_size = psi_get_length(p_tdt) + PSI_HEADER_SIZE;
    uint8_t i_tid = psi_get_tableid(p_tdt);

    if (psi_get_syntax(p_tdt) || i_tid != TDT_TABLE_ID
         || i_section_size < TDT_HEADER_SIZE)
        return false;

    return true;
}

/*****************************************************************************
 * Time Offset Table
 *****************************************************************************/
#define TOT_PID                 0x14
#define TOT_TABLE_ID            0x73
#define TOT_HEADER_SIZE         (PSI_HEADER_SIZE + 7)

#define tot_set_utc tdt_set_utc
#define tot_get_utc tdt_get_utc

static inline void tot_init(uint8_t *p_tot)
{
    psi_init(p_tot, false);
    psi_set_tableid(p_tot, TOT_TABLE_ID);
    p_tot[8] = 0xf0;
}

static inline void tot_set_desclength(uint8_t *p_tot, uint16_t i_length)
{
    p_tot[8] &= ~0xf;
    p_tot[8] |= i_length >> 8;
    p_tot[9] = i_length & 0xff;
}

static inline uint16_t tot_get_desclength(const uint8_t *p_tot)
{
    return ((p_tot[8] & 0xf) << 8) | p_tot[9];
}

static inline bool tot_validate(const uint8_t *p_tot)
{
    uint16_t i_section_size = psi_get_length(p_tot) + PSI_HEADER_SIZE;
    uint8_t i_tid = psi_get_tableid(p_tot);

    if (psi_get_syntax(p_tot) || i_tid != TOT_TABLE_ID
         || i_section_size < TOT_HEADER_SIZE + PSI_CRC_SIZE)
        return false;

    /* TOT is a syntax0 table with CRC */
    if (!psi_check_crc(p_tot))
        return false;

    if (i_section_size < TOT_HEADER_SIZE
         || i_section_size < TOT_HEADER_SIZE + tot_get_desclength(p_tot))
        return false;

    if (!descs_validate(p_tot + 8))
        return false;

    return true;
}

#ifdef __cplusplus
}
#endif

#endif
