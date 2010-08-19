/*****************************************************************************
 * psi.h: ISO/IEC 13818-1 Program Stream Information
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
 */

#ifndef __BITSTREAM_MPEG_PSI_H__
#define __BITSTREAM_MPEG_PSI_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/ts.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor
 *****************************************************************************/
#define DESC_HEADER_SIZE        2

static inline void desc_set_tag(uint8_t *p_desc, uint8_t i_tag)
{
    p_desc[0] = i_tag;
}

static inline uint8_t desc_get_tag(const uint8_t *p_desc)
{
    return p_desc[0];
}

static inline void desc_set_length(uint8_t *p_desc, uint8_t i_length)
{
    p_desc[1] = i_length;
}

static inline uint8_t desc_get_length(const uint8_t *p_desc)
{
    return p_desc[1];
}

static inline void desc_print(const uint8_t *p_desc, f_print pf_print,
                              void *opaque)
{
    pf_print(opaque, "    - desc %2.2hhx unknown", desc_get_tag(p_desc));
}

/*****************************************************************************
 * Descriptor 0x05: Registration descriptor
 *****************************************************************************/
#define DESC05_HEADER_SIZE      (DESC_HEADER_SIZE + 4)

static inline void desc05_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x05);
    desc_set_length(p_desc, DESC05_HEADER_SIZE - DESC_HEADER_SIZE);
}

static inline void desc05_set_identifier(uint8_t *p_desc, uint8_t p_id[4])
{
    p_desc[2] = p_id[0];
    p_desc[3] = p_id[1];
    p_desc[4] = p_id[2];
    p_desc[5] = p_id[3];
}

static inline const uint8_t *desc05_get_identifier(const uint8_t *p_desc)
{
    return p_desc + 2;
}

static inline bool desc05_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC05_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc05_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque)
{
    pf_print(opaque, "    - desc 05 identifier=%4.4s",
             desc05_get_identifier(p_desc));
}

/*****************************************************************************
 * Descriptor 0x09: Conditional access descriptor
 *****************************************************************************/
#define DESC09_HEADER_SIZE      (DESC_HEADER_SIZE + 4)

static inline uint16_t desc09_get_sysid(const uint8_t *p_desc)
{
    return (p_desc[2] << 8) | p_desc[3];
}

static inline uint16_t desc09_get_pid(const uint8_t *p_desc)
{
    return ((p_desc[4] & 0x1f) << 8) | p_desc[5];
}

static inline bool desc09_validate(const uint8_t *p_desc)
{
    return desc_get_length(p_desc) >= DESC09_HEADER_SIZE - DESC_HEADER_SIZE;
}

static inline void desc09_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque)
{
    pf_print(opaque, "    - desc 09 sysid=0x%hx pid=%hu",
             desc09_get_sysid(p_desc), desc09_get_pid(p_desc));
}

/*****************************************************************************
 * Descriptor 0x0a: ISO-639 language descriptor
 *****************************************************************************/
#define DESC0A_HEADER_SIZE      DESC_HEADER_SIZE
#define DESC0A_LANGUAGE_SIZE    4

static inline void desc0a_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x0a);
}

static inline uint8_t *desc0a_get_language(uint8_t *p_desc, uint8_t n)
{
    uint8_t *p_desc_n = p_desc + DESC0A_HEADER_SIZE + n * DESC0A_LANGUAGE_SIZE;
    if (p_desc_n + DESC0A_LANGUAGE_SIZE - p_desc
         > desc_get_length(p_desc) + DESC0A_HEADER_SIZE)
        return NULL;
    return p_desc_n;
}

static inline void desc0an_set_code(uint8_t *p_desc_n, const uint8_t p_code[3])
{
    p_desc_n[0] = p_code[0];
    p_desc_n[1] = p_code[1];
    p_desc_n[2] = p_code[2];
}

static inline const uint8_t *desc0an_get_code(const uint8_t *p_desc_n)
{
    return p_desc_n;
}

