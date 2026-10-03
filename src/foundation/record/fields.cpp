#include "foundation/record/fields.hpp"

#include "foundation/core/codec.hpp"

namespace verguenza {

namespace {
constexpr std::size_t kFieldCeiling = 1u << 20; // 1 MiB per field; bodies are small
}

Fields& Fields::set_u32(std::uint8_t tag, std::uint32_t v) {
    Writer w;
    w.u32(v);
    return set(tag, w.bytes());
}

std::string_view Fields::get(std::uint8_t tag) const {
    auto it = values_.find(tag);
    return it == values_.end() ? std::string_view{} : std::string_view(it->second);
}

std::optional<std::uint32_t> Fields::get_u32(std::uint8_t tag) const {
    if (!has(tag)) return std::nullopt;
    Reader r(get(tag));
    const std::uint32_t v = r.u32();
    if (!r.finish()) return std::nullopt;
    return v;
}

Bytes Fields::encode() const {
    Writer w;
    for (const auto& [tag, value] : values_) {
        w.u8(tag);
        w.blob(value);
    }
    return w.take();
}

Status Fields::decode(std::string_view in, Fields& out) {
    Fields f;
    Reader r(in);
    int last = -1;
    while (!r.failed() && r.position() < in.size()) {
        const std::uint8_t tag = r.u8();
        std::string_view value = r.blob(kFieldCeiling);
        if (r.failed()) break;
        if (tag <= last) return Status::fail("a body's fields are out of order or repeated (not canonical)");
        last = tag;
        f.values_[tag] = Bytes(value);
    }
    if (!r.finish()) return Status::fail("a body is truncated or has trailing bytes");
    out = std::move(f);
    return Status::ok();
}

} // namespace verguenza
