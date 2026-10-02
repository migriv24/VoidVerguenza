---
type: Concept
title: Products — a foundation and an application, and who else uses them
description: "The author, 2026-10-01: consider making Void Verguenza 'not actually come from void maiz', and work on two kinds of application: a visual one that is useful on its own (the password manager, a direct use Palabra never had) and one that carries the libraries other applications need. The two products and the line between them, the foundation's layers (the access model, the record's rules, sealing, standing) on Void Palabra and without Void Maiz, the application as its first client, and what each sibling gains: Hormiga's members and keys, Chisme's admin actions and device logins, Unity's fog of war, Maiz's honest views and Allomone's private user graph, Palabra's capabilities, Core's glyph- and mantle-scoped grants."
tags: [status:planned, audience:all, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# Two products

> *"we may want to consider making Void Verguenza not actually come from void maiz.
> and we work on 2 types of application. one where we have a 'visual version' where we
> can actually USE this application on its own, and another that will have the
> neccesary libraries and code to maybe be more useful to other applications that want
> to use this. … this will be directly useful (password manager application). Palabra
> didn't have a direct use for a visual application. … however, i do think we should
> focus on this general fundemental structure."* — the author, 2026-10-01

| | **the foundation** (`voidverguenza`) | **the application** (Void Verguenza) |
|---|---|---|
| is | the general structure: access to information in a graph, with no one at the top ([access](/concepts/access.md)) | a password manager for a group, usable by itself |
| built on | Void Palabra (and its Reticulum companion). **Not Void Maiz, not Void Core** | the foundation, Void Core, and **Void Maiz** (decided 2026-10-01, [Q15](/developer_questions.md)) |
| used by | the application, Hormiga, Chisme, any Void application | people |
| is to Verguenza what | Palabra's library is to Palabra | what Palabra never had: a direct use |

**The foundation comes first, and the application keeps it honest.** Palabra was
designed without a product of its own and found its shape through clients (Hormiga's
save system, a LAN chat). Verguenza has its own client built in: every layer of the
foundation must make the password manager work, which is a test no abstraction can
skip. The author's ordering is kept: the general structure is the focus, and the
password manager is how we know it is right.

# The foundation's layers

| layer | is | depends on |
|---|---|---|
| **access model** | groups in groups, capabilities over regions (vault, mantle, glyph, rune), consent formulas, mandates and recall, templates and labels ([authority](/concepts/authority.md), [access](/concepts/access.md)) | nothing: pure functions over a set of objects |
| **the record's rules** | judging by causal past, cutoffs, removals win, authority only through valid objects ([trust](/concepts/trust.md)) | the access model; a `Signer`/`Verifier` interface |
| **sealing** | envelopes, seals, re-sealing, audiences proposed by rules ([sealing](/concepts/sealing.md), [access](/concepts/access.md)) | Palabra's Reticulum operations |
| **standing** | knowledge, authority and consent as derived weights; lenses; thresholds; diagnostics ([standing](/concepts/standing.md)) | the access model and the seals |
| **consent requests** | requests, approvals, presence ([quorum](/concepts/quorum.md)) | the record |
| **recovery** | re-admission and re-sealing; cold holders ([recovery](/concepts/recovery.md)) | all of the above |

None of it knows what a password is. A **secret** ([secret](/concepts/secret.md)) is
the application's glyph: a rune whose content the foundation seals.

**Why not Void Maiz.** Maiz is the family's view layer; the foundation has nothing to
show. Linking it would drag a view library into Hormiga's and Chisme's access checks.
**Why not Void Core either.** Palabra already holds the line that a library about
change does not link Core (`VoidPalabra:include/voidpalabra/utterance.hpp`); the
foundation speaks of mantles, glyphs and runes **by name**, which is all a region
needs. An application maps those names to its own Core document.

# The application

Void Verguenza, the password manager: the [screens](/concepts/application.md), the
[commands](/concepts/commands.md), intake, the clipboard, auto-lock, the
[clients](/concepts/clients.md) agent. Its secret kinds are the foundation's first
glyphs. Its screens use **Void Maiz** (the author, 2026-10-01: *"yes, the visual should use
maiz"*): the family's view layer, and the home of the canvas the standing graph
needs. Maiz is the application's, never the foundation's.

# Who else gains, and what

| sibling | today | with the foundation |
|---|---|---|
| **Void Hormiga** | a members registry with admin roles; the Antfarm's Key node holding values in its own vault; LAN sharing between members | members, groups and roles become the access model (labels computed, not stored); keys become Verguenza secrets its effects *ask* for ([clients](/concepts/clients.md)); per-mantle and per-rune access for an organization's records (a member's salary, a donor's address), sealed. The author: *"Hormiga will eventually adopt Verguenza to help it manage this exact thing."* |
| **Void Chisme** | network administration: devices as runes, *admin actions as logged, rehearsable commands* | the **device logins** it needs (the camera recorder's admin password is in Verguenza's founding pitch) are Verguenza secrets it asks for; **who may run an admin action** is a capability with a consent formula (*"restarting the core switch needs IT, or two managers present"*) |
| **Void Unity** | multiplayer worlds; numbers derived, never stored | the knowledge relation is **fog of war**: what each player has seen only grows; hidden information (a hand of cards, a secret objective) is a sealed rune; who may command which units is a capability; alliance decisions are consent formulas. Standing's weights follow Unity's own no-independent-floats law, which is where they learned it |
| **Void Maiz** and **Void Allomone** | views of a document; a user graph whose shared form Allomone held back for privacy (its Q20) | views that render *"here is something you cannot open"* honestly ([access](/concepts/access.md)); and an answer to Q20's cost: a shared attention graph **sealed to the group it describes**, so sharing it within a team does not broadcast it to the whole document |
| **Void Palabra** | trust planned as Meadowcap-shaped capabilities, waiting for a client | a client with a worked design: the record's rules, and the access model on top ([boundary](/concepts/boundary.md)) |
| **Void Core** | glyphs, mantles, runes, a journal; no notion of who | capabilities scoped to Core's own names; one ask (the journal saying which mantle a command changed, Palabra's finding 4) |

# How Claude can help the family with this (the author asked)

The author also asked how other Void applications might benefit from what this session
can do. Within the family's own rules (OKF first; upstream messages unless an edit is
granted), the useful things are:

- **Read across siblings and find the seams**, as these sessions did: Reticulum's
  unseeded generator, Palabra's trust findings, Core's values-on-edges, Unity's
  no-floats law, Allomone's sync boundary all turned up by reading the code and OKFs
  side by side, not by assuming.
- **Write the general structure once, in the foundation, with tests that are the OKF's
  sentences**, so each sibling adopts a checked library instead of re-deriving it
  (Palabra's founding complaint was every application re-solving storage alone).
- **Draft the adoption for each sibling** as a message: what changes in Hormiga's
  members page, Chisme's admin actions, Unity's multiplayer, in each one's own
  vocabulary.
- **Keep the honesty conventions checked**: labels that say what enforces them,
  `status:current` only with code behind it, measured versus estimated said apart.
