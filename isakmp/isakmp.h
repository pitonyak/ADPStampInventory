#pragma once

#include <cstddef>
#include <cstdint>

/**
 * @brief Payload types for ISAKMP packets (IKEv1 and IKEv2).
 */
enum class isakmp_payload_type : uint8_t
{
    None = 0,
    IKE_SecurityAssociation = 1,
    IKE_Proposal = 2,
    IKE_Transform = 3,
    IKE_KeyExchange = 4,
    IKE_Identification = 5,
    IKE_Certificate = 6,
    IKE_CertificateRequest = 7,
    IKE_Hash = 8,
    IKE_Signature = 9,
    IKE_Nonce = 10,
    IKE_Notification = 11,
    IKE_Delete = 12,
    IKE_VendorId = 13,
    IKE2_SecurityAssociation = 33,
    IKE2_KeyEnchange = 34,
    IKE2_IdentificaionInitiator = 35,
    IKE2_IdentificationResponder = 36,
    IKE2_Certifcate = 37,
    IKE2_CertifcateRequest = 38,
    IKE2_Authentication = 39,
    IKE2_Nonce = 40,
    IKE2_Notify = 41,
    IKE2_Delete = 42,
    IKE2_VendorId = 43,
    IKE2_TrafficSelectorInitiator = 44,
    IKE2_TrafficSelectorResponder = 45,
    IKE2_Encrypted = 46,
    IKE2_Configuration = 47,
    IKE2_ExtensibleAuthentication = 48
};

/// Length in bytes of ISAKMP cookies/SPIs (initiator or responder).
constexpr size_t isakmp_cookie_len = 8;

/**
 * @brief Structure describing the header of an ISAKMP packet.
 */
struct isakmp_header
{
    uint8_t initiator_cookie[isakmp_cookie_len];
    uint8_t responder_cookie[isakmp_cookie_len];
    isakmp_payload_type next_payload;
    uint8_t version;
    uint8_t exchange_type;
    uint8_t flags;
    uint32_t message_id;
    uint32_t length;
};

/**
 * @brief Structure describing the generic header applicable to all ISAKMP payloads.
 */
struct isakmp_generic_payload_header
{
    isakmp_payload_type next_payload;
    uint8_t reserved;
    uint16_t length;
};

/**
 * @brief ISAKMP flag constants.
 */
struct isakmp_flags
{
    static constexpr uint8_t IKE_Encrypt = (1 << 0);
    static constexpr uint8_t IKE_Commit = (1 << 1);
    static constexpr uint8_t IKE_Authentication = (1 << 2);
    static constexpr uint8_t IKE2_Initiator = (1 << 3);
    static constexpr uint8_t IKE2_Version = (1 << 4);
    static constexpr uint8_t IKE2_Response = (1 << 5);
};
