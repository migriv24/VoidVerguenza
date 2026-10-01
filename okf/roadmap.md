---
type: Roadmap
title: Roadmap
description: "Phases gated by exit tests, not dates: a vault on one device, the client seam Hormiga needs, several people, approvals, split secrets and recovery, the phone. Each phase names what it needs from Void Core, Void Maiz and Void Palabra."
tags: [status:planned, audience:dev, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

**Nothing is built.** Phases are ordered so that each is useful by itself.

## Phase 0 — The repository

A CMake project consuming Void Core, Void Maiz and Void Palabra the sibling way
(`add_subdirectory`), libsodium vendored as Void Hormiga does it, an empty Void
Maiz window, the OKF checker in ctest.

**Exit:** the window opens; `ctest` runs the OKF and layering checks.

## Phase 1 — One person, one device

Secrets of every kind, envelopes sealed to this one device, the device keys under
a passphrase, lock and auto-lock, hold-to-reveal, a self-clearing copy, the audit
record, the `.verguenza` bundle (save, open, back up).

**Exit:** a vault with ten secrets survives a restart; a wrong passphrase and a
flipped byte in any envelope both fail cleanly; no plaintext appears in the
state document, the log or the bundle (checked by a test that greps for the
generated values).

## Phase 2 — Clients (what Void Hormiga needs first)

The `voidverguenza` library (envelope, client protocol), the agent, `describe`
and `use` with open and confirm policies, the `clients` mantle and grants.

**Exit:** Void Hormiga, on the same computer, publishes a site with a GitHub
token it never stored, and the Verguenza audit record shows the use with
Hormiga's consequence sentence.

## Phase 3 — Several people

Members and devices, enrolment with the short code over Reticulum, sync over
Void Palabra, signed changes checked before merge, revocation with the
*needs rotation* list.

**Exit:** two computers in one room share a vault; a third device enrolled with a
wrong code is refused; a removed device cannot open a secret written after its
removal. **Needs two machines.**

## Phase 4 — Approvals

Requests, signed time-limited approvals, the approvals inbox, guarded policies,
vault-wide policies, presence on the local network.

**Exit:** revealing a guarded secret needs a second device's approval; an approval
given eleven minutes ago (with a ten-minute window) does not count; a policy
cannot be weakened without satisfying itself.

## Phase 5 — Split secrets and recovery

Threshold sharing (once [Q1](/developer_questions.md) is answered), split
policies, shares delivered over links, social recovery.

**Exit:** a split secret with k = 2 cannot be opened by one device even with a
modified client (tested by a test client that skips the policy check);
recovery on a new device works with k holders and fails with k − 1.

## Phase 6 — The phone as approver

After Void Hormiga's and Void Maiz's phone work matures.

## What each phase needs upstream

| phase | Void Core | Void Maiz | Void Palabra |
|---|---|---|---|
| 1 | nothing new | the shell, widgets | the bundle (persistence) |
| 3 | nothing new | `voidmaiz_net` over Reticulum | links; **signed merges** may belong here ([Q5](/developer_questions.md)) |
| 4 | nothing new | an inbox surface; notifications on a desktop | — |
| 5 | nothing new | — | — (threshold sharing is Verguenza's) |