static inline void desc0an_set_audiotype(uint8_t *p_desc_n, uint8_t i_type)
{
    p_desc_n[3] = i_type;
}

static inline uint8_t desc0an_get_audiotype(const uint8_t *p_desc_n)
{
    return p_desc_n[3];
}

static inline bool desc0a_validate(const uint8_t *p_desc)
{
    return !(desc_get_length(p_desc) % DESC0A_LANGUAGE_SIZE);
}

static inline void desc0a_print(uint8_t *p_desc, f_print pf_print,
                                void *opaque)
{
    uint8_t j = 0;
    uint8_t *p_desc_n;

    while ((p_desc_n = desc0a_get_language(p_desc, j)) != NULL) {
        j++;
        pf_print(opaque, "    - desc 0a language=%3.3s audiotype=0x%hhx",
                 (const char *)desc0an_get_code(p_desc_n),
                 desc0an_get_audiotype(p_desc_n));
    }
}

/*****************************************************************************
 * Descriptors list
 *****************************************************************************/
#define DESCS_HEADER_SIZE       2
#define DESCS_MAX_SIZE          4095

static inline void descs_set_length(uint8_t *p_descs, uint16_t i_length)
{
    p_descs[0] &= 0xf0;
    p_descs[0] |= (i_length >> 8) & 0xf;
    p_descs[1] = i_length & 0xff;
}

static inline uint16_t descs_get_length(const uint8_t *p_descs)
{
    return ((p_descs[0] & 0xf) << 8) | p_descs[1];
}

static inline uint8_t *descs_get_desc(uint8_t *p_descs, uint16_t n)
{
    uint8_t *p_desc = p_descs + DESCS_HEADER_SIZE;
    uint16_t i_descs_size = descs_get_length(p_descs) + DESCS_HEADER_SIZE;

    while (n) {
        if (p_desc + DESC_HEADER_SIZE - p_descs > i_descs_size) return NULL;
        p_desc += DESC_HEADER_SIZE + desc_get_length(p_desc);
        n--;
    }
    if (p_desc - p_descs >= i_descs_size) return NULL;
    return p_desc;
}

static inline bool descs_validate(const uint8_t *p_descs)
{
    const uint8_t *p_desc = p_descs + DESCS_HEADER_SIZE;
    uint16_t i_descs_size = descs_get_length(p_descs) + DESCS_HEADER_SIZE;

    while (p_desc + DESC_HEADER_SIZE - p_descs <= i_descs_size)
        p_desc += DESC_HEADER_SIZE + desc_get_length(p_desc);

    return (p_desc - p_descs == i_descs_size);
}

/*****************************************************************************
 * p_psi_crc_table
 *****************************************************************************
 * This table is used to compute a PSI CRC byte per byte instead of bit per
 * bit. It's been generated by 'gen_crc' in the 'misc' directory:
 *
 *   uint32_t table[256];
 *   uint32_t i, j, k;
 *
 *   for(i = 0; i < 256; i++)
 *   {
 *     k = 0;
 *     for (j = (i << 24) | 0x800000; j != 0x80000000; j <<= 1)
 *       k = (k << 1) ^ (((k ^ j) & 0x80000000) ? 0x04c11db7 : 0);
 *     table[i] = k;
 *   }
 *
 * A CRC is computed like this:
 *
 *   initialization
 *   --------------
 *   uint32_t i_crc = 0xffffffff;
 *
 *   for each data byte do
 *   ---------------------
 *   i_crc = (i_crc << 8) ^ s_crc32_table[(i_crc >> 24) ^ (data_byte)];
 *****************************************************************************/
