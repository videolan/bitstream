/*****************************************************************************
 * biss.h: BISS-CA Entitlement Management Message Table (EMM)
 *****************************************************************************
 * Copyright (C) 2018 Open Broadcast Systems Ltd.
 *
 * Authors: Rafaël Carré <funman@videolan.org>
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
 *  - EBU Tech 3292
 */

#ifndef __BITSTREAM_EBU_BISS_H__
#define __BITSTREAM_EBU_BISS_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Entitlement Management Message Table
 *****************************************************************************/
#define BISSCA_EMM_TABLE_ID            0x81
#define BISSCA_EMM_TABLE_ID_LAST       0x8f
#define BISSCA_EMM_HEADER_SIZE         PSI_HEADER_SIZE_SYNTAX1

#define BISSCA_EMM_CIPHER_RSA_2048_OAEP    0
#define BISSCA_EMM_RSA_2048_OAEP_ESD_SIZE  (2048/8)
#define BISSCA_EMM_RSA_2048_OAEP_EKID_SIZE  (64/8)

static inline void bissca_emm_init(uint8_t *p_emm, uint16_t esid)
{
    psi_init(p_emm, true);
    psi_set_tableid(p_emm, BISSCA_EMM_TABLE_ID);
    p_emm[1] &= ~0x40;
    psi_set_tableidext(p_emm, esid);
    psi_set_section(p_emm, 0);
    psi_set_lastsection(p_emm, 0);
    p_emm[BISSCA_EMM_HEADER_SIZE+3] = 0x0f;
    p_emm[BISSCA_EMM_HEADER_SIZE+4] = 0xf0;
}

static inline void bissca_emm_set_onid(uint8_t *p_emm, uint16_t onid)
{
    p_emm[BISSCA_EMM_HEADER_SIZE] = onid >> 8;
    p_emm[BISSCA_EMM_HEADER_SIZE+1] = onid & 0xff;
}

static inline uint16_t bissca_emm_get_onid(const uint8_t *p_emm)
{
    return (p_emm[BISSCA_EMM_HEADER_SIZE] << 8) |
        p_emm[BISSCA_EMM_HEADER_SIZE+1];
}

static inline void bissca_emm_set_last_table_id(uint8_t *p_emm, uint8_t last_table_id)
{
    p_emm[BISSCA_EMM_HEADER_SIZE+2] = last_table_id;
}

static inline uint8_t bissca_emm_get_last_table_id(const uint8_t *p_emm)
{
    return p_emm[BISSCA_EMM_HEADER_SIZE+2];
}

static inline void bissca_emm_set_emm_cipher_type(uint8_t *p_emm, uint8_t bissca_emm_cipher_type)
{
    p_emm[BISSCA_EMM_HEADER_SIZE+3] &= 0x0f;
    p_emm[BISSCA_EMM_HEADER_SIZE+3] |= (bissca_emm_cipher_type << 5);
}

static inline uint8_t bissca_emm_get_emm_cipher_type(const uint8_t *p_emm)
{
    return p_emm[BISSCA_EMM_HEADER_SIZE+3] >> 5;
}

static inline void bissca_emm_set_entitlement_priv_data_loop(uint8_t *p_emm, bool entitlement_priv_data_loop)
{
    p_emm[BISSCA_EMM_HEADER_SIZE+3] &= 0x0f;
    p_emm[BISSCA_EMM_HEADER_SIZE+3] |= (!!entitlement_priv_data_loop << 4);
}

static inline bool bissca_emm_get_entitlement_priv_data_loop(const uint8_t *p_emm)
{
    return p_emm[BISSCA_EMM_HEADER_SIZE+3] & 0x10;
}

static inline void bissca_emm_set_length(uint8_t *p_emm, uint16_t i_emm_length)
{
    // TODO: take N of loops ?
    psi_set_length(p_emm, BISSCA_EMM_HEADER_SIZE + 6 + PSI_CRC_SIZE - PSI_HEADER_SIZE
                    + i_emm_length);
}

