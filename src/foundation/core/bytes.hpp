/* foundation/core/bytes.hpp — byte strings and the fixed-size values the formats use.
 *
 * `Bytes` holds PUBLIC bytes only: ciphertext, signatures, addresses, encoded objects.
 * A plaintext never lives in one (ground rule 4); that is what `SecretBuffer` is for
 * (foundation/secret/secret_buffer.hpp). The distinction is the whole reason these are
 * two types. */
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace verguenza {

using Bytes = std::string; // binary-safe; public bytes only (see above)

using Digest = std::array<std::uint8_t, 32>;    // SHA-256: object addresses, vault ids
using DeviceId = std::array<std::uint8_t, 16>;  // a Reticulum identity hash
using PublicKey = std::array<std::uint8_t, 64>; // X25519 public ‖ Ed25519 public
using Signature = std::array<std::uint8_t, 64>; // Ed25519
using Id16 = std::array<std::uint8_t, 16>;      // secret, member, policy, request ids

constexpr std::size_t kPrivateKeySize = 64; // X25519 private ‖ Ed25519 private
constexpr std::size_t kValueKeySize = 64;   // a token key: AES-256-CBC + HMAC-SHA256 halves

inline const std::uint8_t* u8(std::string_view s) { return reinterpret_cast<const std::uint8_t*>(s.data()); }

template <std::size_t N>
std::string_view view(const std::array<std::uint8_t, N>& a) {
    return {reinterpret_cast<const char*>(a.data()), N};
}

template <std::size_t N>
bool is_zero(const std::array<std::uint8_t, N>& a) {
    for (auto b : a)
        if (b) return false;
    return true;
}

std::string to_hex(std::string_view bytes);
template <std::size_t N>
std::string to_hex(const std::array<std::uint8_t, N>& a) { return to_hex(view(a)); }

/* False on odd length or a non-hex character; `out` is untouched then. */
bool from_hex(std::string_view hex, Bytes& out);
template <std::size_t N>
bool from_hex(std::string_view hex, std::array<std::uint8_t, N>& out) {
    Bytes b;
    if (!from_hex(hex, b) || b.size() != N) return false;
    for (std::size_t i = 0; i < N; ++i) out[i] = static_cast<std::uint8_t>(b[i]);
    return true;
}

} // namespace verguenza
