---
type: Concept
title: Commands — the dispatcher's verbs, and the door a value never uses
description: "Ground rule 5 made concrete. Void Core has no host-registered verbs, so Verguenza's verbs (`secret add`, `device enrol`, `approve`, `reveal`…) are compiled by the application into ordinary Core commands, the way Void Hormiga's `farm` verbs are. The cryptography (sealing, signing) happens in the host before the commands run; the commands record the resulting ledger objects by address. A value enters through an intake slot, never through a command line, because Void Core's journal and log record the canonical command text. A reveal is recorded before it is shown. The verb catalogue for phases 1 to 4."
tags: [status:planned, audience:dev, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# The constraint that shapes everything here

Void Core records every successful mutating command twice: on the **log spine**
(`[ISO] INFO verb: <the full command>`) and, when enabled, in the **journal**,
whose entries carry the **canonical command line** (`VoidCore:SPEC.md` §6.2, §9).
That is what makes a change replayable and auditable, and it means:

> **Anything in a command line is written down.** A value, a passphrase, a DEK or
> a share must never be one.

Void Hormiga met the same wall and said so on its own screen: *"a value typed
here would stay in the command history"* (its `farm` verbs refuse a key value in
the command bar and take it from the Inspector's box or standard input).
Verguenza makes that the rule for every verb.

# How a Verguenza verb runs

Void Core has no host-registered verbs: its catalogue is fixed, and a host's own
operation reaches it as `effect <op>` (`VoidCore:SPEC.md` §6.2, §9). Hormiga's
answer, which Verguenza takes: **an application verb compiles to ordinary Core
commands** (one `batch`, attributed to the actor), and only real crossings to the
outside world go through `effect`.

```
person or agent ──► "secret add --kind password --title 'POS admin' --value @intake:1"
                       │
   application         │ 1. parse; refuse anything that looks like a value in a line
   (verguenza host)    │ 2. take the value from intake slot 1 (guarded memory)
                       │ 3. seal it, sign the envelope        ← Reticulum, through Palabra
                       │ 4. store the envelope in the signed record   ← content-addressed
                       │ 5. wipe the intake slot
                       ▼
   Void Core           batch: rune new secrets/… ; set envelope sha256:… ; set kind password …
                       (pure commands: replayable, journalled, no value in them)
```

**Replay.** Replaying the journal's pure entries rebuilds the document,
*given the signed objects they name*, which are content-addressed and never
change. Steps 3 and 4 are not replayed; their results are what the commands
point at. This is the same split as Void Palabra's content references: the
document names, the store holds.

**The projection is rebuilt the same way.** When objects arrive from a peer,
the host checks them ([trust](/concepts/trust.md)) and dispatches the Core
commands that bring the projection mantles up to date, attributed to a
`sync` actor so the log says why the document changed.

# Intake: the only door a value uses

An **intake slot** is a locked, wiped buffer (the `Secret` type, [sealing](/concepts/sealing.md))
that holds one value on its way into an envelope. It is filled by:

| source | when |
|---|---|
| the secret-entry box in the application | a person types or pastes a value |
| standard input, in `verguenza-cli` | a script pipes a value in (`echo … \| verguenza-cli secret set …`) |
| a file chosen in a dialog | a `file` secret, read straight into the slot |
| the generator | Verguenza makes the password itself (Reticulum's `random` once it is fit for keys, a chosen alphabet) |

A command names a slot (`@intake:1`), never its contents. A slot is wiped when its
command finishes, when it fails, and when the vault locks. A command naming an
empty slot fails. A command line that contains `--value=` followed by anything
other than `@intake:N` **is refused before it reaches the dispatcher**, so a value
typed into the command bar by mistake is never logged.

**What is honest about intake.** The secret-entry box is a Void Maiz (Dear ImGui)
text field, and ImGui keeps its own working copy of the text being edited, in
memory Verguenza does not allocate and cannot wipe. Phase 1 measures where a typed
value ends up ([roadmap](/roadmap.md), the memory scan) before claiming anything
about it. If the copy survives, the fix is a Void Maiz secure text field, asked
for upstream (ground rule 7).

# Reveal: recorded, then shown

A reveal touches no state document: it puts pixels on a screen or text on a
clipboard. It still goes through the dispatcher, because ground rule 5 and the
[audit](/concepts/audit.md) record need it to:

1. `reveal <secret> --how show|copy|qr` checks the policy (and the approvals,
   for a guarded secret, or collects the shares, for a split one).
2. The host writes the signed **reveal act** to the signed record and dispatches the
   command that projects it.
3. **Only then** is the value decrypted and shown or copied.
4. Hiding it again (releasing the button, the clipboard timer) wipes the buffer.

**If step 2 fails, step 3 does not happen.** A reveal that cannot be recorded is
not performed. The order matters because the other order (show, then record)
loses the record exactly when something goes wrong: a crash, a full disk, a
person pulling the plug.

A **use** by a client follows the same order: the use act is written before the
value leaves the agent ([clients](/concepts/clients.md)).

# The verb catalogue

Verb names are provisional; the shape is not. *Writes* names the signed object a
verb produces; every verb also dispatches the Core commands that project it.

| verb | does | writes | phase |
|---|---|---|---|
| `vault new <name>` | founds a vault: the source capability, the founding group, its first person and this device | signed objects | 1 |
| `group new <name> [--in <group>]` / `group add <group> <person\|group>` | groups inside groups ([authority](/concepts/authority.md)) | signed object (+ consent) | 1 |
| `grant <who> <verbs> --over <region> [--until <time>] [--template <t>]` / `recall <capability>` | delegate, restrict, recall; a template grants its bundle in one gesture | capability (+ consent) / recall | 1 |
| `template define <name> …` | names a bundle of capabilities for granting and describing | — (description only) | 1 |
| `vault lock` / `vault unlock` | wipes every key and value in memory / unlocks this device's identity ([Q12](/developer_questions.md): the OS key store, or a passphrase from intake) | — (unlock is not a change; lock is not either) | 1 |
| `secret add --kind <k> --title <t> [--field k=v]… --value @intake:N` | a new secret | envelope (v1) | 1 |
| `secret set <secret> --value @intake:N` | rotates its value | envelope (v+1) | 1 |
| `secret describe <secret> --field k=v…` | edits the description | — (description only; ordinary merge) | 1 |
| `secret remove <secret>` | removes it from view; its history ages out per policy | signed object (so it cannot be undone through the merge) | 1 |
| `secret history <secret>` | lists versions | — (read) | 1 |
| `reveal <secret> --how show\|copy\|qr` | above | act | 1 |
| `generate --length N --alphabet …` | fills an intake slot with a fresh password | — | 1 |
| `bundle export <path>` / `bundle import <path>` | the `.verguenza` file ([sealing](/concepts/sealing.md)) | — (export of envelopes is not a reveal; a *plaintext* export is, below) | 1 |
| `export plaintext <path>` | writes values out unencrypted, under the vault-wide policy | act, request | 4 |
| `client grant <app> <device> <grant>` / `client revoke …` | [clients](/concepts/clients.md): a capability whose holder is a client | capability | 2 |
| `use` (agent only, never typed) | a client asks for a value | act | 2 |
| `person admit <name> --to <group> [--template <t>]` / `person remove …` | [people and devices](/concepts/people-and-devices.md); in phase 1 only the founder exists, so these work but admit no one who can sync | signed object (+ consent) | 1 (3 to be useful) |
| `device enrol` / `device lost <device>` | the in-person enrolment, the loss | signed object, new envelopes (re-seal) | 3 |
| `reseal <secret> [--to <device>]` | seals an existing value to a device that may now open it | envelope (same version, new seals) | 3 |
| `policy set <scope> --consent '<formula>' …` | sets or changes a policy | signed object (+ the old policy's consent) | 4 |
| `request <action>` / `approve <request>` / `deny <request>` | [quorum](/concepts/quorum.md) | request, approval | 4 |
| `cold-holder add …` / `recover <person> --onto <device>` | [recovery](/concepts/recovery.md): re-admission, then re-sealing by those who know; a cold holder only when no one does | signed object, request, envelopes | 3 (cold holders 5) |

**Agents use the same verbs.** The command bar, `verguenza-cli` and a script all
reach this table; none of them can reach a value except through intake and
reveal, which a person (or a client's grant) has to authorize.
