/* foundation/core/codec.hpp — the little-endian, length-prefixed encoding every
 * Verguenza format uses (okf/concepts/formats.md): one way to write each object, so
 * its hash is its name.
 *
 * The Reader never throws and never reads past the end: the first short read sets
 * `failed()` and every later read returns zeros. A decoder reads everything, then
 * asks once whether it failed and whether bytes were left over. That keeps each
 * decoder a straight line, and makes "truncated" and "trailing bytes" impossible to
 * forget, because `finish()` checks both. */
#pragma once

#include "foundation/core/bytes.hpp"

#include <cstdint>
#include <string_view>

namespace verguenza {

class Writer {
public:
    void u8(std::uint8_t v) { out_.push_back(static_cast<char>(v)); }
    void u16(std::uint16_t v) { put(v, 2); }
    void u32(std::uint32_t v) { put(v, 4); }
    void i64(std::int64_t v) { put(static_cast<std::uint64_t>(v), 8); }
    void raw(std::string_view b) { out_.append(b.data(), b.size()); }
    template <std::size_t N>
    void fixed(const std::array<std::uint8_t, N>& a) { raw(view(a)); }
    /* u32 length, then the bytes. */
    void blob(std::string_view b) {
        u32(static_cast<std::uint32_t>(b.size()));
        raw(b);
    }
    const Bytes& bytes() const { return out_; }
    Bytes take() { return std::move(out_); }

private:
    void put(std::uint64_t v, int n) {
        for (int i = 0; i < n; ++i) out_.push_back(static_cast<char>((v >> (8 * i)) & 0xff));
    }
    Bytes out_;
};

class Reader {
public:
    explicit Reader(std::string_view in) : in_(in) {}

    std::uint8_t u8() { return static_cast<std::uint8_t>(get(1)); }
    std::uint16_t u16() { return static_cast<std::uint16_t>(get(2)); }
    std::uint32_t u32() { return static_cast<std::uint32_t>(get(4)); }
    std::int64_t i64() { return static_cast<std::int64_t>(get(8)); }

    std::string_view raw(std::size_t n) {
        if (failed_ || in_.size() - pos_ < n) {
            failed_ = true;
            return {};
        }
        std::string_view v = in_.substr(pos_, n);
        pos_ += n;
        return v;
    }
    template <std::size_t N>
    std::array<std::uint8_t, N> fixed() {
        std::array<std::uint8_t, N> a{};
        std::string_view v = raw(N);
        if (!failed_)
            for (std::size_t i = 0; i < N; ++i) a[i] = static_cast<std::uint8_t>(v[i]);
        return a;
    }
    /* A u32-length blob, refused if longer than `ceiling` (readers refuse, formats.md). */
    std::string_view blob(std::size_t ceiling) {
        const std::uint32_t n = u32();
        if (n > ceiling) failed_ = true;
        return raw(failed_ ? 0 : n);
    }

    std::size_t position() const { return pos_; }
    bool failed() const { return failed_; }
    /* True only if nothing failed and every byte was read. */
    bool finish() const { return !failed_ && pos_ == in_.size(); }

private:
    std::uint64_t get(int n) {
        std::string_view v = raw(static_cast<std::size_t>(n));
        std::uint64_t out = 0;
        if (!failed_)
            for (int i = 0; i < n; ++i) out |= static_cast<std::uint64_t>(static_cast<std::uint8_t>(v[i])) << (8 * i);
        return out;
    }
    std::string_view in_;
    std::size_t pos_ = 0;
    bool failed_ = false;
};

} // namespace verguenza
