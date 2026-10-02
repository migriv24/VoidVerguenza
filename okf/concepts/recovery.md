---
type: Concept
title: Recovery — what is lost, where it lives, who untangles it
description: "The author, 2026-10-01: 'where is something being stored and recovered? who owns that storage? what process untangles the recovery?' Recovery splits into four losses with four owners. The vault's bytes are on other replicas already (Palabra). A device's keys are never recovered; a new device is a new identity and a fork (Palabra's rule). A person's standing is re-attested by the group (Verguenza's consent). Access to secrets comes back by RE-SEALING from the people who still know them (Verguenza). A cryptographic recovery key is needed only when nobody who knows a secret is left, and it is opt-in. Who stores what, and the process, step by step."
tags: [status:planned, audience:all, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# Recovery was four things wearing one word

The founding design treated recovery as one mechanism: a recovery key sealed into
every envelope, split among recovery holders. Asking the author's three questions of
it (*where is it stored, who owns the storage, what untangles it*) shows four
different losses, each with a different owner:

| what is lost | where it actually lives | who owns that | how it comes back |
|---|---|---|---|
| **the vault's data**: descriptions, the record, envelopes | on every other device's **replica**, and in any **archive** someone kept | each device owns its own copy; Palabra keeps it | **sync**. A new device starts an empty replica and receives everything. Nothing special |
| **a device's keys** | only on that device | that device | **never**. A lost key is replaced, not recovered. The new device is a new identity (one per device, the author's ruling) and a **fork**, never a copy (Palabra: *"the one thing provisioning must never hand over is replica identity"*) |
| **a person's standing**: membership, mandates, group places | in the signed record, on every replica | the group | **re-attested**: the group's consent rule admits the new device as this person's ([people and devices](/concepts/people-and-devices.md)), and the old one is marked lost |
| **access to secrets**: the seals | in envelopes, sealed to devices that still exist | the people who **still know** each secret | **re-sealed**: any device that can open a secret, and whose person consents, seals it to the new device |

Only the last is hard, and only in one case.

# Recovery is re-sealing by the people who still know

When Ana loses her laptop:

1. **Another device of Ana's, or the group**, asks: *"admit this new device as
   Ana's."* That is an enrolment request under the vault's consent rule for it
   ([authority](/concepts/authority.md)), with the in-person code if the policy
   asks for it.
2. Once admitted, the new device syncs: it has the whole vault, and opens nothing.
3. **Every device that can open a secret Ana was allowed to know** sees the
   admission and, when its person approves (or automatically, if that person's
   policy says so), **re-seals** the secret to Ana's new device. It does not need
   to open the value: re-sealing a key needs only the key ([formats](/concepts/formats.md)).
4. The lost laptop is marked lost. Its secrets go onto the *needs rotation* list
   ([sharing](/concepts/sharing.md) §"Revocation"), because whoever has it may have
   seen them.

Nobody held a special recovery key. The group recovered Ana the way a group of
people recovers a member who lost their keys: those who trust her let her back in,
and those who know the combination tell her again.

**Who untangles it:** the request (step 1) is Verguenza's, the consent is the
group's, the re-sealing is each knower's device, and the bytes travel through
Palabra. Every step is a signed, recorded act, so the [audit](/concepts/audit.md)
record says who let Ana back in and who re-sealed what.

**Split secrets recover the same way**, through their share holders: each holder
re-shares (or the group re-splits) to the new device. No recovery path goes around
a split ([quorum](/concepts/quorum.md)).

# When nobody who knows is left

Re-sealing needs at least one device, still in the vault, that can open the secret.
It fails in two cases:

- **A vault of one person** who loses their only device.
- **A secret known only to people who are all gone**: the owner was the only one
  sealed to the bank login, and the owner's devices are lost.

Here, and only here, a **cold holder** is needed: something that can open secrets
and is not in daily use. Two shapes, both opt-in:

| cold holder | is | stored by | cost |
|---|---|---|---|
| **a cold device** | a real device (an old phone in a drawer, a laptop in the safe) enrolled like any other, sealed to like any other, rarely switched on | its owner, physically | a device that is lost while cold is lost without anyone noticing; the application shows when each cold device last checked in |
| **a recovery identity split among holders** | the founding design's recovery key: a Reticulum identity created for the purpose, sealed to by envelopes that opt in, its private key split k-of-n among named people | the holders' devices, each holding one share | needs threshold sharing ([Q1](/developer_questions.md)); must never reach split secrets by default |

The application asks when a vault is created, and says what skipping means:
*"if every device that knows a secret is lost, that secret is lost."*
[Q4](/developer_questions.md), [Q14](/developer_questions.md).

# Where things are stored, and who owns the storage

Palabra's **tiers** (`VoidPalabra:/concepts/peer-and-tier.md`) already let a device
declare how much it carries. Recovery needs one more dimension, and it is the same
shape (a capability statement, not a class):

| a device that is | holds | can it open anything? |
|---|---|---|
| **a member's device** | the replica, the envelopes, its own seals | what is sealed to it |
| **a cold device** | the same, rarely synced | what is sealed to it |
| **a keeper** (an always-on node in the back office, a relay) | the replica and the envelopes, **no seals** | nothing. It is backup storage that cannot read |

A **keeper** answers *"where is it stored"* for a group that is never all online at
once: it holds every envelope, so a new device can sync from it at any hour, and it
never holds a key, so stealing it yields what stealing a `.verguenza` bundle yields
(names, and sealed bytes). This replaces the founding [sharing](/concepts/sharing.md)
page's *"a relay that holds nothing"*: a relay that holds **ciphertext** is useful,
and holds nothing that opens.

**Nobody owns "the vault".** Each device owns its copy, the group owns the rules,
and each person owns what they know. That is the answer to *"who owns that
storage?"* and it is why there is no server to subpoena, breach or lose.

# What this changes elsewhere

- [Quorum](/concepts/quorum.md) §"Social recovery" now points here; the recovery
  key is a cold holder, not the mechanism.
- [Sharing](/concepts/sharing.md): the always-on node becomes a **keeper**.
- The [threat model](/concepts/threat-model.md)'s *"every recovery holder lost"*
  row becomes *"every device that knows a secret lost, and no cold holder"*.
