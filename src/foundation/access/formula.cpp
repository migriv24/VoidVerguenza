#include "foundation/access/formula.hpp"

#include <cctype>
#include <sstream>

namespace verguenza::access {

// ── evaluation ──────────────────────────────────────────────────────────────

bool Formula::satisfied_by(const People& approvers, const Resolver& resolve) const {
    switch (kind) {
    case Kind::all:
        for (const auto& p : parts)
            if (!p->satisfied_by(approvers, resolve)) return false;
        return true;
    case Kind::any:
        for (const auto& p : parts)
            if (p->satisfied_by(approvers, resolve)) return true;
        return false;
    case Kind::threshold: {
        People eligible;
        for (const auto& n : names)
            for (const auto& person : resolve(n)) eligible.insert(person);
        int count = 0;
        for (const auto& a : approvers) count += eligible.count(a) ? 1 : 0;
        return count >= k;
    }
    }
    return false;
}

People Formula::players(const Resolver& resolve) const {
    People out;
    if (kind == Kind::threshold) {
        for (const auto& n : names)
            for (const auto& p : resolve(n)) out.insert(p);
    } else {
        for (const auto& part : parts)
            for (const auto& p : part->players(resolve)) out.insert(p);
    }
    return out;
}

std::string Formula::to_string() const {
    std::ostringstream s;
    if (kind == Kind::threshold) {
        if (k == 1 && names.size() == 1) return names[0];
        s << k << "-of(";
        for (std::size_t i = 0; i < names.size(); ++i) s << (i ? ", " : "") << names[i];
        s << ")";
        return s.str();
    }
    const char* op = kind == Kind::all ? " and " : " or ";
    for (std::size_t i = 0; i < parts.size(); ++i) {
        const bool paren = parts[i]->kind != Kind::threshold && parts[i]->kind != kind;
        s << (i ? op : "") << (paren ? "(" : "") << parts[i]->to_string() << (paren ? ")" : "");
    }
    return s.str();
}

// ── parsing ─────────────────────────────────────────────────────────────────

namespace {

class Parser {
public:
    explicit Parser(const std::string& t) : t_(t) {}

    Status run(Formula& out) {
        auto f = parse_or();
        skip();
        if (!error_.empty()) return Status::fail(error_);
        if (pos_ != t_.size()) return Status::fail("unexpected text at position " + std::to_string(pos_) + " in the formula");
        out = *f;
        return Status::ok();
    }

private:
    void skip() {
        while (pos_ < t_.size() && std::isspace(static_cast<unsigned char>(t_[pos_]))) ++pos_;
    }
    bool eat(const std::string& s) {
        skip();
        if (t_.compare(pos_, s.size(), s) != 0) return false;
        // a word operator must end at a word boundary ("order" is not "or")
        if (std::isalpha(static_cast<unsigned char>(s[0])) && pos_ + s.size() < t_.size() &&
            (std::isalnum(static_cast<unsigned char>(t_[pos_ + s.size()])) || t_[pos_ + s.size()] == '_'))
            return false;
        pos_ += s.size();
        return true;
    }
    bool eat_or() { return eat("or") || eat("|") || eat("\xE2\x88\xA8"); }  // ∨
    bool eat_and() { return eat("and") || eat("&") || eat("\xE2\x88\xA7"); } // ∧

