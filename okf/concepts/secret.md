---
type: Concept
title: Secret
description: "What a secret is in Void Verguenza: a rune whose content DESCRIBES a secret (its kind, its provider, who added it, when it expires, what it costs, what it is for) and whose value lives only in a sealed envelope named by content address. The kinds, the fields, providers and compatibility carried over from Void Hormiga's Antfarm v2 Key node, rotation, expiry and cost, and the mantles a vault is made of."
tags: [status:planned, audience:dev, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# A secret is two things

| part | what it is | where it lives | who can read it |
|---|---|---|---|
| **the description** | a rune: name, kind, provider, notes, expiry, who added it, which envelope holds its value | the vault's state document, synced like any Void Core data | every member of the vault |
| **the value** | the password, the token, the file | a sealed [envelope](/concepts/sealing.md), named in the rune by its content address | only the devices it is sealed to, and only while unlocked |

This is the shape Void Hormiga reached for its Antfarm v2 Key node (the rune
syncs, the value travels sealed into each vault). Verguenza makes it the whole
application.

**Why the description is readable by every member.** A shared vault is useful
because people can see *what* is in it: that the store has a camera-recorder
admin login, that it expires in March, that Ana added it. Hiding even the names
from members is possible (a description can be sealed too) and is offered as a
per-secret choice, `hidden`, for the rare secret whose existence is itself
sensitive. It costs search and the graph, so it is not the default.

# Kinds

A glyph per kind, because each kind has different fields and a different face:

| kind | value | typical fields |
|---|---|---|
| `password` | a password | username, address (URL or device), notes |
| `api_key` | a token or key | provider, scopes, account, expires |
| `wifi` | a Wi-Fi passphrase | network name (SSID), security type; shown as a QR code to join |
| `recovery` | a list of one-time codes | for which account; how many remain |
| `ssh_key` | a private key | public key (readable), host, user |
| `note` | free text | |
| `file` | a file (a certificate, a scanned document) | original name, type, size |
| `pin` | a short code (an alarm, a safe) | where it is used |

# The fields every secret has

| field | means |
|---|---|
| `title` | what people call it ("POS back office admin") |
| `envelope` | the content address of the sealed value (`sha256:…`) |
| `policy` | the [policy](/concepts/quorum.md) that governs it (open, guarded, split) |
| `added_by` | the member and device that added it |
| `rotated` | when the value last changed |
| `expires` | when it stops working, if known |
| `tags` | ordinary Void Core tags (`store:downtown`, `vendor:cloudflare`) |

**History of the value.** Rotating a secret writes a new envelope and points the
rune at it. The old envelope stays in the vault as history for a set time (so a
rotation that breaks something can be undone) and is then dropped. How long is a
policy setting.

# Providers and compatibility (from Void Hormiga)

Hormiga's Key node declares **what a key is compatible with**: a Cloudflare key
serves websites and email, a Google Maps key serves maps
(`../VoidHormiga/okf/concepts/platform/antfarm/v2/keys.md` §3). The author:
*"different API keys define themselves by the documents they are compatible
with."*

Verguenza keeps the provider declaration as data, not code: a small table of
providers, each with a name, a sign-up link, the scopes it can carry, and a list
of **uses** (`website`, `email`, `maps`, `object-storage`…). Verguenza does not
interpret uses; it carries them so a [client](/concepts/clients.md) like Hormiga
can ask *"which secrets in this vault can serve a map?"* and get an answer.

# Expiry, usage and cost

Hormiga's design wanted monitor nodes for keys: usage, budget, expiry. In
Verguenza these belong to the secret:

- **Expiry** is a field, and the application warns ahead of it (14 days by
  default).
- **Usage** is read from the [audit](/concepts/audit.md) record: every time a
  client used the secret, with which device and for what, is already a logged
  command. *"Who used the Google Maps key this month"* is a query, offline.
- **Cost** is a vendor's number and arrives only through a vendor's usage API.
  Verguenza makes no network calls to vendors by itself; a client that can (Hormiga)
  may write a `spend` measure onto the secret.

# A vault, as Void Core mantles

| mantle | holds |
|---|---|
| `secrets` | one rune per secret (glyph per kind) |
| `members` | one rune per person: display name, role, joined |
| `devices` | one rune per device: owner, public keys, enrolled by, last seen |
| `policies` | named policies ([quorum](/concepts/quorum.md)) |
| `clients` | other applications allowed to ask, and their grants ([clients](/concepts/clients.md)) |
| `requests` | pending and finished requests for approval, with their approvals (an `act` rune each) |

Envelopes are not runes. They are content-addressed blobs beside the state
document, the way Void Palabra already carries files a document names but does
not hold (`VoidPalabra:/concepts/content-reference.md`).
