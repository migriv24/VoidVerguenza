---
okf_version: "0.1"
type: Index
title: Void Verguenza
description: The front door — what Void Verguenza is, the commitments it is built on, the vocabulary, where every other document lives, and the honest status (design only).
tags: [status:planned, audience:all, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# Void Verguenza

**Secrets that several people share, kept so that losing one device, or
trusting one person too much, is survivable.** A native C++20 desktop
application on Void Core, Void Maiz and Void Palabra. Founded 2026-10-01.

One sentence: **Void Verguenza keeps passwords, API keys and other secrets as
sealed envelopes that only named devices can open, syncs them between the people
who share them over Reticulum, records every use, and can require several of
those devices to agree before a secret is revealed or a person is added.**

The author, founding it:

> this is a void maiz based application that's about passwords and "secrets" in
> general. its just a storage software, however, the importance is going to be
> assuming that multiple people might be using this software. this software will
> need to consider lan based authentication (such as having multiple devices
> authenticate an action). extra encryption and protections in general. I want
> this to use similar structures as the API Key node there is in the antfarm,
> where eventually the antfarm will reference Verguenza's structures to use
> instead of our current api key node.

**Nothing on these pages is built.** Every concept is `status:planned`. A page
may claim `status:current` only with a `resource:` link to the code behind it,
the convention the whole Void family keeps.

**Desktop first.** A phone comes later, and it is the most natural *approver*
there is (the device in your pocket confirming what your desktop is about to
do), so the design keeps a phone in mind without building for it.

# The commitments

1. **A secret's value is sealed to devices, not to a server.** There is no
   server. Each secret's value is encrypted once with its own key, and that key
   is sealed separately to every device allowed to open it. A device that is not
   on the list holds bytes it cannot read. See [sealing](/concepts/sealing.md).
2. **A person is not a device.** People are members; each member has one or
   more devices, each with its own keys. Losing a phone revokes a phone, not a
   person. See [people and devices](/concepts/people-and-devices.md).
3. **Some actions need more than one yes.** Revealing a guarded secret, adding a
   member, exporting the vault: a policy can require the agreement of a set of
   people described by a **consent formula** (*k of n* is its simplest case), and
   can require them to be present on the local network. Where possible the
   requirement is enforced by mathematics (the secret cannot be decrypted with
   fewer approvals), and where it is enforced only by honest software, the design
   and the screen say so. See [quorum](/concepts/quorum.md).
4. **Everything is recorded.** Every addition, approval, reveal and use is a
   signed, logged dispatcher command, so the question *"who saw the alarm code,
   and when?"* has an answer that one member cannot quietly edit. See
   [audit](/concepts/audit.md).
5. **Local first, synced by Void Palabra over Reticulum.** A vault works on one
   computer with no network forever. Sharing is a network the members add, and
   it is end-to-end encrypted by construction. See [sharing](/concepts/sharing.md).
6. **Hierarchy is a projection, not the structure** (2026-10-01). Who *knows* a
   secret, who *may* act, and whose *consent* an action needs are three relations
   kept apart; *owner*, *admin* and *guest* are labels computed from them for the
   screen, the way Palabra computes a timeline from a partial order. See
   [authority](/concepts/authority.md).
7. **Other applications ask; they do not hold.** Void Hormiga's Antfarm will
   name a Verguenza secret instead of holding a key, and ask for its value at the
   moment an effect needs it. See [clients](/concepts/clients.md).

# The vocabulary

| word | what it is |
|---|---|
| **vault** | one shared set of secrets and the people who share them. A `.verguenza` bundle, like Hormiga's `.miga` |
| **secret** | one thing to keep: a password, a key, a code, a note, a file. A rune whose content describes it; its value is never in the rune |
| **envelope** | a secret's value, encrypted, plus one **seal** per device allowed to open it |
| **member** | a person in the vault, in one or more groups |
| **device** | one computer or phone a member uses; one Reticulum identity per device |
| **group** | people and other groups, governing their own region by their own consent rule |
| **capability** | a signed grant: these verbs, over this region, until this time, recallable by those whose consent made it |
| **consent formula** | whose agreement an action needs, written over people and groups (`IT ∨ 2-of(managers)`) |
| **template** | a named bundle of capabilities (*owner*, *admin*, *member*, *guest*), used to grant and to describe; never a rank |
| **knowledge** | who can open what, and who already has; it only grows |
| **standing** | knowledge, authority and consent as derived weights on edges to three measure runes; never decides anything |
| **lens** | a named way of projecting the partial order of standing into a list for the screen |
| **audience** | who a rule proposes should read a rune; sealing makes it a fact |
| **policy** | what a secret or an action requires: what enforces it, and a consent formula |
| **approval** | a signed statement from one device: *"yes to this request, until this time"* |
| **signed record** | a vault's authority: signed objects (people, devices, groups, capabilities, envelopes, requests, approvals, acts) carried by Palabra, never removed by sync, judged by each device the same way |
| **roster** | the part of the record that says who is in the vault: people, devices, groups, capabilities, policies |
| **keeper** | an always-on device that holds the replica and every envelope, and no seal |
| **cold holder** | a device or a split recovery identity that can open secrets and is not in daily use: the last resort of [recovery](/concepts/recovery.md) |
| **intake** | the guarded slot a value enters through; a command names the slot, never the value |
| **client** | another application allowed to ask for certain secrets, for certain uses (Hormiga is the first) |
| **grant** | what a client may ask for, and what each ask requires |

# Map

## concepts

- [Secret](/concepts/secret.md) — what a secret is as a rune: kinds, fields,
  providers and what a key is compatible with (carried over from Hormiga's Key
  node), expiry, rotation, cost.
- [Sealing](/concepts/sealing.md) — the cryptography, on Reticulum's primitives:
  envelopes, per-device seals, what Reticulum gives and what it does not (its
  randomness, measured unfit for keys today; no password hardening; no wiping),
  memory hygiene.
- [People and devices](/concepts/people-and-devices.md) — people, devices (one
  Reticulum identity each), enrolling a device with a committed code compared on
  two screens, losing one.
- [Quorum](/concepts/quorum.md) — the heart of it: actions that need several
  devices to agree, present on the network, within a window. Policy-enforced
  and cryptographically enforced, kept apart.
- [Sharing](/concepts/sharing.md) — sync over Void Palabra and Reticulum: what
  travels, what never does, revocation and its honest limits.
- [Audit](/concepts/audit.md) — the signed record of every use, and what it can
  and cannot prove.
- [Clients](/concepts/clients.md) — how Void Hormiga's Antfarm (and later any
  Void application) asks for a secret instead of holding one.
