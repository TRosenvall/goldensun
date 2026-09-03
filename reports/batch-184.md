# Batch 184

Five elevated, one parked, and a class the notebook called its most valuable
open question closed for one sub-case — not by another spelling sweep, but by
reading two cost functions.

The round's shape: the `[cse]` marker introduced last batch turned out to be
wrong four times out of five, in four different ways, and each failure named a
distinct mechanism. That is a better yield than the marker being right would
have been.

## Function breakdown

| # | function | address | file | what it took |
|---|---|---|---|---|
| 1 | `Func_801faa8` | `0x0801faa8` | [rom_1de5c_…_a_a.c](src/rom_15000/rom_1de5c_c_c_c_c_a_a_c_c_a_a.c) | left-to-right association; **an initialiser picks `_call_via_rN`** |
| 2 | `OvlFunc_922_2009948` | `0x02009948` | [ovl_30_…_c_a_b.c](src/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_a_c_c_a_b.c) | name both stack args; **sibling arms are not a join** |
| 3 | `Func_80a7380` | `0x080a7380` | [rom_a7380_a_b.c](src/rom_a1000/rom_a7380_a_b.c) | **blocker 1b is the `int` type**; an r2/r3 swap computed, not guessed |
| 4 | `Debug_TransferTest` | `0x0800679c` | [rom_5cf8_c.c](src/rom_c0/rom_5cf8_c.c) | `volatile`; **`goto` to deny LICM**; a `char` frame object |
| 5 | `OvlFunc_951_200973c` | `0x0200973c` | [ovl_30_c_c_c_c_b.c](src/overlays/rom_7d6418/ovl_30_c_c_c_c_b.c) | **the copy after a pool load decides the spelling** |

Parked: `OvlFunc_959_200cf60` (14 of 147).

## CONSTANT CSE INSIDE ONE BASIC BLOCK: CLOSED, WITH A NUMBER

`arm_rtx_costs`, Thumb, `CONST_INT` as a `SET`, returns 0 below 256,
`COSTS_N_INSNS(2)` for a shiftable value at or above 256, `COSTS_N_INSNS(3)`
otherwise. `COSTS_N_INSNS(N)` is `N * 4 - 2` in this tree — so 6 and 10, **not**
8 and 12, which is what the commoner `N * 4` definition gives. `cse.c`'s `COST`
scores a pseudo at **1** and doubles `rtx_cost` for anything else:

| the constant | rtx_cost | CSE COST | against a pseudo's 1 |
|---|---|---|---|
| below 256 | 0 | **0** | constant wins — always rematerialised |
| shiftable, ≥ 256 | 6 | **12** | register wins — always CSEd |
| other, ≥ 256 | 10 | **20** | register wins — always CSEd |

Two consequences. The corpus rule that a bare `mov rN, #imm8` is free and must
not be counted as a repeat now has its mechanism: **the boundary is literally
`< 256`**. And since COST is a property of the `const_int`, and every C spelling
folds to the same `const_int` before cse runs, **within one basic block a
repeated constant of 256 or more is unreachable from C**. Not unfound —
nonexistent. Stop sweeping and park it.

Only that sub-case is closed. Repeats across a boundary with one use dominating
are still the `CSE_CFLAGS` shape; repeats in mutually exclusive arms need no flag
at all; repeats of *different* values are the split lever.

## THE `[cse]` MARKER WAS WRONG FOUR TIMES, USEFULLY

| function | what the marker fired on | what was actually true |
|---|---|---|
| `Func_801faa8` | two message ids | arms that cannot reach one another — no flag |
| `Func_80a7380` | a global's **address** | a pool-loaded base re-dereferenced after calls is never commoned |
| `OvlFunc_951_200973c` | a pooled constant | mutually exclusive arms — and the flag **costs** 3 |
| `OvlFunc_959_200cf60` | the iwram repeats | those reload fine; the constant that blocks is in **one straight-line block** |

The last is the sharpest. The detector finds *a* repeat, not *the* repeat that
matters — and the constant that actually blocked `200cf60` is one its own
"label between the repeats" test would have cleared.

The refinement worth carrying: the `CSE_CFLAGS` shape is a repeated **value
consumed as a register argument**, not a repeated **symbol address that is
immediately dereferenced**, and not repeats in sibling arms.

## BLOCKER 1b IS A CONSTRAINT-ORDERING FACT

This corrects how 1b was explained twice in the previous batch. The recipe was
right; the reason was not, and the wrong reason made it look mysterious.

In the Thumb halfword-move pattern the source constraint list puts an
alternative accepting any `CONST_INT` — which loads from the pool — **before**
the alternative that would emit a `mov`. recog takes the first match, so for a
HImode `const_int` the `mov` alternative is **unreachable**, and every halfword
store of a literal pools. Probed in isolation: storing 1 pools, storing 0 pools,
an `int` local stores with `mov`.

So the escape is **making the value SImode** — an `int` local, set by the word
move and stored through a subreg. It is not "assign it in a dominating block".
That effect is real but is a *separate*, ordinary blocker-2 question about which
register the `int` then lands in. Restated as two steps:

1. Is the stored constant an `int` local? If not it pools, always.
2. *Then* place the assignment to get the ROM's register — and `.17.lreg` will
   tell you what happened.

