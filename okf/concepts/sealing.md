---
type: Concept
title: Sealing — the cryptography
description: "How a secret's value is kept, on Reticulum's primitives through Void Palabra (the author, 2026-10-01: 'reticulum as much as possible, not libsodium'). One random key per value; the value encrypted with Reticulum's token (AES-256-CBC + HMAC-SHA256); that key sealed to every device allowed to open it with Reticulum's encrypt-to-identity (ephemeral X25519, HKDF, token); the envelope signed by the writing device's identity (Ed25519). What Reticulum gives, and the three things it does not: randomness fit for keys (measured: it is not, today), a password-hardening function for keys at rest, and wipeable memory. Each is named, with what Verguenza does until it is answered."
tags: [status:planned, audience:dev, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# The rule this page serves

Ground rule 3, as amended 2026-10-01: **cryptography comes from Reticulum, through
Void Palabra. Never a hand-rolled primitive, never a fallback secret.** Where
Reticulum has no primitive the design needs, the gap is an open question, not a
quiet substitute. The [boundary](/concepts/boundary.md) says why the line sits
there: one identity per device, one cryptographic stack per family, and Reticulum is
the family's.

**Why not libsodium, said fairly.** The author's reason was mobile. For the record,
libsodium does build for phones (Void Hormiga built it for Android arm64 from the
signed tarball on 2026-09-24, though it has not yet run on a device). The reason that
holds regardless is the architectural one: every Void device already *is* a Reticulum
identity, already signs and encrypts with it, and a second cryptographic library
would be a second identity system to keep in step. Verguenza uses the one the family
has.

# An envelope

```
value ──► K    = random(64)                              one key per value, per version
      ──► body = Token(K).encrypt( header-digest ‖ value ) AES-256-CBC, HMAC-SHA256
      ──► for each device D allowed to open it:
              seal_D = Identity(D.public).encrypt(K)    ephemeral X25519 → HKDF → Token
      ──► sig  = Identity(author).sign("verguenza/envelope/v1" ‖ header ‖ body ‖ seals)
```

| part | Reticulum | why |
|---|---|---|
| **one key per value** | `random` | rotating a secret, or removing a device, never touches another secret's ciphertext |
| **the body** | `Token` with a 64-byte key: AES-256-CBC, PKCS7, encrypt-then-MAC with HMAC-SHA256 | authenticated: a changed byte fails |
| **binding the body to its place** | the **header digest inside the plaintext** | Reticulum's token has no associated data. So the first 32 bytes of the plaintext are the SHA-256 of the header (vault, secret, version, policy). A body moved to another secret decrypts to a digest that does not match its new header and is refused. The signature covers header and body as well |
| **a seal per device** | `Identity.encrypt` to the device's public identity | Reticulum's own *encrypt to a destination*: anonymous sender, the recipient need not be online, a device not on the list cannot open `K` |
| **a signature by the device that wrote it** | `Identity.sign`, Ed25519 | a member cannot slip in a replacement envelope as someone else; the [audit](/concepts/audit.md) record says who changed a value |

The envelope is a content-addressed object (SHA-256 of its bytes), carried by
Palabra as a content reference. A new version is a new envelope; nothing is edited
in place. Byte layout: [formats](/concepts/formats.md).

**Adding a device** needs one device that can already open the secret: it opens `K`
and seals it again to the new device (a *reseal*: same body, new seals, new
signature). That is why adding a device to a guarded secret is an action a
[policy](/concepts/quorum.md) can require approvals for, and why
[recovery](/concepts/recovery.md) is mostly re-sealing.

**Removing a device** cannot take back what it already saw. It means a new `K`,
sealed to everyone except the removed device, *and* changing the secret at the
vendor, which only a person can do.

# A device's identity

**One identity per device** (the author, 2026-10-01), and it is the device's
**Reticulum identity**: an Ed25519 key to sign and an X25519 key to receive seals,
made and kept by Palabra's `Node`. Every vault the device belongs to knows it by
that identity. [People and devices](/concepts/people-and-devices.md) has what this
replaces ([Q3](/developer_questions.md), [Q11](/developer_questions.md)).

**The same keys now do two jobs**, transport and sealing. Reticulum's own design
already uses the identity's X25519 key for *encrypt to this identity*, so sealing is
that use, not a new one. The Ed25519 key signs both Reticulum's announces and
Verguenza's objects; Verguenza's signatures always begin with a
`verguenza/<type>/v1` prefix ([formats](/concepts/formats.md)), so no Verguenza
signature is a valid announce, link proof or other Reticulum message.

# What Reticulum gives, and what it does not

Read in Void Palabra's vendored copy (microReticulum `40fa628`, the Crypto fork
`984dc89`) on 2026-10-01.

| need | Reticulum | status |
|---|---|---|
| sign, verify | `Identity.sign` / `validate` (Ed25519) | **present** in microReticulum; **not exposed** by Palabra's header |
| seal to a device, open a seal | `Identity.encrypt` / `decrypt` | **present**; **not exposed** |
| symmetric encryption | `Token` (AES-256-CBC + HMAC-SHA256) | **present**; **not exposed**. Its MAC check compares with an early-exit comparison, not in constant time |
| SHA-256, HKDF, HMAC | present | **not exposed** |
| **randomness fit for keys** | the Crypto library's `RNG` | **not fit, today** (below) |
| **a password-hardening function** (Argon2id, scrypt) for keys at rest under a passphrase | **none** | gap: [Q12](/developer_questions.md) |
| **wipeable, lockable memory** | none; its `Bytes` type copies freely and never wipes | gap: below |
| **threshold sharing** | none (neither does libsodium) | [Q1](/developer_questions.md) |

