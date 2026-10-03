/* application/vault_service.hpp — the use cases of a vault on one device (roadmap 1B).
 *
 * The one place the application's rules live, so the desktop window, a CLI and a
 * test drive exactly the same thing (okf/concepts/commands.md: agents use the same
 * verbs). Every change is two halves, in this order:
 *   1. the signed record (foundation/record): authority, envelopes, acts;
 *   2. the Void Core document (ModelPort), through dispatcher commands that NEVER carry
 *      a value: descriptions and display pointers only (ground rule 5; commands.md).
 * then the vault file is saved.
 *
 * Plaintext enters only as a SecretBuffer argument (intake, commands.md) and leaves
 * only through `reveal` (recorded first; not done if it cannot be recorded) and `copy`. */
#pragma once

#include "application/ports/ports.hpp"
#include "foundation/ports/crypto_port.hpp"
#include "foundation/record/record.hpp"
#include "foundation/secrets/generator.hpp"

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace verguenza::app {

struct Ports {
    CryptoPort& crypto;
    VaultStorePort& store;
    KeyStorePort& keys;
    ClipboardPort& clipboard;
    ModelPort& model;
    ClockPort& clock;
};

struct AuditLine {
    std::int64_t time = 0;
    std::string who;    // person and device
    std::string what;   // "revealed", "copied"
    std::string secret; // the secret's title, or its id if it has none
};

class VaultService {
public:
    explicit VaultService(Ports ports);
    ~VaultService();

    // ── the vault ───────────────────────────────────────────────────────────
    Status create(const std::string& path, const std::string& vault_name, const std::string& person_name,
                  const std::string& device_name);
    Status unlock(const std::string& path);
    /* Wipes every key and value this service holds and forgets the descriptions. */
    void lock();
    bool unlocked() const { return unlocked_; }
    bool vault_exists(const std::string& path) { return ports_.store.exists(path); }
    const std::string& vault_name() const { return vault_name_; }
    std::string protection() const { return ports_.keys.protection(); }

    // ── secrets ─────────────────────────────────────────────────────────────
    std::vector<Description> list();
    std::optional<Description> describe(const std::string& id);

    /* value: plaintext from intake, read only; sealed to this device and never stored
     * anywhere else. `id_out` receives the new secret's id. */
    Status add_secret(const std::string& kind, const std::map<std::string, std::string>& fields,
                      const SecretBuffer& value, std::string* id_out = nullptr);
    /* value: plaintext from intake, read only; sealed as the next version. */
    Status set_value(const std::string& id, const SecretBuffer& value);
    Status set_fields(const std::string& id, const std::map<std::string, std::string>& fields);
    Status remove(const std::string& id);

    /* out: receives a new plaintext password, made with the crypto port's randomness. */
    Status generate_password(const secrets::GeneratorOptions& options, SecretBuffer& out);

    /* out: receives the plaintext value. The reveal act is written and saved FIRST;
     * if that fails, nothing is decrypted (commands.md §"Reveal"). */
    Status reveal(const std::string& id, SecretBuffer& out, const std::string& how = "show");
    /* Reveal as "copy", place on the clipboard, wipe the local copy. The clipboard is
     * cleared after `clear_after_ms` by tick(). */
    Status copy(const std::string& id, std::int64_t clear_after_ms = 30000);

    /* Housekeeping, called by the shell every frame: the clipboard timer. */
    void tick();

    std::vector<AuditLine> audit();
    std::size_t record_size() const { return record_ ? record_->size() : 0; }

private:
    Status save();
    Status prepare_model();
    Status set_field(const std::string& id, const std::string& field, const std::string& value);
    Status require_unlocked() const;
    bool find_secret(const std::string& id, Id16& out) const;

    Ports ports_;
    bool unlocked_ = false;
    std::string path_;
    std::string vault_name_;
    SecretBuffer device_key_;
    PublicKey public_key_{};
    DeviceId me_{};
    std::optional<Record> record_;
    std::int64_t clipboard_deadline_ = 0;
};

constexpr const char* kSecretsMantle = "secrets";

} // namespace verguenza::app
