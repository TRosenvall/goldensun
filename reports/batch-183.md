# Batch 183

Seven elevated, four parked, one selection tool rewritten twice, and two
long-standing assumptions in this notebook corrected — one about the compiler,
one about where the compiler's own source lives.

The round began with the candidate pipeline **empty** — `tools/filtered.py`
returned zero, and a relaxed sweep at ≥4 calls also returned zero, which made
the remaining 1,278 unparked functions look out of reach. They were not. The
largest single rejection reason across the remainder was the
duplicate-expensive-constant rule, and batch 182 had just solved half of it
without anyone noticing that the filter still rejected the shape.

## Function breakdown

| # | function | address | file | how it was found | what it took |
|---|---|---|---|---|---|
| 1 | `OvlFunc_901_20088a8` | `0x020088a8` | [ovl_314_…_a_a_c.c](src/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a_a_c.c) | own screen | `CSE_CFLAGS`; the stored zero is the flag reassigned; **a callee's return type** |
| 2 | `OvlFunc_898_20084a0` | `0x020084a0` | [ovl_314_c_c_a_c_a_a_a.c](src/overlays/rom_793768/ovl_314_c_c_a_c_a_a_a.c) | new `[join]` candidate | a dominating-block local, three times — **beat the flag** |
| 3 | `FieldMove_Target` | `0x08096960` | [rom_944ec_…_a_b.c](src/rom_8a000/rom_944ec_a_c_c_a_a_a_a_b.c) | new `[join]` candidate | the join-split's **switch-arm form**, one local per arm |
| 4 | `OvlFunc_920_2008304` | `0x02008304` | [ovl_30_c_a_c_c_a_c_c_c.c](src/overlays/rom_7a6ae4/ovl_30_c_a_c_c_a_c_c_c.c) | new `[join]` candidate | `CSE_CFLAGS` — the *other* shape the marker conflates |
| 5 | `OvlFunc_932_200a934` | `0x0200a934` | [ovl_30_…_a_c_b.c](src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_c_a_c_b.c) | new `[cse]` candidate | first candidate, no flag — the marker was wrong |
| 6 | `OvlFunc_933_2008cd0` | `0x02008cd0` | [ovl_4e4_a_c.c](src/overlays/rom_7bc690/ovl_4e4_a_c.c) | new `[split]` candidate | `CSE_CFLAGS`; **blocker 1b at the value 1** |
| 7 | `OvlFunc_939_2009668` | `0x02009668` | [ovl_314_…_c_b.c](src/overlays/rom_7c460c/ovl_314_c_a_c_c_c_c_c_b.c) | new `[split]` candidate | delete an address-only local; **blocker 1b in reverse** |

Parked: `Func_8021390` (36 of 97), `OvlFunc_964_20090c4` (13 real of 116),
`Func_8021228` (15 of 126), `Func_80a5b94` (32 of 118).

Five of the seven came from the candidate list that did not exist at the start of
the round.

## THE FILTER WAS REJECTING THE SHAPE IT SHOULD HAVE BEEN OFFERING

Batch 182 established that a constant re-materialised across a join is two
source variables, because **gcc does not re-materialise a value it kept live
across a branch**. `filtered.py`'s duplicate-constant rule rejected any function
repeating an expensive constant *anywhere* — which is exactly that shape.
`OvlFunc_941_2008210`, batch 182's own specimen, would have been rejected by the
tool that was supposed to find it.

Teaching the rule to ask **where** the repeats sit — reject a repeat inside one
straight-line run, offer a repeat with a label between two sites — turned zero
candidates into 26. Three of this batch's five came directly from that list.

Then two agents, independently and on the same day, found the same problem with
the new marker: **it conflates two shapes whose fixes are inert on each other.**

    a repeated `mov rN, #imm8` feeding a STACK-ARGUMENT slot
        -> two source VARIABLES; split the local          (OvlFunc_941_2008210)

    a repeated POOLED id consumed as a register argument by a `bl`
        -> one source LITERAL that rerun-CSE commons;
           unreachable from source, needs CSE_CFLAGS      (OvlFunc_920_2008304)

On the second, four constant-facing spellings — separate named locals, the equal
spelling `0x181 << 1`, explicit `== 0`, and `goto`-raised label use counts — all
left the *same* 6 instructions in 4 regions, because constant propagation folds
any name back to the same `const_int`. **For a pooled constant there is no
source-level split.** The filter now emits `[split]` and `[cse]` separately.

