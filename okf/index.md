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
   member, exporting the vault: a policy can require *k of n* devices to approve,
   and can require them to be present on the local network. Where possible the
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
6. **Other applications ask; they do not hold.** Void Hormiga's Antfarm will
   name a Verguenza secret instead of holding a key, and ask for its value at the
   moment an effect needs it. See [clients](/concepts/clients.md).

# The vocabulary

| word | what it is |
|---|---|
| **vault** | one shared set of secrets and the people who share them. A `.verguenza` bundle, like Hormiga's `.miga` |
| **secret** | one thing to keep: a password, a key, a code, a note, a file. A rune whose content describes it; its value is never in the rune |
| **envelope** | a secret's value, encrypted, plus one **seal** per device allowed to open it |
| **member** | a person in the vault, with a role |
| **device** | one computer or phone a member uses, with its own signing and agreement keys (its Reticulum identity) |
| **policy** | what a secret or an action requires: who may open it, and how many approvals |
| **approval** | a signed statement from one device: *"yes to this request, until this time"* |
| **client** | another application allowed to ask for certain secrets, for certain uses (Hormiga is the first) |
| **grant** | what a client may ask for, and what each ask requires |

# Map

## concepts

- [Secret](/concepts/secret.md) — what a secret is as a rune: kinds, fields,
  providers and what a key is compatible with (carried over from Hormiga's Key
  node), expiry, rotation, cost.
- [Sealing](/concepts/sealing.md) — the cryptography: envelopes, per-device
  seals, the device key at rest under a passphrase, memory hygiene, the file
  format, what libsodium provides and what it does not.
- [People and devices](/concepts/people-and-devices.md) — members, roles,
  devices as Reticulum identities, enrolling a device with a code compared on
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

## the rest

- [Roadmap](/roadmap.md) — phases, each gated by an exit test.
- [Developer questions](/developer_questions.md) — open decisions with leans.
- [Log](/log.md) — the running history.
- [Prior art](/references/prior-art.md) — the password managers and secret
  stores this design learned from, and where it departs from them.

# Status

**2026-10-01: founded; the OKF is the whole project.** No code, no repository
history beyond this bundle. The first phase (a single-device vault) waits on the
author's answers to the questions that shape the file format.
