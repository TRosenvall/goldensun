# Batch 148 — three levers that decide a register, and two tools that were choosing badly

Verified from a clean build: `make clean` → host recovery for the five
`old_agbcc` objects ([batch-61](batch-61.md)) → `make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`. Every
address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `OvlFunc_927_20099b8` | `020099b8` | ovl_7b4558 | [ovl_30_c_c_a_c_c_c_c_b.c](../src/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_c_b.c) |
| `OvlFunc_941_20091b8` | `020091b8` | ovl_7c5efc | [ovl_30_c_a_c_c_c_a_c_c_b.c](../src/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_c_c_b.c) |
| `OvlFunc_943_20097a0` | `020097a0` | ovl_7c7b9c | [ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_b.c](../src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_b.c) |
| `OvlFunc_951_20089f8` | `020089f8` | ovl_7d6418 | [ovl_30_c_c_c_a_c_a_b.c](../src/overlays/rom_7d6418/ovl_30_c_c_c_a_c_a_b.c) |
| `Func_80b8b48` | `080b8b48` | main ROM | [rom_b8228_c_a_c_a_c_c_b.c](../src/rom_b5000/rom_b8228_c_a_c_a_c_c_b.c) |

Eight functions parked, most of them one or two instructions out. The batch is
short on elevations and long on measurement, and the measurements are the point:
three of the four new rules below are about **which register a value lands in**,
which is what the parked set has been failing on for months.

## A narrow store of a literal: three spellings, three answers

`*(short *)(e + 6) = 0x80 << 8` compiles to `ldr r3, =0xffff8000` — gcc puts the
constant in the pool, sign-extended to the destination's width — where the ROM
has `mov r3, #0x80 / lsl r3, #8`. Measured four times this batch:

| spelling | constant | register |
|---|---|---|
| `*(short *)(p + K) = V` (cast) | POOLED | — |
| `e->f6 = V` (typed field) | mov/lsl | SCRATCH |
| `v = V; *(short *)(p + K) = v` | mov/lsl | CALLEE-SAVED |

**Prefer the typed field.** It is the only one that gets the ROM's instructions
without spending a register, and on `OvlFunc_943_20097a0` that decided the
function: it has exactly two callee-saved registers and both are already claimed
by values that genuinely live across calls, so naming the two single-use
constants would have taken registers they needed. 37 differing → 6.

Reach for the named local only when the ROM itself spends a register on that
value — **look at the prologue.** `20097a0` pushes {r5, r6} and holds
`0xa0 << 7` and a zero there for the whole body, so those two are named and the
two single-use constants are struct fields.

That also refines "do not name zeros": a zero the ROM keeps in a pushed register
across calls *is* a named local. The rule is about zeros gcc would otherwise
rematerialise beside each use.

The same lever fixed an unexplained mid-function literal pool on
`OvlFunc_881_200b57c` — the pooled zero was one extra pool entry, and one extra
entry was enough to make gcc dump the pool early and jump over it. 31 differing
→ 2.

## Explicit gotos are how you control block order

Two functions needed the case bodies where the ROM puts them, and neither
ordinary construct produces that layout:

  * an **if/else-if chain** puts each body INLINE behind a `bne`, so the tests
    are separated by their bodies. `OvlFunc_927_20099b8` scored 99 differing.
  * a **switch** SORTS the cases and emits a balanced compare tree
    (`cmp #9 / beq / cmp #9 / bgt / …`). That is visible immediately: a `bgt`
    against a case value that is not a range bound means gcc built a tree.

The ROM shape in both was a contiguous run of tests branching forward to bodies
that follow in source order. Writing that with labels and gotos took `20099b8`
from 99 differing to 37 in one edit.

A switch is still right when the ROM's own compare order is **sorted** — that is
the tell that gcc built the tree rather than the author writing a chain.

### And the branch sense inverts

```
if (c) goto X;      ==>     b!c  Y
goto Y;                     b    X
```

gcc expands a conditional goto as "jump-if-false to the next statement", and
jump threading folds the following unconditional jump into the branch — so the
branch you get is the opposite of the one you wrote. To reproduce a ROM that
reads `bne loop / b out`, write `if (!c) goto out; goto loop;`. Two sites on
`OvlFunc_941_20091b8` were four of its six differing lines.