    std::shared_ptr<Formula> combine(Formula::Kind kind, std::vector<std::shared_ptr<Formula>> parts) {
        if (parts.size() == 1) return parts[0];
        auto f = std::make_shared<Formula>();
        f->kind = kind;
        f->parts = std::move(parts);
        return f;
    }
    std::shared_ptr<Formula> parse_or() {
        std::vector<std::shared_ptr<Formula>> parts{parse_and()};
        while (error_.empty() && eat_or()) parts.push_back(parse_and());
        return combine(Formula::Kind::any, std::move(parts));
    }
    std::shared_ptr<Formula> parse_and() {
        std::vector<std::shared_ptr<Formula>> parts{parse_atom()};
        while (error_.empty() && eat_and()) parts.push_back(parse_atom());
        return combine(Formula::Kind::all, std::move(parts));
    }
    std::string name() {
        skip();
        const std::size_t start = pos_;
        while (pos_ < t_.size() && (std::isalnum(static_cast<unsigned char>(t_[pos_])) || t_[pos_] == '_' ||
                                    t_[pos_] == '-' || t_[pos_] == '.' || t_[pos_] == ':'))
            ++pos_;
        return t_.substr(start, pos_ - start);
    }
    std::shared_ptr<Formula> parse_atom() {
        auto f = std::make_shared<Formula>();
        if (!error_.empty()) return f;
        if (eat("(")) {
            f = parse_or();
            if (!eat(")")) error_ = "a '(' in the formula is never closed";
            return f;
        }
        skip();
        // k-of(name, name, …)
        std::size_t save = pos_;
        if (pos_ < t_.size() && std::isdigit(static_cast<unsigned char>(t_[pos_]))) {
            int k = 0;
            while (pos_ < t_.size() && std::isdigit(static_cast<unsigned char>(t_[pos_]))) k = k * 10 + (t_[pos_++] - '0');
            if (t_.compare(pos_, 4, "-of(") == 0) {
                pos_ += 4;
                f->kind = Formula::Kind::threshold;
                f->k = k;
                do {
                    std::string n = name();
                    if (n.empty()) {
                        error_ = "a k-of(…) list needs names";
                        return f;
                    }
                    f->names.push_back(n);
                } while (eat(","));
                if (!eat(")")) error_ = "a k-of( list is never closed";
                if (k < 1) error_ = "k in k-of(…) must be at least 1";
                return f;
            }
            pos_ = save;
        }
        std::string n = name();
        if (n.empty()) {
            error_ = pos_ < t_.size() ? "unexpected '" + std::string(1, t_[pos_]) + "' in the formula"
                                      : "the formula ends where a name was expected";
            return f;
        }
        f->kind = Formula::Kind::threshold;
        f->k = 1;
        f->names = {n};
        return f;
    }

    const std::string& t_;
    std::size_t pos_ = 0;
    std::string error_;
};

constexpr std::size_t kMaxPlayers = 20;

} // namespace

Status parse(const std::string& text, Formula& out) { return Parser(text).run(out); }

// ── voting power ────────────────────────────────────────────────────────────

Status banzhaf(const Formula& f, const Resolver& resolve, std::map<Person, double>& out) {
    const People all = f.players(resolve);
    if (all.size() > kMaxPlayers)
        return Status::fail("more than " + std::to_string(kMaxPlayers) + " people: exact voting power is not computed");
    const std::vector<Person> players(all.begin(), all.end());
    const std::size_t n = players.size();
    std::vector<long long> swings(n, 0);
    for (std::size_t mask = 0; mask < (std::size_t{1} << n); ++mask) {
        People coalition;
        for (std::size_t i = 0; i < n; ++i)
            if (mask & (std::size_t{1} << i)) coalition.insert(players[i]);
        if (!f.satisfied_by(coalition, resolve)) continue;
        for (std::size_t i = 0; i < n; ++i) {
            if (!(mask & (std::size_t{1} << i))) continue;
            People without = coalition;
            without.erase(players[i]);
            if (!f.satisfied_by(without, resolve)) ++swings[i];
        }
    }
    long long total = 0;
    for (auto s : swings) total += s;
    out.clear();
    for (std::size_t i = 0; i < n; ++i) out[players[i]] = total ? static_cast<double>(swings[i]) / total : 0.0;
    return Status::ok();
}

Status weaker_than(const Formula& candidate, const Formula& current, const Resolver& resolve, bool& weaker) {
    People all = candidate.players(resolve);
    for (const auto& p : current.players(resolve)) all.insert(p);
    if (all.size() > kMaxPlayers)
        return Status::fail("more than " + std::to_string(kMaxPlayers) + " people: the comparison is not computed");
    const std::vector<Person> players(all.begin(), all.end());
    weaker = false;
    for (std::size_t mask = 0; mask < (std::size_t{1} << players.size()) && !weaker; ++mask) {
        People coalition;
        for (std::size_t i = 0; i < players.size(); ++i)
            if (mask & (std::size_t{1} << i)) coalition.insert(players[i]);
        weaker = candidate.satisfied_by(coalition, resolve) && !current.satisfied_by(coalition, resolve);
    }
    return Status::ok();
}

} // namespace verguenza::access
