---
type: Concept
title: The boundary — who does what
description: "The line the author asked to be drawn sharply (2026-10-01): Reticulum is the cryptography and the wire, Void Palabra is how anything is remembered, named, merged, stored, carried and checked, and Void Verguenza is the social structure of a group that shares secrets: who may do what, who knows what, whose agreement an action needs, and how a group heals after a loss. Every responsibility in the design, assigned to one owner, with the reason; what Verguenza borrows instead of building; and what it has to ask Palabra for."
tags: [status:planned, audience:dev, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# The rule

The author, 2026-10-01: *"we should use whatever functionality is specific to
palabra as much as possible, really drawing that philosophical and architectural
line."* And: *"we wanna see about reticulum as much as possible, not libsodium."*

So Verguenza builds **only what is about people and secrets**, and asks for
everything else. When a need could be stated without the words *person*, *secret*,
*consent* or *knowledge*, it is not Verguenza's.

| owner | its question | in one word |
|---|---|---|
| **Reticulum** (through Palabra's companion) | *who is this, and can anyone else read it?* | identity |
| **Void Palabra** | *what happened, in what causal order, what is the state now, and is this signed change well-formed and within its capability?* | memory |
| **Void Verguenza** | *who should be able to do and know what, whose agreement does it take, and how does a group recover?* | standing |
| **the Verguenza application** | *what does a person see and touch?* | the screens |

# Every responsibility, assigned

| responsibility | owner | why there |
|---|---|---|
| a device's keys (Ed25519 + X25519) | **Reticulum** | a Reticulum identity is exactly that pair; the author: one identity per device ([people and devices](/concepts/people-and-devices.md)) |
| signing and verifying | **Reticulum** (`Identity.sign` / `validate`) | the identity already signs announces and link proofs |
| sealing a key to a device | **Reticulum** (`Identity.encrypt` / `decrypt`: an ephemeral X25519 exchange, HKDF, then an AES-256-CBC + HMAC-SHA256 token) | it is Reticulum's own *encrypt to a destination*, which is what a seal is |
| encrypting a value | **Reticulum**'s token, under a random key | the same construction as above, without the key exchange |
| hashing, content addresses | **Reticulum**'s SHA-256, which is also Palabra's | one hash across the family |
| randomness for identity keys | **Reticulum**, seeded from the OS by **Palabra** (fixed 2026-10-01) | it was not fit for keys before ([sealing](/concepts/sealing.md) §"Randomness") |
| randomness for what Verguenza makes (value keys, nonces, ids) | **the operating system**, through `verguenza_platform` (lean, [Q14](/developer_questions.md)) | the facility Reticulum itself should be stirring from; not a primitive anyone writes |
| moving bytes between devices; presence; announces | **Reticulum** links, through **Palabra**'s `Node` | decided for the whole family, 2026-09-23 |
| history: a change naming its causes by hash | **Palabra** ([utterances, the history graph](/concepts/trust.md)) | Palabra re-invented version control without a line; Verguenza's record is that, not a second one |
| convergence; conflicts as values | **Palabra** (join, conflict) | |
| deletions that stay deleted; replica identity; forks | **Palabra** (replica) | a new device is a fork, never a copy |
| bytes a document names but does not hold (envelopes) | **Palabra** (content references; check before storing) | an envelope is exactly such a thing |
| saving, loading, the file on disk, archives | **Palabra** (persistence, archive) | the `.verguenza` bundle is a Palabra archive with Verguenza's content |
| how much history a device keeps | **Palabra** (tiers: State, Recent, Full) | [recovery](/concepts/recovery.md) leans on it |
| provisioning a new device (keys, local files, approvals) | **the application**, by Palabra's own rule | *"setting up a peer is not syncing with one"* (`VoidPalabra:/concepts/peer-and-tier.md`) |
| **capabilities**: unforgeable, delegable, restrictable grants over a region, and checking a change against them | **Palabra** (planned, Meadowcap-shaped, `VoidPalabra:/design/open-questions.md` §6) | it is about *any* data any application writes; Verguenza is its first forcing client |
| **who should hold which capability, under whose consent, recallable by whom** | **Verguenza** ([authority](/concepts/authority.md)) | this is the social structure, and the thing Hormiga will adopt |
| **who knows what**: seals, exposure, what must rotate after a loss | **Verguenza** | knowledge of secrets is Verguenza's whole subject |
| **standing**: knowledge, authority and consent as derived weights on edges to measure runes; lenses that project an order | **Verguenza** computes them ([standing](/concepts/standing.md)); **Void Core** holds them, if compiled, as values on edges (0.14) | a readout of Verguenza's structure, in Core's own vocabulary for numbers |
| **access to mantles and runes**: audiences proposed by rules, seals as the read decision, capabilities as the write decision | **Verguenza** ([access](/concepts/access.md)); regions written in **Void Core**'s names; admission checked by **Palabra**'s capability mechanism | |
| **consent**: requests, approvals, access structures (who must agree) | **Verguenza** ([quorum](/concepts/quorum.md)) | |
| **recovery as a process**: who re-attests, who re-seals | **Verguenza** ([recovery](/concepts/recovery.md)); the bytes come back through **Palabra** | |
| a secret's description, kinds, providers | **Verguenza** ([secret](/concepts/secret.md)) | |
| the screens, the clipboard, auto-lock, hold-to-reveal | **the application** | |

# What moved because of this page

- **The ledger is not Verguenza's invention any more.** Its parts were always
  Palabra's: content-addressed changes naming their parents (utterances), a graph
  that holds an unplaceable change until its parents arrive, signed changes and
  capabilities (planned). [Trust](/concepts/trust.md) now describes Verguenza's
  *rules* and asks Palabra for the *mechanism*.
- **libsodium leaves the design** except where Reticulum has nothing to offer, and
  each such place is an open question ([Q12](/developer_questions.md)).
- **Recovery is mostly Palabra's** (the bytes are on other replicas already) plus
  a consent process that is Verguenza's ([recovery](/concepts/recovery.md)).

# What Verguenza must ask Palabra for

Drafted as messages at the repository root (ground rule 7):

1. ~~Randomness fit for keys~~: **done 2026-10-01** under the author's grant
   ([sealing](/concepts/sealing.md)).
2. **The identity's operations, through Palabra's header**: sign, verify against a
   public key, seal to a public key, open, random bytes, a token with a given key,
   SHA-256, all without a microReticulum type, and taking or returning plaintext in
   caller-owned buffers so Verguenza can wipe them.
3. **Signed changes and capabilities** (Palabra's §6), with Verguenza's
   [authority](/concepts/authority.md) and [trust](/concepts/trust.md) pages as a
   worked client.

Until 2 exists, Verguenza cannot seal anything with Reticulum. Until 3 exists,
Verguenza prototypes the rules in its own library behind an interface shaped like
what it is asking for, so the move into Palabra is a change of owner, not of
design ([Q5](/developer_questions.md)).
