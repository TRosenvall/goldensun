# Batch 79 — the screen was wrong about pool loads, and it had been for a year

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `OvlFunc_914_2008b24` | `02008b24` | [ovl_30_c_c_c_c_a_b.c](../src/overlays/rom_7a1ff0/ovl_30_c_c_c_c_a_b.c) |
| `OvlFunc_915_2008cf4` | `02008cf4` | [ovl_30_c_c_c_a_b.c](../src/overlays/rom_7a2bf0/ovl_30_c_c_c_a_b.c) |
| `OvlFunc_916_2008ecc` | `02008ecc` | [ovl_30_c_c_c_a_c_b.c](../src/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_b.c) |
| `OvlFunc_917_20097d0` | `020097d0` | [ovl_30_c_c_c_c_a_c_b.c](../src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_c_b.c) |
| `BreakItem` | `08078a34` | [rom_78414_c_c_c_a.c](../src/rom_77000/rom_78414_c_c_c_a.c) |
| `Func_800c5b4` | `0800c5b4` | [rom_c004_c_a_c_c_a.c](../src/rom_9000/rom_c004_c_a_c_c_a.c) |

Six functions, and the C for five of them was either trivial or already written.
What was in the way was a defect in `tools/tryc.py`.

## Thumb-1 has no pc-relative `ldrh`

gcc prints a HImode pool reference as

    ldrh r2, .L0

and the ROM disassembly writes

    ldr r2, =0x1f

These are **the same instruction**. Thumb-1's only PC-form load is
`LDR Rd, [PC, #imm8*4]`; there is no halfword or byte encoding of it, so gas
assembles gcc's line to the identical `0x4a0d`. `tryc.py` compares assembly
*text* and its pool normaliser matched only the `=` spelling, so it has been
reporting a difference that does not exist in any object file.

The cost is measurable. Sweeping all 134 parked files with the old tool and the
new one, four verdicts change and **two go from XX to OK**:

| Function | Rounds spent | Park's own diagnosis |
|---|---|---|
| `BreakItem` | eleven formulations | "gcc pools a small constant as a HALFWORD when the target is `unsigned short`… it is not yet known what C shape gets gcc there" |
| `Func_800c5b4` | eight formulations over four batches | "the inverted narrow_constant class — gcc narrowing what the ROM keeps wide" |

Both are now elevated with the C unchanged from what was parked. `BreakItem`'s
park had even closed with *"worth retrying once another function in the corpus
is found that pools a small constant as a word — that one will show the shape."*
`OvlFunc_914_2008b24` was that function, and the shape it showed is that there
was never anything to find.

## The four-member family the candidate list refused to show

`OvlFunc_914_2008b24` and its three twins are a BGR555 warming fade: pull
`rate`-th parts out of green and blue, push a quarter of one back into red,
clamp red and repack. 48 instructions, four byte-identical copies, second by
payoff in `find_twins.py` behind only the thirteen-member group.

`pick_candidates.py` **excluded all four outright**, because the ROM keeps its
literal pool inside the function body behind a `.pool_aligned` — the batch-30
rule. That rule is wrong, and this family is the counterexample:

> gcc emits a mid-body pool, `b` around it and all, whenever the pool reference
> is **narrow**. `arm.md` gives a HImode pool load a `pool_range` of 32–60 bytes
> against SImode's 1020, so it cannot reach the barrier at the end of the
> function and `dump_table` manufactures a jump over an early pool.

The exclusion is now off by default (`--no-inline-pool` restores it); the
worklist goes from 47 candidates to 55. What batch 30 actually caught remains
true and is unchanged: **the screen cannot see PC-relative offsets**, so an
inline-pool reference has to go to `make compare` before it is believed.
`tryc.py` warns about that on both the OK and the near-miss paths.

Three ordinary levers finished the body — an `unsigned short` parameter so the
zero-extending `lsl r0, #16` is shared by all three field extractions;
declaration order `b, g, r`, which picks whether green lands in r5 or r7; and a
right-associated repack `r | ((b << 10) | (g << 5))`, which puts blue's shift
first and combines the two into r3 before touching r6.

## The false lead, recorded because it was convincing

Reading the ROM's `ldr r2, =0x1f` as the pool tell — gcc never pools what
`mov r2, #0x1f` can build, therefore the operand was a symbol — gives

    extern int _K_1f;
    ... & (int)&_K_1f

and that reproduces the ROM's assembly **text** exactly, first line to last.

It is wrong. An SImode symbol load has the full 1020-byte range, so the pool
then moves to the end of the function and the `b` disappears: right text, wrong
bytes. The narrow mask was never a symbol. **Check the encoding before believing
a pool-tell diagnosis** — one `objdump -d` on each object settles it, and it is
now the first line of that section in [elevation.md](../docs/elevation.md).

## Pool ORDER is a readout of operand modes

With the false difference gone, `Func_800c5b4` still failed `make compare` by
fourteen bytes with **every instruction identical**. The pool words were in a
different order:

    rom   0x1000  Func_800c62c  Func_800c880  0xf1ff
    ours  0xf1ff  0x1000        Func_800c62c  Func_800c880

gcc orders a minipool by each entry's `max_address` — the referencing
instruction's address plus its `pool_range` — so the order is a direct statement
about the **mode** of every operand. An early narrow constant sorts before a
late wide one. The ROM's order says its `0xf1ff` is SImode and its `0x1000` is
HImode; ours said both were narrow.

So the park's mode analysis had been right all along and only its evidence was
wrong. The fix is the split the park had already tried:

```c
v = *p & 0xf1ff;      /* v stays u32 here, so the AND is SImode */
*p = v | 0x1000;      /* the store narrows only the OR */
```

That experiment was run four rounds ago and scored **"no change"**, because the
screen drops pool words on both sides by design — the one thing it changed was
the one thing the measurement could not see. Compare pool order whenever every
instruction matches and the bytes do not.

## Tool changes

`tools/tryc.py`

* `ldr[bh]? rD, .Lnn` is now resolved as a pool load and the mnemonic rewritten
  to `ldr`. Only a bare label operand matches, so `ldrh r2, [r3, #4]` is
  untouched.
* Labels are numbered **after** the unreferenced ones are dropped, not before.
  Numbering first hands an index to a label that is then discarded, so every
  later label on that side is off by one — which is why a byte-identical
  function reported `ble L1` against `ble L2`.
* A near-miss whose differences are `bl __divsi3` against `bl _divsi3_RAM` now
  prints the linker-alias line. Deliberately **not** normalised away: 17 call
  sites inside `asm/overlays/` do call `__divsi3` directly, so equating the two
  names would hide a real difference in those.

Both normaliser changes were regression-swept across all 134 parked files with
a live reference, comparing old and new verdicts. Four changed; the two XX→OK
are this batch's, and the other two are diff *counts* moving on functions that
remain parked.

## What this opens

`Func_80c0e38` and `Func_80c0e70` were parked in batch 74 as *"literal pool
PLACEMENT — 18 of 19, and the one missing instruction is a `b` that exists only
to jump over the ROM's mid-function pool."* That is now a described mechanism
with a lever rather than a dead end, and it is the first thing to retry.

More broadly, the 336 overlay references that `pick_candidates.py` was skipping
are back on the list.