- [Threat model](/concepts/threat-model.md) — what Verguenza protects against,
  what it does not, and why the second list matters as much.
- [The application](/concepts/application.md) — the desktop screens: vaults,
  secrets, the approvals inbox, the graph of who can open what.
- [Boundary](/concepts/boundary.md) — **who does what**: Reticulum (identity),
  Palabra (memory), Verguenza (standing), the application (screens); what
  Verguenza asks Palabra for.
- [Authority](/concepts/authority.md) — **hierarchy without a line**: knowledge,
  authority and consent kept apart; capabilities, mandates and recall; groups in
  groups; consent formulas; labels as projections.
- [Recovery](/concepts/recovery.md) — four losses, four owners: re-sealing by the
  people who still know; keepers and cold holders.
- [Standing](/concepts/standing.md) — weights on edges to measure runes (Core
  0.14), derived never stored (Unity's law), order as lenses over a dominance
  partial order, Banzhaf power for consent; a weight never decides.
- [Access](/concepts/access.md) — to mantles and runes: read by sealing, write by
  capability, administer by consent; rules propose, capabilities admit, seals decide.
- [Products](/concepts/products.md) — the foundation (no Maiz, no Core) and the
  password manager; what Hormiga, Chisme, Unity, Maiz, Allomone, Palabra and Core
  gain.
- [Trust](/concepts/trust.md) — where authority lives: a signed record on
  Palabra's mechanism, judged by each object's own causal past, with cutoffs on
  removals; the Core document as its projection.
- [Commands](/concepts/commands.md) — the verbs, compiled to Void Core commands;
  intake, the only door a value uses; a reveal recorded before it is shown.
- [Formats](/concepts/formats.md) — the bytes: identifiers, the signing rule, the
  envelope v1 on Reticulum's token, signed objects, and readers that refuse.
- [Architecture](/concepts/architecture.md) — targets and layers, the toolchain
  (GCC, not MSVC), no cryptography in this repository, the `Secret` type, the tests
  that keep the ground rules true.

## the rest

- [Roadmap](/roadmap.md) — phases, each gated by an exit test.
- [Developer questions](/developer_questions.md) — open decisions with leans.
- [Log](/log.md) — the running history.
- [Prior art](/references/prior-art.md) — the password managers and secret
  stores this design learned from, and where it departs from them.
- [Governance](/references/governance.md) — political systems and their
  mathematics: heterarchy, mandates, sociocracy, Ostrom, sortition, lattices,
  capabilities, access structures, and the two nearest local-first systems.

# Status

**2026-10-01: founded; the OKF is the whole project.** No code yet.

**2026-10-01, second session: planned down to phase 1's steps.** The design was
checked against what the siblings have actually built, which moved authority out
of the merge and into the [ledger](/concepts/trust.md), specified the
[commands](/concepts/commands.md), [formats](/concepts/formats.md) and
[architecture](/concepts/architecture.md), and broke phases 0 and 1 into tested
steps ([roadmap](/roadmap.md)).

**2026-10-01, third session: Reticulum, Palabra first, and hierarchy without a
line.** At the author's direction the cryptography moved to Reticulum through
Palabra ([sealing](/concepts/sealing.md)), every responsibility was assigned to one
owner ([boundary](/concepts/boundary.md)), hierarchy was rethought as a projection
of three relations ([authority](/concepts/authority.md), researched in
[governance](/references/governance.md)), and recovery became re-sealing by the
people who still know ([recovery](/concepts/recovery.md)). Reading Palabra's
vendored Reticulum found its random generator **not fit for keys** on desktop and
Android. **Phase 0 and phase 1A (the authority model) can start now; everything that
makes a key waits on Palabra** ([roadmap](/roadmap.md)).

**2026-10-01, fourth session: standing, access, two products.** Values on edges
(Core 0.14) and Unity's no-independent-floats law give [standing](/concepts/standing.md);
[access](/concepts/access.md) extends the structure to any mantle and rune;
[products](/concepts/products.md) splits the Maiz-free foundation from the password
manager. The author granted direct edits to Void Palabra ([log](/log.md)).
