/*****************************************************************************
 * ait_print.h: ETSI TS 102 809 Application Information Table (AIT) (printing)
 *****************************************************************************
 * Copyright (C) 2024 EasyTools
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

#ifndef __BITSTREAM_DVB_AIT_PRINT_H__
#define __BITSTREAM_DVB_AIT_PRINT_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/psi.h>
#include <bitstream/mpeg/psi/descs_print.h>
#include <bitstream/dvb/si/ait.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Application Information Table descriptors
 *****************************************************************************/
static inline void ait_desc_app_print(const uint8_t *p_desc,
                                      f_print pf_print, void *print_opaque,
                                      f_iconv pf_iconv, void *iconv_opaque,
                                      print_type_t i_print_type)
{
    switch (i_print_type) {
        case PRINT_XML:
            pf_print(print_opaque, "<APP service_bound=\"%s\" "
                     "visibilty=\"%u\" priority=\"%u\">",
                     ait_desc_app_get_service_bound(p_desc) ? "true" : "false",
                     ait_desc_app_get_visibily(p_desc),
                     ait_desc_app_get_priority(p_desc));
            break;
        default:
            pf_print(print_opaque, "    - app service_bound=%s visibilty=%u "
                     "priority=%u",
                     ait_desc_app_get_service_bound(p_desc) ? "true" : "false",
                     ait_desc_app_get_visibily(p_desc),
                     ait_desc_app_get_priority(p_desc));
    }

    ait_desc_app_each_profile(p_desc, p_profile) {
        switch (i_print_type) {
            case PRINT_XML:
                pf_print(print_opaque, "<PROFILE type=\"%u\" "
                         "major=\"%u\" minor=\"%u\" micro=\"%u\" />",
                         ait_desc_app_profile_get_type(p_profile),
                         ait_desc_app_profile_get_major(p_profile),
                         ait_desc_app_profile_get_minor(p_profile),
                         ait_desc_app_profile_get_micro(p_profile));
                break;

            default:
                pf_print(print_opaque, "      - profile type=%u "
                         "version=%u.%u.%u",
                         ait_desc_app_profile_get_type(p_profile),
                         ait_desc_app_profile_get_major(p_profile),
                         ait_desc_app_profile_get_minor(p_profile),
                         ait_desc_app_profile_get_micro(p_profile));

        }
    }

    ait_desc_app_each_transport_proto(p_desc, p_proto) {
        switch (i_print_type) {
            case PRINT_XML:
                pf_print(print_opaque, "<TRANSPORT_PROTOCOL value=\"%u\"/>",
                         *p_proto);
                break;

            default:
                pf_print(print_opaque, "      - transport_protocol %u",
                         *p_proto);
        }
    }
}

static inline void ait_desc_app_name_print(const uint8_t *p_desc,
                                           f_print pf_print, void *print_opaque,
                                           f_iconv pf_iconv, void *iconv_opaque,
                                           print_type_t i_print_type)
{
    switch (i_print_type) {
        case PRINT_XML:
            pf_print(print_opaque, "<APP_NAME>");
            break;
        default:
            pf_print(print_opaque, "    - app_name");
    }

    ait_desc_app_name_each_name(p_desc, p_name) {
        uint8_t *code = ait_desc_app_name_get_code(p_name);
        uint8_t text_len;
        const uint8_t *text = ait_desc_app_name_get_text(p_name, &text_len);
        char *psz_text = dvb_string_get(text, text_len, pf_iconv, iconv_opaque);

        switch (i_print_type) {
            case PRINT_XML:
                pf_print(print_opaque, "<NAME language=\"%3.3s\" test=\"%s\"/>",
                         (char *)code, psz_text);
                break;

            default:
                pf_print(print_opaque, "      - language=%3.3s test=\"%s\"",
                         (char *)code, psz_text);
        }

        free(psz_text);
    }
}

static inline void ait_desc_transport_proto_print(const uint8_t *p_desc,
                                                  f_print pf_print,
                                                  void *print_opaque,
                                                  f_iconv pf_iconv,
                                                  void *iconv_opaque,
                                                  print_type_t i_print_type)
{
    switch (i_print_type) {
        case PRINT_XML:
            pf_print(print_opaque, "<TRANSPORT_PROTOCOL id=\"%u\" label=\"%u\""
                     ">",
                     ait_desc_transport_proto_get_id(p_desc),
                     ait_desc_transport_proto_get_label(p_desc));
            break;
        default:
            pf_print(print_opaque, "    - transport_protocol id=%u label=%u",
                     ait_desc_transport_proto_get_id(p_desc),
                     ait_desc_transport_proto_get_label(p_desc));
    }
}

