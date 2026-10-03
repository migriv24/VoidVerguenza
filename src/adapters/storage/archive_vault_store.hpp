/* adapters/storage/archive_vault_store.hpp — the vault store port, as a Void Palabra
 * Archive: the `.verguenza` file (okf/concepts/sealing.md §"The vault bundle").
 *
 *   saves   one per change: the whole Void Core document (descriptions; it opens
 *           nothing), versioned and deduplicated by Palabra, so a vault keeps its own
 *           history of descriptions for free
 *   assets  "obj/<sha256 hex>": every signed object and envelope, content-addressed
 *
 * Written atomically by Palabra (temp beside the target, flushed, renamed). Palabra
 * re-hashes every block on read and refuses a mismatch; Verguenza re-checks every
 * object again when it builds the record. */
#pragma once

#include "application/ports/ports.hpp"

namespace verguenza::adapters {

class ArchiveVaultStore final : public app::VaultStorePort {
public:
    bool exists(const std::string& path) override;
    Status load(const std::string& path, app::VaultFile& out) override;
    Status save(const std::string& path, const app::VaultFile& file) override;
};

} // namespace verguenza::adapters
