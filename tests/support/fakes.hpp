/* tests/support/fakes.hpp — ports met in memory, for tests. Cryptography and Void Core are
 * NOT faked: tests use the real Reticulum adapter and the real model adapter, so what is
 * tested is what ships. Every secret a test uses is generated at test time and obviously
 * fake (ground rule 2). */
#pragma once

#include "application/ports/ports.hpp"

#include <map>
#include <string>

namespace vtest {

using namespace verguenza;

/* The vault file, kept in memory; `bytes_of` gives everything saved, for the plaintext
 * grep. `fail_saves` simulates a full disk. */
class MemoryStore final : public app::VaultStorePort {
public:
    std::map<std::string, app::VaultFile> files;
    bool fail_saves = false;
    bool exists(const std::string& path) override { return files.count(path) != 0; }
    Status load(const std::string& path, app::VaultFile& out) override {
        if (!files.count(path)) return Status::fail("no such vault");
        out = files[path];
        return Status::ok();
    }
    Status save(const std::string& path, const app::VaultFile& file) override {
        if (fail_saves) return Status::fail("the disk is full (simulated)");
        files[path] = file;
        return Status::ok();
    }
    std::string bytes_of(const std::string& path) {
        std::string all = files[path].state_json;
        for (const auto& [addr, b] : files[path].objects) all += b;
        return all;
    }
};

/* A key store that keeps the key in plain test memory: a fake for tests ONLY, which is
 * exactly the protection the real one exists to avoid. */
class MemoryKeyStore final : public app::KeyStorePort {
public:
    std::string key;
    bool has_identity() override { return !key.empty(); }
    Status save_identity(const SecretBuffer& private_key) override {
        key.assign(reinterpret_cast<const char*>(private_key.data()), private_key.size());
        return Status::ok();
    }
    Status load_identity(SecretBuffer& out) override {
        out = SecretBuffer::copy_of(key.data(), key.size());
        return Status::ok();
    }
    std::string protection() const override { return "test memory (not protected)"; }
};

class MemoryClipboard final : public app::ClipboardPort {
public:
    std::string text;
    bool ours = false;
    Status put_secret(const SecretBuffer& value) override {
        text.assign(reinterpret_cast<const char*>(value.data()), value.size());
        ours = true;
        return Status::ok();
    }
    void clear_if_unchanged() override {
        if (ours) text.clear();
        ours = false;
    }
};

class FixedClock final : public app::ClockPort {
public:
    std::int64_t now = 1'790'000'000'000; // 2026, roughly
    std::int64_t now_ms() override { return now; }
};

} // namespace vtest
