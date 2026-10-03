#include "application/vault_service.hpp"

#include "foundation/secrets/kinds.hpp"
#include "foundation/sealing/envelope.hpp"

#include <algorithm>

namespace verguenza::app {

namespace {

std::string json_string(const std::string& s) {
    std::string out = "\"";
    for (char c : s) {
        if (c == '"' || c == '\\') out += '\\';
        out += c;
    }
    return out + "\"";
}

/* A glyph declaration per kind, so the document describes itself (Void Core 0.2.14:
 * declared glyphs travel with the document). Description fields only. */
std::string glyph_json(const secrets::KindSpec& k) {
    std::string fields;
    for (const auto& f : secrets::all_field_names(k)) fields += (fields.empty() ? "" : ",") + json_string(f);
    return "{\"glyph\":" + json_string(k.glyph) + ",\"label\":" + json_string(k.label) +
           ",\"editor\":\"form\",\"fields\":[" + fields + "]}";
}

} // namespace

VaultService::VaultService(Ports ports) : ports_(ports) {}
VaultService::~VaultService() { lock(); }

Status VaultService::require_unlocked() const {
    return unlocked_ ? Status::ok() : Status::fail("the vault is locked");
}

// ── the vault ───────────────────────────────────────────────────────────────

Status VaultService::create(const std::string& path, const std::string& vault_name, const std::string& person_name,
                            const std::string& device_name) {
    // refusals first: a create that is refused must not lock a vault that is open
    if (vault_name.empty() || person_name.empty()) return Status::fail("a vault needs a name and a person");
    if (ports_.store.exists(path)) return Status::fail("a vault already exists at " + path);
    lock();

    // this device's identity: made once, kept by the operating system's key store
    SecretBuffer key;
    if (ports_.keys.has_identity()) {
        if (Status st = ports_.keys.load_identity(key); !st) return st;
    } else {
        if (!ports_.crypto.generate_identity(key, public_key_)) return Status::fail("could not make this device's identity");
        if (Status st = ports_.keys.save_identity(key); !st) return st;
    }
    if (!ports_.crypto.public_key_of(key, public_key_)) return Status::fail("this device's identity is not usable");

    Record record(ports_.crypto);
    if (Status st = Record::found(vault_name, person_name, device_name, key, ports_.clock.now_ms(), ports_.crypto, record);
        !st)
        return st;

    device_key_ = std::move(key);
    me_ = ports_.crypto.device_id(public_key_);
    record_.emplace(std::move(record));
    path_ = path;
    vault_name_ = vault_name;
    unlocked_ = true;

    if (Status st = ports_.model.reset(""); !st) return st;
    if (Status st = prepare_model(); !st) {
        lock();
        return st;
    }
    if (Status st = save(); !st) {
        lock();
        return st;
    }
    return Status::ok();
}

Status VaultService::prepare_model() {
    auto& m = ports_.model;
    if (Status st = m.dispatch("config set actor verguenza"); !st) return st;
    if (Status st = m.dispatch(std::string("mantle new ") + kSecretsMantle); !st) return st;
    for (const auto& k : secrets::kinds())
        if (Status st = m.dispatch("glyph declare " + m.quote(glyph_json(k))); !st) return st;
    return Status::ok();
}

Status VaultService::unlock(const std::string& path) {
    lock();
    if (!ports_.store.exists(path)) return Status::fail("no vault at " + path);
    if (!ports_.keys.has_identity()) return Status::fail("this device has no identity yet: create a vault first");

    VaultFile file;
    if (Status st = ports_.store.load(path, file); !st) return st;

    SecretBuffer key;
    if (Status st = ports_.keys.load_identity(key); !st) return st;
    if (!ports_.crypto.public_key_of(key, public_key_)) return Status::fail("this device's identity is not usable");
    const DeviceId me = ports_.crypto.device_id(public_key_);

    Record record(ports_.crypto);
    for (const auto& [addr, bytes] : file.objects) (void)record.add(bytes); // refusals are kept as evidence
    if (!record.founded()) return Status::fail("the vault file holds no valid founding entry");
    if (!record.device(me)) return Status::fail("this device is not one of this vault's devices");

    if (Status st = ports_.model.reset(file.state_json); !st) return st;
    if (Status st = ports_.model.dispatch(std::string("use ") + kSecretsMantle); !st) return st;

    device_key_ = std::move(key);
    me_ = me;
    vault_name_ = record.vault_name();
    record_.emplace(std::move(record));
    path_ = path;
    unlocked_ = true;
    return Status::ok();
}

void VaultService::lock() {
    if (clipboard_deadline_) {
        ports_.clipboard.clear_if_unchanged();
        clipboard_deadline_ = 0;
    }
    device_key_.clear();
    record_.reset();
    if (unlocked_) (void)ports_.model.reset(""); // nothing of the vault stays on screen
    unlocked_ = false;
    vault_name_.clear();
    path_.clear();
}

Status VaultService::save() {
    VaultFile f;
    f.state_json = ports_.model.export_state();
    f.objects = record_->all_bytes();
    return ports_.store.save(path_, f);
}

// ── secrets ─────────────────────────────────────────────────────────────────

bool VaultService::find_secret(const std::string& id, Id16& out) const { return from_hex(id, out); }

std::vector<Description> VaultService::list() {
    if (!unlocked_) return {};
    std::vector<Description> out;
    for (auto& d : ports_.model.descriptions(kSecretsMantle)) {
        Id16 sid{};
        if (find_secret(d.id, sid) && record_->retired(sid)) continue; // the record wins over the document
        out.push_back(std::move(d));
    }
    std::sort(out.begin(), out.end(),
              [](const Description& a, const Description& b) { return a.field("title") < b.field("title"); });
    return out;
}

std::optional<Description> VaultService::describe(const std::string& id) {
    for (auto& d : list())
        if (d.id == id) return d;
    return std::nullopt;
}

Status VaultService::set_field(const std::string& id, const std::string& field, const std::string& value) {
    auto& m = ports_.model;
    return m.dispatch("set " + m.quote(id) + " " + field + " " + m.quote(value));
}

// value: plaintext from intake, read only; sealed to this device's identity.
Status VaultService::add_secret(const std::string& kind, const std::map<std::string, std::string>& fields,
                                const SecretBuffer& value, std::string* id_out) {
    if (Status st = require_unlocked(); !st) return st;
    const secrets::KindSpec* spec = secrets::find_kind(kind);
    if (!spec) return Status::fail("unknown kind of secret: " + kind);
    if (value.empty()) return Status::fail("a secret needs a value");

    EnvelopeHeader h;
    h.vault = record_->vault_id();
    if (!ports_.crypto.random(h.secret.data(), h.secret.size())) return Status::fail("no random bytes for the secret's id");
    h.version = 1;

    Envelope env;
    if (Status st = seal_value(h, value, {{me_, public_key_}}, record_->heads(), record_->last_of(me_), me_, device_key_,
                               ports_.crypto, env);
        !st)
        return st;
    Digest addr{};
    if (Status st = record_->add(encode(env), &addr); !st) return st;

    const std::string id = to_hex(h.secret);
    auto& m = ports_.model;
    if (Status st = m.dispatch(std::string("use ") + kSecretsMantle); !st) return st;
    if (Status st = m.dispatch("rune new " + kind + " " + m.quote(id)); !st) return st;
    for (const auto& [field, text] : fields)
        if (Status st = set_field(id, field, text); !st) return st;
    if (Status st = set_field(id, "envelope", "sha256:" + to_hex(addr)); !st) return st; // display only (trust.md)
    if (Status st = set_field(id, "version", "1"); !st) return st;
    if (id_out) *id_out = id;
    return save();
}

// value: plaintext from intake, read only; sealed as the next version.
Status VaultService::set_value(const std::string& id, const SecretBuffer& value) {
    if (Status st = require_unlocked(); !st) return st;
    Id16 sid{};
    if (!find_secret(id, sid)) return Status::fail("not a secret id: " + id);
    auto current = record_->current_envelope(sid);
    if (!current) return Status::fail("this secret has no value in the record");
    if (value.empty()) return Status::fail("a secret needs a value");

    EnvelopeHeader h = current->header;
    h.version += 1;
    Envelope env;
    if (Status st = seal_value(h, value, {{me_, public_key_}}, record_->heads(), record_->last_of(me_), me_, device_key_,
                               ports_.crypto, env);
        !st)
        return st;
    Digest addr{};
    if (Status st = record_->add(encode(env), &addr); !st) return st;
    if (Status st = set_field(id, "envelope", "sha256:" + to_hex(addr)); !st) return st;
    if (Status st = set_field(id, "version", std::to_string(h.version)); !st) return st;
    return save();
}

Status VaultService::set_fields(const std::string& id, const std::map<std::string, std::string>& fields) {
    if (Status st = require_unlocked(); !st) return st;
    for (const auto& [field, text] : fields) {
        if (field == "envelope" || field == "version") return Status::fail("'" + field + "' is kept by the record");
        if (Status st = set_field(id, field, text); !st) return st;
    }
    return save();
}

Status VaultService::remove(const std::string& id) {
    if (Status st = require_unlocked(); !st) return st;
    Id16 sid{};
    if (!find_secret(id, sid)) return Status::fail("not a secret id: " + id);
    // the removal is a signed object, so a merge cannot quietly bring the secret back
    if (Status st = record_->append(ObjectType::retire, Fields().set(act_field::secret, sid), ports_.clock.now_ms(), me_,
                                    device_key_);
        !st)
        return st;
    if (Status st = ports_.model.dispatch("rm " + ports_.model.quote(id)); !st) return st;
    return save();
}

// out: receives the plaintext value, only after the act is recorded and saved.
Status VaultService::reveal(const std::string& id, SecretBuffer& out, const std::string& how) {
    if (Status st = require_unlocked(); !st) return st;
    Id16 sid{};
    if (!find_secret(id, sid)) return Status::fail("not a secret id: " + id);
    bool conflict = false;
    auto env = record_->current_envelope(sid, &conflict);
    if (!env) return Status::fail("this secret has no value in the record");
    if (conflict) return Status::fail("two different values share this secret's newest version: settle it by setting a new one");

    // recorded, then shown (commands.md §"Reveal"): no record, no reveal
    if (Status st = record_->append(ObjectType::act, Fields().set(act_field::secret, sid).set(act_field::how, how),
                                    ports_.clock.now_ms(), me_, device_key_);
        !st)
        return Status::fail("the reveal could not be recorded, so it was not done: " + st.error());
    if (Status st = save(); !st)
        return Status::fail("the reveal could not be saved to the record, so it was not done: " + st.error());

    return open_value(*env, me_, device_key_, ports_.crypto, out);
}

// The value passes through a local SecretBuffer onto the clipboard and is wiped here.
Status VaultService::copy(const std::string& id, std::int64_t clear_after_ms) {
    SecretBuffer value;
    if (Status st = reveal(id, value, "copy"); !st) return st;
    if (Status st = ports_.clipboard.put_secret(value); !st) return st;
    clipboard_deadline_ = ports_.clock.now_ms() + clear_after_ms;
    return Status::ok();
}

// out: receives the generated plaintext password.
Status VaultService::generate_password(const secrets::GeneratorOptions& options, SecretBuffer& out) {
    return secrets::generate_password(options, ports_.crypto, out);
}

void VaultService::tick() {
    if (clipboard_deadline_ && ports_.clock.now_ms() >= clipboard_deadline_) {
        ports_.clipboard.clear_if_unchanged();
        clipboard_deadline_ = 0;
    }
}

std::vector<AuditLine> VaultService::audit() {
    std::vector<AuditLine> out;
    if (!unlocked_) return out;
    std::map<std::string, std::string> titles;
    for (const auto& d : ports_.model.descriptions(kSecretsMantle)) titles[d.id] = d.field("title");
    for (const auto& a : record_->acts()) {
        AuditLine line;
        line.time = a.time;
        if (auto dev = record_->device(a.author)) line.who = dev->person_name + " on " + dev->name;
        line.what = a.how == "copy" ? "copied" : a.how == "show" ? "revealed" : a.how;
        const std::string id = to_hex(a.secret);
        line.secret = titles.count(id) && !titles[id].empty() ? titles[id] : id;
        out.push_back(line);
    }
    return out;
}

} // namespace verguenza::app
