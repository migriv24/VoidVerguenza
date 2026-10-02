---
type: Concept
title: Clients — how other applications ask for a secret
description: "Void Hormiga's Antfarm v2 Key node holds a credential's description and keeps its value in Hormiga's own vault. The author: 'eventually the antfarm will reference Verguenza's structures to use instead of our current api key node.' A client is an application enrolled in a vault with a grant (which secrets, for which uses, under which policy). It asks for a value at the moment an effect needs it, with the effect's consequence sentence, and wipes it after. The agent that answers, how it is reached, what Hormiga's Key node becomes, and the order to build it in."
tags: [status:planned, audience:dev, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# What exists in Hormiga today

Void Hormiga's Antfarm v2 has a **Key** node
(`../VoidHormiga/okf/concepts/platform/antfarm/v2/keys.md`):

- the **rune** (provider, what it serves, scopes, expiry, who added it and from
  which device) syncs with the Antfarm;
- the **value** lives sealed in Hormiga's vault on each device (Argon2id +
  XChaCha20-Poly1305, `src/platform/vault.cpp`), entered through a password box
  or `farm key set` on standard input;
- a domain, bucket or image host node **uses** a key through a wire, and when it
  publishes, Hormiga reads the value from its vault, uses it, and drops it.

Hormiga's own open question Q88 asks where at-rest encryption should live, and
its lean names what would change it: *"a second application wanting the same
vault format."* Verguenza is that application.

# A client

A **client** is another application enrolled in a vault, recorded in the
`clients` mantle:

| field | example |
|---|---|
| `app` | `void-hormiga` |
| `device` | the device it runs on (a client is per device, like everything that holds keys) |
| `identity` | the client's own Reticulum identity (Ed25519 to sign its asks, X25519 to receive values), made when it enrolled |
| `grant` | which secrets (by name, by tag, or by provider use: *"keys that serve `website`"*), and which operation |
| `policy` | what each ask requires: nothing (open), the device's person to confirm, or a quorum |

**Two operations, never one:**

| operation | the client gets | example |
|---|---|---|
| `use` | the value, for one stated purpose, to be wiped after | Hormiga publishing a site with a GitHub token |
| `describe` | the description only | Hormiga listing which keys could serve a map |

`reveal` (show a value to a person) is not a client operation. A person reveals
in Verguenza itself.

# Asking

```
Hormiga                          Verguenza agent (same device)
  │  use github-main               │
  │  purpose: deploy-site          │
  │  consequence: "PUBLISHES THE   │
  │   WEBSITE…"                    │
  ├──────────────────────────────►│  check the grant
  │                                │  check the policy
  │                                │   ├ open: answer now
  │                                │   ├ confirm: ask this device's person
  │                                │   └ quorum: send a request to approvers,
  │                                │     with Hormiga's consequence sentence
  │                                │  write the `use` act to the signed record
  │◄──────────────────────────────┤  the value, once, sealed to the
  │  open it, use it, wipe it      │   client's own X25519 key
```

**The value is sealed to the client, and the act comes first** (amended
2026-10-01). The agent seals the value to the client's own Reticulum identity
(`Identity.encrypt`) before it leaves, whatever path it takes: a Reticulum link is a transport and
never the only protection on a value ([quorum](/concepts/quorum.md) §"How a split
secret opens" gives the reason). And the `use` act is written before the value is
sent; a use that cannot be recorded is not answered
([commands](/concepts/commands.md) §"Reveal").

**The consequence sentence travels with the ask.** Hormiga's effects already
carry one (*"PUBLISHES THE WEBSITE. Whatever is in site/ becomes the live page
everyone sees, immediately…"*). Verguenza shows it to whoever must approve, so a
manager approving on their own device knows exactly what the key is about to do.
That is the strongest reason for the design: **the approval and the action are
about the same sentence.**

# The agent, and how it is reached

The **Verguenza agent** is the part of Verguenza that answers clients. It runs
while a vault is unlocked on that device (inside the desktop application, or as
a small background process once there is a reason for one).

**How a client reaches it: lean, a Reticulum link to the agent's local
destination.** Every Void application already runs a Reticulum node
(`VoidPalabra:/concepts/reticulum.md`), links are encrypted and authenticated,
and the same path then works when the client is on a *different* device than the
vault (Hormiga on a laptop asking the back-office computer's Verguenza). The
alternative is an operating-system pipe, which is simpler and local only.
[Q7](/developer_questions.md).

**A library for the parts that are not the agent.** The envelope format, the
signature checks and the client protocol live in a small library,
`voidverguenza`, with no user interface and no Void Maiz dependency, so Hormiga
can link the *client* half without linking the application. That is row 2 of
Hormiga's application boundaries (`../VoidHormiga/okf/concepts/foundation/application-boundaries.md`):
a capability package, like Void Palabra.

# What Hormiga's Key node becomes

| today (Hormiga v2) | with Verguenza |
|---|---|
| `provider`, `expires`, `added_by` fields on the Key rune | read from the Verguenza secret's description; the Key rune keeps only `verguenza: <vault>/<secret>` |
| the value in Hormiga's vault | in Verguenza's envelope; Hormiga holds nothing |
| `farm key set` | *"add or change it in Verguenza"*; the Key node shows the secret's state |
| readiness *"no value on this device"* | *"Verguenza is locked"*, *"no grant for this key"*, *"needs 2 approvals"* |
| Hormiga's key monitors (usage, expiry) | Verguenza's [audit](/concepts/audit.md) and expiry, read through `describe` |
| sharing keys between members (Hormiga's V3) | Verguenza's sharing, already end-to-end and signed |

**Both work, for a while.** A Hormiga Key node can name a value in Hormiga's
vault (as today) or a Verguenza secret. Hormiga's migration to Verguenza is a
command that moves values over and leaves references, and it is Hormiga's to
design and run.

# The order

1. The `voidverguenza` library's envelope and the vault on one device
   (Verguenza [roadmap](/roadmap.md) phase 1).
2. The agent and the client protocol, with `describe` and `use`, open policy only.
3. Hormiga: a Key node that names a Verguenza secret.
4. Quorum on client asks.