static inline uint16_t bissca_emm_get_desclength(const uint8_t *p_emm)
{
    return ((p_emm[BISSCA_EMM_HEADER_SIZE + 4] & 0xf) << 8) |
        p_emm[BISSCA_EMM_HEADER_SIZE + 5];
}

static inline void bissca_emm_set_desclength(uint8_t *p_emm, uint16_t i_desc_len)
{
    p_emm[BISSCA_EMM_HEADER_SIZE + 4] &= 0xf0;
    p_emm[BISSCA_EMM_HEADER_SIZE + 4] |= (i_desc_len << 8) & 0xf;
    p_emm[BISSCA_EMM_HEADER_SIZE + 5] = i_desc_len & 0xff;
}

static inline uint8_t *bissca_emm_get_descl(uint8_t *p_emm)
{
    return p_emm + BISSCA_EMM_HEADER_SIZE + 6;
}

static inline const uint8_t *bissca_emm_get_descl_const(const uint8_t *p_emm)
{
    return p_emm + BISSCA_EMM_HEADER_SIZE + 6;
}

static inline uint8_t *bissca_emm_get_emmn(uint8_t *p_emm, int n)
{
    uint16_t i_section_size = psi_get_length(p_emm) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;
    uint8_t *p_emm_n = p_emm + BISSCA_EMM_HEADER_SIZE + 6 + bissca_emm_get_desclength(p_emm);
    if (p_emm_n - p_emm > i_section_size) return NULL;
    bool priv = bissca_emm_get_entitlement_priv_data_loop(p_emm);

    while (n) {
        if (p_emm_n + BISSCA_EMM_RSA_2048_OAEP_ESD_SIZE + BISSCA_EMM_RSA_2048_OAEP_EKID_SIZE - p_emm > i_section_size) return NULL;
        p_emm_n += BISSCA_EMM_RSA_2048_OAEP_ESD_SIZE + BISSCA_EMM_RSA_2048_OAEP_EKID_SIZE;
        if (priv) {
            if (p_emm_n + DESCS_HEADER_SIZE - p_emm > i_section_size) return NULL;
            uint16_t desc_len = descs_get_length(p_emm_n);
            if (p_emm_n + DESCS_HEADER_SIZE + desc_len - p_emm > i_section_size) return NULL;
            p_emm_n += DESCS_HEADER_SIZE + desc_len;
        }
        n--;
    }
    if (p_emm_n - p_emm >= i_section_size) return NULL;
    return p_emm_n;
}

static inline void bissca_emmn_get_ekid(uint8_t *p_emm_n, uint8_t ekid[BISSCA_EMM_RSA_2048_OAEP_EKID_SIZE])
{
    memcpy(ekid, p_emm_n, BISSCA_EMM_RSA_2048_OAEP_EKID_SIZE);
}

static inline void bissca_emmn_set_ekid(uint8_t *p_emm_n, const uint8_t ekid[BISSCA_EMM_RSA_2048_OAEP_EKID_SIZE])
{
    memcpy(p_emm_n, ekid, BISSCA_EMM_RSA_2048_OAEP_EKID_SIZE);
}

static inline void bissca_emmn_get_esd(uint8_t *p_emm_n, uint8_t esd[BISSCA_EMM_RSA_2048_OAEP_ESD_SIZE])
{
    memcpy(esd, &p_emm_n[BISSCA_EMM_RSA_2048_OAEP_EKID_SIZE], BISSCA_EMM_RSA_2048_OAEP_ESD_SIZE);
}

static inline void bissca_emmn_set_esd(uint8_t *p_emm_n, const uint8_t esd[BISSCA_EMM_RSA_2048_OAEP_ESD_SIZE])
{
    memcpy(&p_emm_n[BISSCA_EMM_RSA_2048_OAEP_EKID_SIZE], esd, BISSCA_EMM_RSA_2048_OAEP_ESD_SIZE);
}

