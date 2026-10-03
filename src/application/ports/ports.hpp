/* application/ports/ports.hpp — what the vault service needs from the world.
 *
 * Each port has one adapter in src/adapters/ (okf/concepts/architecture.md):
 *   VaultStorePort  → adapters/storage   (the .verguenza file, a Palabra Archive)
 *   KeyStorePort    → adapters/platform  (DPAPI on Windows: Q12, answered)
 *   ClipboardPort   → adapters/platform  (with history and cloud exclusion)
 *   ModelPort       → adapters/model     (Void Core, through Void Maiz)
 *   ClockPort       → adapters/platform
 * Tests meet them with fakes (tests/support/fakes.hpp). */
#pragma once

#include "foundation/core/bytes.hpp"
#include "foundation/core/status.hpp"
#include "foundation/secret/secret_buffer.hpp"

#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace verguenza::app {

/* Everything a vault keeps at rest: the Void Core document (descriptions, which open
 * nothing) and every signed object and envelope by address. Never a plaintext, never
 * a private key (okf/concepts/sealing.md §"The vault bundle"). */
struct VaultFile {
    std::string state_json;
    std::map<Digest, Bytes> objects;
};

class VaultStorePort {
public:
    virtual ~VaultStorePort() = default;
    virtual bool exists(const std::string& path) = 0;
    virtual Status load(const std::string& path, VaultFile& out) = 0;
    /* Atomic: a crash costs the save, never the vault. */
    virtual Status save(const std::string& path, const VaultFile& file) = 0;
};

/* This device's identity at rest. Q12 (answered 2026-10-01): the operating system's key
 * store, no passphrase, no libsodium. */
class KeyStorePort {
public:
    virtual ~KeyStorePort() = default;
    virtual bool has_identity() = 0;
    /* private_key: plaintext key material, read only; handed to the OS key store. */
    virtual Status save_identity(const SecretBuffer& private_key) = 0;
    /* out: receives the plaintext private key. */
    virtual Status load_identity(SecretBuffer& out) = 0;
    /* What this protection stops and what it does not, in words for the lock screen. */
    virtual std::string protection() const = 0;
};

class ClipboardPort {
public:
    virtual ~ClipboardPort() = default;
    /* value: plaintext, read only; placed on the clipboard, marked for exclusion from
     * clipboard history and cloud sync where the platform supports it. */
    virtual Status put_secret(const SecretBuffer& value) = 0;
    /* Clears the clipboard only if it still holds what put_secret placed there. */
    virtual void clear_if_unchanged() = 0;
};

/* A secret's description, as the Void Core document holds it. */
struct Description {
    std::string id;   // the rune's name: the secret id, in hex
    std::string kind; // the rune's glyph
    std::map<std::string, std::string> fields;
    std::vector<std::string> tags;
    std::string field(const std::string& name) const {
        auto it = fields.find(name);
        return it == fields.end() ? std::string() : it->second;
    }
};

/* The model lives in Void Core; the dispatcher is the only door (ground rule 5). */
class ModelPort {
public:
    virtual ~ModelPort() = default;
    /* Replace the document ("" is the empty document). */
    virtual Status reset(std::string_view state_json) = 0;
    virtual std::string export_state() = 0;
    virtual Status dispatch(std::string_view command) = 0;
    /* One dispatcher argument, quoted by Void Core's own codec (SPEC §6.1). */
    virtual std::string quote(std::string_view value) = 0;
    virtual std::vector<Description> descriptions(const std::string& mantle) = 0;
};

class ClockPort {
public:
    virtual ~ClockPort() = default;
    virtual std::int64_t now_ms() = 0; // unix ms; recorded for people, never consulted for order
};

} // namespace verguenza::app
