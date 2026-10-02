---
type: Concept
title: Access — to mantles, to runes, and where rules fit
description: "The author, 2026-10-01: Verguenza is 'about managing access to mantles, and access to the runes within a mantle as well (which would involve rules)', and 'all about access of information in a graph based world, where we have no hierarchy, but there still must be some sort of control in what is given to someone.' The general structure, beyond passwords: three kinds of access (read, enforced by sealing; write, by capability at admission; administer, by consent), regions written in names Void Core already has (vault, mantle, glyph, rune), and the line rules must respect: rules PROPOSE who should see what, capabilities ADMIT writes, seals DECIDE reads. Why a rule over tags may choose an audience but never admit a write (Palabra's finding 3), what happens when a rule's answer changes, and the costs of encrypting a rune's content inside a merging document."
tags: [status:planned, audience:all, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# The general problem

A password is a rune whose content should reach some devices and not others. So is a
salary in Hormiga's member records, a player's hand in a card game, a medical note, a
draft nobody else should see yet. The author's framing makes the generality explicit:

> *"this is about managing access to mantles, and access to the runes within a mantle
> as well (which would involve rules)."*
>
> *"verguenza is all about access of information in a graph based world. where we have
> no heiarchy, but there still must be some sort of control in what is given to
> someone. There still has to be 'shame'."* — the author, 2026-10-01

**Shame, as a structure**: the boundary of what is given to whom. Without hierarchy
there is no one at the top who sees everything, so the boundary has to be a property
of each piece of information, held by the people it was given to. That is what a seal
is.

# Three kinds of access, three enforcers

| access | means | enforced by | strength |
|---|---|---|---|
| **read** | can see a rune's content | **sealing**: the content is encrypted and sealed only to the devices that may read it ([sealing](/concepts/sealing.md)) | mathematics. A device without a seal holds bytes it cannot read, whatever software it runs |
| **write** | can change a rune, add one, remove one | **capability at admission**: every device judges a signed change against the writer's capabilities at the change's causal past ([trust](/concepts/trust.md)), and refuses what is outside them | every honest device refuses; a modified client can write locally but cannot make anyone else accept it |
| **administer** | can grant, recall, change who reads and writes | **consent**: the change carries the approvals its formula asks for ([authority](/concepts/authority.md)) | the same as write, plus the approvals |

Read access is the only one mathematics can enforce on its own, which is why a
password manager is the place this structure starts: for secrets, *read* is the whole
point.

# Regions are names Void Core already has

A capability's region must be **decidable from the change alone, never from state**
(Palabra's finding 3, `VoidPalabra:/design/open-questions.md` §6.1): a region that
depends on tags still merging gives two devices two verdicts that never reconcile. So
regions are written in the names a change carries:

| region | written as | example |
|---|---|---|
| a vault | its id | *everything in the store's vault* |
| a **mantle** | its name | *the `payroll` mantle* |
| a **glyph** within a mantle (or anywhere) | the glyph name | *every `wifi` rune* |
| a **rune** | its id | *this one alarm code* |
| a group of people (for administer) | the group id | *the front desk's membership* |

These nest (vault ⊃ mantle ⊃ glyph-in-mantle ⊃ rune), so a capability restricted from
a mantle to a glyph to a rune only ever narrows, which is Meadowcap's *restrict*
([governance](/references/governance.md)). One need goes upstream: a Core journal entry
does not say which mantle a command changed (Palabra's finding 4), and admission by
mantle needs it. Palabra already named this as an ask for Void Core.

# Where rules fit: they propose, capabilities admit, seals decide

The author expects rules in rune-level access, and rules are how a group would want to
say it: *"anything tagged `payroll` is for the bookkeeping group"*, *"a `wifi` rune is
for everyone in the building"*. But a rule over tags is exactly the state-dependent
region finding 3 forbids for admission. The two fit together once each decision goes
to the enforcer that can carry it:

1. **A rule derives an audience.** In Allomone's derive-only style, a rule produces an
   annotation `(rune, audience) → group`, recomputed from the current tags, never
   stored as truth ([standing](/concepts/standing.md) has the same discipline).
2. **The writing device acts on it at the moment of writing.** When a rune's content is
   written (or rewritten), the writer's device evaluates the rules it holds, and seals
   the content to the devices of the derived audience. **The result is a set of seals:
   a fact, signed, identical for everyone from then on.** No device has to agree with
   another about tags to agree about who can read.
3. **Write admission never looks at a rule.** Who may change a rune is decided by
   capabilities over the names above, only.

So: **rules propose who should see what; seals decide who can; capabilities decide who
may change it.** The disagreement a rule can cause between devices is about a
*proposal*, which is harmless; the facts that matter are never computed from it.

## When a rule's answer changes

Tags change, so a rule's audience for a rune can change after its content was sealed:

| change | what Verguenza does |
|---|---|
| the audience **grows** (someone new should see it) | a device that can open the rune is asked to **re-seal** it to the new devices, with its person's approval or under a standing policy ([recovery](/concepts/recovery.md) uses the same step). Knowledge grows |
| the audience **shrinks** (someone should no longer see it) | they keep what they already saw (knowledge only grows, [authority](/concepts/authority.md)). The next version of the rune is sealed without them, and the application **proposes** rotation where the content is a credential. Never automatic, never silent |

This is Allomone's Weaver at autonomy level 1: *propose, never write*. Re-sealing is a
signed act a person (or a standing policy they chose) performs.

# The costs of sealing inside a merging document

A sealed rune is still a rune in a Palabra document, and that has prices worth
stating before anyone builds on it:

- **Merges become coarse.** Palabra merges a rune's content field by field. A sealed
  content is one opaque value (an envelope's address), so two concurrent edits to
  different fields of a sealed rune become a **whole-value conflict** shown to people,
  instead of merging. The finer the sealing (one envelope per field), the finer the
  merge, and the more seals.
- **Rules cannot read what they cannot open.** A rule evaluated on a device that cannot
  open a rune sees only its unsealed parts (glyph, tags, links). So rules that decide
  audiences should key on unsealed facts, and a group's rules mean slightly different
  things on devices with different knowledge, which is correct for proposals
  (Allomone: peers may legitimately derive differently).
- **What stays visible.** A sealed rune's existence, glyph, tags and links stay readable
  by the whole vault unless the rune is marked `hidden` ([secret](/concepts/secret.md)),
  which seals its description too and costs search and the graph.
- **Search, sort and views** see only what this device can open. A view of a mantle
  is therefore per device, and Void Maiz's views must render "here is something you
  cannot open" honestly rather than as absence.

# What this makes Verguenza

A password manager is the first application of a general structure: **access to
information in a graph, with no one at the top**. The structure is the
[authority](/concepts/authority.md) model (who may), [standing](/concepts/standing.md)
(how it looks), this page (what it governs), [trust](/concepts/trust.md) (how it is
checked), and [sealing](/concepts/sealing.md) (what makes reading real). Who else
uses it is in [products](/concepts/products.md).
