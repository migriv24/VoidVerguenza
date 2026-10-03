/* foundation/secrets/kinds.hpp — the kinds of secret, and the description fields each
 * carries. okf/concepts/secret.md §"Kinds".
 *
 * A secret is two things: a DESCRIPTION every member can read (these fields, kept in
 * the Void Core document as a rune of the kind's glyph) and a VALUE that lives only in
 * an envelope. No field here ever holds the value. */
#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace verguenza::secrets {

struct FieldSpec {
    std::string name;  // the content key in the rune ("username")
    std::string label; // what the screen calls it ("Username")
};

struct KindSpec {
    std::string glyph;        // "password"
    std::string label;        // "Password"
    std::string value_label;  // what the value is called on screen ("Password", "Token")
    bool multiline = false;   // notes, recovery codes, keys
    std::vector<FieldSpec> fields;
};

const std::vector<KindSpec>& kinds();
const KindSpec* find_kind(std::string_view glyph);

/* Every description field any kind has, plus "title" and "notes": the glyph field list
 * Void Core is told about, so it never rejects a field. */
std::vector<std::string> all_field_names(const KindSpec& kind);

} // namespace verguenza::secrets
