---
type: Concept
title: Trust — authority without a trusted merge
description: "Where a vault's authority lives, and whose mechanism carries it. Not in the merged document (which any member can edit, and from which removes cannot be attributed) but in a SIGNED RECORD: content-addressed, signed objects that name their causal parents, judged by their own causal past the same way on every device. The mechanism is Void Palabra's (utterances, the history graph, content references, and the capabilities it plans); the rules are Verguenza's: judge by causal past, removals carry a cutoff, removals all take effect, authority flows only through valid objects. The Core document is a projection of the record. Amended 2026-10-01 from a Verguenza-owned 'ledger' to Palabra's mechanism, at the author's direction."
tags: [status:planned, audience:dev, confidence:asserted]
timestamp: 2026-10-01T00:00:00Z
---

# Why authority cannot live in the merge

The founding [sharing](/concepts/sharing.md) page checked each change against the
`devices` mantle as it stood. Void Palabra's own trust findings
(`VoidPalabra:/design/open-questions.md` §6.1) show three reasons that cannot work:

1. **Sync ships whole merged documents**, so a signature proves who relayed, not who
   wrote (finding 1).
2. **A remove in a merged document cannot be attributed.** A member could delete the
   record of their own removal (finding 1).
3. **Checking a change against state that is still merging diverges** between
   devices that check before and after a concurrent role change (findings 3 and 5).

So authority travels as **signed objects**, and the merged document carries only
what opens nothing.

# The mechanism is Palabra's

Every part of the mechanism already exists in Palabra, or is planned there
([boundary](/concepts/boundary.md)):

| need | Palabra |
|---|---|
| a change named by the hash of its bytes, naming its causes | the **utterance**: content-addressed, `parents` by hash, `who` |
| a partial order that is its own clock | the **history graph**; concurrency kept as concurrency |
| a change whose causes have not arrived is held, not judged | the graph's rule: *"an utterance whose parents are not all present is not admitted"*, and it is handed back, not discarded |
| bytes carried by address and checked on arrival | **content references** (`content_matches`) |
| how much history a device keeps | **tiers** |
| a signature on what travels; capabilities over a region | **planned** (§6: signed deltas, Meadowcap-shaped capabilities) |

Verguenza's signed objects (roster entries, capabilities, envelopes, requests,
approvals, acts; [formats](/concepts/formats.md)) are exactly *signed utterances
about a vault's people and secrets*. Until Palabra signs utterances, Verguenza's
library writes them in its own frame, shaped to move into Palabra's
([Q5](/developer_questions.md)).

**Authority needs the Full tier, for the record only.** Judging an object needs its
causal past. So every device keeps the vault's *signed record* at Palabra's **Full**
tier (it is small: people, devices, grants, approvals), whatever tier it keeps for
everything else. A phone may forget last month's description edits; it may not
forget who admitted whom.

**The record only grows.** Objects are never removed by sync (a device may
*withhold* objects, not make another device forget one). Palabra's merge removes
things on purpose; the signed record is the one place in a vault where removal is
not a thing sync does, and a removal is a new signed object instead.

# Verguenza's rules

These are the "who may" layer Palabra leaves to applications. Each is a pure
function of a set of objects, so every device holding the same set computes the
same answer.

**Rule 1: judge by causal past.** An object names its parents. The record replayed
up to those parents says whether its author's device held, at that point, a
capability for this action ([authority](/concepts/authority.md)), with the consent
its rule demands. Fixed by hash, so the verdict never changes with arrival order.
That is Palabra's finding 3 answered: the region is written in the object, and its
ancestry is immutable.

**Rule 2: a removal carries a cutoff.** Rule 1 alone lets a removed device backdate
(name old parents, from before its removal). So an object that removes a device or a
person, or recalls a capability, names **the last objects of theirs it accepts**.
Once a device holds that removal, anything by the removed party outside the
cutoff's causal past is **void**: kept, shown as *"written after removal"*, given no
authority. The verdict depends on which removals a device has seen, so devices can
disagree *while* a removal travels, and agree once they hold the same set.

**Rule 3: removals all take effect.** Two removals of each other, made concurrently,
both stand. Removals are judged by rule 1 alone, before rule 2 voids anything.
Losing power is the safe direction ([authority](/concepts/authority.md): *recall is
easier than grant*). p2panda's group management reached the same rule independently
(*strong removal*, [governance](/references/governance.md)). If concurrent removals
leave no one able to act, the vault falls to [recovery](/concepts/recovery.md).
[Q10](/developer_questions.md).

**Rule 4: authority flows only through valid objects.** Void and unauthorized
objects are skipped when the record is replayed. An honest object that names a void
one as a parent is judged on the rest of its causal past. A revocation **drops** the
revoked party's late work; it does not poison honest work built beside it. That is
Palabra's finding 5 answered for a small, rarely changing record. Not offered for
ordinary data, where it would be too strict.

**What this costs.** Work a device did offline, which the remover had not seen when
it removed that device, is dropped even if it was honest. Its person redoes it from
another device.

# What each thing's current value is

Because authority is in the record, **"the current value of secret *s*" is computed
from it**: the highest version among valid envelopes for *s*. Editing a rune to
point at an older envelope rolls nothing back. Two valid envelopes with the same
version are a Palabra conflict, shown to people, settled by writing a new version.

**A policy change is a signed object.** An envelope whose policy differs from its
secret's policy at the envelope's parents is invalid. So nobody weakens a guarded
secret by writing a new envelope marked *open*: weakening needs a signed change
carrying the consent the old rule asks for.

# The Core document is a projection

Ground rule 5 stands: the model lives in Void Core and every change goes through the
dispatcher ([commands](/concepts/commands.md)). The mantles that show people,
devices, groups, grants and requests are **rebuilt by each device from the valid
objects it holds**, through ordinary Core commands. They exist so Void Core and Void
Maiz can show, search and draw them; nothing is decided from them.

What stays ordinary mergeable data in Palabra's document merge: a secret's title,
notes, tags, fields and provider uses (description, which opens nothing), view state
and the graph's layout. A description deleted through the merge loses a description,
not a secret; the application lists *"secrets with no description"*.

# What this is not

- **Not a blockchain.** No consensus, no global order: a partial order of signed
  objects, with rules each device evaluates alone.
- **Not protection from a member who holds a seal.** A guarded secret is still
  openable by a modified client on a device it is sealed to. The record makes sure
  that device cannot *grant*, *weaken* or *forge*.
- **Not invented here.** Keybase's signature chains, Matrix's authorization rules
  (each event judged against the state at its own place in the graph), p2panda's
  strong removal and Keyhive's convergent capabilities are the precedents
  ([prior art](/references/prior-art.md), [governance](/references/governance.md)).
