# Batch 310 — three landings, a new allocation lever, and four doc sections that wrote off reachable work

Eight agents, 40 targets, assigned by **technique and branch density** rather than size. Gate green at
`5c4695205413df7db52b9a184815a07783999971` at every commit.

**3 landed, 19 parks installed or improved, 1 agent declined.**
State: **4,788 of 5,710 (83.9%)** — 52 available, 779 parked.

## Landed

| function | from | how |
|---|---|---|
| `ActorCmd_Wander` | 2 of 187 (barriered) | split + `CSE_CFLAGS` + a source order — **pin-free** |
| `OvlFunc_881_20081c4` | 12 of 63 | `ALIAS_CFLAGS`, **no source change** |
| `OvlFunc_889_2008074` | 82 differing | a symbol named as a local — 82 → 4 in one edit |

## The best result: a new allocation lever that moved two functions at once

The pair hypothesis **held**. One change, written once and applied verbatim to both halves of a named
two-function blocker:

    int clen = 0x80 << 7;      /* initialised AT its declaration -- that is the lever */

`Anim_Djinni` **26 → 16 of 738** and `Anim_CriticalHit` **19 → 15 of 707**, both size- and count-exact,
and **both relocation tables went from two rows off to exact**.

**The mechanism is the inverse of everything else we use.** Initialising at the declaration makes the
pseudo **live from function entry**; since `allocno_compare` divides by `LIVE_LENGTH`, that is the
*longest* range and therefore the *lowest* priority. It is allocated last, loses its hard register, and
because its `REG_EQUIV` is a constant, reload **rematerialises** it at each reference. That is exactly
the ROM's form.

**The park had the reading right and the handle wrong** — it said "the pseudo fails to get a hard
register and reload rematerialises it", which is correct. What was missing is that **the source handle
is the initialiser position, not the constant's spelling.** Four recorded spellings were inert because
all were short-range forms, and **a body assignment *raises* priority**: `clen = 0x80 << 7;` in the body
is inert at 26, the declaration initialiser gives 16.

So the allocation family now has **three** members: reuse **raises your own** priority, region-splitting
**lowers a competitor's**, and declaration-initialising **lowers your own** to force rematerialisation.
And the third does not substitute for the second — measured on the other pair, whose victim is already
whole-function-lived, it does not reach them.

## Four doc sections that would have written off reachable work

This is the batch's structural finding, and it is about the document rather than the compiler.

1. **`.call_via` is a structural blocker** — retracted elsewhere in the same file and **never struck**.
   It cost work twice in one day: a batch-302 agent declined a figure on its authority, and I screened
   28 targets against it and ordered three viable functions abandoned.
2. **`cmp #K / bge` is unreachable** — false; 21 surviving sites in compiler-generated output.
3. **The switch `case 0` shape transfers to 36 functions** — mine, and measured away by a tree-wide
   screen: only one function carries the shape that hides a case.
4. **The Thumb backend never emits register-offset addressing** — true of `agbcc`, false of the gcc296
   we build with: **1,156 sites across 401 generated `.s` files**, and the mode is the *target* of a
   recorded lever.

Each had an unqualified heading. **A section that tells you to exclude functions must carry its scope
in its heading**, because the heading is what a reader greps and acts on. All four are now struck or
qualified in place.

## The 800+ band model was rebuilt by two agents independently

- **Branch density cannot separate the tail.** Five functions at 3–4 branches split into *both*
  populations. The separator is the reference's **wide-constant reuse fraction** — a gap, not a
  gradient: two at 0.0% against three at 20–24%. Triage with
  `grep -coE '\b(r8|r9|r10|r11|sl|fp)\b'` on the reference, or equivalently grep for the high-save
  prologue; **a reference without one is the hard case.**
- **My branch-count triage was noisy**, and both agents caught it: **14 of 18 branch targets are pool
  skips**, only 4 are real tests. Register state carries across a pool skip — misreading two cost four
  wrong argument fills.
- **The pin IS the source shape that denies cse1 the commoning** — the band doc's open question,
  answered by controlled experiment: the byte-exact function against a **mechanically pin-stripped copy
  of the same program** gives 0 against 44 high-register mentions.
- **The band doc's test points at the wrong dump and inverts.** The clean signal is `18.greg`, not
  `03.cse`.
- **The smallest reproduction is four instructions** — one reference builds `-1` three times inside a
  single argument setup, two apart, un-commoned. That rules out every distance-, call-boundary- and
  block-extent-based explanation, leaving the cost model or the RTL shape.

