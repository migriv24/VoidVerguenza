/* foundation/secrets/generator.hpp — Verguenza makes a password itself.
 *
 * Characters are drawn from the alphabet by rejection sampling over random bytes, so
 * every character is equally likely (a plain `byte % size` would favour the first
 * characters of any alphabet whose size does not divide 256). The randomness comes
 * through the crypto port; the result is written straight into a SecretBuffer. */
#pragma once

#include "foundation/core/status.hpp"
#include "foundation/ports/crypto_port.hpp"
#include "foundation/secret/secret_buffer.hpp"

#include <string>

namespace verguenza::secrets {

struct GeneratorOptions {
    std::size_t length = 20;
    bool lower = true;
    bool upper = true;
    bool digits = true;
    bool symbols = true;
};

std::string alphabet(const GeneratorOptions& options);

/* out: receives a new plaintext password of options.length characters, followed by a
 * NUL inside the buffer's capacity (so a text box can show it) but not in its size. */
Status generate_password(const GeneratorOptions& options, CryptoPort& crypto, SecretBuffer& out);

} // namespace verguenza::secrets