static inline void ait_desc_app_location_print(const uint8_t *p_desc,
                                               f_print pf_print,
                                               void *print_opaque,
                                               f_iconv pf_iconv,
                                               void *iconv_opaque,
                                               print_type_t i_print_type)
{
    uint8_t initial_path_len;
    uint8_t *initial_path =
        ait_desc_app_location_get_initial_path(p_desc, &initial_path_len);
    switch (i_print_type) {
        case PRINT_XML:
            pf_print(print_opaque, "<TRANSPORT_PROTOCOL id=\"%u\" label=\"%u\""
                     "initial_path=\"%.*s\"/>",
                     ait_desc_transport_proto_get_id(p_desc),
                     ait_desc_transport_proto_get_label(p_desc),
                     (int)initial_path_len, (char *)initial_path);
            break;
        default:
            pf_print(print_opaque, "    - transport_protocol id=%u label=%u "
                     "initial_path=\"%.*s\"",
                     ait_desc_transport_proto_get_id(p_desc),
                     ait_desc_transport_proto_get_label(p_desc),
                     (int)initial_path_len, (char *)initial_path);
    }
}

/*****************************************************************************
 * Application Information Table
 *****************************************************************************/
static inline void ait_table_print(uint8_t **pp_sections,
                                   f_print pf_print, void *print_opaque,
                                   f_iconv pf_iconv, void *iconv_opaque,
                                   print_type_t i_print_type)
{
    uint8_t i_last_section = psi_table_get_lastsection(pp_sections);
    uint8_t i;

    uint16_t idext = psi_table_get_tableidext(pp_sections);
    uint8_t test = idext & 0x8000 ? 1 : 0;
    uint16_t type = idext & 0x7fff;

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque, "<AIT test=\"%s\" type=\"%hu\" version=\"%hhu\" "
                 "current_next=\"%d\">",
                 test ? "true" : "false",
                 type,
                 psi_table_get_version(pp_sections),
                 !psi_table_get_current(pp_sections) ? 0 : 1);
        break;
    default:
        pf_print(print_opaque, "new AIT %stype=%hu version=%hhu%s",
                 test ? "test " : "",
                 type,
                 psi_table_get_version(pp_sections),
                 !psi_table_get_current(pp_sections) ? " (next)" : "");
    }

    descs_print(ait_get_descs(psi_table_get_section(pp_sections, 0)),
                pf_print, print_opaque, pf_iconv, iconv_opaque, i_print_type);

    for (i = 0; i <= i_last_section; i++) {
        uint8_t *p_section = psi_table_get_section(pp_sections, i);

        ait_each_app(p_section, p_app) {
            switch (i_print_type) {
                case PRINT_XML:
                    break;
                default:
                    pf_print(print_opaque, "  * app organisation=%u id=%hu "
                             "control_code=%s",
                             aitn_get_organisation_id(p_app),
                             aitn_get_application_id(p_app),
                             aitn_control_code_txt(aitn_get_control_code(p_app))
                             );
                    break;
            }

            aitn_each_desc(p_app, p_desc) {
                uint8_t tag = desc_get_tag(p_desc);

                switch (tag) {
#define CASE_DESC(Value, Name) \
                case AIT_DESC_##Value: \
                    if (ait_desc_##Name##_validate(p_desc)) \
                        ait_desc_##Name##_print(p_desc, \
                                                pf_print, print_opaque, \
                                                pf_iconv, iconv_opaque, \
                                                i_print_type); \
                    else \
                        desc_print_error(p_desc, pf_print, print_opaque, \
                                         i_print_type); \
                    break;

                CASE_DESC(APP, app)
                CASE_DESC(APP_NAME, app_name)
                CASE_DESC(TRANSPORT_PROTO, transport_proto)
                CASE_DESC(APP_LOCATION, app_location)
#undef CASE_DESC

                default:
                    desc_print(p_desc, pf_print, print_opaque, i_print_type);
                    break;
                }
            }
        }
    }

    switch (i_print_type) {
    case PRINT_XML:
        pf_print(print_opaque, "</AIT>");
        break;
    default:
        pf_print(print_opaque, "end AIT");
    }
}

#ifdef __cplusplus
}
#endif

#endif
