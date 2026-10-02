---
type: Concept
title: Standing — weights on edges, and order as a projection
description: "The author, 2026-10-01: knowledge, authority and consent as shared runes, with weighted edges from each person that grow, and 'some sort of other algorithm to actually figure out order, or the projection of order'. Built on what Void Core already has (an edge's weight IS an attribute's value when it points at a measure rune, 0.14) and on Void Unity's law (no independent floats: every number is derived from facts, keyed by its source, never written back) and Allomone's sync boundary (inputs sync, derived values never do). Three measure runes; how each weight is derived (exposure, reach, and voting power by the Banzhaf index over consent formulas); order as a dominance partial order with lenses for display; thresholds that become tags rules can read; the diagnostics a group needs (single points of knowledge, concentration); and the one thing a weight may never do: decide."
tags: [status:planned, audience:all, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# The author's picture

> *"if there is a value, its held and associated via an edge. like a number or
> something is by the weight of an edge between node concepts. so something like
> 'knowledge' sounds like the 'amount of knowledge' can grow. So it would be a shared
> node of 'knowledge' and 'authority' and 'consent' and the weight between these three
> runes and to the runes of the members of the network, it would like, grow in
> weights. … we would use some sort of other algorithm to actually figure out 'order'
> or the projection of order."* — the author, 2026-10-01

This page builds that picture out of what the family already has, and adds one rule
the picture needs to be safe.

# What the family already has

| from | what | how Verguenza uses it |
|---|---|---|
| **Void Core 0.2.14** (`VoidCore:SPEC.md` §3.7.1) | *an edge weight MAY be an attribute's value*: when an edge's `to` is a `measure` rune, its `weight` **is** the value (`player --[5.0]--> speed`). Read with the `values` verb | `ana --[w]--> knowledge` is Ana's knowledge, in Core's own vocabulary, with no new mechanism |
| **Void Core**, same section | *"if a number is read by rules that produce new structure, it belongs on an edge; if only by renderers, in a field"*; and **weights in one mantle are not commensurable** (a graph holding "supports, 0.8" and "speed, 900" has no meaningful weighted degree) | standing is read by rules (thresholds, below), so edges are right; and knowledge, authority and consent are **never added together** |
| **Void Unity** (`VoidUnity:/concepts/derived-properties.md`) | **no independent floats**: *"health is not JUST alone — its value is derived from other things."* Numbers are derived from facts and authored leaf constants, keyed by **source** (two identical +3 rings total +6), never written back; conditions reach rules only as **threshold tags**, with hysteresis | every standing weight is derived from facts in the [signed record](/concepts/trust.md); nobody types a weight |
| **Void Allomone** (`VoidAllomone:/concepts/multi-user.md`) | **the sync boundary**: inputs sync, derived values never do; each peer re-derives | weights are never synced and never signed; the facts they come from are |
| **Void Allomone**, governance | *"a goal ranks, and a lattice deliberately does not"*: an objective function makes everything comparable, and ⊤ exists to say some things are not | order is a partial order first; a ranking is a lens chosen for display |
| **Void Maiz's user graph** (`VoidMaiz/include/voidmaiz/usergraph.hpp`) | symmetric, timeless coincidence weights that only grow (counts under addition), accumulated and then *compiled* into runes on request | the shape of "weights that grow": a join that is a sum of non-negative counts, compiled into the document only when a view asks |

# Three measure runes

Each vault has a `standing` mantle (a projection, like the others,
[trust](/concepts/trust.md)) holding three runes whose glyph is `kind: measure`:

```
knowledge   measure, unit "exposure"     how much a person can open, and has opened
authority   measure, unit "reach"        how much a person may do, and to how much
consent     measure, unit "power"        how much a person's agreement decides
```

and one edge per person to each:

```
ana --[ 14.0 ]--> knowledge
ana --[  3.0 ]--> authority
ana --[ 0.42 ]--> consent
```

Groups get the same three edges (a group's knowledge is the union of its members',
not the sum), so the graph screen can show the front desk's standing beside a
person's.

# How each weight is derived

**No weight is ever written by a person.** Each is a readout over facts already in
the signed record, plus a few authored **leaf constants** (Unity's third law: content
floats may only ever be leaves).

| weight | derived from | readout | grows? |
|---|---|---|---|
| **knowledge** | the seals: every secret ever sealed to any of the person's devices (knowledge edges, [authority](/concepts/authority.md)) | Σ over those secrets of the secret's **sensitivity** (a leaf, authored per secret or defaulted from its policy: open 1, guarded 3, split 9) | **only grows**: a seal once held is knowledge forever. A second readout, *can open now*, falls when a device is removed; the gap between them is what must be rotated |
| **authority** | the person's valid capabilities ([trust](/concepts/trust.md) rule 4) | Σ over capabilities of (how many things the region covers × the **weight of the verbs**, a leaf table: read 1, write 2, grant 4, recall 4) | rises and falls with grants and recalls |
| **consent** | the vault's consent formulas ([quorum](/concepts/quorum.md)) | the person's **Banzhaf power**: in how many of the coalitions that satisfy a formula the person is a *swing* (removing them makes it fail), normalized; averaged over the formulas, weighted by what each governs | changes when formulas or memberships change |

Each sum is **keyed by its source** (the seal, the capability, the formula), so
the readout is order-free and two identical facts count twice, exactly as Unity's
`DerivationLattice` and Maiz's `Tally` do.

**Why the Banzhaf index for consent.** A consent formula is a *simple game* in the
sense of voting theory: a monotone family of winning coalitions, which is the same
object as a monotone access structure ([governance](/references/governance.md)).
Voting theory has measured a player's power in such a game since Banzhaf (1965) and
Shapley and Shubik (1954). The measure says something a ladder cannot: in
`bookkeeper ∧ 1-of(owners)`, the bookkeeper is a swing in every winning coalition and
each of three owners in only some, so the bookkeeper holds more consent power over
payroll than any single owner, without being "above" them anywhere else. For the
small groups Verguenza serves, it is computed exactly by enumerating coalitions
(2^20 is a million, fast); a larger organization (Hormiga's) would need the formula's
structure or a deterministic approximation.

# Order is a projection, computed by lenses

Three weights per person give a point in three dimensions. The structure underneath
is the **dominance order**: *p ≥ q* when p's knowledge, authority and consent are each
at least q's. It is a **partial order**, and its incomparable pairs are the honest
answer: the IT person (high authority, low knowledge) and the bookkeeper (the
reverse) are not above or below each other.

The screen needs a list anyway, so it offers **lenses**, each one a linear extension
chosen on purpose and named:

| lens | orders by | answers |
|---|---|---|
| **exposure** | knowledge | *if this person's devices were all stolen, how bad is it?* |
| **reach** | authority | *who can change the most?* |
| **voice** | consent | *whose agreement decides the most?* |
| **dominance** | the partial order, drawn as layers (people no one dominates on top) | *who is plainly ahead of whom, and who is simply different* |

A lens is chosen by a person and shown with its name; there is no default "rank".
That is Palabra's timeline argument applied to people: a linear extension computed for
display, never stored as the truth ([authority](/concepts/authority.md)).

# Thresholds become tags

A rule cannot read a float (Void Core's tag grammar is set membership; Unity found the
same). So, as in Unity, a derived weight **crossing a line dispatches one tag command**,
with hysteresis so a weight sitting on a line does not flicker:

```
knowledge(ana) crosses 10     →  tag ana +standing:exposed
authority(it)  crosses 20     →  tag it +standing:wide-reach
```

Those tags are what [access](/concepts/access.md) rules and Allomone scripts can see:
*highlight people tagged `standing:exposed` in the graph*, *require rotate-after-reveal
for secrets known only by `standing:exposed` people*. The tags are projections too,
rebuilt from the facts, never authored.

# What the weights are for: the group seeing itself

Ostrom's fourth principle is monitoring by the people who use the commons
([governance](/references/governance.md)). Standing is how a group monitors its own
shame-surface, and Allomone's governance page already named the useful measures
(*conflict density, influence concentration*). Verguenza's versions:

- **Single points of knowledge**: a secret known by one person's devices only. If
  they are lost, so is it ([recovery](/concepts/recovery.md)).
- **Concentration**: how unevenly knowledge (or reach, or voice) is spread. One person
  holding most of the guarded knowledge is a risk the group should see.
- **Stale exposure**: knowledge that has not been *used* (no reveal or use act) for a
  long time. A person who knows a code they never use may not need it.
- **The rotation gap**: knowledge minus can-open-now: what departed people and lost
  devices still know.

# The one rule: a weight never decides

**No standing weight is ever consulted to allow or refuse anything.** Authority comes
only from capabilities granted on purpose, with consent ([authority](/concepts/authority.md));
weights are a readout of that structure, for people to look at. Three reasons:

1. **A computed authority is an attack surface.** Advogato's flow-based standing was
   elegant and its resistance to manipulation was disputed
   ([governance](/references/governance.md)). A password manager's power must be
   something someone granted.
2. **Derived values differ between peers** while facts are still arriving, and that is
   correct for a view (Allomone: *two peers agreeing on scripts and disagreeing on
   derived annotations is correct behaviour*) and fatal for a decision.
3. **Precedence is a presentation, never a resolution** (Palabra, finding 6).

Threshold tags inherit the rule: a rule may *show*, *warn* or *propose* on
`standing:*` tags; it may not grant.

# Whether weights enter the document

The derivation is pure and cheap, so the default is **derive on demand, store
nothing** (Allomone's sync boundary). A view that wants Core's `values` verb to see
them (the graph screen, an Allomone script) asks for a **compile**, which writes the
measure edges into the `standing` mantle as a projection, attributed to a `standing`
actor, rebuilt rather than merged, and excluded from what Verguenza signs. Whether a
compiled standing mantle may sync at all, given that it describes people, is the same
privacy question Allomone filed about the user graph. [Q16](/developer_questions.md).
