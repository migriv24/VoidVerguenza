/* The audit screen (every reveal and copy, from the signed record: okf/concepts/audit.md)
 * and the log strip (every dispatcher command, the family's total observability). Neither
 * ever shows a value: acts name secrets, and commands never carry values. */
#include "desktop/shell.hpp"

#include "imgui.h"

#include <ctime>

namespace verguenza::desktop {

namespace {
std::string when(std::int64_t ms) {
    const std::time_t t = static_cast<std::time_t>(ms / 1000);
    char buf[32];
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::strftime(buf, sizeof buf, "%Y-%m-%d %H:%M:%S", &tm);
    return buf;
}
} // namespace

void draw_audit_screen(Shell& s) {
    ImGui::TextDisabled("Every reveal and copy, as signed objects in the vault's record. Written before the value "
                        "was shown; a reveal that could not be recorded did not happen.");
    ImGui::Separator();
    if (ImGui::BeginTable("audit", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("When");
        ImGui::TableSetupColumn("Who");
        ImGui::TableSetupColumn("What");
        ImGui::TableSetupColumn("Secret");
        ImGui::TableHeadersRow();
        const auto lines = s.vault.audit();
        for (auto it = lines.rbegin(); it != lines.rend(); ++it) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(when(it->time).c_str());
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(it->who.c_str());
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(it->what.c_str());
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(it->secret.c_str());
        }
        ImGui::EndTable();
    }
}

void draw_log_screen(Shell& s) {
    ImGui::TextDisabled("Every command sent to Void Core. Commands carry descriptions, never values.");
    ImGui::Separator();
    ImGui::BeginChild("log");
    for (const auto& line : s.log) ImGui::TextUnformatted(line.c_str());
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();
}

} // namespace verguenza::desktop
