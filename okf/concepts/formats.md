---
type: Concept
title: Formats — the bytes of an envelope and a signed object
description: "The layouts phase 1 builds, on Reticulum's primitives (amended 2026-10-01): identifiers (a device is its Reticulum identity), the signing rule (a type prefix on everything signed, so no Verguenza signature can pass as another kind, or as a Reticulum announce), the envelope v1 with the header digest inside its token because Reticulum's token has no associated data, the signed-object frame with its causal parents, and readers that refuse rather than guess. Lean until phase 1's exit, when v1 freezes."
resource: src/foundation/sealing/envelope.cpp
tags: [status:current, audience:dev, confidence:measured]
timestamp: 2026-10-01T00:00:00Z
---

# Status of this page

**Implemented 2026-10-01** (`src/foundation/record/object.cpp`,
`src/foundation/record/fields.cpp`, `src/foundation/sealing/envelope.cpp`; tested in
`tests/integration/sealing_test.cpp`). One thing the page left open is now fixed: a body
is **tagged fields** (tag u8, length u32, bytes; tags strictly increasing, so the encoding
is canonical), the same layout for every type (`record/fields.hpp`).

**Leans, frozen at the end of phase 1.** Until then any field may change. After
it, a change is a new version (`VGE2`…), and readers keep reading v1. Every layout
below is little-endian, length-prefixed, with no optional fields and no padding:
one way to write each object, so its hash is its name.

**Amended 2026-10-01** from libsodium's primitives to Reticulum's
([sealing](/concepts/sealing.md)). The device keyring file is gone: the identity is
Palabra's `Node`'s, and how it is protected at rest is [Q12](/developer_questions.md).

# Identifiers

| id | is | size |
|---|---|---|
| **device id** | the device's **Reticulum identity hash** (truncated SHA-256 of its public keys, as Reticulum computes it) | 16 bytes |
| **device public key** | X25519 ‖ Ed25519, as Reticulum writes it; recorded once, in the roster entry that admits the device | 64 |
| **vault id** | SHA-256 of the founding roster entry | 32 |
| **member id**, **group id**, **secret id**, **policy id**, **request id** | `random(16)` | 16 |
| **object address** | SHA-256 of the object's bytes, written `sha256:<hex>` | 32 |

SHA-256 everywhere, because Reticulum and Void Palabra's content references both
name things by it.

# The signing rule

**Every signature covers a type prefix, then the object.**

```
sig = Identity(device).sign( "verguenza/" <type> "/v1" 0x00 ‖ object-bytes-before-sig )
```

with `<type>` one of `roster`, `capability`, `envelope`, `request`, `approval`,
`act`, `enrol-code`. The prefix does two jobs now:

- a signature on an approval cannot be passed off as one on an envelope;
- **no Verguenza signature is a valid Reticulum signature.** The identity's Ed25519
  key also signs Reticulum's announces and link proofs, whose signed bytes begin
  with a destination hash and never with the ASCII `verguenza/`.

Every signed object ends with `author device id (16) ‖ sig (64)`, and names its
**parents** (the objects it was written after, by address: Palabra's causal clock,
[trust](/concepts/trust.md)) and **this device's previous object**:

```
parent count (u16) ‖ parent address (32) × count ‖ previous (32, zeros for a device's first)
```

# Envelope v1

```
"VGE1"                       4   magic and version
vault id                    32   ┐
secret id                   16   │ the header: what the value is bound to.
value version (u32)          4   │ Its SHA-256 is the first 32 bytes of the
policy id                   16   ┘ plaintext inside the body
parents                2 + 32n
previous                    32
body length (u32)            4
body                         …   Token(K).encrypt( SHA-256(header) ‖ value )
                                   K = random(64): AES-256-CBC + HMAC-SHA256, IV inside
seal count (u16)             2
  per seal:
  kind (u8)                  1   1 device · 2 cold holder · 3 share (split)
  recipient id              16   a device id, or a cold holder's identity hash
  seal length (u16)          2
  seal                       …   Identity(recipient).encrypt(payload)
                                   = ephemeral X25519 public (32) ‖ token
author device id            16
sig                         64   the signing rule, type "envelope"
```

- **The header digest inside the plaintext** replaces associated data, which
  Reticulum's token does not have. Opening an envelope decrypts the body and refuses
  it unless its first 32 bytes equal the SHA-256 of the header it arrived with, so a
  body moved to another vault, secret, version or policy fails instead of opening
  wrong. The signature, over header, body and seals, is the second lock.
- **Re-sealing keeps the body.** A new device added to a secret gets a new envelope
  with the **same body** and new seals: a new address and a new signature, no
  re-encryption, the same version. That is `reseal` in
  [commands](/concepts/commands.md) and most of [recovery](/concepts/recovery.md).
- **A seal's payload** is the 64-byte `K` for kinds 1 and 2, and a share of it for
  kind 3 (its layout waits on [Q1](/developer_questions.md)).
- **Removing a device, or strengthening a policy, is not a reseal.** It needs a new
  `K` and a new version, because the removed device (or every device that held the
  whole `K` before a split) can still open the old body.

# Signed objects

The same frame for roster entries, capabilities, requests, approvals and acts:

```
"VGO1" ‖ type (u8) ‖ vault id (32) ‖ parents (2 + 32n) ‖ previous (32)
       ‖ time (i64, unix ms) ‖ body length (u32) ‖ body
       ‖ embedded approvals (2 + n × approval) ‖ author device id (16) ‖ sig (64)
```

The body is a small fixed layout per type, written when each phase builds its type.
**`time` is the writer's clock, recorded for people and never consulted for
order**: order comes from the parents, as in Palabra's history graph. The two places
a clock *is* consulted are an approval's `until` and a capability's expiry, and
[quorum](/concepts/quorum.md) says what that can and cannot promise.

Whether this frame stays Verguenza's or becomes Palabra's signed utterance is
[Q5](/developer_questions.md). It is written so that the move changes the magic and
the owner, not the fields.

# Readers refuse

Every reader in `voidverguenza` refuses, with a distinct message: a wrong magic, an
unknown version, a truncated object, trailing bytes, a length beyond a fixed ceiling
(64 MiB for a body, 4096 seals, 1024 parents), a hash that does not match the address
the object arrived under, a header digest that does not match, and a bad signature.
Nothing is parsed "leniently". Each refusal has a test with a crafted input
([architecture](/concepts/architecture.md)).
