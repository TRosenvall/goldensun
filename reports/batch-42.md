# Batch 42 — reading the compiler, and sweeping two axes instead of one

*Status: ready to port.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–41 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare` — 96 overlays compared
byte-for-byte and `goldensun.gba: OK`. Every address read back from the linked
ELF with `nm`, and the new symbol from `stage1.o`. **A clean build was required
rather than incremental: this batch adds three per-file compiler flag rules, and
`make` tracks timestamps, not command lines.**

## The rematerialisation question is settled, by reading `local-alloc.c`

Three parks were filed as sharing "one missing construct" — a way to make gcc
rebuild a constant at its use instead of keeping it live. **The construct does
not exist.** `update_equiv_regs`:

    if (REG_N_REFS (regno) == 2
        && REG_BASIC_BLOCK (regno) < 0
        && rtx_equal_p (XEXP (note, 0), SET_SRC (set)))
      reg_equiv_replace[regno] = 1;

with gcc's own comment: *"If the register is only used in one basic block, this
can't succeed or combine would have done it."*

Those two conditions predict **every** limit the basic-block lever was found to
have, each of which cost a park to discover:

| observed over six rounds | which condition |
|---|---|
| separate locals work, one local used twice does not | `REG_N_REFS == 2` |
| a branch is required | `REG_BASIC_BLOCK < 0` is set only when the pseudo spans >1 block |
| straight-line can never use it | one block, so never GLOBAL; calls do not split blocks |
| it fails inside a loop | `REG_N_REFS` is weighted by `loop_depth + 1` |

**The straight-line case is unreachable, not unsolved.** The condition is about
the control-flow graph, not the source. That settles
`src/non_matching/ovl_77dd1c/200c5b8.c`,
`src/non_matching/ovl_7c7b9c/200c218.c`, and the `-1` triple in
`src/non_matching/ovl_787e04/20093e4.c`, and it changes what
[fakematch-worklist.md](fakematch-worklist.md) is: those are not a debt better C
will pay off.

**The compiler source is at `/opt/camelot-gcc/gcc-2.96/gcc/` in the build image.**
Ten minutes of reading against roughly six rounds of probing.

## Flags and source forms are two axes; we were sweeping them separately

`tools/rank_parks.py --flags` screens every park under every per-file build
setting the tree uses. **Its first run unparked two functions.**

`OvlFunc_939_20087f4` was parked in batch 32 with four source formulations and
five flags recorded. It matches at **`-O1` with the parked C unchanged.** `--O1`
had been tried on formulations 1, 3 and 4 — never on 2, because by the time 2 was
written the flags had already been ruled out on the others.

Each axis was swept at the moment of parking and never again. **A park written
before a per-file rule existed is never revisited by anything.** Run `--flags`
whenever a new rule is added; that is exactly when the parked set may have
quietly become solvable.

## The `-O1` rules are not all the same case

The Makefile's existing `-O1` rules say they verify *"only at -O1 (equivalently
`-O2 -fno-schedule-insns2`)"*. `OvlFunc_917_20092b4` is a **counter-example**: at
`-O2` gcc cross-jumps its two `bl __Func_8091254` calls into one shared tail,
`-fno-schedule-insns2` leaves it seven positions out, and only real `-O1`
matches. Cross-jumping is a jump-pass decision, not a scheduling one. The new
rules state which case each is.

## Functions

| function | address | overlay | build |
|---|---|---|---|
| `OvlFunc_936_2009858` | `0x02009858` | rom_7c097c | default |
| `OvlFunc_891_2008054` | `0x02008054` | rom_78c76c | default |
| `OvlFunc_917_20092b4` | `0x020092b4` | rom_7a4370 | **`-O1`** (cross-jump) |
| `OvlFunc_933_2008c38` | `0x02008c38` | rom_7bc690 | **`-O1`** (two pool loads) |
| `OvlFunc_939_20087f4` | `0x020087f4` | rom_7c460c | **`-O1`**, unparked |
| `Func_80cd488` | `0x080cd488` | main ROM | **`CSE_CFLAGS`**, unparked |

One symbol added: `_AREA_35`.

### Two smaller findings worth having

**A lone trailing `mov r0, #imm` the ROM lacks is a return-type question.**
`Func_80cd488` was parked as `unsigned int … return 0;`; the ROM never sets `r0`,
so it returns nothing. That difference is one instruction and reads exactly like
codegen noise.

**Pointer arithmetic: the ROM's `add` says which form to write.** A destructive
two-operand `add r3, r2` wants a walk (`p = base; p += off;`); a three-operand
`add r3, r6, r2` wants one expression (`p = base + off;`). Getting it backwards
cost six positions on `OvlFunc_923_20091b4` — and **both forms are right in that
one function**, for two different pointers.

## For your attention: `CSE_CFLAGS` now has a main-ROM member

`Func_80cd488` is the **eighth** TU on `-fno-rerun-cse-after-loop` and the first
outside an overlay. HANDOFF carries a standing question about whether that flag
reflects the original build or a gcc-2.96 difference; every prior instance being
in an overlay was itself weak evidence for the compiler-difference reading, and
it no longer is.

## Counts

361 functions elevated in total, of which 7 are fakematches. 2,934 hand-written
functions remain in `asm/` of 5,714. 90 parked functions and the two
large-function experiments.
