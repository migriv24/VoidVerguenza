#include "foundation/core/bytes.hpp"

namespace verguenza {

std::string to_hex(std::string_view bytes) {
    static constexpr char kDigits[] = "0123456789abcdef";
    std::string out;
    out.reserve(bytes.size() * 2);
    for (unsigned char c : bytes) {
        out.push_back(kDigits[c >> 4]);
        out.push_back(kDigits[c & 0x0f]);
    }
    return out;
}

namespace {
int nibble(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}
} // namespace

bool from_hex(std::string_view hex, Bytes& out) {
    if (hex.size() % 2) return false;
    Bytes b;
    b.reserve(hex.size() / 2);
    for (std::size_t i = 0; i < hex.size(); i += 2) {
        const int hi = nibble(hex[i]), lo = nibble(hex[i + 1]);
        if (hi < 0 || lo < 0) return false;
        b.push_back(static_cast<char>((hi << 4) | lo));
    }
    out = std::move(b);
    return true;
}

} // namespace verguenza