Then function 7 showed the *new* test was wrong too, and in the more damaging
direction. `_site_kind`'s predecessor answered "reaches a `bl`" against
"everything else", so a **struct offset** — feeding an `add` or an `ldrsh`
index, reaching neither a stack slot nor a call — read as the split shape.
**Seventeen of the twenty-six candidates were labelled `[split]` on that basis;
exactly one actually is.** `OvlFunc_939_2009668` has no stack frame at all,
`push {r5, lr}` and no `sp` adjustment, so there was nothing for the lever to
act on, and giving each block its own offset local went 33 aligned to **38**.
gcc-2.96 Thumb already builds a shiftable constant with `mov`/`lsl` from a bare
literal, so plain literals on a named base are what reproduce the ROM's
per-block rebuild. The classifier is now three-way — `[split]`, `[cse]`,
`[offset]` — with "do not split these" attached to the third.

And function 5 showed the `[cse]` half is still only a hint.
`OvlFunc_932_200a934` was offered as `[cse]` and matched on the first candidate
with no flag at all, because **its two sites are in mutually exclusive arms and
rerun-CSE does not common across those**. The flag is for the guard/set shape,
where one use dominates the other. Cheap check before reaching for a build rule:
can either site reach the other?

## BLOCKER 1b HAS A DIRECTION SWITCH, AND THE ROM SETS IT

Two functions in this batch moved 1b, in opposite directions, and together they
turn it from a blocker into a lever.

`OvlFunc_933_2008cd0` shows **1b reaches down to the value 1**. Writing
`*(short *)(p + K) = 1;` narrows to a HImode `const_int`, and although
`thumb.md`'s `*movhi_insn` has an `I` alternative that would give `mov r3, #1`,
the value arrives through `force_reg` from the `movhi` expander and comes out of
the literal pool: `ldrh r3, .L10` with `.word 1`, where the ROM has `mov r3, #1`.
A pool word holding **1** looks absurd enough to be misread as a scheduling
artefact. Its escape is three separate locals in the dominating block.

`OvlFunc_939_2009668` shows the same mechanism wanted from the other end. There
the ROM *has* `mov r3, #0xa / strh`, a literal at the store gives the pooled
`ldrh`, and `int t = 0xa;` **in the entry block** is worth 19 of 123.

Set beside the `FieldMove_NoTarget` park earlier in this batch — where an `int`
local crossing a block boundary was the thing that *forced* an unwanted
`ldr rN, =0xffff` — all three are one rule read from different sides:

> The load width follows the width of the eventual store, so a value that must
> exist in a register **before** the storing block cannot narrow. When the ROM
> pools the constant, keep the value inside the store's expression. When the ROM
> has `mov` + `strh`, hoist it into a dominating block.

The ROM tells you which way to set the switch. That is the whole of 1b now.

One symptom not to chase, from the same function: at 123 instructions its early
return degraded from `bne <epilogue>` to `beq .LCB31 / b .L2 @long jump`, purely
because the spurious pool words had pushed the epilogue out of conditional-branch
range. **The long jump was a symptom of 1b, not a branch-polarity problem**, and
it closed when the pool words did.

## THE JOIN-SPLIT LEVER, BOUNDED FROM BOTH SIDES

Most of the batch's value is in knowing when batch 182's rule is *false*,
because it is cheap to try and all three failure modes look like near-misses.

**A switch-arm form** — `FieldMove_Target` repeats `gState + 0x24a` in two
mutually exclusive arms, and all three spellings are distinguishable:

| spelling | result |
|---|---|
| folded inline | one pool word `=gState+586`; the ROM has two and an `add` — 11 |
| one shared local | range spans the switch, priority drops, diff moves into the prologue — 13 |
| **one local per arm** | defeats the fold **and** keeps the range short — **exact** |

The cost side runs backwards from the usual dominance rule: naming the value
*adds* an instruction pair here, and that pair is what the ROM has.

**Inert when both uses already left the assignment's block** —
`OvlFunc_898_20084a0` was flagged on `0xcccc`, but one local and a split pair
compile *byte-identically*, because each use is already in a different basic
block from the assignment and gets rematerialised anyway. Split only when the
shared range spans the join and forces a high register. **Check the push list
first**: the signature is r10 plus an extra `mov` at every use.

