/* The lock screen: nothing of the vault, not even names, until it is unlocked
 * (okf/concepts/application.md). It states, in words, what protects this device's key
 * (ground rule 6: honest about what enforces what). */
#include "desktop/shell.hpp"

#include "imgui.h"

namespace verguenza::desktop {

namespace {
char g_vault_name[128] = "Our vault";
char g_person_name[128] = "";
} // namespace

void draw_lock_screen(Shell& s) {
    const bool exists = s.vault.vault_exists(s.settings.vault_path);
    ImGui::Dummy(ImVec2(0, 40));
    ImGui::Indent(40);
    ImGui::TextUnformatted("Void Verguenza");
    ImGui::TextDisabled("Secrets that several people share.");
    ImGui::Dummy(ImVec2(0, 16));
    ImGui::TextDisabled("Vault: %s", s.settings.vault_path.c_str());
    ImGui::Dummy(ImVec2(0, 8));

    if (exists) {
        if (ImGui::Button("Unlock", ImVec2(160, 0))) s.report(s.vault.unlock(s.settings.vault_path), "Unlocked.");
    } else {
        ImGui::TextUnformatted("No vault here yet. Create one:");
        ImGui::SetNextItemWidth(320);
        ImGui::InputText("Vault name", g_vault_name, sizeof g_vault_name);
        ImGui::SetNextItemWidth(320);
        ImGui::InputText("Your name", g_person_name, sizeof g_person_name);
        if (ImGui::Button("Create vault", ImVec2(160, 0)))
            s.report(s.vault.create(s.settings.vault_path, g_vault_name, g_person_name, s.settings.device_name),
                     "Vault created. This device's key is kept by Windows for your account.");
    }

    ImGui::Dummy(ImVec2(0, 24));
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 560);
    ImGui::TextDisabled("What protects this device's key");
    ImGui::TextWrapped("%s", s.vault.protection().c_str());
    ImGui::Dummy(ImVec2(0, 8));
    ImGui::TextDisabled("Randomness: Reticulum's generator, seeded from %s.", s.settings.entropy_source.c_str());
    ImGui::TextDisabled("Values in memory: %s.", s.settings.memory_protection.c_str());
    ImGui::PopTextWrapPos();
    ImGui::Unindent(40);
}

} // namespace verguenza::desktop
