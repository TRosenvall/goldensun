# Batch 223 — cashing in the demoted filter

Seven functions elevated, three of them candidates the previous batch made
visible by demoting a wrong hard reject. Clean `make clean && make compare`
green, SHA1 `5c4695205413df7db52b9a184815a07783999971`.

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_944_20084b0` | `0x020084b0` | [ovl_30_c_c_a_c_c_a_c_b.c](src/overlays/rom_7ca63c/ovl_30_c_c_a_c_c_a_c_b.c) |
| 2 | `Func_8097384` | `0x08097384` | [rom_97384_a.c](src/rom_8a000/rom_97384_a.c) |
| 3 | `OvlFunc_941_2009760` | `0x02009760` | [ovl_30_c_c_c_c_c_a_c.c](src/overlays/rom_7c5efc/ovl_30_c_c_c_c_c_a_c.c) |
| 4 | `OvlFunc_959_200d324` | `0x0200d324` | [ovl_9dc_c_c_c_a_a_c_b.c](src/overlays/rom_7e7574/ovl_9dc_c_c_c_a_a_c_b.c) |
| 5 | `OvlFunc_948_200a334` | `0x0200a334` | [ovl_30_…_c_c_b.c](src/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_b.c) |
| 6 | `OvlFunc_910_20081e4` | `0x020081e4` | [ovl_30_c_c_c_c_a_a.c](src/overlays/rom_79dd90/ovl_30_c_c_c_c_a_a.c) |
| 7 | `OvlFunc_883_200acb0` | `0x0200acb0` | [ovl_30_…_a_a_a_b.c](src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_a_a_b.c) |

Three of these — `OvlFunc_944_20084b0`, `Func_8097384` and
`OvlFunc_948_200a334` — were **invisible to the candidate filter** until batch
222 demoted the duplicate-constant hard reject. All three matched, and
`OvlFunc_948_200a334`'s shape shows why the reject was wrong: its ROM prologue
is `push {r5, lr}`, and the single genuine CSE victim is a two-instruction
constant at two adjacent call sites — the cheapest possible instance of the
class the filter was refusing outright.

## A new mechanism: gcse's cprop kills a held constant

`add rD, rN, #k` off a pooled constant is a **positive tell for a named local**.
`cse.c` relates two `CONST` values only through `get_related_value`, which needs
a `SYMBOL_REF`, so two independent `CONST_INT`s can never produce that add. When
the ROM does `ldr r5, =0x1c45 / mov r0, r5` and then `add r0, r5, #6` a hundred
instructions later, the source held one value in a variable.

**But a plain `int m` does not survive -O2.** gcse's cprop substitutes the
constant into the later block and cse folds `m + 6` to `0x1c4b`, giving two
independent pool loads and no held register — 228 of 237 encodings wrong.
Isolated on a four-line probe, the fold fires **only when the function has more
than one basic block**, because `gcse.c` bails at `n_basic_blocks <= 1`. That is
why it never appears on small specimens.

`register int m __asm__("r5")` is the cure and needs no build change: cprop will
not substitute a constant into a hard register. `GCSE_CFLAGS` with a plain `int`
also matches but changes flags for the whole translation unit.

## The pin rules gained a boundary and lost a ranking

**"Pin the first use" holds only when a branch separates the sites.** On
`OvlFunc_948_200a334` two `__StartTask` calls build the same constant *adjacent
in one basic block*, and both need pinning — first only leaves 2 differing,
second only leaves 15. With the first written straight into r1 the second still
finds a live pseudo.

**The one-statement fill is not better, it is different.** Batch 222 recorded it
as a way to avoid a scheduling barrier. Measured worse twice here: 25 differing
on `OvlFunc_941_2009760` and 11 on `OvlFunc_948_200a334` — the latter identical
to no pins at all — because both ROMs emit the `lsl` or the slot `mov`
mid-group, which only per-instruction statements can express. Look at where the
ROM puts the shift, then pick the form.

**And "N pins is a size, not a set" got its clearest small case.** On
`OvlFunc_959_200d324`, `0x86 << 2` is used at two sites; each pin is
*individually inert* and the pair is *not jointly removable* — dropping both
costs 36 differing and shifts every later relocation by four bytes. Exactly one
must survive, and **both choices are byte-exact**: keeping the first and keeping
the second each give 332 bytes and 129 identical encodings.

## The symbol trap, caught in the act

On `OvlFunc_883_200acb0`, spelling the message base as `(int)&_MSG_1c45` gives
**231 lines and one instruction differing** by tryc — a better score than most
rejected spellings. `objcmp` shows the candidate carrying an `R_ARM_ABS32` the
reference does not have. The reference's only relocation in that function's
range is `iwram_3001ebc`; every pooled id in it is a bare literal.

This is the rule from batch 222 paying for itself immediately: **a symbol
hypothesis is settled by relocations, not by line count.** The sibling
`OvlFunc_883_200af14` had to make the same judgement in the opposite direction
last batch, so the two files together show both answers.

## Other findings

**The element-type fold lever has a second form.** `(&iwram_3001ebc)[5]` folds
the offset into the pool word and then pays a `sub` to get back to the base;
declaring the symbol as an array and writing `iwram_3001ebc[5]` keeps the base
bare and reaches both globals with immediate offsets, as the ROM does. So it is
not only byte-offset versus subscript — address-of-a-scalar versus a declared
array does it too, with the same tell: **a folded `=sym+offset` where the ROM
has a bare symbol, and a length one instruction over.**

**Block layout is source order, and `goto` loses it.** Writing a flag ladder as
`if (cond == 0) goto L;` lets jump optimisation invert a later test and pull its
block up as the fallthrough. Nested `if`/`else` expands in the ROM's order.

**A byte store must not go through a named local** when the offset is past the
5-bit range: `p = GetActor(n); p[0x55] = 0;` gives `p + 0x55` its own pseudo and
Thumb's destructive `add` forces a `mov` at every site. Calling through the
return value lets the add happen in r0 in place — the whole length gap on
`OvlFunc_948_200a334`.

## New in the tree

**`label.sym`**, INCLUDEd from `stage1.ld`. Hand-written asm names its data
tables with assembler-local `.L` labels, which are not C identifiers; each
overlay already solved this in its own `overlay.ld`, and code linking through
`stage1.ld` had nowhere to put such an assignment. `Func_8097384` needed one.
Absolute assignments emit no bytes, so a wrong name costs nothing and a wrong
target fails to link.

**`tools/guard_generated.sh`.** The generated-`.s` check was an ad-hoc shell
loop retyped per commit, and once it was *chained* to the `git commit` so it
printed its warning and the commit went through anyway. It is a tool now, with
`--fix` to unstage, and both paths were tested against a real generated file.
It earned itself immediately: it fired on three commits in this batch — every
one a single-function `.s` converted whole, where the build regenerates a file
at the same path and `git add` stages a modification instead of a deletion.

## Discipline

Every commit ran the guard as its own step and none needed an amend. The one
time the tool refused, the `--fix` path handled it and the commit followed
cleanly — which is the whole point of it not being joined to the commit.
