#include "foundation/record/object.hpp"

#include "foundation/core/codec.hpp"

namespace verguenza {

namespace {

constexpr std::string_view kMagic = "VGO1";

void write_unsigned(Writer& w, const SignedObject& o) {
    w.raw(kMagic);
    w.u8(static_cast<std::uint8_t>(o.type));
    w.fixed(o.vault);
    w.u16(static_cast<std::uint16_t>(o.parents.size()));
    for (const Digest& p : o.parents) w.fixed(p);
    w.fixed(o.previous);
    w.i64(o.time);
    w.blob(o.body);
    w.u16(static_cast<std::uint16_t>(o.approvals.size()));
    for (const Bytes& a : o.approvals) w.blob(a);
    w.fixed(o.author);
}

} // namespace

const char* type_name(ObjectType type) {
    switch (type) {
    case ObjectType::roster: return "roster";
    case ObjectType::capability: return "capability";
    case ObjectType::request: return "request";
    case ObjectType::approval: return "approval";
    case ObjectType::act: return "act";
    case ObjectType::retire: return "retire";
    }
    return "unknown";
}

bool known_type(std::uint8_t raw) {
    switch (raw) {
    case 1: case 2: case 4: case 5: case 6: case 7: return true;
    default: return false;
    }
}

Bytes signing_bytes(const SignedObject& o) {
    Writer w;
    w.raw("verguenza/");
    w.raw(type_name(o.type));
    w.raw("/v1");
    w.u8(0);
    write_unsigned(w, o);
    return w.take();
}

Bytes encode(const SignedObject& o) {
    Writer w;
    write_unsigned(w, o);
    w.fixed(o.signature);
    return w.take();
}

Status decode(std::string_view in, SignedObject& out) {
    Reader r(in);
    if (r.raw(4) != kMagic) return Status::fail("not a Verguenza signed object (bad magic or version)");
    SignedObject o;
    const std::uint8_t type = r.u8();
    if (!r.failed() && !known_type(type)) return Status::fail("unknown signed-object type " + std::to_string(type));
    o.type = static_cast<ObjectType>(type);
    o.vault = r.fixed<32>();
    const std::uint16_t parents = r.u16();
    if (parents > kMaxParents) return Status::fail("too many parents");
    for (std::uint16_t i = 0; i < parents && !r.failed(); ++i) o.parents.push_back(r.fixed<32>());
    o.previous = r.fixed<32>();
    o.time = r.i64();
    o.body = Bytes(r.blob(kMaxBody));
    const std::uint16_t approvals = r.u16();
    if (approvals > kMaxApprovals) return Status::fail("too many embedded approvals");
    for (std::uint16_t i = 0; i < approvals && !r.failed(); ++i) o.approvals.push_back(Bytes(r.blob(kMaxBody)));
    o.author = r.fixed<16>();
    o.signature = r.fixed<64>();
    if (r.failed()) return Status::fail("a signed object is truncated");
    if (!r.finish()) return Status::fail("a signed object has trailing bytes");
    out = std::move(o);
    return Status::ok();
}

// private_key: read only, handed to the crypto port to sign.
Status sign(SignedObject& o, const SecretBuffer& private_key, CryptoPort& crypto) {
    if (!crypto.sign(private_key, signing_bytes(o), o.signature)) return Status::fail("signing failed");
    return Status::ok();
}

bool verify(const SignedObject& o, const PublicKey& author_key, CryptoPort& crypto) {
    return crypto.verify(author_key, signing_bytes(o), o.signature);
}

} // namespace verguenza
