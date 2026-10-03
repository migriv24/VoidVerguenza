#include "foundation/record/record.hpp"

#include <algorithm>

namespace verguenza {

namespace {

std::vector<Digest> links_of(const std::vector<Digest>& parents, const Digest& previous) {
    std::vector<Digest> out = parents;
    if (!is_zero(previous) && std::find(out.begin(), out.end(), previous) == out.end()) out.push_back(previous);
    return out;
}

bool is_founding(const SignedObject& o) { return o.type == ObjectType::roster && is_zero(o.vault); }

} // namespace

// device_key: read only, to sign the founding entry.
Status Record::found(const std::string& vault_name, const std::string& person_name, const std::string& device_name,
                     const SecretBuffer& device_key, std::int64_t time, CryptoPort& crypto, Record& out) {
    PublicKey pub{};
    if (!crypto.public_key_of(device_key, pub)) return Status::fail("this device's key is not usable");
    Id16 person{};
    if (!crypto.random(person.data(), person.size())) return Status::fail("no random bytes for the person's id");

    SignedObject o;
    o.type = ObjectType::roster;
    o.time = time;
    o.author = crypto.device_id(pub);
    o.body = Fields()
                 .set(roster_field::kind, "found")
                 .set(roster_field::vault_name, vault_name)
                 .set(roster_field::device_key, pub)
                 .set(roster_field::device_name, device_name)
                 .set(roster_field::person_id, person)
                 .set(roster_field::person_name, person_name)
                 .encode();
    if (Status st = sign(o, device_key, crypto); !st) return st;

    Record r(crypto);
    if (Status st = r.add(encode(o)); !st) return st;
    out = std::move(r);
    return Status::ok();
}

Status Record::add(std::string_view bytes, Digest* address) {
    const Digest addr = crypto_->sha256(bytes);
    if (address) *address = addr;
    if (entries_.count(addr) || pending_.count(addr)) return Status::ok(); // already held: adding twice is free

    Entry e;
    e.bytes = Bytes(bytes);
    if (bytes.substr(0, 4) == "VGE1") {
        e.is_envelope = true;
        if (Status st = decode(bytes, e.envelope); !st) return st;
        e.author = e.envelope.author;
        e.ancestry_links = links_of(e.envelope.parents, e.envelope.previous);
    } else {
        if (Status st = decode(bytes, e.object); !st) return st;
        e.author = e.object.author;
        e.ancestry_links = links_of(e.object.parents, e.object.previous);
    }

    // held, not judged, until every parent is here (the history graph's own rule)
    for (const Digest& p : e.ancestry_links)
        if (!entries_.count(p)) {
            pending_.emplace(addr, std::move(e));
            return Status::ok();
        }
    Status st = admit(addr, std::move(e));
    retry_held();
    return st;
}

Status Record::admit(const Digest& address, Entry entry) {
    if (Status st = judge(address, entry); !st) {
        refused_.emplace(address, entry.bytes);
        return st;
    }
    if (!entry.is_envelope && is_founding(entry.object)) {
        Fields f;
        (void)Fields::decode(entry.object.body, f);
        vault_id_ = address;
        vault_name_ = std::string(f.get(roster_field::vault_name));
    }
    if (!entry.is_envelope && entry.object.type == ObjectType::roster) {
        Fields f;
        (void)Fields::decode(entry.object.body, f);
        DeviceInfo d;
        d.key = *f.get_fixed<64>(roster_field::device_key);
        d.id = crypto_->device_id(d.key);
        d.name = std::string(f.get(roster_field::device_name));
        if (auto p = f.get_fixed<16>(roster_field::person_id)) d.person = *p;
        d.person_name = std::string(f.get(roster_field::person_name));
        devices_[d.id] = d;
    }
    entries_.emplace(address, std::move(entry));
    return Status::ok();
}

/* Rule 1, membership only (see record.hpp): the founding entry is signed by the device
 * it admits; every other entry belongs to this vault and is signed by a device that a
 * valid roster entry among ITS OWN ancestors admitted. */
Status Record::judge(const Digest& address, const Entry& e) const {
    (void)address;
    if (!e.is_envelope && is_founding(e.object)) {
        if (founded()) return Status::fail("a second founding entry in one vault");
        Fields f;
        if (Status st = Fields::decode(e.object.body, f); !st) return st;
        if (f.get(roster_field::kind) != "found") return Status::fail("a roster entry with no vault is not a founding");
        auto key = f.get_fixed<64>(roster_field::device_key);
        if (!key) return Status::fail("the founding entry names no device key");
        if (crypto_->device_id(*key) != e.author) return Status::fail("the founding entry is not signed by the device it admits");
        if (!verify(e.object, *key, *crypto_)) return Status::fail("the founding entry's signature is not valid");
        return Status::ok();
    }

    const Digest& vault = e.is_envelope ? e.envelope.header.vault : e.object.vault;
    if (!founded() || vault != vault_id_) return Status::fail("an entry from another vault");

    // the device must be admitted by a roster entry in this entry's causal past
    const std::set<Digest> past = ancestors(e.ancestry_links);
    const PublicKey* key = nullptr;
    for (const Digest& a : past) {
        const Entry& anc = entries_.at(a);
        if (anc.is_envelope || anc.object.type != ObjectType::roster) continue;
        Fields f;
        if (!Fields::decode(anc.object.body, f)) continue;
        auto k = f.get_fixed<64>(roster_field::device_key);
        if (k && crypto_->device_id(*k) == e.author) {
            key = &devices_.at(e.author).key;
            break;
        }
    }
    if (!key) return Status::fail("written by a device this vault had not admitted at that point");

    const bool ok = e.is_envelope ? verify(e.envelope, *key, *crypto_) : verify(e.object, *key, *crypto_);
    if (!ok) return Status::fail("a signature in the record is not valid");
    return Status::ok();
}

std::set<Digest> Record::ancestors(const std::vector<Digest>& links) const {
    std::set<Digest> seen;
    std::vector<Digest> stack = links;
    while (!stack.empty()) {
        Digest d = stack.back();
        stack.pop_back();
        if (!seen.insert(d).second) continue;
        auto it = entries_.find(d);
        if (it == entries_.end()) continue;
        for (const Digest& p : it->second.ancestry_links) stack.push_back(p);
    }
    return seen;
}

void Record::retry_held() {
    bool progress = true;
    while (progress && !pending_.empty()) {
        progress = false;
        for (auto it = pending_.begin(); it != pending_.end();) {
            const bool ready = std::all_of(it->second.ancestry_links.begin(), it->second.ancestry_links.end(),
                                           [&](const Digest& p) { return entries_.count(p) != 0; });
            if (!ready) {
                ++it;
                continue;
            }
            Digest addr = it->first;
            Entry e = std::move(it->second);
            it = pending_.erase(it);
            (void)admit(addr, std::move(e));
            progress = true;
        }
    }
}

// device_key: read only, to sign the new object.
Status Record::append(ObjectType type, const Fields& body, std::int64_t time, const DeviceId& me,
                      const SecretBuffer& device_key, Digest* address) {
    if (!founded()) return Status::fail("the record has no vault yet");
    SignedObject o;
    o.type = type;
    o.vault = vault_id_;
    o.parents = heads();
    o.previous = last_of(me);
    o.time = time;
    o.body = body.encode();
    o.author = me;
    if (Status st = sign(o, device_key, *crypto_); !st) return st;
    return add(encode(o), address);
}

std::vector<Digest> Record::heads() const {
    std::set<Digest> referenced;
    for (const auto& [addr, e] : entries_)
        for (const Digest& p : e.ancestry_links) referenced.insert(p);
    std::vector<Digest> out;
    for (const auto& [addr, e] : entries_)
        if (!referenced.count(addr)) out.push_back(addr);
    return out;
}

Digest Record::last_of(const DeviceId& device) const {
    std::set<Digest> mine, referenced;
    for (const auto& [addr, e] : entries_) {
        if (e.author != device) continue;
        mine.insert(addr);
        const Digest& prev = e.is_envelope ? e.envelope.previous : e.object.previous;
        if (!is_zero(prev)) referenced.insert(prev);
    }
    for (const Digest& d : mine)
        if (!referenced.count(d)) return d;
    return Digest{};
}

std::optional<DeviceInfo> Record::device(const DeviceId& id) const {
    auto it = devices_.find(id);
    if (it == devices_.end()) return std::nullopt;
    return it->second;
}

std::vector<DeviceInfo> Record::devices() const {
    std::vector<DeviceInfo> out;
    for (const auto& [id, d] : devices_) out.push_back(d);
    return out;
}

std::optional<Envelope> Record::current_envelope(const Id16& secret, bool* conflict) const {
    if (conflict) *conflict = false;
    if (retired(secret)) return std::nullopt;
    const Envelope* best = nullptr;
    for (const auto& [addr, e] : entries_) {
        if (!e.is_envelope || e.envelope.header.secret != secret) continue;
        if (!best || e.envelope.header.version > best->header.version) {
            best = &e.envelope;
            if (conflict) *conflict = false;
        } else if (e.envelope.header.version == best->header.version && e.envelope.body != best->body) {
            if (conflict) *conflict = true; // a reseal keeps the body; a different body is a real conflict
        }
    }
    if (!best) return std::nullopt;
    return *best;
}

bool Record::retired(const Id16& secret) const {
    for (const auto& [addr, e] : entries_) {
        if (e.is_envelope || e.object.type != ObjectType::retire) continue;
        Fields f;
        if (Fields::decode(e.object.body, f) && f.get_fixed<16>(act_field::secret) == secret) return true;
    }
    return false;
}

std::vector<ActInfo> Record::acts() const {
    std::vector<ActInfo> out;
    for (const auto& [addr, e] : entries_) {
        if (e.is_envelope || e.object.type != ObjectType::act) continue;
        Fields f;
        if (!Fields::decode(e.object.body, f)) continue;
        ActInfo a;
        a.address = addr;
        a.time = e.object.time;
        a.author = e.author;
        if (auto s = f.get_fixed<16>(act_field::secret)) a.secret = *s;
        a.how = std::string(f.get(act_field::how));
        out.push_back(a);
    }
    std::sort(out.begin(), out.end(), [](const ActInfo& a, const ActInfo& b) { return a.time < b.time; });
    return out;
}

std::map<Digest, Bytes> Record::all_bytes() const {
    std::map<Digest, Bytes> out;
    for (const auto& [addr, e] : entries_) out.emplace(addr, e.bytes);
    for (const auto& [addr, e] : pending_) out.emplace(addr, e.bytes);
    for (const auto& [addr, b] : refused_) out.emplace(addr, b);
    return out;
}

} // namespace verguenza
