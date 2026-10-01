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

Everything is already a dispatcher command (ground rule 5). The record is those
commands, signed:

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

Each device signs its own entries. Void Palabra keeps history as a partial order
of changes (`VoidPalabra:/concepts/history-graph.md`), and each signed entry
names the entries it follows by their content addresses. So:

- **Removing an entry** breaks the chain on every device that has the later
  entries, and shows.
- **Changing an entry** breaks its signature.
- **One device lying about its own past** is possible only before its entries
  reach anyone else. Once synced, its history is everyone's.

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
