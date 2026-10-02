---
type: Concept
title: Authority — hierarchy without a line
description: "What it means to be 'higher' in a group that shares secrets, when the structure underneath is a graph. Palabra found that a timeline is a linear extension of a partial order, computed for display; Verguenza proposes that a rank (owner, admin, member, guest) is the same kind of thing: a projection, useful on screen, never the structure. Underneath are three relations kept apart: KNOWLEDGE (who can open what; it only grows, like time), AUTHORITY (who may do what to which region; a graph of delegations, recallable), and CONSENT (whose agreement an action needs; a monotone access structure, not a number). Groups nest, mandates flow both ways, recall is easier than grant, and 'higher priority' never silently wins a conflict. How the IT person still ends up with more power than the front desk, and where every idea came from."
tags: [status:planned, audience:all, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# The problem, in the author's words

> *"Similar to how palabra re-invented git, verguenza might have to reinvent what
> it means to be 'higher priority' in a graph based structural environment. Where
> we don't want to think linearly. Note that hierarchy is also a linear dimension,
> similar to how time was."*
>
> *"in practice things will be linear in nature (the IT guy should have more
> privileges than a normal desk worker). however, the WAY in which that manifests
> matters. We could perhaps keep labels like 'admin' and 'guest' mostly for UI/UX
> purposes. but architecturally we need to think outside the box."*
> — the author, 2026-10-01

# The move Palabra made, made again

Palabra's central argument (`VoidPalabra:/design/why-not-linear.md`): a linear
history **stores an arbitrary choice as though it were a fact**. Two independent
changes are unordered; a timeline puts one first anyway. So Palabra keeps the
partial order as the truth, and computes a timeline (a *linear extension*) only to
draw it.