static inline bool bissca_emm_validate(const uint8_t *p_emm)
{
    uint16_t i_section_size = psi_get_length(p_emm) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;

    if (!psi_get_syntax(p_emm) || psi_get_section(p_emm)
         || psi_get_lastsection(p_emm)
         || psi_get_tableid(p_emm) < BISSCA_EMM_TABLE_ID
         || psi_get_tableid(p_emm) > BISSCA_EMM_TABLE_ID_LAST)
        return false;

    if (i_section_size < BISSCA_EMM_HEADER_SIZE + 6
         || i_section_size < BISSCA_EMM_HEADER_SIZE + 6 + bissca_emm_get_desclength(p_emm))
        return false;

    if (!descl_validate(bissca_emm_get_descl_const(p_emm), bissca_emm_get_desclength(p_emm)))
        return false;

    // TODO: validate RSA loop ?

    return true;
}

static inline bool bissca_emm_table_validate(uint8_t **pp_sections)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);

        if (!psi_check_crc(p_section))
            return false;
    }

    return true;
}

/*****************************************************************************
 * Entitlement Control Message Table
 *****************************************************************************/

#define BISSCA_ECM_TABLE_ID            0x80
#define BISSCA_ECM_HEADER_SIZE         PSI_HEADER_SIZE_SYNTAX1

#define BISSCA_ECM_CIPHER_AES_128_CBC   0

static inline void bissca_ecm_init(uint8_t *p_ecm, uint16_t esid)
{
    psi_init(p_ecm, true);
    psi_set_tableid(p_ecm, BISSCA_ECM_TABLE_ID);
    p_ecm[1] &= ~0x40;
    psi_set_tableidext(p_ecm, esid);
    psi_set_section(p_ecm, 0);
    psi_set_lastsection(p_ecm, 0);
    p_ecm[BISSCA_ECM_HEADER_SIZE+2] = 0x0f;
    p_ecm[BISSCA_ECM_HEADER_SIZE+3] = 0xf0;
}

static inline void bissca_ecm_set_onid(uint8_t *p_ecm, uint16_t onid)
{
    bissca_emm_set_onid(p_ecm, onid);
}

static inline uint16_t bissca_ecm_get_onid(const uint8_t *p_ecm)
{
    return bissca_emm_get_onid(p_ecm);
}

static inline void bissca_ecm_set_cipher_type(uint8_t *p_ecm, uint8_t cipher_type)
{
    p_ecm[BISSCA_EMM_HEADER_SIZE+2] &= ~(7 << 5);
    p_ecm[BISSCA_EMM_HEADER_SIZE+2] |= (cipher_type & 7) << 5;
}

static inline uint8_t bissca_ecm_get_cipher_type(const uint8_t *p_ecm)
{
    return p_ecm[BISSCA_EMM_HEADER_SIZE+2] >> 5;
}

static inline uint16_t bissca_ecm_get_desclength(const uint8_t *p_ecm)
{
    return ((p_ecm[BISSCA_ECM_HEADER_SIZE + 2] & 0xf) << 8) |
        p_ecm[BISSCA_ECM_HEADER_SIZE + 3];
}

static inline void bissca_ecm_set_desclength(uint8_t *p_ecm, uint16_t i_desc_len)
{
    p_ecm[BISSCA_ECM_HEADER_SIZE + 2] &= 0xf0;
    p_ecm[BISSCA_ECM_HEADER_SIZE + 2] |= (i_desc_len << 8) & 0xf;
    p_ecm[BISSCA_ECM_HEADER_SIZE + 3] = i_desc_len & 0xff;
}

static inline uint8_t *bissca_ecm_get_descl(uint8_t *p_ecm)
{
    return p_ecm + BISSCA_ECM_HEADER_SIZE + 4;
}

static inline const uint8_t *bissca_ecm_get_descl_const(const uint8_t *p_ecm)
{
    return p_ecm + BISSCA_ECM_HEADER_SIZE + 4;
}

static inline void bissca_ecm_set_session_key_parity(uint8_t *p_ecm, bool odd)
{
    uint16_t desc_len = bissca_ecm_get_desclength(p_ecm);
    p_ecm[BISSCA_ECM_HEADER_SIZE + 4 + desc_len] = (!!odd << 7) | 0x7f;
}

