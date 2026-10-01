---
type: Questions
title: Developer questions
description: Open decisions for the author, each with a lean so that silence has a sensible default. Answers fold into the concepts and clear from here.
tags: [status:current, audience:author, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# Open

- **Q1 — Threshold sharing is not in libsodium. What do we use?** Split secrets
  and social recovery need Shamir's secret sharing. Options: (a) vendor a small,
  reviewed, constant-time implementation over GF(256) as source, with its
  license, behind one `split`/`combine` pair; (b) write one ourselves; (c) drop
  split secrets and keep only guarded ones. **Lean: (a).** Writing our own breaks
  the spirit of "never hand-roll", even though Shamir is not a cipher, because the
  subtle parts (constant time, randomness for coefficients, share validation) are
  exactly where homemade code fails. Choosing *which* library is research for
  phase 5. ([sealing](/concepts/sealing.md), [quorum](/concepts/quorum.md))

- **Q2 — Convenience unlock with the operating system's keychain?** **Lean:
  offered, off by default, per device, with its cost on screen** (*anyone who can
  log in as you can open this vault*). Hormiga chose the convenient end for its
  vault; a password manager should start at the strict end.
  ([sealing](/concepts/sealing.md))

- **Q3 — A device's Verguenza keys and its Reticulum identity: the same keys or
  separate?** **Lean: separate, bound by a signature**, so a transport identity can
  be replaced without re-sealing, and no key does two protocols' jobs.
  ([people and devices](/concepts/people-and-devices.md))

- **Q4 — Social recovery: required when a vault is created, or optional?**
  **Lean: strongly offered at creation, skippable, and the skip stated plainly**
  (*"if every owner device is lost, this vault cannot be recovered"*). Forcing it
  on a single person's vault would be wrong; hiding the consequence of skipping
  would be worse. ([quorum](/concepts/quorum.md))

- **Q5 — Signed merges: Verguenza's, or Void Palabra's?** Checking every change's
  signature and role before merging is a "who may" layer Palabra deliberately left
  to applications. **Lean: build it in Verguenza first, and offer it to Palabra
  when a second application needs it** (Hormiga's members registry will).
  ([sharing](/concepts/sharing.md))

- **Q6 — Hardware security keys (FIDO2) as approvers?** Would need libfido2,
  vendored. **Lean: later; design approvals so a hardware key is just another kind
  of approving device.**

- **Q7 — How does a client reach the agent: a Reticulum link, or an operating
  system pipe?** **Lean: Reticulum**, because it is the family's network, it is
  already encrypted and authenticated, and it lets a client on another device ask
  too. Measure its local latency in phase 2 before committing.
  ([clients](/concepts/clients.md))

- **Q8 — Can a vault hide even the names of its secrets from members?** **Lean:
  per secret (`hidden`), not per vault**: names are what make a shared vault
  useful. ([secret](/concepts/secret.md))
