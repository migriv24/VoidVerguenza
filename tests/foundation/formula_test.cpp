/* tests/foundation/formula_test.cpp — consent formulas (roadmap 1A.3): every formula on the
 * authority and quorum pages, satisfied and unsatisfied sets, voting power, "weaker than".
 * Pure functions: no crypto, no Void Core. */
#include "foundation/access/formula.hpp"
#include "support/check.hpp"

#include <cmath>

using namespace verguenza::access;

namespace {
// the store from okf/concepts/authority.md
People resolve(const std::string& name) {
    if (name == "managers") return {"mara", "mike"};
    if (name == "owners") return {"olga", "omar", "otto"};
    if (name == "front_desk") return {"fay", "finn", "flo"};
    if (name == "IT") return {"ivan"};
    if (name == "bookkeeper") return {"bea"};
    return {name}; // a person's own name
}
Formula f(const std::string& text) {
    Formula out;
    CHECK_OK(parse(text, out));
    return out;
}
bool sat(const std::string& text, const People& who) { return f(text).satisfied_by(who, resolve); }
} // namespace

int main() {
    // enrol a device = IT ∨ 2-of(managers)
    CHECK(sat("IT or 2-of(managers)", {"ivan"}));
    CHECK(sat("IT ∨ 2-of(managers)", {"mara", "mike"}));
    CHECK(!sat("IT | 2-of(managers)", {"mara"}));
    CHECK(!sat("IT or 2-of(managers)", {"bea", "olga"}));

    // reveal payroll = bookkeeper ∧ 1-of(owners)
    CHECK(sat("bookkeeper and 1-of(owners)", {"bea", "omar"}));
    CHECK(!sat("bookkeeper ∧ 1-of(owners)", {"olga", "omar", "otto"}));
    CHECK(!sat("bookkeeper & 1-of(owners)", {"bea"}));

    // change front desk = 2-of(front_desk) ∨ (1-of(front_desk) ∧ 1-of(managers))
    const std::string fd = "2-of(front_desk) or (1-of(front_desk) and 1-of(managers))";
    CHECK(sat(fd, {"fay", "flo"}));
    CHECK(sat(fd, {"finn", "mike"}));
    CHECK(!sat(fd, {"mara", "mike"}));

    // k-of over several names, and an unknown name resolves to nobody (the safe direction)
    CHECK(sat("2-of(IT, bookkeeper, managers)", {"ivan", "mara"}));
    CHECK(!sat("nobody_here", {"ivan"})); // a name no group claims is one person, and ivan is not them
    CHECK(!sat("2-of(IT)", {"ivan"}));             // only one person can be counted

    // parsing refusals
    Formula bad;
    CHECK(!parse("IT or", bad));
    CHECK(!parse("(IT or managers", bad));
    CHECK(!parse("2-of(managers", bad));
    CHECK(!parse("0-of(managers)", bad));
    CHECK(!parse("IT managers", bad));
    // "order" is a name, not "or" followed by "der"
    CHECK(sat("order", {"order"}));

    // round trip through to_string
    CHECK(f(f(fd).to_string()).to_string() == f(fd).to_string());

    // monotone: adding someone to a satisfying set still satisfies (property, by enumeration)
    {
        const Formula g = f(fd);
        const std::vector<Person> everyone = {"fay", "finn", "flo", "mara", "mike", "ivan"};
        for (unsigned mask = 0; mask < (1u << everyone.size()); ++mask) {
            People s;
            for (unsigned i = 0; i < everyone.size(); ++i)
                if (mask & (1u << i)) s.insert(everyone[i]);
            if (!g.satisfied_by(s, resolve)) continue;
            for (const auto& extra : everyone) {
                People t = s;
                t.insert(extra);
                CHECK(g.satisfied_by(t, resolve));
            }
        }
    }

    // Banzhaf: under bookkeeper ∧ 1-of(owners) the bookkeeper is a swing in all 7 winning
    // coalitions, each owner in 1: power 7/10 and 1/10 each (okf/concepts/standing.md)
    {
        std::map<Person, double> power;
        CHECK_OK(banzhaf(f("bookkeeper and 1-of(owners)"), resolve, power));
        CHECK(std::fabs(power["bea"] - 0.7) < 1e-9);
        CHECK(std::fabs(power["olga"] - 0.1) < 1e-9);
        CHECK(power["bea"] > power["omar"]);
        // symmetric players have equal power
        CHECK_OK(banzhaf(f("2-of(front_desk)"), resolve, power));
        CHECK(std::fabs(power["fay"] - power["flo"]) < 1e-9);
    }

    // weaker_than: 1-of(managers) lets one manager in where 2-of(managers) did not
    {
        bool weaker = false;
        CHECK_OK(weaker_than(f("1-of(managers)"), f("2-of(managers)"), resolve, weaker));
        CHECK(weaker);
        CHECK_OK(weaker_than(f("2-of(managers)"), f("1-of(managers)"), resolve, weaker));
        CHECK(!weaker); // stricter is never weaker
        CHECK_OK(weaker_than(f("IT or 2-of(managers)"), f("2-of(managers)"), resolve, weaker));
        CHECK(weaker); // adding an alternative lowers the bar for the IT person
    }

    return vtest::finish("formula");
}