## Randomness: measured, and not fit for keys

On a desktop or an Android build, the Crypto library's random generator is seeded
from **fixed constants, the string "Reticulum", and the time the binary was
compiled**. The only input that changes between runs is the system's monotonic
microsecond clock, XORed into a single 32-bit word each time the generator rekeys
(`Crypto/RNG.cpp`: `begin()`, `rekey()`). There is no operating-system entropy
(`getrandom`, `BCryptGenRandom`, `SecRandomCopyBytes`) anywhere in the path, and
the hook that would add some (`RNG.addNoiseSource`) is commented out in
microReticulum's `Reticulum.cpp`.

Measured: two fresh nodes started a second apart produced different identities, so
the clock does vary the output. But the clock is uptime, which an attacker can often
estimate, and each injection is at most 32 bits. **Estimated, not measured**: the
identity keys and every link's ephemeral keys of a Void device are within reach of
an offline search. This is Palabra's to fix (stir operating-system entropy into the
generator at start and periodically, through its public `stir` call, without
patching), and it blocks every key Verguenza would make. Reported:
`MESSAGE_FOR_VOIDPALABRA_verguenza-reticulum-randomness-2026-10-01.md`.

**Until it is fixed, Verguenza makes no key with it.** No output test can prove a
generator unpredictable (today's already passes "two runs differ"), so phase 0's
check is about the source: Verguenza's configure step looks for Palabra's
entropy-seeding call and refuses to build the parts that make keys without it.

## Plaintext in memory

What the founding design got from libsodium (`sodium_malloc` guard pages,
`sodium_mprotect`, `sodium_memzero`) has no Reticulum counterpart, and is not
cryptography: it is memory handling the operating system provides. So Verguenza's
`Secret` type ([architecture](/concepts/architecture.md)) uses the operating system
directly: `VirtualAlloc` + `VirtualLock` + `SecureZeroMemory` on Windows, `mmap` +
`mlock` + `explicit_bzero` elsewhere, with guard pages where the platform gives them.
That is not hand-rolling a primitive; it is calling the platform's.

The honest gap is on Reticulum's side of the call: microReticulum's `decrypt`
returns plaintext in a `Bytes` that has already been copied and is never wiped. So
Palabra's exposed operations must take and fill **caller-owned buffers** (asked in
the same message), or Verguenza's wiping guards only its own copies.

- **Never on disk.** Not in a temporary file, a log, a crash dump the application
  writes, or the state document. microReticulum's trace logging prints plaintext
  and derived keys (`TRACEF` in `Identity.cpp`); Palabra compiles logging only up to
  level 6 and trace is level 8, so it is compiled out. Verguenza's build **refuses to
  configure** if `VOIDPALABRA_RNS_LOG_LEVEL` is 8 or higher.
- **The clipboard.** Copying a password puts it there for 30 seconds (a setting),
  then clears it if it is still the same text. Windows clipboard history and cloud
  clipboard are told to skip it with the extra clipboard formats Windows reads
  (`ExcludeClipboardContentFromMonitorProcessing`, `CanIncludeInClipboardHistory`,
  `CanUploadToCloudClipboard`; recalled, to be checked against Microsoft's
  documentation when built), which GLFW's clipboard call cannot set, so the copy goes
  through `verguenza_platform`. Other programs can still read it while it is there.
- **The screen.** A value is shown only while a button is held, or for a few seconds
  after a click, never by default.

# A device's keys at rest

Palabra's `Node` keeps the identity in its `storage_dir` as a plain file today:
whoever can read the folder has the device. For a node that only syncs contacts that
is a choice; for a password manager, *"someone copied my user folder"* must not be
enough. What protects the identity file at rest is [Q12](/developer_questions.md),
and the honest options are:

| option | protects against a copied folder? | on phones | cost |
|---|---|---|---|
| **the operating system's key store** (Android Keystore, iOS Keychain / Secure Enclave, Windows DPAPI, macOS Keychain) wrapping the identity file | on phones, yes (hardware-backed, with the device's lock or biometrics); on Windows, **only against other users**: DPAPI opens for anyone logged in as you | **the native way** | per-platform code in `verguenza_platform`; no passphrase to forget |
| **a passphrase**, hardened by Argon2id from libsodium, kept for that one function | yes, everywhere | works, a second library | the exception to "Reticulum only", in one place |
| **a passphrase**, hardened by PBKDF2 built from Reticulum's HMAC | weakly: PBKDF2 is cheap to attack with GPUs, and composing it ourselves is the hand-rolling ground rule 3 forbids | works | not offered |

**Lock.** After a set idle time, on the operating system's lock or sleep, and on
demand. Locking wipes every `Secret`, and the identity's private keys if Palabra's
API lets Verguenza hold them in its own buffers.

# The vault bundle (`.verguenza`)

A Palabra **archive** (`VoidPalabra:/concepts/archive.md`) with Verguenza's content:

| part | carried | sealed |
|---|---|---|
| the state document | yes | **no**: descriptions are readable by members (a secret marked `hidden` has its description sealed like a value) |
| envelopes | yes, as content references | yes, each to its devices |
| the signed record ([trust](/concepts/trust.md)) | yes | no (signed, and holds no values) |
| any device's identity | **never** | — |

So a bundle copied onto a stranger's computer gives the stranger the list of what
exists and nothing that opens.

# What neither stack provides

1. **Threshold sharing**, for split secrets and cold recovery holders, along a
   consent formula ([authority](/concepts/authority.md)). [Q1](/developer_questions.md).
2. **Hardware keys** (FIDO2) as approvers. Later. [Q6](/developer_questions.md).
3. **Threshold signatures**: not needed; k separate signed approvals are simpler and
   auditable.
