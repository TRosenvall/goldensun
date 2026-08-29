# Batch 63 — two blockers measured, and a park that was wrong for eight batches

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.
3019 sources checked, every elevated `.c` has a tracked `.s`; 0 orphans.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_8020088` | `08020088` | main ROM | [rom_1fe2c_c_b.c](../src/rom_15000/rom_1fe2c_c_b.c) |
| `Func_80958e4` | `080958e4` | main ROM | [rom_944ec_a_c_a_a_a_c.c](../src/rom_8a000/rom_944ec_a_c_a_a_a_c.c) |
| `OvlFunc_916_200836c` | `0200836c` | ovl_7a37f0 | [ovl_30_c_c_c_a_a_b.c](../src/overlays/rom_7a37f0/ovl_30_c_c_c_a_a_b.c) |
| `OvlFunc_931_2008c0c` | `02008c0c` | ovl_7b8cb0 | [ovl_30_c_c_c_c_c_c_c_c_c_c_b.c](../src/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_b.c) |
| `OvlFunc_947_2009578` | `02009578` | ovl_7d0e88 | [ovl_1528_a_a_a_c_c.c](../src/overlays/rom_7d0e88/ovl_1528_a_a_a_c_c.c) |

**Four of the five cost almost nothing.** Three were solved by copying work
already in the repository, and one is an unpark. Only `Func_80958e4` needed
anything derived.

## A park that was wrong, and why that matters more than the elevation

`OvlFunc_931_2008c0c` sat in `src/non_matching/` from **batch 53** at 1 of 24.
Its note recorded the fix as tried and rejected:

> the zero named and reused: `v = 0; p[0x55] = v; v -= 0xd;`
> — the genuine reuse lever, and the shape the ROM appears to show —
> **2 (WORSE: it also moves the store at position 3)**

The spelling was right. The **statement order** was not. The subtraction has to
come after the two intervening loads:

```c
z = 0;
*p = z;
c = *(unsigned char **)(a + 0x50);   /* these two loads sit between */
t = c[9];                            /*                             */
z = z - 0xd;
```

Written immediately after the store, gcc also moves the store, and the result is
worse than doing nothing — which is exactly what the note observed, and exactly
the wrong conclusion to draw from it.

**The general lesson: a park note that rules out a lever must say WHERE in the
statement sequence it was placed.** "Tried and worse" about a spelling can be
true while the same spelling three lines later is exact. Eight batches were lost
to that ambiguity in this one file, and the note is otherwise well written — this
is not a sloppiness problem, it is a missing field.

The function was found by [tools/twin_finder.py](../tools/twin_finder.py), below.

## Duplicate code across overlays is real, and rarer than it looks

`OvlFunc_916_200836c` and `OvlFunc_947_2009578` are **the same function compiled
into two overlays** — same instruction sequence, different VCOUNT bound and
different scroll tables. The second was elevated by taking the first's source
and substituting three constants.

That looked like a bulk lever, so it was made into a tool:
`twin_finder.py` builds an opcode signature for every function and reports
unelevated or parked ones matching an already-elevated signature.

**Its measured yield is one.** Across 1620 elevated and 2685 unelevated
functions, exact signature matching found exactly one hit — the
`OvlFunc_931_2008c0c` unpark — and consuming it left zero. That number is in the
docstring so nobody expects a backlog.

It is still worth keeping: it costs seconds to run, and *every batch that
elevates something can create a new twin*. The docstring says to re-run it per
batch rather than treat it as a one-off sweep.

## The pool tell blocks 271 functions

The pool tell has been a documented lever for many batches: when the ROM spends
a literal-pool word on a constant below `0x100`, where `mov #imm8` or
`cmp #imm8` would do, the operand was a **symbol** whose value happens to be
small. What was never measured is how much it blocks.

**271 unelevated functions load a constant below `0x100` from the pool.**
Commonest values: `0x0` (54 functions), `0x2` (21), `0x1f` (19), `0x1` (15),
`0x75` (13).

