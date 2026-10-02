---
type: Concept
title: People and devices
description: "Members are people; devices are the computers and phones they use, each one Reticulum identity, so losing a phone revokes a phone and not a person. Where roles went (to authority: labels are projections of capabilities and groups); enrolling a device in person, with a committed short code both screens show (Void Hormiga's pairing, carried over); one identity per device, and it is the Reticulum identity; presence; losing a device; a member leaving."
tags: [status:planned, audience:dev, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# Members and devices are different things

Void Hormiga learned this the long way: a profile is *a credential on a device,
not a person* (`../VoidHormiga/okf/concepts/platform/identity.md`). Verguenza
starts there.

| | member | device |
|---|---|---|
| is | a person | a computer or a phone |
| has | a name, places in groups, capabilities, the devices they own | one Reticulum identity ([sealing](/concepts/sealing.md)), an owner, a kind (desktop, laptop, phone) |
| is added by | a member with the right to add members, under policy | its owner, on the device, confirmed by an existing device |
| when it is lost | the member removes the device, or others do | its seals are dropped from every envelope on the next re-seal |

**A secret is sealed to devices.** Saying *"Ana can open the alarm code"* means
the envelope carries a seal for each of Ana's devices. Adding a new laptop for
Ana means re-sealing what Ana may open to that laptop.

# Roles became authority

The founding page had a ladder here (owner, admin, member, viewer), each rung
allowing more. **Amended 2026-10-01**, at the author's direction: hierarchy is a
projection, and the structure underneath is a graph of groups, capabilities and
consent rules ([authority](/concepts/authority.md)). *Owner*, *admin*, *member*,
*viewer* and *guest* survive as **templates**: a way to grant in one gesture and to
describe in one word, computed from what a person actually holds, never stored as a
rank.

What stays from the founding page: authority is **enforced by signatures**. Every
change to people, devices, groups, capabilities or policies is a signed object, and
every device judges it by its causal past ([trust](/concepts/trust.md)) before it
counts. A modified client could *show* itself something it should not; it cannot
*open* anything not sealed to it, and it cannot *grant* anything it does not hold.

# Enrolling a device

In person, the way Void Hormiga's LAN join works (`../VoidHormiga/okf/concepts/platform/lan-sharing.md`),
because a password manager's first trust must not be a link in an email:

1. The new device makes its two key pairs and asks to join, over the local
   network (Reticulum).
2. An existing device of the vault shows the request: who, which device, which
   member it claims to be.
3. **Both screens show the same short code**, and the person compares them. This
   is the defence against a third device in the middle, and it is compared, not
   typed.
4. The existing device writes the enrolment as a signed object in the
   [record](/concepts/trust.md), with the consent the vault's rule for enrolment
   asks for ([authority](/concepts/authority.md): perhaps *"the IT person, or any
   two managers"*). Then the devices that know what this person may open re-seal it
   to the new device ([recovery](/concepts/recovery.md) is the same steps).

**How the code is made** (specified 2026-10-01, on Reticulum's SHA-256). From the
two devices' identities and two fresh nonces, not from the link's session keys, so
the code names the identities that will be recorded:

1. The new device picks a random nonce *a* and sends only a **commitment** to it:
   `SHA-256(its public keys ‖ a)`.
2. The existing device replies with its public keys and its own random nonce *b*.
3. The new device reveals *a*; the existing device checks it against the
   commitment.
4. Both compute `SHA-256("verguenza/enrol-code/v1" ‖ new keys ‖ existing keys ‖ a ‖ b)`
   and show six digits of it.

The commitment is what makes six digits enough: a device in the middle must fix
its keys and its nonce before it sees *b*, so it cannot search for a pair that
makes both screens match. It gets one guess in a million per attempt, and every
failed attempt is a refusal the person sees. This is the shape of Bluetooth's
numeric comparison and ZRTP's short authentication string.

**Enrolling remotely** (a member working from home) is possible over Reticulum
through an organization's always-on node, and it is strictly weaker: the code
must then travel by some other channel (a phone call). The application says so,
and a policy can forbid it.

# One identity per device, and it is the Reticulum identity

**Decided by the author, 2026-10-01:** *"we will continue with 1 identity per device
for now."* With *"reticulum as much as possible"*, that identity is the device's
**Reticulum identity**, the one Palabra's `Node` already makes and keeps. So:

- A device is known to every vault it belongs to by the same identity. Two vaults
  that share a member can tell that the same device is in both. The founding lean
  (one identity per vault, to keep groups from linking a person across vaults) is
  set aside; if it matters later, it returns as an option.
- The founding lean of *separate keys bound by a signature* ([Q3](/developer_questions.md))
  is set aside too: sealing to a Reticulum identity is Reticulum's own use of that
  key, and Verguenza's signatures carry a prefix no Reticulum message has
  ([formats](/concepts/formats.md)).
- **What it costs:** reinstalling Reticulum's storage, or resetting a node, makes a
  new identity, so it is a new device: enrolled again, re-sealed to, the old one
  marked lost. And the identity file's protection at rest is now Verguenza's
  problem to solve with Palabra ([Q12](/developer_questions.md)).

# Presence

Which of a vault's devices are reachable right now matters for the
[quorum](/concepts/quorum.md): a request needing two approvals goes to devices
that are here. Presence comes from Reticulum announces on the vault's own
aspect, sealed so that only the vault's devices can read who is present (Void
Hormiga seals its presence to a room key for the same reason). A device that is
not present can still approve later, unless the policy requires presence.

# Losing a device

The person (or an admin) marks it lost from any other device. Then:

1. A roster entry removes it, with a **cutoff** naming its last accepted objects
   ([trust](/concepts/trust.md) rule 2); anything it signs outside the cutoff is
   void, so it cannot backdate an approval or an enrolment.
2. Every envelope that had a seal for it is re-sealed without it (new DEK).
3. The application lists the secrets that device *could* open, and says plainly
   that their values should be changed at the vendor, because a thief who
   unlocks the device has seen them. It offers to mark each as *needs rotation*.

Step 3 is the honest part. No design can take back what a device already held.

# A member leaving

The same as losing every one of their devices, plus their member rune is marked
`left` (kept, so the record of who had access survives). A leaving member's
past approvals stay valid in the record; their future ones do not count.
