---
type: Concept
title: Sealing — the cryptography
description: "How a secret's value is kept: one random key per value, the value encrypted with it (XChaCha20-Poly1305), that key sealed separately to every device allowed to open it (X25519 sealed boxes), the whole envelope signed by the device that made it (Ed25519). A device's own keys rest on disk under a passphrase only its person knows (Argon2id). Plaintext lives in guarded memory for as long as it is used. Every primitive is libsodium's; what libsodium does not provide (threshold sharing, hardware keys) is named, not improvised."
tags: [status:planned, audience:dev, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# The rule this page serves

**libsodium only, never a hand-rolled primitive, never a fallback secret.** The
same rule as Void Hormiga's (`../VoidHormiga/CLAUDE.md`, ground rule 6), stated
here as ground rule 3. Every operation below names the libsodium function it is.

# An envelope

A secret's value is sealed like this:

```
value ──► DEK  = randombytes(32)                       one key per value, per version
      ──► body = crypto_aead_xchacha20poly1305_ietf_encrypt(value, ad, nonce, DEK)
                 ad = vault id ‖ secret id ‖ envelope version ‖ policy id
      ──► for each device D allowed to open it:
              seal_D = crypto_box_seal(DEK, D.x25519_public)
      ──► sig  = crypto_sign_detached(header ‖ body ‖ seals, author_device.ed25519_secret)
```

| part | why |
|---|---|
| **one DEK per value** | rotating a secret, or removing a device, never touches any other secret's ciphertext |
| **XChaCha20-Poly1305 with associated data** | authenticated: a changed byte, or a body moved to a different secret or policy, fails to open instead of opening wrong |
| **a seal per device** (`crypto_box_seal`, X25519 + XSalsa20-Poly1305, anonymous sender) | the sealer does not need the recipient online; a device not on the list cannot open the DEK |
| **a signature by the device that made it** (Ed25519) | a member cannot slip in a replacement envelope as someone else; the [audit](/concepts/audit.md) record can say who changed a value |

The envelope is a content-addressed blob (`sha256` of its bytes). The secret
rune names it. A new version is a new envelope; nothing is edited in place.

**Adding a device to a secret** needs one device that can already open it: that
device opens the DEK and seals it again to the new device. That is why adding a
device to a guarded secret is itself an action a [policy](/concepts/quorum.md)
can require approvals for.

**Removing a device** cannot take back what it already saw. It means a new
envelope (a new DEK) sealed to everyone except the removed device, *and*
changing the secret at the vendor, which only a person can do. The application
says both, in that order. See [sharing](/concepts/sharing.md) §"Revocation".

# A device's own keys

Each [device](/concepts/people-and-devices.md) has two key pairs, never one
reused for both jobs (Void Palabra reached the same rule independently):

| pair | libsodium | used to |
|---|---|---|
| signing | `crypto_sign_keypair` (Ed25519) | sign envelopes, approvals and log entries |
| agreement | `crypto_box_keypair` (X25519) | receive seals |

**At rest, under a passphrase only its person knows.** The two secret keys are
stored in the device's application folder (never in a shared vault bundle),
sealed with `crypto_aead_xchacha20poly1305_ietf` under a key from
`crypto_pwhash` (Argon2id) over the person's passphrase and a per-device salt,
with **`OPSLIMIT_MODERATE` / `MEMLIMIT_MODERATE`** at least. This is
deliberately stronger than Void Hormiga's choice (its vault is sealed with a key
derived from the profile, so whoever can read the profile folder can open it).
For a password manager, *"someone copied my user folder"* must not be enough.

**Convenience unlock** (the operating system's keychain: Windows' Credential
Manager and DPAPI, later the macOS Keychain) is an opt-in per device, and the
screen says what it costs: anyone who can log in as this user can then unlock
the vault. [Q2](/developer_questions.md).

**Lock.** After a set idle time, on the operating system's lock or sleep, and on
demand. Locking zeroes every key and every plaintext in memory.

# Plaintext in memory

- **Guarded buffers.** Keys and plaintext values live in `sodium_malloc`
  memory (guard pages, a canary), marked `sodium_mprotect_noaccess` when not in
  use, `sodium_memzero`'d and freed with `sodium_free`. Never in a
  `std::string`, which copies itself and is never wiped. A `Secret` buffer type
  in the code is the only way a value is held, so a review can find every place.
- **Never on disk.** Not in a temporary file, a log, a crash dump the
  application writes, or the state document. Windows can still page memory out;
  `sodium_mlock` asks it not to, and the design says it is a request.
- **The clipboard.** Copying a password puts it there for 30 seconds (a
  setting), then clears it if it is still the same text. Windows clipboard
  history (Win+V) can keep a copy; the application marks its clipboard data so
  history and cloud clipboard skip it, where Windows honours that.
- **The screen.** A value is shown only while a button is held, or for a few
  seconds after a click, never by default.

# The vault bundle (`.verguenza`)

Shaped like Void Hormiga's `.miga` v3: one file that carries a whole vault for
backup or for moving to a new computer.

| part | carried | sealed |
|---|---|---|
| the state document (every mantle: secrets, members, devices, policies, clients, requests) | yes | **no**: descriptions are readable by members (a secret marked `hidden` has its description sealed like a value) |
| envelopes | yes | yes, each to its devices |
| the signed log | yes | no (it holds no values) |
| any device's private keys | **never** | — |

So a bundle copied onto a stranger's computer gives the stranger the list of
what exists and nothing that opens. A bundle restored onto a *new* device of an
existing member opens nothing until that device is [enrolled](/concepts/people-and-devices.md)
and re-sealed to, or until [recovery](/concepts/quorum.md) runs.

# What libsodium does not provide

The design needs three things libsodium has no primitive for. Each is an open
question, not an implementation:

1. **Threshold sharing** (Shamir's secret sharing), for [split secrets](/concepts/quorum.md)
   and social recovery. The lean is to vendor a small, reviewed, constant-time
   implementation over GF(256) as source with its license, and to keep it behind
   one function pair (`split`, `combine`). [Q1](/developer_questions.md).
2. **Hardware keys** (a FIDO2 security key as one of the approvers). Would mean
   vendoring libfido2. Later. [Q6](/developer_questions.md).
3. **Threshold signatures** (k-of-n devices producing one signature). Not needed:
   k separate Ed25519 approvals, each checked, are simpler and auditable.
