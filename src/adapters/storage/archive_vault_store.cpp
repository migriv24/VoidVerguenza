#include "adapters/storage/archive_vault_store.hpp"

#include "voidpalabra/archive.hpp"

#include "cJSON.h"

#include <filesystem>
#include <memory>

namespace verguenza::adapters {

namespace {

constexpr const char* kObjectPrefix = "obj/";

struct JsonDeleter {
    void operator()(cJSON* j) const { cJSON_Delete(j); }
};
using Json = std::unique_ptr<cJSON, JsonDeleter>;

} // namespace

bool ArchiveVaultStore::exists(const std::string& path) {
    std::error_code ec;
    return std::filesystem::is_regular_file(path, ec);
}

Status ArchiveVaultStore::load(const std::string& path, app::VaultFile& out) {
    voidpalabra::Archive archive;
    std::string why;
    if (!voidpalabra::Archive::read_file(path, archive, &why)) return Status::fail("could not open the vault file: " + why);

    app::VaultFile f;
    Json latest(archive.load_latest());
    if (!latest) return Status::fail("the vault file holds no document");
    char* text = cJSON_PrintUnformatted(latest.get());
    if (!text) return Status::fail("the vault's document could not be read");
    f.state_json = text;
    cJSON_free(text);

    for (const std::string& name : archive.asset_names()) {
        if (name.rfind(kObjectPrefix, 0) != 0) continue;
        Digest addr{};
        if (!from_hex(name.substr(4), addr)) continue;
        Bytes bytes;
        if (archive.get_asset(name, bytes)) f.objects.emplace(addr, std::move(bytes));
    }
    out = std::move(f);
    return Status::ok();
}

Status ArchiveVaultStore::save(const std::string& path, const app::VaultFile& file) {
    // append to the existing archive, so its history of saves is kept
    voidpalabra::Archive archive;
    std::string why;
    if (exists(path) && !voidpalabra::Archive::read_file(path, archive, &why))
        return Status::fail("the existing vault file could not be read, so it was not overwritten: " + why);

    Json state(cJSON_Parse(file.state_json.c_str()));
    if (!state) return Status::fail("the vault's document is not valid JSON");
    archive.save(state.get(), "verguenza");
    for (const auto& [addr, bytes] : file.objects) archive.put_asset(kObjectPrefix + to_hex(addr), bytes);

    std::error_code ec;
    std::filesystem::create_directories(std::filesystem::path(path).parent_path(), ec);
    if (!archive.write_file(path, &why)) return Status::fail("the vault could not be saved: " + why);
    return Status::ok();
}

} // namespace verguenza::adapters