static const uint32_t p_psi_crc_table[256] = {
    0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
    0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
    0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
    0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
    0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
    0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
    0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
    0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
    0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
    0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
    0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
    0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
    0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
    0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
    0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
    0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
    0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
    0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
    0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
    0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
    0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
    0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
    0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
    0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
    0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
    0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
    0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
    0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
    0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
    0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
    0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
    0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
    0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
    0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
    0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
    0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
    0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
    0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
    0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
    0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
    0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
    0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
    0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
    0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
    0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
    0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
    0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
    0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
    0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
    0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
    0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
    0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
    0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
    0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
    0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
    0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
    0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
    0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
    0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
    0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
    0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
    0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
    0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
    0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

/*****************************************************************************
 * PSI section
 *****************************************************************************/
#define PSI_HEADER_SIZE         3
#define PSI_HEADER_SIZE_SYNTAX1 8
#define PSI_CRC_SIZE            4
#define PSI_MAX_SIZE            1021
#define PSI_PRIVATE_MAX_SIZE    4093

#define PSI_DECLARE(p_table)            \
    uint8_t p_table[PSI_MAX_SIZE + PSI_HEADER_SIZE]
#define PSI_PRIVATE_DECLARE(p_table)    \
    uint8_t p_table[PSI_PRIVATE_MAX_SIZE + PSI_HEADER_SIZE]

static inline uint8_t *psi_allocate(void)
{
    return malloc((PSI_MAX_SIZE + PSI_HEADER_SIZE) * sizeof(uint8_t));
}

static inline uint8_t *psi_private_allocate(void)
{
    return malloc((PSI_PRIVATE_MAX_SIZE + PSI_HEADER_SIZE) * sizeof(uint8_t));
}

static inline void psi_set_tableid(uint8_t *p_section, uint8_t i_table_id)
{
    p_section[0] = i_table_id;
}

static inline uint8_t psi_get_tableid(const uint8_t *p_section)
{
    return p_section[0];
}

static inline void psi_set_syntax(uint8_t *p_section)
{
    p_section[1] |= 0x80;
}

static inline bool psi_get_syntax(const uint8_t *p_section)
{
    return !!(p_section[1] & 0x80);
}

static inline void psi_init(uint8_t *p_section, bool b_syntax)
{
    /* set reserved bits */
    p_section[1] = 0x70;
    if (b_syntax) {
        psi_set_syntax(p_section);
        p_section[5] = 0xc0;
    }
}

static inline void psi_set_length(uint8_t *p_section, uint16_t i_length)
{
    p_section[1] &= ~0xf;
    p_section[1] |= (i_length >> 8) & 0xf;
    p_section[2] = i_length & 0xff;
}

static inline uint16_t psi_get_length(const uint8_t *p_section)
{
    return ((p_section[1] & 0xf) << 8) | p_section[2];
}

static inline void psi_set_tableidext(uint8_t *p_section,
                                      uint16_t i_table_id_ext)
{
    p_section[3] = i_table_id_ext >> 8;
    p_section[4] = i_table_id_ext & 0xff;
}

static inline uint16_t psi_get_tableidext(const uint8_t *p_section)
{
    return (p_section[3] << 8) | p_section[4];
}

static inline void psi_set_version(uint8_t *p_section, uint8_t i_version)
{
    p_section[5] = i_version << 1;
}

static inline uint8_t psi_get_version(const uint8_t *p_section)
{
    return (p_section[5] & 0x1e) >> 1;
}

static inline void psi_set_current(uint8_t *p_section)
{
    p_section[5] |= 0x1;
}

static inline bool psi_get_current(const uint8_t *p_section)
{
    return !!(p_section[5] & 0x1);
}

static inline void psi_set_section(uint8_t *p_section, uint8_t i_section)
{
    p_section[6] = i_section;
}

static inline uint8_t psi_get_section(const uint8_t *p_section)
{
    return p_section[6];
}

static inline void psi_set_lastsection(uint8_t *p_section,
                                       uint8_t i_last_section)
{
    p_section[7] = i_last_section;
}

static inline uint8_t psi_get_lastsection(const uint8_t *p_section)
{
    return p_section[7];
}

/*****************************************************************************
 * PSI validation
 *****************************************************************************/
static inline void psi_set_crc(uint8_t *p_section)
{
    uint32_t i_crc = 0xffffffff;
    uint16_t i_end = (((p_section[1] & 0xf) << 8) | p_section[2])
                      + PSI_HEADER_SIZE - PSI_CRC_SIZE;
    uint16_t i;

    for (i = 0; i < i_end; i++)
        i_crc = (i_crc << 8) ^ p_psi_crc_table[(i_crc >> 24) ^ (p_section[i])];

    p_section[i_end] = i_crc >> 24;
    p_section[i_end + 1] = (i_crc >> 16) & 0xff;
    p_section[i_end + 2] = (i_crc >> 8) & 0xff;
    p_section[i_end + 3] = i_crc & 0xff;
}

static inline bool psi_check_crc(const uint8_t *p_section)
{
    uint32_t i_crc = 0xffffffff;
    uint16_t i_end = (((p_section[1] & 0xf) << 8) | p_section[2])
                      + PSI_HEADER_SIZE - PSI_CRC_SIZE;
    uint16_t i;

    for (i = 0; i < i_end; i++)
        i_crc = (i_crc << 8) ^ p_psi_crc_table[(i_crc >> 24) ^ (p_section[i])];

    return p_section[i_end] == (i_crc >> 24)
            && p_section[i_end + 1] == ((i_crc >> 16) & 0xff)
            && p_section[i_end + 2] == ((i_crc >> 8) & 0xff)
            && p_section[i_end + 3] == (i_crc & 0xff);
}

static inline bool psi_validate(const uint8_t *p_section)
{
    if (psi_get_syntax(p_section)
         && (psi_get_length(p_section) < PSI_HEADER_SIZE_SYNTAX1
                                            - PSI_HEADER_SIZE + PSI_CRC_SIZE))
        return false;

    /* only do the CRC check when it is strictly necessary */

    return true;
}

static inline bool psi_compare(const uint8_t *p_section1,
                               const uint8_t *p_section2)
{
    return psi_get_version(p_section1) == psi_get_version(p_section2)
        && psi_get_length(p_section1) == psi_get_length(p_section2)
        && !memcmp(p_section1, p_section2,
                   psi_get_length(p_section1) + PSI_HEADER_SIZE);
}

/*****************************************************************************
 * PSI section gathering
 *****************************************************************************/
static inline void psi_assemble_init(uint8_t **pp_psi_buffer,
                                     uint16_t *pi_psi_buffer_used)
{
    *pp_psi_buffer = NULL;
    *pi_psi_buffer_used = 0;
}

static inline void psi_assemble_reset(uint8_t **pp_psi_buffer,
                                      uint16_t *pi_psi_buffer_used)
{
    free(*pp_psi_buffer);
    psi_assemble_init(pp_psi_buffer, pi_psi_buffer_used);
}

static inline bool psi_assemble_empty(uint8_t **pp_psi_buffer,
                                      uint16_t *pi_psi_buffer_used)
{
    return *pp_psi_buffer == NULL;
}

static inline uint8_t *psi_assemble_payload(uint8_t **pp_psi_buffer,
                                            uint16_t *pi_psi_buffer_used,
                                            const uint8_t **pp_payload,
                                            uint8_t *pi_length)
{
    uint16_t i_remaining_size = PSI_PRIVATE_MAX_SIZE + PSI_HEADER_SIZE
                                 - *pi_psi_buffer_used;
    uint16_t i_copy_size = *pi_length < i_remaining_size ? *pi_length :
                           i_remaining_size;
    uint8_t *p_section = NULL;

    if (*pp_psi_buffer == NULL) {
        if (**pp_payload == 0xff) {
            /* padding table to the end of buffer */
            *pi_length = 0;
            return NULL;
        }
        *pp_psi_buffer = psi_private_allocate();
    }

    memcpy( *pp_psi_buffer + *pi_psi_buffer_used, *pp_payload, i_copy_size );
    *pi_psi_buffer_used += i_copy_size;

    if (*pi_psi_buffer_used >= PSI_HEADER_SIZE) {
        uint16_t i_section_size = psi_get_length(*pp_psi_buffer)
                                   + PSI_HEADER_SIZE;

        if (i_section_size > PSI_PRIVATE_MAX_SIZE) {
            /* invalid section */
            psi_assemble_reset(pp_psi_buffer, pi_psi_buffer_used);
            *pi_length = 0;
            return NULL;
        }
        if (i_section_size <= *pi_psi_buffer_used) {
            p_section = *pp_psi_buffer;
            i_copy_size -= (*pi_psi_buffer_used - i_section_size);
            *pp_psi_buffer = NULL;
            *pi_psi_buffer_used = 0;
        }
    }

    *pp_payload += i_copy_size;
    *pi_length -= i_copy_size;
    return p_section;
}

/*****************************************************************************
 * PSI section splitting
 *****************************************************************************/
static inline void psi_split_end(uint8_t *p_ts, uint8_t *pi_ts_offset)
{
    if (*pi_ts_offset != TS_SIZE) {
        memset(p_ts + *pi_ts_offset, 0xff, TS_SIZE - *pi_ts_offset);
        *pi_ts_offset = TS_SIZE;
    }
}

static inline void psi_split_section(uint8_t *p_ts, uint8_t *pi_ts_offset,
                                     const uint8_t *p_section,
                                     uint16_t *pi_section_offset)
{
    uint16_t i_section_length = psi_get_length(p_section) + PSI_HEADER_SIZE
                                 - *pi_section_offset;
    uint8_t i_ts_length, i_copy;

    if (!*pi_ts_offset) {
        ts_init(p_ts);
        ts_set_payload(p_ts);
        *pi_ts_offset = ts_payload(p_ts) - p_ts;
    }

    if (!*pi_section_offset) {
        if (TS_SIZE - *pi_ts_offset < 2) {
            psi_split_end(p_ts, pi_ts_offset);
            return;
        }
        if (!ts_get_unitstart(p_ts)) {
            uint8_t *p_payload = ts_payload(p_ts);
            uint8_t i_payload_length = *pi_ts_offset - (p_payload - p_ts);
            if (i_payload_length)
                memmove(p_payload + 1, p_payload, i_payload_length);
            (*pi_ts_offset)++;
            *p_payload = i_payload_length; /* pointer_field */
            ts_set_unitstart(p_ts);
        }
    }
    i_ts_length = TS_SIZE - *pi_ts_offset;

    i_copy = i_ts_length < i_section_length ?
             i_ts_length : i_section_length;
    memcpy(p_ts + *pi_ts_offset, p_section + *pi_section_offset, i_copy);
    *pi_ts_offset += i_copy;
    *pi_section_offset += i_copy;
}

/*****************************************************************************
 * PSI table gathering
 *****************************************************************************/
#define PSI_TABLE_MAX_SECTIONS         256

#define PSI_TABLE_DECLARE(pp_table)    \
    uint8_t *pp_table[PSI_TABLE_MAX_SECTIONS]

static inline uint8_t **psi_table_allocate(void)
{
    return malloc(PSI_TABLE_MAX_SECTIONS * sizeof(uint8_t *));
}

static inline void psi_table_init(uint8_t **pp_sections)
{
    int i;
    for (i = 0; i < PSI_TABLE_MAX_SECTIONS; i++)
        pp_sections[i] = NULL;
}

static inline void psi_table_free(uint8_t **pp_sections)
{
    int i;
    for (i = 0; i < PSI_TABLE_MAX_SECTIONS; i++)
        free(pp_sections[i]);
}

static inline bool psi_table_validate(uint8_t * const *pp_sections)
{
    return pp_sections[0] != NULL;
}

static inline void psi_table_copy(uint8_t **pp_dest, uint8_t **pp_src)
{
    memcpy(pp_dest, pp_src, PSI_TABLE_MAX_SECTIONS * sizeof(uint8_t *));
}

#define psi_table_get_tableid(pp_sections)      \
    psi_get_tableid(pp_sections[0])
#define psi_table_get_version(pp_sections)      \
    psi_get_version(pp_sections[0])
#define psi_table_get_current(pp_sections)      \
    psi_get_current(pp_sections[0])
#define psi_table_get_lastsection(pp_sections)  \
    psi_get_lastsection(pp_sections[0])
#define psi_table_get_tableidext(pp_sections)   \
    psi_get_tableidext(pp_sections[0])

static inline bool psi_table_section(uint8_t **pp_sections, uint8_t *p_section)
{
    uint8_t i_section = psi_get_section( p_section );
    uint8_t i_last_section = psi_get_lastsection( p_section );
    uint8_t i_version = psi_get_version( p_section );
    uint16_t i_tableidext = psi_get_tableidext( p_section );
    int i;

    free(pp_sections[i_section]);
    pp_sections[i_section] = p_section;

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p = pp_sections[i];
        if (p == NULL)
            return false;
        if (psi_get_lastsection(p) != i_last_section
             || psi_get_version(p) != i_version
             || psi_get_tableidext(p) != i_tableidext)
            return false;
    }

    /* free spurious, invalid sections */
    for (; i < PSI_TABLE_MAX_SECTIONS; i++)
        free(pp_sections[i]);

    /* a new, full table is available */
    return true;
}

