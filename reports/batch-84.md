# Batch 84 — widths in both directions, and a tool that could not see `.lcomm`

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `OvlFunc_932_200ba44` | `0200ba44` | [ovl_30_c_c_b.c](../src/overlays/rom_7b9cb4/ovl_30_c_c_b.c) |
| `OvlFunc_957_2008ee0` | `02008ee0` | [ovl_30_c_c_a_c_c_c_c_c_c_c_c_b.c](../src/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_c_c_b.c) |
| `Func_80b10cc` | `080b10cc` | [rom_b0070_a_a_c_c_a_c_b.c](../src/rom_b0000/rom_b0070_a_a_c_c_a_c_b.c) |
| `OvlFunc_886_20084dc` | `020084dc` | [ovl_30_c_c_c_c_c_c_c_c_c_c_a_b.c](../src/overlays/rom_786f0c/ovl_30_c_c_c_c_c_c_c_c_c_c_a_b.c) |
| `OvlFunc_886_20085d4` | `020085d4` | [ovl_30_c_c_c_c_c_c_c_c_c_c_c_b.c](../src/overlays/rom_786f0c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_b.c) |

Five elevated, five parked, and one gap closed in `split_asm.py`.

## `split_asm.py` could not see a `.lcomm` definition

Its label-definition pattern matched `.Lxxxx:` and nothing else, so

    .lcomm  .L525c, 4

— which reserves a `.bss` slot and defines the symbol exactly as a colon would —
was invisible. Splitting `ovl_30_c_c.s` reported **one** label to export when
three were needed, and the link died with `undefined reference to '.L525c'`
from the piece that had just been cut away.

The pattern now accepts `.lcomm` and `.comm`. Re-run on the same file it reports
all three. Checked against three unrelated splits for regressions.

## The same rule, pointing both ways

Batch 83 found that naming a constant in a local **of the width it is combined
with** decides which operand becomes an `orr` destination. This batch used the
width lever twice more, and the two point in opposite directions:

| function | ROM has | we had | fix |
|---|---|---|---|
| `OvlFunc_957_2008ee0` | `mov r2, #0xf` | `ldr r2, =0xf` | an **`int`** local for the mask — widening it to SImode lets gcc use the immediate |
| batch 83's `_CONST_2` | `ldr r3, =2` | `mov r3, #2` | a **narrow** local, then a symbol |

Three other spellings on `2008ee0` — a named intermediate for `*p + 1`, an
explicit `(unsigned short)` cast, the increment split in two — all leave the
pool load in place. Only re-typing the **mask** moves it.

So the rule is not "name it narrow" or "name it wide": it is **read the width
off the ROM**, because the constant's mode is what decides between a `mov` and a
pool entry.

`OvlFunc_932_200ba44` is the same question with nothing to do: `ldr r2, =0` is
**not** the pool tell there, because both stores are to `unsigned short` globals
and a HImode zero is pooled by construction. No symbol invented.

## The absence of two instructions says which width to write

Both shop counters test whether the player is inside a facing arc:

    ldrh r3, [r0, #6] / add r3, r2   @ r2 = 0xffff5fff
    cmp r3, r2                       @ r2 = 0x3ffe
    bhi ...

There is **no `lsl #16 / lsr #16` pair**, so the subtraction wraps in 32 bits and
the comparison is unsigned over the whole word:
`(unsigned int)(f6 - 0xa001) <= 0x3ffe`. The `(unsigned short)` form the tree
uses for other arc tests would add the shift pair — the two missing instructions
are the whole signal. Both matched on the first screen.

## Declare `gState` as a struct, not as bytes

    extern unsigned char gState[];
    *(int *)(gState + 0x10)     ->  ldr r3, =gState+16 / ldr r0, [r3]
    gState.f10                  ->  ldr r3, =gState    / ldr r0, [r3, #0x10]

gcc folds a constant offset into the pool entry. The byte-array spelling is
convenient for reaching an unknown offset and is the wrong tool whenever the ROM
keeps base and offset apart. Two of `Func_80b10cc`'s 25 lines, and it recurred
in `OvlFunc_899_2008310` below.

## `-fno-strict-aliasing`, earning its rule again

`OvlFunc_957_2008ee0` re-reads its counter halfword after two `int` stores:

    ldrh r3, [r1] ... str r3, [r0, #0x18] / str r3, [r0, #0x1c] ... ldrh r3, [r1]

At `-O2` strict aliasing says an `int` store cannot touch an `unsigned short`,
so gcc keeps the first read. With the flag both reads come back and the function
goes from 20 differing lines to one.

## The five parks

| function | state | blocker |
|---|---|---|
| `OvlFunc_899_2008310` | 23 of 25 | signed lower bound rewritten to `cmp #(K-1) / ble`; **third** member of that class. Two things solved on the way — the compound condition fuses and must be nested as two `if`s, and `gState` again |
| `OvlFunc_923_2008f48` | 34 of 36 | argument precompute; both declaration levers tried in both directions, plus naming each constant |
| `OvlFunc_959_2008e80` | 20 of 37 | constant CSE **in the direction the tree had not seen** — gcc shares two identical register arguments where the ROM builds both |
| `OvlFunc_883_20091d8` | 29 of 38 | one pool load scheduled a call too early. Two things solved: the second message id is `id + 2` off the first, and the last call must stay implicit |
| `Func_80270ac` | 19 vs 20 | gcc turns a halfword store into a word read-modify-write |

Two of those are worth reading past the headline.

**`OvlFunc_959_2008e80` is a new direction for constant CSE.** The blocker
`pick_candidates.py` screens for is gcc hoisting a *pool load* into a
callee-saved register; here gcc shares a *computed* value between two argument
registers where the ROM computes it twice, which costs one instruction less and
which none of `-fno-gcse`, `-fno-cse-follow-jumps` or `-fno-rerun-cse-after-loop`
prevents. Batch 83's mirror — naming stack arguments to keep two pseudos alive —
does not apply, because these are register arguments and naming them does not
stop gcc noticing they are equal.

**`Func_80270ac`'s RMW is not the uninitialised read.** The ROM reads r9 without
ever writing it, which is the documented uninitialised-local shape, and that was
the first suspicion. Replacing the uninitialised value with a literal `0` still
produces the read-modify-write, so the two are independent.

## Also re-attempted

The `GetFlag` family was re-run with the batch-82 and batch-83 levers. Neither
reaches it — there is no commutative operator to reorder and no constant to
re-type — and the floor stays at 3 of 13 with the seven attempts now tabulated
in its park.