**Only for a conditional goto immediately followed by an unconditional one.**
The six single-branch tests in the same function came out right written the
natural way, and inverting those breaks them.

## `ldmia`/`stmia` into the outgoing argument area means a struct BY VALUE

`OvlFunc_927_20099b8` calls a six-argument function; the ROM loads four words
into r0–r3 and block-copies the remaining two with `ldmia r3!, {r0,r1} / stmia
r2!, {r0,r1}`. Written as six separate int arguments gcc uses move-by-pieces —
two spare registers and two `str`s — which costs a **third** callee-saved
register and a `push {r7}` the ROM does not have. Passing the whole 24-byte
struct produces the block move and frees the register.

## The declaration is a per-call-site choice — three more confirmations

Batch 147 found that a callee called twice whose two sites want opposite
argument orders needs **two declarations**, the odd site going through an
`__asm__` alias with a different return type. It came up three more times here:
`__Func_8092c40` on `20091b8` and again on `20089f8`, and `__MapActor_SetPos` on
`20097a0` — where it is five sites against two, the five undeclared and the two
through the alias.

## Two tools were choosing badly

**`tools/pickable.py` was ranking on nothing that predicted failure.** It now
prints how many **argument-construction interleave** sites a candidate carries —
`mov r0` sitting inside another argument's mov/lsl pair — and rejects any
function with **three or more `neg`**, the `-1` triple. Three of this batch's
parks are nothing but interleave, and the queue's top candidate carries three of
them. 56 candidates → 42, and the ones that survive are the ones worth reading.

**The `-1` triple narrowed.** `OvlFunc_881_2009a98` is a clean instance: ONE
call to `__Func_80933f8(-1, -1, -1, 0)` and nothing else in the function touching
`-1`. So the reuse is **not** cross-site CSE, which is what
`src/non_matching/overlays/constant_reuse.c` assumed from
`OvlFunc_955_2009424`. gcc simply will not materialise the same constant three
times for three argument registers, and there is nothing at the call site to
change. Three further spellings — separate locals, no prototype, mismatched
parameter types so the constants would have different modes — all inert.

## What the parks are blocked on now

Two of them are one instruction from matching and both stop at a decision made
**after** register allocation:

  * `OvlFunc_926_2008afc` — the ROM does not keep its zero live across the
    message branches; it rematerialises it as `ldr r5, =0x0`, a **pool** load,
    because a constant materialised by reload never reaches the thumb mov/lsl
    splitter. We keep the pseudo in r5, which is one instruction shorter, and
    the missing pool entry lets our pool sit past the epilogue where the ROM has
    to jump over it. One absent reload costs three lines.
  * `OvlFunc_common1_fac` — `a * 60` is live in r6 across a branch; the positive
    arm reuses it and the ROM's negative arm **rebuilds** `a * -60` from scratch
    in three instructions where gcc emits `neg r0, r6`. Eight spellings and five
    flag sets inert.

And `OvlFunc_946_2009c84` is 77 of 77 lines where **every one of the eleven
differing lines is the same two values in swapped registers**. Both have the
same live range and similar reference counts, so which one wins r5 is a near tie
gcc breaks the other way. Eleven spellings inert. It is a clean two-pseudo test
case for the `REG_ALLOC_ORDER` hypothesis that `docs/elevation.md` says needs a
rebuilt compiler to settle.

## Two process notes

I started renaming `.L3`/`.L13` in the shared hand-written asm that exports them
before finding that `docs/elevation.md` already documents the `_TBL_L*` linker
aliases for exactly that hazard — gcc numbers its own labels from `.L1`, so a
compiled function that needs three labels *defines* `.L3` itself and a short
`.LN` extern silently rebinds to it. The rename broke the link and was
unnecessary. **Grep the doc before editing shared asm.**

And several scratch filenames collided with tracked ones: this filesystem is
case-insensitive, so `Z1.c` *is* `z1.c`, and `rm scratch/W*.c` matched lowercase
files too. Everything was restored from HEAD; scratch files now carry a
per-round prefix.

## Numbers

3272 elevated / 2145 remaining / 388 parked.
