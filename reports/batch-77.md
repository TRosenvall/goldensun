# Batch 77 — twins found by hashing, and one member read two ways

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `OvlFunc_923_2009c20` | `02009c20` | [ovl_1a3c_a_a_b.c](../src/overlays/rom_7aa430/ovl_1a3c_a_a_b.c) |
| `OvlFunc_924_200d1b0` | `0200d1b0` | [ovl_35b8_a_a_c_c_b.c](../src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_b.c) |
| `OvlFunc_968_200832c` | `0200832c` | [ovl_30_a_a_a_c_c_a_a_a.c](../src/overlays/rom_7f2f14/ovl_30_a_a_a_c_c_a_a_a.c) |
| `OvlFunc_common1_2018` | `0200b8b0` | [common1_c_c_a.c](../src/overlays/common/common1_c_c_a.c) |
| `OvlFunc_921_200974c` | `0200974c` | [ovl_30_c_c_c_c_c_a_c_b.c](../src/overlays/rom_7a7298/ovl_30_c_c_c_c_c_a_c_b.c) |

## Two solves, two free twins

Two of the five are byte-identical duplicates of functions solved minutes
earlier in the same round. I found them by hashing every function body with
labels normalised away, *after* each solve rather than while hunting for
clusters — the C ported verbatim with only the symbol changed.

That is worth making a habit: the duplicate scan is cheap, and a solved shape is
exactly when it pays.

## One `short` member, read two ways, no casts

`OvlFunc_921_200974c` reads its angle word with **`ldrsh`** where it is scaled
into a position — the sign matters there — and with **`ldrh`** where it is
stepped by 2 and stored straight back, because the sign cannot affect an add
that is truncated to sixteen bits.

Declaring the member `short` gives both. Declaring it `unsigned short` to match
the `ldrh` breaks the `ldrsh`. Same trap as the `ActorAttrOp` family in batch 75
and `OvlFunc_886_2008088` in batch 72 — three times now, which makes it a rule
rather than a coincidence.

## Where the int-width dance does and does not apply

`OvlFunc_923_2009c20`'s step counter is a **halfword**, so the increment has to
be done at int width and narrowed only where it is tested:

```c
t = a->f64 + 1;          /* int */
a->f64 = t;
if ((short)t > 0x1f)     /* narrow HERE, not in the arithmetic */
```

`OvlFunc_921_200974c`'s lifetime counter is an **int**, and there
`if (--a->f68 == 0)` is simply correct — gcc keeps the decremented value in the
register and tests it directly.

The rule from batch 76 needed that boundary drawn: the dance is for counters the
store truncates, not for every counter.

## Two statement swaps in a six-instruction preamble

`OvlFunc_968_200832c`'s body was exact on the first screen; all nine differing
lines were setup:

- reading the block pointer into its own local **before** the caller's first
  coordinate is what puts the caller in r4 and the block in r2;
- initialising the loop counter **before** the coordinate shift rather than
  after moves `mov r5, #8` past the `asr`.

`i = 8;` and `qx = q->x >> 20;` are independent statements. Which one comes
first decides two instructions.

Its counter is unsigned — the ROM's `cmp r5, #0x41 / bls` says so, and an `int`
gives `ble`. Third function in three batches to end on exactly that.

## A wiring slip the orphan check caught

The twin's cut was at the **tail** of its file, so no `_c` piece exists — and I
wrote all three piece names into the linker script anyway. `asmfacts --orphans`
named it immediately.

That check has now caught two different wiring slips in three batches. It costs
one command and belongs on every split, not just when something already looks
wrong.

## One park, and a warning about screening it

`OvlFunc_965_200a660` sits at 30 lines against 30 with 19 differing, all of them
r5/r6 and r0/r1 exchanged. The ROM keeps the actor in r5 and the stack block in
r6; we do the opposite. The block's pseudo is created when gcc lowers the local
array's address, ahead of any statement, so no reordering of the body moves it.

**And the screen lies about it by default.** `tryc.py` takes per-file flags from
the Makefile, and the wildcard rule
`src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c` hands this path `-O1`. The tool
prints its own warning that a wildcard rule may describe a *neighbouring*
translation unit — and it is right: at `-O1` the diff is 22 lines, at `-O2` it is
19. The park records the `-O2` numbers and says to pass `--cflags "-O2"`, because
otherwise the next person measures the wrong compiler.
