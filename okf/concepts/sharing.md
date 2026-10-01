---
type: Concept
title: Sharing — sync between the people who share a vault
description: "How a vault reaches more than one device: Void Palabra's merge over Reticulum links, the same stack every Void application uses. What travels (descriptions, envelopes, signed commands), what never does (private keys, plaintext), why a merge cannot accept an unsigned or wrongly signed change, revocation and its honest limit, working offline, and an always-on node for a group that is not always in one room."
tags: [status:planned, audience:dev, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# The stack

Nothing new is invented here. Every Void application syncs the same way:

| layer | piece |
|---|---|
| the merge | Void Palabra (`VoidPalabra:/concepts/join.md`): two copies of a document converge, deletions stay deleted, conflicts surface as values a person settles |
| the session | Void Palabra's sync session (`VoidPalabra:/concepts/sync-session.md`) |
| the network | Reticulum, through `voidpalabra_reticulum`: links that are sealed and forward-secret, over LAN, TCP, or radio |
| discovery and joining | Void Maiz's `voidmaiz_net` on Reticulum (`VoidMaiz/include/voidmaiz/rnslink.hpp`): announce, join, Allow/Deny |

Void Hormiga runs on exactly this. Verguenza adds one thing a database of
contacts did not need: **every change must be signed by a device the vault
trusts, and every device checks before it merges.**

# What travels

| thing | travels | readable by |
|---|---|---|
| secret descriptions, members, devices, policies, clients, requests | yes (the state document) | every member's devices |
| envelopes | yes (content-addressed blobs, Palabra's content references) | only the devices each is sealed to |
| approvals and the signed log | yes | every member's devices |
| a device's private keys | **never** | — |
| a plaintext value | **never** | — |

A network observer sees Reticulum traffic between the vault's devices and
nothing else: Reticulum carries no source addresses and seals links. A member
sees what members see. Only a device a secret is sealed to can open it.

# Signed changes

Void Palabra's merge accepts any well-formed change by design; it is a library
about convergence, not permission (`VoidPalabra:/concepts/reticulum.md`:
*"Reticulum answers who is this. It never answers may they."*). So Verguenza
puts the permission check in front of it:

- Every command a device makes is signed by that device.
- Before merging a peer's changes, a device checks each signature against the
  `devices` mantle **as it stood when the change was made**, and checks the
  signer's role allowed it (adding a member needs an admin, and the policy's
  approvals).
- A change that fails is not merged, and the device records that it was
  offered one. A device offering bad changes repeatedly is shown to the members.

This is the "who may" layer Void Palabra deliberately left to applications. It
is also a candidate to move into Palabra once a second application needs signed
merges. [Q5](/developer_questions.md).

# Revocation, and its honest limit

Removing a member or a device:

1. stops their future signatures counting, from the moment of removal;
2. re-seals every envelope they could open, without them (new DEKs);
3. **cannot take back what they saw.** The values they could open must be
   changed at their vendors to be safe, and only people can do that. The
   application lists those secrets and marks them *needs rotation*.

A removed device that comes back online still holds the old envelopes and the
seals it had. It can still open old values it already could. It cannot open any
value written after its removal.

# Offline

A vault works on one computer with no network, forever. Two devices that have
not met for a week converge when they meet. A request needing approvals waits,
or expires, while approvers are away.

# A group not always in one room

A store's staff are in the building; a vault's owner may not be. Reticulum
reaches across the internet through a **transport node**: an always-on device
(a small computer in the back office) that relays links. It holds the vault's
envelopes like any member device would if it is enrolled as one, or relays
without holding anything if it is only a Reticulum relay. **Lean: a relay that
holds nothing.** A device that is always on and unattended is the worst place to
keep seals.