## A frame word can own 41% of a residue, and the gate is REG_N_SETS

`Anim_Ramses`' entire aligned shortfall is **one frame word**. `local-alloc.c`'s `update_equiv_regs`
gives a **one-set** constant pseudo a `REG_EQUIV`, so `reload1.c` never allocates a slot — ours is
`0x50` where the ROM's is `0x54`, and that word shifts **every `[sp,#N]` in 818 instructions.**

**The gate is the SET count, not the reference count** — this document's existing treatment is entirely
about the `REG_N_REFS == 2` clause. The doubled-use probe produced **the best size-and-count pair in the
table with the frame still wrong**, and the aligned figure fell.

## Figures that lie — now a five-rung ladder, each checked by something structural

1. a closer **size** can be a wrong program;
2. a lower **objcmp count** can be the worse candidate;
3. **both axes exact** can be our own work totalling the ROM's deficit — check the **frame and
   prologue**;
4. an **exact count** can be two cancelling defects — check the **relocation sequence**;
5. **the best size-and-count pair in a probe table** can leave the frame wrong — carry **frame size as
   a column**.

The sharpest instance: on `Anim_Unsummon`, moving one definition later made **both axes exact** (432 of
432, objcmp 381) — the most persuasive-looking move in this method — and was **rejected** on three
structural checks. The four "recovered" instructions were our own work in our own places that happened
to total the ROM's deficit.

## Levers and bounds

- **A pointer-typed carrier, not an `int` one**, for a pinned `gBuffer`-derived pointer — the twins
  14 → 13.
- **A run of consecutive IDs is a named base plus literal offsets** (+56/+6 → +4/+3), and it is the
  **converse** of batch 307's reading. Discriminator: far fewer pooled constants in the reference than
  uses means a base; the same few repeated means commoning.
- **A two-step computed constant, used in reverse** — the doc uses that mechanism to *avoid* holding a
  value; here the ROM holds it, so write it computed.
- **`bl` to a local label is a long BRANCH, not a call** — one function opens with `bl` to a label 1,520
  instructions away, because Thumb-1's `b` is ±2KB and the function is 4KB.
- **The HImode carrier split is per SITE** — 5 of 5 non-zero halfword stores want it, only 1 of 3 zero
  stores does. Fourth independent bound on that family.
- **The read-once-global lever's discriminator is WHAT crosses the call** — the pointer's *value* forces
  a local; its *address* is a constant cse commons free.
- **The reuse precondition is "the SET is settled", not "count already exact".**
- **The frame procedure is three greps**: `sub sp, #imm`, **`mov rX, sp` + `add rX, #K`**, and
  **`add rX, sp` + a LOAD** to tell a spill from argument staging. Two phantom holes and one lost array
  came from running only the first.
- **The veneer was irrelevant on all five targets of one brief** — so the 35-function queue is real as
  *unblocked targets*, not near-landings. The veneer removes a structural obstacle; it does not shorten
  an existing residue.

## Process

- **An agent declined brief F** on a copyright reading, as one did in batch 307. Before stopping it
  produced the finding that **none of its five targets has a single spill slot** — every `str [sp,#K]`
  is outgoing-argument staging — which bounds the spill-slot lever I had put in all eight briefs.
- **An agent disproved its own suggestion before shipping it**: it proposed `-fno-gcse` as a diagnostic,
  ran it, and found it deletes a PRE the ROM has too.
- **An overlay path hazard nearly cost a park**: a recipe named a path already holding a *different*
  function's park, same address, different overlay. Derive an overlay park's directory from the
  reference's bank, never the address.
- **`OvlFunc_889_2008074` landed with 233 pins and the set was NOT minimised to a fixpoint.**
  Minimisation is owed on that file; the tree's standard asks for it and the last comparable landing did
  it at 19.
- Two more claim lines disagreed with their bodies (1058 vs 320; 740 vs 739) — fifth and sixth
  occurrences.

## Open

- **52 available, all 800+.** Triage by the reference's high-register use, not branch count.
- `Anim_CriticalHit` 15 of 707 and `Anim_Djinni` 16 of 738 are the closest parks, both size- and
  count-exact with exact relocations.
- `Func_80f7f78` is named as the best next reconstruction: smallest frame, five calls, zero veneers, no
  split export.
- The cse1 commoning question is down to the cost model or the RTL shape, reproducible in four
  instructions.
