---
type: Questions
title: Developer questions
description: Open decisions for the author, each with a lean so that silence has a sensible default. Answers fold into the concepts and clear from here.
tags: [status:current, audience:author, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# Leans accepted for now

The author, 2026-10-01: *"i'll trust your lean for now on things."* Every question
below is **settled on its lean** until reopened, and the concepts are built on it.
They stay listed here, with their options, because "for now" means each can be
reopened when building shows a lean was wrong; reopening one is a log entry, not a
quiet change.


- **Q1 — Threshold sharing: what do we use?** Split secrets and cold recovery
  holders need secret sharing, now along a **consent formula**, not only *k of n*
  ([authority](/concepts/authority.md): Benaloh–Leichter's construction generalizes
  Shamir's). Neither Reticulum nor libsodium has it. Options: (a) vendor a small,
  reviewed, constant-time Shamir implementation over GF(256) as source and build
  formulas from it; (b) write one ourselves; (c) drop split secrets and keep only
  guarded ones. **Lean: (a).** The subtle parts (constant time, randomness for
  coefficients, share validation) are where homemade code fails. Choosing *which*
  library is research for phase 5. ([sealing](/concepts/sealing.md),
  [quorum](/concepts/quorum.md))

- **Q4 — Is a cold holder offered when a vault is created?** *(Reworked
  2026-10-01 around [recovery](/concepts/recovery.md).)* Recovery is now mostly
  re-sealing by the people who still know; a **cold holder** (a cold device, or a
  recovery identity split among holders) is needed only when nobody who knows a
  secret is left. **Lean: offered at creation, skippable, the skip stated plainly**
  (*"if every device that knows a secret is lost, that secret is lost"*), and **a
  cold holder never reaches a split secret by default**: a policy may opt one in,
  and the screen says what that means. For a one-person vault the application
  presses harder, because there re-sealing has no one to come from.

- **Q5 — The signed record: Palabra's mechanism, Verguenza's rules?** *(Reframed
  again 2026-10-01, at the author's direction to use Palabra as much as possible.)*
  The pieces (content-addressed changes naming their parents, a graph that holds an
  unplaceable change, content references, and the planned signed deltas and
  Meadowcap-shaped capabilities) are all Palabra's ([boundary](/concepts/boundary.md),
  [trust](/concepts/trust.md)). **Lean: Palabra owns the mechanism; Verguenza owns
  the rules (judge by causal past, cutoffs, removals win, authority only through
  valid objects) and the social model. Until Palabra has signed utterances and
  capabilities, Verguenza writes its objects in a frame shaped to move there, behind
  an interface named for what it asks Palabra for.** A message describing this is
  drafted at the repository root.

- **Q6 — Hardware security keys (FIDO2) as approvers?** Would need libfido2,
  vendored. **Lean: later; design approvals so a hardware key is just another kind
  of approving device.**

- **Q7 — How does a client reach the agent: a Reticulum link, or an operating
  system pipe?** **Lean: Reticulum**, more firmly now that every other path is
  Reticulum too. Measure its local latency in phase 2. A related constraint found
  2026-10-01: one Reticulum node per process, and two running nodes may not share a
  storage folder, so `verguenza-cli` and the application cannot both hold the
  device's identity at once; the CLI may have to ask the running application.
  ([clients](/concepts/clients.md))

- **Q8 — Can a vault hide even the names of its secrets from members?** **Lean:
  per secret (`hidden`), not per vault**: names are what make a shared vault
  useful. ([secret](/concepts/secret.md))

- **Q10 — When two removals happen at once, do both take effect?** **Lean: yes,
  both** (rule 3 in [trust](/concepts/trust.md)): between *keep* and *remove* a
  security system should fall to *remove*. p2panda's group management reached the
  same rule independently (*strong removal*, [governance](/references/governance.md)).
  The alternative (a deterministic tiebreak) keeps one admin but lets a stolen
  device win a race by luck.

- **Q13 — Authority defaults.** Four leans from [authority](/concepts/authority.md),
  each a default a group can change:
  1. **Recall is easier than grant**: any one of those whose consent made a grant
     may recall it. *Lean: yes.*
  2. **Grants expire by default** (a mandate, renewed with fresh consent). *Lean:
     no for people's base membership, yes for elevated capabilities (device
     authority, client grants), with a long default (a year) the group can change.*
  3. **Labels are computed, never stored.** *Lean: yes; the author already said
     labels are for the screen.*
  4. **A concurrent conflict over a consent rule applies the stricter rule until
     settled.** *Lean: yes.*

- **Q14 — Where should Verguenza get the randomness for the keys it makes itself?**
  Value keys, nonces for the enrolment code and request ids are Verguenza's, not
  Reticulum's. Options: (a) Reticulum's generator, once Palabra seeds it properly;
  (b) the operating system's generator directly (`BCryptGenRandom`, `getrandom`,
  `SecRandomCopyBytes`), from `verguenza_platform`. **Lean: (b).** It is the
  operating system's facility, not a cryptographic primitive anyone writes, it is
  the source Reticulum itself should be stirring from, and it unblocks value keys
  without waiting on Palabra. Identity keys stay Palabra's and wait for its fix.

- **Q16 — May a compiled standing mantle sync?** Standing weights are derived and
  stored nowhere by default ([standing](/concepts/standing.md)). A view that compiles
  them into the document creates runes *describing people* (who knows most, who holds
  most power). **Lean: compiled standing never syncs; each device derives its own**,
  the same lean Allomone took for the user graph (its Q20), and for the same reason:
  un-sharing something already synced is not possible.

- **Q17 — Who sets a secret's sensitivity?** Knowledge is weighted by it
  ([standing](/concepts/standing.md)). **Lean: defaulted from the secret's policy
  (open 1, guarded 3, split 9), overridable per secret by whoever may change that
  secret's policy**, so the weight is a leaf someone chose, never a number the system
  invented.

# Answered

- **Q12 — What protects a device's identity at rest?** **The operating system's key
  store; no libsodium at all** (the author, 2026-10-01: *"i really dont think we need
  libsodium if we have reticulum"*). On Windows that is DPAPI (`CryptProtectData`),
  which opens for anyone logged in as the person and for no one else; the lock screen
  says so in words. Phones get their hardware-backed stores when phones come. A
  passphrase returns only if Reticulum (or the platform) offers a password-hardening
  function; composing one ourselves stays refused.

- **Q15 — Does the visual application use Void Maiz?** **Yes, the application only**
  (the author, 2026-10-01: *"yes, the visual should use maiz."*). The foundation stays
  free of Maiz and Core ([products](/concepts/products.md)).

- **Q3 — A device's Verguenza keys and its Reticulum identity: same or
  separate?** **Same**, by the author's direction (2026-10-01: one identity per
  device; Reticulum as much as possible). Sealing to a Reticulum identity is
  Reticulum's own use of its key, and Verguenza's signatures carry a prefix no
  Reticulum message has. Folded into [people and devices](/concepts/people-and-devices.md),
  [formats](/concepts/formats.md).
- **Q11 — One device identity for all vaults, or one per vault?** **One per
  device**, the author, 2026-10-01: *"we will continue with 1 identity per device
  for now."* The lean (one per vault) is set aside; the cost (two vaults can tell a
  shared device is the same) is recorded in
  [people and devices](/concepts/people-and-devices.md).
- **Q2 — Convenience unlock** and **Q9 — How libsodium arrives**: superseded by
  Q12 when the author moved the design onto Reticulum.
