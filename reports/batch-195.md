# Batch 195

Five elevated, three parked, and one documented blocker class corrected.

The correction is the batch's result. A rule the notebook has relied on since
batch 192 turned out to rest on a coincidence of the single case it was written
from, and the two rounds that stalled in the middle of this batch stalled
*because* of it — they were sweeping spellings against a wall whose shape had
been mis-stated.

## Function breakdown

| # | function | address | file | what it took |
|---|---|---|---|---|
| 1 | `OvlFunc_947_200a040` | `0x0200a040` | [ovl_1528_a_c_a.c](src/overlays/rom_7d0e88/ovl_1528_a_c_a.c) | **fakematch**; *previously parked* at 11 of 29; one pin |
| 2 | `OvlFunc_948_2008ccc` | `0x02008ccc` | [ovl_30_…_a_c_b.c](src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_a_c_b.c) | **fakematch**; a pin *removed* was the lever |
| 3 | `OvlFunc_903_20084f4` | `0x020084f4` | [ovl_314_c_a_c_a_b.c](src/overlays/rom_798dc4/ovl_314_c_a_c_a_b.c) | **fakematch**; rematerialised constant `2` |
| 4 | `OvlFunc_922_2009154` | `0x02009154` | [ovl_30_…_c_a_b.c](src/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_a_a_a_b_c_a_b.c) | **no pins**; a real match in two screens |
| 5 | `OvlFunc_916_20088b0` | `0x020088b0` | [ovl_30_…_a_a_b.c](src/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a_a_b.c) | **fakematch**; a pooled constant rematerialised |

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## THE "SAME-VALUE MOVS" CLASS WAS MIS-STATED

Batch 192 closed `OvlFunc_881_200b2f0` and explained a transposed `mov` pair by
saying **all three registers receive the same value, so nothing orders them**.
That has been quoted in parks ever since. It is wrong, and a minimal reproducer
— an `extern void f(int,int,int)` and nothing else — settles it in one line:

    q1 = 0x80; q2 = 0x80; q2 <<= 9; q0 = 9; q1 <<= 10;   ->  mov r2 / mov r1 / lsl r2 / mov r0 / lsl r1
    q1 = 0x81; q2 = 0x80; q2 <<= 9; q0 = 9; q1 <<= 10;   ->  mov r2 / mov r1 / lsl r2 / mov r0 / lsl r1

**Different constants transpose identically.** Equality of the values never had
anything to do with it.

The actual rule: **gcc emits the `mov`s in the order their consuming SHIFTS
appear.** Shift r2 first and `mov r2` goes first. Measured inert against it:
source assignment order, declaration order, `do { } while (0)`,
`__asm__ volatile("")`, building one constant in two steps, and deriving one
operand from the other — seven forms, byte-identical.

### The trap: the two orders cannot be set independently

The ROM shape that stalled two functions wants the `mov`s in one order and the
shifts in the other:

    target   mov r1 / mov r2 / lsl r2 / mov r0 / lsl r1

Writing the third argument inline *does* break the coupling — `f(q0, q1, 0x80 << 9)`
puts `mov r1` first — and takes the tail with it:

    f(q0, q1, 0x80 << 9)                    ->  mov r1 / mov r2 / mov r0 / lsl r1 / lsl r2
    f(q0, q1 << 10, q2)   and three others  ->  mov r2 / mov r1 / lsl r2 / mov r0 / lsl r1

Two mutually exclusive reachable states, the ROM a third. Structurally the same
two-state trap measured on `200cf44` in batch 194, reached from the other side.

**`tools/crossed.py`** turns this into a pre-filter: compare the ROM's `mov`
order against its shift order, and if they are crossed, park rather than sweep.
**Three of the five best-ranked candidates carry the shape.** That hit rate is
the whole explanation for two rounds of stalling.

*The tool shipped with a bug that it exists to prevent.* Its first version
reported CLEAN for both functions known to carry the wall, because it split call
blocks on `startswith('bl ')` and the disassembly separates the mnemonic with a
**tab** — nothing split, every function collapsed into one block. It was caught
only because it was validated against the known-bad names before use. That is
recorded in the docstring together with the two names to re-check against: a
filter that passes the cases it exists to catch is worse than no filter.

Still open, and stated as such in the docs: batch 192's interleave *did* close
`OvlFunc_881_200b2f0`, and the identically-shaped call in `2008b68` resists it.
One of the two has a property the other lacks and it has not been identified.

