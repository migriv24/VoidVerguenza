#include "foundation/secrets/kinds.hpp"

namespace verguenza::secrets {

const std::vector<KindSpec>& kinds() {
    static const std::vector<KindSpec> k = {
        {"password", "Password", "Password", false,
         {{"username", "Username"}, {"address", "Address (URL or device)"}}},
        {"api_key", "API key", "Key", false,
         {{"provider", "Provider"}, {"account", "Account"}, {"scopes", "Scopes"}, {"expires", "Expires"}}},
        {"wifi", "Wi-Fi", "Passphrase", false, {{"ssid", "Network name (SSID)"}, {"security", "Security"}}},
        {"recovery", "Recovery codes", "Codes", true, {{"account", "For which account"}}},
        {"ssh_key", "SSH key", "Private key", true,
         {{"public_key", "Public key"}, {"host", "Host"}, {"user", "User"}}},
        {"pin", "PIN", "PIN", false, {{"where", "Where it is used"}}},
        {"note", "Secure note", "Note", true, {}},
    };
    return k;
}

const KindSpec* find_kind(std::string_view glyph) {
    for (const auto& k : kinds())
        if (k.glyph == glyph) return &k;
    return nullptr;
}

std::vector<std::string> all_field_names(const KindSpec& kind) {
    std::vector<std::string> out = {"title", "notes", "envelope", "version"};
    for (const auto& f : kind.fields) out.push_back(f.name);
    return out;
}

} // namespace verguenza::secrets