The `=0` cases are the least ambiguous — `mov rN, #0` is always available, so a
PC-relative load of zero is never a compiler choice. These are disassembled ROM
bytes rather than our output; the disassembler prints `ldr r3, =0` because the
encoded instruction really is a PC-relative load.

**Stated as an upper bound.** The count is of functions containing at least one
such load, not functions blocked *only* by it; some have other blockers, and it
mixes Thumb and ARM-mode common code.

Even discounted, this is **the largest identified blocker in the corpus** —
larger than argument precompute (11 functions, batch 62). Unlike that one it is
fixable, but not by this effort: the fix is to name the symbols, and naming has
been deliberately deferred throughout. **It is a maintainer's call, and it is the
highest-leverage decision available.** Two worked examples with the evidence
written out: [`OvlFunc_952_2008070`](../src/non_matching/ovl_7d768c/2008070.c)
(pooled `0x8b`) and its sibling `OvlFunc_963_200808c` (pooled `0xaa` and `0xa9`).

## Register-pressure residue, consolidated

Six parks had independently reached the same conclusion, so it is now one
HANDOFF.md entry rather than six per-file notes. **What registers the ROM uses is
a consequence of pressure in the original translation unit, not of how the C is
written.** Three shapes recur: an elided copy, a dead callee-saved register, and
a constant hoisted or not hoisted out of a loop.

**The diagnostic that settles it is to find the near-twin that DOES match.**
`Func_80bf54c` and `Func_80bf574` are the same shape; the two-named-locals
spelling produces the copy in one and not the other, and the difference is that
`Func_80bf574` has a second store keeping more values live. Likewise
`Func_80a9cbc` matches and `Func_80a9d84`, identical but for a third constant,
does not.

**The planning consequence:** these are most likely to fall out for free once
more of the surrounding TU is elevated. They should be re-screened *after* their
neighbours, not retried now. The declaration lever, statement reordering, extra
named locals and the derived-initialiser lever have all been tried across all
six and are byte-identical to the default.

## Levers confirmed transferable

`Func_8020088` is a structural twin of `Func_801ff14` from batch 62 and matched
on the **first screen** by transferring that batch's lever unchanged: the
walking *offset* is the pointer-typed variable and the loaded base is a plain
`unsigned int`, which is what puts the offset first in `ldr r0, [r5, r7]`. A
lever proven on one function is worth trying verbatim on its shape-mates.

`Func_80958e4` needed **both halves** of the pointer lever in one function: a
derived initialiser for the pointer the ROM copies (`f = p + 0x9d` →
`mov r6, r5 / add r6, #0x9d`) and a destructive `+=` for the one it walks in
place.

## Splitting again: a `.L` label in `.rodata` needs `.global`

A *second* split of an already-split file can separate a function from the
`.rodata` it references. Local labels do not cross object boundaries:

```
stage1.o: in function `Func_801ffd8':
(rom_15000+0xb080): undefined reference to `.L73854'
```

That reads like a missing file rather than a scoping problem. Add `.global` to
the label in the piece that defines it — already the project's convention
(`ovl_30_c_c.s` carries `.global .L5238`). Symbol binding is link-time metadata,
so the emitted bytes do not change and `make compare` still passes.

## A parked function whose blocker is loop-invariant hoisting

`Func_80a9d84` is the twin of the *matching* `Func_80a9cbc` plus a third
constant. gcc hoists all three loop-invariant constants; the ROM hoists two and
materialises the last inside the loop. That costs a second high register and
four extra prologue/epilogue instructions — 14 of 30, on a function whose twin
is exact.

Naming the constant or inlining it is byte-identical, and
`-fno-rerun-cse-after-loop`, `-fno-strength-reduce` and `-fno-thread-jumps` are
all byte-identical to the default. Loop-invariant motion has no flag of its own
in this compiler, and a constant stored every iteration is invariant however it
is spelled.