## PINS ARE NOT FREE

Batches 193 and 194 established where a pin *reaches* something. This batch has
the first two cases of one **costing**.

`OvlFunc_948_2008ccc` matched only when pins were **removed**. After the flag
fix it sat at 2 of 150 on a transposed pair, and every attempt to steer that
pair was byte-identical — opposite source order, a barrier between them, moving
the interleaved store, a data dependence (which gcc folds back to the constant,
losing the ordering with it), and plain `int` locals. What matched was deleting
the pins at that one call: gcc's natural order there was already the ROM's, and
pinning is what broke it.

`OvlFunc_903_20084f4` then had a pin that was merely dead weight. Its teardown:

    the pin on the two `*p = 2` stores      108 differing without it
    the pin on the two orr sites              4 differing without it
    the second SetPos argument fill           3 differing without it
    the named zero at +0x55                   2 differing without it
    the FIRST SetPos argument fill            STILL MATCHES  -- removed

Three pinned registers written because the site *looked like* the others.
**Try the site unpinned before assuming a residue needs a lever**, and tear down
every lever afterwards — it is the only thing separating a lever from a
superstition.

## REMATERIALISATION IS NOW THE DOMINANT SINGLE CAUSE

Four of the five elevations turned on it, and the class widened twice.

**It is not limited to inline-able constants.** In `OvlFunc_916_20088b0` the
hoisted value is a *pool load* — `ldr r5, =0x3333` feeding two calls where the
ROM issues `ldr r2, =0x3333` at each. The mechanism is the register class, not
the width of the value.

**Hoisting can be expensive rather than merely different.** In the parked
`2008ff0` gcc hoisted a speed pair and spilled **r8 and r10** to afford it;
in `2008b68` it spilled **r8, r9 and r10** — eight instructions the ROM does not
have. The tell is not a `mov`: it is high registers appearing in a function with
no business touching them.

A related new variant: `*(short *)(p) = 0xf8 << 8` gives `ldr r3, =0xfffff800`,
because `0xf800` does not fit a *signed* short and gcc materialises the
sign-extended pattern from the pool where the ROM builds `mov`+`lsl`. Computing
it in an `int` first fixes it. **The trigger is the sign of the value, not the
store width.**

## Smaller results

**Anchor the value that has to MOVE, not the one that has to arrive.**
`OvlFunc_947_200a040` was a register role swap: pinning the *temp* alone leaves
5 differing because it says nothing about where the actor goes, while pinning
the *actor* alone matches, because moving it off `r0` is the whole decision.

**Block-scoped pins reach sites function-scoped ones do not.** A pin reused
across a dozen call sites is weaker than one declared beside the call it serves.

**A six-argument call needs both stack arguments live at once.** The ROM builds
both before storing either; gcc reuses one register for both. Same instruction
count, different registers, so the length never moves and the residue is purely
allocation. `OvlFunc_922_2009154` was 21 of 164 and *every one* of the 21 was
this — fixed with named locals at seven sites and **no pins anywhere**, which is
why that file carries no `fakematch.txt` entry.

Which values get a local is decided by the ROM's register choice: callee-saved
means one local assigned once, scratch means a local reassigned before each call.

## Parks

| park | state | blocker |
|---|---|---|
| [2008124](src/non_matching/ovl_7ec968/2008124.c) | 104 of 144, length exact | base lifetime; C scope is **not** the lever, and a pin makes it worse |
| [2008ff0](src/non_matching/ovl_7c460c/2008ff0.c) | 2 of 157, length exact | the crossed mov/shift order |
| [2008b68](src/non_matching/ovl_7d30e0/2008b68.c) | 140 lines against 139 | the same, twice |

Both of the last two are stamped with the correction, since their original
diagnoses named the superseded "same value" reasoning.

## Correction to batch 194

Batch 194 reported nine parks sitting under a non-default-flag wildcard and
called them the cheapest work available. That was **softer than stated**. Four
improve at the default flags but none matches outright, so mis-scoping cannot be
claimed the way it could for `rom_7f2f14`; the `common2_%` entries are not
mis-scoped at all; and one entry was a false hit — the sweep matched parks to
translation units by address **suffix**, and two functions in different overlays
end in `_200a040`. The one that elevated did so for reasons unrelated to flags.
Match a park to its `.s` by the function name in its header, never by the
address in its filename.
