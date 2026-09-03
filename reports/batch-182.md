# Batch 182

Six elevated, one parked with its wall measured, one blocker-class entry
corrected. The batch's theme is **live range as the thing you are actually
writing**: four of the six turned on where a value's first definition sits
rather than on what it is spelled as, and both of the batch's new levers are
statements about live length rather than about types or operators.

## Function breakdown

| # | function | address | file | previously | what it took |
|---|---|---|---|---|---|
| 1 | `Func_80b595c` | `0x080b595c` | [rom_b5368_b.c](src/rom_b5000/rom_b5368_b.c) | — | message id as a linker symbol |
| 2 | `Func_80b1868` | `0x080b1868` | [rom_b0070_a_a_c_c_c_a_b.c](src/rom_b0000/rom_b0070_a_a_c_c_c_a_b.c) | — | message id as a linker symbol (`0xad0 = 0xad << 4`) |
| 3 | `OvlFunc_934_2009390` | `0x02009390` | [ovl_1300_c_c_a_b.c](src/overlays/rom_7bdeb0/ovl_1300_c_c_a_b.c) | — | message ids `_MSG_810`..`_MSG_813` |
| 4 | `Func_80b3284` | `0x080b3284` | [rom_b0070_c_c_c_b.c](src/rom_b0000/rom_b0070_c_c_c_b.c) | — | symbol + **delete the halfword pointer** + name the base *inside* the else arm |
| 5 | `OvlFunc_941_2008210` | `0x02008210` | [ovl_30_c_a_c_c_c_a_a.c](src/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_a.c) | — | **a re-materialised constant after a join is two locals** |
| 6 | `OvlFunc_973_20080ec` | `0x020080ec` | [ovl_30_c_a_c_c_c_a_a_b.c](src/overlays/rom_7fc720/ovl_30_c_a_c_c_c_a_a_b.c) | — | symbol + **an initialiser is not an assignment** |

Parked: `FieldMove_NoTarget` (`0x08096810`), 6 aligned of 137, wall quantified.

## SIX MESSAGE IDS WERE SYMBOLS, AND THE TELL IS THAT GCC WOULD HAVE BUILT THEM

Half this batch turned on the same recorded tell, applied six times:
`_MSG_810`, `_MSG_811`, `_MSG_812`, `_MSG_813`, `_MSG_ad0`, `_MSG_d1c`,
`_MSG_c20`. Each is a value gcc can synthesise in two cheap instructions —
`0x810 = 0x81 << 4`, `0xad0 = 0xad << 4`, `0xc20 = 0xc2 << 4` — and each is a
value the ROM spends a whole pool word on. gcc pools only what it cannot make
with `mov`+`lsl`, so a pooled *shiftable byte* is positive evidence that the
source named a linker symbol.

Two of them added something the recorded tell did not have. **gcc will spend a
callee-saved register on a symbol address and not on an integer it can
rematerialise.** `Func_80b3284` holds `0xd1c` in a callee-saved register and
reaches `0xd1d`..`0xd20` with `add r0, #K`; written as a plain integer, each
site gets its own `ldr r0, =0xd1f`, the high register disappears and the push
list changes. And in `OvlFunc_973_20080ec` the knock-on was larger still: once
the id lives in a register rather than a pool slot, cse's `related_value` stops
deriving `0xc21` and `0xc22` from the base, so the ROM's `add r0, r5, #1` /
`add r5, #2` pair goes too and the r5/r6/r7 assignment rotates. **27 aligned
regions with the literal, 13 with the symbol** — nine instructions of which only
two were the constant itself.

The corollary for the next round: when a pooled small constant is present, do
not measure the two-instruction cost. Measure what CSE can and cannot derive
from a pool slot, because that is where the rest of the diff lives.

## A RE-MATERIALISED CONSTANT AFTER A JOIN IS TWO LOCALS — the read-count rule's mirror

`OvlFunc_941_2008210` has a guarded prologue and then a join, and both halves
pass `0x15` as a stack argument. One shared local is the obvious reading of
that and it is wrong by **18 instructions**: the shared pseudo's live range
spans the whole function, `allocno_compare` ranks it below the short-lived pair
locals, gcc gives it r10, and all eight stack-argument sites pay

    ours   mov r3, r10 / str r3, [sp]
    rom    str r5, [sp]

The reference states the answer plainly, and this is the shape to learn:

    rom    mov  r5, #0x15        <- inside the if body
           ...
    .Ljoin:
           mov  r5, #0x15        <- AGAIN, where r5 already holds it

**gcc does not re-materialise a value it kept live across a branch.** A second
`mov rN, #imm` on a path where rN already holds `imm` therefore means the source
had a *second variable*, whose live range begins at that assignment. Splitting
the if-body's uses into their own pair shortened both ranges, lifted them above
the pair locals, and put the hot values in r5/r6 with one `str` per site. Exact.

This is the read-count rule turned around, and the pair is worth stating
together:

| ROM shows | means | do |
|---|---|---|
| a load then `mov rB, rA` | a CSEd **second read of one object** | remove a name, read `*p` twice |
| a constant materialised twice across a join | **two objects** | split the local in two |

Loads collapse toward one variable; constants split into several. In both cases
**the number of materialisations in the ROM is the number of source-level
things**, and the register rotation beside it is the symptom.

