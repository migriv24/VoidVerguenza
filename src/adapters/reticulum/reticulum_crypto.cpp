#include "adapters/reticulum/reticulum_crypto.hpp"

#include "voidpalabra/crypto.hpp"
#include "voidpalabra/reticulum.hpp"

#include <cstring>
#include <string>

namespace verguenza::adapters {

namespace rc = voidpalabra::reticulum::crypto;

static_assert(rc::kPrivateKeySize == kPrivateKeySize);
static_assert(rc::kTokenKeySize == kValueKeySize);
static_assert(rc::kPublicKeySize == std::tuple_size<PublicKey>::value);
static_assert(rc::kSignatureSize == std::tuple_size<Signature>::value);
static_assert(rc::kIdentityHashSize == std::tuple_size<DeviceId>::value);

bool ReticulumCrypto::random(std::uint8_t* out, std::size_t len) { return rc::random(out, len); }

Digest ReticulumCrypto::sha256(std::string_view data) {
    Digest d{};
    rc::sha256(u8(data), data.size(), d.data());
    return d;
}

// private_key: receives new plaintext key material, written straight into locked memory.
bool ReticulumCrypto::generate_identity(SecretBuffer& private_key, PublicKey& public_key) {
    SecretBuffer key(kPrivateKeySize);
    if (!rc::generate_identity(key.data(), public_key.data())) return false;
    private_key = std::move(key);
    return true;
}

// private_key: read only, to derive the public half.
bool ReticulumCrypto::public_key_of(const SecretBuffer& private_key, PublicKey& public_key) {
    return private_key.size() == kPrivateKeySize && rc::public_key_of(private_key.data(), public_key.data());
}

DeviceId ReticulumCrypto::device_id(const PublicKey& public_key) {
    DeviceId id{};
    rc::identity_hash(public_key.data(), id.data());
    return id;
}

// private_key: read only, to sign.
bool ReticulumCrypto::sign(const SecretBuffer& private_key, std::string_view message, Signature& out) {
    return private_key.size() == kPrivateKeySize &&
           rc::sign(private_key.data(), u8(message), message.size(), out.data());
}

bool ReticulumCrypto::verify(const PublicKey& public_key, std::string_view message, const Signature& signature) {
    return rc::verify(public_key.data(), u8(message), message.size(), signature.data());
}

// plaintext: read only, handed to Palabra to encrypt to the recipient's identity.
bool ReticulumCrypto::seal(const PublicKey& recipient, const SecretBuffer& plaintext, Bytes& sealed) {
    return rc::encrypt_to(recipient.data(), plaintext.data(), plaintext.size(), sealed);
}

// out: receives the plaintext, decrypted by Palabra straight into locked memory.
bool ReticulumCrypto::open(const SecretBuffer& private_key, std::string_view sealed, SecretBuffer& out) {
    if (private_key.size() != kPrivateKeySize || sealed.empty()) return false;
    SecretBuffer buf(sealed.size()); // the plaintext is never longer than its ciphertext
    std::size_t written = 0;
    if (!rc::decrypt(private_key.data(), u8(sealed), sealed.size(), buf.data(), buf.capacity(), &written)) return false;
    buf.resize(written);
    out = std::move(buf);
    return true;
}

// key and plaintext: read only, handed to Palabra's token.
bool ReticulumCrypto::token_encrypt(const SecretBuffer& key, const SecretBuffer& plaintext, Bytes& token) {
    return key.size() == kValueKeySize && rc::token_encrypt(key.data(), plaintext.data(), plaintext.size(), token);
}

// out: receives the plaintext, decrypted by Palabra straight into locked memory.
bool ReticulumCrypto::token_decrypt(const SecretBuffer& key, std::string_view token, SecretBuffer& out) {
    if (key.size() != kValueKeySize || token.empty()) return false;
    SecretBuffer buf(token.size());
    std::size_t written = 0;
    if (!rc::token_decrypt(key.data(), u8(token), token.size(), buf.data(), buf.capacity(), &written)) return false;
    buf.resize(written);
    out = std::move(buf);
    return true;
}

const char* ReticulumCrypto::entropy_source() {
    static const std::string s = voidpalabra::reticulum::Node::entropy_source();
    return s.c_str();
}

} // namespace verguenza::adapters
