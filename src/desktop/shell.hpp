/* desktop/shell.hpp — the window's state between frames, and which screen is showing.
 *
 * Thin on purpose (the author: "we likely don't need to do too much for the desktop
 * application that uses void maiz"). Every rule lives in application/vault_service; the
 * shell only draws what the service says and passes on what a person does. View state
 * here is never a source of truth. */
#pragma once

#include "application/vault_service.hpp"
#include "foundation/secret/secret_buffer.hpp"

#include <deque>
#include <string>

namespace verguenza::desktop {

struct Settings {
    std::string vault_path;
    std::string device_name;
    std::string entropy_source;   // which OS generator seeds Reticulum's
    std::string memory_protection;
    double idle_lock_seconds = 300;
    std::int64_t clipboard_clear_ms = 30000;
};

enum class Tab { secrets, audit, log };

struct Shell {
    explicit Shell(app::VaultService& v) : vault(v) {}

    app::VaultService& vault;
    Settings settings;

    Tab tab = Tab::secrets;
    std::string selected;       // the selected secret's id
    std::string search;
    std::string status;         // the last result, shown in the status bar
    bool status_is_error = false;
    double last_activity = 0;   // seconds, glfwGetTime()
    std::deque<std::string> log;

    // the add / edit form
    bool adding = false;
    bool setting_value = false; // replacing the selected secret's value
    std::string form_kind = "password";

    // a held reveal: the plaintext lives here only while the button is held
    SecretBuffer revealed;
    std::string revealed_id;

    void ok(const std::string& message);
    void report(const Status& st, const std::string& success_message);
    void lock();
};

/* One frame of the whole window. */
void draw(Shell& shell);

// the screens (desktop/screens/*.cpp)
void draw_lock_screen(Shell& shell);
void draw_secrets_screen(Shell& shell);
void draw_secret_form(Shell& shell);
void draw_audit_screen(Shell& shell);
void draw_log_screen(Shell& shell);

} // namespace verguenza::desktop