Cheap check in the other direction: one local for a value the ROM materialises
twice means two have been merged.

## AN INITIALISER IS NOT AN ASSIGNMENT

The one-character lever, and the first source-level control the notebook has for
*lowering* an allocno's priority rather than raising it.

    int redraw;  ...  redraw = 1;      13 aligned regions
    int redraw = 1;                     3 aligned regions

Both emit the *same instruction at the same index*. The difference is that the
initialiser makes the flag's first definition function entry, which lengthens
its live range, which lowers `n_refs / live_length` in `allocno_compare`, which
demotes it from r5 to r7 — and lets the CSE-hoisted `&gKeyPress` take r5, which
is what the ROM does.

Every previously recorded birth-order lever works by *raising* a local's
priority (assign it later, shorten its range). This is the other direction, and
it costs one character to try. Blocker 2 now has both.

The last region closed on where `redraw = 0;` sits: anywhere inside the guarded
block except after the final `bl` gives the ROM, because **sched2 sinks a store
freely down into an argument block but will not hoist it back over a call**.
That asymmetry is a one-probe fix worth remembering.

## BLOCKER 1b IS PARTLY REACHED

The entry said no formulation reaches the ROM's word-sized pool load for a
halfword store. Two do, and the entry's own reasoning explains why they are the
only shapes that can.

The load width follows the width of the eventual store, so anything that keeps
the value inside the store's expression narrows. What cannot narrow is a value
that must already exist in a register before the block that stores it:

    int inval;
    inval = 0xffff;                          /* the FIRST statement */
    ...
    if (v != -1) { ... *slot = inval; }      ->  ldr rN, =0xffff / strh

Assigned inside the arm, gcc folds it straight back to a HImode `const_int` -1,
commons it with the `mov #1 / neg` from the `!= -1` test, and stores from that
register — three instructions short. **It is the dominating block that does the
work, not the `int` type.** Worth 27 aligned down to 9 on `FieldMove_NoTarget`.

The corpus template was already in the tree and unnoticed:
[rom_ea54_c_b.c](src/rom_9000/rom_ea54_c_b.c) sets `rv = 0xfc88;` at the top of
the function and stores it in a guarded arm. Grepping the corpus for the shape
would have found this months ago; the blocker entry was read as a dead end and
so nobody looked.

1b is no longer "nothing reaches it". It is "the value must live in a register
the store cannot narrow into" — which also explains the constant-zero escape
found in batch 181, where a register-allocated `short zero = 0;` is the same
statement at a different width.

## Two placement levers on the inn

`Func_80b3284` needed neither a type change nor an operator change; both of its
remaining diffs were about *where* a name existed.

- **Delete the halfword pointer** and write its address expression out at all
  four use sites, or the offset build hoists above the actor lookup.
- **Name the `gState` base inside the else arm.** Named above the branch, gcc
  takes a fourth callee-saved register and rotates the other three.

Same family as batch 181's "delete the local", with the discriminator being that
here the local's *scope*, not its existence, was the variable.

## Parks

`FieldMove_NoTarget` (`0x08096810`) —
[src/non_matching/rom_8a000/8096810.c](src/non_matching/rom_8a000/8096810.c).
**6 aligned of 137**, with instructions 12–136 identical including every
register. The wall is quantified from gcc's own allocation ranking rather than
inferred: `global.c:allocno_compare` scores
`floor_log2(n_refs) * n_refs / live_length`, and the function turns on one
comparison between the kind variable and the `gState` slot pointer, whose
priority is 1600. Only a live length of 18 wins it, which needs the kind
variable assigned *last*; but the ROM's emission order needs it assigned
*first*, or the pooled base dies before its load and the anti-dependency
reorders the two bases. Mutually exclusive at every statement order tried, gap
exactly one RTL instruction, measured on both sides.

Two levers did fire there and are recorded in the park: the negative-offset
global spelling, and the dominating-block halfword constant above.

## A hazard, in its cheap form

`asm/rom_b0000/rom_b0070_c_c_c.s` holds five functions and a trailing `.rodata`
block. The split was taken at the *first* function, which leaves the data with
the four that stay and needed no hand-editing. That is the cheap case of the
batch-181 hazard (a whole-file conversion that dropped exported data and failed
at link, not at the screen). **The expensive case is when the target is last**,
because then the trailing data travels with it into the `.c`. Check which end
the target is on before choosing the split.

The overlay split for `OvlFunc_973_20080ec` was verified byte-neutral —
`make compare` green with three `.s` files and no `.c` — *before* any C landed,
which is the discipline `split_s.py` prints and which separates a layout
mistake from a bad decompilation.

## State

- **1,884 functions remain in assembly**: 653 unparked and 286 parked in the
  main ROM, 625 unparked and 320 parked across the overlays.
- 3,472 elevated `.c` files in the tree.
- `make clean && make -j8 && make compare` green; SHA1
  `5c4695205413df7db52b9a184815a07783999971`. Every address in the breakdown
  table checked against the linked object, `.gcc2_compiled.` present in each.
- Subagent screening continues to carry most of the throughput; two of this
  batch's six came from agents, and both were re-verified by `make compare`,
  which remains the only authority. One agent correctly stopped short of
  claiming a match because closing it required a `message.sym` line it had been
  forbidden to write — the right call, and the reason the screen and the build
  are kept separate.
