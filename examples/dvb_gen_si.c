/*****************************************************************************
 * dvb_gen_si.c: Generate SI tables and descriptors
 *****************************************************************************
 * Copyright (c) 2011 Unix Solutions Ltd.
 *
 * Authors:
 *   Georgi Chorbadzhiyski <gf@unixsol.org>
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

#include <bitstream/mpeg/ts.h>
#include <bitstream/mpeg/psi.h>
#include <bitstream/dvb/si.h>
#include <bitstream/dvb/si_print.h>

uint16_t tsid     = 10000;
uint16_t sid      = 20000;
uint16_t event_id = 30000;
uint16_t onid     = 40000;

uint16_t pmt_pid  = 100;

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
    desc_set_length(desc, lang_n - desc - DESC0A_HEADER_SIZE);
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
    desc_set_length(desc, service_n - desc - DESC41_HEADER_SIZE);
}

/* ---  Descriptor 0x42: Stuffing descriptor */
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

/* ---  Descriptor 0x45: VBI data descriptor */
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
    desc_set_length(desc, lang_n - desc - DESC46_HEADER_SIZE);
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

/* ---  Descriptor 0x49: Country availability descriptor */
/* DVB  Descriptor 0x4a: Linkage descriptor (partially implemented) */
/* ---  Descriptor 0x4b: NVOD_reference_descriptor */
/* ---  Descriptor 0x4c: time_shifted_service_descriptor */
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

/* ---  Descriptor 0x4f: time_shifted_event_descriptor */
/* ---  Descriptor 0x50: component_descriptor */
/* ---  Descriptor 0x51: mosaic_descriptor */
/* DVB  Descriptor 0x52: Stream identifier descriptor */
static void build_desc52(uint8_t *desc) {
    desc52_init(desc);
    desc52_set_component_tag(desc, 46);
}

/* ---  Descriptor 0x53: CA_identifier_descriptor */
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
    desc_set_length(desc, content_n - desc - DESC54_HEADER_SIZE);
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
    desc_set_length(desc, rating_n - desc - DESC55_HEADER_SIZE);
}

/* DVB  Descriptor 0x56: Teletext descriptor */
static void build_desc56(uint8_t *desc) {
    // Desc 46 is the same as desc 56, only
    // the descriptor tag is different
    build_desc46(desc);
    desc56_init(desc);
}
/* ---  Descriptor 0x57: telephone_descriptor */

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
    desc_set_length(desc, lto_n - desc - DESC58_HEADER_SIZE);
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
    desc_set_length(desc, lang_n - desc - DESC59_HEADER_SIZE);
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

/* ---  Descriptor 0x5b: multilingual_network_name_descriptor */
/* ---  Descriptor 0x5c: multilingual_bouquet_name_descriptor */
/* ---  Descriptor 0x5d: multilingual_service_name_descriptor */
/* ---  Descriptor 0x5e: multilingual_component_descriptor */
/* DVB  Descriptor 0x5f: Private data specifier descriptor */
static void build_desc5f(uint8_t *desc) {
    desc5f_init(desc);
    desc5f_set_specifier(desc, 0xaabbccdd);
}

/* ---  Descriptor 0x60: service_move_descriptor */
/* ---  Descriptor 0x61: short_smoothing_buffer_descriptor */
/* ---  Descriptor 0x62: frequency_list_descriptor */
/* ---  Descriptor 0x63: partial_transport_stream_descriptor */
/* ---  Descriptor 0x64: data_broadcast_descriptor */
/* ---  Descriptor 0x65: scrambling_descriptor */
/* ---  Descriptor 0x66: data_broadcast_id_descriptor */
/* ---  Descriptor 0x67: transport_stream_descriptor */
/* ---  Descriptor 0x68: DSNG_descriptor */
/* ---  Descriptor 0x69: PDC_descriptor */
/* DVB  Descriptor 0x6a: AC-3 descriptor */
/* ---  Descriptor 0x6b: ancillary_data_descriptor */
/* ---  Descriptor 0x6c: cell_list_descriptor */
/* ---  Descriptor 0x6d: cell_frequency_link_descriptor */
/* ---  Descriptor 0x6e: announcement_support_descriptor */
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
/* ---  Descriptor 0x7a: enhanced_AC-3_descriptor */
/* ---  Descriptor 0x7b: DTS descriptor */
/* ---  Descriptor 0x7c: AAC descriptor */
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

        write(fileno(stdout), ts, TS_SIZE);
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
            build_desc5f(desc);

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
            build_desc5f(desc);

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
        eitn_set_running_status(eit_n, 2);
        eitn_set_free_CA_mode(eit_n, false);
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
        eitn_set_running_status(eit_n, 1);
        eitn_set_free_CA_mode(eit_n, false);
        eitn_set_desclength(eit_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = eitn_get_descs(eit_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc4e(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc4d(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        eit_n = eit_get_event(eit, eit_n_counter++);
        eitn_set_event_id(eit_n, event_id + 200);
        eitn_set_start_time(eit_n, dvb_time_encode_UTC(ts_2));
        eitn_set_duration_bcd(eit_n, dvb_time_encode_duration(7200));
        eitn_set_running_status(eit_n, 0);
        eitn_set_free_CA_mode(eit_n, false);
        eitn_set_desclength(eit_n, 0);
        {
            // Add descriptors to transport_stream_n
            desc_counter = 0;
            desc_loop = eitn_get_descs(eit_n);
            descs_set_length(desc_loop, DESCS_MAX_SIZE); // This is needed so descs_get_desc(x, n) works

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc54(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc55(desc);

            desc = descs_get_desc(desc_loop, desc_counter++);
            build_desc4d(desc);

            // Finish descriptor generation
            desc = descs_get_desc(desc_loop, desc_counter); // Get next descriptor pos
            descs_set_length(desc_loop, desc - desc_loop - DESCS_HEADER_SIZE);
        }

        eit_n = eit_get_event(eit, eit_n_counter++);
        eitn_set_event_id(eit_n, event_id + 300);
        eitn_set_start_time(eit_n, dvb_time_encode_UTC(ts_2));
        eitn_set_duration_bcd(eit_n, dvb_time_encode_duration(7200));
        eitn_set_running_status(eit_n, 4);
        eitn_set_free_CA_mode(eit_n, true);
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
    rstn_set_running_status(rst_n, 1);

    rst_n = rst_get_status(rst, j++);
    rstn_init(rst_n);
    rstn_set_tsid(rst_n, tsid + 100);
    rstn_set_onid(rst_n, onid + 100);
    rstn_set_service_id(rst_n, sid + 100);
    rstn_set_event_id(rst_n, event_id + 100);
    rstn_set_running_status(rst_n, 2);

    rst_n = rst_get_status(rst, j++);
    rstn_init(rst_n);
    rstn_set_tsid(rst_n, tsid + 200);
    rstn_set_onid(rst_n, onid + 200);
    rstn_set_service_id(rst_n, sid + 200);
    rstn_set_event_id(rst_n, event_id + 200);
    rstn_set_running_status(rst_n, 3);

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

int main(void)
{
    generate_pat();
    generate_cat();
    generate_nit();
    generate_bat();
    generate_sdt();
    generate_eit();
    generate_tdt();
    generate_tot();
    generate_rst();
    generate_pmt();
    generate_dit();

    return EXIT_SUCCESS;
}