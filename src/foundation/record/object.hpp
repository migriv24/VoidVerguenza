/* foundation/record/object.hpp — a signed object: the unit of a vault's record.
 *
 * okf/concepts/formats.md §"Signed objects", and okf/concepts/trust.md for why authority
 * lives in these rather than in the merged document. The frame:
 *
 *   "VGO1" ‖ type (u8) ‖ vault id (32) ‖ parents (u16 + 32n) ‖ previous (32)
 *          ‖ time (i64, unix ms) ‖ body (u32 + bytes) ‖ approvals (u16 + n × (u32 + bytes))
 *          ‖ author device id (16) ‖ signature (64)
 *
 * The signature covers "verguenza/<type>/v1" 0x00 followed by every byte before it,
 * so a signature made for one type never verifies as another, nor as a Reticulum
 * announce (whose signed bytes never begin with "verguenza/").
 *
 * `time` is the writer's clock, for people; order comes from `parents` alone. */
#pragma once

#include "foundation/core/bytes.hpp"
#include "foundation/core/status.hpp"
#include "foundation/ports/crypto_port.hpp"

#include <cstdint>
#include <string_view>
#include <vector>

namespace verguenza {

enum class ObjectType : std::uint8_t {
    roster = 1,     // founding a vault; admitting a device or a person (phase 3)
    capability = 2, // a grant (phase 1A, authority)
    request = 4,    // asking for consent (phase 4)
    approval = 5,   // giving it (phase 4)
    act = 6,        // a reveal, a copy, a use: the audit record
    retire = 7,     // a secret removed from view (so the merge cannot undo it)
};
/* The envelope's own frame is in sealing/envelope.hpp; its type name is "envelope". */

const char* type_name(ObjectType type);
bool known_type(std::uint8_t raw);

struct SignedObject {
    ObjectType type = ObjectType::act;
    Digest vault{};                // all zeros only in the founding entry
    std::vector<Digest> parents;   // the objects this one was written after
    Digest previous{};             // the author's previous object; zeros for its first
    std::int64_t time = 0;         // unix ms, for display only
    Bytes body;                    // Fields-encoded, public bytes only
    std::vector<Bytes> approvals;  // embedded approvals (phase 4)
    DeviceId author{};
    Signature signature{};
};

/* The bytes that are signed: the type prefix, then the frame up to the author id. */
Bytes signing_bytes(const SignedObject& o);
Bytes encode(const SignedObject& o);
Status decode(std::string_view in, SignedObject& out);

/* private_key: read only, to sign. Sets `o.signature`. */
Status sign(SignedObject& o, const SecretBuffer& private_key, CryptoPort& crypto);
bool verify(const SignedObject& o, const PublicKey& author_key, CryptoPort& crypto);

/* Readers refuse past these (formats.md §"Readers refuse"). */
constexpr std::size_t kMaxBody = 64u << 20;
constexpr std::size_t kMaxParents = 1024;
constexpr std::size_t kMaxApprovals = 256;

} // namespace verguenza
