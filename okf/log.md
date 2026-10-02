---
type: Log
title: Log
description: The running history of Void Verguenza, newest last.
tags: [status:current, audience:dev, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# Founded (2026-10-01)

Founded by the author in a Void Hormiga session, together with Void Chisme
(network observation, `../VoidChisme`). Both are separate applications; the
author gave them to the session that had just built Hormiga's Antfarm v2 because
*"there's a lot we can take from hormiga, or plan with hormiga in mind"*.
Instruction: OKF only, no code yet; desktop first, phone later.

The brief, in the author's words: a Void Maiz application for passwords and
secrets in general, *"assuming that multiple people might be using this
software"*, with *"lan based authentication (such as having multiple devices
authenticate an action)"*, *"extra encryption and protections in general"*, and
structures like Hormiga's Antfarm Key node, so that *"eventually the antfarm will
reference Verguenza's structures to use instead of our current api key node."*

What the founding bundle decides (all leans until the author answers):

- A secret is a **description** (a rune every member can read) and a **value**
  (a sealed envelope only named devices can open). Hormiga's Key node had the
  same split.
- **Envelopes**: one key per value (XChaCha20-Poly1305), sealed to each device
  (X25519 sealed boxes), signed by the device that made it (Ed25519). libsodium
  only.
- **Device keys rest under a passphrase only the person knows** (Argon2id,
  moderate limits): deliberately stricter than Hormiga's profile-derived vault key.
- **People are not devices.** Enrolment is in person, with a short code both
  screens show (Hormiga's LAN pairing).
- **Quorum** in three strengths, never confused: open, guarded (honest software
  counts approvals) and split (threshold sharing, enforced by mathematics).
  Approvals are signed, time-limited, and can be required to come from the local
  network.
- **Sync is Void Palabra over Reticulum**, with a signature-and-role check in front
  of the merge.
- **Clients** (Hormiga first) ask for a value at the moment of use, carrying the
  effect's consequence sentence to whoever must approve.
- Eight open questions; the first (threshold sharing is not in libsodium) gates
  split secrets.

Hormiga's side of the seam is recorded in Hormiga's OKF
(`../VoidHormiga/okf/concepts/projects/void-verguenza.md`).

# The design checked against the siblings, and phase 1 planned (2026-10-01)

The author: *"continue development of this application, specifically let's
continue with the okf and planning first."* So no code; the founding bundle was
read against what Void Core, Void Maiz, Void Palabra and Void Hormiga have
actually built, and the plan was taken down to steps a session can build.

**What reading the siblings found** (each is now folded into a concept):

- **The signature check in front of the merge could not work.** Void Palabra's
  own trust findings (`VoidPalabra:/design/open-questions.md` §6.1) say sync
  ships whole merged documents rather than signed deltas, removes in a merged
  document cannot be attributed, and checking a change against roles that are
  still merging diverges. The founding [sharing](/concepts/sharing.md) page
  assumed all three away. **New page: [trust](/concepts/trust.md).** Authority
  moves into a **ledger**: a grow-only set of signed, content-addressed objects,
  each naming the roster heads it was written against and judged by its own
  causal past; removals carry a **cutoff** so a removed device cannot backdate;
  concurrent removals all take effect; the Core document becomes a projection.
  Phase 3 no longer waits on Palabra's trust work. [Q5](/developer_questions.md)
  reframed; [Q10](/developer_questions.md) new.
- **A share, and a client's value, were trusting a Reticulum link for
  confidentiality.** Reticulum's link cryptography is its own (AES-256-CBC,
  HMAC-SHA256, a vendored Crypto library, ten measured microReticulum defects),
  not libsodium. Now both are sealed with `crypto_box_seal` to the receiving
  device's (or client's) own key, and the link is only a transport
  ([quorum](/concepts/quorum.md), [clients](/concepts/clients.md)). The rule,
  stated once: **nothing secret relies on the link.**
- **Recovery would have opened every split secret.** A recovery seal on every
  envelope lets any k recovery holders open a split secret, whatever its own
  holders. Lean: no recovery seal on split secrets by default
  ([quorum](/concepts/quorum.md), folded into [Q4](/developer_questions.md)).
- **Strengthening a policy protects only the next value**: devices that held
  the whole DEK can still open the old envelope. *"Make this split"* now means
  *"and change the value"* ([quorum](/concepts/quorum.md)).
