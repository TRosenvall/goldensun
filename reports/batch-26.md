# Batch 26 — 5 functions, and a rejected filter turned into a worklist

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–25 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare`, unassisted. Every address
below read back from the linked ELFs and every path confirmed to exist.

## The `-fno-rerun-cse-after-loop` rule, and when it applies

Batch 25 identified gcc-2.96's second common-subexpression pass as the mechanism
behind the constant-CSE class but could only show one function matching with the
flag. Two things changed that.

**First, the bound was wrong and is corrected.** Batch 25's note said the flag
fixes the *pooled* constant variant and does nothing for the *register-built*
one. `OvlFunc_890_2008108` builds `0x200` twice with `mov` + `lsl` — register
built — and the flag fixes it.

The distinction that actually holds is **where the repetition sits**:

| | |
|---|---|
| across separate **calls** | the rerun-CSE pass hoists the value into a callee-saved register; the flag stops it |
| inside **one argument block** | `OvlFunc_965_2009158` builds `-1` three times for a single call. The flag changes nothing — that is argument setup, not CSE |

**Second, applying it globally was tested and fails.** Adding
`-fno-rerun-cse-after-loop` to `GCC296_CFLAGS` and building from clean breaks
several overlays (`rom_78603c`, `rom_786f0c`, `rom_787e04`) and leaves an
undefined reference to `_call_via_sl` in the main ROM link. So the pass is wanted
almost everywhere and unwanted in these TUs — which is what a per-file rule
means, and is a point in favour of the per-TU reading over a whole-compiler
difference. **Six TUs now carry `CSE_CFLAGS`**; the standing caveat is in the
Makefile and in `HANDOFF.md`.

**The filter became a worklist.** `tools/pick_candidates.py` rejects candidates
that load the same pooled constant twice, because that is the constant-CSE
shape. Once the rule above was known, running it with `--allow-repeat` turned
that rejection list into a queue: five whole-file candidates tagged, three
matched immediately with the flag. The tool's docstring now says so, so the
filter is not read as a dead end.

## Functions

| function | address | overlay | note |
|---|---|---|---|
| `OvlFunc_959_20092e0` | `0x020092e0` | rom_7e7574 | corrects a batch-24 claim |
| `OvlFunc_890_2008108` | `0x02008108` | rom_78b2ac | `CSE_CFLAGS`, register-built constant |
| `OvlFunc_935_200848c` | `0x0200848c` | rom_7bf5a8 | `CSE_CFLAGS` |
| `OvlFunc_935_20084d0` | `0x020084d0` | rom_7bf5a8 | `CSE_CFLAGS`, twin of the above |
| `OvlFunc_907_2008240` | `0x02008240` | rom_79b154 | `CSE_CFLAGS`, sibling of batch 25's `OvlFunc_909_200828c` |

## A correction to batch 24

Batch 24 parked `OvlFunc_924_2008ffc` on arg-interleave and concluded that where
r0 is filled in the **middle** of an argument block, neither declaration lever
reaches it. **The conclusion was too strong.** `__Func_809228c` in
`OvlFunc_959_20092e0` has the identical shape —

    rom    mov r2, #0x0 / mov r0, #0x9 / mov r1, #0x0
    ours   mov r2, #0x0 / mov r1, #0x0 / mov r0, #0x9

— and declaring the callee fixes it. So "r0 is not at either end" is not by
itself a blocker. What separates the two cases is open; on the surface the one
that resists has its other arguments coming from pool loads rather than plain
`mov`s, which is an observation and not an explanation. Both files carry the
correction.

## Two smaller tells

**Three consecutive conditional branches to a COMMON exit label are one `&&`
chain**, not three separate `if`s. The two `rom_7bf5a8` functions only match
written as short-circuit evaluation; as separate `if`s the control flow is
identical and the instruction stream is not.

**A flag change requires `make clean`.** make tracks timestamps, not command
lines, so reverting the global-flag experiment left stale objects and
`make compare` reported three overlays failing that had nothing to do with the
current tree. `tools/asmfacts.py --orphans` does not catch this — it verifies
sources exist, not that objects are current. Now in `docs/elevation.md`.

## Counts

258 functions elevated in total. 3,041 hand-written functions remain in `asm/`
of 5,714. 95 parked functions, plus 4 files documenting blocker classes.
