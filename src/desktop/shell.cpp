#include "desktop/shell.hpp"

#include "imgui.h"

#include <GLFW/glfw3.h>

namespace verguenza::desktop {

void Shell::ok(const std::string& message) {
    status = message;
    status_is_error = false;
}

void Shell::report(const Status& st, const std::string& success_message) {
    status = st ? success_message : st.error();
    status_is_error = !st;
}

void Shell::lock() {
    revealed.clear();
    revealed_id.clear();
    selected.clear();
    adding = setting_value = false;
    vault.lock();
    ok("Locked.");
}

namespace {

bool any_input(const ImGuiIO& io) {
    if (io.MouseDelta.x != 0 || io.MouseDelta.y != 0 || io.MouseWheel != 0) return true;
    for (bool down : io.MouseDown)
        if (down) return true;
    return io.InputQueueCharacters.Size > 0 || ImGui::IsAnyItemActive();
}

void draw_top_bar(Shell& s) {
    ImGui::TextUnformatted(s.vault.vault_name().c_str());
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 60);
    if (ImGui::Button("Lock")) s.lock();
    if (ImGui::BeginTabBar("tabs")) {
        if (ImGui::BeginTabItem("Secrets")) {
            s.tab = Tab::secrets;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Audit")) {
            s.tab = Tab::audit;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Log")) {
            s.tab = Tab::log;
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void draw_status_bar(Shell& s) {
    ImGui::Separator();
    if (s.status_is_error) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.45f, 0.4f, 1.0f));
    ImGui::TextWrapped("%s", s.status.c_str());
    if (s.status_is_error) ImGui::PopStyleColor();
}

} // namespace

void draw(Shell& s) {
    const double now = glfwGetTime();
    if (any_input(ImGui::GetIO())) s.last_activity = now;
    // auto-lock: idle too long (okf/concepts/sealing.md §"Lock")
    if (s.vault.unlocked() && now - s.last_activity > s.settings.idle_lock_seconds) {
        s.lock();
        s.ok("Locked after " + std::to_string(static_cast<int>(s.settings.idle_lock_seconds / 60)) + " idle minutes.");
    }
    s.vault.tick();

    const ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    ImGui::Begin("Void Verguenza", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);
    const float status_height = ImGui::GetTextLineHeightWithSpacing() * 2.5f;
    ImGui::BeginChild("body", ImVec2(0, -status_height));
    if (!s.vault.unlocked()) {
        draw_lock_screen(s);
    } else {
        draw_top_bar(s);
        switch (s.tab) {
        case Tab::secrets: draw_secrets_screen(s); break;
        case Tab::audit: draw_audit_screen(s); break;
        case Tab::log: draw_log_screen(s); break;
        }
    }
    ImGui::EndChild();
    draw_status_bar(s);
    ImGui::End();
}

} // namespace verguenza::desktop
