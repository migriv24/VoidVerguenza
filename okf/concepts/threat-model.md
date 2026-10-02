---
type: Concept
title: Threat model
description: "Who Verguenza defends against and how, written as concrete situations in a small business with a shared vault: a stolen laptop, a copied user folder, a lost phone, an eavesdropper on the store's Wi-Fi, a curious member, a member who leaves angry, a modified client, a compromised device. And the list of what it does NOT defend against, which a password manager owes its users as much as the first list."
tags: [status:planned, audience:all, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# Defended

| situation | what happens | the mechanism |
|---|---|---|
| **A laptop is stolen, powered off** | the thief has sealed files, and the device's identity file | **depends on [Q12](/developer_questions.md)**: a passphrase or a hardware-backed key store protects the identity; Windows DPAPI alone does not stop someone who can log in as the person — [sealing](/concepts/sealing.md) §"A device's keys at rest" |
| **Someone copies a user's folder** | the same, and the same dependency | Palabra keeps the identity as a plain file today; Verguenza wraps it ([Q12](/developer_questions.md)) |
| **A phone is lost** | it is marked lost from another device; it stops counting for approvals; secrets are re-sealed without it; the app lists what to change at vendors | [people and devices](/concepts/people-and-devices.md) §"Losing a device" |
| **An eavesdropper on the store's network** | sees encrypted Reticulum traffic, no addresses inside | Reticulum links (sealed, forward-secret) — [sharing](/concepts/sharing.md) |
| **A device in the middle during enrolment** | the two screens show different codes, and the person refuses | the short code compared on both screens, after a commitment so the middle device cannot search for a match — [people and devices](/concepts/people-and-devices.md) |
| **A removed or lost device backdates an approval or an enrolment** | its object names parents from before its removal, and is void once the removal is known | the removal's cutoff — [trust](/concepts/trust.md) rule 2 |
| **A member edits the shared document to roll a secret back, or to mark a guarded secret open** | nothing changes: the current value and its policy are computed from signed objects, not read from the document | the [ledger](/concepts/trust.md) |
| **Someone relaying or recording traffic gets a split secret's share** | it is sealed to the requesting device's own key; the link is only a transport | [quorum](/concepts/quorum.md) §"How a split secret opens" |
| **A device tells two members two different histories** | its chain forks, and the fork is shown | each object names its device's previous one — [audit](/concepts/audit.md) |
| **A member looks at secrets they were not given** | they hold bytes they cannot open | per-device seals |
| **A member reveals a guarded secret without asking** | a normal client refuses; the attempt and every reveal are recorded | guarded policy, [audit](/concepts/audit.md) |
| **A member with a modified client tries to open a split secret alone** | cannot: they hold one share | split policy (mathematics) — [quorum](/concepts/quorum.md) |
| **A member forges an approval or a membership change** | every device refuses a change without a valid signature from a device holding the capability, with the consent its rule asks for | signed objects judged by their causal past — [trust](/concepts/trust.md), [authority](/concepts/authority.md) |
| **Someone edits the record afterwards** | the chain breaks on every device that synced past it | signed, content-addressed history |
| **A member leaves angry** | their devices stop counting and stop receiving; the app lists what they could open, to rotate at vendors | revocation, honestly limited |
| **A client application (Hormiga) is compromised** | it gets only what its grant allows, only when asked, and every use is recorded | [clients](/concepts/clients.md) grants |

# Not defended, said plainly

| situation | why not |
|---|---|
| **A device that is unlocked, in front of an attacker** | it holds open keys. Auto-lock shortens the window; nothing closes it |
| **Malware on an unlocked device** (a keylogger, a screen reader) | it sees what the person sees. No application can defend against its own machine |
| **A member who already saw a secret** | they know it. Only changing it at the vendor helps |
| **A guarded (not split) secret on a modified client** | the device holds a seal; honest software is what refuses. The app labels it so |
| **Faked presence** | "present" means the approval came through a local interface; whoever controls the network can fake that |
| **Every device that knows a secret lost, and no cold holder** | that secret is lost by design. A recovery that works without them is a backdoor — [recovery](/concepts/recovery.md) |
| **A forgotten passphrase (if [Q12](/developer_questions.md) chooses one)** | that device is lost; the group re-admits the person on a new one ([recovery](/concepts/recovery.md)) |
| **Predictable keys from Reticulum's random generator, until Palabra seeds it from the operating system** | not defended by Verguenza, and so Verguenza makes no keys until it is fixed — [sealing](/concepts/sealing.md) §"Randomness" |
| **Windows paging memory to disk, or the clipboard history** | requested against, not guaranteed |
| **Another program reading the clipboard while a copied value is on it** | any program of the same user may; the 30-second clear shortens the window |
| **The text field's own copy of a typed value** | Void Maiz's text fields are Dear ImGui's, which keeps a working copy Verguenza cannot wipe. Measured in phase 1 before anything is claimed ([commands](/concepts/commands.md) §"Intake") |
| **A device's honest offline roster work, when it is removed concurrently** | dropped by design: the cost of rule 2 in [trust](/concepts/trust.md). Its person redoes it from another device |
| **A stolen admin device removing others before it is itself removed** | concurrent removals all take effect ([trust](/concepts/trust.md) rule 3). A vault-wide policy requiring two approvals to remove anyone is what stops one device doing it alone; the default asks for that |

# The rule this page enforces

Any feature that would move a row from the second table to the first must say
how. Any feature that would move a row the other way (convenience unlock, a
remote enrolment, an open policy) must be visible as such on screen, at the
moment it is chosen.
