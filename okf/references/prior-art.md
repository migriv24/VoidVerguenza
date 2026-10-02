---
type: Reference
title: Prior art
description: "Password managers and secret stores Verguenza learned from, and where it departs. Recalled, not re-checked against current documentation on 2026-10-01: good enough to design against; anything load-bearing gets checked when it is built."
tags: [status:current, audience:dev, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

**How to read this page.** The descriptions are from memory, accurate to the best
of the founding session's knowledge, and not verified against each project's
current documentation. A design decision that rests on one of them must re-check
it first.

| tool | what it does | what Verguenza takes | where Verguenza departs |
|---|---|---|---|
| **KeePass / KeePassXC** | a single encrypted file, opened with a master password (and optionally a key file); local only | local-first, a file you own, Argon2 for the master key | sharing a KeePass file means sharing one master password; Verguenza seals per device |
| **Bitwarden / Vaultwarden** | end-to-end encrypted vault on a server; organizations and collections share items | per-item keys wrapped for members; organizations with roles | needs a server; Verguenza has none (Reticulum peers instead) |
| **1Password** | vaults shared within a team; a secret key plus a password derives the account key | separating a device-held secret from the password a person remembers | a company service |
| **pass / gopass** | one file per secret, encrypted with GPG to every recipient's key, kept in git | **one envelope per secret, sealed to each recipient**: the closest ancestor | GPG and git are hard to use; no quorum |
| **age / SOPS** | modern file encryption to several recipients; SOPS encrypts values inside config files | multi-recipient encryption with small, modern primitives | files, not an application |
| **HashiCorp Vault** | a server for secrets; "unsealing" it needs k of n key shares (Shamir) | **k-of-n unsealing**: the model for split secrets and recovery | a server, an operator, a cluster |
| **Keybase** | per-device keys; a new device is added by an existing one; a lost device is revoked | **per-device keys and device-to-device provisioning** | a company service, now dormant |
| **Signal's safety numbers** | two people compare a number to rule out a middleman | the short code on both screens | — |
| **Bluetooth numeric comparison, ZRTP's short authentication string** | two devices commit to their keys before revealing them, then show a short code to compare | **commit, then reveal**: why six digits are enough at enrolment ([people and devices](/concepts/people-and-devices.md)) | — |
| **Keybase signature chains** | each user's history is a hash-linked chain of signed statements: devices added, devices revoked | per-device chains of signed objects ([trust](/concepts/trust.md), [audit](/concepts/audit.md)) | a server orders Keybase's chains; Verguenza's ledger has no order but the partial one |
| **Matrix authorization rules** | each event in a room's graph is checked against the room state at its own place in the graph, with no server in charge of the room | **judging an object by its own causal past** ([trust](/concepts/trust.md) rule 1) | Matrix's state resolution is far more elaborate; Verguenza needs it only for a small roster |
| **p2panda-auth** | offline-first group management as a DAG of operations; *strong removal* | concurrent removals all stand; a removed member's concurrent actions are void ([trust](/concepts/trust.md)) | its access levels are a ladder; Verguenza's are not ([authority](/concepts/authority.md)) |
| **Keyhive (Ink & Switch)** | local-first access control: convergent capabilities, groups in groups, coordination-free revocation | groups in groups; capabilities that agree with CRDTs | not yet read in depth ([governance](/references/governance.md)) |
| **Social recovery (e.g. some cryptocurrency wallets)** | k of n trusted people restore access | recovery holders | — |

**What no one of them does, and Verguenza tries to:** per-device sealing,
k-of-n approval of individual actions with an honest label for how each is
enforced, no server, and other applications asking for a value with the
consequence of the action attached.
