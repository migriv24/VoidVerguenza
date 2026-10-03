/* adapters/reticulum/reticulum_crypto.hpp — the crypto port, met by Reticulum through
 * Void Palabra's crypto.hpp (ground rule 3). The only file in Verguenza that names a
 * cryptographic library. */
#pragma once

#include "foundation/ports/crypto_port.hpp"

namespace verguenza::adapters {

class ReticulumCrypto final : public CryptoPort {
public:
    bool random(std::uint8_t* out, std::size_t len) override;
    Digest sha256(std::string_view data) override;
    /* private_key: receives new plaintext key material. */
    bool generate_identity(SecretBuffer& private_key, PublicKey& public_key) override;
    /* private_key: read only. */
    bool public_key_of(const SecretBuffer& private_key, PublicKey& public_key) override;
    DeviceId device_id(const PublicKey& public_key) override;
    /* private_key: read only, to sign. */
    bool sign(const SecretBuffer& private_key, std::string_view message, Signature& out) override;
    bool verify(const PublicKey& public_key, std::string_view message, const Signature& signature) override;
    /* plaintext: read only, encrypted to the recipient's identity. */
    bool seal(const PublicKey& recipient, const SecretBuffer& plaintext, Bytes& sealed) override;
    /* private_key: read only; out: receives the plaintext. */
    bool open(const SecretBuffer& private_key, std::string_view sealed, SecretBuffer& out) override;
    /* key and plaintext: read only. */
    bool token_encrypt(const SecretBuffer& key, const SecretBuffer& plaintext, Bytes& token) override;
    /* key: read only; out: receives the plaintext. */
    bool token_decrypt(const SecretBuffer& key, std::string_view token, SecretBuffer& out) override;

    /* Which operating-system generator seeds Reticulum's ("" means none: refuse). */
    static const char* entropy_source();
};

} // namespace verguenza::adapters
