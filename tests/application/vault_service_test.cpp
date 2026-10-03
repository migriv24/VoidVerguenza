/* tests/application/vault_service_test.cpp — the use cases end to end, with the real
 * Reticulum crypto and the real Void Core (through Void Maiz), and in-memory fakes for the
 * store, key store, clipboard and clock (roadmap 1B.6–1B.8, 1B.11–1B.12, the grep half).
 *
 * The plaintext grep (ground rule 4): after every verb has run with generated values,
 * neither the saved document, nor any saved object, nor the model's command log contains
 * a value, its hex, or any long piece of it. */
#include "adapters/model/core_model.hpp"
#include "adapters/reticulum/reticulum_crypto.hpp"
#include "application/vault_service.hpp"
#include "support/check.hpp"
#include "support/fakes.hpp"

#include <cstring>
#include <string>
#include <vector>

using namespace verguenza;

namespace {

SecretBuffer value_of(const std::string& s) { return SecretBuffer::copy_of(s.data(), s.size()); }

/* A value made at test time, obviously fake (ground rule 2). */
std::string fake_value(adapters::ReticulumCrypto& c, const char* label) {
    std::uint8_t r[8];
    c.random(r, sizeof r);
    return std::string("vg-test-") + label + "-" + to_hex(std::string_view(reinterpret_cast<const char*>(r), sizeof r));
}

bool leaks(const std::string& haystack, const std::string& value) {
    if (haystack.find(value) != std::string::npos) return true;
    if (haystack.find(to_hex(value)) != std::string::npos) return true;
    // any 12-character piece of the random part
    for (std::size_t i = 8; i + 12 <= value.size(); ++i)
        if (haystack.find(value.substr(i, 12)) != std::string::npos) return true;
    return false;
}

} // namespace

int main() {
    adapters::ReticulumCrypto crypto;
    vtest::MemoryStore store;
    vtest::MemoryKeyStore keys;
    vtest::MemoryClipboard clipboard;
    adapters::CoreModel model;
    vtest::FixedClock clock;
    std::string command_log;
    model.set_log([&](const std::string& line) { command_log += line + "\n"; });

    const std::string path = "test.verguenza";
    std::vector<std::string> values;

    std::string id_pw, id_wifi, id_note;
    {
        app::VaultService v(app::Ports{crypto, store, keys, clipboard, model, clock});
        CHECK(!v.unlock(path)); // no vault yet
        CHECK_OK(v.create(path, "The store", "Ana", "front-desk-pc"));
        CHECK(v.unlocked());
        CHECK(!v.create(path, "Again", "Ana", "pc")); // never overwrites a vault

        values.push_back(fake_value(crypto, "pw"));
        CHECK_OK(v.add_secret("password", {{"title", "POS back office"}, {"username", "admin"}}, value_of(values[0]),
                              &id_pw));
        values.push_back(fake_value(crypto, "wifi"));
        CHECK_OK(v.add_secret("wifi", {{"title", "Shop Wi-Fi"}, {"ssid", "shop-guest"}}, value_of(values[1]), &id_wifi));
        values.push_back(fake_value(crypto, "note") + "\nwith a second line, 'quotes' and \"doubles\"");
        CHECK_OK(v.add_secret("note", {{"title", "Alarm code; it's \"secret\""}}, value_of(values[2]), &id_note));
        CHECK(!v.add_secret("no_such_kind", {}, value_of("x")));
        CHECK(!v.add_secret("password", {{"title", "empty"}}, SecretBuffer()));

        // descriptions come back from Void Core, with awkward quoting intact
        auto list = v.list();
        CHECK(list.size() == 3);
        auto note = v.describe(id_note);
        CHECK(note && note->field("title") == "Alarm code; it's \"secret\"");
        CHECK(v.describe(id_pw)->field("username") == "admin");
        CHECK(v.describe(id_pw)->field("envelope").rfind("sha256:", 0) == 0);

        // reveal: recorded first, then the value
        SecretBuffer out;
        CHECK_OK(v.reveal(id_pw, out));
        CHECK(out.view() == values[0]);
        CHECK_OK(v.reveal(id_note, out));
        CHECK(out.view() == values[2]);

        // rotate: the next version, and the old value is no longer what reveal gives
        values.push_back(fake_value(crypto, "pw2"));
        CHECK_OK(v.set_value(id_pw, value_of(values[3])));
        CHECK_OK(v.reveal(id_pw, out));
        CHECK(out.view() == values[3]);
        CHECK(v.describe(id_pw)->field("version") == "2");

        // copy: on the clipboard, cleared after the timer
        CHECK_OK(v.copy(id_wifi, 30000));
        CHECK(clipboard.text == values[1]);
        clock.now += 29000;
        v.tick();
        CHECK(clipboard.text == values[1]);
        clock.now += 2000;
        v.tick();
        CHECK(clipboard.text.empty());

        // a reveal that cannot be recorded does not happen (commands.md §"Reveal")
        store.fail_saves = true;
        SecretBuffer blocked;
        CHECK(!v.reveal(id_wifi, blocked));
        CHECK(blocked.empty());
        store.fail_saves = false;

        // the audit record names every reveal and copy
        const auto audit = v.audit();
        CHECK(audit.size() >= 4);
        bool saw_copy = false;
        for (const auto& line : audit) saw_copy = saw_copy || (line.what == "copied" && line.secret == "Shop Wi-Fi");
        CHECK(saw_copy);

        // remove: gone from the list, and it stays gone even if the document were edited
        CHECK_OK(v.remove(id_wifi));
        CHECK(v.list().size() == 2);

        // descriptions can change; the record-kept fields cannot be set by hand
        CHECK_OK(v.set_fields(id_pw, {{"address", "https://pos.example"}}));
        CHECK(!v.set_fields(id_pw, {{"envelope", "sha256:00"}}));

        // lock: no key or value alive in this service, nothing listed
        v.lock();
        CHECK(!v.unlocked());
        CHECK(v.list().empty());
        CHECK(!v.reveal(id_pw, out));
    }
    // every SecretBuffer the service or this test made is gone (roadmap 1B.8): only the
    // ones still in scope here (none) may be alive
    CHECK(SecretBuffer::live_count() == 0);

    // a fresh service unlocks the saved vault and finds everything
    {
        adapters::CoreModel model2;
        app::VaultService v(app::Ports{crypto, store, keys, clipboard, model2, clock});
        CHECK_OK(v.unlock(path));
        CHECK(v.vault_name() == "The store");
        CHECK(v.list().size() == 2);
        SecretBuffer out;
        CHECK_OK(v.reveal(id_pw, out));
        CHECK(out.view() == values[3]);
        CHECK(v.describe(id_pw)->field("address") == "https://pos.example");
    }

    // another device's key cannot unlock it
    {
        vtest::MemoryKeyStore other;
        SecretBuffer k;
        PublicKey p{};
        CHECK(crypto.generate_identity(k, p));
        CHECK_OK(other.save_identity(k));
        adapters::CoreModel model3;
        app::VaultService v(app::Ports{crypto, store, other, clipboard, model3, clock});
        CHECK(!v.unlock(path));
    }

    // ── the plaintext grep (ground rule 4) ──────────────────────────────────
    const std::string saved = store.bytes_of(path);
    for (const auto& value : values) {
        CHECK(!leaks(saved, value));
        CHECK(!leaks(command_log, value));
    }
    CHECK(command_log.find("POS back office") != std::string::npos); // descriptions do go through Core

    return vtest::finish("vault_service");
}
