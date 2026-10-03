/* The secrets screen: a searchable list on the left, the selected secret on the right.
 * A value is shown only while "Hold to reveal" is held (okf/concepts/application.md). */
#include "desktop/shell.hpp"

#include "foundation/secrets/kinds.hpp"

#include "imgui.h"

#include <algorithm>
#include <cctype>

namespace verguenza::desktop {

namespace {

std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

bool matches(const app::Description& d, const std::string& query) {
    if (query.empty()) return true;
    const std::string q = lower(query);
    for (const auto& [k, v] : d.fields)
        if (k != "envelope" && lower(v).find(q) != std::string::npos) return true;
    return lower(d.kind).find(q) != std::string::npos;
}

void draw_list(Shell& s, const std::vector<app::Description>& all) {
    static char search[128] = "";
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##search", "Search", search, sizeof search);
    s.search = search;
    if (ImGui::Button("+ New secret", ImVec2(-1, 0))) {
        s.adding = true;
        s.setting_value = false;
    }
    ImGui::Separator();
    for (const auto& d : all) {
        if (!matches(d, s.search)) continue;
        const auto* kind = secrets::find_kind(d.kind);
        const std::string title = d.field("title").empty() ? "(untitled)" : d.field("title");
        const std::string label = title + "##" + d.id;
        if (ImGui::Selectable(label.c_str(), s.selected == d.id)) {
            s.selected = d.id;
            s.adding = false;
            s.setting_value = false;
        }
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 90);
        ImGui::TextDisabled("%s", kind ? kind->label.c_str() : d.kind.c_str());
    }
    if (all.empty()) ImGui::TextDisabled("No secrets yet.");
}

void draw_value_row(Shell& s, const app::Description& d) {
    const auto* kind = secrets::find_kind(d.kind);
    ImGui::TextDisabled("%s", kind ? kind->value_label.c_str() : "Value");

    ImGui::Button("Hold to reveal");
    if (ImGui::IsItemActivated()) {
        // recorded first, then opened (VaultService::reveal); the plaintext lives in
        // s.revealed only while the button is held
        Status st = s.vault.reveal(d.id, s.revealed, "show");
        if (st) s.revealed_id = d.id;
        else s.report(st, "");
    }
    const bool holding = ImGui::IsItemActive() && s.revealed_id == d.id;
    if (ImGui::IsItemDeactivated()) {
        s.revealed.clear(); // wiped the moment the button is released
        s.revealed_id.clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("Copy")) {
        s.report(s.vault.copy(d.id, s.settings.clipboard_clear_ms),
                 "Copied. The clipboard clears in " + std::to_string(s.settings.clipboard_clear_ms / 1000) +
                     " seconds if it still holds this value.");
    }

    // plaintext on screen: drawn straight from the locked buffer while held, never copied
    ImGui::BeginChild("value", ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 4), true);
    if (holding)
        ImGui::TextUnformatted(reinterpret_cast<const char*>(s.revealed.data()),
                               reinterpret_cast<const char*>(s.revealed.data()) + s.revealed.size());
    else
        ImGui::TextDisabled("(hidden: hold the button to see it)");
    ImGui::EndChild();
}

void draw_detail(Shell& s, const app::Description& d) {
    const auto* kind = secrets::find_kind(d.kind);
    ImGui::TextUnformatted(d.field("title").empty() ? "(untitled)" : d.field("title").c_str());
    ImGui::TextDisabled("%s", kind ? kind->label.c_str() : d.kind.c_str());
    ImGui::Separator();

    if (kind)
        for (const auto& f : kind->fields) {
            const std::string v = d.field(f.name);
            if (v.empty()) continue;
            ImGui::TextDisabled("%s", f.label.c_str());
            ImGui::TextWrapped("%s", v.c_str());
        }
    if (!d.field("notes").empty()) {
        ImGui::TextDisabled("Notes");
        ImGui::TextWrapped("%s", d.field("notes").c_str());
    }
    ImGui::Dummy(ImVec2(0, 8));
    draw_value_row(s, d);

    ImGui::Dummy(ImVec2(0, 8));
    if (ImGui::Button("Set a new value")) {
        s.setting_value = true;
        s.adding = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Remove")) ImGui::OpenPopup("remove?");
    if (ImGui::BeginPopupModal("remove?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextUnformatted("Remove this secret from the vault?");
        ImGui::TextDisabled("Its history stays in the signed record. Anyone who saw it still knows it.");
        if (ImGui::Button("Remove")) {
            s.report(s.vault.remove(d.id), "Removed.");
            s.selected.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    ImGui::Dummy(ImVec2(0, 8));
    ImGui::TextDisabled("Version %s · %s", d.field("version").c_str(), d.field("envelope").substr(0, 23).c_str());
}

} // namespace

void draw_secrets_screen(Shell& s) {
    const std::vector<app::Description> all = s.vault.list();
    ImGui::BeginChild("list", ImVec2(320, 0), true);
    draw_list(s, all);
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("detail", ImVec2(0, 0), true);
    if (s.adding || s.setting_value) {
        draw_secret_form(s);
    } else {
        auto it = std::find_if(all.begin(), all.end(), [&](const app::Description& d) { return d.id == s.selected; });
        if (it != all.end()) draw_detail(s, *it);
        else ImGui::TextDisabled("Select a secret, or add one.");
    }
    ImGui::EndChild();
}

} // namespace verguenza::desktop
