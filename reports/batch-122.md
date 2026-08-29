# Batch 122 — two blocker classes read out of the compiler, and five build-system bugs

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`. All 21
symbols read back out of `goldensun.elf` / the linked overlay ELFs.

**21 elevated, 4 parked. 2296 → 2275 remaining.**

## A blocker class is now decidable from the ref, without writing any C

The signed-lower-bound residue has been recorded for many batches as "no
spelling reaches it". The mechanism is now read out of `combine.c`'s
`simplify_comparison` (gcc-2.96, ~lines 10150–10180), which rewrites
unconditionally for `MODE_INT`:

    LT C (C>0) -> LE (C-1)        GE C (C>0) -> GT (C-1)
    LE C (C>0) -> unchanged       GT C (C>0) -> unchanged

So for **K > 0**, gcc-2.96 can emit `cmp #K / ble` and `cmp #K / bgt`, and can
**never** emit `cmp #K / blt` or `cmp #K / bge`. The ARM hook
`arm_canonicalize_comparison` is not involved — it fires only for constants that
are not ARM-encodable, which never happens for these bounds.

> Grep the ref for `cmp rN, #K` followed by `bge`/`blt` with K > 0. **Each site
> is a hard floor of two instructions**, known before the first screen.

## `goto` loops disable loop optimisation entirely

The doc had "write the control flow with `goto`" filed as a shape-matching
trick. The mechanism is much larger: `loop_optimize` is driven by
`NOTE_INSN_LOOP_BEG`/`END` notes that `stmt.c` emits **only** for `while`, `for`
and `do`. A backward `goto` emits none, so loop-invariant hoisting, strength
reduction and `check_dbra_loop` counter reversal all switch off together.

`Func_8090584` was **95 differing of 99** — gcc had hoisted a state pointer, two
masks, a base address and three store values out of the loop — and no flag
touched it: `-fno-gcse`, `-fno-rerun-cse-after-loop`, `-fno-move-all-movables`,
`-fno-strength-reduce` and `-fno-expensive-optimizations` all left it at 95. The
`goto` rewrite alone took it to **3**, then to exact.

> If the ROM rebuilds a loop-invariant constant inside the loop body, the
> source's loop was not a `while`/`for`.

**Corollary that inverts a reading:** once hoisting is off, anything set up in a
register *before* the loop had to be written there. A `mov r7, #0x1f` in the
prologue of a `goto`-loop function is `int mask = 0x1f;` in the source, not gcc
hoisting `& 0x1f`.

## Five build-system bugs, three of them holding functions hostage

**Three more mis-scoped `O1_CFLAGS` wildcards**, found by a systematic sweep of
every pattern rule against its actual membership. `OvlFunc_968_2009644`,
`OvlFunc_965_200a46c` and `OvlFunc_965_200a5c8` were **parked with full
measurement tables taken at the wrong optimisation level** — 5 of 39, 2 of 30
and 17 of 55 at `-O1`, and all three exact at `-O2`. Their TUs sit inside a
wildcard they merely share a filename prefix with.

That is **five mis-scoped wildcards this session** across three overlays. The
sweep also bounds what remains: `CSE_CFLAGS`, `GCSE_CFLAGS`, `ALIAS_CFLAGS`,
`SCHED2_CFLAGS`, `FIXEDR7_CFLAGS` and `STRENGTH_CFLAGS` are on **explicit
targets only** — zero wildcards. The residual exposure is **12 not-yet-elevated
`.s` TUs sitting inside a wildcard**, which inherit its flag the moment anyone
elevates them. Two clean negatives came with it, which is why the positives are
trustworthy: one wildcard where `-O1` is genuinely right (9 of 56 at `-O1`, 42 at
`-O2`), and the `common2` no-interwork group.

**`tryc.py` understood only two of the eight flag groups.** `makefile_flags()`
knew `O1_CFLAGS` and `COMMON2_CFLAGS`; the six newer groups were invisible, so a
screen against a TU carrying one ran at plain `-O2` with no warning — the same
failure the O1 handling exists to prevent. Fixed. Exposure was bounded today
because those six are all on explicit targets, but the next one added as a
wildcard would have reopened it.

**Five overlay linker scripts were missing `__divsi3 = _divsi3_RAM;`** while
already exporting `divsi3_RAM` from their own `imports.s`. Two functions were
byte-exact apart from that relocation's symbol name.

## A miscompile that reads as harmless label noise

`__asm__(".LNN")` on an `extern` collides with gcc's own local labels. One
function referenced `.L3`; gcc numbers that TU's branch targets `.L1`–`.L7`, so
the reference bound to a **branch target inside the function**. The screen shows
it as ten off-by-one label mismatches, not as a symbol error.

The asm-label extension is only safe when the label number exceeds what gcc will
emit in that TU. `.L36750`, `.L9d7a8`, `.L3c50` are fine; `.L1`, `.L2`, `.L3` are
unusable. Test: re-screen with an out-of-range name and see whether the count
collapses.

## Working by family

`tools/twin_families.py` groups the remaining functions by identical opcode
stream, so one solved `.c` is a template. It produced a four-member DMA family
in one round — including a member parked for several batches whose answer was in
its siblings — and a two-member family whose sibling matched on the first screen
after six constants were substituted.

With the caution it also produced: **a family can be uniformly blocked as easily
as uniformly solvable.** A three-member family calling `f(-1, -1, -1, 0)` is
parked because none of the three has a control-flow boundary, while
`OvlFunc_923_2009208` — same callee, same arguments, same three-locals spelling —
matches, because it has an early return between the assignments and the uses.

## Levers and readings

`gcc-2.96`'s thumb `REG_ALLOC_ORDER` is **`{3,2,1,0,12,14,4,5,6,7,8,10,9,11}`**,
read from `arm.h:989`. So for a call-crossing value the order is r5, r6, r7, r8,
**r10, r9**, r11 — r10 before r9 is normal — and **r12/r14 come before r4**, so
`mov r12, rN` in a ROM is a register-pressure readout, not a special construct.

Other things measured this batch:

* **A parameter and a loop counter can be the same variable.** One function was
  33 of 115 with an r5↔r6 exchange; the ROM gives r5 to a parameter used *once*
  because `allocno_compare` weights by basic-block frequency, and merging it with
  a loop counter multiplies its priority. Reusing it matched exactly.
* **Apply `volatile` at the use site**, not to the declaration — qualifying the
  declaration de-optimised the other reads and came out a line long.
* **The constant-as-destination lever is per-operation**: `orr` wants an
  `unsigned char` local (an `int` is folded away), `and` wants an `int` (an
  `unsigned char` puts the loaded value in the destination). The statement form
  is inert under either.
* **A variable with disjoint live ranges should be two variables** if the ROM
  gives the two regions different registers — the counterpart to "naming one
  level too many costs a register".
