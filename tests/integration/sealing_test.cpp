/* tests/integration/sealing_test.cpp — envelopes and the signed record, with the real
 * Reticulum adapter (roadmap 1B.2, 1B.3, 1B.5). Every refusal in formats.md that these
 * layers own has a crafted input here. */
#include "adapters/reticulum/reticulum_crypto.hpp"
#include "foundation/record/record.hpp"
#include "foundation/sealing/envelope.hpp"
#include "support/check.hpp"

#include <cstring>

using namespace verguenza;

namespace {

struct Device {
    SecretBuffer key;
    PublicKey pub{};
    DeviceId id{};
};

Device make_device(CryptoPort& c) {
    Device d;
    CHECK(c.generate_identity(d.key, d.pub));
    d.id = c.device_id(d.pub);
    return d;
}

SecretBuffer value_of(const char* s) { return SecretBuffer::copy_of(s, std::strlen(s)); }

} // namespace

int main() {
    adapters::ReticulumCrypto crypto;
    CHECK(std::strlen(adapters::ReticulumCrypto::entropy_source()) > 0);

    Device a = make_device(crypto), b = make_device(crypto);

    // ── envelopes ──────────────────────────────────────────────────────────
    EnvelopeHeader h;
    crypto.random(h.vault.data(), h.vault.size());
    crypto.random(h.secret.data(), h.secret.size());
    const SecretBuffer value = value_of("vg-test-7f3a9c-not-a-real-password");

    Envelope env;
    CHECK_OK(seal_value(h, value, {{a.id, a.pub}}, {}, Digest{}, a.id, a.key, crypto, env));
    CHECK(verify(env, a.pub, crypto));
    CHECK(!verify(env, b.pub, crypto));

    const Bytes encoded = encode(env);
    CHECK(encoded.find(std::string(value.view())) == std::string::npos); // no plaintext in the bytes

    Envelope back;
    CHECK_OK(decode(encoded, back));
    SecretBuffer opened;
    CHECK_OK(open_value(back, a.id, a.key, crypto, opened));
    CHECK(opened.view() == value.view());

    // not sealed to b: refused
    SecretBuffer nope;
    CHECK(!open_value(back, b.id, b.key, crypto, nope));

    // a flipped byte in each region fails: decode, signature, or opening
    for (std::size_t i = 0; i < encoded.size(); i += 7) {
        Bytes bad = encoded;
        bad[i] ^= 0x01;
        Envelope e2;
        if (!decode(bad, e2)) continue;
        SecretBuffer v2;
        const bool opens = open_value(e2, a.id, a.key, crypto, v2) && v2.view() == value.view();
        CHECK(!verify(e2, a.pub, crypto) || !opens);
    }
    // truncated and trailing bytes are refused
    Envelope tmp;
    CHECK(!decode(encoded.substr(0, encoded.size() - 1), tmp));
    CHECK(!decode(encoded + "x", tmp));

    // the body moved under another header opens to a digest that does not match
    {
        Envelope moved = back;
        moved.header.version = 2;
        SecretBuffer v;
        Status st = open_value(moved, a.id, a.key, crypto, v);
        CHECK(!st);
        CHECK(st.error().find("another header") != std::string::npos);
    }

    // reseal to b: same body, new seal, b can now open it
    {
        Envelope wider;
        CHECK_OK(reseal(back, a.id, a.key, {{b.id, b.pub}}, {}, Digest{}, crypto, wider));
        CHECK(wider.body == back.body);
        CHECK(wider.seals.size() == 2);
        SecretBuffer v;
        CHECK_OK(open_value(wider, b.id, b.key, crypto, v));
        CHECK(v.view() == value.view());
    }

    // ── signed objects ─────────────────────────────────────────────────────
    {
        SignedObject o;
        o.type = ObjectType::act;
        o.body = Fields().set(1, "x").encode();
        o.author = a.id;
        CHECK_OK(sign(o, a.key, crypto));
        CHECK(verify(o, a.pub, crypto));
        SignedObject as_other = o;
        as_other.type = ObjectType::approval; // a signature made for one type fails as another
        CHECK(!verify(as_other, a.pub, crypto));
        SignedObject back_o;
        CHECK_OK(decode(encode(o), back_o));
        CHECK(verify(back_o, a.pub, crypto));
        CHECK(!decode(encode(o) + "x", back_o));
    }

    // ── the record ─────────────────────────────────────────────────────────
    {
        Record rec(crypto);
        CHECK_OK(Record::found("test vault", "Ana", "laptop", a.key, 1000, crypto, rec));
        CHECK(rec.founded() && rec.vault_name() == "test vault");
        CHECK(rec.device(a.id).has_value());

        EnvelopeHeader h1;
        h1.vault = rec.vault_id();
        crypto.random(h1.secret.data(), h1.secret.size());
        Envelope v1, v2;
        CHECK_OK(seal_value(h1, value, {{a.id, a.pub}}, rec.heads(), rec.last_of(a.id), a.id, a.key, crypto, v1));
        CHECK_OK(rec.add(encode(v1)));
        EnvelopeHeader h2 = h1;
        h2.version = 2;
        const SecretBuffer value2 = value_of("vg-test-second-version");
        CHECK_OK(seal_value(h2, value2, {{a.id, a.pub}}, rec.heads(), rec.last_of(a.id), a.id, a.key, crypto, v2));
        CHECK_OK(rec.add(encode(v2)));

        auto cur = rec.current_envelope(h1.secret);
        CHECK(cur && cur->header.version == 2); // the highest valid version, whatever a document says

        // a device the vault never admitted cannot add an envelope
        Envelope stranger;
        EnvelopeHeader h3 = h1;
        h3.version = 9;
        CHECK_OK(seal_value(h3, value, {{b.id, b.pub}}, rec.heads(), Digest{}, b.id, b.key, crypto, stranger));
        CHECK(!rec.add(encode(stranger)));
        CHECK(rec.current_envelope(h1.secret)->header.version == 2);

        // rebuild from the saved bytes, in reverse order: held, then judged
        Record again(crypto);
        const auto all = rec.all_bytes();
        for (auto it = all.rbegin(); it != all.rend(); ++it) (void)again.add(it->second);
        CHECK(again.founded());
        CHECK(again.held_count() == 0);
        CHECK(again.current_envelope(h1.secret)->header.version == 2);

        // retire removes it from the current values
        CHECK_OK(rec.append(ObjectType::retire, Fields().set(act_field::secret, h1.secret), 2000, a.id, a.key));
        CHECK(!rec.current_envelope(h1.secret));
    }

    return vtest::finish("sealing");
}
