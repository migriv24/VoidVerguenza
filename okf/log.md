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