A ranked hierarchy makes the same mistake about people. *"Admin > member > guest"*
says the bookkeeper is above or below the IT person. Usually neither is true: the
IT person may enrol devices and must not see payroll, and the bookkeeper is the
other way round. They are **incomparable**, and a ladder has to lie about one of
them. Every ranked system then grows exceptions ("admins can't see payroll, except
...") until the ladder is a graph that pretends not to be.

So, the same move: **the structure is a graph; a rank is a projection of it,
computed for the screen.** The labels the author wants to keep (*owner, admin,
member, guest*) survive as exactly that.

# Three relations, never folded into one

A ladder folds three different questions into one number. Verguenza keeps them
apart, because they behave differently:

| relation | answers | shape | changes how |
|---|---|---|---|
| **knowledge** | *who can open what, and who already has* | a bipartite graph: devices → secrets (a seal is an edge) | **only grows**. A person cannot unlearn a password. Revoking stops future knowledge; past knowledge is a fact, answered by rotation at the vendor |
| **authority** | *who may do what, to which part of the vault* | a graph of **delegations**: each edge a capability (a region, the verbs, the conditions, an expiry), signed by whoever delegated it | **grants and recalls**. Not monotone |
| **consent** | *whose agreement does this action need* | an **access structure**: the family of sets of people who together may act. Monotone: adding someone to a set that may act still may | **by consent itself**: changing a rule needs the agreement the rule asks for |

Each relation also has a **weight** per person, derived and never stored, on an edge to
a shared measure rune; how they are computed and projected into an order is
[standing](/concepts/standing.md). What they govern, down to a single rune, is
[access](/concepts/access.md).

They are independent, and the independence is the point:

- **The IT person** holds the authority to enrol and remove devices for the whole
  vault, and no seal for payroll. High authority, low knowledge.
- **The bookkeeper** holds the payroll seal and no authority over devices. High
  knowledge, low authority.
- **The owner** of a small store holds both, and the vault's consent rules still
  say no single person may remove someone alone.
- **A front-desk worker** knows the Wi-Fi and the POS login, and may approve
  requests within the front-desk group.

Nobody here sits on a ladder, and the screen can still say *IT: admin*,
*bookkeeper: member*, *owner: owner* (below, §"Labels").

**Knowledge is the second arrow.** Palabra's history only grows, because the past
cannot be changed. Knowledge only grows, because a secret cannot be unseen. That is
why the [threat model](/concepts/threat-model.md) keeps saying *"cannot take back
what they saw"*: it is not a limitation of the design, it is the shape of the
relation. Authority, by contrast, can be recalled, so it must never be confused
with knowledge.

# Authority as delegation, not as level

A **capability** is a signed statement: *"the holder of this key may do these
verbs, to this region, under these conditions, until this time."* Regions are
written in the capability itself (a group, a secret, a tag of secrets, the vault's
membership), never computed from state that is still merging (Palabra's finding 3,
`VoidPalabra:/design/open-questions.md` §6.1). Four operations, as in Willow's
Meadowcap:

| operation | means |
|---|---|
| **source** | the founding authority over a vault, held by its first group |
| **delegate** | give some of what you hold to someone else, signed by you |
| **restrict** | give it narrower: a smaller region, fewer verbs, an earlier expiry |
| **merge** | hold several as one |

**You cannot delegate what you do not hold** (*"only connectivity begets
connectivity"*, the object-capability rule). So the authority graph is also an
honest map of where power came from: every capability has a chain back to the
source, and the [graph screen](/concepts/application.md) can draw it.

**"Higher" becomes a relation, not a number.** A holds *at least* B's authority over
a region when every verb B may do there, A may do there too. That relation is a
**partial order** (set inclusion), so it has incomparable pairs, exactly as the IT
person and the bookkeeper are. Rank is one of its **linear extensions**: there are
many, and the screen picks one for display, the way Palabra picks a timeline.

**It may even have cycles, and that is fine.** The IT person may suspend the
bookkeeper's devices; the bookkeeper's group may recall the IT person's mandate.
Warren McCulloch called this a **heterarchy** (1945): preferences in a nervous net
can be circular (A over B, B over C, C over A), which he read not as inconsistency
but as *"consistency of an order too high to permit construction of a scale of
values."* A system that checks each action against its own chain of delegation and
its own consent rule never needs a global scale, so it never has to break the
cycle.

# Mandates: authority can flow upward too

A ladder only grants downward. The anarchist and council traditions (Proudhon in
1848, the Paris Commune, the Zapatistas' *mandar obedeciendo*, "to govern by
obeying") insist on the other direction: a delegate holds power **by mandate from
those they act for, and those people can recall them.**

Verguenza takes this literally, because it is cheap to express and valuable for a
password manager:

1. **Every grant records whose consent made it.** A capability carries the
   approvals that authorized it ([quorum](/concepts/quorum.md)), so *who mandated
   this* is a fact on the capability, not a memory.
2. **Recall is easier than grant.** Granting the IT person device authority might
   need two owners; recalling it needs any one of the people whose consent created
   it, or the group that mandated them. Losing power is the safe direction: a wrong
   recall costs convenience, a wrong grant costs secrets. This asymmetry is the same
   instinct as [trust](/concepts/trust.md)'s rule 3 (*concurrent removals all take
   effect*), which p2panda's group management independently calls **strong
   removal**. [Q13](/developer_questions.md).
3. **Mandates can expire.** A capability may carry an end (Meadowcap's time range).
   A seasonal worker's front-desk access lasts the season; an IT contractor's lasts
   the contract. Renewal is a fresh grant with fresh consent, the way rotation
   worked in Athens. Whether grants expire **by default** is
   [Q13](/developer_questions.md).

# Groups inside groups

A vault is not a flat list of people. It is **groups that contain people and other
groups**: the front desk, the managers, IT, the owners, and the whole store
containing them. Keyhive (Ink & Switch) and p2panda both arrived at groups as
members of groups for local-first access control; Ostrom found the same shape in
long-lived human commons (*nested enterprises*, her eighth principle); sociocracy
calls them circles.

- **A group governs its own region** by its own consent rule: the front desk
  decides who among them may approve front-desk requests. Ostrom's third principle:
  most people affected by a rule can take part in changing it.
- **A group is present in its parent by delegates it chooses**, and recalls. That
  is sociocracy's double link (a leader carrying the parent's view in, a delegate
  carrying the group's view out), and it is what stops "the managers" from being a
  rank above "the front desk": they are a neighbouring group whose reach into the
  front desk's region is whatever the front desk's consent rule grants them.
- **An outer group can hold reach into an inner one** (the owners may suspend
  anyone), because the inner group was founded inside it. That is where the
  practical, top-down part of the author's *"in practice things will be linear"*
  comes from: not from a rank, but from **where a group was founded and what its
  founders kept**.

# Consent as an access structure, not a number

A quorum of *"2 of 5"* is the simplest access structure. Real groups need others:
*"the IT person, or any two managers"*, *"one owner and one person from the
affected group"*, *"anyone in the front desk, while present in the building"*.
Cryptographers have studied these since Ito, Saito and Nishizeki (1987) showed every
monotone access structure can be realised by secret sharing, and Benaloh and Leichter
(1988) showed how to build one from its **monotone boolean formula**:

```
enrol a device      =  IT  ∨  2-of(managers)
reveal payroll      =  bookkeeper ∧ 1-of(owners)
change front desk   =  2-of(front_desk) ∨ (1-of(front_desk) ∧ 1-of(managers))
```

So a policy ([quorum](/concepts/quorum.md)) is a formula over people and groups,
and *k of n* is one case of it. Some of these formulas cannot be written as any
threshold at all (Benaloh and Leichter proved it), which is the mathematical form of
"a ladder cannot express this".

**The formula is the same object whether it is enforced by software or by
mathematics.** A *guarded* policy counts approvals against it; a *split* policy
divides the key along it (Benaloh–Leichter's construction, generalizing Shamir). The
honest labels from [quorum](/concepts/quorum.md) stay: the formula does not change
what enforces it.

# "Higher priority" in a conflict

Palabra has already ruled on this, from a client asking for "admins first":
**precedence is a presentation, never a resolution** (finding 6). Roles are synced
state, so "whose side is the admin's" can differ between devices mid-sync; a silent
winner would turn that disagreement into divergence.

Verguenza keeps the ruling and adds the one exception it needs:

| conflict | what happens |
|---|---|
| two edits to a description | Palabra's conflict, shown to people; the side written by whoever holds more authority over that region may be **offered first**, never chosen silently |
| a grant and a recall at once | **recall wins** (strong removal). Not because the recaller ranks higher: because losing power is the safe direction |
| two recalls of each other | both take effect ([trust](/concepts/trust.md) rule 3) |
| two concurrent changes to a consent rule | a conflict, shown; until it is settled, **the stricter rule applies** (the one more sets of people fail) |

Nowhere does a rank decide anything. That is the whole architectural claim of this
page.

# Labels

*Owner*, *admin*, *member*, *viewer*, *guest* are **templates**: named bundles of
capabilities and memberships that the application uses to grant in one gesture and
to describe in one word.

- Granting from a template writes the capabilities, not the label.
- Describing a person **matches** their actual capabilities against the templates
  and shows the best match, or *custom* with what differs (*"admin, without
  payroll"*). The label can never disagree with the truth, because it is computed
  from it.
- A group can define its own templates (*shift lead*). Templates are description,
  not authority: editing one changes nothing anyone holds.

# What this asks for, and from whom

| need | owner |
|---|---|
| capabilities: source, delegate, restrict, merge; checking a signed change against one; regions written in the change | **Palabra** ([boundary](/concepts/boundary.md)), Meadowcap-shaped as its own §6 already proposes |
| groups of groups, mandates and recall, consent formulas, templates, the knowledge graph | **Verguenza** |
| evaluating a monotone formula over a set of signed approvals | **Verguenza**: pure, small, and tested as a pure function |
| splitting a key along a formula (split policies) | open: [Q1](/developer_questions.md) |

# Ideas considered and not taken (yet)

- **Authority computed from the graph** (Advogato's trust metric, Levien 1998:
  standing as network flow from trusted seeds, the idea PageRank shares). Elegant,
  and a good way to *show* how embedded someone is. Rejected for *authority*: a
  computed authority is an attack surface (Advogato's own resistance was later
  disputed), and a password manager's authority must be something a person
  granted on purpose.
- **Approvers chosen by lot** (Athenian sortition): a request drawn to random
  approvers from a pool, so collusion cannot be arranged in advance. Attractive
  for large groups. Needs randomness no requester can steer, which is a protocol of
  its own. Later, if a group asks.
- **Liquid delegation** (a person delegates their approval to someone, who may
  delegate on). Expressible as a capability over *approving*, with its cycles
  resolved by abstention. Not needed for a small business; recorded because
  Hormiga's larger organizations might.

The sources for each tradition, and how it translates, are in
[governance](/references/governance.md).