static inline bool bissca_ecm_get_session_key_parity(const uint8_t *p_ecm)
{
    uint16_t desc_len = bissca_ecm_get_desclength(p_ecm);
    return p_ecm[BISSCA_ECM_HEADER_SIZE + 4 + desc_len] & 0x80;
}

static inline void bissca_ecm_set_iv(uint8_t *p_ecm, const uint8_t iv[16])
{
    uint16_t desc_len = bissca_ecm_get_desclength(p_ecm);
    uint8_t *p_iv = &p_ecm[BISSCA_ECM_HEADER_SIZE + 4 + desc_len + 1];
    memcpy(p_iv, iv, 16);
}

static inline void bissca_ecm_get_iv(const uint8_t *p_ecm, uint8_t iv[16])
{
    uint16_t desc_len = bissca_ecm_get_desclength(p_ecm);
    const uint8_t *p_iv = &p_ecm[BISSCA_ECM_HEADER_SIZE + 4 + desc_len + 1];
    memcpy(iv, p_iv, 16);
}

static inline void bissca_ecm_set_even_word(uint8_t *p_ecm, const uint8_t even[16])
{
    uint16_t desc_len = bissca_ecm_get_desclength(p_ecm);
    uint8_t *p_even = &p_ecm[BISSCA_ECM_HEADER_SIZE + 4 + desc_len + 1 + 16];
    memcpy(p_even, even, 16);
}

static inline void bissca_ecm_get_even_word(const uint8_t *p_ecm, uint8_t even[16])
{
    uint16_t desc_len = bissca_ecm_get_desclength(p_ecm);
    const uint8_t *p_even = &p_ecm[BISSCA_ECM_HEADER_SIZE + 4 + desc_len + 1 + 16];
    memcpy(even, p_even, 16);
}

static inline void bissca_ecm_set_odd_word(uint8_t *p_ecm, const uint8_t odd[16])
{
    uint16_t desc_len = bissca_ecm_get_desclength(p_ecm);
    uint8_t *p_odd = &p_ecm[BISSCA_ECM_HEADER_SIZE + 4 + desc_len + 1 + 32];
    memcpy(p_odd, odd, 16);
}

static inline void bissca_ecm_get_odd_word(const uint8_t *p_ecm, uint8_t odd[16])
{
    uint16_t desc_len = bissca_ecm_get_desclength(p_ecm);
    const uint8_t *p_odd = &p_ecm[BISSCA_ECM_HEADER_SIZE + 4 + desc_len + 1 + 32];
    memcpy(odd, p_odd, 16);
}

static inline bool bissca_ecm_validate(const uint8_t *p_ecm)
{
    uint16_t i_section_size = psi_get_length(p_ecm) + PSI_HEADER_SIZE
                               - PSI_CRC_SIZE;

    if (!psi_get_syntax(p_ecm) || psi_get_section(p_ecm)
         || psi_get_lastsection(p_ecm)
         || psi_get_tableid(p_ecm) != BISSCA_ECM_TABLE_ID)
        return false;

    if (i_section_size < BISSCA_ECM_HEADER_SIZE + 6
         || i_section_size < BISSCA_ECM_HEADER_SIZE + 6 + bissca_ecm_get_desclength(p_ecm))
        return false;

    if (bissca_ecm_get_cipher_type(p_ecm) == BISSCA_ECM_CIPHER_AES_128_CBC
          && i_section_size < BISSCA_ECM_HEADER_SIZE + 6 + bissca_ecm_get_desclength(p_ecm) + 49)
        return false;

    if (!descl_validate(bissca_ecm_get_descl_const(p_ecm), bissca_ecm_get_desclength(p_ecm)))
        return false;

    return true;
}

static inline bool bissca_ecm_table_validate(uint8_t **pp_sections)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);

        if (!psi_check_crc(p_section))
            return false;
    }

    return true;
}

#ifdef __cplusplus
}
#endif

#endif
