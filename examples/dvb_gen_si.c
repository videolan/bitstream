/*****************************************************************************
 * dvb_gen_si.c: Generate SI tables and descriptors
 *****************************************************************************
 * Copyright (c) 2011 Unix Solutions Ltd.
 * Copyright (c) 2015 VideoLAN
 *
 * Authors:
 *   Georgi Chorbadzhiyski <gf@unixsol.org>
 *   Christophe Massiot <massiot@via.ecp.fr>
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

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <bitstream/mpeg/ts.h>
#include <bitstream/mpeg/psi.h>
#include <bitstream/dvb/si.h>
#include <bitstream/dvb/si_print.h>
#include <bitstream/scte/35.h>
#include <bitstream/scte/35_print.h>

uint16_t tsid     = 10000;
uint16_t sid      = 20000;
uint16_t event_id = 30000;
uint16_t onid     = 40000;

uint16_t pmt_pid  = 100;
uint16_t scte35_pid  = 200;

time_t ts_0 = 1234567890;
time_t ts_1 = 1;
time_t ts_2 = 999999999;
time_t ts_3 = 0x7fffffff;

uint8_t  cc = 0;

/* =========================================================================
 * MPEG defined descriptors
 * ========================================================================= */

/* MPEG Descriptor 0x02: Video stream descriptor */
static void build_desc02_1(uint8_t *desc) {
    desc02_init(desc);
    desc02_set_multiple_frame_rate (desc, true);
    desc02_set_frame_rate_code     (desc, 3); // 25.00
    desc02_set_mpeg1_only_flag     (desc, true);
    desc02_set_constraint_parameter(desc, true);
    desc02_set_still_picture       (desc, true);
}

static void build_desc02_2(uint8_t *desc) {
    desc02_init(desc);
    desc02_set_multiple_frame_rate (desc, true);
    desc02_set_frame_rate_code     (desc, 4); // 29.97
    desc02_set_mpeg1_only_flag     (desc, false); // Allow more flags
    desc02_set_constraint_parameter(desc, true);
    desc02_set_still_picture       (desc, true);
    desc02_set_profile             (desc, 4); // Main
    desc02_set_level               (desc, 8); // Main
    desc02_set_chroma_format       (desc, 2); // 4:2:2
    desc02_set_frame_rate_extension(desc, false);
}

/* MPEG Descriptor 0x03: Audio stream descriptor */
static void build_desc03(uint8_t *desc) {
    desc03_init(desc);
    desc03_set_free_format_flag(desc, true);
    desc03_set_ID(desc, true);
    desc03_set_layer(desc, 2); // Layer II
    desc03_set_vbr_flag(desc, true);
}

/* MPEG Descriptor 0x04: Hierarchy descriptor */
static void build_desc04(uint8_t *desc) {
    desc04_init(desc);
    desc04_set_type(desc, 1);
    desc04_set_layer_index(desc, 6);
    desc04_set_embedded_layer_index(desc, 7);
    desc04_set_channel(desc, 8);
}

/* MPEG Descriptor 0x05: Registration descriptor */
static void build_desc05(uint8_t *desc) {
    desc05_init(desc);
    desc05_set_identifier(desc, (uint8_t *)"TEST");
}

/* MPEG Descriptor 0x06: Data stream alignment descriptor */
static void build_desc06(uint8_t *desc) {
    desc06_init(desc);
    desc06_set_alignment(desc, 2); // Video access unit
}

/* MPEG Descriptor 0x07: Target Background Grid descriptor */
static void build_desc07(uint8_t *desc) {
    desc07_init(desc);
    desc07_set_horizontal_size(desc, 720);
    desc07_set_vertical_size(desc, 576);
    desc07_set_aspect_ratio_info(desc, 7);
}

/* MPEG Descriptor 0x08: Video Window descriptor */
static void build_desc08(uint8_t *desc) {
    desc08_init(desc);
    desc08_set_horizontal_offset(desc, 1024);
    desc08_set_vertical_offset(desc, 768);
    desc08_set_window_priority(desc, 15);
}

/* MPEG Descriptor 0x09: Conditional access descriptor */
static void build_desc09(uint8_t *desc) {
    desc09_init(desc);
    desc09_set_sysid(desc, 0xb000);
    desc09_set_pid(desc, 1234);
}

/* MPEG Descriptor 0x0A: ISO-639 language descriptor */
static void build_desc0a(uint8_t *desc) {
    uint8_t k = 0;
    uint8_t *lang_n;

    desc0a_init(desc);
    desc_set_length(desc, 255);

    lang_n = desc0a_get_language(desc, k++);
    desc0an_set_code(lang_n, (uint8_t *)"eng");
    desc0an_set_audiotype(lang_n, 0);

    lang_n = desc0a_get_language(desc, k++);
    desc0an_set_code(lang_n, (uint8_t *)"eng");
    desc0an_set_audiotype(lang_n, 1);

    lang_n = desc0a_get_language(desc, k++);
    desc0an_set_code(lang_n, (uint8_t *)"eng");
    desc0an_set_audiotype(lang_n, 2);

    lang_n = desc0a_get_language(desc, k++);
    desc0an_set_code(lang_n, (uint8_t *)"eng");
    desc0an_set_audiotype(lang_n, 3);

    lang_n = desc0a_get_language(desc, k++);
    desc0an_set_code(lang_n, (uint8_t *)"bul");
    desc0an_set_audiotype(lang_n, 4);

    lang_n = desc0a_get_language(desc, k++);
    desc0an_set_code(lang_n, (uint8_t *)"bul");
    desc0an_set_audiotype(lang_n, 0);

    lang_n = desc0a_get_language(desc, k);
    desc_set_length(desc, lang_n - desc - DESC_HEADER_SIZE);
}

/* MPEG Descriptor 0x0B: System clock descriptor */
static void build_desc0b(uint8_t *desc) {
    desc0b_init(desc);
    desc0b_set_external_clock_reference_indicator(desc, true);
    desc0b_set_clock_accuracy_integer(desc, 60);
    desc0b_set_clock_accuracy_exponent(desc, 4);
}

/* MPEG Descriptor 0x0C: Multiplex buffer utilization descriptor */
static void build_desc0c(uint8_t *desc) {
    desc0c_init(desc);
    desc0c_set_bound_valid(desc, true);
    desc0c_set_LTW_offset_lower_bound(desc, 1234);
    desc0c_set_LTW_offset_upper_bound(desc, 5678);
}

/* MPEG Descriptor 0x0D: Copyright descriptor */
static void build_desc0d(uint8_t *desc) {
    desc0d_init(desc);
    desc0d_set_copyright_identifier(desc, 0x12345678);
    desc0d_set_additional_copyright_info(desc, (uint8_t *)"ABCD", 4);
}

/* MPEG Descriptor 0x0E: Maximum bitrate descriptor */
static void build_desc0e(uint8_t *desc) {
    desc0e_init(desc);
    desc0e_set_max_bitrate(desc, 123456);
}

/* MPEG Descriptor 0x0F: Private data indicator descriptor */
static void build_desc0f(uint8_t *desc) {
    desc0f_init(desc);
    desc0f_set_private_indicator(desc, 0xaabbccdd);
}

/* MPEG Descriptor 0x10: Smoothing buffer descriptor */
static void build_desc10(uint8_t *desc) {
    desc10_init(desc);
    desc10_set_sb_leak_rate(desc, 112233);
    desc10_set_sb_size(desc, 4112233);
}

/* MPEG Descriptor 0x11: STD descriptor */
static void build_desc11(uint8_t *desc) {
    desc11_init(desc);
    desc11_set_leak_valid_flag(desc, true);
}

/* MPEG Descriptor 0x12: IBP descriptor */
static void build_desc12(uint8_t *desc) {
    desc12_init(desc);
    desc12_set_closed_gop_flag(desc, true);
    desc12_set_identical_gop_flag(desc, false);
    desc12_set_max_gop_length(desc, 5000);
}

/* MPEG Descriptor 0x1b: MPEG4 Video descriptor */
static void build_desc1b(uint8_t *desc) {
    desc1b_init(desc);
    desc1b_set_mpeg4_visual_profile_and_level(desc, 0x12);
}

/* MPEG Descriptor 0x1c: MPEG4 Audio descriptor */
static void build_desc1c(uint8_t *desc) {
    desc1c_init(desc);
    desc1c_set_mpeg4_audio_profile_and_level(desc, 0x13);
}

/* MPEG Descriptor 0x1d: IOD descriptor */
static void build_desc1d(uint8_t *desc) {
    desc1d_init(desc);
    desc1d_set_scope(desc, 0x01);
    desc1d_set_iod_label(desc, 0x23);
    desc1d_set_iod(desc, 0x45);
}

/* MPEG Descriptor 0x1e: SL descriptor */
static void build_desc1e(uint8_t *desc) {
    desc1e_init(desc);
    desc1e_set_es_id(desc, 0x1234);
}

/* MPEG Descriptor 0x1f: FMC descriptor */
static void build_desc1f(uint8_t *desc) {
    uint8_t k = 0;
    uint8_t *entry_n;

    desc1f_init(desc);
    desc_set_length(desc, 255);

    entry_n = desc1f_get_entry(desc, k++);
    desc1fn_set_es_id(entry_n, 0x1122);
    desc1fn_set_flexmux_channel(entry_n, 0x33);

    entry_n = desc1f_get_entry(desc, k++);
    desc1fn_set_es_id(entry_n, 0x4455);
    desc1fn_set_flexmux_channel(entry_n, 0x66);

    entry_n = desc1f_get_entry(desc, k++);
    desc1fn_set_es_id(entry_n, 0x7788);
    desc1fn_set_flexmux_channel(entry_n, 0x99);

    entry_n = desc1f_get_entry(desc, k);
    desc_set_length(desc, entry_n - desc - DESC_HEADER_SIZE);
}

/* MPEG Descriptor 0x20: External ES_ID descriptor */
static void build_desc20(uint8_t *desc) {
    desc20_init(desc);
    desc20_set_external_es_id(desc, 0x1234);
}

/* MPEG Descriptor 0x21: MuxCode descriptor */
static void build_desc21(uint8_t *desc) {
    uint8_t n = 0, k = 0, r;
    uint8_t *entry_n, *entry_k;

    desc21_init(desc);
    desc_set_length(desc, 255);

    entry_n = desc21_get_entry(desc, n++);
    desc21n_set_muxcode(entry_n, 1);
    desc21n_set_version(entry_n, 1);
    desc21n_set_length(entry_n, DESC21_ENTRY_HEADER_SIZE - 1);
    desc21n_set_substruct_count(entry_n, 0);

    entry_n = desc21_get_entry(desc, n++);
    desc21n_set_muxcode(entry_n, 2);
    desc21n_set_version(entry_n, 2);
    desc21n_set_length(entry_n, 255);
    desc21n_set_substruct_count(entry_n, 255);
    {
        k = 0;
        entry_k = desc21n_get_substruct(entry_n, k++);
        desc21k_set_repetition_count(entry_k, 1);
        desc21k_set_slot_count(entry_k, 0);

        entry_k = desc21n_get_substruct(entry_n, k++);
        desc21k_set_repetition_count(entry_k, 5);
        desc21k_set_slot_count(entry_k, 5);
        for (r=0; r<desc21k_get_slot_count(entry_k); r++) {
            desc21k_set_flex_mux_channel(entry_k, r, r + 10);
            desc21k_set_number_of_bytes(entry_k,  r, r + 20);
        }

        entry_k = desc21n_get_substruct(entry_n, k++);
        desc21k_set_repetition_count(entry_k, 3);
        desc21k_set_slot_count(entry_k, 1);
        for (r=0; r<desc21k_get_slot_count(entry_k); r++) {
            desc21k_set_flex_mux_channel(entry_k, r, r + 30);
            desc21k_set_number_of_bytes(entry_k,  r, r + 40);
        }

        entry_k = desc21n_get_substruct(entry_n, k);
        desc21n_set_length(entry_n, entry_k - entry_n - 1);
        desc21n_set_substruct_count(entry_n, k);
    }

    entry_n = desc21_get_entry(desc, n++);
    desc21n_set_muxcode(entry_n, 3);
    desc21n_set_version(entry_n, 3);
    desc21n_set_length(entry_n, DESC21_ENTRY_HEADER_SIZE - 1);
    desc21n_set_substruct_count(entry_n, 0);

    entry_n = desc21_get_entry(desc, n++);
    desc21n_set_muxcode(entry_n, 4);
    desc21n_set_version(entry_n, 4);
    desc21n_set_length(entry_n, 255);
    desc21n_set_substruct_count(entry_n, 255);
    {
        k = 0;
        entry_k = desc21n_get_substruct(entry_n, k++);
        desc21k_set_repetition_count(entry_k, 3);
        desc21k_set_slot_count(entry_k, 2);
        for (r=0; r<desc21k_get_slot_count(entry_k); r++) {
            desc21k_set_flex_mux_channel(entry_k, r, r + 50);
            desc21k_set_number_of_bytes(entry_k,  r, r + 60);
        }

        entry_k = desc21n_get_substruct(entry_n, k++);
        desc21k_set_repetition_count(entry_k, 4);
        desc21k_set_slot_count(entry_k, 4);
        for (r=0; r<desc21k_get_slot_count(entry_k); r++) {
            desc21k_set_flex_mux_channel(entry_k, r, r + 70);
            desc21k_set_number_of_bytes(entry_k,  r, r + 80);
        }

        entry_k = desc21n_get_substruct(entry_n, k);
        desc21n_set_length(entry_n, entry_k - entry_n - 1);
        desc21n_set_substruct_count(entry_n, k);
    }

    entry_n = desc21_get_entry(desc, n);
    desc_set_length(desc, entry_n - desc - DESC_HEADER_SIZE);
}

