---
type: Reference
title: Governance — political systems and their mathematics
description: "The research the author asked for on 2026-10-01 ('it might be useful to look into niche political systems or something (like anarchism maybe?) and how those systems could mathematically translate'). Each tradition, what it actually says, its mathematical translation, and what Verguenza takes from it: heterarchy, anarchist and council mandates, sociocracy, Ostrom's commons, sortition, liquid democracy, lattice security, object capabilities, Meadowcap, access structures, trust metrics, and the two local-first group systems (Keyhive, p2panda) closest to Verguenza's problem. Checked against sources on 2026-10-01, cited by URL."
tags: [status:current, audience:all, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

**How to read this page.** Each row was checked against the source linked, on
2026-10-01, at the depth of the summary given (not a full reading of every book).
Where a translation is Verguenza's own and not the source's, it says so.
[Authority](/concepts/authority.md) is where these become design.

# The traditions

## Heterarchy (McCulloch, 1945)

**What it says.** Warren McCulloch, *A heterarchy of values determined by the
topology of nervous nets* (Bulletin of Mathematical Biophysics 7, 89–93). Measured
preferences can be circular (A over B, B over C, C over A). He read this as
*"consistency of an order too high to permit construction of a scale of values"*,
grounded in the circular topology of the net itself.

**Translation.** A relation of "may override" does not have to be transitive or
acyclic to be coherent, provided each decision is made locally against its own
structure rather than against a global scale. *(Verguenza's reading.)*

**Taken.** Authority may contain cycles; nothing in Verguenza computes a global
rank. Sources: [Springer](https://link.springer.com/doi/10.1007/BF02478457),
[vordenker.de on heterarchy](https://vordenker.de/heterarchy/b_heterarchy-e.pdf).

## Mandated and recallable delegates (Proudhon, the Paris Commune, the soviets, the Zapatistas)

**What it says.** Proudhon (1848): *"the imperative mandate, and permanent
revocability are the most immediate and incontestable consequences of the electoral
principle."* The Paris Commune and the early soviets implemented mandated,
recallable delegates; anarchist federalism has groups federate through them; the
Zapatista communities govern by *mandar obedeciendo* ("governing by obeying"),
through mandated, recallable delegates and *consultas*.

**Translation.** Authority is a lease from the people it is exercised for, and each
lease names who may end it. A delegation edge carries its mandators; recall is an
operation available to them. *(Verguenza's reading.)*

**Taken.** Every grant records whose consent made it; recall is easier than grant.
Sources: [Anarchist Library, "Going Beyond Picking Rulers"](https://theanarchistlibrary.org/library/anarcho-going-beyond-picking-rulers),
[Andrew Flood on the Zapatistas](https://theanarchistlibrary.org/library/andrew-flood-zapatistas-an-inspirational-decade-on.html),
[Isonomía, *Mandar obedeciendo*](https://www.isonomia.itam.mx/index.php/revista-cientifica/article/view/554).

## Sociocracy (consent and double-linked circles)

**What it says.** Decisions are taken by **consent**: approved when no member raises
a reasoned, paramount objection, which is weaker than agreement and stronger than a
vote. Circles govern their own domains, and each circle is linked to the next by
**two** people (a leader carrying information in, a delegate carrying it out).

**Translation.** Consent is a veto structure: a decision passes when the set of
objectors is empty. Double-linking makes the circle graph bidirectional, so no
circle is simply "above" another. *(Verguenza's reading.)*

**Taken.** Groups govern their own region; a group is present in its parent through
delegates it chooses. *Consent with a reasoned objection* is a candidate form for a
request (an approval set plus an empty objection set), recorded for
[quorum](/concepts/quorum.md) and not yet adopted. Sources:
[UK co-op, double-linked structure](https://www.uk.coop/resources/sociocracy-co-operative-organisations/double-linked-structure),
[Nonprofit Quarterly](https://nonprofitquarterly.org/sociocracy-an-organizational-structure-for-distributed-leadership/).

## Ostrom's commons (1990)

**What it says.** Elinor Ostrom, *Governing the Commons*, eight principles of
long-lived common-pool resources, including: clear boundaries; collective-choice
arrangements (*"most individuals affected by the operational rules can participate
in modifying the operational rules"*); monitoring by people accountable to the
users; **graduated sanctions**; cheap local conflict resolution; **nested
enterprises**.

**Translation.** A shared vault *is* a commons: a pool of secrets whose overuse
(over-sharing) hurts everyone. The principles map almost one to one: membership
(boundaries), consent rules changeable by those bound (collective choice), the
[audit](/concepts/audit.md) record read by members (monitoring), groups in groups
(nesting). *(Verguenza's reading.)*

**Taken.** Nested groups; rule changes by those affected; audit as members'
monitoring. **Graduated sanctions** (suspend before remove; narrow before revoke) is
recorded as a direction for [authority](/concepts/authority.md), not designed yet.
Sources: [P2P Foundation](https://wiki.p2pfoundation.net/Elinor_Ostrom’s_Eight_Commons_Governance_Design_Principles).

## Sortition (Athens)

**What it says.** Most Athenian officials were chosen by lot (the *kleroterion*),
for short terms, publicly, which made bribing an electorate impossible and kept
power from going to those who sought it.

**Translation.** Random selection from a qualified pool, plus rotation, defeats
pre-arranged collusion. Needs randomness no participant can steer.

**Taken.** Rotation (mandates that expire). Lot-chosen approvers recorded as
*later*. Source: [Wikipedia, Sortition](https://en.wikipedia.org/wiki/Sortition).

## Liquid democracy

**What it says.** A voter votes directly or delegates, transitively; delegations
are revocable at any time; cycles arise and are resolved by rule (abstention,
cycle-breaking, fallback).

**Translation.** A directed delegation graph, resolved per decision.

**Taken.** Nothing yet; delegating an *approval* is expressible as a capability, if
larger organizations need it. Source:
[arXiv, multi-proxy transitive delegation](https://arxiv.org/pdf/1412.4039).

# The mathematics

## Lattices of security classes (Denning, 1976)

Dorothy Denning, *A Lattice Model of Secure Information Flow* (CACM 19(5)).
Security classes form a **lattice**: a partial order with joins, not a line. The
classical "Top Secret > Secret > Confidential" is the special case where the lattice
is a chain; adding compartments makes it a lattice in which two clearances can be
incomparable.

**Taken.** The precedent, in the security literature itself, for "a hierarchy is a
partial order whose linear form is a special case." Verguenza's authority order is
set inclusion of capabilities, which is a lattice in the same sense. Source:
[Denning 1976](https://courses.cs.duke.edu/spring15/cps210/readings/ifc-denning76.pdf).

## Object capabilities (Miller)

*"Only connectivity begets connectivity"*: an object can obtain a reference only
through a chain of references it already had. Authority is reachability in the
reference graph, and the rules say how that graph may grow.

**Taken.** You cannot delegate what you do not hold; every capability has a chain
back to its source. Source:
[Wikipedia, Object-capability model](https://en.wikipedia.org/wiki/Object-capability_model).

## Meadowcap (Willow)

Capabilities over a three-dimensional area (subspace, path, time range), in four
kinds: **source**, **delegation** (by signature), **restriction** (intersect with a
smaller area), **merge**. No central authority.

**Taken.** The four operations, and regions written in the capability. Void
Palabra already names Meadowcap as the shape for its own trust work
(`VoidPalabra:/concepts/peer-and-tier.md`). Sources:
[Meadowcap (deno.land)](https://deno.land/x/meadowcap),
[Spritely forum](https://community.spritely.institute/t/meadowcap-capability-system-for-controlling-access-to-willow-data/411).

## Access structures (Ito–Saito–Nishizeki 1987; Benaloh–Leichter 1988)

Every **monotone access structure** (a family of authorized sets, closed under
supersets) can be realized by a secret-sharing scheme (Ito, Saito, Nishizeki).
Benaloh and Leichter built schemes directly from the structure's **monotone boolean
formula** (AND and OR of thresholds), and proved some structures are not threshold
schemes at all.

**Taken.** A consent rule is a monotone formula; *k of n* is one case; split policies
can follow the formula. Sources:
[Ito–Saito–Nishizeki (DOI)](https://www.doi.org/10.1007/0-387-34799-2_3),
[Stinson, *Cryptography: Theory and Practice*, ch. 11](https://www.lait.fe.uni-lj.si/gradiva/KK/KNJIGE/Cryptography%20-Theory%20And%20Practice-Stinson/ch11/332-334.html).

## Trust metrics (Advogato, Levien and Aiken, 1998)

Peer certifications form a graph; standing is computed by **network flow** from
trusted seeds, so a cluster of fake accounts cannot certify itself up. Levien noted
the kinship with PageRank. Its attack resistance was later disputed.

**Taken.** Not for authority. Possibly for display (*how embedded is this person*).
Sources: [USENIX Security 1998](https://www.usenix.org/publications/library/proceedings/sec98/full_papers/levien/levien_html/levien.html),
[a dissent](https://squarefree.com/?p=263).

# The nearest working systems

## p2panda-auth

Decentralized, offline-first group management. Operations form a DAG; concurrent
operations are resolved deterministically. Its access levels are a **ladder** (Pull ⊂
Read ⊂ Write ⊂ Manage). Its conflict rules (**strong removal**): *"removal or
demotion of a manager causes any concurrent actions by that member to be
invalidated"*; mutual removals both apply; a re-added member's concurrent actions
stay invalid.

**Taken.** Independent confirmation of [trust](/concepts/trust.md)'s rules 2 and 3.
**Departed from:** the ladder. Source:
[docs.rs, p2panda-auth 0.7.1](https://docs.rs/crate/p2panda-auth/0.7.1).

## Keyhive (Ink & Switch)

Local-first access control: *convergent capabilities* (a capability model built for
CRDTs), a group-management CRDT with concurrent membership changes and
coordination-free revocation, groups as members of groups, and end-to-end encryption
whose group key agreement (BeeKEM, after MLS's TreeKEM) tolerates concurrent updates.

**Taken.** Groups in groups; the confirmation that capabilities and CRDTs can be made
to agree. **Not yet read in depth**: its revocation rules are in its lab notebooks,
to be read before [trust](/concepts/trust.md) is built. Source:
[Ink & Switch, Keyhive](https://www.inkandswitch.com/project/keyhive/),
[notebook](https://www.inkandswitch.com/keyhive/notebook/).
