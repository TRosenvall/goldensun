# Batch 168

Six elevated. Verified after a clean `make clean && make compare`; SHA1
`5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
linked images.

## The functions

| function | address | image | ratio | what closed it |
| --- | --- | --- | --- | --- |
| `OvlFunc_969_2008518` | 0x02008518 | rom_7f6e64 | 0.978 | first screen, no iteration |
| `Func_80990cc` | 0x080990cc | goldensun.elf | 0.891 | first screen, no iteration |
| `Func_8078320` | 0x08078320 | goldensun.elf | 0.906 | `SCHED2_CFLAGS` |
| `Func_80782a0` | 0x080782a0 | goldensun.elf | 0.897 | shift-pair spelling |
| `OvlFunc_918_20095ac` | 0x020095ac | rom_7a5214 | 0.901 | `ALIAS_CFLAGS` |
| `Func_80aac84` | 0x080aac84 | goldensun.elf | 0.908 | `goto`-loop rewrite |

Every one came off `tools/fuzzy_solved.py`, which batch 167 introduced. Two
needed no iteration at all; the other four each stopped on a single named
mechanism.

## `Func_80aac84` settles the `goto`-loop discriminator

Batch 167 left an open contradiction: the `goto`-loop rewrite took
`Func_8090584` from 95 differing to 3, and took `Func_80bac6c` from 34 to 51.
This batch resolves it.

`Func_80aac84` screened at 74 lines against 77 with **71 differing**, because
gcc had both **reversed** the inner loop and **strength-reduced** the address,
where the ROM counts up, keeps the base in r14 and recomputes `base + j` and
`idx * 2` every iteration. The rewrite took it to 77 lines and 2 differing; a
statement-order swap closed it.

| function | what gcc did to the loop | `goto` rewrite |
| --- | --- | --- |
| `Func_8090584` | hoisted a pointer, two masks, a base, three store values | 95 → 3 |
| `Func_80aac84` | reversed it **and** strength-reduced the address | 71 → 2 |
| `Func_80bac6c` | hoisted one constant, one induction variable | 34 → 51 |

> **The rewrite pays when gcc applied a loop TRANSFORMATION — reversal or
> strength reduction — not merely a hoist.** A transformation restructures the
> whole body, so disabling loop optimisation recovers many instructions at once.
> A single hoisted constant is cheaper to live with than the rewrite's overhead.

Two corollaries held: with hoisting off, the ROM's `mov r7, #0x1f` before the
outer loop is an explicit `int mask = 31;` and not gcc lifting `& 31` out; and
the order of the two initialisers becomes observable — the counter before the
base, or 2 differing.

## A recorded warning, corrected

`-fno-schedule-insns2` is on record as "an actively misleading probe", from a
round where it pushed the first difference back to instruction 1 and multiplied
the count on every function tried (3 → 15, 17 → 41, 8 → 25, 2 → 23).

`Func_8078320` is the opposite case, and the warning as written would have
stopped the elevation. Its residue was three lines — a `strh` and the `ldrsh`
after it in the wrong order — and the flag closed it outright with nothing else
disturbed. So the rule is now a **signature**, not a verdict on the flag:

* first difference jumps toward instruction 1 and the count multiplies → the
  flag is destroying the evidence; put it down.
* the residue closes with nothing else moving → post-reload scheduling really
  was the difference.

Its sibling `Func_80782a0` shows the other half: the same flag took that residue
from 5 differing to 4 and did **not** close it, and the real fix was a source
spelling. **A flag that improves but does not close a small residue is still the
wrong answer.**

## `x <<= 16; x >>= 2;` is not `(short)x << 14`

That sibling's fix is worth its own line. The ROM widens a just-stored halfword
in place. Written as a cast, gcc computes the shift into a second register and
hoists it above the store. Written as two shift statements on an `int`, it
matches. The cast asks for a value; the shift pair asks for an operation on a
register, and only the second gets the in-place form.

## The per-use-site-locals lever needs a dominating boundary

`OvlFunc_948_200938c` and `_200949c` (parked together, same cutscene over
different slots) each call `__MapActor_SetSpeed` twice with the same pooled
pair. The ROM reloads at each site; gcc hoists into r5/r6, which shows in the
prologue as `push {r5, r6, r14}` against `push {r14}`.

The recorded remedy is separate locals per use site. Copying its worked instance
here is **exactly inert** — byte for byte identical to plain literals — and
`-fno-rerun-cse-after-loop`, `-fno-gcse` and `-fno-cse-follow-jumps` are inert
too.

The difference is guard placement: the working instance has three `if` blocks
*between* the assignments and the uses; these two have their only guard *after*
both calls. **So "separate locals per use site" is the dominating-block
mechanism under another name**, and it joins the argument interleave and
constant-CSE levers in needing a branch to rematerialise across.

## Two build-discipline notes

**`split_s.py` refusing on local labels is a two-step.** Splitting
`ovl_314_c_c_c.s` was refused because two `.L` labels would cross files. Export
first, verify `make compare` on the export *alone*, then split. A `.global`
emits no bytes, so the export is provably byte-neutral by itself; done together
with the split, a layout mistake and a bad export are indistinguishable.

**`AGBCC_DIR=/opt/agbcc` is required in the container, and not for the reason it
looks like.** It selects no compiler for any elevated function — every one
builds with gcc-2.96 — and feeds only five library objects built with
`old_agbcc`. But the Makefile's default `tools/agbcc` resolves to the
checked-in *macOS* Mach-O binary, which the container cannot exec; it fails with
`Syntax error: "(" unexpected`, which reads like a corrupted toolchain rather
than a wrong path.

## Where the leads stand

`tools/fuzzy_solved.py` is down to **17** remaining above 0.80 from the original
26. Batches 167 and 168 took nine of them, at roughly one to three screens each.
