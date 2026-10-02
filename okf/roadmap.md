---
type: Roadmap
title: Roadmap
description: "Phases gated by exit tests, not dates: the repository; standing (the authority model, headless, which needs no cryptography and is what Hormiga will adopt) and a vault on one device; the client seam Hormiga needs; several people and recovery by re-sealing; approvals; split secrets and cold holders; the phone. Each phase names what it needs from Void Core, Void Maiz and Void Palabra. Reordered 2026-10-01 so the admin side starts first."
tags: [status:planned, audience:dev, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

**Nothing is built.** Phases are ordered so that each is useful by itself.

**What blocks what, as of 2026-10-01.** The authority model (phase 1, part A) needs
nothing from anyone and can start now. Everything that makes or uses a key (phase 1,
part B, onward) waits on two things from Void Palabra: **randomness fit for keys**,
and **the identity's operations in its header** ([boundary](/concepts/boundary.md)).
Both are drafted as messages. Ground rule 7 allows the author to grant direct edits
to a sibling; if that is granted for Palabra, these become Verguenza's own work.

## Phase 0 — The repository

A CMake project consuming Void Maiz (which brings Void Core) and Void Palabra with
its Reticulum companion, the sibling way (`add_subdirectory`, paths as cache
variables), built with GCC under MSYS2 UCRT64 (MSVC cannot build Palabra's Reticulum
companion, [architecture](/concepts/architecture.md)), an empty Void Maiz window, and
the checks.

| step | done when |
|---|---|
| 0.1 `CMakeLists.txt`, the five targets as empty shells, the sibling paths | configure succeeds from a clean clone with the siblings beside it, and fails with a clear message without them |
| 0.2 the configure-time checks on Palabra | configure refuses `VOIDPALABRA_RNS_LOG_LEVEL` ≥ 8; it reports whether Palabra's generator is seeded from the operating system, and the key-making targets are off until it is |
| 0.3 `tools/check_okf_links.py`, `tools/check_layering.py`, `tools/check_plaintext_paths.py` | each fails on a deliberately broken input in its own test, and passes on the tree |
| 0.4 the empty window | `voidverguenza-app` opens a Void Maiz window titled *Void Verguenza* and closes cleanly |

**Exit:** the window opens; `ctest` runs the OKF check and the layering check.

## Phase 1 — Standing, then one device

### Part A — the authority model, headless (no cryptography; can start now)

What the author asked to start on: *"managing the admin aspect of things"*. All of
it is pure functions over a set of objects, so it is built and tested before any key
exists, behind a `Signer`/`Verifier` interface that tests fill with a fake and phase
1B fills with Reticulum. It is also exactly the part Hormiga will adopt.

| step | done when |
|---|---|
| 1A.1 people, devices, **groups inside groups** ([authority](/concepts/authority.md)) | a group can contain people and groups; cycles of containment are refused; a person's groups are computed transitively |
| 1A.2 **capabilities**: source, delegate, restrict, merge; regions written in the capability | you cannot delegate what you do not hold; a restriction never widens; every capability walks back to its source |
| 1A.3 **consent formulas** and their evaluation | every formula in [authority](/concepts/authority.md) and [quorum](/concepts/quorum.md) evaluates right on satisfied and unsatisfied sets; a property test confirms monotonicity; "weaker than" between two formulas is decided correctly |
| 1A.4 **the record's rules** ([trust](/concepts/trust.md)) | judging by causal past; an object with missing parents is held; a backdated object after a removal is void; concurrent mutual removals both stand; a void parent does not poison an honest child |
| 1A.5 **mandates and recall** | a grant records whose consent made it; one of them can recall it; recall beats a concurrent grant |
| 1A.6 **templates and labels** | a person's label is computed from what they hold; *custom: admin without payroll* is reported when nothing matches; editing a template changes no one's authority |
| 1A.7 **the knowledge graph** | exposure per person and per device; *"what must we rotate if this laptop is lost"* answered from seals |
| 1A.8 **access regions and audiences** ([access](/concepts/access.md)) | a capability over a mantle, a glyph or a rune admits exactly its region; a rule over tags proposes an audience and is never consulted for admission; a shrinking audience proposes, never revokes |
| 1A.9 **standing** ([standing](/concepts/standing.md)) | the three weights derived from facts, keyed by source; Banzhaf power matches hand-computed values for the formulas on the authority page; the dominance order leaves the IT person and the bookkeeper incomparable; a threshold crossing emits one tag, with hysteresis; no code path reads a weight to decide anything (a test greps the decision functions) |

**Exit (A):** the store from [authority](/concepts/authority.md) (owner, IT,
bookkeeper, front desk, managers) can be built as objects, and every sentence on that
page about who may do what is a passing test.

### Part B — one person, one device (waits on Palabra)

| step | in | done when |
|---|---|---|
| 1B.1 `Secret` (locked, wiped memory) | `voidverguenza`, `verguenza_platform` | move-only; access outside `read` faults; a counter of live buffers reads zero after lock |
| 1B.2 the signing rule and the object frame, on Reticulum | `voidverguenza` | round trip; one refusal test per refusal in [formats](/concepts/formats.md); a signature made for one type fails as every other type, and as a Reticulum announce |
| 1B.3 envelope v1: seal, open, reseal | `voidverguenza` | round trip; a flipped byte in each region fails; a body under another header fails on its digest; a reseal keeps the body |
| 1B.4 the identity at rest ([Q12](/developer_questions.md)) | `verguenza_platform` | as Q12 decides: the copied-folder test fails to open it |
| 1B.5 the record on Palabra, with part A's rules | `voidverguenza` | the current value is the highest valid version, whatever the document says |
| 1B.6 intake, verbs, projection | `verguenza_host` | every phase-1 verb in [commands](/concepts/commands.md) through `verguenza-cli`; `--value=<anything but @intake:N>` refused before the dispatcher; replaying the journal rebuilds the same document |
| 1B.7 reveal, recorded first | `verguenza_host` | a reveal whose act cannot be written shows nothing |
| 1B.8 lock: idle, OS lock and sleep, on demand | `verguenza_platform`, host | after lock, no `Secret` is alive |
| 1B.9 the clipboard with history exclusion and the timer | `verguenza_platform` | cleared after the timer; something else copied meanwhile is left alone |
| 1B.10 the screens: lock, secrets, secret detail, people and groups, audit | the app | a person can do everything in 1B.6 without the command bar |
| 1B.11 the bundle (a Palabra archive) | `verguenza_host` | export, import on a fresh profile with the same identity: identical; without it: descriptions only |
| 1B.12 the plaintext grep and the memory scan | tests | the grep finds nothing on disk; the memory scan's findings (ImGui's copy, microReticulum's) are written into [commands](/concepts/commands.md) §"Intake", and Void Maiz and Void Palabra messages drafted for what survives |

**Exit (B):** a vault with ten secrets survives a restart; a flipped byte in any
envelope fails cleanly; no plaintext in the state document, the journal, the log, the
record or the bundle; a reveal that cannot be recorded does not happen; the memory
scan's result is recorded, whatever it is. **The v1 formats freeze here.**

## Phase 2 — Clients (what Void Hormiga needs first)

The client protocol in `voidverguenza`, the agent, `describe` and `use` with open and
confirm policies (the value sealed to the client's own identity, the `use` act written
first), client grants as capabilities.

**Exit:** Void Hormiga, on the same computer, publishes a site with a GitHub token it
never stored, and the Verguenza audit record shows the use with Hormiga's consequence
sentence.

## Phase 3 — Several people, and recovery by re-sealing

Enrolment with the committed short code over Reticulum, the record synced by
Palabra, descriptions by Palabra's merge, revocation with the *needs rotation* list,
**recovery as re-admission plus re-sealing** ([recovery](/concepts/recovery.md)), a
keeper node.

**Exit:** two computers in one room share a vault; a third device enrolled with a
wrong code is refused; a removed device cannot open a secret written after its
removal, and an enrolment it signs afterwards, naming old parents, is void on both
remaining devices; a person whose laptop is "lost" is re-admitted on a third device
and receives, by re-sealing, exactly what they were allowed to know. **Needs two
machines** (or processes: one Reticulum node per process).

## Phase 4 — Approvals

Requests, signed time-limited approvals, the approvals inbox, guarded policies with
consent formulas, vault-wide policies, presence on the local network.

**Exit:** revealing a guarded secret under `bookkeeper ∧ 1-of(owners)` needs both; an
approval given eleven minutes ago (with a ten-minute window) does not count; a policy
cannot be weakened without satisfying itself.

## Phase 5 — Split secrets and cold holders

Threshold sharing along a formula (once [Q1](/developer_questions.md) is answered),
split policies, shares sealed to the requester, cold holders
([recovery](/concepts/recovery.md), [Q4](/developer_questions.md)).

**Exit:** a split secret cannot be opened by fewer than its formula needs even with a
modified client (a test client that skips the policy check); a cold holder recovers a
one-person vault, and does not reach a split secret.

## Phase 6 — The phone as approver

After Void Hormiga's and Void Maiz's phone work matures. The phone's key store is the
natural answer to [Q12](/developer_questions.md) there.

## What each phase needs upstream

| phase | Void Core | Void Maiz | Void Palabra |
|---|---|---|---|
| 1A | nothing | nothing | nothing |
| 1B | nothing new | the shell, widgets; possibly a **secure text field** (if 1B.12 finds ImGui's copy) | **randomness fit for keys; the identity's operations in its header, with caller-owned buffers**; the archive |
| 3 | nothing new | `voidmaiz_net` over Reticulum | links and content references (built); signed utterances and capabilities when Palabra builds them ([Q5](/developer_questions.md)) |
| 4 | nothing new | an inbox surface; notifications on a desktop | — |
| 5 | nothing new | — | — (threshold sharing is Verguenza's) |
