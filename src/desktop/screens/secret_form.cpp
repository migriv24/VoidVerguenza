/* The form for a new secret, or a new value for the selected one.
 *
 * Intake (okf/concepts/commands.md §"Intake"): the value box writes straight into a
 * SecretBuffer this file owns, which is wiped the moment the value is saved or the form
 * is closed. Dear ImGui keeps its own working copy of the text being edited, which
 * Verguenza cannot wipe; the roadmap's memory scan (1B.12) measures it before anything
 * is claimed about it. Description fields are ordinary strings: they open nothing. */
#include "desktop/shell.hpp"

#include "foundation/ports/secure_memory_port.hpp"
#include "foundation/secrets/generator.hpp"
#include "foundation/secrets/kinds.hpp"

#include "imgui.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <map>

namespace verguenza::desktop {

namespace {

constexpr std::size_t kIntakeCapacity = 8192;

SecretBuffer g_intake;    // the value being typed: plaintext, locked memory
bool g_show_value = false;
std::map<std::string, std::array<char, 512>> g_fields;
secrets::GeneratorOptions g_gen;

SecretBuffer& intake() {
    if (g_intake.capacity() != kIntakeCapacity) g_intake = SecretBuffer(kIntakeCapacity); // zeroed
    return g_intake;
}

void close_form(Shell& s) {
    g_intake.clear(); // wiped
    g_fields.clear();
    g_show_value = false;
    s.adding = s.setting_value = false;
}

char* field(const std::string& name) {
    auto& f = g_fields[name];
    return f.data();
}

} // namespace

/* Generating needs randomness, which the vault service reaches through its crypto port;
 * the shell asks the service rather than holding a port of its own. */
void draw_secret_form(Shell& s) {
    const bool new_secret = s.adding;
    ImGui::TextUnformatted(new_secret ? "New secret" : "New value");
    ImGui::Separator();

    const secrets::KindSpec* kind = secrets::find_kind(s.form_kind);
    if (new_secret) {
        if (ImGui::BeginCombo("Kind", kind ? kind->label.c_str() : s.form_kind.c_str())) {
            for (const auto& k : secrets::kinds())
                if (ImGui::Selectable(k.label.c_str(), k.glyph == s.form_kind)) s.form_kind = k.glyph;
            ImGui::EndCombo();
        }
        kind = secrets::find_kind(s.form_kind);
        ImGui::InputText("Title", field("title"), 512);
        if (kind)
            for (const auto& f : kind->fields) ImGui::InputText(f.label.c_str(), field(f.name), 512);
        ImGui::InputTextMultiline("Notes", field("notes"), 512, ImVec2(0, ImGui::GetTextLineHeight() * 3));
    } else if (auto d = s.vault.describe(s.selected)) {
        kind = secrets::find_kind(d->kind);
        ImGui::TextDisabled("for %s", d->field("title").c_str());
    }

    // ── the value: intake ──────────────────────────────────────────────────
    ImGui::Dummy(ImVec2(0, 6));
    const char* value_label = kind ? kind->value_label.c_str() : "Value";
    SecretBuffer& in = intake();
    if (kind && kind->multiline) {
        // plaintext: typed straight into the locked intake buffer
        ImGui::InputTextMultiline(value_label, in.chars(), in.capacity(), ImVec2(0, ImGui::GetTextLineHeight() * 6));
    } else {
        // plaintext: typed straight into the locked intake buffer, masked unless asked
        ImGui::InputText(value_label, in.chars(), in.capacity(), g_show_value ? 0 : ImGuiInputTextFlags_Password);
        ImGui::SameLine();
        ImGui::Checkbox("Show", &g_show_value);
    }
    if (ImGui::TreeNode("Generate a password")) {
        int len = static_cast<int>(g_gen.length);
        ImGui::SliderInt("Length", &len, 8, 64);
        g_gen.length = static_cast<std::size_t>(len);
        ImGui::Checkbox("a-z", &g_gen.lower);
        ImGui::SameLine();
        ImGui::Checkbox("A-Z", &g_gen.upper);
        ImGui::SameLine();
        ImGui::Checkbox("2-9", &g_gen.digits);
        ImGui::SameLine();
        ImGui::Checkbox("!#$", &g_gen.symbols);
        if (ImGui::Button("Generate")) {
            // plaintext: made by the service's generator, copied into intake, then wiped
            SecretBuffer pw;
            Status st = s.vault.generate_password(g_gen, pw);
            if (st) {
                wipe(in.data(), in.capacity());
                std::memcpy(in.data(), pw.data(), std::min(pw.size(), in.capacity() - 1));
                s.ok("Generated. It is not saved until you press Save.");
            } else {
                s.report(st, "");
            }
        }
        ImGui::TreePop();
    }

    // ── save / cancel ──────────────────────────────────────────────────────
    ImGui::Dummy(ImVec2(0, 6));
    if (ImGui::Button("Save", ImVec2(120, 0))) {
        // plaintext: only the typed length is copied into the value handed to the service
        SecretBuffer value = SecretBuffer::copy_of(in.data(), in.c_string_length());
        Status st = Status::ok();
        std::string id;
        if (new_secret) {
            std::map<std::string, std::string> fields;
            for (auto& [name, buf] : g_fields)
                if (buf[0]) fields[name] = buf.data();
            if (fields.count("title") == 0) fields["title"] = kind ? kind->label : "Secret";
            st = s.vault.add_secret(s.form_kind, fields, value, &id);
        } else {
            st = s.vault.set_value(s.selected, value);
        }
        s.report(st, new_secret ? "Saved, sealed to this device." : "New value saved as the next version.");
        if (st) {
            if (new_secret) s.selected = id;
            close_form(s);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0))) close_form(s);
}

} // namespace verguenza::desktop