- **Void Core writes every command line down** (the log spine and the journal's
  canonical text), and has no host-registered verbs. **New page:
  [commands](/concepts/commands.md).** Verguenza verbs compile to ordinary Core
  commands, as Hormiga's `farm` verbs do; values enter only through **intake**
  slots; a `--value=` that is not a slot is refused before the dispatcher; a
  reveal (and a client's use) is **recorded before it is shown**, and not done if
  it cannot be recorded.
- **Void Maiz's text fields are Dear ImGui's**, which keeps a working copy of the
  text being edited. Not assumed either way: phase 1 ends with a memory scan,
  and a Void Maiz message follows if the copy survives
  ([threat model](/concepts/threat-model.md)).
- **MSVC cannot build Palabra's Reticulum companion** (Palabra `a02b154`), so
  Verguenza builds with GCC under MSYS2 UCRT64, as Hormiga does
  ([architecture](/concepts/architecture.md)).
- **Hormiga's vendored libsodium is a prebuilt Windows binary**, which Hormiga
  itself found to be a one-platform dependency. [Q9](/developer_questions.md),
  leaning to vendoring the signed sources.
- **The enrolment code needed a commitment**, or a device in the middle could
  search for keys that make six digits match. Now commit, then reveal, over
  Verguenza's own keys with `crypto_generichash`
  ([people and devices](/concepts/people-and-devices.md)).

**New pages:** [trust](/concepts/trust.md), [commands](/concepts/commands.md),
[formats](/concepts/formats.md) (the signing rule with a type prefix on every
signature; envelope v1, whose associated data leaves out the seals so a re-seal
needs no re-encryption; each device's objects chained by `previous`, so a gap or
a fork shows; the keyring with stored, floor-checked Argon2id limits),
[architecture](/concepts/architecture.md) (five targets; `voidverguenza` links
only libsodium so Hormiga can link it; the `Secret` type; the plaintext grep,
the memory scan, and a check that every plaintext path has its comment).

**Trued up:** [sharing](/concepts/sharing.md), [secret](/concepts/secret.md)
(the envelope field is for display; the current value is computed from the
ledger), [sealing](/concepts/sealing.md), [quorum](/concepts/quorum.md),
[people and devices](/concepts/people-and-devices.md) (one identity per vault,
[Q11](/developer_questions.md); a lost device's removal carries a cutoff),
[audit](/concepts/audit.md), [clients](/concepts/clients.md),
[threat model](/concepts/threat-model.md) (five rows defended, four more not),
[prior art](/references/prior-art.md), [roadmap](/roadmap.md) (phase 0 in four
steps, phase 1 in twelve, each with its test; the v1 formats freeze at phase 1's
exit), the [index](/index.md).

**Upstream:** a message for Void Palabra is drafted at the repository root
(`MESSAGE_FOR_VOIDPALABRA_verguenza-a-ledger-instead-of-signed-merges-2026-10-01.md`),
offering the ledger as one answer to its §6.1 findings 1, 3 and 5, for the author
to relay.

**Waiting on the author:** Q9 before phase 0's libsodium step; the rest have
leans phase 1 can build on.

# Reticulum, Palabra first, and hierarchy without a line (2026-10-01)

The author, after the planning session:

> remember we wanna see about reticulum as much as possible, not libsodium (because
> it doesn't work on mobile)
>
> we should use whatever functionality is specific to palabra as much as possible,
> really drawing that philosophical and architectural line.
>
> … we should begin work on actually managing the admin aspect of things. because
> heiarchy matters … Similar to how palabra re-invented git, verguenza might have to
> reinvent what it means to be "higher priority" in a graph based structural
> enviornement. … Note that heiarchy is also a linear dimension, similar to how time
> was. … it might be useful to look into niche political systems or something (like
> anarchism maybe?) and how those systems could mathematically translate …
>
> we will continue with 1 identity per device for now.
>
> Recovery is also interesting, because where is something being stored and
> recovered? who owns that storage? what process untangles the recovery? … It might
> be very useful to closely read and look into palabra for this sort of thing.

**Ground rule 3 amended** (`CLAUDE.md`): cryptography comes from Reticulum through
Palabra; libsodium only where an open question's answer says so; operating-system
facilities are not primitives. **Rule 3a added:** Palabra first: anything statable
without *person*, *secret*, *consent* or *knowledge* is Palabra's.

**New pages:**
- [Boundary](/concepts/boundary.md): every responsibility assigned to one owner
  (Reticulum: identity; Palabra: memory; Verguenza: standing; the app: screens), and
  what Verguenza must ask Palabra for.
- [Authority](/concepts/authority.md): hierarchy as a **projection**, the way
  Palabra's timeline is a linear extension of a partial order. Three relations kept
  apart: **knowledge** (only grows, the second arrow beside time), **authority**
  (capabilities delegated, restricted, merged, recalled; a partial order by
  inclusion, cycles allowed: McCulloch's heterarchy), **consent** (monotone formulas,
  `IT ∨ 2-of(managers)`, after Ito–Saito–Nishizeki and Benaloh–Leichter). Mandates flow
  upward too (Proudhon, the Commune, *mandar obedeciendo*): **recall is easier than
  grant**. Groups inside groups (Ostrom's nesting, sociocracy's double link, Keyhive,
  p2panda). "Higher priority" never silently wins: Palabra's *precedence is a
  presentation* stands, with one exception, recall beats grant. Labels (*owner,
  admin, guest*) are templates, computed, never stored.
- [Recovery](/concepts/recovery.md): four losses, four owners. Data comes back by
  sync (Palabra); keys never (a new device is a new identity and a fork); standing by
  the group's consent; access by **re-sealing from the people who still know**. A
  cold holder (cold device, or a split recovery identity) only when nobody who knows
  is left. A **keeper** holds every envelope and no seal.
- [Governance](/references/governance.md): the research, each tradition checked
  against a source and translated.

**Rewritten onto Reticulum:** [sealing](/concepts/sealing.md) (token +
encrypt-to-identity + sign; the header digest inside the plaintext replaces associated
data), [formats](/concepts/formats.md) (device id = identity hash; the keyring file
gone), [architecture](/concepts/architecture.md) (no cryptography in this
repository; `voidverguenza` links Palabra; the `Secret` type on OS memory calls),
[trust](/concepts/trust.md) (the "ledger" becomes **Palabra's mechanism, Verguenza's
rules**: a signed record of signed utterances, kept at the Full tier).

**What reading Palabra's vendored Reticulum found** (Crypto `984dc89`,
microReticulum `40fa628`):
- **The random generator is not seeded from the operating system** on desktop or
  Android builds: constants, the string "Reticulum", the compile time, and a 32-bit
  microsecond clock value per rekey. Measured: two fresh identities differ.
  Estimated: identity and link keys are searchable offline by someone who can guess
  uptime. Affects every Void application on Reticulum. **Reported as priority**
  (`MESSAGE_FOR_VOIDPALABRA_verguenza-reticulum-randomness-2026-10-01.md`), with a fix
  through the public `RNG.stir` and the order it must happen in.
- The token's MAC check is not constant-time; `randomnum()` has 8 bits.
- Palabra's header exposes none of sign, verify, encrypt-to-identity, decrypt or
  token; decrypt returns plaintext in never-wiped `Bytes`; the identity is a plain
  file. Asked in
  `MESSAGE_FOR_VOIDPALABRA_verguenza-identity-operations-and-a-trust-client-2026-10-01.md`,
  which **replaces** the earlier draft `…a-ledger-instead-of-signed-merges…` (deleted
  before relay; it still assumed libsodium and a Verguenza-owned ledger).
- No password-hardening function: [Q12](/developer_questions.md).

**On "libsodium doesn't work on mobile"**: recorded fairly in
[sealing](/concepts/sealing.md). Hormiga built libsodium for Android arm64 on
2026-09-24 (not yet run on a phone). The move stands on the architectural reason:
one identity system per family.

**Questions:** Q3 and Q11 **answered** (one identity per device, and it is the
Reticulum identity). Q2 and Q9 superseded by **Q12** (identity at rest). Q4 reworked
around cold holders, Q5 reframed (Palabra's mechanism, Verguenza's rules). New: **Q13**
(authority defaults), **Q14** (OS randomness for what Verguenza makes).

**Trued up:** [people and devices](/concepts/people-and-devices.md) (roles became
templates; the enrolment code on SHA-256), [quorum](/concepts/quorum.md) (consent
formulas; recovery moved), [sharing](/concepts/sharing.md) (the keeper),
[commands](/concepts/commands.md) (group, grant, recall, template verbs),
[clients](/concepts/clients.md), [audit](/concepts/audit.md),
[secret](/concepts/secret.md), [threat model](/concepts/threat-model.md),
[application](/concepts/application.md) (the graph shows three relations),
[prior art](/references/prior-art.md), [roadmap](/roadmap.md) (phase 1 split: **1A,
the authority model headless, needs nothing and can start now**; 1B waits on Palabra),
the [index](/index.md).

# Standing, access, and two products (2026-10-01)

The author pointed at Void Unity's handling of numbers and Void Allomone's user
graphs, and asked how the structure would land in Verguenza and elsewhere:

> *"if there is a value, its held and associated via an edge … a shared node of
> 'knowledge' and 'authority' and 'consent' and the weight between these three runes
> and to the runes of the members of the network, it would like, grow in weights … we
> would use some sort of other algorithm to actually figure out 'order' or the
> projection of order."*
>
> *"also yes you can edit palabra if youd like."*
>
> *"we may want to consider making Void Verguenza not actually come from void maiz.
> and we work on 2 types of application …"*
>
> *"this is about managing access to mantles, and access to the runes within a mantle
> as well (which would involve rules) … verguenza is all about access of information
> in a graph based world. where we have no heiarchy, but there still must be some sort
> of control in what is given to someone. There still has to be 'shame'."*

**Grant recorded (ground rule 7): the author grants Verguenza sessions direct edits to
Void Palabra, 2026-10-01.** Not yet used. The first planned uses are the two drafted
messages: seeding Reticulum's generator from the operating system, and exposing the
identity's operations in `reticulum.hpp`. Each edit follows Palabra's own CLAUDE.md
and OKF and is logged in both bundles.

**What reading the siblings gave:**
- **Void Core 0.2.14** already holds the author's picture: an edge's weight *is* an
  attribute's value when it points at a `measure` rune (§3.7.1), with the warning that
  weights in one mantle are not commensurable.
- **Void Unity**: no independent floats; numbers derived from facts keyed by source,
  never written back; conditions reach rules only as threshold tags with hysteresis.
- **Void Allomone**: inputs sync, derived values never do; *a goal ranks, and a lattice
  deliberately does not*; the shared user graph held back for privacy (its Q20).
- **Void Maiz's user graph**: weights that only grow (counts under addition), compiled
  into runes only on request.

**New pages:**
- [Standing](/concepts/standing.md): three measure runes, `knowledge`, `authority`,
  `consent`; each person's weight on an edge to each, **derived, never typed**
  (exposure from seals, reach from capabilities, **Banzhaf voting power** over consent
  formulas); order as a **dominance partial order** with named **lenses** for the
  screen; thresholds become tags; diagnostics for the group (single points of
  knowledge, concentration, the rotation gap). **A weight never decides.**
- [Access](/concepts/access.md): read by sealing, write by capability at admission,
  administer by consent; regions in Core's names (vault, mantle, glyph, rune). **Rules
  propose audiences, capabilities admit writes, seals decide reads**, which keeps
  tag-based rules (state-dependent) out of admission (Palabra's finding 3). A shrinking
  audience proposes rotation, never revokes. The costs of sealing inside a merge
  (coarser conflicts, per-device views).
- [Products](/concepts/products.md): **the foundation** (`voidverguenza`, on Palabra,
  without Maiz or Core) and **the application** (the password manager, its first
  client); what Hormiga, Chisme, Unity, Maiz, Allomone, Palabra and Core each gain.

**Questions:** new **Q15** (does the application use Maiz? lean: yes, the application
only), **Q16** (may compiled standing sync? lean: no), **Q17** (who sets a secret's
sensitivity? lean: defaulted from policy, overridable).

**Trued up:** [authority](/concepts/authority.md), [boundary](/concepts/boundary.md),
[architecture](/concepts/architecture.md), [application](/concepts/application.md),
[roadmap](/roadmap.md) (1A.8 access, 1A.9 standing), the [index](/index.md).

# Leans accepted; the application uses Maiz (2026-10-01)

The author: *"yes, the visual should use maiz. And i'll trust your lean for now on
things."* **Q15 answered**: the password manager's screens use Void Maiz; the
foundation stays free of Maiz and Core. **Every other open question is settled on its
lean for now** (Q1, Q4, Q5, Q6, Q7, Q8, Q10, Q12, Q13, Q14, Q16, Q17), kept listed in
[developer questions](/developer_questions.md) so building can reopen one with a log
entry. Notable consequences: Q12's lean keeps **libsodium for Argon2id only**, on
desktops, as the one exception ground rule 3 allows; Q14's lean takes Verguenza's own
randomness from the operating system.