And check which side you are on first: `OvlFunc_951_200973c` reproduces the
*pooled* form from the plainest spelling, because there the ROM wants it.

## READ THE RIGHT COMPILER TREE

`/opt/gcc296` — what `tryc.py` and the build actually drive — is built from
`/opt/camelot-gcc/gcc-2.96/`. The `agbcc` tree beside it is a **different
compiler**: it does not define `REG_ALLOC_ORDER` at all, and its halfword-move
constraints differ in exactly the way that would have given the wrong answer to
the 1b question above.

Some passes agree — `local-alloc.c`'s `update_equiv_regs` gate is the same rule
in both, at line 886 and 868 — so the `Func_80a5b94` park stands. But do not
assume it. Cite `gcc-2.96`.

This matters because the batch before this one *discovered* the source was
readable and immediately started citing it. Finding the source was right;
assuming the two copies were interchangeable was not.

## LOOP-INVARIANT MOTION IS A PRINTED COST MODEL

`move_movables` hoists only when `threshold * savings * lifetime >= insn_count`.
A global read **once** inside a loop has `lifetime == 1` — its address load and
its use are adjacent — so in a large loop it can never clear the bar, and gcc
rematerialises the pool load at the use. The `-da` `.08.loop` dump prints the
verdict per insn.

So when the ROM holds a global's address in a callee-saved register across a
loop but dereferences it once, **LICM did not put it there; the source did**, as
a pointer local. And when LICM *does* hoist, it inserts a copy, because the
pre-loop and in-loop uses are separate pseudos. **The copy is the tell:**

| ROM shows | means | write |
|---|---|---|
| `ldr rA, =sym` reloaded at each use | LICM declined | a pointer local |
| `ldr rA, =sym` then `mov rB, rA` | LICM hoisted and copied | a bare global |

`OvlFunc_951_200973c` has three globals needing both spellings in one function.

**Two reasons to reach for `goto`, and they pull opposite ways.** The un-rotated
loop shape needs *no* `goto` — `for (init; ; inc)` with a trailing `break`
produces it, `expand_end_loop` carrying a Cygnus-local transform that does the
rewrite. There the `goto` spelling is **worse** (27 against 11), because a
backward `goto` is not a natural loop and gets no invariant motion at all. But
that is exactly why `Debug_TransferTest` needs one: its outer pass must *deny*
hoisting so the address load stays in the inner preheader. Same construct,
opposite jobs; the tell for the second is a pool load the ROM repeats per pass.

## Other mechanisms worth keeping

**An initialiser picks which `_call_via_rN` you get.** `Func_801faa8`'s last
three instructions were the veneer register: ours took r4, the ROM takes r3, and
r3 is first in `REG_ALLOC_ORDER`, so the ROM's pointer has the shorter live
range. Assigning it immediately before the call fixes it — and declaring it in
the same block *with an initialiser* measures 3, identically to assigning it
early, because an initialiser puts the definition at the top of its scope. Same
value, same instruction, one register worse.

**A register swap is arithmetic.** `local-alloc.c` ranks quantities by
`floor_log2(refs) * refs * size / (death - birth)` and says in its own comment
that shorter-lived ones rank higher. The recipe needs both halves: give the
operand you want in the *lower* register its own statement first, then assign
the r3 candidate immediately before its use. Assigning the constant late alone
fails, because the pointer is then born after it and is shorter still.

**A stack-argument slot fed by a literal is one live pseudo; fed by a named
local it is two.** Worth 15 of 20 on `OvlFunc_922_2009948`. But splitting into
per-arm locals did *nothing* there — five disjoint pairs and one shared pair
compile byte-identically, because sibling arms of an else-if chain already
decompose into non-overlapping webs. **The batch-182 split needs the shared
range to span a join; sibling arms do not count.**

**`volatile` on a polled input global** fixed both a register assignment and a
read count at once on `Debug_TransferTest`, 24 aligned to 6, after plain reads
refused to move under any spelling.

## Parks

`OvlFunc_959_200cf60` (`0x0200cf60`) — 14 of 147, length exact; the jump table,
all 18 slots, the case groupings and the epilogue are exact. Parked on the
sub-case closed above, and the park says so: *do not sweep this again*. Confirmed
at `-da` that the collapse happens in the **first** cse pass, so the rerun flag
cannot help and measures 20 against 20.

It also carries a two-directional return-type result: declaring one callee `int`
rather than `void` is worth 17 → 14 *on a value nobody uses*, while declaring a
different callee in the same function `int` costs 22. The oracle is per-callee
and both directions are real.

## State

- **1,872 functions remain in assembly** — 644 unparked and 291 parked in the
  main ROM, 615 unparked and 322 parked across the overlays. 3,484 elevated `.c`
  files.
- `make clean && make -j8 && make compare` green; SHA1
  `5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
  linked ELF, `.gcc2_compiled.` present in each object.
- Three splits, each verified byte-neutral before any `.c` landed. One of them
  was **hand-split**: `ovl_30_c_c_c_c.s` carries the whole overlay's `.data` and
  `.bss` after the target function, and `split_s.py` keeps trailing data with the
  function it follows, which would have carried it into the `.c` and dropped it.
  Five `.lcomm` objects needed `.global` to cross the new boundary.
- **No new `CSE_CFLAGS` rules this batch** — which is the point of the `[cse]`
  section above.