**Powerless against two variables holding the same constant** — `Func_8021390`
wants `0` before a call and `0` inside a guard, materialised twice. Constant
propagation folds two source variables holding 0 into one `const_int` long
before allocation. Five assignment positions, two separate named zeros and five
types all measured exactly 36. Proved by construction: change *only* the
pre-call argument from `0` to `9` and gcc's whole allocation flips to the ROM's
two-materialisation shape. **The split is about variables; the fold is about
values, and the fold wins.**

## A DOMINATING-BLOCK LOCAL CAN BEAT THE FLAG

The recorded advice for a CSEd constant is to reach for
`-fno-rerun-cse-after-loop` and keep the literals. `OvlFunc_898_20084a0` is the
counter-example, and it is worth the space.

| | aligned |
|---|---|
| literals, `-O2` | 12 |
| literals, `-fno-rerun-cse-after-loop` | 6 |
| **`s = 0xcccc` named in the dominating block, no flag** | **4** |
| + `t = 0x1999` (same lever) | 2 |
| + `u`/`v` assigned before the `if`, used inside it | **exact** |

With literals gcc CSEs the constant into a callee-saved register and grows a
`push` the ROM lacks. The flag removes the hoist but leaves three
argument-scheduling residues it cannot touch. The local fixes *both*, because a
rematerialised pseudo has low `rtx_cost` and drops out of
`precompute_register_parameters` — which is what lets `mov r0, #2` land between
the two pool loads. **When one constant is both CSEd and mis-scheduled, try the
local before the flag.**

Two refinements to the guard/set note came out of function 4 alongside this. The
pair does not have to be in one block or one arm; and `-fno-gcse`,
`-fno-cse-follow-jumps`, `-fno-thread-jumps` and `-fno-expensive-optimizations`
all leave the copies in place, so **`-fno-gcse` not helping is positive evidence
FOR `CSE_CFLAGS`**, not evidence that the shape is unreachable.

## WHEN UNRELATED SPELLINGS TIE EXACTLY, CHECK A SIGNATURE

The notebook already says identical counts across unrelated spellings indict the
variable's existence. `OvlFunc_901_20088a8` extends where to look next: four
spellings — a named zero, a narrower zero, a hoisted zero, a named slot — all
measured **exactly 2**, and the residue was in none of them. It was in a
declaration. `__Func_8092c40` returns a value, and marking the callee `int`
makes r0 live out of the previous call, changing what
`precompute_register_parameters` may reorder across. Two instructions, one word
in an `extern`.

The ladder, when unrelated spellings tie: delete the variable; **then check every
callee's return type**; then check the flags. Only after all three is it a wall.

## THE CALL-SAVED ALLOCATION ORDER IS NOT MONOTONIC

Every register-birth-order note in this notebook up to now reads as if gcc hands
out call-saved registers in numerical order, so an r8/r9 diff is "adjacent
allocnos" and an r9/r10 diff is one step. That is wrong, and it has been
mis-sizing every estimate of how close a register diff is to closing.

An agent asserted this from `arm.h:989`. The tree ships no compiler source, so
rather than record it on trust it was settled with a probe
(`scratch/regorder/p.c`): seven values, each born at a distinct point, each live
across a call, so the allocator must rank them and hand out registers strictly in
priority order. Compiled with the production flags, by birth order:

    a1 (longest-lived, lowest priority)  -> r11
    a2                                   -> r9
    a3                                   -> r10
    a4                                   -> r8
    a5                                   -> r6
    a6 (shortest, highest priority)      -> r5

Highest priority first, that is **r5, r6, [r7], r8, r10, r9, r11** — **r10 is
handed out before r9.** So an r8/r9 diff is allocnos four and six trading
places, and an r9/r10 difference is a two-place move. The probe could not place
r7 (a live frame pointer reserved it); its position is carried from the existing
notes and the corpus, which agree.

## Other mechanisms worth keeping

