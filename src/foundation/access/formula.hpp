/* foundation/access/formula.hpp — consent formulas: whose agreement an action needs.
 *
 * okf/concepts/authority.md §"Consent as an access structure", okf/concepts/quorum.md
 * §"Policies", okf/concepts/standing.md (consent power). A formula is a monotone
 * boolean expression over people and groups; k-of-n is one case of it:
 *
 *     IT or 2-of(managers)
 *     bookkeeper and 1-of(owners)
 *     2-of(front_desk) or (1-of(front_desk) and 1-of(managers))
 *
 * `and` / `&` / ∧ and `or` / `|` / ∨ are accepted. A bare name means "any one of
 * them" (one person, or any member of a group). Names resolve to sets of people
 * through a Resolver the caller supplies, so the formula itself never depends on
 * merging state (Palabra's finding 3): the same formula and the same membership give
 * the same answer on every device.
 *
 * Pure functions. Nothing here grants anything: a formula is checked against a set of
 * approvals someone else has verified. */
#pragma once

#include "foundation/core/status.hpp"

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace verguenza::access {

using Person = std::string;
using People = std::set<Person>;

/* name → the people it stands for. A person's own name resolves to just them. An
 * unknown name resolves to nobody, which makes any term needing it unsatisfiable:
 * the safe direction. */
using Resolver = std::function<People(const std::string& name)>;

struct Formula {
    enum class Kind { all, any, threshold };
    Kind kind = Kind::threshold;
    int k = 1;                                   // threshold only
    std::vector<std::string> names;              // threshold: the names whose people count
    std::vector<std::shared_ptr<Formula>> parts; // all / any

    bool satisfied_by(const People& approvers, const Resolver& resolve) const;
    /* Everyone the formula can mention, resolved. */
    People players(const Resolver& resolve) const;
    std::string to_string() const;
};

Status parse(const std::string& text, Formula& out);

/* Banzhaf power of each player: the share of swings, where a swing is a satisfying
 * coalition that stops satisfying when that player leaves. Normalized to sum to 1
 * (all zero if no coalition satisfies). Exact, by enumeration: refused above 20 players
 * (2^20 coalitions). okf/concepts/standing.md. */
Status banzhaf(const Formula& f, const Resolver& resolve, std::map<Person, double>& out);

/* True when some set of people satisfies `candidate` but not `current`: replacing
 * `current` with `candidate` would lower the bar for someone (quorum.md: a policy cannot
 * be weakened without satisfying itself). Exact, by enumeration over the players of both. */
Status weaker_than(const Formula& candidate, const Formula& current, const Resolver& resolve, bool& weaker);

} // namespace verguenza::access
