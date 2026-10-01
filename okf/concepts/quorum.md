---
type: Concept
title: Quorum — actions that need several devices to agree
description: "The author's 'lan based authentication (such as having multiple devices authenticate an action)', as a design: requests, approvals (signed, time-limited, optionally required to come from devices present on the local network), and policies of k-of-n. Three strengths kept apart and labelled: open, guarded (honest software counts the approvals) and split (the value's key is divided so it cannot be opened without k devices, enforced by mathematics). Which actions need a quorum, how a request reaches approvers with a sentence saying what it will do, social recovery, and what a quorum cannot do."
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
  not good at 17:00. Each policy sets the window (default 10 minutes).
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
2. Each holder that approves opens *its* share (its seal) and sends the share to
   the requester over a Reticulum link: forward-secret, and authenticated by
   [the device's key](/concepts/people-and-devices.md), so the share reaches only
   the device that asked.
3. With k shares, the requester rebuilds the DEK, opens the value, uses it, and
   wipes the DEK and the shares.

**What split cannot do.** After step 3 the requester has seen the value. A
quorum guards *the moment of opening*, not the future. For the most sensitive
secrets a policy can add *rotate after reveal*: the application reminds the
group to change the value once it has been used, and marks the secret until
someone does.

Split needs threshold sharing, which libsodium does not provide. That is
[Q1](/developer_questions.md).

# Policies

A policy is a rune in the `policies` mantle:

| field | example |
|---|---|
| `strength` | `open`, `guarded`, `split` |
| `k`, `of` | `2` of `member`s (or of a named group: `managers`) |
| `present` | `required` / `preferred` / `no` |
| `window` | `10m` |
| `rotate_after_reveal` | `yes` / `no` |
| `self_counts` | does the requester's own approval count toward k |

**Vault-wide policies** cover actions that are not about one secret:

| action | default policy |
|---|---|
| add or remove a member | admin + 1 more (2 of admins and owners) |
| enrol a device for an existing member | that member's other device, or an admin |
| change a policy | the stricter of the old and new policy, plus an admin |
| export the vault in plaintext | 2 of owners, present |
| add a client, or widen a grant | an admin |
| recovery | k of the recovery holders (below) |

**Changing a policy can never make it weaker without satisfying the old one.**
Otherwise anyone could lower the bar and then walk under it.

# Requests across the network

A request is a rune in the `requests` mantle (an `act`), so it syncs to every
device like any other change, and the approvals land beside it. Over Reticulum,
devices that are present are also told immediately, so an approval can arrive in
seconds: a phone buzzing in a manager's pocket is the eventual picture (phones
come later; a desktop's approvals inbox comes first).

A request that is not satisfied within its window **expires**, and the record
keeps it, with who approved and who did not.

# Social recovery

If a person forgets their passphrase, or every device of a vault's only owner is
lost, something must let the group recover, and it must not be a backdoor.

**Lean: recovery is a split secret like any other.** When a vault is created, a
**recovery key pair** (X25519) is generated. Its public half is one more
recipient on every envelope: each envelope carries a seal for it, like a seal
for a device. Its private half is split into shares, one sealed to each of n
named *recovery holders* (people, through their devices), any k of which rebuild
it. Recovery is a request that needs k holders to send their shares to a new
device; that device rebuilds the recovery key, opens the envelopes, re-seals them
to itself, and wipes the recovery key. No holder alone, and no developer, can
recover anything. If too many holders are lost, the vault is lost, and the
application says so when the holders are chosen. [Q4](/developer_questions.md).

# What a quorum is not

- **Not protection from a device that is already unlocked and open in front of
  someone malicious.** It protects the actions that need approval, not the
  secrets that device may open freely.
- **Not proof of physical presence.** Present means *came through a local
  interface*. Someone who controls the network can fake it.
- **Not a substitute for changing a leaked secret at its vendor.**
