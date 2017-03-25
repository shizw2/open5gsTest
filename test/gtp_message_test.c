#include "core_pkbuf.h"
#include "core_lib.h"
#include "core_debug.h"

#include "3gpp_defs.h"
#include "3gpp_conv.h"
#include "gtp_types.h"
#include "gtp_tlv.h"

#include "testutil.h"

static void gtp_message_test1(abts_case *tc, void *data)
{
    status_t rv;
    char *_payload = 
        "0100080055153011 340010f44c000600 9471527600414b00 0800536120009178"
        "840056000d001855 f501102255f50100 019d015300030055 f501520001000657"
        "0009008a80000084 0a32360a57000901 87000000000a3236 254700220005766f"
        "6c7465036e673204 6d6e6574066d6e63 303130066d636335 3535046770727380"
        "000100fc63000100 014f000500010000 00007f0001000048 000800000003e800"
        "0007d04e001a0080 8021100100001081 0600000000830600 000000000d00000a"
        "005d001f00490001 0005500016004505 0000000000000000 0000000000000000"
        "0000000072000200 40005f0002005400";
    char *_value = NULL;
    gtp_create_session_request_t req;
    c_uint8_t tmp[256];
    pkbuf_t *pkbuf = NULL;
    char hexbuf[MAX_SDU_LEN];

    pkbuf = pkbuf_alloc(0, 240);
    ABTS_PTR_NOTNULL(tc, pkbuf);
    memcpy(pkbuf->payload, 
        CORE_HEX(_payload, strlen(_payload), hexbuf), pkbuf->len);

    memset(&req, 0, sizeof(req));
    rv = tlv_parse_msg(&req, &tlv_desc_create_session_request, pkbuf,
            TLV_MODE_T1_L2_I1);
    ABTS_INT_EQUAL(tc, CORE_OK, rv);

    pkbuf_free(pkbuf);

    ABTS_INT_EQUAL(tc, 1, req.imsi.presence);
    ABTS_INT_EQUAL(tc, 8, req.imsi.len);
    _value = "55153011 340010f4";
    ABTS_TRUE(tc, memcmp(CORE_HEX(_value, strlen(_value), tmp), 
                req.imsi.data, req.imsi.len) == 0);

    ABTS_INT_EQUAL(tc, 1, req.msisdn.presence);
    ABTS_INT_EQUAL(tc, 6, req.msisdn.len);
    _value = "94715276 0041";
    ABTS_TRUE(tc, memcmp(CORE_HEX(_value, strlen(_value), tmp), 
                req.msisdn.data, req.msisdn.len) == 0);

    ABTS_INT_EQUAL(tc, 1, req.me_identity.presence);
    ABTS_INT_EQUAL(tc, 8, req.me_identity.len);
    _value = "53612000 91788400";
    ABTS_TRUE(tc, memcmp(CORE_HEX(_value, strlen(_value), tmp),
        req.me_identity.data, req.me_identity.len) == 0);

    ABTS_INT_EQUAL(tc, 1, req.user_location_information.presence);
    ABTS_INT_EQUAL(tc, 1, req.serving_network.presence);
    ABTS_INT_EQUAL(tc, 1, req.rat_type.presence);
    ABTS_INT_EQUAL(tc, 0, req.indication_flags.presence);
    ABTS_INT_EQUAL(tc, 1, req.sender_f_teid_for_control_plane.presence);
    ABTS_INT_EQUAL(tc, 1, req.pgw_s5_s8_address_for_control_plane_or_pmip.
            presence);
    ABTS_INT_EQUAL(tc, 1, req.access_point_name.presence);
    ABTS_INT_EQUAL(tc, 1, req.selection_mode.presence);
    ABTS_INT_EQUAL(tc, 1, req.pdn_type.presence);
    ABTS_INT_EQUAL(tc, 1, req.pdn_address_allocation.presence);
    ABTS_INT_EQUAL(tc, 1, req.maximum_apn_restriction.presence);
    ABTS_INT_EQUAL(tc, 1, req.aggregate_maximum_bit_rate.presence);
    ABTS_INT_EQUAL(tc, 0, req.linked_eps_bearer_id.presence);
    ABTS_INT_EQUAL(tc, 0, req.trusted_wlan_mode_indication.presence);
    ABTS_INT_EQUAL(tc, 1, req.protocol_configuration_options.presence);
    ABTS_INT_EQUAL(tc, 1, req.bearer_contexts_to_be_created.presence);
    ABTS_INT_EQUAL(tc, 1, req.
                bearer_contexts_to_be_created.eps_bearer_id.presence);
    ABTS_INT_EQUAL(tc, 0x05, 
            req.bearer_contexts_to_be_created.eps_bearer_id.u8);
    ABTS_INT_EQUAL(tc, 0, req.
                bearer_contexts_to_be_created.tft.presence);
    ABTS_INT_EQUAL(tc, 0, req.
                bearer_contexts_to_be_created.s1_u_enodeb_f_teid.presence);
    ABTS_INT_EQUAL(tc, 0, req.
                bearer_contexts_to_be_created.s2b_u_epdg_f_teid_5.presence);
    ABTS_INT_EQUAL(tc, 0, req.
                bearer_contexts_to_be_created.s2a_u_twan_f_teid_6.presence);
    ABTS_INT_EQUAL(tc, 1, req.
                bearer_contexts_to_be_created.bearer_level_qos.presence);
    ABTS_INT_EQUAL(tc, 22,
            req.bearer_contexts_to_be_created.bearer_level_qos.len);
    _value = "45050000 00000000 00000000 00000000 00000000 0000";
    ABTS_TRUE(tc, memcmp(CORE_HEX(_value, strlen(_value), tmp),
        req.bearer_contexts_to_be_created.bearer_level_qos.data,
        req.bearer_contexts_to_be_created.bearer_level_qos.len) 
            == 0);
    ABTS_INT_EQUAL(tc, 0, req.
                bearer_contexts_to_be_created.s11_u_mme_f_teid.presence);
    ABTS_INT_EQUAL(tc, 0, req.
                bearer_contexts_to_be_created.cause.presence);
    ABTS_INT_EQUAL(tc, 0, req.
                bearer_contexts_to_be_created.transaction_identifier.presence);
    ABTS_INT_EQUAL(tc, 0, req.
                bearer_contexts_to_be_created.packet_flow_id.presence);
    ABTS_INT_EQUAL(tc, 0, req.bearer_contexts_to_be_removed.presence);
    ABTS_INT_EQUAL(tc, 0, req.recovery.presence);
    ABTS_INT_EQUAL(tc, 0, req.mme_fq_csid.presence);
    ABTS_INT_EQUAL(tc, 0, req.sgw_fq_csid.presence);
    ABTS_INT_EQUAL(tc, 0, req.epdg_fq_csid.presence);
    ABTS_INT_EQUAL(tc, 0, req.twan_fq_csid.presence);
    ABTS_INT_EQUAL(tc, 1, req.ue_time_zone.presence);
    ABTS_INT_EQUAL(tc, 0, req.user_csg_information.presence);
    ABTS_INT_EQUAL(tc, 1, req.charging_characteristics.presence);
    ABTS_INT_EQUAL(tc, 0, req.mme_s4_sgsn_ldn.presence);
    ABTS_INT_EQUAL(tc, 0, req.sgw_ldn.presence);
    ABTS_INT_EQUAL(tc, 0, req.epdg_ldn.presence);
    ABTS_INT_EQUAL(tc, 0, req.mo_exception_data_counter.presence);
    ABTS_INT_EQUAL(tc, 0, req.ue_tcp_port.presence);
}