static inline uint8_t *psi_table_get_section(uint8_t **pp_sections, uint8_t n)
{
    return pp_sections[n];
}

static inline bool psi_table_compare(uint8_t **pp_sections1,
                                     uint8_t **pp_sections2)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections1);
    uint8_t i;

    if (i_last_section != psi_table_get_lastsection(pp_sections2))
        return false;

    for (i = 0; i < i_last_section; i++) {
        const uint8_t *p_section1 = psi_table_get_section(pp_sections1, i);
        const uint8_t *p_section2 = psi_table_get_section(pp_sections2, i);
        if (!psi_compare(p_section1, p_section2))
            return false;
    }

    return true;
}

/*****************************************************************************
 * Program Association Table
 *****************************************************************************/
#define PAT_PID                 0x0
#define PAT_TABLE_ID            0x0
#define PAT_HEADER_SIZE         PSI_HEADER_SIZE_SYNTAX1
#define PAT_PROGRAM_SIZE        4

#define pat_set_tsid psi_set_tableidext
#define pat_get_tsid psi_get_tableidext

static inline void pat_init(uint8_t *p_pat)
{
    psi_init(p_pat, true);
    psi_set_tableid(p_pat, PAT_TABLE_ID);
    p_pat[1] &= ~0x40;
}