/* MPEG Descriptor 0x22: FmxBufferSize descriptor */
static void build_desc22(uint8_t *desc) {
    uint8_t k = 0;
    uint8_t *entry_n;

    desc22_init(desc);
    desc22_set_default_buffer_size(desc, 0x112233);

    desc_set_length(desc, 255);
    entry_n = desc22_get_entry(desc, k++);
    desc22n_set_flexmux_channel(entry_n, 0x11);
    desc22n_set_flexmux_buffer_size(entry_n, 0x223344);

    entry_n = desc22_get_entry(desc, k++);
    desc22n_set_flexmux_channel(entry_n, 0x55);
    desc22n_set_flexmux_buffer_size(entry_n, 0x667788);

    entry_n = desc22_get_entry(desc, k++);
    desc22n_set_flexmux_channel(entry_n, 0x99);
    desc22n_set_flexmux_buffer_size(entry_n, 0xaabbcc);

    entry_n = desc22_get_entry(desc, k);
    desc_set_length(desc, entry_n - desc - DESC_HEADER_SIZE);
}

/* MPEG Descriptor 0x23: MultiplexBuffer descriptor */
static void build_desc23(uint8_t *desc) {
    desc23_init(desc);
    desc23_set_mb_buffer_size(desc, 112233);
    desc23_set_tb_leak_rate(desc, 445566);
}

/* MPEG Descriptor 0x24: Content labeling descriptor */
static void build_desc24_1(uint8_t *desc) {
    desc24_init(desc);

    desc24_set_metadata_application_format(desc, 0xabcd);
    desc24_set_content_reference_id_record_flag(desc, false);
    desc24_set_content_time_base_indicator(desc, 0);

    desc24_set_length(desc);
}

static void build_desc24_2(uint8_t *desc) {
    desc24_init(desc);

    desc24_set_metadata_application_format(desc, 0xfff0);

    desc24_set_content_reference_id_record_flag(desc, false);

    desc24_set_content_time_base_indicator(desc, 3);
    desc24_set_time_base_association(desc, 3, (uint8_t *)"ABC");

    desc24_set_length(desc);
}

static void build_desc24_3(uint8_t *desc) {
    desc24_init(desc);

    desc24_set_metadata_application_format(desc, 0xffff);
    // Needs desc24_set_metadata_application_format(desc, 0xffff);
    desc24_set_metadata_application_format_identifier(desc, 0xaabbccdd);

    desc24_set_content_reference_id_record_flag(desc, true);
    // Needs desc24_set_content_reference_id_record_flag(desc, true);
    desc24_set_content_reference_id_record(desc, 3, (uint8_t *)"abc");

    desc24_set_content_time_base_indicator(desc, 2);
    // Needs desc24_set_content_time_base_indicator(desc, 1..2);
    desc24_set_content_time_base_value (desc, 0x1F0F0F0F0); // 8337289456
    desc24_set_metadata_time_base_value(desc, 0x1F1F1F1F1); // 8354132465

    // Needs desc24_set_content_time_base_indicator(desc, 2);
    desc24_set_content_id(desc, 100);

    // Needs desc24_set_content_time_base_indicator(desc, 3..7);
    desc24_set_time_base_association(desc, 3, (uint8_t *)"ABC");

    desc24_set_length(desc);
}

/* MPEG Descriptor 0x25: Metadata pointer descriptor */
static void build_desc25(uint8_t *desc) {
    desc25_init(desc);

    desc25_set_metadata_application_format(desc, 0xffff);
    // Needs desc25_set_metadata_application_format(desc, 0xffff);
    desc25_set_metadata_application_format_identifier(desc, 0x00112233);

    desc25_set_metadata_format(desc, 0xff);
    // Needs desc25_set_metadata_format(desc, 0xff);
    desc25_set_metadata_format_identifier(desc, 0xdeadbeaf);

    desc25_set_metadata_service_id(desc, 0x88);

    desc25_set_metadata_locator_record_flag(desc, true);
    // Needs desc25_set_metadata_locator_record_flag(desc, true);
    desc25_set_metadata_locator_record(desc, 3, (uint8_t *)"abc");

    desc25_set_mpeg_carriage_flags(desc, 1);
    // Needs desc25_set_mpeg_carriage_flags(desc, 0..2);
    desc25_set_program_number(desc, 1000);
    // Needs desc25_set_mpeg_carriage_flags(desc, 1);
    desc25_set_ts_location(desc, 2000);
    desc25_set_ts_id(desc, 3000);

    desc25_set_length(desc);
}

/* MPEG Descriptor 0x26: Metadata descriptor */
static void build_desc26_1(uint8_t *desc) {
    desc26_init(desc);

    desc26_set_metadata_application_format(desc, 0xfff0);
    desc26_set_metadata_format(desc, 0xf0);
    desc26_set_metadata_service_id(desc, 0x88);
    desc26_set_dsm_cc_flag(desc, false);
    desc26_set_decoder_config_flags(desc, 0);

    desc26_set_length(desc);
}

static void build_desc26_2(uint8_t *desc) {
    desc26_init(desc);

    desc26_set_metadata_application_format(desc, 0xffff);
    // Needs desc26_set_metadata_application_format(desc, 0xffff);
    desc26_set_metadata_application_format_identifier(desc, 0x00112233);

    desc26_set_metadata_format(desc, 0xff);
    // Needs desc26_set_metadata_format(desc, 0xff);
    desc26_set_metadata_format_identifier(desc, 0xdeadbeaf);

    desc26_set_metadata_service_id(desc, 0x88);

    desc26_set_dsm_cc_flag(desc, true);
    // Needs desc26_set_dsm_cc_flag(desc, true);
    desc26_set_service_identification_record(desc, 3, (uint8_t *)"abc");

    desc26_set_decoder_config_flags(desc, 3);
    // Needs desc26_set_decoder_config_flags(desc, 1);
    desc26_set_decoder_config(desc, 3, (uint8_t *)"123");

    // Needs desc26_set_decoder_config_flags(desc, 3);
    desc26_set_dec_config_identification_record(desc, 3, (uint8_t *)"ABC");

    // Needs desc26_set_mpeg_carriage_flags(desc, 4);
    desc26_set_decoder_config_metadata_service_id(desc, 100);

    desc26_set_length(desc);
}

/* MPEG Descriptor 0x27: Metadata STD descriptor */
static void build_desc27(uint8_t *desc) {
    desc27_init(desc);
    desc27_set_input_leak_rate(desc, 12345);
    desc27_set_buffer_size(desc, 23456);
    desc27_set_output_leak_rate(desc, 34567);
}

/* MPEG Descriptor 0x28: AVC video descriptor */
static void build_desc28(uint8_t *desc) {
    desc28_init(desc);
    desc28_set_profile_idc(desc, 0x12);
    desc28_set_constraint_set0_flag(desc, true);
    desc28_set_constraint_set1_flag(desc, true);
    desc28_set_constraint_set2_flag(desc, false);
    desc28_set_avc_compatible_flags(desc, 0x0a);
    desc28_set_level_idc(desc, 0x34);
    desc28_set_avc_still_present(desc, false);
    desc28_set_avc_24_hour_picture_flag(desc, false);
}

/* MPEG Descriptor 0x2a: AVC timing and HRD descriptor */
static void build_desc2a(uint8_t *desc) {
    desc2a_init(desc);
    desc2a_set_hrd_management_valid_flag(desc, false);
    desc2a_set_picture_and_timing_info_present(desc, true);
    desc2a_set_90khz_flag(desc, false); // depends on picture_and_timing_info
    desc2a_set_N(desc, 12345678); // depends on 90khz == false
    desc2a_set_K(desc, 34567890); // depends on 90khz == false
    desc2a_set_num_units_in_tick(desc, 456789); // depends on picture_and_timing_info
    desc2a_set_fixed_frame_rate_flag(desc, true);
    desc2a_set_temporal_poc_flag(desc, false);
    desc2a_set_picture_to_display_conversion_flag(desc, true);
}

/* MPEG Descriptor 0x2b: MPEG-2 AAC audio descriptor */
static void build_desc2b(uint8_t *desc) {
    desc2b_init(desc);
    desc2b_set_aac_profile(desc, 0x12);
    desc2b_set_aac_channel_config(desc, 0x05);
    desc2b_set_aac_additional_info(desc, 0x00);
}

/* MPEG Descriptor 0x2c: FlexMuxTiming descriptor */
static void build_desc2c(uint8_t *desc) {
    desc2c_init(desc);
    desc2c_set_fcr_es_id(desc, 0x1234);
    desc2c_set_fcr_resolution(desc, 123456789);
    desc2c_set_fcr_length(desc, 55);
    desc2c_set_fmx_rate_length(desc, 32);
}

/* =========================================================================
 * DVB defined descriptors
 * ========================================================================= */

/* DVB  Descriptor 0x40: Network name descriptor */
static void build_desc40(uint8_t *desc) {
    char *network_name = "Test Network Name";
    desc40_init(desc);
    desc40_set_networkname(desc, (uint8_t *)network_name, strlen(network_name));
}

