/* foundation/sealing/envelope.hpp — a secret's value, sealed to the devices that may
 * open it. okf/concepts/sealing.md and okf/concepts/formats.md §"Envelope v1":
 *
 *   value ──► K = random(64)
 *         ──► body = Token(K).encrypt( SHA-256(header) ‖ value )
 *         ──► per device D:  seal_D = encrypt_to(D, K)
 *         ──► sig = sign("verguenza/envelope/v1" 0x00 ‖ everything before the signature)
 *
 * The header digest inside the plaintext stands in for associated data, which
 * Reticulum's token does not have: a body moved under another header (another vault,
 * secret, version or policy) opens to a digest that does not match and is refused.
 *
 * Re-sealing to a new device keeps the body and adds a seal: a new envelope, a new
 * address and signature, the same version (formats.md). */
#pragma once

#include "foundation/core/bytes.hpp"
#include "foundation/core/status.hpp"
#include "foundation/ports/crypto_port.hpp"
#include "foundation/secret/secret_buffer.hpp"

#include <cstdint>
#include <string_view>
#include <vector>

namespace verguenza {

struct EnvelopeHeader {
    Digest vault{};
    Id16 secret{};
    std::uint32_t version = 1;
    Id16 policy{};
};

enum class SealKind : std::uint8_t { device = 1, cold_holder = 2, share = 3 };

struct Seal {
    SealKind kind = SealKind::device;
    DeviceId recipient{};
    Bytes sealed; // ciphertext of the value key
};

struct Envelope {
    EnvelopeHeader header;
    std::vector<Digest> parents;
    Digest previous{};
    Bytes body; // ciphertext
    std::vector<Seal> seals;
    DeviceId author{};
    Signature signature{};
};

struct Recipient {
    DeviceId id{};
    PublicKey key{};
};

Bytes encode_header(const EnvelopeHeader& h);
Bytes signing_bytes(const Envelope& e);
Bytes encode(const Envelope& e);
Status decode(std::string_view in, Envelope& out);
bool verify(const Envelope& e, const PublicKey& author_key, CryptoPort& crypto);

/* Seal a value to `recipients` and sign it as `author`.
 * value: plaintext, read only; copied into a locked buffer with the header digest,
 *        encrypted, and that buffer wiped.
 * author_key: read only, to sign. */
Status seal_value(const EnvelopeHeader& header, const SecretBuffer& value, const std::vector<Recipient>& recipients,
                  const std::vector<Digest>& parents, const Digest& previous, const DeviceId& author,
                  const SecretBuffer& author_key, CryptoPort& crypto, Envelope& out);

/* Open the value with this device's key.
 * my_key: read only. out: receives the plaintext value. The value key and the
 *         decrypted body (digest ‖ value) live in SecretBuffers and are wiped here. */
Status open_value(const Envelope& e, const DeviceId& me, const SecretBuffer& my_key, CryptoPort& crypto,
                  SecretBuffer& out);

/* The same body, sealed additionally to `extra`, signed by a device that can open it.
 * my_key: read only, to open the value key and to sign; the value itself is never
 *         decrypted. */
Status reseal(const Envelope& e, const DeviceId& me, const SecretBuffer& my_key, const std::vector<Recipient>& extra,
              const std::vector<Digest>& parents, const Digest& previous, CryptoPort& crypto, Envelope& out);

constexpr std::size_t kMaxSeals = 4096;

} // namespace verguenza