static inline void pat_set_length(uint8_t *p_pat, uint16_t i_pat_length)
{
    psi_set_length(p_pat, PAT_HEADER_SIZE + PSI_CRC_SIZE - PSI_HEADER_SIZE
                    + i_pat_length);
}

static inline void patn_init(uint8_t *p_pat_n)
{
    p_pat_n[2] = 0xe0;
}

static inline void patn_set_program(uint8_t *p_pat_n, uint16_t i_program)
{
    p_pat_n[0] = i_program >> 8;
    p_pat_n[1] = i_program & 0xff;
}

static inline uint16_t patn_get_program(const uint8_t *p_pat_n)
{
    return (p_pat_n[0] << 8) | p_pat_n[1];
}

static inline void patn_set_pid(uint8_t *p_pat_n, uint16_t i_pid)
{
    p_pat_n[2] &= ~0x1f;
    p_pat_n[2] |= i_pid >> 8;
    p_pat_n[3] = i_pid & 0xff;
}

static inline uint16_t patn_get_pid(const uint8_t *p_pat_n)
{
    return ((p_pat_n[2] & 0x1f) << 8) | p_pat_n[3];
}

static inline uint8_t *pat_get_program(uint8_t *p_pat, uint8_t n)
{
    uint8_t *p_pat_n = p_pat + PAT_HEADER_SIZE + n * PAT_PROGRAM_SIZE;
    if (p_pat_n + PAT_PROGRAM_SIZE - p_pat
         > psi_get_length(p_pat) + PSI_HEADER_SIZE - PSI_CRC_SIZE)
        return NULL;
    return p_pat_n;
}