/* DVB  Descriptor 0x41: Service list descriptor */
static void build_desc41(uint8_t *desc) {
    uint8_t k = 0;
    uint8_t *service_n;

    desc41_init(desc);
    desc_set_length(desc, 255);

    service_n = desc41_get_service(desc, k++);
    desc41n_set_sid(service_n, sid);
    desc41n_set_type(service_n, 1);

    service_n = desc41_get_service(desc, k++);
    desc41n_set_sid(service_n, sid + 100);
    desc41n_set_type(service_n, 2);

    service_n = desc41_get_service(desc, k++);
    desc41n_set_sid(service_n, sid + 200);
    desc41n_set_type(service_n, 1);

    service_n = desc41_get_service(desc, k++);
    desc41n_set_sid(service_n, sid + 300);
    desc41n_set_type(service_n, 2);

    service_n = desc41_get_service(desc, k);
    desc_set_length(desc, service_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x43: Satellite delivery system descriptor */
static void build_desc43(uint8_t *desc) {
    desc43_init(desc);
    desc43_set_frequency_bcd  (desc, 0x1175725); // 11,75725 GHz
    desc43_set_position_bcd   (desc, 0x0192); // 19.2
    desc43_set_east           (desc, true);
    desc43_set_polarization   (desc, 1); // 0-H, 1-V, 2-L, 3-R
    desc43_set_rolloff        (desc, 0); // Must be 0 if dvbs2 is false
    desc43_set_dvbs2          (desc, false);
    desc43_set_modulation     (desc, 1); // QPSK
//    desc43_set_rolloff        (desc, 1); // 0,25
//    desc43_set_dvbs2          (desc, true);
//    desc43_set_modulation     (desc, 2); // 8PSK
    desc43_set_symbolrate_bcd (desc, 0x274500); // 27,450
    desc43_set_fecinner       (desc, 5); // 7/8
}

/* DVB  Descriptor 0x44: Cable delivery system descriptor */
static void build_desc44(uint8_t *desc) {
    desc44_init(desc);
    desc44_set_frequency_bcd  (desc, 0x3120000); // 312,0000 Mhz
    desc44_set_fecouter       (desc, 0); // 0 - not defined
    desc44_set_modulation     (desc, 3); // 64-QAM
    desc44_set_symbolrate_bcd (desc, 0x274500); // 27,450
    desc44_set_fecinner       (desc, 6); // 8/9
}

/* DVB  Descriptor 0x45: VBI data descriptor */
static void build_desc45(uint8_t *desc) {
    uint8_t k = 0;
    uint8_t *data_n;

    desc45_init(desc);
    desc_set_length(desc, 255);

    data_n = desc45_get_data(desc, k++);
    desc45n_set_service_id (data_n, 0x01);   // EBU teletext
    desc45n_set_data_length(data_n, 8);      // How much bytes are after this
    {
        uint8_t j, max_j = desc45n_get_data_length(data_n);
        for (j = 0; j < max_j; j++) {
            desc45n_set_field_parity(data_n, j, (j % 2) == 0);
            desc45n_set_line_offset (data_n, j, j);
        }
    }

    data_n = desc45_get_data(desc, k++);
    desc45n_set_service_id (data_n, 0x03);   // Reserved
    desc45n_set_data_length(data_n, 4);      // How much bytes are after this
    desc45n_set_byte(data_n, 0, 0x12);
    desc45n_set_byte(data_n, 1, 0x34);
    desc45n_set_byte(data_n, 2, 0x56);
    desc45n_set_byte(data_n, 3, 0x78);

    data_n = desc45_get_data(desc, k++);
    desc45n_set_service_id (data_n, 0x04);   // VPS
    desc45n_set_data_length(data_n, 1);      // How much bytes are after this
    {
        uint8_t j, max_j = desc45n_get_data_length(data_n);
        for (j = 0; j < max_j; j++) {
            desc45n_set_field_parity(data_n, j, (j % 2) == 2);
            desc45n_set_line_offset (data_n, j, j + 10);
        }
    }

    data_n = desc45_get_data(desc, k++);
    desc45n_set_service_id (data_n, 0x05);   // WSS
    desc45n_set_data_length(data_n, 4);      // How much bytes are after this
    {
        uint8_t j, max_j = desc45n_get_data_length(data_n);
        for (j = 0; j < max_j; j++) {
            desc45n_set_field_parity(data_n, j, (j % 2) == 1);
            desc45n_set_line_offset (data_n, j, j + 20);
        }
    }

    data_n = desc45_get_data(desc, k++);
    desc45n_set_service_id (data_n, 0x06);   // Closed Captioning
    desc45n_set_data_length(data_n, 0);      // How much bytes are after this

    data_n = desc45_get_data(desc, k);
    desc_set_length(desc, data_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x46: VBI teletext descriptor */
static void build_desc46(uint8_t *desc) {
    uint8_t k = 0;
    uint8_t *lang_n;

    desc46_init(desc);
    desc_set_length(desc, 255);

    lang_n = desc46_get_language(desc, k++);
    desc46n_set_code(lang_n, (uint8_t *)"eng");
    desc46n_set_teletexttype(lang_n, 1);
    desc46n_set_teletextmagazine(lang_n, 3);
    desc46n_set_teletextpage(lang_n, 255);

    lang_n = desc46_get_language(desc, k++);
    desc46n_set_code(lang_n, (uint8_t *)"bul");
    desc46n_set_teletexttype(lang_n, 2);
    desc46n_set_teletextmagazine(lang_n, 2);
    desc46n_set_teletextpage(lang_n, 127);

    lang_n = desc46_get_language(desc, k++);
    desc46n_set_code(lang_n, (uint8_t *)"fre");
    desc46n_set_teletexttype(lang_n, 3);
    desc46n_set_teletextmagazine(lang_n, 1);
    desc46n_set_teletextpage(lang_n, 64);

    lang_n = desc46_get_language(desc, k);
    desc_set_length(desc, lang_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x47: Bouquet name descriptor */
static void build_desc47(uint8_t *desc) {
    char *bouquet_name = "Test Bouquet Name";
    desc47_init(desc);
    desc47_set_bouquetname(desc, (uint8_t *)bouquet_name, strlen(bouquet_name));
}

/* DVB  Descriptor 0x48: Service descriptor */
static void build_desc48(uint8_t *desc) {
    uint8_t service_type = 0x01; // digital tv
    char *provider_name = "Test Provider Name";
    char *service_name = "Test Service Name";
    desc48_init(desc);
    desc48_set_type(desc, service_type);
    desc48_set_provider(desc, (uint8_t *)provider_name, strlen(provider_name));
    desc48_set_service(desc, (uint8_t *)service_name, strlen(service_name));
    desc48_set_length(desc);
}

/* DVB  Descriptor 0x49: Country availability descriptor */
static void build_desc49(uint8_t *desc, bool b_available) {
    uint8_t k = 0;
    uint8_t *code_n;

    desc49_init(desc);
    desc49_set_country_availability_flag(desc, b_available);

    desc_set_length(desc, 255);

    code_n = desc49_get_code(desc, k++);
    desc49n_set_code(code_n, (uint8_t *)"GBR");

    code_n = desc49_get_code(desc, k++);
    desc49n_set_code(code_n, (uint8_t *)"FRA");

    code_n = desc49_get_code(desc, k++);
    desc49n_set_code(code_n, (uint8_t *)"BUL");

    code_n = desc49_get_code(desc, k);
    desc_set_length(desc, code_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x4a: Linkage descriptor */
static void build_desc4a_std(uint8_t *desc) {
    desc4a_init(desc);
    desc4a_set_tsid(desc, tsid + 500);
    desc4a_set_onid(desc, onid + 500);
    desc4a_set_sid (desc, 0x0000);
    desc4a_set_linkage(desc, 0x04); // new ts
}

static void build_desc4a_mobile(uint8_t *desc) {
    desc4a_init(desc);
    desc4a_set_tsid(desc, tsid + 600);
    desc4a_set_onid(desc, onid + 600);
    desc4a_set_sid (desc, sid + 600);
    // mobile handover
    desc4a_set_linkage(desc, DESC4A_LINKAGE_MOBILE);
    desc4a_set_mobile_handover_type(desc, 1);
    desc4a_set_mobile_origin_type(desc, false);
    desc4a_set_mobile_nid(desc, onid + 1000);
    desc4a_set_mobile_initial_sid(desc, sid + 1000);
    desc4a_set_length(desc);
}

static void build_desc4a_event(uint8_t *desc) {
    desc4a_init(desc);
    desc4a_set_tsid(desc, tsid + 700);
    desc4a_set_onid(desc, onid + 700);
    desc4a_set_sid (desc, sid + 700);
    // event linkage
    desc4a_set_linkage(desc, DESC4A_LINKAGE_EVENT);
    desc4a_set_event_target_event_id(desc, event_id + 1000);
    desc4a_set_event_target_listed(desc, true);
    desc4a_set_event_simulcast(desc, true);
    desc4a_set_length(desc);
}

static void build_desc4a_extended_event(uint8_t *desc) {
    uint8_t k = 0;
    uint8_t *ext_n;

    desc4a_init(desc);
    desc4a_set_tsid(desc, tsid + 800);
    desc4a_set_onid(desc, onid + 800);
    desc4a_set_sid (desc, sid + 800);
    // extended event linkage
    desc4a_set_linkage(desc, DESC4A_LINKAGE_EXT_EVENT);

    desc_set_length(desc, 255);

    ext_n = desc4a_get_ext_event(desc, k++);
    desc4an_set_ext_event_target_event_id(ext_n, event_id + 1000);
    desc4an_set_ext_event_target_listed  (ext_n, false);
    desc4an_set_ext_event_simulcast      (ext_n, true);
    desc4an_set_ext_event_link_type      (ext_n, 1); // HD
    desc4an_set_ext_event_target_id_type (ext_n, 3); // user defined id
    desc4an_set_ext_event_onid_id_flag   (ext_n, true);
    desc4an_set_ext_event_service_id_flag(ext_n, true);
    desc4an_set_ext_event_user_defined_id(ext_n, 7878);
    desc4an_set_ext_event_target_tsid    (ext_n, tsid + 1000);  // !!!
    desc4an_set_ext_event_target_onid    (ext_n, onid + 1000);  // !!!
    desc4an_set_ext_event_service_id     (ext_n, sid + 1000);   // !!!

    ext_n = desc4a_get_ext_event(desc, k++);
    desc4an_set_ext_event_target_event_id(ext_n, event_id + 2000);
    desc4an_set_ext_event_target_listed  (ext_n, true);
    desc4an_set_ext_event_simulcast      (ext_n, true);
    desc4an_set_ext_event_link_type      (ext_n, 0); // SD
    desc4an_set_ext_event_target_id_type (ext_n, 0); // onid_id_flag, target_service_id_flag
    desc4an_set_ext_event_onid_id_flag   (ext_n, true);
    desc4an_set_ext_event_service_id_flag(ext_n, true);
    desc4an_set_ext_event_user_defined_id(ext_n, 8787);         // !!!
    desc4an_set_ext_event_target_tsid    (ext_n, tsid + 2000);  // !!!
    desc4an_set_ext_event_target_onid    (ext_n, onid + 2000);
    desc4an_set_ext_event_service_id     (ext_n, sid + 2000);

    ext_n = desc4a_get_ext_event(desc, k++);
    desc4an_set_ext_event_target_event_id(ext_n, event_id + 3000);
    desc4an_set_ext_event_target_listed  (ext_n, true);
    desc4an_set_ext_event_simulcast      (ext_n, true);
    desc4an_set_ext_event_link_type      (ext_n, 2); // 3D
    desc4an_set_ext_event_target_id_type (ext_n, 1); // target_tsid, onid_id_flag, target_service_id_flag
    desc4an_set_ext_event_onid_id_flag   (ext_n, true);
    desc4an_set_ext_event_service_id_flag(ext_n, true);
    desc4an_set_ext_event_user_defined_id(ext_n, 8787);         // !!!
    desc4an_set_ext_event_target_tsid    (ext_n, tsid + 3000);
    desc4an_set_ext_event_target_onid    (ext_n, onid + 3000);
    desc4an_set_ext_event_service_id     (ext_n, sid + 3000);

    ext_n = desc4a_get_ext_event(desc, k++);
    desc4an_set_ext_event_target_event_id(ext_n, event_id + 4000);
    desc4an_set_ext_event_target_listed  (ext_n, false);
    desc4an_set_ext_event_simulcast      (ext_n, false);
    desc4an_set_ext_event_link_type      (ext_n, 0); // SD
    desc4an_set_ext_event_target_id_type (ext_n, 0); // onid_id_flag, target_service_id_flag
    desc4an_set_ext_event_onid_id_flag   (ext_n, false);
    desc4an_set_ext_event_service_id_flag(ext_n, false);
    desc4an_set_ext_event_user_defined_id(ext_n, 8787);         // !!!
    desc4an_set_ext_event_target_tsid    (ext_n, tsid + 4000);  // !!!
    desc4an_set_ext_event_target_onid    (ext_n, onid + 4000);  // !!!
    desc4an_set_ext_event_service_id     (ext_n, sid + 4000);   // !!!

    ext_n = desc4a_get_ext_event(desc, k);
    desc_set_length(desc, ext_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x4b: NVOD_reference_descriptor */
static void build_desc4b(uint8_t *desc, bool b_available) {
    uint8_t k = 0;
    uint8_t *ref_n;

    desc4b_init(desc);
    desc_set_length(desc, 255);

    ref_n = desc4b_get_reference(desc, k++);
    desc4bn_set_tsid(ref_n, tsid);
    desc4bn_set_onid(ref_n, onid);
    desc4bn_set_sid (ref_n, sid);

    ref_n = desc4b_get_reference(desc, k++);
    desc4bn_set_tsid(ref_n, tsid + 100);
    desc4bn_set_onid(ref_n, onid + 100);
    desc4bn_set_sid (ref_n, sid  + 100);

    ref_n = desc4b_get_reference(desc, k++);
    desc4bn_set_tsid(ref_n, tsid + 200);
    desc4bn_set_onid(ref_n, onid + 200);
    desc4bn_set_sid (ref_n, sid  + 200);

    ref_n = desc4b_get_reference(desc, k);
    desc_set_length(desc, ref_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x4c: time_shifted_service_descriptor */
static void build_desc4c(uint8_t *desc) {
    desc4c_init(desc);
    desc4c_set_reference_sid(desc, sid + 1000);
}

/* DVB  Descriptor 0x4d: Short event descriptor */
static void build_desc4d(uint8_t *desc) {
    char *event_name = "Major TV event";
    char *text = "The event of the century!";
    desc4d_init(desc);
    desc4d_set_lang(desc, (uint8_t *)"eng");
    desc4d_set_event_name(desc, (uint8_t *)event_name, strlen(event_name));
    desc4d_set_text(desc, (uint8_t *)text, strlen(text));
    desc4d_set_length(desc);
}

/* DVB  Descriptor 0x4e: Extended event descriptor */
static void build_desc4e(uint8_t *desc) {
    uint8_t k = 0;
    uint8_t *item_n;
    char *item1_desc = "Director";
    char *item1_text = "Famous director";
    char *item2_desc = "Year";
    char *item2_text = "2011";
    char *item3_desc = "Rating";
    char *item3_text = "***++";
    char *text = "Wow, what an event!";

    desc4e_init(desc);
    desc_set_length(desc, 255);

    desc4e_set_desc_number(desc, 0);
    desc4e_set_last_desc_number(desc, 0);
    desc4e_set_lang(desc, (uint8_t *)"eng");
    desc4e_set_items_length(desc, 0);

    {
        desc4e_set_items_length(desc, 255);
        uint8_t *first_item = desc4e_get_item(desc, 0);

        item_n = desc4e_get_item(desc, k++);
        desc4en_set_item_description(item_n, (uint8_t *)item1_desc, strlen(item1_desc));
        desc4en_set_item_text(item_n, (uint8_t *)item1_text, strlen(item1_text));

        item_n = desc4e_get_item(desc, k++);
        desc4en_set_item_description(item_n, (uint8_t *)item2_desc, strlen(item2_desc));
        desc4en_set_item_text(item_n, (uint8_t *)item2_text, strlen(item2_text));

        item_n = desc4e_get_item(desc, k++);
        desc4en_set_item_description(item_n, (uint8_t *)item3_desc, strlen(item3_desc));
        desc4en_set_item_text(item_n, (uint8_t *)item3_text, strlen(item3_text));

        item_n = desc4e_get_item(desc, k);
        desc4e_set_items_length(desc, item_n - first_item);
    }

    desc4e_set_text(desc, (uint8_t *)text, strlen(text));
    desc4e_set_length(desc);
}

/* DVB  Descriptor 0x4f: time_shifted_event_descriptor */
static void build_desc4f(uint8_t *desc) {
    desc4f_init(desc);
    desc4f_set_reference_sid(desc, sid + 2000);
    desc4f_set_reference_event_id(desc, event_id + 2000);
}

/* DVB  Descriptor 0x50: Component descriptor */
static void build_desc50(uint8_t *desc) {
    char *text = "Stereo";
    desc50_init(desc);
    desc50_set_stream_content(desc, 0x02);
    desc50_set_component_type(desc, 0x03);
    desc50_set_component_tag(desc, 46);
    desc50_set_language(desc, (uint8_t *)"eng");
    desc50_set_text(desc, (uint8_t *)text, strlen(text)); // Not required
}

/* DVB  Descriptor 0x51: Mosaic descriptor */
static void build_desc51(uint8_t *desc) {
    uint8_t k = 0;
    uint8_t *cell_n;

    desc51_init(desc);
    desc51_set_mosaic_entry_point(desc, 1);
    desc51_set_horizontal_cells(desc, 2); // +1 == 3
    desc51_set_vertical_cells(desc, 2);   // +1 == 3 (3x3 == 9 cells)

    desc_set_length(desc, 255);
    cell_n = desc51_get_logical_cell(desc, k++);
    {
        desc51n_set_logical_cell_id(cell_n, 0);
        desc51n_set_logical_cell_presentation_info(cell_n, 1);
        desc51n_set_elementary_cell_field_length(cell_n, 3);
        {
            desc51n_set_elementary_cell_id(cell_n, 0, 0);
            desc51n_set_elementary_cell_id(cell_n, 1, 1);
            desc51n_set_elementary_cell_id(cell_n, 2, 2);
        }

        desc51n_set_cell_linkage_info(cell_n, 1); // bouquet_id
        desc51n_set_bouquet_id(cell_n, onid + 100);
    }

    cell_n = desc51_get_logical_cell(desc, k++);
    {
        desc51n_set_logical_cell_id(cell_n, 1);
        desc51n_set_logical_cell_presentation_info(cell_n, 2);
        desc51n_set_elementary_cell_field_length(cell_n, 3);
        {
            desc51n_set_elementary_cell_id(cell_n, 0, 3);
            desc51n_set_elementary_cell_id(cell_n, 1, 4);
            desc51n_set_elementary_cell_id(cell_n, 2, 5);
        }
        desc51n_set_cell_linkage_info(cell_n, 2); // 2 or 3, onid, tsid, sid
        desc51n_set_onid(cell_n, onid + 500);
        desc51n_set_tsid(cell_n, tsid + 500);
        desc51n_set_sid(cell_n, sid + 500);
    }

    cell_n = desc51_get_logical_cell(desc, k++);
    {
        desc51n_set_logical_cell_id(cell_n, 2);
        desc51n_set_logical_cell_presentation_info(cell_n, 3);
        desc51n_set_elementary_cell_field_length(cell_n, 3);
        {
            desc51n_set_elementary_cell_id(cell_n, 0, 6);
            desc51n_set_elementary_cell_id(cell_n, 1, 7);
            desc51n_set_elementary_cell_id(cell_n, 2, 8);
        }
        desc51n_set_cell_linkage_info(cell_n, 4); // onid, tsid, sid, event_id
        desc51n_set_onid(cell_n, onid + 1000);
        desc51n_set_tsid(cell_n, tsid + 1000);
        desc51n_set_sid(cell_n, sid + 1000);
        desc51n_set_event_id(cell_n, event_id + 1000);
    }

    cell_n = desc51_get_logical_cell(desc, k);
    desc_set_length(desc, cell_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x52: Stream identifier descriptor */
static void build_desc52(uint8_t *desc) {
    desc52_init(desc);
    desc52_set_component_tag(desc, 46);
}

/* DVB  Descriptor 0x53: CA_identifier_descriptor */
static void build_desc53(uint8_t *desc) {
    uint8_t k = 0;
    uint8_t *cas_n;

    desc53_init(desc);
    desc_set_length(desc, 255);

    cas_n = desc53_get_ca(desc, k++);
    desc53n_set_ca_sysid(cas_n, 0xaabb);

    cas_n = desc53_get_ca(desc, k++);
    desc53n_set_ca_sysid(cas_n, 0xccdd);

    cas_n = desc53_get_ca(desc, k++);
    desc53n_set_ca_sysid(cas_n, 0xeeff);

    cas_n = desc53_get_ca(desc, k);
    desc_set_length(desc, cas_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x54: Content descriptor */
static void build_desc54(uint8_t *desc) {
    uint8_t k = 0;
    uint8_t *content_n;

    desc54_init(desc);
    desc_set_length(desc, 255);

    content_n = desc54_get_content(desc, k++);
    desc54n_set_content_l1(content_n, 2);
    desc54n_set_content_l2(content_n, 4);
    desc54n_set_user(content_n, 78);

    content_n = desc54_get_content(desc, k++);
    desc54n_set_content_l1(content_n, 6);
    desc54n_set_content_l2(content_n, 8);
    desc54n_set_user(content_n, 177);

    content_n = desc54_get_content(desc, k);
    desc_set_length(desc, content_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x55: Parental rating descriptor */
static void build_desc55(uint8_t *desc) {
    uint8_t k = 0;
    uint8_t *rating_n;

    desc55_init(desc);
    desc_set_length(desc, 255);

    rating_n = desc55_get_rating(desc, k++);
    desc55n_set_country_code(rating_n, (uint8_t *)"USA");
    desc55n_set_rating(rating_n, 0);

    rating_n = desc55_get_rating(desc, k++);
    desc55n_set_country_code(rating_n, (uint8_t *)"CHI");
    desc55n_set_rating(rating_n, 15);

    rating_n = desc55_get_rating(desc, k++);
    desc55n_set_country_code(rating_n, (uint8_t *)"FRA");
    desc55n_set_rating(rating_n, 12);

    rating_n = desc55_get_rating(desc, k++);
    desc55n_set_country_code(rating_n, (uint8_t *)"BUL");
    desc55n_set_rating(rating_n, 24);

    rating_n = desc55_get_rating(desc, k);
    desc_set_length(desc, rating_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x56: Teletext descriptor */
static void build_desc56(uint8_t *desc) {
    // Desc 46 is the same as desc 56, only
    // the descriptor tag is different
    build_desc46(desc);
    desc56_init(desc);
}

/* DVB  Descriptor 0x57: telephone_descriptor */
static void build_desc57(uint8_t *desc) {
    char *country_prefix = "+";
    char *international_area_code = "359";
    char *operator_code = "2";
    char *national_area_code = "";
    char *core_number = "9868620";

    desc57_init(desc);

    desc57_set_foreign_availability(desc, true);
    desc57_set_connection_type(desc, 3);

    desc57_set_country_prefix(desc, country_prefix, strlen(country_prefix));
    desc57_set_international_area_code(desc, international_area_code, strlen(international_area_code));
    desc57_set_operator_code(desc, operator_code, strlen(operator_code));
    desc57_set_national_area_code(desc, national_area_code, strlen(national_area_code));
    desc57_set_core_number(desc, core_number, strlen(core_number));
    desc57_set_length(desc);
}

/* DVB  Descriptor 0x58: Local time offset descriptor */
static void build_desc58(uint8_t *desc) {
    uint8_t k = 0;
    uint8_t *lto_n;

    desc58_init(desc);
    desc_set_length(desc, 255);

    lto_n = desc58_get_lto(desc, k++);
    desc58n_set_country_code(lto_n, (uint8_t *)"BUL");
    desc58n_set_country_region_id(lto_n, 2);
    desc58n_set_lto_polarity(lto_n, 1);
    desc58n_set_lt_offset(lto_n, 0x0200);
    desc58n_set_time_of_change(lto_n, dvb_time_encode_UTC(ts_0));
    desc58n_set_next_offset(lto_n, 0x0300);

    lto_n = desc58_get_lto(desc, k++);
    desc58n_set_country_code(lto_n, (uint8_t *)"USA");
    desc58n_set_country_region_id(lto_n, 9);
    desc58n_set_lto_polarity(lto_n, 0);
    desc58n_set_lt_offset(lto_n, 0x1400);
    desc58n_set_time_of_change(lto_n, dvb_time_encode_UTC(ts_2));
    desc58n_set_next_offset(lto_n, 0x1830);

    lto_n = desc58_get_lto(desc, k);
    desc_set_length(desc, lto_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x59: Subtitling descriptor */
static void build_desc59(uint8_t *desc) {
    uint8_t k = 0;
    uint8_t *lang_n;

    desc59_init(desc);
    desc_set_length(desc, 255);

    lang_n = desc59_get_language(desc, k++);
    desc59n_set_code(lang_n, (uint8_t *)"eng");
    desc59n_set_subtitlingtype(lang_n, 1);
    desc59n_set_compositionpage(lang_n, 2);
    desc59n_set_ancillarypage(lang_n, 3);

    lang_n = desc59_get_language(desc, k++);
    desc59n_set_code(lang_n, (uint8_t *)"bul");
    desc59n_set_subtitlingtype(lang_n, 2);
    desc59n_set_compositionpage(lang_n, 3);
    desc59n_set_ancillarypage(lang_n, 4);

    lang_n = desc59_get_language(desc, k++);
    desc59n_set_code(lang_n, (uint8_t *)"fre");
    desc59n_set_subtitlingtype(lang_n, 5);
    desc59n_set_compositionpage(lang_n, 6);
    desc59n_set_ancillarypage(lang_n, 7);

    lang_n = desc59_get_language(desc, k);
    desc_set_length(desc, lang_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x5a: Terrestrial delivery system descriptor */
static void build_desc5a(uint8_t *desc) {
    desc5a_init(desc);
    desc5a_set_frequency      (desc, 1234567890); // Hz * 10
    desc5a_set_bandwidth      (desc, 2); // 6 Mhz
    desc5a_set_priority       (desc, false); // HP, hierarchy must 0
    desc5a_set_timeslicing    (desc, true);
    desc5a_set_mpefec         (desc, true);
    desc5a_set_constellation  (desc, 2); // 64-QAM
    desc5a_set_hierarchy      (desc, 0xe);
    desc5a_set_coderatehp     (desc, 2); // 3/4
    desc5a_set_coderatelp     (desc, 3); // 5/6
    desc5a_set_guard          (desc, 1); // 1/16
    desc5a_set_transmission   (desc, 2); // 4k mode
    desc5a_set_otherfrequency (desc, true);
}

/* DVB  Descriptor 0x5b: multilingual_network_name_descriptor */
static void build_desc5b(uint8_t *desc) {
    char *network_name = "M Network";
    uint8_t k = 0;
    uint8_t *data_n;

    desc5b_init(desc);
    desc_set_length(desc, 255);

    data_n = desc5b_get_data(desc, k++);
    desc5bn_set_code(data_n, (uint8_t *)"eng");
    desc5bn_set_networkname(data_n, (uint8_t *)network_name, strlen(network_name));

    data_n = desc5b_get_data(desc, k++);
    desc5bn_set_code(data_n, (uint8_t *)"fre");
    desc5bn_set_networkname(data_n, (uint8_t *)network_name, strlen(network_name));

    data_n = desc5b_get_data(desc, k++);
    desc5bn_set_code(data_n, (uint8_t *)"bul");
    desc5bn_set_networkname(data_n, (uint8_t *)network_name, strlen(network_name));

    data_n = desc5b_get_data(desc, k);
    desc_set_length(desc, data_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x5c: Multilingual bouquet name descriptor */
static void build_desc5c(uint8_t *desc) {
    char *bouquet_name = "M Bouquet";
    uint8_t k = 0;
    uint8_t *data_n;

    desc5c_init(desc);
    desc_set_length(desc, 255);

    data_n = desc5c_get_data(desc, k++);
    desc5cn_set_code(data_n, (uint8_t *)"eng");
    desc5cn_set_bouquetname(data_n, (uint8_t *)bouquet_name, strlen(bouquet_name));

    data_n = desc5c_get_data(desc, k++);
    desc5cn_set_code(data_n, (uint8_t *)"fre");
    desc5cn_set_bouquetname(data_n, (uint8_t *)bouquet_name, strlen(bouquet_name));

    data_n = desc5c_get_data(desc, k++);
    desc5cn_set_code(data_n, (uint8_t *)"bul");
    desc5cn_set_bouquetname(data_n, (uint8_t *)bouquet_name, strlen(bouquet_name));

    data_n = desc5c_get_data(desc, k);
    desc_set_length(desc, data_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x5d: Multilingual service name descriptor */
static void build_desc5d(uint8_t *desc) {
    char *provider_name = "M Provider";
    char *service_name = "M Service";
    uint8_t k = 0;
    uint8_t *data_n;

    desc5d_init(desc);
    desc_set_length(desc, 255);

    data_n = desc5d_get_data(desc, k++);
    desc5dn_set_code(data_n, (uint8_t *)"eng");
    desc5dn_set_provider_name(data_n, (uint8_t *)provider_name, strlen(provider_name));
    desc5dn_set_service_name(data_n, (uint8_t *)service_name, strlen(service_name));

    data_n = desc5d_get_data(desc, k++);
    desc5dn_set_code(data_n, (uint8_t *)"fre");
    desc5dn_set_provider_name(data_n, (uint8_t *)provider_name, strlen(provider_name));
    desc5dn_set_service_name(data_n, (uint8_t *)service_name, strlen(service_name));

    data_n = desc5d_get_data(desc, k++);
    desc5dn_set_code(data_n, (uint8_t *)"bul");
    desc5dn_set_provider_name(data_n, (uint8_t *)provider_name, strlen(provider_name));
    desc5dn_set_service_name(data_n, (uint8_t *)service_name, strlen(service_name));


    data_n = desc5d_get_data(desc, k);
    desc_set_length(desc, data_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x5e: Multilingual component descriptor */
static void build_desc5e(uint8_t *desc) {
    char *text = "Stereo";
    uint8_t k = 0;
    uint8_t *data_n;

    desc5e_init(desc);
    desc5e_set_component_tag(desc, 46);
    desc_set_length(desc, 255);

    data_n = desc5e_get_data(desc, k++);
    desc5en_set_code(data_n, (uint8_t *)"eng");
    desc5en_set_text(data_n, (uint8_t *)text, strlen(text));

    data_n = desc5e_get_data(desc, k++);
    desc5en_set_code(data_n, (uint8_t *)"fre");
    desc5en_set_text(data_n, (uint8_t *)text, strlen(text));

    data_n = desc5e_get_data(desc, k++);
    desc5en_set_code(data_n, (uint8_t *)"bul");
    desc5en_set_text(data_n, (uint8_t *)text, strlen(text));

    data_n = desc5e_get_data(desc, k);
    desc_set_length(desc, data_n - desc - DESC_HEADER_SIZE);
}


/* DVB  Descriptor 0x5f: Private data specifier descriptor */
static void build_desc5f(uint8_t *desc) {
    desc5f_init(desc);
    desc5f_set_specifier(desc, 0xaabbccdd);
}

/* DVB  Descriptor 0x60: Service move descriptor */
static void build_desc60(uint8_t *desc) {
    desc60_init(desc);
    desc60_set_new_onid(desc, 10000);
    desc60_set_new_tsid(desc, 15000);
    desc60_set_new_service_id(desc, 20000);
}

/* DVB  Descriptor 0x61: Short smoothing buffer descriptor */
static void build_desc61(uint8_t *desc) {
    desc61_init(desc);
    desc61_set_sb_size(desc, 1); // 1536 bytes buffer size
    desc61_set_sb_leak_rate(desc, 10); // 0.5 Mbit/s
}

/* DVB  Descriptor 0x62: Frequency list descriptor */
static void build_desc62(uint8_t *desc) {
    uint8_t k = 0;
    uint8_t *freq_n;

    desc62_init(desc);
    desc62_set_coding_type(desc, 2); // Cable
    desc_set_length(desc, 255);

    freq_n = desc62_get_frequency(desc, k++);
    desc62n_set_freq(freq_n, 0x3120000); // 312,0000 Mhz

    freq_n = desc62_get_frequency(desc, k++);
    desc62n_set_freq(freq_n, 0x3180000); // 318,0000 Mhz

    freq_n = desc62_get_frequency(desc, k++);
    desc62n_set_freq(freq_n, 0x3240000); // 324,0000 Mhz

    freq_n = desc62_get_frequency(desc, k++);
    desc62n_set_freq(freq_n, 0x3300000); // 330,0000 Mhz

    freq_n = desc62_get_frequency(desc, k++);
    desc62n_set_freq(freq_n, 0x3360000); // 336,0000 Mhz

    freq_n = desc62_get_frequency(desc, k);
    desc_set_length(desc, freq_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x63: Partial transport stream descriptor */
static void build_desc63(uint8_t *desc) {
    desc63_init(desc);
    desc63_set_peak_rate(desc, 5000);
    desc63_set_min_overall_smoothing_rate(desc, 7000);
    desc63_set_max_overall_smoothing_buffer(desc, 1000);
}

/* DVB  Descriptor 0x64: Data broadcast descriptor */
static void build_desc64(uint8_t *desc) {
    char *selector_byte = "BY\"TE";
    char *text = "Some text";
    desc64_init(desc);
    desc64_set_broadcast_id(desc, 0x1122);
    desc64_set_component_tag(desc, 78);
    desc64_set_selector_byte(desc, (uint8_t *)selector_byte, strlen(selector_byte));
    desc64_set_lang(desc, (uint8_t *)"eng");
    desc64_set_text(desc, (uint8_t *)text, strlen(text));
    desc64_set_length(desc);
}

/* DVB  Descriptor 0x65: Scrambling descriptor */
static void build_desc65(uint8_t *desc) {
    desc65_init(desc);
    desc65_set_scrambling_mode(desc, 0x01);
}

/* DVB  Descriptor 0x66: Data broadcast id descriptor */
static void build_desc66(uint8_t *desc) {
    char *selector_byte = "UNIX";
    desc66_init(desc);
    desc66_set_broadcast_id(desc, 0x1122);
    desc66_set_selector_byte(desc, (uint8_t *)selector_byte, strlen(selector_byte));
}

/* DVB  Descriptor 0x67: Transport stream descriptor */
static void build_desc67(uint8_t *desc, char *bytes) {
    desc67_init(desc);
    desc67_set_bytes(desc, (uint8_t *)bytes, strlen(bytes));
}

/* DVB  Descriptor 0x68: DSNG_descriptor */
static void build_desc68(uint8_t *desc) {
    char *dsng_bytes = "1234,SNG_Headquarter,SNG_Provider";
    desc68_init(desc);
    desc68_set_bytes(desc, (uint8_t *)dsng_bytes, strlen(dsng_bytes));
}

/* DVB  Descriptor 0x69: PDC_descriptor */
static void build_desc69(uint8_t *desc) {
    desc69_init(desc);
//    desc69_set_pil(desc, 0x5d805);
    desc69_set_day(desc, 11);
    desc69_set_month(desc, 11);
    desc69_set_hour(desc, 0);
    desc69_set_minute(desc, 5);
}

/* DVB  Descriptor 0x6a: AC-3 descriptor */
static void build_desc6a(uint8_t *desc) {
    desc6a_init(desc);

    desc6a_set_component_type_flag  (desc, true);
    desc6a_set_component_type       (desc, 10);

    desc6a_set_bsid_flag            (desc, true);
    desc6a_set_bsid                 (desc, 20);

    desc6a_set_mainid_flag          (desc, true);
    desc6a_set_mainid               (desc, 30);

    desc6a_set_asvc_flag            (desc, true);
    desc6a_set_asvc                 (desc, 40);

    desc6a_set_length(desc);
}

/* DVB  Descriptor 0x6b: Ancillary data descriptor */
static void build_desc6b(uint8_t *desc) {
    desc6b_init(desc);
    desc6b_set_dvd_video_ancillary_data_flag    (desc, true);
    desc6b_set_extended_ancillary_data_flag     (desc, false);
    desc6b_set_announcement_switching_data_flag (desc, true);
    desc6b_set_dab_ancillary_data_flag          (desc, false);
    desc6b_set_scale_factor_error_check_flag    (desc, true);
    desc6b_set_mpeg4_ancillary_data_flag        (desc, false);
    desc6b_set_rds_via_uecp_flag                (desc, true);
}

/* DVB  Descriptor 0x6c: Cell list descriptor */
static void build_desc6c(uint8_t *desc) {
    uint8_t n = 0, k;
    uint8_t *cell_n, *subcell_k;

    desc6c_init(desc);
    desc_set_length(desc, 255);

    cell_n = desc6c_get_cell(desc, n++);
    desc6cn_set_cell_id(cell_n, 1234);
    desc6cn_set_cell_latitude(cell_n, 4567);
    desc6cn_set_cell_longtitude(cell_n, 5678);
    desc6cn_set_cell_extend_of_latitude(cell_n, 123);
    desc6cn_set_cell_extend_of_longtitude(cell_n, 345);
    desc6cn_set_subcell_info_loop_length(cell_n, 0);

    cell_n = desc6c_get_cell(desc, n++);
    desc6cn_set_cell_id(cell_n, 4456);
    desc6cn_set_cell_latitude(cell_n, 5567);
    desc6cn_set_cell_longtitude(cell_n, 6678);
    desc6cn_set_cell_extend_of_latitude(cell_n, 1234);
    desc6cn_set_cell_extend_of_longtitude(cell_n, 1234);
    desc6cn_set_subcell_info_loop_length(cell_n, 0);
    {
        k = 0;
        desc6cn_set_subcell_info_loop_length(cell_n, 255);

        subcell_k = desc6cn_get_subcell(cell_n, k++);
        desc6ck_set_cell_id_extension(subcell_k, 0);
        desc6ck_set_subcell_latitude(subcell_k, 0x1122);
        desc6ck_set_subcell_longtitude(subcell_k, 0x3344);
        desc6ck_set_subcell_extend_of_latitude(subcell_k, 0xf567);
        desc6ck_set_subcell_extend_of_longtitude(subcell_k, 0xf89a);

        subcell_k = desc6cn_get_subcell(cell_n, k++);
        desc6ck_set_cell_id_extension(subcell_k, 1);
        desc6ck_set_subcell_latitude(subcell_k, 0x2233);
        desc6ck_set_subcell_longtitude(subcell_k, 0x4455);
        desc6ck_set_subcell_extend_of_latitude(subcell_k, 0xf678);
        desc6ck_set_subcell_extend_of_longtitude(subcell_k, 0xf9ab);

        subcell_k = desc6cn_get_subcell(cell_n, k++);
        desc6ck_set_cell_id_extension(subcell_k, 2);
        desc6ck_set_subcell_latitude(subcell_k, 0x2233);
        desc6ck_set_subcell_longtitude(subcell_k, 0x4455);
        desc6ck_set_subcell_extend_of_latitude(subcell_k, 0xf678);
        desc6ck_set_subcell_extend_of_longtitude(subcell_k, 0xf9ab);

        subcell_k = desc6cn_get_subcell(cell_n, k);
        desc6cn_set_subcell_info_loop_length(cell_n, subcell_k - cell_n - DESC6C_DATA_SIZE);
    }

    cell_n = desc6c_get_cell(desc, n++);
    desc6cn_set_cell_id(cell_n, 0xffff);
    desc6cn_set_cell_latitude(cell_n, 0xeeee);
    desc6cn_set_cell_longtitude(cell_n, 0xdddd);
    desc6cn_set_cell_extend_of_latitude(cell_n, 0xf789);
    desc6cn_set_cell_extend_of_longtitude(cell_n, 0xfabc);
    desc6cn_set_subcell_info_loop_length(cell_n, 0);
    {
        k = 0;
        desc6cn_set_subcell_info_loop_length(cell_n, 255);

        subcell_k = desc6cn_get_subcell(cell_n, k++);
        desc6ck_set_cell_id_extension(subcell_k, 0x00);
        desc6ck_set_subcell_latitude(subcell_k, 0x1122);
        desc6ck_set_subcell_longtitude(subcell_k, 0x3344);
        desc6ck_set_subcell_extend_of_latitude(subcell_k, 0xf567);
        desc6ck_set_subcell_extend_of_longtitude(subcell_k, 0xf89a);

        subcell_k = desc6cn_get_subcell(cell_n, k);
        desc6cn_set_subcell_info_loop_length(cell_n, subcell_k - cell_n - DESC6C_DATA_SIZE);
    }

    cell_n = desc6c_get_cell(desc, n);
    desc_set_length(desc, cell_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x6d: Cell frequency link descriptor */
static void build_desc6d(uint8_t *desc) {
    uint8_t n = 0, k;
    uint8_t *cell_n, *subcell_k;

    desc6d_init(desc);
    desc_set_length(desc, 255);

    cell_n = desc6d_get_cell(desc, n++);
    desc6dn_set_cell_id(cell_n, 1234);
    desc6dn_set_frequency(cell_n, 4567);
    desc6dn_set_subcell_info_loop_length(cell_n, 0);

    cell_n = desc6d_get_cell(desc, n++);
    desc6dn_set_cell_id(cell_n, 4456);
    desc6dn_set_frequency(cell_n, 5567);
    desc6dn_set_subcell_info_loop_length(cell_n, 0);
    {
        k = 0;
        desc6dn_set_subcell_info_loop_length(cell_n, 255);

        subcell_k = desc6dn_get_subcell(cell_n, k++);
        desc6dk_set_cell_id_extension(subcell_k, 0);
        desc6dk_set_transponder_frequency(subcell_k, 1122);

        subcell_k = desc6dn_get_subcell(cell_n, k++);
        desc6dk_set_cell_id_extension(subcell_k, 1);
        desc6dk_set_transponder_frequency(subcell_k, 2233);

        subcell_k = desc6dn_get_subcell(cell_n, k++);
        desc6dk_set_cell_id_extension(subcell_k, 2);
        desc6dk_set_transponder_frequency(subcell_k, 3344);

        subcell_k = desc6dn_get_subcell(cell_n, k);
        desc6dn_set_subcell_info_loop_length(cell_n, subcell_k - cell_n - DESC6D_DATA_SIZE);
    }

    cell_n = desc6d_get_cell(desc, n++);
    desc6dn_set_cell_id(cell_n, 7890);
    desc6dn_set_frequency(cell_n, 56789);
    desc6dn_set_subcell_info_loop_length(cell_n, 0);
    {
        k = 0;
        desc6dn_set_subcell_info_loop_length(cell_n, 255);

        subcell_k = desc6dn_get_subcell(cell_n, k++);
        desc6dk_set_cell_id_extension(subcell_k, 0);
        desc6dk_set_transponder_frequency(subcell_k, 889911);

        subcell_k = desc6dn_get_subcell(cell_n, k);
        desc6dn_set_subcell_info_loop_length(cell_n, subcell_k - cell_n - DESC6D_DATA_SIZE);
    }

    cell_n = desc6d_get_cell(desc, n);
    desc_set_length(desc, cell_n - desc - DESC_HEADER_SIZE);
}

/* DVB  Descriptor 0x6e: Announcement support descriptor */
static void build_desc6e(uint8_t *desc) {
    desc6e_init(desc);

    desc6e_set_emergency_alarm_flag         (desc, true);
    desc6e_set_road_traffic_flash_flag      (desc, false);
    desc6e_set_public_transport_flash_flag  (desc, true);
    desc6e_set_warning_message_flag         (desc, true);
    desc6e_set_news_flash_flag              (desc, true);
    desc6e_set_weather_flash_flag           (desc, true);
    desc6e_set_event_announcement_flag      (desc, false);
    desc6e_set_personal_call_flag           (desc, false);

    {
        uint8_t n = 0;
        uint8_t *ann_n;
        desc_set_length(desc, 255);

        ann_n = desc6e_get_announcement(desc, n++);
        desc6en_set_announcement_type(ann_n, 0); // Emergency alarm
        desc6en_set_reference_type(ann_n, 0);

        ann_n = desc6e_get_announcement(desc, n++);
        desc6en_set_announcement_type(ann_n, 3); // Warning alarm
        desc6en_set_reference_type(ann_n, 1);

        // The following are valid only if
        //  reference_type == 0x01
        //  || reference_type == 0x02
        //  || reference_type == 0x03
        //
        desc6en_set_onid(ann_n, 0x1122);
        desc6en_set_tsid(ann_n, 0x3344);
        desc6en_set_service_id(ann_n, 0x5566);
        desc6en_set_component_tag(ann_n, 0x77);

        ann_n = desc6e_get_announcement(desc, n++);
        desc6en_set_announcement_type(ann_n, 5); // Weather flash
        desc6en_set_reference_type(ann_n, 0);

        ann_n = desc6e_get_announcement(desc, n++);
        desc6en_set_announcement_type(ann_n, 2); // Public Transport flash
        desc6en_set_reference_type(ann_n, 3);

        // The following are valid only if
        //  reference_type == 0x01
        //  || reference_type == 0x02
        //  || reference_type == 0x03
        //
        desc6en_set_onid(ann_n, 0x6677);
        desc6en_set_tsid(ann_n, 0x8899);
        desc6en_set_service_id(ann_n, 0x4455);
        desc6en_set_component_tag(ann_n, 0x88);

        ann_n = desc6e_get_announcement(desc, n++);
        desc6en_set_announcement_type(ann_n, 4); // News flash
        desc6en_set_reference_type(ann_n, 0);

        ann_n = desc6e_get_announcement(desc, n);
        desc_set_length(desc, ann_n - desc - DESC_HEADER_SIZE);
    }
}

/* ---  Descriptor 0x6f: application_signalling_descriptor */
/* ---  Descriptor 0x70: adaptation_field_data_descriptor */
/* ---  Descriptor 0x71: service_identifier_descriptor */
/* ---  Descriptor 0x72: service_availability_descriptor */
/* ---  Descriptor 0x73: default_authority_descriptor */
/* ---  Descriptor 0x74: related_content_descriptor */
/* ---  Descriptor 0x75: TVA_id_descriptor */
/* ---  Descriptor 0x76: content_identifier_descriptor */
/* ---  Descriptor 0x77: time_slice_fec_identifier_descriptor */
/* ---  Descriptor 0x78: ECM_repetition_rate_descriptor */
/* ---  Descriptor 0x79: S2_satellite_delivery_system_descriptor */

/* DVB  Descriptor 0x7a: Enhanced AC-3 descriptor */
static void build_desc7a(uint8_t *desc) {
    desc7a_init(desc);

    desc7a_set_component_type_flag  (desc, true);
    desc7a_set_component_type       (desc, 10);

    desc7a_set_bsid_flag            (desc, true);
    desc7a_set_bsid                 (desc, 20);

    desc7a_set_mainid_flag          (desc, true);
    desc7a_set_mainid               (desc, 30);

    desc7a_set_asvc_flag            (desc, true);
    desc7a_set_asvc                 (desc, 40);

    desc7a_set_mixinfoexists_flag   (desc, true);

    desc7a_set_substream1_flag      (desc, true);
    desc7a_set_substream1           (desc, 50);

    desc7a_set_substream2_flag      (desc, true);
    desc7a_set_substream2           (desc, 60);

    desc7a_set_substream3_flag      (desc, true);
    desc7a_set_substream3           (desc, 70);

    desc7a_set_length(desc);
}

/* DVB  Descriptor 0x7b: DTS descriptor */
static void build_desc7b(uint8_t *desc) {
    desc7b_init(desc);
    desc7b_set_sample_rate_code(desc, 8);
    desc7b_set_bit_rate_code(desc, 11);
    desc7b_set_nblks(desc, 5);
    desc7b_set_fsize(desc, 95);
    desc7b_set_surround_mode(desc, 3);
    desc7b_set_lfe_flag(desc, true);
    desc7b_set_extended_surround_flag(desc, 2);
}

/* DVB  Descriptor 0x7c: AAC descriptor */
static void build_desc7c(uint8_t *desc) {
    desc7c_init(desc);
    desc7c_set_profile_and_level(desc, 0x14);
    desc7c_set_aac_type_flag(desc, true);
    desc7c_set_aac_type(desc, 0x1f);
}

/* ---  Descriptor 0x7d: XAIT location descriptor */
/* ---  Descriptor 0x7e: FTA_content_management_descriptor */
/* ---  Descriptor 0x7f: extension descriptor */

static void output_psi_section(uint8_t *section, uint16_t pid, uint8_t *cc) {
    uint16_t section_length = psi_get_length(section) + PSI_HEADER_SIZE;
    uint16_t section_offset = 0;
    do {
        uint8_t ts[TS_SIZE];
        uint8_t ts_offset = 0;
        memset(ts, 0xff, TS_SIZE);

        psi_split_section(ts, &ts_offset, section, &section_offset);

        ts_set_pid(ts, pid);
        ts_set_cc(ts, *cc);
        (*cc)++;
        *cc &= 0xf;

        if (section_offset == section_length)
            psi_split_end(ts, &ts_offset);

        if (write(fileno(stdout), ts, TS_SIZE) < 0)
            perror("write");
    } while (section_offset < section_length);
}

/* =========================================================================
 * SI Tables
 * ========================================================================= */

/* MPEG Program Allocation Table (PAT) */
static void generate_pat(void) {
    // Generate empty PAT
    uint8_t *pat = psi_allocate();
    uint8_t *pat_n;
    uint8_t j = 0;

    // Generate empty PAT
    pat_init(pat);
    pat_set_length(pat, 0);
    pat_set_tsid(pat, tsid);
    psi_set_version(pat, 0);
    psi_set_current(pat);
    psi_set_section(pat, 0);
    psi_set_lastsection(pat, 0);
    psi_set_crc(pat);
    output_psi_section(pat, PAT_PID, &cc);

    // Increase PAT version
    psi_set_version(pat, 1);
    psi_set_current(pat);
    psi_set_crc(pat);
    output_psi_section(pat, PAT_PID, &cc);

    // Add couple of programs to PAT
    psi_set_version(pat, 2);
    psi_set_current(pat);
    psi_set_length(pat, PSI_MAX_SIZE);

    pat_n = pat_get_program(pat, j++);
    patn_init(pat_n);
    patn_set_program(pat_n, 0);
    patn_set_pid(pat_n, NIT_PID);

    pat_n = pat_get_program(pat, j++);
    patn_init(pat_n);
    patn_set_program(pat_n, sid);
    patn_set_pid(pat_n, pmt_pid);

    pat_n = pat_get_program(pat, j++);
    patn_init(pat_n);
    patn_set_program(pat_n, sid + 100);
    patn_set_pid(pat_n, pmt_pid + 100);

    pat_n = pat_get_program(pat, j++);
    patn_init(pat_n);
    patn_set_program(pat_n, sid + 200);
    patn_set_pid(pat_n, pmt_pid + 200);

    pat_n = pat_get_program(pat, j++);
    patn_init(pat_n);
    patn_set_program(pat_n, sid + 300);
    patn_set_pid(pat_n, pmt_pid + 300);

    // Set correct PAT length
    pat_n = pat_get_program(pat, j); // Get offset of the end of last program
    pat_set_length(pat, pat_n - pat - PAT_HEADER_SIZE);
    psi_set_crc(pat);

    output_psi_section(pat, PAT_PID, &cc);

    free(pat);
}

/* MPEG Conditional Access Table (CAT) */
static void generate_cat(void) {
    // Generate empty cat
    uint8_t *cat = psi_allocate();
    uint8_t *desc;
    uint8_t desc_loop[DESCS_HEADER_SIZE + DESCS_MAX_SIZE];
    uint8_t desc_counter;

    cat_init(cat);
    cat_set_length(cat, 0);
    psi_set_version(cat, 0);
    psi_set_current(cat);
    psi_set_crc(cat);
    output_psi_section(cat, CAT_PID, &cc);

    // Increase CAT version
    psi_set_version(cat, 1);
    psi_set_current(cat);
    psi_set_crc(cat);
    output_psi_section(cat, CAT_PID, &cc);

    // Add couple of descriptors to CAT
    psi_set_version(cat, 2);
    psi_set_current(cat);
    psi_set_length(cat, PSI_MAX_SIZE);

    descs_set_length(desc_loop, DESCS_MAX_SIZE);
    desc_counter = 0;

    desc = descs_get_desc(desc_loop, desc_counter++);
    build_desc09(desc);

    desc = descs_get_desc(desc_loop, desc_counter++);
    build_desc09(desc);

    desc = descs_get_desc(desc_loop, desc_counter++);
    build_desc09(desc);

    // Finish descriptor generation
    desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
    descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
    cat_set_desclength(cat, descs_get_length(desc_loop));
    // Put descriptor loop into CAT
    memcpy(cat_get_descl(cat), desc_loop + DESCS_HEADER_SIZE,
        descs_get_length(desc_loop));

    psi_set_crc(cat);
    output_psi_section(cat, CAT_PID, &cc);

    free(cat);
}

/* MPEG Conditional Access Table (TSDT) */
static void generate_tsdt(void) {
    // Generate empty tsdt
    uint8_t *tsdt = psi_allocate();
    uint8_t *desc;
    uint8_t desc_loop[DESCS_HEADER_SIZE + DESCS_MAX_SIZE];
    uint8_t desc_counter;

    tsdt_init(tsdt);
    tsdt_set_length(tsdt, 0);
    psi_set_version(tsdt, 0);
    psi_set_current(tsdt);
    psi_set_crc(tsdt);
    output_psi_section(tsdt, TSDT_PID, &cc);

    // Add couple of descriptors to TSDT
    psi_set_version(tsdt, 1);
    psi_set_current(tsdt);
    psi_set_length(tsdt, PSI_MAX_SIZE);

    descs_set_length(desc_loop, DESCS_MAX_SIZE);
    desc_counter = 0;

    desc = descs_get_desc(desc_loop, desc_counter++);
    build_desc67(desc, "DVB");

    desc = descs_get_desc(desc_loop, desc_counter++);
    build_desc67(desc, "CONT");

    desc = descs_get_desc(desc_loop, desc_counter++);
    build_desc68(desc);

    // Finish descriptor generation
    desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
    descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
    tsdt_set_desclength(tsdt, descs_get_length(desc_loop));
    // Put descriptor loop into TSDT
    memcpy(tsdt_get_descl(tsdt), desc_loop + DESCS_HEADER_SIZE,
        descs_get_length(desc_loop));

    psi_set_crc(tsdt);
    output_psi_section(tsdt, TSDT_PID, &cc);

    free(tsdt);
}

/* DVB  Network Information Table (NIT) */
static void generate_nit(void) {
    uint8_t *nit = psi_allocate();
    uint8_t *nit_tsloop;
    uint8_t *desc_loop, *desc;
    uint8_t desc_counter;

    // Generate empty nit
    nit_init(nit, true);
    psi_set_version(nit, 0);
    psi_set_current(nit);
    nit_set_nid(nit, onid);
    nit_set_length(nit, 2);

    nit_set_desclength(nit, 0);

    nit_tsloop = nit_get_header2(nit);
    nith_init(nit_tsloop);
    nith_set_tslength(nit_tsloop, 0);

    psi_set_crc(nit);

    output_psi_section(nit, NIT_PID, &cc);

    // Add descriptors and transport stream loop
    nit_init(nit, true);
    psi_set_version(nit, 1);
    psi_set_current(nit);
    nit_set_nid(nit, onid);
    nit_set_length(nit, PSI_MAX_SIZE);
    nit_set_desclength(nit, 0);

    {
        // Add descriptors to network descriptors
        desc_counter = 0;
        desc_loop = nit_get_descs(nit);

        nit_set_desclength(nit, DESCS_MAX_SIZE);
        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc40(desc);

        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc43(desc);

        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc44(desc);

        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc5a(desc);

        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc5b(desc);

        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc62(desc);

        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc6c(desc);

        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc6d(desc);

        // Finish descriptor generation
        desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
        nit_set_desclength(nit, desc - desc_loop - DESCS_HEADER_SIZE);
        nit_set_length(nit, 2 + nit_get_desclength(nit));
    }

    // Process transport stream loop
    nit_tsloop = nit_get_header2(nit);
    nith_init(nit_tsloop);
    nit_set_length(nit, PSI_MAX_SIZE); // This needed so nit_get_ts works
    {
        uint8_t *nit_n;
        uint8_t nit_n_counter = 0;

        nit_n = nit_get_ts(nit, nit_n_counter++);
        nitn_init(nit_n);
        nitn_set_tsid(nit_n, tsid);
        nitn_set_onid(nit_n, onid);
        nitn_set_desclength(nit_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = nitn_get_descs(nit_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc41(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc4a_std(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        nit_n = nit_get_ts(nit, nit_n_counter++);
        nitn_init(nit_n);
        nitn_set_tsid(nit_n, tsid + 100);
        nitn_set_onid(nit_n, onid + 100);
        nitn_set_desclength(nit_n, 0);

        nit_n = nit_get_ts(nit, nit_n_counter++);
        nitn_init(nit_n);
        nitn_set_tsid(nit_n, tsid + 200);
        nitn_set_onid(nit_n, onid + 200);
        nitn_set_onid(nit_n, onid + 200);
        nitn_set_desclength(nit_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = nitn_get_descs(nit_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc41(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc4a_mobile(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        nit_n = nit_get_ts(nit, nit_n_counter++);
        nitn_init(nit_n);
        nitn_set_tsid(nit_n, tsid + 300);
        nitn_set_onid(nit_n, onid + 300);
        nitn_set_desclength(nit_n, 0);

        // Set transport_stream_loop length
        nit_n = nit_get_ts(nit, nit_n_counter); // Get last ts
        nith_set_tslength(nit_tsloop, nit_n - nit_tsloop - NIT_HEADER2_SIZE);
    }
    // Set NIT length
    nit_set_length(nit, 2 + nit_get_desclength(nit) + nith_get_tslength(nit_tsloop));
    psi_set_crc(nit);

    output_psi_section(nit, NIT_PID, &cc);

    free(nit);
}

/* DVB  Bouquet Association Table (BAT) */
static void generate_bat(void) {
    uint8_t *bat = psi_allocate();
    uint8_t *bat_tsloop;
    uint8_t *desc_loop, *desc;
    uint8_t desc_counter;

    // Generate empty BAT
    bat_init(bat);
    psi_set_version(bat, 0);
    psi_set_current(bat);
    bat_set_bouquet_id(bat, onid);
    bat_set_length(bat, 2);

    bat_set_desclength(bat, 0);

    bat_tsloop = bat_get_header2(bat);
    bath_init(bat_tsloop);
    bath_set_tslength(bat_tsloop, 0);

    psi_set_crc(bat);

    output_psi_section(bat, BAT_PID, &cc);

    // Add descriptors and transport stream loop
    bat_init(bat);
    psi_set_version(bat, 1);
    psi_set_current(bat);
    bat_set_bouquet_id(bat, onid);
    bat_set_length(bat, PSI_MAX_SIZE);
    bat_set_desclength(bat, 0);

    {
        // Add descriptors to bouquet descriptors
        bat_set_desclength(bat, DESCS_MAX_SIZE);

        desc_counter = 0;
        desc_loop = bat_get_descs(bat);

        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc47(desc);

        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc5c(desc);

        // Finish descriptor generation
        desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
        bat_set_desclength(bat, desc - desc_loop - DESCS_HEADER_SIZE);
        bat_set_length(bat, 2 + bat_get_desclength(bat));
    }

    // Process transport stream loop
    bat_tsloop = bat_get_header2(bat);
    bath_init(bat_tsloop);
    bat_set_length(bat, PSI_MAX_SIZE); // This needed so bat_get_ts works
    {
        uint8_t *bat_n;
        uint8_t bat_n_counter = 0;

        bat_n = bat_get_ts(bat, bat_n_counter++);
        batn_init(bat_n);
        batn_set_tsid(bat_n, tsid);
        batn_set_onid(bat_n, onid);
        batn_set_desclength(bat_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = batn_get_descs(bat_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc41(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        bat_n = bat_get_ts(bat, bat_n_counter++);
        batn_init(bat_n);
        batn_set_tsid(bat_n, tsid + 100);
        batn_set_onid(bat_n, onid + 100);
        batn_set_desclength(bat_n, 0);

        bat_n = bat_get_ts(bat, bat_n_counter++);
        batn_init(bat_n);
        batn_set_tsid(bat_n, tsid + 200);
        batn_set_onid(bat_n, onid + 200);
        batn_set_desclength(bat_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = batn_get_descs(bat_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc41(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        bat_n = bat_get_ts(bat, bat_n_counter++);
        batn_init(bat_n);
        batn_set_tsid(bat_n, tsid + 300);
        batn_set_onid(bat_n, onid + 300);
        batn_set_desclength(bat_n, 0);

        // Set transport_stream_loop length
        bat_n = bat_get_ts(bat, bat_n_counter); // Get last ts
        bath_set_tslength(bat_tsloop, bat_n - bat_tsloop - BAT_HEADER2_SIZE);
    }
    // Set BAT length
    bat_set_length(bat, 2 + bat_get_desclength(bat) + bath_get_tslength(bat_tsloop));
    psi_set_crc(bat);

    output_psi_section(bat, BAT_PID, &cc);

    free(bat);
}

/* DVB  Service Definition Table (SDT) */
static void generate_sdt(void) {
    uint8_t *sdt = psi_allocate();
    uint8_t *desc_loop, *desc;
    uint8_t desc_counter;

    // Generate empty SDT
    sdt_init(sdt, true);
    psi_set_version(sdt, 0);
    psi_set_current(sdt);
    sdt_set_tsid(sdt, tsid);
    sdt_set_onid(sdt, onid);
    sdt_set_length(sdt, 0);
    psi_set_crc(sdt);
    output_psi_section(sdt, SDT_PID, &cc);

    // Add service descriptions
    sdt_init(sdt, true);
    psi_set_version(sdt, 1);
    psi_set_current(sdt);
    sdt_set_tsid(sdt, tsid);
    sdt_set_onid(sdt, onid);

    // Process transport stream loop
    sdt_set_length(sdt, PSI_MAX_SIZE); // This needed so sdt_get_ts works
    {
        uint8_t *sdt_n;
        uint8_t sdt_n_counter = 0;

        sdt_n = sdt_get_service(sdt, sdt_n_counter++);
        sdtn_init(sdt_n);
        sdtn_set_sid(sdt_n, sid);
        sdtn_set_running(sdt_n, 2);
        sdtn_set_desclength(sdt_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = sdtn_get_descs(sdt_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc48(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc49(desc, true);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc51(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc57(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc5d(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc5f(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        sdt_n = sdt_get_service(sdt, sdt_n_counter++);
        sdtn_init(sdt_n);
        sdtn_set_sid(sdt_n, sid + 100);
        sdtn_set_eitschedule(sdt_n);
        sdtn_set_running(sdt_n, 1);
        sdtn_set_desclength(sdt_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = sdtn_get_descs(sdt_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc48(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc49(desc, false);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc5f(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc6e(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        sdt_n = sdt_get_service(sdt, sdt_n_counter++);
        sdtn_init(sdt_n);
        sdtn_set_sid(sdt_n, sid + 200);
        sdtn_set_eitschedule(sdt_n);
        sdtn_set_eitpresent(sdt_n);
        sdtn_set_running(sdt_n, 3);
        sdtn_set_desclength(sdt_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = sdtn_get_descs(sdt_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc48(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc4c(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc5f(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc64(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc66(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        sdt_n = sdt_get_service(sdt, sdt_n_counter++);
        sdtn_init(sdt_n);
        sdtn_set_sid(sdt_n, sid + 300);
        sdtn_set_eitschedule(sdt_n);
        sdtn_set_eitpresent(sdt_n);
        sdtn_set_running(sdt_n, 2);
        sdtn_set_ca(sdt_n);
        sdtn_set_desclength(sdt_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = sdtn_get_descs(sdt_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc48(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc4b(desc, false);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc5f(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        // Set transport_stream_loop length
        sdt_n = sdt_get_service(sdt, sdt_n_counter); // Get last service
        sdt_set_length(sdt, sdt_n - sdt_get_service(sdt, 0));
    }
    psi_set_crc(sdt);
    output_psi_section(sdt, SDT_PID, &cc);

    free(sdt);
}

/* DVB  Event Information Table (EIT) */
static void generate_eit(void) {
    uint8_t *eit = psi_private_allocate();
    uint8_t *desc_loop, *desc;
    uint8_t desc_counter;

    // Generate empty EIT
    eit_init(eit, true);
    psi_set_version(eit, 0);
    psi_set_current(eit);

    eit_set_length(eit, 0);
    eit_set_sid(eit, sid);
    eit_set_tsid(eit, tsid);
    eit_set_onid(eit, onid);
    eit_set_segment_last_sec_number(eit, 0);
    eit_set_last_table_id(eit, 0);
    psi_set_crc(eit);
    output_psi_section(eit, EIT_PID, &cc);

    // Add service descriptions
    eit_init(eit, true);
    psi_set_version(eit, 1);
    psi_set_current(eit);
    eit_set_tsid(eit, tsid);
    eit_set_onid(eit, onid);

    // Process transport stream loop
    eit_set_length(eit, PSI_MAX_SIZE); // This needed so eit_get_ts works
    {
        uint8_t *eit_n;
        uint8_t eit_n_counter = 0;

        eit_n = eit_get_event(eit, eit_n_counter++);
        eitn_set_event_id(eit_n, event_id);
        eitn_set_start_time(eit_n, dvb_time_encode_UTC(ts_0));
        eitn_set_duration_bcd(eit_n, dvb_time_encode_duration(86399));
        eitn_set_running(eit_n, 2);
        eitn_set_desclength(eit_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = eitn_get_descs(eit_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc4d(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        eit_n = eit_get_event(eit, eit_n_counter++);
        eitn_set_event_id(eit_n, event_id + 100);
        eitn_set_start_time(eit_n, dvb_time_encode_UTC(ts_1));
        eitn_set_duration_bcd(eit_n, dvb_time_encode_duration(3600));
        eitn_set_running(eit_n, 1);
        eitn_set_desclength(eit_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = eitn_get_descs(eit_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc4a_event(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc4e(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc4d(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc61(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        eit_n = eit_get_event(eit, eit_n_counter++);
        eitn_set_event_id(eit_n, event_id + 200);
        eitn_set_start_time(eit_n, dvb_time_encode_UTC(ts_2));
        eitn_set_duration_bcd(eit_n, dvb_time_encode_duration(7200));
        eitn_set_running(eit_n, 0);
        eitn_set_desclength(eit_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = eitn_get_descs(eit_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc4a_extended_event(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc54(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc53(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc55(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc4d(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc4f(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        eit_n = eit_get_event(eit, eit_n_counter++);
        eitn_set_event_id(eit_n, event_id + 300);
        eitn_set_start_time(eit_n, dvb_time_encode_UTC(ts_2));
        eitn_set_duration_bcd(eit_n, dvb_time_encode_duration(7200));
        eitn_set_running(eit_n, 4);
        eitn_set_ca(eit_n);
        eitn_set_desclength(eit_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = eitn_get_descs(eit_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc54(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc4d(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc4e(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc55(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc69(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        // Set transport_stream_loop length
        eit_n = eit_get_event(eit, eit_n_counter); // Get last service
        eit_set_length(eit, eit_n - eit_get_event(eit, 0));
    }
    psi_set_crc(eit);
    output_psi_section(eit, EIT_PID, &cc);

    free(eit);
}

/* DVB  Time and Date Table (TDT) */
static void generate_tdt(void) {
    uint8_t *tdt = psi_allocate();

    tdt_init(tdt);
    tdt_set_utc(tdt, dvb_time_encode_UTC(ts_0));
    if (dvb_time_decode_UTC(tdt_get_utc(tdt)) != ts_0)
        fprintf(stderr, "check tdt_get_set_utc!!!\n");
    output_psi_section(tdt, TDT_PID, &cc);

    tdt_set_utc(tdt, dvb_time_encode_UTC(ts_1));
    if (dvb_time_decode_UTC(tdt_get_utc(tdt)) != ts_1)
        fprintf(stderr, "check tdt_get_set_utc!!!\n");
    output_psi_section(tdt, TDT_PID, &cc);

    tdt_set_utc(tdt, dvb_time_encode_UTC(ts_2));
    if (dvb_time_decode_UTC(tdt_get_utc(tdt)) != ts_2)
        fprintf(stderr, "check tdt_get_set_utc!!!\n");
    output_psi_section(tdt, TDT_PID, &cc);

    tdt_set_utc(tdt, dvb_time_encode_UTC(ts_3));
    if (dvb_time_decode_UTC(tdt_get_utc(tdt)) != ts_3)
        fprintf(stderr, "check tdt_get_set_utc!!!\n");
    output_psi_section(tdt, TDT_PID, &cc);

    free(tdt);
}

/* DVB  Time Offset Table (TOT) */
static void generate_tot(void) {
    uint8_t *tot = psi_allocate();
    uint8_t *desc_loop, *desc;
    uint8_t desc_counter;

    tot_init(tot);
    tot_set_length(tot, 0);
    tot_set_desclength(tot, 0);

    tot_set_utc(tot, dvb_time_encode_UTC(ts_0));
    if (dvb_time_decode_UTC(tot_get_utc(tot)) != ts_0)
        fprintf(stderr, "check tot_get_set_utc!!!\n");
    psi_set_crc(tot);
    output_psi_section(tot, TOT_PID, &cc);

    tot_set_utc(tot, dvb_time_encode_UTC(ts_1));
    if (dvb_time_decode_UTC(tot_get_utc(tot)) != ts_1)
        fprintf(stderr, "check tot_get_set_utc!!!\n");
    psi_set_crc(tot);
    output_psi_section(tot, TOT_PID, &cc);

    /* Change time */
    tot_set_utc(tot, dvb_time_encode_UTC(ts_2));

    /* Add some descriptors */
    desc_loop = tot_get_descs(tot); // First descriptor
    tot_set_desclength(tot, DESCS_MAX_SIZE);
    desc_counter = 0;

    desc = descs_get_desc(desc_loop, desc_counter++);
    build_desc58(desc);

    // Finish descriptor generation
    desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
    tot_set_desclength(tot, desc - desc_loop - DESCS_HEADER_SIZE);
    tot_set_length(tot, tot_get_desclength(tot) + DESCS_HEADER_SIZE);
    psi_set_crc(tot);

    output_psi_section(tot, TOT_PID, &cc);

    free(tot);
}

/* DVB  Running Status Table (RST) */
static void generate_rst(void) {
    uint8_t *rst = psi_allocate();
    uint8_t *rst_n;
    uint8_t j = 0;

    // Generate empty rst
    rst_init(rst);
    rst_set_length(rst, 0);
    output_psi_section(rst, RST_PID, &cc);

    // Add couple of statuses to rst
    rst_set_length(rst, PSI_MAX_SIZE);

    rst_n = rst_get_status(rst, j++);
    rstn_init(rst_n);
    rstn_set_tsid(rst_n, tsid);
    rstn_set_onid(rst_n, onid);
    rstn_set_service_id(rst_n, sid);
    rstn_set_event_id(rst_n, event_id);
    rstn_set_running(rst_n, 1);

    rst_n = rst_get_status(rst, j++);
    rstn_init(rst_n);
    rstn_set_tsid(rst_n, tsid + 100);
    rstn_set_onid(rst_n, onid + 100);
    rstn_set_service_id(rst_n, sid + 100);
    rstn_set_event_id(rst_n, event_id + 100);
    rstn_set_running(rst_n, 2);

    rst_n = rst_get_status(rst, j++);
    rstn_init(rst_n);
    rstn_set_tsid(rst_n, tsid + 200);
    rstn_set_onid(rst_n, onid + 200);
    rstn_set_service_id(rst_n, sid + 200);
    rstn_set_event_id(rst_n, event_id + 200);
    rstn_set_running(rst_n, 3);

    // Set correct rst length
    rst_n = rst_get_status(rst, j); // Get offset of the end of last status
    rst_set_length(rst, rst_n - rst - RST_HEADER_SIZE);

    output_psi_section(rst, RST_PID, &cc);

    free(rst);
}

/* MPEG Program Map Table (PMT) */
static void generate_pmt(void) {
    uint8_t *pmt = psi_allocate();
    uint8_t *pmt_n;
    uint8_t pmt_n_counter;
    uint8_t *desc_loop, *desc;
    uint8_t desc_counter;

    // Generate empty PMT
    pmt_init(pmt);
    psi_set_version(pmt, 0);
    psi_set_current(pmt);

    pmt_set_length(pmt, 0);
    pmt_set_program(pmt, sid);
    pmt_set_pcrpid(pmt, pmt_pid + 10);
    pmt_set_desclength(pmt, 0);

    psi_set_crc(pmt);
    output_psi_section(pmt, pmt_pid, &cc);

    // Add elementary streams
    pmt_init(pmt);
    psi_set_version(pmt, 1);
    psi_set_current(pmt);
    pmt_set_pcrpid(pmt, pmt_pid + 10);

    {
        // Add descriptors to program descriptors
        pmt_set_length(pmt, PSI_MAX_SIZE);
        pmt_set_desclength(pmt, DESCS_MAX_SIZE);

        desc_counter = 0;
        desc_loop = pmt_get_descs(pmt);

        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc0b(desc);

        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc0c(desc);

        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc0d(desc);

        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc0e(desc);

        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc0f(desc);

        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc10(desc);

        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc12(desc);

        // Finish descriptor generation
        desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
        pmt_set_desclength(pmt, desc - desc_loop - DESCS_HEADER_SIZE);
        pmt_set_length(pmt, pmt_get_desclength(pmt));
    }

    {
        pmt_set_length(pmt, PSI_MAX_SIZE); // This needed so pmt_get_es works

        // Process elementary streams
        pmt_n_counter = 0;

        pmt_n = pmt_get_es(pmt, pmt_n_counter++);
        pmtn_init(pmt_n);
        pmtn_set_streamtype(pmt_n, 0x02); // MPEG2 Video
        pmtn_set_pid(pmt_n, pmt_pid + 20);
        pmtn_set_desclength(pmt_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = pmtn_get_descs(pmt_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc02_1(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc02_2(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc04(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc05(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc06(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc07(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc08(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc11(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc1b(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        pmt_n = pmt_get_es(pmt, pmt_n_counter++);
        pmtn_init(pmt_n);
        pmtn_set_streamtype(pmt_n, 0x04); // MPEG2 audio
        pmtn_set_pid(pmt_n, pmt_pid + 21);
        pmtn_set_desclength(pmt_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = pmtn_get_descs(pmt_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc03(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc0a(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc52(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc1c(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc50(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc5e(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc6a(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc7a(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc7b(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        pmt_n = pmt_get_es(pmt, pmt_n_counter++);
        pmtn_init(pmt_n);
        pmtn_set_streamtype(pmt_n, 0x06); // PES Private Data (desc46 - VBI teletext)
        pmtn_set_pid(pmt_n, pmt_pid + 22);
        pmtn_set_desclength(pmt_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = pmtn_get_descs(pmt_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc46(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc45(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        pmt_n = pmt_get_es(pmt, pmt_n_counter++);
        pmtn_init(pmt_n);
        pmtn_set_streamtype(pmt_n, 0x06); // PES Private Data (desc56 - Teletext)
        pmtn_set_pid(pmt_n, pmt_pid + 23);
        pmtn_set_desclength(pmt_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = pmtn_get_descs(pmt_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc56(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc60(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc65(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        pmt_n = pmt_get_es(pmt, pmt_n_counter++);
        pmtn_init(pmt_n);
        pmtn_set_streamtype(pmt_n, 0x06); // PES Private Data (desc59 - Subtitles)
        pmtn_set_pid(pmt_n, pmt_pid + 24);
        pmtn_set_desclength(pmt_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = pmtn_get_descs(pmt_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc59(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        pmt_n = pmt_get_es(pmt, pmt_n_counter++);
        pmtn_init(pmt_n);
        pmtn_set_streamtype(pmt_n, 0x06);
        pmtn_set_pid(pmt_n, pmt_pid + 25);
        pmtn_set_desclength(pmt_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = pmtn_get_descs(pmt_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc1e(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc1d(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc1f(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc20(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc21(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc22(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc23(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc27(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc2c(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        pmt_n = pmt_get_es(pmt, pmt_n_counter++);
        pmtn_init(pmt_n);
        pmtn_set_streamtype(pmt_n, 0x1b);
        pmtn_set_pid(pmt_n, pmt_pid + 26);
        pmtn_set_desclength(pmt_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = pmtn_get_descs(pmt_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc24_1(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc24_2(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc24_3(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc25(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc26_1(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc26_2(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc28(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc2a(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc6b(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        pmt_n = pmt_get_es(pmt, pmt_n_counter++);
        pmtn_init(pmt_n);
        pmtn_set_streamtype(pmt_n, 0x0f); // AAC
        pmtn_set_pid(pmt_n, pmt_pid + 27);
        pmtn_set_desclength(pmt_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = pmtn_get_descs(pmt_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

/* DVB  Descriptor 0x42: Stuffing descriptor */
            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc0f(desc);
            desc42_init(desc); /* Every descriptor can be stuffed */

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc2b(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc7c(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        pmt_n = pmt_get_es(pmt, pmt_n_counter++);
        pmtn_init(pmt_n);
        pmtn_set_streamtype(pmt_n, PMT_STREAMTYPE_SCTE_35);
        pmtn_set_pid(pmt_n, scte35_pid);
        pmtn_set_desclength(pmt_n, 0);

        // Set transport_stream_loop length
        pmt_n = pmt_get_es(pmt, pmt_n_counter); // Get last service
        pmt_set_length(pmt, pmt_n - pmt_get_es(pmt, 0) + pmt_get_desclength(pmt));
    }
    psi_set_crc(pmt);
    output_psi_section(pmt, pmt_pid, &cc);

    free(pmt);
}

/* DVB  Discontinuity Information Table (DIT) */
static void generate_dit(void) {
    uint8_t *dit = psi_allocate();

    dit_init(dit);

    dit_set_transition_flag(dit, true);
    output_psi_section(dit, DIT_PID, &cc);

    dit_set_transition_flag(dit, false);
    output_psi_section(dit, DIT_PID, &cc);

    free(dit);
}

/* Selection Information Table (SIT) */
static void generate_sit(void) {
    uint8_t *sit = psi_allocate();
    uint8_t *sit_n;
    uint8_t sit_n_counter;
    uint8_t *desc_loop, *desc;
    uint8_t desc_counter;

    // Generate empty SIT
    sit_init(sit);
    psi_set_version(sit, 0);
    psi_set_current(sit);
    sit_set_length(sit, 0);
    sit_set_desclength(sit, 0);
    psi_set_crc(sit);
    output_psi_section(sit, SIT_PID, &cc);

    // Add elementary streams
    sit_init(sit);
    psi_set_version(sit, 1);
    psi_set_current(sit);
    sit_set_length(sit, 0);
    sit_set_desclength(sit, 0);

    {
        // Add descriptors to program descriptors
        sit_set_length(sit, PSI_MAX_SIZE);
        sit_set_desclength(sit, DESCS_MAX_SIZE);

        desc_counter = 0;
        desc_loop = sit_get_descs(sit);

        desc = descs_get_desc(desc_loop, desc_counter++);
        build_desc63(desc);

        // Finish descriptor generation
        desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
        sit_set_desclength(sit, desc - desc_loop - DESCS_HEADER_SIZE);
        sit_set_length(sit, sit_get_desclength(sit));
    }

    {
        sit_set_length(sit, PSI_MAX_SIZE); // This needed so sit_get_es works

        // Process elementary streams
        sit_n_counter = 0;

        sit_n = sit_get_service(sit, sit_n_counter++);
        sitn_init(sit_n);
        sitn_set_sid(sit_n, sid);
        sitn_set_running(sit_n, 1);
        sitn_set_desclength(sit_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = sitn_get_descs(sit_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc05(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc05(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc05(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        sit_n = sit_get_service(sit, sit_n_counter++);
        sitn_init(sit_n);
        sitn_set_sid(sit_n, sid + 1000);
        sitn_set_running(sit_n, 3);
        sitn_set_desclength(sit_n, 0);

        // Set transport_stream_loop length
        sit_n = sit_get_service(sit, sit_n_counter); // Get last service
        sit_set_length(sit, sit_n - sit_get_service(sit, 0) + sit_get_desclength(sit));
    }

    psi_set_crc(sit);
    output_psi_section(sit, SIT_PID, &cc);

    free(sit);
}

/* SCTE 35 Splice Information Table */
static void generate_scte35(void) {
    uint8_t *scte35 = psi_allocate();

    // Generate empty section
    scte35_init(scte35);
    psi_set_length(scte35, PSI_MAX_SIZE);
    scte35_set_pts_adjustment(scte35, 0);
    scte35_null_init(scte35);
    scte35_set_desclength(scte35, 0);
    psi_set_length(scte35,
            scte35_get_descl(scte35) + PSI_CRC_SIZE - scte35 - PSI_HEADER_SIZE);
    psi_set_crc(scte35);
    output_psi_section(scte35, scte35_pid, &cc);

    // Generate insert section
    scte35_init(scte35);
    psi_set_length(scte35, PSI_MAX_SIZE);
    scte35_set_pts_adjustment(scte35, 0);
    scte35_insert_init(scte35,
            SCTE35_INSERT_HEADER2_SIZE +
            SCTE35_SPLICE_TIME_HEADER_SIZE + SCTE35_SPLICE_TIME_TIME_SIZE +
            SCTE35_BREAK_DURATION_HEADER_SIZE + SCTE35_INSERT_FOOTER_SIZE);
    scte35_insert_set_cancel(scte35, false);
    scte35_insert_set_event_id(scte35, 4242);
    scte35_insert_set_out_of_network(scte35, true);
    scte35_insert_set_program_splice(scte35, true);
    scte35_insert_set_duration(scte35, true);
    scte35_insert_set_splice_immediate(scte35, false);

    uint8_t *splice_time = scte35_insert_get_splice_time(scte35);
    scte35_splice_time_init(splice_time);
    scte35_splice_time_set_time_specified(splice_time, true);
    scte35_splice_time_set_pts_time(splice_time, 270000000);

    uint8_t *duration = scte35_insert_get_break_duration(scte35);
    scte35_break_duration_init(duration);
    scte35_break_duration_set_auto_return(duration, true);
    scte35_break_duration_set_duration(duration, 27000000);

    scte35_insert_set_unique_program_id(scte35, 2424);
    scte35_insert_set_avail_num(scte35, 0);
    scte35_insert_set_avails_expected(scte35, 0);
    scte35_set_desclength(scte35, 0);
    psi_set_length(scte35,
            scte35_get_descl(scte35) + PSI_CRC_SIZE - scte35 - PSI_HEADER_SIZE);
    psi_set_crc(scte35);
    output_psi_section(scte35, scte35_pid, &cc);

    free(scte35);
}

int main(void)
{
    generate_pat();
    generate_cat();
    generate_tsdt();
    generate_nit();
    generate_bat();
    generate_sdt();
    generate_eit();
    generate_tdt();
    generate_tot();
    generate_rst();
    generate_pmt();
    generate_dit();
    generate_sit();
    generate_scte35();

    return EXIT_SUCCESS;
}
