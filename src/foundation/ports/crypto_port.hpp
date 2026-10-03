/* foundation/ports/crypto_port.hpp — every cryptographic operation the domain uses.
 *
 * Ground rule 3: cryptography comes from Reticulum, through Void Palabra. The domain
 * does not link either; it declares what it needs here, and
 * adapters/reticulum/reticulum_crypto.cpp meets it with Palabra's crypto.hpp.
 * Nothing in the foundation implements a primitive.
 *
 * Plaintext arguments and results are SecretBuffers; everything else is public bytes. */
#pragma once

#include "foundation/core/bytes.hpp"
#include "foundation/secret/secret_buffer.hpp"

#include <string_view>

namespace verguenza {

class CryptoPort {
public:
    virtual ~CryptoPort() = default;

    /* Random bytes for what the domain makes itself (value keys, ids, nonces).
     * False if no entropy source is available: callers refuse rather than continue. */
    virtual bool random(std::uint8_t* out, std::size_t len) = 0;
    virtual Digest sha256(std::string_view data) = 0;

    /* A new device identity. private_key: plaintext key material, written into a
     * SecretBuffer of kPrivateKeySize. */
    virtual bool generate_identity(SecretBuffer& private_key, PublicKey& public_key) = 0;
    /* private_key: read only, to derive the public half. */
    virtual bool public_key_of(const SecretBuffer& private_key, PublicKey& public_key) = 0;
    virtual DeviceId device_id(const PublicKey& public_key) = 0;

    /* private_key: read only, to sign. */
    virtual bool sign(const SecretBuffer& private_key, std::string_view message, Signature& out) = 0;
    virtual bool verify(const PublicKey& public_key, std::string_view message, const Signature& signature) = 0;

    /* Seal to an identity (Reticulum's encrypt-to-identity). plaintext: read only;
     * the result is ciphertext. */
    virtual bool seal(const PublicKey& recipient, const SecretBuffer& plaintext, Bytes& sealed) = 0;
    /* Open a seal with a private key. private_key: read only; out: receives the
     * plaintext, sized to it. */
    virtual bool open(const SecretBuffer& private_key, std::string_view sealed, SecretBuffer& out) = 0;

    /* A token under a 64-byte key (AES-256-CBC + HMAC-SHA256). key and plaintext:
     * read only; the token is ciphertext. */
    virtual bool token_encrypt(const SecretBuffer& key, const SecretBuffer& plaintext, Bytes& token) = 0;
    /* key: read only; out: receives the plaintext, sized to it. */
    virtual bool token_decrypt(const SecretBuffer& key, std::string_view token, SecretBuffer& out) = 0;
};

} // namespace verguenza
