---
type: Concept
title: The application (desktop first)
description: "What a person sees: the lock screen, the vault and its secrets, a secret's detail with hold-to-reveal and a copy that clears itself, the approvals inbox, the people and devices, and a Void Maiz graph of who can open what. Built as a Void Maiz host on the Interaction Combinators shell pattern, every gesture a dispatcher command. What the phone will add later."
tags: [status:planned, audience:all, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# Shape

A Void Maiz host, one window, panes, view state in config: the pattern
Interaction Combinators established and Void Hormiga follows. Every gesture is
a dispatcher command; the command bar reaches the same verbs an agent uses.

# Screens

| screen | what it shows |
|---|---|
| **Lock** | the passphrase box. Nothing of the vault, not even names, until unlocked |
| **Secrets** | the vault's secrets as cards or a list: title, kind, tags, expiry, the policy's strength as a small symbol (open, guarded, split). Search by tag, provider, kind |
| **Secret detail** | the description; **hold to reveal**; **copy** (clears the clipboard after 30 s); history of versions; who can open it (members and devices); the audit trail of this secret |
| **Approvals** | requests waiting for this device's approval, each with its sentence, who asks, from which device, present or not, and time left. Approve / Deny. Below, this device's own requests and how many approvals each has |
| **People and groups** | people, the groups they are in (groups inside groups), each person's label computed from what they hold (*admin*, *member*, or *custom: admin without payroll*), their devices, last seen, presence. Admit someone, grant or recall a capability (showing whose consent it needs), enrol a device (the code on both screens), mark lost. Amended 2026-10-01 ([authority](/concepts/authority.md)) |
| **Graph** | a Void Maiz canvas of the three relations, switchable or overlaid ([authority](/concepts/authority.md)): **knowledge** (devices *can open* secrets; who *already has*), **authority** (who *delegated what to whom*, back to its source; who *may recall* it), **consent** (which groups an action needs). The question *"if Ana's laptop is stolen, what must we rotate?"* is a knowledge query: select the laptop, see every secret it reaches. *"Who could take the IT person's power away?"* is an authority query. The People list orders itself only by a **lens** the person picks (exposure, reach, voice, dominance), never by a default rank ([standing](/concepts/standing.md)) |
| **Clients** | applications enrolled in this vault (Hormiga on the office PC), their grants, their recent uses |
| **Audit** | the whole record, filterable by secret, member, device, client, kind of event |

**The graph is why this is a Void Maiz application** rather than a list with
buttons. Access in a shared vault is a graph (people, devices, secrets, shares,
grants), and the dangerous facts are paths in it: one device that alone reaches
every guarded secret, a share holder who is also the only owner, a client with a
grant wider than anything it uses.

# Things the application refuses to do

- Show a value without a deliberate action.
- Write a value into a log, a toast, an error message or the command bar.
- Offer an "export to plain text" that does not go through its
  [policy](/concepts/quorum.md).
- Let an open policy look like a guarded one.

# The phone, later

The phone's natural job is **approver**: a request appears, the phone shows its
sentence, a person approves with the phone's own lock (fingerprint or face, where
the platform offers it). Second: reading a secret in the field (the Wi-Fi
passphrase for a new device, shown as a QR code). The phone holds its own device
keys like any device. Void Hormiga's phone work (`../VoidHormiga/okf/concepts/sections/mobile.md`)
and Void Maiz's touch support are what it will be built on.
