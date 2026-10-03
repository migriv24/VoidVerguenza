/* desktop/main.cpp — the composition root: every adapter is made here and handed to the
 * vault service as its ports (okf/concepts/architecture.md). The only file that knows
 * which adapter meets which port. */
#include "adapters/model/core_model.hpp"
#include "adapters/platform/system_clock.hpp"
#include "adapters/platform/win32/clipboard.hpp"
#include "adapters/platform/win32/dpapi_key_store.hpp"
#include "adapters/platform/win32/secure_memory.hpp"
#include "adapters/reticulum/reticulum_crypto.hpp"
#include "adapters/storage/archive_vault_store.hpp"
#include "application/vault_service.hpp"
#include "desktop/shell.hpp"

#include "voidmaiz/glhost.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>

namespace {

std::filesystem::path data_folder() {
    const char* appdata = std::getenv("APPDATA");
    std::filesystem::path p = appdata ? appdata : ".";
    return p / "VoidVerguenza";
}

std::string device_name() {
    const char* name = std::getenv("COMPUTERNAME");
    return name ? name : "this computer";
}

} // namespace

int main() {
    using namespace verguenza;

    // ── ports, met ─────────────────────────────────────────────────────────
    adapters::Win32SecureMemory memory;
    install_secure_memory(&memory); // before any SecretBuffer exists

    adapters::ReticulumCrypto crypto;
    if (std::string(adapters::ReticulumCrypto::entropy_source()).empty()) {
        std::fprintf(stderr, "no operating-system entropy source: Void Verguenza will not make keys without one\n");
        return 1;
    }
    const std::filesystem::path folder = data_folder();
    adapters::ArchiveVaultStore store;
    adapters::DpapiKeyStore keys((folder / "device.identity").make_preferred().string());
    adapters::Win32Clipboard clipboard;
    adapters::CoreModel model;
    adapters::SystemClock clock;
    app::VaultService vault(app::Ports{crypto, store, keys, clipboard, model, clock});

    desktop::Shell shell(vault);
    shell.settings.vault_path = (folder / "default.verguenza").make_preferred().string();
    shell.settings.device_name = device_name();
    shell.settings.entropy_source = adapters::ReticulumCrypto::entropy_source();
    shell.settings.memory_protection = memory.description();
    model.set_log([&](const std::string& line) {
        shell.log.push_back(line);
        if (shell.log.size() > 2000) shell.log.pop_front();
    });

    // ── the window: Void Maiz's substrate (GLFW + OpenGL 3 + Dear ImGui) ──────
    glfwSetErrorCallback([](int code, const char* desc) { std::fprintf(stderr, "glfw error %d: %s\n", code, desc); });
    if (!glfwInit()) return 1;
    const char* glsl = maiz::gl_context_hints();
    GLFWwindow* window = glfwCreateWindow(1100, 700, "Void Verguenza", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr; // no layout file: nothing of the vault is written beside it
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
            // minimized: lock, so a hidden window holds no keys
            if (vault.unlocked()) shell.lock();
            glfwWaitEventsTimeout(0.5);
            continue;
        }
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        desktop::draw(shell);
        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.09f, 0.09f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    shell.lock(); // wipe before exit
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
