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
| **Social recovery (e.g. some cryptocurrency wallets)** | k of n trusted people restore access | recovery holders | — |

**What no one of them does, and Verguenza tries to:** per-device sealing,
k-of-n approval of individual actions with an honest label for how each is
enforced, no server, and other applications asking for a value with the
consequence of the action attached.
