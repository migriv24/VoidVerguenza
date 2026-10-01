---
type: Concept
title: People and devices
description: "Members are people; devices are the computers and phones they use, each with its own keys, so losing a phone revokes a phone and not a person. Roles and what they allow; enrolling a device in person, with a short code both screens show (Void Hormiga's pairing, carried over); how a device's Verguenza keys relate to its Reticulum identity; presence; losing a device; a member leaving."
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
| has | a name, a role, the devices they own | two key pairs ([sealing](/concepts/sealing.md)), an owner, a kind (desktop, laptop, phone) |
| is added by | a member with the right to add members, under policy | its owner, on the device, confirmed by an existing device |
| when it is lost | the member removes the device, or others do | its seals are dropped from every envelope on the next re-seal |

**A secret is sealed to devices.** Saying *"Ana can open the alarm code"* means
the envelope carries a seal for each of Ana's devices. Adding a new laptop for
Ana means re-sealing what Ana may open to that laptop.

# Roles

| role | may |
|---|---|
| **owner** | everything, subject to the vault's policies (an owner can still be required to get approvals) |
| **admin** | add and remove members and devices, change policies, manage clients |
| **member** | open the secrets sealed to their devices, add secrets, approve requests |
| **viewer** | see descriptions; open nothing unless a secret is explicitly sealed to them |

Roles are recorded in the `members` mantle and **enforced by signatures**: every
command that changes membership or policy is signed by a device whose member has
the role, and every device checks that signature before applying it. A command
with a bad or missing signature is refused by every honest device, which is what
makes the record trustworthy without a server. The design is honest that a
modified client could *show* itself something it should not; it cannot *open*
anything that is not sealed to it.

# Enrolling a device

In person, the way Void Hormiga's LAN join works (`../VoidHormiga/okf/concepts/platform/lan-sharing.md`),
because a password manager's first trust must not be a link in an email:

1. The new device makes its two key pairs and asks to join, over the local
   network (Reticulum).
2. An existing device of the vault shows the request: who, which device, which
   member it claims to be.
3. **Both screens show the same short code**, derived from both devices' public
   keys and the link. The person compares them. This is the defence against a
   third device in the middle, and it is compared, not typed.
4. The existing device signs the enrolment (subject to policy: an admin's
   approval, or two). Then it re-seals to the new device whatever its member may
   open.

**Enrolling remotely** (a member working from home) is possible over Reticulum
through an organization's always-on node, and it is strictly weaker: the code
must then travel by some other channel (a phone call). The application says so,
and a policy can forbid it.

# A device's Verguenza keys and its Reticulum identity

Every Void application networks over Reticulum (decided 2026-09-23;
`VoidPalabra:/concepts/reticulum.md`), and a Reticulum identity is itself an
Ed25519 + X25519 pair. Using the *same* keys for Reticulum links and for sealing
secrets would be key reuse across two protocols, which is exactly what the
family's rule forbids.

**Lean: separate keys, bound by a signature.** The Verguenza signing key signs a
short statement *"this Reticulum identity speaks for this device"*, and every
device checks it when a link opens. A Reticulum identity can then be replaced
(a new install, a reset) without re-sealing every secret, and a leaked transport
key opens nothing. [Q3](/developer_questions.md).

# Presence

Which of a vault's devices are reachable right now matters for the
[quorum](/concepts/quorum.md): a request needing two approvals goes to devices
that are here. Presence comes from Reticulum announces on the vault's own
aspect, sealed so that only the vault's devices can read who is present (Void
Hormiga seals its presence to a room key for the same reason). A device that is
not present can still approve later, unless the policy requires presence.

# Losing a device

The person (or an admin) marks it lost from any other device. Then:

1. It is removed from the `devices` mantle; its signatures stop counting for
   approvals from that moment.
2. Every envelope that had a seal for it is re-sealed without it (new DEK).
3. The application lists the secrets that device *could* open, and says plainly
   that their values should be changed at the vendor, because a thief who
   unlocks the device has seen them. It offers to mark each as *needs rotation*.

Step 3 is the honest part. No design can take back what a device already held.

# A member leaving

The same as losing every one of their devices, plus their member rune is marked
`left` (kept, so the record of who had access survives). A leaving member's
past approvals stay valid in the record; their future ones do not count.
