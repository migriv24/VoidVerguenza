/* foundation/record/fields.hpp — the body of a signed object: tagged fields.
 *
 * formats.md left each type's body "a small fixed layout, written when each phase
 * builds its type". This is that layout, once for all types: a sequence of
 * (tag u8, length u32, bytes), tags strictly increasing. Strictly increasing makes the
 * encoding canonical (one way to write a body, so an object's hash is its name), and a
 * reader refuses anything else rather than guess. Bodies hold public bytes only. */
#pragma once

#include "foundation/core/bytes.hpp"
#include "foundation/core/status.hpp"

#include <cstdint>
#include <map>
#include <optional>
#include <string_view>

namespace verguenza {

class Fields {
public:
    Fields& set(std::uint8_t tag, std::string_view value) {
        values_[tag] = Bytes(value);
        return *this;
    }
    template <std::size_t N>
    Fields& set(std::uint8_t tag, const std::array<std::uint8_t, N>& v) { return set(tag, view(v)); }
    Fields& set_u32(std::uint8_t tag, std::uint32_t v);

    bool has(std::uint8_t tag) const { return values_.count(tag) != 0; }
    std::string_view get(std::uint8_t tag) const;
    std::optional<std::uint32_t> get_u32(std::uint8_t tag) const;
    template <std::size_t N>
    std::optional<std::array<std::uint8_t, N>> get_fixed(std::uint8_t tag) const {
        std::string_view v = get(tag);
        if (!has(tag) || v.size() != N) return std::nullopt;
        std::array<std::uint8_t, N> a{};
        for (std::size_t i = 0; i < N; ++i) a[i] = static_cast<std::uint8_t>(v[i]);
        return a;
    }

    Bytes encode() const;
    static Status decode(std::string_view in, Fields& out);

private:
    std::map<std::uint8_t, Bytes> values_; // ordered, so encode() is canonical
};

} // namespace verguenza