**A negative multiplier is what selects the shift chain.** When the ROM expands a
constant multiply as shifts and adds where a pool load and a `mul` would plainly
be cheaper, that is not gcc preferring shifts: `expand_mult` calls `synth_mult`
on the *absolute* value and negates after, with the cost budget taken from the
negative `MULT`. `x * 6553` gives `ldr =0x1999 / mul`; only `x * -6553` gives the
seven-instruction chain ending in a `neg`. **Read the sign off the ROM.**
`OvlFunc_964_20090c4` has both spellings in one function — a clean internal
control. Two cautions with it: `C - x*k` and `x*-k + C` are indistinguishable in
an *isolated* probe and separate only under the real function's register
pressure, so isolated probes are not safe for sign questions; and a statement
break stops the distribution, `(r - 5) * k` folding to `r*k - 5k` where the split
form gives the ROM's `sub / mov / mul`.

**`mov rLow, rHigh` before a store is a SECOND reload**, and it is evidence about
control flow rather than statement order: gcc emits it only when a call sits
between the pointer's definition and its use. With the definition adjacent,
reload inherits the scratch and the copy disappears. Worth checking before
sweeping declaration orders — `OvlFunc_964_20090c4`'s park tried 34 placements
and 28 declaration orders for nothing, because the question was never where the
assignment sat.

**A spilled parameter is a statement about some other pseudo's live range.**
`Func_8021228`'s ROM spills a parameter to the frame; naming the frame pointer as
the *first* statement adds the eighth allocno that pushes it out, which is what
the ROM has (60 → 54). Do not read a spill as a mistake to avoid.

**Do not reuse one local for two roles.** Splitting one two-def pseudo into two
variables — same instructions, same values — went 54 → 26 on the same function.
The ROM said so: a value born once and used once is a variable of its own. This
is the read-count rule's third face, after the load form and the constant form.

## Parks

`Func_8021390` (`0x08021390`) — 36 of 97. The dominance contradiction, third
instance, and the first where the constant is **zero** and the hoist is provably
not CSE or PRE: twelve flags including `-fno-gcse` and
`-fno-rerun-cse-after-loop` are byte-identical. Characterised by construction
rather than inference. Ceiling if the zero were solved is 11 of 97.

`OvlFunc_964_20090c4` (`0x020090c4`) — 17 aligned of 116, of which **four are
phantom** (`__umodsi3` vs `_umodsi3_RAM`, already aliased at
`overlays/rom_7ed0a0/overlay.ld:110`), so 13 real. Instructions 0–35 and the
whole loop body are byte-for-byte. Wall is reload inheritance around a
callee-saved high register. ~100 spellings and 8 flags; the floor is insensitive
to all of them.

`Func_8021228` (`0x08021228`) — 15 of 126, one of which is a `_MSG_980` symbol
line. **The symbol was deliberately not added to `message.sym`**: it closes
nothing while the function is parked, and the namespace should not accumulate
names for functions that have not landed. Two residues that are really one wall,
since fixing either in isolation costs more than it saves.

## State

- **1,877 functions remain in assembly** — 647 unparked and 291 parked in the
  main ROM, 618 unparked and 321 parked across the overlays. 3,479 elevated `.c`
  files in the tree.
- `make clean && make -j8 && make compare` green; SHA1
  `5c4695205413df7db52b9a184815a07783999971`. Every address in the breakdown
  table checked against the linked ELF, `.gcc2_compiled.` present in each object.
- Four splits taken, each verified byte-neutral with `make compare` green
  *before* any `.c` landed. Three new `CSE_CFLAGS` rules in the Makefile.
- Subagents contributed six of the seven matches and all four parks. Two agent
  claims about compiler internals were checked before being recorded, and both
  held: `REG_ALLOC_ORDER`'s non-monotonic tail, and the `update_equiv_regs`
  gate at `local-alloc.c:868`. The checking was still the right call — one of
  them turned up the fact that the source was readable at all.

## What the round says about method

Three of this batch's corrections were to things *this notebook already said*,
and none of them were found by looking for errors. They fell out of doing the
work:

- the selection filter rejected the shape the previous batch had just solved,
  because nobody re-read the filter after changing the rule;
- the replacement marker then conflated three shapes, and the largest bucket was
  wrong, because "reaches a `bl`" was tested instead of "reaches a stack slot";
- blocker 1b was recorded as unreachable in one direction and unremarkable in
  the other, when it is one mechanism with a switch the ROM sets.

The common thread is that each was a note written once, from one function, and
never measured against a second. The parks are better than the notes for exactly
this reason: a park carries its measurements, so the next reader can see what was
tried and what it cost. **A finding stated without its table is a finding that
cannot be corrected.**
