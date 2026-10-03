/* adapters/platform/win32/dpapi_key_store.hpp — this device's identity at rest, under
 * Windows' DPAPI (CryptProtectData). Q12, answered 2026-10-01: the operating system's key
 * store, no passphrase, no libsodium.
 *
 * What DPAPI stops: another Windows user on this computer, and someone who copies the
 * file to another computer. What it does NOT stop: anyone who can log in as this user
 * (or malware running as them). `protection()` says both, for the lock screen. */
#pragma once

#include "application/ports/ports.hpp"

#include <string>

namespace verguenza::adapters {

class DpapiKeyStore final : public app::KeyStorePort {
public:
    explicit DpapiKeyStore(std::string identity_path) : path_(std::move(identity_path)) {}

    bool has_identity() override;
    /* private_key: plaintext key material, read only; encrypted by DPAPI before it is written. */
    Status save_identity(const SecretBuffer& private_key) override;
    /* out: receives the plaintext private key. */
    Status load_identity(SecretBuffer& out) override;
    std::string protection() const override;

private:
    std::string path_;
};

} // namespace verguenza::adapters
