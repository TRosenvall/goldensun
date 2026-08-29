# Batch 73 — the register class, measured and then narrowed by experiment

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `OvlFunc_956_2008204` | `02008204` | [ovl_30_a_c_c_a_c_c_a_b.c](../src/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_a_b.c) |
| `OvlFunc_910_200809c` | `0200809c` | [ovl_30_c_c_a_a_a_a_a.c](../src/overlays/rom_79dd90/ovl_30_c_c_a_a_a_a_a.c) |
| `OvlFunc_910_20084bc` | `020084bc` | [ovl_30_c_c_c_c_a_c_a_a.c](../src/overlays/rom_79dd90/ovl_30_c_c_c_c_a_c_a_a.c) |
| `Func_80173ac` | `080173ac` | [rom_15e8c_c_a_c_a_a_a.c](../src/rom_15000/rom_15e8c_c_a_c_a_a_a.c) |
| `OvlFunc_911_20080a0` | `020080a0` | [ovl_30_a_c_a_a_c_a.c](../src/overlays/rom_79e5c0/ovl_30_a_c_a_a_c_a.c) |
| `HeightTile_3` | `08011d34` | [rom_11ce0_a_c_c_a_a_a_a.c](../src/rom_9000/rom_11ce0_a_c_c_a_a_a_a.c) |

The batch's real content is the register-allocation class, which was measured,
then tested by rebuilding the compiler, and came out narrower than it looked.

## Measuring it: `tools/reg_map.py`

Register allocation is the terminal blocker — once a function's semantics are
right, what is left is which register each value lives in. So it was worth
measuring rather than describing.

For every parked `.c` whose stream is the same length as the ROM's, the tool
aligns the two, keeps only the instruction pairs that **already agree** in
mnemonic and shape, and reads off the register correspondence.

**41 same-length parks differ in registers. 26 are a clean permutation** —
every ROM register maps to exactly one of ours and back, so the function differs
*only* in naming. The other 15 are **conflicting** maps: same instructions,
different value layout, which is not a renaming and will not respond to a
renaming fix. Nobody had made that split before, and it says this is two
problems rather than one.

The permutation is dominated by a single adjacent transposition:

```
rom r2 -> ours r3   9        rom r1 -> ours r0   5
rom r3 -> ours r2   9        rom r0 -> ours r1   3
```

Eight functions are exactly `r2↔r3` and nothing else. `REG_ALLOC_ORDER`
(arm.h:989) is `{3, 2, 1, 0, 12, 14, 4, 5, ...}` — the first pseudo gets r3, the
second r2. The data is what you would see if the original compiler's order began
`{2, 3, 0, 1, ...}`.

## Testing it: the compiler was rebuilt, and the hypothesis is refuted

`/opt/camelot-gcc/` in the build image ships the gcc-2.96 **source** with a
working `build.sh`, and the image has a host toolchain. `REG_ALLOC_ORDER` was
patched to `{2, 3, 0, 1, ...}`, `cc1` rebuilt, and the result mounted over
`/opt/gcc296/cc1` in a throwaway container. Nothing in the repo or the image was
changed.

**On eighteen register-allocation parks: 5 improved, 11 got worse, 2 unchanged,
zero matched.**

```
80c23a0    4 of 16 ->  2      2008d68    2 of 22 ->  6
rom_c0cc   7 of 20 ->  4      2009458    3 of 36 ->  8
rom_e3a3c  5 of 39 ->  3      rom_15e8c  7 of 21 -> 12
800fa8c   20 of 28 -> 17      808ddb8   12 of 26 -> 17
```

**The control settles it.** Building the whole ROM with the patched order leaves
**724,691 bytes** differing. The stock `{3, 2, 1, 0, ...}` is unambiguously
right for this corpus. The build was restored and re-verified.

### What that means — the class is SOURCE-shaped, not compiler-shaped

A wrong allocator order would give a *uniform* improvement. We got a trade: some
functions want the swap, more do not. So the transposition is **not** an
allocator-configuration difference.

What is left is pseudo **creation** order. gcc hands out registers in the order
values come into existence, and creation order is a property of the source. That
is why `OvlFunc_957_200b610` (batch 71) responded to reading its sprite pointer
one statement earlier, and why naming operands *within* one statement does
nothing — no pseudo exists yet to reorder.

**The prediction was then tested.** `OvlFunc_903_2008d68` is a clean `r2↔r3`
transposition whose two values are operands of one statement, `*p = 8 | *p`.
The reading says unreachable. Three further spellings — compound assignment,
reversed operands, the load hoisted into its own statement — are all
byte-identical. Prediction made, tested, held.

So the class splits: **reachable** where the two values originate in separate
statements, **a genuine floor** where they are operands of one expression. That
is a much smaller floor than "all register-allocation parks", and it is the
first time this class has behaved predictably.

## Two levers that earned their keep immediately

**An offset in the TYPE is not a value.** Batch 72 used struct members to fix an
`ldrsh` addressing form; `Func_80173ac` shows the same lever doing a second job.
Written as pointer arithmetic, five halfword stores fail twice — the constant is
pooled **as a halfword** because the store is to a `short`, and once an `int`
local fixes that, gcc **derives** each offset from the last (`sub r0, #0x6` to
get from `0xeae` to `0xea8`) where the ROM loads each from the pool. No
arrangement of int locals stops the derivation. As struct members both problems
vanish at once: each address is generated independently, so there is nothing to
derive from.

**Read the branch suffix.** `OvlFunc_911_20080a0` came down to one instruction of
twenty-three — `bls` against our `ble` — because its loop counter was unsigned.
The rule went into `docs/elevation.md` the same round, and `HeightTile_3` then
came down to exactly the same thing, `bhi` against `bgt`, one instruction of
twenty-four. Two functions in two rounds.

## `OvlFunc_922_2009a34` and the struct-member read

Also unparked this batch, from 18 of 50. Its park diagnosed "a folded address"
correctly and put the fix in the wrong place: Thumb `ldrsh` has no
immediate-offset form, so it needs an index register either way, and written as
arithmetic gcc makes the index carry the offset. `gState.area` forces the
address to be materialised with a zero index — the ROM's shape.

The park also spent effort forcing gcc to derive `0x1c0` from `0x16c`. **Not
needed** — gcc does it unaided from plain struct members.

## Parks

| Function | Blocker |
|---|---|
| `OvlFunc_880_2008384` | gcc will not use `negsi2` on a comparison result — 1 instruction, seven spellings tried |
| `OvlFunc_924_200a648` | loop reversal; `-fno-strength-reduce` is **byte-identical**, so the obvious flag is the wrong flag |
| `Func_809b0dc` | the width of a pooled constant's load — 1 instruction of 29 |

`Func_809b0dc` is worth reading for the distinction it draws. Its
`ldr r3, =0x0` where `mov r3, #0` would do looks exactly like the pool tell —
and it is not one, because **gcc pools the zero too**. The only disagreement is
the load width, which follows the store, and the store is a `strb`. A pooled
small constant is a symbol tell **only when our gcc would have used an
immediate**; when both pool it, the question is width, not naming.
