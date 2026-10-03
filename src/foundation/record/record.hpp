/* foundation/record/record.hpp — a vault's signed record: every signed object and
 * envelope, by address, and the verdict on each.
 *
 * okf/concepts/trust.md. What is built here, and what is not yet:
 *
 *   BUILT  objects are content-addressed and checked on arrival (hash, frame,
 *          signature); an object whose parents have not all arrived is HELD, not judged,
 *          and judged when they come; rule 1, judging by causal past, for MEMBERSHIP:
 *          an object counts only if its author's device was admitted by a valid roster
 *          entry among its own ancestors; the current value of a secret is the highest
 *          valid version among its envelopes, whatever the document says.
 *   NOT YET (phase 1A, the access model; phase 3, several devices): capabilities in the
 *          verdict, rule 2 (cutoffs), rule 3 (removals all take effect), rule 4 (authority
 *          only through valid objects). With one founding device there is nothing for them
 *          to decide yet; the structure they need (ancestry per object) is here.
 *
 * The record only grows: nothing is ever removed from it. */
#pragma once

#include "foundation/core/bytes.hpp"
#include "foundation/core/status.hpp"
#include "foundation/ports/crypto_port.hpp"
#include "foundation/record/fields.hpp"
#include "foundation/record/object.hpp"
#include "foundation/sealing/envelope.hpp"

#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace verguenza {

/* Field tags inside roster bodies. */
namespace roster_field {
constexpr std::uint8_t kind = 1;        // "found"; later "admit-device", "admit-person", "remove"
constexpr std::uint8_t vault_name = 2;
constexpr std::uint8_t device_key = 3;  // the admitted device's public key
constexpr std::uint8_t device_name = 4;
constexpr std::uint8_t person_id = 5;
constexpr std::uint8_t person_name = 6;
} // namespace roster_field

/* Field tags inside act and retire bodies. */
namespace act_field {
constexpr std::uint8_t secret = 1;
constexpr std::uint8_t how = 2;     // "show", "copy", "qr", "use", "export"
constexpr std::uint8_t purpose = 3; // a client's stated purpose (phase 2)
} // namespace act_field

struct DeviceInfo {
    DeviceId id{};
    PublicKey key{};
    std::string name;
    Id16 person{};
    std::string person_name;
};

struct ActInfo {
    Digest address{};
    std::int64_t time = 0;
    DeviceId author{};
    Id16 secret{};
    std::string how;
};

class Record {
public:
    explicit Record(CryptoPort& crypto) : crypto_(&crypto) {}

    /* A new vault: the founding roster entry, signed by this device.
     * device_key: read only, to sign. */
    static Status found(const std::string& vault_name, const std::string& person_name, const std::string& device_name,
                        const SecretBuffer& device_key, std::int64_t time, CryptoPort& crypto, Record& out);

    /* Add encoded bytes (a signed object or an envelope; told apart by magic). Checked on
     * arrival; held if its parents are missing. `address` receives its address. */
    Status add(std::string_view bytes, Digest* address = nullptr);

    /* Write, sign and add an object as this device: parents are the current heads,
     * previous is this device's last object. device_key: read only, to sign. */
    Status append(ObjectType type, const Fields& body, std::int64_t time, const DeviceId& me,
                  const SecretBuffer& device_key, Digest* address = nullptr);

    /* What a new object written now would name. */
    std::vector<Digest> heads() const;
    Digest last_of(const DeviceId& device) const;

    // ── reading the record ──────────────────────────────────────────────────
    bool founded() const { return !is_zero(vault_id_); }
    const Digest& vault_id() const { return vault_id_; }
    const std::string& vault_name() const { return vault_name_; }
    std::optional<DeviceInfo> device(const DeviceId& id) const;
    std::vector<DeviceInfo> devices() const;

    /* The highest valid version of a secret's value, if it has one and is not retired.
     * `conflict` is set when two valid envelopes share that version (written
     * concurrently): a Palabra-style conflict for people to settle. */
    std::optional<Envelope> current_envelope(const Id16& secret, bool* conflict = nullptr) const;
    bool retired(const Id16& secret) const;
    std::vector<ActInfo> acts() const; // oldest first, by the writer's clock (display only)

    /* Everything, encoded, for saving: valid and held alike (the record only grows). */
    std::map<Digest, Bytes> all_bytes() const;
    std::size_t held_count() const { return pending_.size(); }
    std::size_t size() const { return entries_.size(); }

private:
    struct Entry {
        bool is_envelope = false;
        Bytes bytes;
        SignedObject object;
        Envelope envelope;
        std::vector<Digest> ancestry_links; // parents ∪ {previous}
        DeviceId author{};
    };

    Status admit(const Digest& address, Entry entry);
    Status judge(const Digest& address, const Entry& entry) const;
    std::set<Digest> ancestors(const std::vector<Digest>& links) const;
    void retry_held();

    CryptoPort* crypto_;
    Digest vault_id_{};
    std::string vault_name_;
    std::map<Digest, Entry> entries_;  // valid entries only
    std::map<Digest, Entry> pending_;  // held: some parent has not arrived
    std::map<Digest, Bytes> refused_;  // kept as evidence, given no authority
    std::map<DeviceId, DeviceInfo> devices_;
};

} // namespace verguenza
