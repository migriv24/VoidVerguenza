---
type: Concept
title: Audit — the record of every use
description: "Every addition, change, approval, reveal and client use is a signed dispatcher command, so 'who saw the alarm code, and when' has an answer no single member can quietly edit. What the record proves, what it cannot (a device that opened a value offline and never synced), how it is kept tamper-evident, what it never contains, and how it feeds expiry, usage and Void Hormiga's key monitors."
tags: [status:planned, audience:dev, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# Why

In a vault several people share, the most common question after an incident is
not *"was the encryption broken"* but *"who looked at this?"* The audit record
answers it.

# What is recorded

Everything is already a dispatcher command (ground rule 5). The record is the
signed objects those commands write to the vault's [signed record](/concepts/trust.md)
([commands](/concepts/commands.md) lists which verb writes what):

| event | recorded as |
|---|---|
| a secret added, changed, rotated, removed | the command, the device, the member, the new envelope's address |
| a value **revealed** on a screen or copied | a `reveal` act: secret, device, member, how (shown, copied, exported) |
| a value **used** by a client | a `use` act: secret, client, the client's stated purpose ("deploy-site"), device |
| a request, each approval, each denial, expiry | the request act and its approvals |
| a member or device added, removed, marked lost | the command, with its approvals |
| a policy changed | old and new, with approvals |

**Never recorded:** a value, a DEK, a share, a passphrase, or anything from
which they could be derived.

# Tamper evidence

Each device signs its own objects, and each object names two things by content
address: its parents (the objects it was written after), and **the previous object this
device wrote** ([formats](/concepts/formats.md)). So every device's objects form a
chain of their own, inside the record's partial order. And:

- **Nothing is removed by sync.** The signed record only grows; a device that
  deletes an entry from its own store deletes it nowhere else.
- **Withholding shows.** A device's chain with a gap (an object naming a previous
  one nobody has received) is visibly incomplete.
- **Forking shows.** Two objects by one device naming the same previous object
  mean that device told two stories. That is evidence of a modified client, shown
  to the members.
- **Changing an entry** breaks its signature and its address.
- **One device lying about its own past** is possible only before its objects
  reach anyone else. Once synced, its history is everyone's.

**Recorded before shown.** A reveal's act is written before the value is
decrypted for the screen, and a reveal that cannot be recorded is not performed
([commands](/concepts/commands.md) §"Reveal").

**A removed device's acts are kept.** An act signed by a device outside its
removal's cutoff has no authority ([trust](/concepts/trust.md) rule 2), but it is
still evidence: a reveal a lost laptop reports when it reconnects is exactly what
the members need to know. It is shown as *"reported by a removed device"*.

# What the record cannot prove

- **A reveal on a device that never syncs again.** A device can open a value it
  holds a seal for while offline, and if it is then lost, its `reveal` entry never
  arrives. The record proves what was reported. For *guarded* secrets, the
  approvals needed beforehand *are* recorded on the approvers' devices, so the
  request is known even if the reveal is not. For *split* secrets, the shares
  were sent by the approvers, so the opening is known.
- **What a person did with a value after seeing it.**

# What it feeds

- **Usage per secret, per member, per device, per client**: the key monitors
  Void Hormiga's Antfarm v2 designed (`../VoidHormiga/okf/concepts/platform/antfarm/v2/keys.md`
  §4: *"usage from our own log comes first on purpose"*).
- **Stale access**: a member who has not used a secret in a year may not need it.
- **Rotation reminders**: secrets revealed under *rotate after reveal*.
