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
| **A laptop is stolen, powered off** | the thief has sealed files | device keys rest under the person's passphrase (Argon2id) — [sealing](/concepts/sealing.md) |
| **Someone copies a user's folder** | the same: nothing opens without the passphrase | the passphrase, deliberately not a key derived from a file in that folder |
| **A phone is lost** | it is marked lost from another device; it stops counting for approvals; secrets are re-sealed without it; the app lists what to change at vendors | [people and devices](/concepts/people-and-devices.md) §"Losing a device" |
| **An eavesdropper on the store's network** | sees encrypted Reticulum traffic, no addresses inside | Reticulum links (sealed, forward-secret) — [sharing](/concepts/sharing.md) |
| **A device in the middle during enrolment** | the two screens show different codes, and the person refuses | the short code compared on both screens |
| **A member looks at secrets they were not given** | they hold bytes they cannot open | per-device seals |
| **A member reveals a guarded secret without asking** | a normal client refuses; the attempt and every reveal are recorded | guarded policy, [audit](/concepts/audit.md) |
| **A member with a modified client tries to open a split secret alone** | cannot: they hold one share | split policy (mathematics) — [quorum](/concepts/quorum.md) |
| **A member forges an approval or a membership change** | every device refuses a change without a valid signature from a device with the role | signed changes — [sharing](/concepts/sharing.md) |
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
| **Every recovery holder lost** | the vault is lost by design. A recovery that works without them is a backdoor |
| **A forgotten passphrase with no recovery set up** | the same |
| **Windows paging memory to disk, or the clipboard history** | requested against, not guaranteed |

# The rule this page enforces

Any feature that would move a row from the second table to the first must say
how. Any feature that would move a row the other way (convenience unlock, a
remote enrolment, an open policy) must be visible as such on screen, at the
moment it is chosen.
