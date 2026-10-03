/* tests/adapters/platform_test.cpp — the real adapters on disk (Windows): the DPAPI key
 * store and the Palabra-archive vault store, driven through the vault service, in a
 * temporary folder. Then the plaintext grep over the ACTUAL FILES: the vault, the
 * identity, and everything else in the folder (ground rule 4, on disk; roadmap 1B.12).
 *
 * The clipboard adapter is not driven here: it would overwrite the clipboard of whoever
 * runs the tests. */
#include "adapters/model/core_model.hpp"
#include "adapters/platform/win32/dpapi_key_store.hpp"
#include "adapters/platform/win32/secure_memory.hpp"
#include "adapters/reticulum/reticulum_crypto.hpp"
#include "adapters/storage/archive_vault_store.hpp"
#include "application/vault_service.hpp"
#include "support/check.hpp"
#include "support/fakes.hpp"

#include <windows.h>

#include <filesystem>
#include <fstream>
#include <iterator>

namespace fs = std::filesystem;
using namespace verguenza;

namespace {
std::string read_all(const fs::path& p) {
    std::ifstream f(p, std::ios::binary);
    return std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}
} // namespace

int main() {
    adapters::Win32SecureMemory memory;
    install_secure_memory(&memory);

    const fs::path dir = fs::temp_directory_path() / ("verguenza-test-" + std::to_string(::GetCurrentProcessId()));
    fs::remove_all(dir);
    fs::create_directories(dir);

    adapters::ReticulumCrypto crypto;
    adapters::ArchiveVaultStore store;
    adapters::DpapiKeyStore keys((dir / "device.identity").string());
    vtest::MemoryClipboard clipboard;
    vtest::FixedClock clock;
    const std::string path = (dir / "test.verguenza").string();

    std::uint8_t r[8];
    crypto.random(r, sizeof r);
    const std::string value = "vg-test-disk-" + to_hex(std::string_view(reinterpret_cast<const char*>(r), sizeof r));
    std::string id;
    {
        adapters::CoreModel model;
        app::VaultService v(app::Ports{crypto, store, keys, clipboard, model, clock});
        CHECK_OK(v.create(path, "Disk test", "Ana", "test-pc"));
        CHECK_OK(v.add_secret("password", {{"title", "On disk"}}, SecretBuffer::copy_of(value.data(), value.size()), &id));
        SecretBuffer out;
        CHECK_OK(v.reveal(id, out));
        CHECK(out.view() == value);
    }

    // the identity is DPAPI-wrapped: the file is not the raw key, and it opens again
    {
        SecretBuffer k;
        CHECK_OK(keys.load_identity(k));
        CHECK(k.size() == kPrivateKeySize);
        const std::string file = read_all(dir / "device.identity");
        CHECK(file.rfind("VGI1", 0) == 0);
        CHECK(file.find(std::string(k.view())) == std::string::npos);
    }

    // a fresh process-like session reopens the real file
    {
        adapters::CoreModel model;
        app::VaultService v(app::Ports{crypto, store, keys, clipboard, model, clock});
        CHECK_OK(v.unlock(path));
        SecretBuffer out;
        CHECK_OK(v.reveal(id, out));
        CHECK(out.view() == value);
        CHECK(v.audit().size() == 2);
    }

    // a damaged vault file is refused, not half-read
    {
        std::string bytes = read_all(path);
        bytes[bytes.size() / 2] ^= 0x40;
        const std::string damaged = (dir / "damaged.verguenza").string();
        std::ofstream(damaged, std::ios::binary).write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
        app::VaultFile f;
        CHECK(!store.load(damaged, f));
    }

    // ── the plaintext grep, over every file the run left behind ─────────────
    std::size_t files = 0;
    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;
        ++files;
        const std::string bytes = read_all(entry.path());
        CHECK(bytes.find(value) == std::string::npos);
        CHECK(bytes.find(to_hex(value)) == std::string::npos);
    }
    CHECK(files >= 2);

    fs::remove_all(dir);
    return vtest::finish("platform");
}
