# Batch 64 — a tooling bug, a duplicate census, and a blocker that fell after being called unreachable

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.
3047 sources checked, every elevated `.c` has a tracked `.s`; 0 orphans.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_8079754` | `08079754` | main ROM | [rom_79460_c_c_a_a_a_b.c](../src/rom_77000/rom_79460_c_c_a_a_a_b.c) |
| `GetSpriteVoiceEntry` | `08091560` | main ROM | [rom_8d9a4_c_c_c_c_c_a.c](../src/rom_8a000/rom_8d9a4_c_c_c_c_c_a.c) |
| `OvlFunc_919_200826c` | `0200826c` | ovl_7a67d8 | [ovl_30_c_a_c_b.c](../src/overlays/rom_7a67d8/ovl_30_c_a_c_b.c) |
| `OvlFunc_970_2008168` | `02008168` | ovl_7fa4ec | [ovl_30_c_c_c_a_a_b.c](../src/overlays/rom_7fa4ec/ovl_30_c_c_c_a_a_b.c) |
| `OvlFunc_927_20089f4` | `020089f4` | ovl_7b4558 | [ovl_30_a_a_c_c_c_c_b.c](../src/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c_b.c) |
| `OvlFunc_946_20089f4` | `020089f4` | ovl_7ced6c | [ovl_30_a_a_c_c_c_b.c](../src/overlays/rom_7ced6c/ovl_30_a_a_c_c_c_b.c) |
| `OvlFunc_964_20089f4` | `020089f4` | ovl_7ed0a0 | [ovl_30_a_a_a_c_c_c_c_b.c](../src/overlays/rom_7ed0a0/ovl_30_a_a_a_c_c_c_c_b.c) |
| `OvlFunc_965_20089f4` | `020089f4` | ovl_7ef4f4 | [ovl_30_a_a_a_c_c_c_c_b.c](../src/overlays/rom_7ef4f4/ovl_30_a_a_a_c_c_c_c_b.c) |

**The last four are one function elevated four times** — operand-identical copies
in four overlays, one source with the name changed.

## A tooling bug hid the easiest candidates for many rounds

`tools/pool_candidates.py` required at least one call (`if not calls or ...`) and
so **silently excluded every leaf function**. Leaves are the best candidates
available: no callee signature to guess wrong, and the argument-precompute
blocker cannot apply because there are no arguments to set up.

**123 unparked leaves, 12–40 instructions, had never been screened.** Fixing the
condition took the candidate pool from 44 to 78. Three of this batch's
elevations came straight out of it, two of them matching on the first screen
with no lever at all.

The lesson is not about leaves. It is that a filtered list had been read for many
rounds without anyone checking what the filter dropped, and two rounds were
reported as "the pool is picked over" when the pool was missing its easiest
third.

## A blocker that fell after being recorded as unreachable

Three parks recorded that basic-block placement is decided after the source has
had its say and cannot be reached from C:

```
rom   cmp r5, #0 / beq .Lzero      <the body>   mov r0, r5 / b .Lexit
      .Lzero: mov r0, #0
ours  cmp r5, #0 / beq .Lzero / mov r0, #0 ...  <- emitted at the guard
```

What had actually been tested was a `goto` spelling of the same early-return
shape — which changes nothing, because both put the return at the guard. **The
edit that works is inverting the guard so the body is the taken branch:**

```c
if (p != 0) {
    <body>
    return p;
}
return 0;
```

Identical control flow, opposite layout, and the layout is the ROM's. That
elevated four `__CreateActor` wrappers at once.

**It is not universal**, and the limit was measured rather than assumed:
`OvlFunc_945_20080fc` has a comparison chain on one side of its guard instead of
a single return, and inverting there leaves it at 19 of 28. A result variable
with a single exit does not work either — 11 of 41. The lever wants a **short
return block on one side and the bulk of the function on the other**.

## The remaining corpus is duplicated, and the first count of it was wrong

`twin_finder.py` was extended to report clusters of unelevated twins. The first
report of this — 102 functions in 12 clusters — **implied one solution would port
to all of them, and that was wrong.** A signature match is the same opcode
sequence; operands may differ. Measured across the 118 shape-matched functions
in clusters of four or more:

| | count |
|---|---|
| **EXACT** — operand-identical, ports verbatim | **15** (3 shapes) |
| **SHAPE only** — same opcodes, different constants | 103 |

The 18×172, 17×139 and 17×132 clusters contain **no two identical functions**.
Reporting the shape count alone overstates the free work by nearly 8×. The tool
now computes both and labels every cluster, so the error cannot be repeated by
reading its output.

Shape-only clusters are still worth real money — one `.c` ports with the
constants substituted, which is how `OvlFunc_916_200836c` was elevated from
`OvlFunc_947_2009578` in batch 63. The word that changed is *free*, not
*valuable*.

## Where the blocked work is concentrated

Duplicate-aware selection, not the candidate list, found all of these:

| functions | blocker | status |
|---|---|---|
| 7 | one hoisted load, scheduler-related | **open** — operand-identical, so one fix ports verbatim; the highest-value park in the corpus |
| 4 | one register naming in the `__CreateActor` wrappers | **open** — was 8 before the guard-inversion lever took half of them |
| 12 | argument precompute, `calls.c:805` | closed — compiler difference, not fixable from C |

The seven-function cluster was **parked twice independently** before the
duplication was noticed, in different batches, both concluding "one hoisted load"
by different routes. They are now cross-linked.

## `dma.h` was never a blocker

Five parks were filed under "`include/dma.h` register binding" in batches 54–55.
Re-screening all five shows **no park is held by it**. `DMA3_SET(&buf, d, cnt)`
reproduces the ROM's `stmia r3!, {r0, r1, r2} / sub r3, #0xc` exactly, with the
halfword staged at `sp+2` by a plain local. The class is retired; reach for the
header rather than treating it as a known-lost cause.

## Levers added this batch

- **Assign back into the parameter** when the ROM's load is destructive on an
  argument register (`ldr r0, [r0, #0x50]`). A fresh local gets a fresh
  register and, under pressure, a callee-saved one. `Func_800c570`: 8 of 21 → 1.
- **A plain `return K;` per path**, not a result variable, when several paths
  return different constants. gcc merges the epilogues itself.
  `Func_80bf3bc`: 8 of 31 → 2.
- **Invert the guard** (above).

## Corrections made this batch

Four claims were retracted after measurement, all of them originally mine:

- "Register-pressure explains the elided copy in the `Func_80bf*` family" —
  **false**. All four siblings elide it, including one with strictly more
  pressure. It is a plain codegen difference.
- "Load-then-copy is a blocker signature, 508 functions" — **a size artifact**.
  Controlling for length, elevated and unelevated rates are 6% and 8%.
- "102 functions port from 12 cluster solutions" — **15 do**.
- "Basic-block placement cannot be reached from C" — **it can**, see above.

The common thread is reporting the first plausible measurement before running
the control. The tooling changes in this batch — labelled cluster kinds, the
measured-accuracy docstring on the precompute predictor — are meant to make that
structurally harder rather than relying on remembering to check.
