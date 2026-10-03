#include "foundation/secrets/generator.hpp"

#include "foundation/ports/secure_memory_port.hpp"

namespace verguenza::secrets {

std::string alphabet(const GeneratorOptions& o) {
    std::string a;
    if (o.lower) a += "abcdefghijkmnopqrstuvwxyz";  // no 'l': read beside '1'
    if (o.upper) a += "ABCDEFGHJKLMNPQRSTUVWXYZ";   // no 'I', 'O'
    if (o.digits) a += "23456789";                  // no '0', '1'
    if (o.symbols) a += "!#$%&*+-=?@^_~";
    return a;
}

// out: receives the generated plaintext password.
Status generate_password(const GeneratorOptions& options, CryptoPort& crypto, SecretBuffer& out) {
    const std::string a = alphabet(options);
    if (a.empty()) return Status::fail("choose at least one kind of character");
    if (options.length < 4 || options.length > 1024) return Status::fail("a generated password is 4 to 1024 characters");
    const unsigned limit = 256 - (256 % a.size()); // bytes at or above this are rejected

    SecretBuffer pw(options.length + 1);
    std::size_t filled = 0;
    std::uint8_t pool[64];
    while (filled < options.length) {
        if (!crypto.random(pool, sizeof pool)) return Status::fail("no random bytes to make a password from");
        for (std::uint8_t b : pool) {
            if (filled == options.length) break;
            if (b >= limit) continue;
            pw.data()[filled++] = static_cast<std::uint8_t>(a[b % a.size()]);
        }
    }
    wipe(pool, sizeof pool);
    pw.data()[options.length] = 0;
    pw.resize(options.length);
    out = std::move(pw);
    return Status::ok();
}

} // namespace verguenza::secrets