static void gtp_message_test2(abts_case *tc, void *data)
{
    status_t rv;
    pkbuf_t *pkbuf = NULL;
    gtp_create_session_request_t req;
    gtp_uli_t uli;
    c_uint8_t buffer[20];
    c_uint16_t size = 0;

    memset(&req, 0, sizeof(gtp_create_session_request_t));

    req.imsi.presence = 1;
    req.imsi.len = 8;
    req.imsi.data = (c_uint8_t *)"\x55\x15\x30\x11\x34\x00\x10\xf4";

    req.msisdn.presence = 1;
    req.msisdn.len = 6;
    req.msisdn.data = (c_uint8_t *)"\x94\x71\x52\x76\x00\x41";

    req.me_identity.presence = 1;
    req.me_identity.len = 8;
    req.me_identity.data = (c_uint8_t *)"\x53\x61\x20\x00\x91\x78\x84\x00";

    req.user_location_information.presence = 1;
    memset(&uli, 0, sizeof(gtp_uli_t));
    uli.flags.ecgi = 1;
    uli.flags.tai = 1;
    plmn_id_build(&uli.tai.plmn_id, 555, 10, 2);
    uli.tai.tac = 4130;
    plmn_id_build(&uli.ecgi.plmn_id, 555, 10, 2);
    uli.ecgi.eci = 105729;
    req.user_location_information.data = buffer;
    size = gtp_encode_uli(&req.user_location_information, &uli);
    ABTS_INT_EQUAL(tc, 13, req.user_location_information.len);
    memset(&uli, 0, sizeof(gtp_uli_t));
    size = gtp_decode_uli(&uli, &req.user_location_information);
    ABTS_INT_EQUAL(tc, 13, size);
    ABTS_INT_EQUAL(tc, 0, uli.flags.lai);
    ABTS_INT_EQUAL(tc, 1, uli.flags.ecgi);
    ABTS_INT_EQUAL(tc, 105729, uli.ecgi.eci);
    ABTS_INT_EQUAL(tc, 1, uli.flags.tai);
    ABTS_INT_EQUAL(tc, 4130, uli.tai.tac);
    ABTS_INT_EQUAL(tc, 0, uli.flags.rai);
    ABTS_INT_EQUAL(tc, 0, uli.flags.sai);
    ABTS_INT_EQUAL(tc, 0, uli.flags.cgi);

    rv = tlv_build_msg(&pkbuf, &tlv_desc_create_session_request, &req,
            TLV_MODE_T1_L2_I1);

    ABTS_INT_EQUAL(tc, CORE_OK, rv);

    pkbuf_free(pkbuf);
#if 0
    req.serving_network.presence = 1;
    req.rat_type.presence = 1;
    req.sender_f_teid_for_control_plane.presence = 1;
    req.pgw_s5_s8_address_for_control_plane_or_pmip = 1;
    
    req.access_point_name.presence = 1;
    req.selection_mode.presence = 1;
    req.pdn_type.presence = 1;
    req.pdn_address_allocation.presence = 1;
    req.maximum_apn_restriction.presence = 1;
    req.aggregate_maximum_bit_rate.presence 1;
    req.protocol_configuration_options.presence = 1;
    req.bearer_contexts_to_be_created.presence = 1;
    req.contexts_to_be_created.eps_bearer_id.presence = 1;
    req.contexts_to_be_created.bearer_level_qos.presence = 1;
    req.bearer_contexts_to_be_created.bearer_level_qos.len = 22;
#endif
}

abts_suite *test_gtp_message(abts_suite *suite)
{
    suite = ADD_SUITE(suite)

    abts_run_test(suite, gtp_message_test1, NULL);
    abts_run_test(suite, gtp_message_test2, NULL);

    return suite;
}
