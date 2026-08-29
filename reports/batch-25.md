# Batch 25 — 6 functions, the constant-CSE pass, and a wrong park found

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–24 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare`, unassisted. Every address
below read back from the linked ELFs.

## Read this first — a parked function had semantically wrong C

`OvlFunc_909_200828c` was parked with a correct blocker diagnosis (constant-CSE)
and **incorrect code**. It had `__ActorMessage` and `__SetFlag` inside the `if`:

    if (__GetFlag(0x303)) { __MessageID(0x176c); __ActorMessage(0xf, 0);
                            __SetFlag(0x303); }

The ROM runs both unconditionally — its join label sits **before** the
`__ActorMessage`, not after the `__SetFlag`. The parked code would have skipped a
line of speech and never set the flag on the path that matters.

**This is the failure mode worth taking from this batch.** The blocker was real,
so the diagnosis explained the diff, so nobody looked further. It is the second
occurrence — `OvlFunc_931_2008360` in batch 20 was the first — and **both times
the tell was a label in a different position**, not an instruction.

`tools/audit_parks.py` now checks for it across the whole parked set: it screens
every parked `.c` and reports differing lines that are *labels*, separating
same-length streams (where a displaced label is meaningful) from different-length
ones (where every later label shifts and the signal is noise). `tryc.py` gained
`--full` for it, because the keyhole is what let this case hide — on a 41-line
function the label difference sat outside the printed window.

**Result of the first full run: 83 files screened, two same-length hits, both
checked, both benign.** `DecFlagByte` reuses a load on the path where the store
did not happen (semantically identical); `Func_80b09fc`'s label is a pool-skip
target. No parked file currently carries the failure mode. Both findings are
recorded in the tool so they are not re-investigated.

## The constant-CSE pass is identified

gcc-2.96 runs a **second** common-subexpression pass after loop optimisation,
and that is what hoists a repeated pooled constant into a callee-saved register.
`-fno-rerun-cse-after-loop` removes it. `-fno-gcse`, `-fno-cse-follow-jumps`,
`-fno-cse-skip-blocks` and `-fno-expensive-optimizations` all leave it in place.

That names the mechanism behind what has been the tree's highest-value open
blocker — the class where the ROM loads a value twice and gcc caches it, paying a
push, a pop and two moves to save one pool load. One member comes out **longer
than the ROM** as a result.

**It is not a general lever, and the evidence is thin.** Sweeping all 85 parked
files with the flag matched exactly one. Two overlay TUs are now built with it
(`CSE_CFLAGS` in the Makefile, flagged in `HANDOFF.md`), and that is an
assumption about the original build in the same category as the existing -O1
rules but on less evidence. **It may instead mean gcc-2.96 runs a pass the
original compiler did not**, in which case the right fix is a compiler
difference and those two rules should be dropped. That needs someone who knows
the original toolchain.

`tools/tryc.py` takes `--no-rerun-cse` so the pass can be probed without
hand-compiling.

## Functions

| function | address | overlay | note |
|---|---|---|---|
| `OvlFunc_959_20090a8` | `0x020090a8` | rom_7e7574 | six calls, two answers on r0 |
| `OvlFunc_947_200a0f0` | `0x0200a0f0` | rom_7d0e88 | six-argument call, two stack slots |
| `OvlFunc_899_200852c` | `0x0200852c` | rom_794ac0 | **unparked**, `CSE_CFLAGS` |
| `OvlFunc_909_200828c` | `0x0200828c` | rom_79c738 | **unparked**, wrong C + `CSE_CFLAGS` |
| `OvlFunc_959_2009038` | `0x02009038` | rom_7e7574 | result cached in r8 |
| `OvlFunc_882_2008198` | `0x02008198` | rom_77dd1c | no lever needed |

## Two tells worth having

**The declaration lever reports per-CALL-SITE facts, not per-callee ones.**
`OvlFunc_959_20090a8` has six multi-argument calls that **disagree** about where
r0 goes — three want it last, three want it first. Three declarations, three left
implicit, matched first screen. Six calls, two answers, in one function: so this
is not a property of the callee or a house style, it is whether the original
translation unit had that prototype in scope.

**"Has stack arguments" is not the stack-arg blocker.** `OvlFunc_947_200a0f0`
passes two arguments on the stack and needed nothing special. The parked class is
about the *order* the slots are filled relative to the registers; here the ROM
fills both slots first and then the registers, which is what gcc does anyway.

## A bound on the declaration lever

**It cannot reach the first call after a control-flow JOIN.** The lever works by
fixing whether r0 is live across the *preceding* call, so where the preceding
call differs per path, no declaration can decide the question.

`src/non_matching/ovl_793768/2008e0c.c` is 41 instructions against 41 with
**39 identical** and one misplaced `mov r0` — the plain fill-order shape the
lever normally retires. Seven declaration combinations produce byte-identical
output. Its `__ActorMessage` sits immediately after a three-way join where the
predecessor is `__CutsceneWait` on one arm and `__MessageID` on the other two.
Check for a join above the mismatching call before spending screens on it; this
cost seven.

## A correction to earlier counts

Batches 20–24 quoted a "parked" figure taken from a file count under
`src/non_matching/`. That count includes four files documenting blocker *classes*
rather than individual functions (`narrow_constant.c`, `constant_reuse.c`,
`interleaved_arg_setup.c`, `preheader_load_merge.c`). The figures in those
reports are each about four too high. The correct current number is below.

## Counts

253 functions elevated in total. 3,046 hand-written functions remain in `asm/`
of 5,714. **92 parked functions**, plus 4 files documenting blocker classes.