static inline bool pat_validate(const uint8_t *p_pat)
{
    if (!psi_get_syntax(p_pat) || psi_get_tableid(p_pat) != PAT_TABLE_ID)
        return false;
    if ((psi_get_length(p_pat) - PAT_HEADER_SIZE + PSI_HEADER_SIZE
         - PSI_CRC_SIZE) % PAT_PROGRAM_SIZE)
        return false;

    return true;
}

static inline uint8_t *pat_table_find_program(uint8_t **pp_sections,
                                              uint16_t i_program)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);
        uint8_t *p_program;
        int j = 0;

        while ((p_program = pat_get_program(p_section, j)) != NULL) {
            j++;
            if (patn_get_program(p_program) == i_program)
                return p_program;
        }
    }

    return NULL;
}

static inline bool pat_table_validate(uint8_t **pp_sections)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);
        uint8_t *p_program;
        int j = 0;

        if (!psi_check_crc(p_section))
            return false;

        while ((p_program = pat_get_program(p_section, j)) != NULL) {
            j++;
            /* check that the program number if not already in the table */
            if (pat_table_find_program(pp_sections,
                                patn_get_program(p_program)) != p_program)
                return false;
        }
    }

    return true;
}

static inline void pat_table_print(uint8_t **pp_sections, f_print pf_print,
                                   void *opaque)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    pf_print(opaque, "new PAT tsid=%hu version=%hhu%s",
             psi_table_get_tableidext(pp_sections),
             psi_table_get_version(pp_sections),
             !psi_table_get_current(pp_sections) ? " (next)" : "");

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);
        const uint8_t *p_program;
        int j = 0;

        while ((p_program = pat_get_program(p_section, j)) != NULL) {
            uint16_t i_program = patn_get_program(p_program);
            uint16_t i_pid = patn_get_pid(p_program);
            j++;
            if (i_program == 0)
                pf_print(opaque, "  * NIT pid=%hu", i_pid);
            else
                pf_print(opaque, "  * program number=%hu pid=%hu",
                         i_program, i_pid);
        }
    }

    pf_print(opaque, "end PAT");
}

