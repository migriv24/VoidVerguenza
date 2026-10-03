#include "foundation/sealing/envelope.hpp"

#include "foundation/core/codec.hpp"
#include "foundation/record/object.hpp"

#include <cstring>

namespace verguenza {

namespace {

constexpr std::string_view kMagic = "VGE1";
constexpr std::size_t kSealCeiling = 4096;

void write_header(Writer& w, const EnvelopeHeader& h) {
    w.fixed(h.vault);
    w.fixed(h.secret);
    w.u32(h.version);
    w.fixed(h.policy);
}

void write_unsigned(Writer& w, const Envelope& e) {
    w.raw(kMagic);
    write_header(w, e.header);
    w.u16(static_cast<std::uint16_t>(e.parents.size()));
    for (const Digest& p : e.parents) w.fixed(p);
    w.fixed(e.previous);
    w.blob(e.body);
    w.u16(static_cast<std::uint16_t>(e.seals.size()));
    for (const Seal& s : e.seals) {
        w.u8(static_cast<std::uint8_t>(s.kind));
        w.fixed(s.recipient);
        w.u16(static_cast<std::uint16_t>(s.sealed.size()));
        w.raw(s.sealed);
    }
    w.fixed(e.author);
}

// my_key: read only. key_out: receives the value key (plaintext key material).
Status open_value_key(const Envelope& e, const DeviceId& me, const SecretBuffer& my_key, CryptoPort& crypto,
                      SecretBuffer& key_out) {
    for (const Seal& s : e.seals) {
        if (s.kind != SealKind::device || s.recipient != me) continue;
        if (!crypto.open(my_key, s.sealed, key_out) || key_out.size() != kValueKeySize)
            return Status::fail("this device's seal on the envelope did not open (wrong key, or changed bytes)");
        return Status::ok();
    }
    return Status::fail("the envelope is not sealed to this device");
}

// author_key: read only, to sign.
Status finish_signed(Envelope& e, const SecretBuffer& author_key, CryptoPort& crypto) {
    if (!crypto.sign(author_key, signing_bytes(e), e.signature)) return Status::fail("signing the envelope failed");
    return Status::ok();
}

} // namespace

Bytes encode_header(const EnvelopeHeader& h) {
    Writer w;
    write_header(w, h);
    return w.take();
}

Bytes signing_bytes(const Envelope& e) {
    Writer w;
    w.raw("verguenza/envelope/v1");
    w.u8(0);
    write_unsigned(w, e);
    return w.take();
}

Bytes encode(const Envelope& e) {
    Writer w;
    write_unsigned(w, e);
    w.fixed(e.signature);
    return w.take();
}

Status decode(std::string_view in, Envelope& out) {
    Reader r(in);
    if (r.raw(4) != kMagic) return Status::fail("not a Verguenza envelope (bad magic or version)");
    Envelope e;
    e.header.vault = r.fixed<32>();
    e.header.secret = r.fixed<16>();
    e.header.version = r.u32();
    e.header.policy = r.fixed<16>();
    const std::uint16_t parents = r.u16();
    if (parents > kMaxParents) return Status::fail("too many parents");
    for (std::uint16_t i = 0; i < parents && !r.failed(); ++i) e.parents.push_back(r.fixed<32>());
    e.previous = r.fixed<32>();
    e.body = Bytes(r.blob(kMaxBody));
    const std::uint16_t seals = r.u16();
    if (seals > kMaxSeals) return Status::fail("too many seals");
    for (std::uint16_t i = 0; i < seals && !r.failed(); ++i) {
        Seal s;
        const std::uint8_t kind = r.u8();
        if (!r.failed() && (kind < 1 || kind > 3)) return Status::fail("unknown seal kind");
        s.kind = static_cast<SealKind>(kind);
        s.recipient = r.fixed<16>();
        const std::uint16_t len = r.u16();
        if (len > kSealCeiling) return Status::fail("a seal is too long");
        s.sealed = Bytes(r.raw(len));
        e.seals.push_back(std::move(s));
    }
    e.author = r.fixed<16>();
    e.signature = r.fixed<64>();
    if (r.failed()) return Status::fail("an envelope is truncated");
    if (!r.finish()) return Status::fail("an envelope has trailing bytes");
    out = std::move(e);
    return Status::ok();
}

bool verify(const Envelope& e, const PublicKey& author_key, CryptoPort& crypto) {
    return crypto.verify(author_key, signing_bytes(e), e.signature);
}

// value: plaintext, read only. It is copied after the header digest into a locked
// buffer, encrypted under a fresh key, and both buffers are wiped on return.
Status seal_value(const EnvelopeHeader& header, const SecretBuffer& value, const std::vector<Recipient>& recipients,
                  const std::vector<Digest>& parents, const Digest& previous, const DeviceId& author,
                  const SecretBuffer& author_key, CryptoPort& crypto, Envelope& out) {
    if (recipients.empty()) return Status::fail("an envelope needs at least one device to seal to");
    SecretBuffer key(kValueKeySize);
    if (!crypto.random(key.data(), key.size())) return Status::fail("no random bytes for the value key");

    const Digest digest = crypto.sha256(encode_header(header));
    SecretBuffer plain(digest.size() + value.size());
    std::memcpy(plain.data(), digest.data(), digest.size());
    if (!value.empty()) std::memcpy(plain.data() + digest.size(), value.data(), value.size());

    Envelope e;
    e.header = header;
    e.parents = parents;
    e.previous = previous;
    e.author = author;
    if (!crypto.token_encrypt(key, plain, e.body)) return Status::fail("encrypting the value failed");
    for (const Recipient& r : recipients) {
        Seal s;
        s.recipient = r.id;
        if (!crypto.seal(r.key, key, s.sealed)) return Status::fail("sealing the value key to a device failed");
        e.seals.push_back(std::move(s));
    }
    if (Status st = finish_signed(e, author_key, crypto); !st) return st;
    out = std::move(e);
    return Status::ok();
}

// out: receives the plaintext value. The value key and the decrypted body are
// SecretBuffers local to this function and are wiped when it returns.
Status open_value(const Envelope& e, const DeviceId& me, const SecretBuffer& my_key, CryptoPort& crypto,
                  SecretBuffer& out) {
    SecretBuffer key;
    if (Status st = open_value_key(e, me, my_key, crypto, key); !st) return st;
    SecretBuffer plain;
    if (!crypto.token_decrypt(key, e.body, plain)) return Status::fail("the envelope's body did not open (changed bytes)");
    const Digest digest = crypto.sha256(encode_header(e.header));
    if (plain.size() < digest.size() || std::memcmp(plain.data(), digest.data(), digest.size()) != 0)
        return Status::fail("the envelope's body belongs to another header (vault, secret, version or policy)");
    out = SecretBuffer::copy_of(plain.data() + digest.size(), plain.size() - digest.size());
    return Status::ok();
}

// my_key: read only, to open the value key (never the value) and to sign.
Status reseal(const Envelope& e, const DeviceId& me, const SecretBuffer& my_key, const std::vector<Recipient>& extra,
              const std::vector<Digest>& parents, const Digest& previous, CryptoPort& crypto, Envelope& out) {
    SecretBuffer key;
    if (Status st = open_value_key(e, me, my_key, crypto, key); !st) return st;
    Envelope n = e;
    n.parents = parents;
    n.previous = previous;
    n.author = me;
    for (const Recipient& r : extra) {
        bool present = false;
        for (const Seal& s : n.seals) present = present || (s.kind == SealKind::device && s.recipient == r.id);
        if (present) continue;
        Seal s;
        s.recipient = r.id;
        if (!crypto.seal(r.key, key, s.sealed)) return Status::fail("sealing the value key to a device failed");
        n.seals.push_back(std::move(s));
    }
    if (Status st = finish_signed(n, my_key, crypto); !st) return st;
    out = std::move(n);
    return Status::ok();
}

} // namespace verguenza
