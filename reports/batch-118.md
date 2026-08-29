# Batch 118 — the round that paid for the parallel experiment

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
All 31 symbols read back out of `goldensun.elf` / the linked overlay ELFs with
`arm-none-eabi-nm`.

**31 elevated, 2 parked. 2397 → 2366 remaining.**

Four agents on the 60–99 instruction band returned **29 verified matches from 48
functions — a 60% hit rate**, against 55% in round 2 on an easier band. Every one
reproduced on my re-screen. Two more came from my own rounds.

## What that band looks like now

The 60–99 band had **20 parks across 570 functions** when the round started —
essentially untouched — and it turned out to be the most productive band worked
so far. That is the useful planning fact: the small bands are picked over, the
mid band is not, and the levers accumulated over 117 batches transfer to it
without modification.

## A screening bug that was costing correct answers

`mov r2, r3` and `add r2, r3, #0` **assemble to the same halfword** (`0x1c1a`).
The ROM's disassembly writes one, gcc-2.96 writes the other, and `tryc.py`
reported the difference. `Func_80a19a0` screened 1-of-79 dirty while being
byte-for-byte identical.

Folded in `tools/tryc.py` — **low registers only**. `mov r8, r3` is `0x4698`, a
genuinely different instruction with no `add` equivalent, so folding a high
register would hide a real difference.

That makes three false-negative shapes found so far (`.pool_aligned` in a loop,
two returns cross-jumped to one epilogue, and this). All three fail in the safe
direction — a correct function reported as wrong — so the cost is wasted screens.

## A correction I made to batch 116, and a "fix" I nearly made and shouldn't have

**The `push {r4}` hypothesis was overstated.** Batch 116 called it the thing to do
first and said it would reclassify "a slice of the register-allocation class,
which is the largest blocked class in the corpus". Measured: **16 of 2,452
hand-written functions push r4** — eight in `common2`, seven in the m4a driver
(genuine hand-written assembly), one in boot. Four parks carry it, all
`common2`. And the 164-park sweep I proposed **had already been run** and was
recorded in the tree's own park notes: improves eight, matches none. Corrected in
`reports/batch-116.md` directly.

**`asm/overlays/rom_7ac2d8/ovl_d58_a.s` defines `.Ld90:` twice**, which makes a
byte-identical function screen 6-of-69 dirty. It looks exactly like a
transcription defect, deleting a label is byte-neutral, and `make compare` stayed
green when I removed it. **It is not a defect** — removing it took the screen from
6 differing to 47, because gcc genuinely emits two labels at that address from
the correct C. The duplication is faithful to the original object. Restored.

The general form is worth keeping: *`make compare` staying green proves a change
is byte-neutral, not that it is correct.*

## New compiler-flag territory

**`-ffixed-r7`.** `OvlFunc_945_200d6dc` needs three callee-saved registers; gcc
takes r5, r6, r7 and the ROM takes r5, r6 and **r8** — which costs
`mov r6, r8 / push {r6}` and the matching pop. Reserving r7 gives 55 → 59 lines
(the ROM's count) and 41 differing → 9. `-fno-omit-frame-pointer` also reserves
r7 and is wrong: it adds frame setup, 61 lines. Not a class key — checked against
two parks with the same signature, both unchanged.

**An explicit rule beating a wrong wildcard.** `OvlFunc_968_20096a4` was caught by
a `rom_7f2f14/ovl_30_c_a_c_a_c_c%` wildcard applying `O1_CFLAGS`: 36 differing at
`-O1`, 5 at `-O2`, exact at `-O2 -fno-rerun-cse-after-loop`. An explicit rule
overrides it without narrowing the wildcard.

**`-fno-gcse` reaches a re-read no `cse`-family flag does** — but where a
`volatile` declaration also matches, `volatile` is better because it costs no
flag group. `OvlFunc_947_200a230` is wired that way.

## The rule I had been over-applying

The HImode-literal rule said an `int` local is needed for a constant stored
through a `u16 *`. Measured across four functions, **plain literals are correct
for 1…0x7fff; only `0` and values ≥ 0x8000 need the local.** `Func_801c188`
stores four `u16` members with bare literals and is exact.

The ≥ 0x8000 case has a trap in front of it, which cost me two screens on
`OvlFunc_884_2008674`: through a *signed* `short *`, `0xb000` becomes `-0x5000`
and pools as `0xffffb000`. Making the pointer unsigned fixes the pool word but
gcc still pools it. Both changes are needed — 37 differing → 37 → exact.

## Selection, not screening, is where two rounds were lost

Between the useful rounds I attempted four functions and elevated none. Two of
them were straight-line call scripts that failed **on the same thing as each
other**: a constant used at two call sites with no control-flow boundary to
satisfy the constant-CSE precondition. Seven flags and the symbol-address
technique all left the instruction *count* wrong.

It was visible in the assembly before I wrote a line of C, so
`tools/script_candidates.py` now ranks straight-line scripts by repeated
*expensive* constant. Two refinements it needed:

* Counting bare `mov rN, #imm8` reported 40 of 41 functions as blocked — the
  opposite of useful. Small immediates are rematerialised for free.
* It must count `mov`+`neg` negatives as well as pool loads and shifted builds.
  `OvlFunc_881_2009b5c` read as clean and is blocked; the corpus test is that
  **0 generated `.s` files contain two consecutive `neg rN, rN`**, so a call
  taking two negative constants is unreachable rather than merely unreached.

In band 30–70 there are 62 such scripts and 31 have no repeated constant. Two
picked off the clean list matched on the first screen.

## Parked

[2008c74](../src/non_matching/ovl_7c460c/2008c74.c) and
[2008504](../src/non_matching/ovl_7c097c/2008504.c) — 2 of 53 and 2 of 54, the
same two-line residue with different callees, which is what makes it a family
rather than a coincidence. `f(0, 0, -8)`: the ROM slots `mov r1, #0` between the
`mov` and the `neg` of the `-8` build. Eleven spellings and three flags measured;
the only one that moves the count moves it the wrong way.

## Also documented this batch

Eleven findings from the agents are now in `docs/elevation.md`: `volatile` as a
*reading* rather than a hack (a global the ROM reloads; a stack halfword stored
then loaded back); naming the store's **destination** pointer, which in one case
dissolved a difference twenty positions *earlier*; `i = 0;` as its own statement
versus a `for`-init; deleting a single-use local to reach an r0↔r4 exchange;
deleting a loop-bound local to move the bound into a high register; strict
aliasing **sinking** a store and a `char *` lvalue pinning it; struct members
rather than two pointer locals for two nearby fields; a measured negative that
the symbol tell does not govern argument-setup order; and that
`-fno-schedule-insns2` is an actively misleading probe — on every
scheduling-shaped residue this round it multiplied the count and destroyed the
evidence.
