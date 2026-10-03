---
type: Concept
title: Architecture — targets, layers, toolchain and the checks
description: "How the code will be laid out before any of it exists: a UI-free `voidverguenza` library that Void Hormiga can link (formats, envelopes, the authority model and the record's rules, the `Secret` type), resting on Void Palabra for history, storage, transport and (through its Reticulum companion) every cryptographic operation; a host layer that turns verbs into Void Core commands; a platform layer for what only the operating system does (locked and wiped memory, the clipboard's history flags, the key store, lock and sleep events); the Void Maiz application and a CLI. The layering rules and the scripts that check them, the toolchain Reticulum forces (GCC or Clang, not MSVC), and the tests that keep the ground rules true. Amended 2026-10-01: Reticulum through Palabra instead of libsodium."
tags: [status:planned, audience:dev, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# Targets

| target | is | links | may not link |
|---|---|---|---|
| **`voidverguenza`** (the foundation, [products](/concepts/products.md)) | [formats](/concepts/formats.md), envelopes, the [authority](/concepts/authority.md) and [access](/concepts/access.md) model (groups, capabilities over vault/mantle/glyph/rune, consent formulas, templates, audiences proposed by rules), the [record's rules](/concepts/trust.md), [standing](/concepts/standing.md), the `Secret` type, the client protocol (phase 2), threshold sharing (phase 5) | Void Palabra and its Reticulum companion; `verguenza_platform`'s memory functions | Void Core, Void Maiz, anything with a screen |
| **`verguenza_host`** | the application model: [verbs](/concepts/commands.md) compiled to Core commands, the projection, intake, the agent, sync glue | `voidverguenza`, Void Core, through Void Maiz's headless session (the application uses Maiz, [Q15](/developer_questions.md)) | Void Maiz's views, GLFW, ImGui |
| **`verguenza_platform`** | what only the operating system does: locked, wiped memory; the clipboard with history and cloud exclusion; the key store ([Q12](/developer_questions.md)); lock, sleep and session-switch events; the data folder | Win32 (later Cocoa, Android, iOS) | everything above |
| **`voidverguenza-app`** | the desktop application ([application](/concepts/application.md)) | all of the above, Void Maiz | — |
| **`verguenza-cli`** | the same verbs headless, values on standard input | `verguenza_host`, `verguenza_platform` | Void Maiz's views |

**Why `voidverguenza` links Palabra and not Core or Maiz.** Void Hormiga is its first
client ([clients](/concepts/clients.md)) and will adopt its authority model for its
own members ([authority](/concepts/authority.md)). Hormiga already links Palabra, so
the library costs it nothing new; it must not drag a second Void Core or a view
library into Hormiga's binary. And the [boundary](/concepts/boundary.md) puts every
cryptographic operation, history and transport in Palabra, so `voidverguenza` holds
only what is about people and secrets.

**No cryptography is implemented in this repository.** Every sign, verify, seal,
open, hash and random call goes to Palabra's Reticulum companion. A grep for cipher
or curve names in Verguenza's own sources is part of the layering check.

# The code layout: hexagonal

The author, 2026-10-01: *"organize things into folders and a modular hexagonal
structure (already semi provided by the void core)"*, and *"we likely don't need to do
too much for the desktop application that uses void maiz. The core foundations and
logic will likely be taking up more scripts and modules."*

So the domain sits in the middle and knows nothing outside itself; everything it needs
from the world is a **port** (an interface it declares), and every way of meeting that
need is an **adapter** in its own folder. Void Core already works this way (its holidays
and effect handler are ports; hosts are adapters), so the shape is the family's.

```
src/
  foundation/            the domain: voidverguenza (std only; no Palabra, Core, Maiz)
    core/                bytes, hex, little-endian codec, results
    ports/               crypto, secure memory: what the domain needs from outside
    secret/              SecretBuffer: the one type that holds a plaintext
    record/              signed objects (the frame, signing, the record and its rules)
    sealing/             envelopes: seal, open, reseal
    access/              consent formulas, capabilities, groups (phase 1A)
    secrets/             secret kinds and their fields (the application's glyphs)
  application/           use cases (verguenza_app): the vault service, its ports
    ports/               vault store, key store, clipboard, model (Void Core)
  adapters/              each meets a port, each its own library
    reticulum/           crypto port, through Palabra's crypto.hpp
    storage/             vault store port, as a Palabra Archive (the .verguenza file)
    model/               model port, through Void Maiz's embedded Void Core
    platform/win32/      secure memory, clipboard, key store (DPAPI)
  desktop/               the Void Maiz window: thin, screens only
tests/                   one folder per layer, plus shared fakes
tools/                   the checks
```

**Dependencies point inward only**: `desktop → application → foundation`, and
`adapters → (application ports | foundation ports)`. The foundation includes nothing
from `application/`, `adapters/` or `desktop/`; the application includes nothing from
`adapters/` or `desktop/`. `tools/check_layering.py` enforces exactly this.

**Correction (2026-10-01):** the table above says `voidverguenza` links Palabra. In the
hexagonal layout it does not: cryptography reaches it through the crypto **port**, and
the Reticulum **adapter** is what links Palabra. That keeps the domain testable with no
network library, and lets Hormiga supply its own adapter if it ever needs to.

# Layering, checked

`tools/check_layering.py` (Hormiga keeps one; Verguenza writes its own, so its build
never depends on a sibling's `tools/` folder) reads every `#include` and fails the
build if a file includes across a forbidden edge in the table above.
`tools/check_okf_links.py` fails it if an OKF link points at nothing. Both run in
`ctest` from phase 0.

A third check is specific to Verguenza: **`tools/check_plaintext_paths.py`** lists
every function that takes or returns a `Secret` and fails if one is not preceded by
a comment saying what it does with the plaintext (ground rule 4: *every path that
touches one says so in a comment*). A rule nobody checks is a wish.

**And a configure-time check on Palabra:** Verguenza refuses to configure if
`VOIDPALABRA_RNS_LOG_LEVEL` is 8 or more (microReticulum's trace level prints
plaintext and keys), and refuses to build the parts that make keys until Palabra's
generator is seeded from the operating system ([sealing](/concepts/sealing.md)).

# The one type that may hold a plaintext

Built as `SecretBuffer` (`src/foundation/secret/secret_buffer.hpp`): memory from the
installed secure-memory port (on Windows, its own `VirtualAlloc`'d, `VirtualLock`'d pages,
`SecureZeroMemory`'d on release), move-only, counted, with `data()` and `view()` for the few
callers that pass it on. The founding sketch's `read`/`write` calls, which would make the
memory unreadable between uses, are **not built**; the type's header says so, and the
memory scan (roadmap 1B.12) decides whether they are worth their cost.

- The memory calls are the operating system's, in `verguenza_platform`. That is
  memory handling, not a cryptographic primitive, so ground rule 3 is not in play.
- Move-only: a copy is a second plaintext nobody wipes.
- No conversion to `std::string`, `std::string_view` or a `char*` that outlives a
  `read` call. The functions that must hand bytes to the outside (the clipboard,
  ImGui's draw call for a held reveal, Palabra's crypto calls) carry the
  ground-rule-4 comment.
- Keys (value keys, shares) are `Secret`s too. Whether the device's own identity
  keys can be is up to Palabra's API taking caller-owned buffers
  ([boundary](/concepts/boundary.md)).

# Toolchain

**GCC or Clang; MSVC is not supported.** Void Palabra's Reticulum companion cannot
build under MSVC (its vendored Crypto uses GCC statement expressions; Palabra commit
`a02b154`), and every phase now needs it. On Windows that means MSYS2 UCRT64, as Void
Hormiga builds, with the static runtime flags Hormiga keeps in one place. C++20.
Siblings arrive by `add_subdirectory` from `../VoidMaiz` and `../VoidPalabra` (Void
Maiz brings Void Core), each path a cache variable, as Hormiga does.

# Tests that keep the ground rules true

Every secret a test uses is **generated at test time** and obviously fake
(`vg-test-<random hex>`), never a fixture in the tree (ground rule 2).

| test | keeps |
|---|---|
| **round trips** for every format, and **one refusal test per refusal** in [formats](/concepts/formats.md) (a flipped byte in each region of an envelope, a body moved under another header) | the formats |
| **the plaintext grep**: create a vault with generated values, use every verb, then search the state document, the journal, the log, the record, the bundle and the data folder for each value's bytes (and its base64 and hex) | ground rule 4, on disk |
| **the memory scan**: after a value is entered, revealed, hidden and the vault locked, scan the process's own readable memory for it (Windows: `VirtualQuery` over the process) | ground rule 4, in memory. **Expected to find ImGui's copy, and possibly microReticulum's** (its `Bytes` never wipes); the test records what it finds rather than pretending, and becomes strict once each copy is dealt with |
| **the record's rules** as pure functions: backdating after a removal is void, concurrent mutual removal removes both, a rollback through the document changes nothing, a weakened envelope without a signed policy change is invalid | [trust](/concepts/trust.md) |
| **consent formulas** as pure functions: every formula in [authority](/concepts/authority.md), satisfied and unsatisfied sets, monotonicity checked by property test | [authority](/concepts/authority.md) |
| **two processes** (phase 3), because one Reticulum node per process is a microReticulum limit (`VoidPalabra:/concepts/reticulum.md`) | sync, enrolment, recovery |
