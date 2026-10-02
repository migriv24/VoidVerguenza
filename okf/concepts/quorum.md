---
type: Concept
title: Quorum — actions that need several devices to agree
description: "The author's 'lan based authentication (such as having multiple devices authenticate an action)', as a design: requests, approvals (signed, time-limited, optionally required to come from devices present on the local network), and policies whose consent is a formula over people and groups (k-of-n is its simplest case). Three strengths kept apart and labelled: open, guarded (honest software counts the approvals) and split (the value's key is divided so it cannot be opened without k devices, enforced by mathematics). Which actions need a quorum, how a request reaches approvers with a sentence saying what it will do, where recovery went, and what a quorum cannot do."
tags: [status:planned, audience:dev, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# The idea

The author: *"this software will need to consider lan based authentication
(such as having multiple devices authenticate an action)."*

A **request** is a device asking to do something: reveal a secret, add a member,
export the vault, let a client use a key. A **policy** says what the request
needs. An **approval** is another device saying yes. When enough approvals
arrive, the action happens. When they do not, it does not.

This is the familiar idea of two keys turned at once (a bank vault, a launch
console), applied to the devices a group already carries.

# Approvals

An approval is a small signed statement:

```
{ request: <id>, decision: "approve" | "deny",
  device: <device id>, member: <member id>,
  until: <time>, present: <true if sent over a local link> }
  signed with the approving device's Ed25519 key
```

- **Signed**, so every device can check it and the [audit](/concepts/audit.md)
  record can prove it.
- **Time-limited**: an approval for *"reveal the safe code"* given at 9:00 is
  not good at 17:00. Each policy sets the window (default 10 minutes). The window
  is measured on clocks, which devices do not share: for a **guarded** secret the
  opening device checks `until` against its own clock (software-enforced, and a
  device with a wrong clock judges wrongly). For a **split** secret the window
  hardly matters, because a holder sends its share at the moment it approves,
  not later.
- **Present or not.** If it arrived over a Reticulum link through a local
  interface (the store's LAN), it is marked `present`. A policy can require
  present approvals: *"two people in the building must agree."* Presence is a
  fact about the network path and can be faked by someone who controls the local
  network, which is why it is a policy option and never the only requirement.
- **One per device, one per person.** Two approvals from the same member's two
  devices count once unless the policy says otherwise. A quorum of people is the
  usual meaning, and the default.

**The approving screen says what it is approving**, in a sentence written for
the person deciding, the way Void Hormiga's effects carry a consequence sentence:
*"Ana's laptop asks to REVEAL the alarm code. She will see it on her screen."*
or, through a client, *"Void Hormiga on the office PC asks to use the GitHub key
to PUBLISH THE WEBSITE."* An approval of something the approver cannot read is
not an approval.

# Three strengths, never confused

| strength | what stops an action without enough approvals | honest label in the application |
|---|---|---|
| **open** | nothing: any device the secret is sealed to opens it | *"anyone with access can open this"* |
| **guarded** | honest software: the device holds a seal for the secret, but the application refuses to open it until it has collected k valid approvals | *"approvals required (enforced by the app)"* |
| **split** | mathematics: the value's key is divided into n shares, each sealed to one device. Any k shares rebuild it; fewer reveal nothing. To open, the requester must *receive* k decrypted shares from k devices that chose to send them | *"approvals required (enforced by encryption)"* |

**Why both guarded and split exist.** Split is the real guarantee, but it has
costs: every reveal needs k devices to be reachable and willing, a share holder
who loses their device loses a share, and changing who holds shares means
re-splitting. Guarded is convenient and still meaningful against a *person*
(the record shows every reveal, and a normal client refuses), but not against a
*modified client* on a device that holds a seal. The design will not let the two
look alike.

**How a split secret opens:**

1. The requesting device sends a request to the share holders.
2. Each holder that approves opens *its* share (its seal), **seals it again to the
   requesting device's own identity** (`Identity.encrypt`), signs the result with
   its approval, and sends it. The Reticulum link it travels over is only a
   transport.
3. With k shares, the requester rebuilds the DEK, opens the value, uses it, and
   wipes the DEK and the shares.

**Why step 2 re-seals instead of trusting the link** (amended 2026-10-01). Both
use Reticulum's cryptography now, so this is no longer about whose primitives: it is
about **what the protection is attached to**. A link protects bytes while they are on
that link; a share that is relayed, queued, stored by a keeper, or forwarded by a
device that is not the requester is outside it. Sealing to the requester's identity
attaches the protection to the share itself, whatever path it takes. The rule,
everywhere: **nothing secret relies on the link.**

**What split cannot do.** After step 3 the requester has seen the value. A
quorum guards *the moment of opening*, not the future. For the most sensitive
secrets a policy can add *rotate after reveal*: the application reminds the
group to change the value once it has been used, and marks the secret until
someone does.

**Strengthening a policy protects only the next value** (2026-10-01). Moving a
secret from open or guarded to split re-divides a *new* DEK, but every device that
held a whole seal for the old one can still open the old envelope, and may already
have. So the application treats *"make this split"* as *"make this split and
change the value"*: it writes the new policy, then asks for a new value (and, if the
old one ever left the vault, for a change at the vendor). A policy change that
does not rotate is labelled: *"stricter from the next value on; the current value
was open to N devices."*

Split needs threshold sharing, which neither Reticulum nor libsodium provides. That is
[Q1](/developer_questions.md).

# Policies

A policy says **what enforces** an action and **whose agreement** it needs. The
second part is a **consent formula** over people and groups, not a number
([authority](/concepts/authority.md), amended 2026-10-01): *k of n* is the simplest
case, and real groups need others.

| field | example |
|---|---|
| `strength` | `open`, `guarded`, `split` |
| `consent` | `2-of(managers)`, `IT ∨ 2-of(managers)`, `bookkeeper ∧ 1-of(owners)` |
| `present` | `required` / `preferred` / `no`, for the whole formula or one term of it |
| `window` | `10m` |
| `rotate_after_reveal` | `yes` / `no` |
| `self_counts` | does the requester's own approval satisfy a term |

A formula counts **people, not devices**, unless it says otherwise: two approvals
from one person's two devices satisfy one term once.

**Vault-wide policies** cover actions that are not about one secret. The defaults
below are written with the template names ([authority](/concepts/authority.md)
§"Labels"); a group replaces them with its own formulas:

| action | default consent |
|---|---|
| admit a person to a group | `2-of(that group's admins ∪ owners)` |
| recall someone's capability, or remove them | **any one** of those whose consent granted it (recall is easier than grant) |
| enrol a device for an existing person | `that person's other device ∨ 1-of(admins)` |
| change a policy | the old policy's consent, and the new one's if it is stricter |
| export the vault in plaintext | `2-of(owners)`, present |
| grant a client, or widen a grant | `1-of(admins)` |
| admit a recovered person's new device | as enrolment ([recovery](/concepts/recovery.md)) |

**Changing a policy can never make it weaker without satisfying the old one.**
Otherwise anyone could lower the bar and then walk under it. "Weaker" is exact for
formulas: the new formula is weaker when some set of people satisfies it and not the
old one.

# Requests across the network

A request is a signed object in the vault's [signed record](/concepts/trust.md), and so
is each approval; both are projected into the `requests` mantle (an `act` rune
each) for the inbox and the graph. They sync with the record, and the approvals
land beside the request. An approval for a vault-wide action (adding a member,
changing a policy) is also **embedded** in the roster entry it authorizes, so the
entry carries its own proof. Over Reticulum,
devices that are present are also told immediately, so an approval can arrive in
seconds: a phone buzzing in a manager's pocket is the eventual picture (phones
come later; a desktop's approvals inbox comes first).

A request that is not satisfied within its window **expires**, and the record
keeps it, with who approved and who did not.

# Recovery

Moved to its own page, [recovery](/concepts/recovery.md), and rethought
(2026-10-01): recovery is mostly **re-sealing by the people who still know**, with a
group's consent to admit the new device, and the founding *recovery key* survives
only as one kind of **cold holder** for the case where nobody who knows a secret is
left. The finding from the first planning session stands there: no recovery path
goes around a split secret by default.

# What a quorum is not

- **Not protection from a device that is already unlocked and open in front of
  someone malicious.** It protects the actions that need approval, not the
  secrets that device may open freely.
- **Not proof of physical presence.** Present means *came through a local
  interface*. Someone who controls the network can fake it.
- **Not a substitute for changing a leaked secret at its vendor.**
