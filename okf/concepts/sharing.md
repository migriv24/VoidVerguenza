---
type: Concept
title: Sharing — sync between the people who share a vault
description: "How a vault reaches more than one device: Void Palabra's merge over Reticulum links, the same stack every Void application uses. What travels (descriptions through the merge; envelopes and every other signed object through the ledger, by union), what never does (private keys, plaintext), why authority cannot live in the merge, revocation and its honest limit, working offline, and an always-on node for a group that is not always in one room."
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
| secret descriptions | yes (the state document, Palabra's merge) | every member's devices |
| the signed record: people, devices, groups, capabilities, requests, approvals, acts | yes (content-addressed objects, Palabra's content references; never removed by sync) | every member's devices |
| envelopes (signed objects too) | yes | only the devices each is sealed to can open the value |
| a device's private keys | **never** | — |
| a plaintext value | **never** | — |

A network observer sees Reticulum traffic between the vault's devices and
nothing else: Reticulum carries no source addresses and seals links. A member
sees what members see. Only a device a secret is sealed to can open it.

# Signed changes: the signed record, not the merge

The founding version of this section put a signature-and-role check in front of
Void Palabra's merge. That cannot work as written (amended 2026-10-01): Palabra
syncs whole merged documents rather than signed deltas, a remove in a merged
document cannot be attributed to anyone, and checking a change against roles that
are still merging gives different devices different verdicts
(`VoidPalabra:/design/open-questions.md` §6.1, findings 1, 3 and 5).

So authority does not travel in the merge at all. It travels in the vault's
**[signed record](/concepts/trust.md)**: signed objects (people, devices,
capabilities, envelopes, requests, approvals, acts) carried by Palabra, never
removed by sync, each judged by its own causal past the same way on every device. The merged document carries
descriptions, which open nothing, and a projection of the record that each device
rebuilds for itself.

- Every object a device makes is signed by that device.
- A device checks each object it receives against the roster as it stood at the
  heads the object names, and keeps it either way: a refused object is kept as
  evidence and given no authority.
- A device offering objects that fail their signature (not merely ones that are
  unauthorized, which honest concurrency produces) is shown to the members.

This is the "who may" layer Void Palabra deliberately left to applications.
The mechanism is Palabra's; the rules are Verguenza's ([boundary](/concepts/boundary.md), [Q5](/developer_questions.md)).

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
(a small computer in the back office) that relays links. **Amended 2026-10-01:** it
should also be a **keeper** ([recovery](/concepts/recovery.md)): it holds the replica
and every envelope, and **no seals**. So a device joining at 3 a.m. can sync from it,
and stealing it yields what stealing a bundle yields: names and sealed bytes. A
device that is always on and unattended is still the worst place to keep a seal, so
it never gets one.