/*****************************************************************************
 * Program Map Table
 *****************************************************************************/
#define PMT_TABLE_ID            0x2
#define PMT_HEADER_SIZE         (PSI_HEADER_SIZE_SYNTAX1 + 4)
#define PMT_ES_SIZE             5

#define pmt_set_program psi_set_tableidext
#define pmt_get_program psi_get_tableidext

static inline void pmt_init(uint8_t *p_pmt)
{
    psi_init(p_pmt, true);
    psi_set_tableid(p_pmt, PMT_TABLE_ID);
    p_pmt[1] &= ~0x40;
    psi_set_section(p_pmt, 0);
    psi_set_lastsection(p_pmt, 0);
    p_pmt[8] = 0xe0;
    p_pmt[10] = 0xf0;
}

static inline void pmt_set_length(uint8_t *p_pmt, uint16_t i_pmt_length)
{
    psi_set_length(p_pmt, PMT_HEADER_SIZE + PSI_CRC_SIZE - PSI_HEADER_SIZE
                    + i_pmt_length);
}

static inline void pmt_set_pcrpid(uint8_t *p_pmt, uint16_t i_pcr_pid)
{
    p_pmt[8] &= ~0x1f;
    p_pmt[8] |= i_pcr_pid >> 8;
    p_pmt[9] = i_pcr_pid & 0xff;
}

static inline uint16_t pmt_get_pcrpid(const uint8_t *p_pmt)
{
    return ((p_pmt[8] & 0x1f) << 8) | p_pmt[9];
}

static inline void pmt_set_desclength(uint8_t *p_pmt, uint16_t i_length)
{
    p_pmt[10] &= ~0xf;
    p_pmt[10] |= i_length >> 8;
    p_pmt[11] = i_length & 0xff;
}

static inline uint16_t pmt_get_desclength(const uint8_t *p_pmt)
{
    return ((p_pmt[10] & 0xf) << 8) | p_pmt[11];
}

static inline uint8_t *pmt_get_descs(uint8_t *p_pmt)
{
    return &p_pmt[10];
}

