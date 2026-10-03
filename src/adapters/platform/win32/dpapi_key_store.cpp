#include "adapters/platform/win32/dpapi_key_store.hpp"

#include <windows.h>
#include <dpapi.h>

#include <filesystem>
#include <fstream>
#include <iterator>

namespace verguenza::adapters {

namespace {

constexpr std::string_view kMagic = "VGI1"; // Verguenza identity, DPAPI-wrapped, v1

/* Extra entropy DPAPI mixes in: not a secret, but it means a blob made for another
 * application under the same user does not open here, nor ours there. */
DATA_BLOB entropy() {
    static const char kEntropy[] = "verguenza/identity/v1";
    DATA_BLOB b;
    b.pbData = reinterpret_cast<BYTE*>(const_cast<char*>(kEntropy));
    b.cbData = sizeof kEntropy - 1;
    return b;
}

} // namespace

bool DpapiKeyStore::has_identity() {
    std::error_code ec;
    return std::filesystem::is_regular_file(path_, ec);
}

// private_key: plaintext key material, read only; DPAPI encrypts it and only the
// ciphertext is written to disk.
Status DpapiKeyStore::save_identity(const SecretBuffer& private_key) {
    DATA_BLOB in;
    in.pbData = const_cast<BYTE*>(private_key.data());
    in.cbData = static_cast<DWORD>(private_key.size());
    DATA_BLOB ent = entropy();
    DATA_BLOB out{};
    if (!CryptProtectData(&in, L"Void Verguenza device identity", &ent, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &out))
        return Status::fail("Windows could not protect this device's identity (DPAPI error " +
                            std::to_string(GetLastError()) + ")");
    std::string blob(kMagic);
    blob.append(reinterpret_cast<const char*>(out.pbData), out.cbData);
    LocalFree(out.pbData);

    // write-temp-then-rename, so a crash leaves the old identity or the new one
    std::error_code ec;
    std::filesystem::create_directories(std::filesystem::path(path_).parent_path(), ec);
    const std::string tmp = path_ + ".tmp";
    {
        std::ofstream f(tmp, std::ios::binary | std::ios::trunc);
        f.write(blob.data(), static_cast<std::streamsize>(blob.size()));
        f.flush();
        if (!f) return Status::fail("could not write " + tmp);
    }
    if (!MoveFileExA(tmp.c_str(), path_.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        return Status::fail("could not put the identity file in place at " + path_);
    return Status::ok();
}

// out: receives the plaintext private key; DPAPI's own output buffer is wiped and freed.
Status DpapiKeyStore::load_identity(SecretBuffer& out) {
    std::ifstream f(path_, std::ios::binary);
    if (!f) return Status::fail("no identity file at " + path_);
    std::string blob((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    if (blob.size() <= kMagic.size() || blob.compare(0, kMagic.size(), kMagic) != 0)
        return Status::fail("the identity file is not one Void Verguenza wrote");

    DATA_BLOB in;
    in.pbData = reinterpret_cast<BYTE*>(blob.data() + kMagic.size());
    in.cbData = static_cast<DWORD>(blob.size() - kMagic.size());
    DATA_BLOB ent = entropy();
    DATA_BLOB plain{};
    if (!CryptUnprotectData(&in, nullptr, &ent, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &plain))
        return Status::fail("Windows would not open this device's identity: it was protected by another "
                            "Windows user or on another computer");
    out = SecretBuffer::copy_of(plain.pbData, plain.cbData);
    SecureZeroMemory(plain.pbData, plain.cbData);
    LocalFree(plain.pbData);
    return Status::ok();
}

std::string DpapiKeyStore::protection() const {
    return "This device's key is protected by Windows for your Windows account. Another Windows "
           "user, or someone who copies your files to another computer, cannot open it. Anyone who "
           "can sign in as you on this computer can.";
}

} // namespace verguenza::adapters