static inline void pmtn_init(uint8_t *p_pmt_n)
{
    p_pmt_n[1] = 0xe0;
    p_pmt_n[3] = 0xf0;
}

static inline void pmtn_set_streamtype(uint8_t *p_pmt_n, uint8_t i_stream_type)
{
    p_pmt_n[0] = i_stream_type;
}

static inline uint8_t pmtn_get_streamtype(const uint8_t *p_pmt_n)
{
    return p_pmt_n[0];
}

static inline void pmtn_set_pid(uint8_t *p_pmt_n, uint16_t i_pid)
{
    p_pmt_n[1] &= ~0x1f;
    p_pmt_n[1] |= i_pid >> 8;
    p_pmt_n[2] = i_pid & 0xff;
}

static inline uint16_t pmtn_get_pid(const uint8_t *p_pmt_n)
{
    return ((p_pmt_n[1] & 0x1f) << 8) | p_pmt_n[2];
}

static inline void pmtn_set_desclength(uint8_t *p_pmt_n, uint16_t i_length)
{
    p_pmt_n[3] &= ~0xf;
    p_pmt_n[3] |= i_length >> 8;
    p_pmt_n[4] = i_length & 0xff;
}

static inline uint16_t pmtn_get_desclength(const uint8_t *p_pmt_n)
{
    return ((p_pmt_n[3] & 0xf) << 8) | p_pmt_n[4];
}

static inline uint8_t *pmtn_get_descs(uint8_t *p_pmt_n)
{
    return &p_pmt_n[3];
}

static inline uint8_t *pmt_get_es(uint8_t *p_pmt, uint8_t n)
{
    uint16_t i_section_size = psi_get_length(p_pmt) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    uint8_t *p_pmt_n = p_pmt + PMT_HEADER_SIZE + pmt_get_desclength(p_pmt);
    if (p_pmt_n - p_pmt > i_section_size) return NULL;

    while (n) {
        if (p_pmt_n + PMT_ES_SIZE - p_pmt > i_section_size) return NULL;
        p_pmt_n += PMT_ES_SIZE + pmtn_get_desclength(p_pmt_n);
        n--;
    }
    if (p_pmt_n - p_pmt >= i_section_size) return NULL;
    return p_pmt_n;
}

static inline bool pmt_validate(const uint8_t *p_pmt)
{
    uint16_t i_section_size = psi_get_length(p_pmt) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    const uint8_t *p_pmt_n;

    if (!psi_get_syntax(p_pmt) || psi_get_section(p_pmt)
         || psi_get_lastsection(p_pmt)
         || psi_get_tableid(p_pmt) != PMT_TABLE_ID)
        return false;

    if (!psi_check_crc(p_pmt))
        return false;

    if (i_section_size < PMT_HEADER_SIZE
         || i_section_size < PMT_HEADER_SIZE + pmt_get_desclength(p_pmt))
        return false;

    if (!descs_validate(p_pmt + 10))
        return false;

    p_pmt_n = p_pmt + PMT_HEADER_SIZE + pmt_get_desclength(p_pmt);

    while (p_pmt_n + PMT_ES_SIZE - p_pmt <= i_section_size
            && p_pmt_n + PMT_ES_SIZE + pmtn_get_desclength(p_pmt_n) - p_pmt
                <= i_section_size) {
        if (!descs_validate(p_pmt_n + 3))
            return false;

        p_pmt_n += PMT_ES_SIZE + pmtn_get_desclength(p_pmt_n);
    }

    return (p_pmt_n - p_pmt == i_section_size);
}

static inline uint8_t *pmt_find_es(uint8_t *p_pmt, uint16_t i_pid)
{
    uint8_t *p_es;
    uint8_t j = 0;

    while ((p_es = pmt_get_es(p_pmt, j)) != NULL) {
        j++;
        if (pmtn_get_pid(p_es) == i_pid)
            return p_es;
    }

    return NULL;
}

#ifdef __cplusplus
}
#endif

#endif
