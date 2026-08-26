# Batch 80 — spending batch 79's lever, and reading the pool rule out of `arm.c`

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `Func_80c0e38` | `080c0e38` | [rom_bffb8_a_c_c_b.c](../src/rom_b5000/rom_bffb8_a_c_c_b.c) |
| `Func_80c0e70` | `080c0e70` | [rom_bffb8_a_c_c_b.c](../src/rom_b5000/rom_bffb8_a_c_c_b.c) |
| `OvlFunc_882_2008064` | `02008064` | [ovl_30_a_a_b.c](../src/overlays/rom_77dd1c/ovl_30_a_a_b.c) |
| `OvlFunc_943_2008030` | `02008030` | [ovl_30_a_a_a_a.c](../src/overlays/rom_7c7b9c/ovl_30_a_a_a_a.c) |
| `OvlFunc_921_2009f24` | `02009f24` | [ovl_30_c_c_c_c_c_c_c_b.c](../src/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c_b.c) |
| `OvlFunc_922_200a014` | `0200a014` | [ovl_30_c_c_c_c_c_c_c_b.c](../src/overlays/rom_7a8c8c/ovl_30_c_c_c_c_c_c_c_b.c) |

Three solves, each with a twin. Two parks, both with the measurements rather
than a guess.

## The park's own advice was the thing in the way

`Func_80c0e38` and `Func_80c0e70` were parked in batch 74 as *"literal pool
PLACEMENT — 18 lines against 19, and the missing instruction is a `b` that
exists only to jump over the ROM's mid-function pool."* The report for batch 79
named them as the first thing its lever should open. It did, but not the way I
expected: the park's **first recorded lever was actively causing the blocker**.

> 1. 0x2044 THROUGH AN int LOCAL. Stored straight to `REG_BLDCNT` — a `vu16` —
>    gcc pools it as a HALFWORD (`ldrh`); the ROM loads a word.

`ldrh rD, .L` and `ldr rD, =v` are the same instruction in Thumb-1, so the ROM's
spelling never said anything about the mode. What the `int` local did say was
SImode — `pool_range` 1020 instead of 64 — which let the pool reach the barrier
at the end of the function, and the `b` disappeared with it. Writing the literal
straight to the `vu16` keeps the reference narrow and the pool lands exactly
where the ROM has it.

Pool order is the readout, and it is worth seeing as a table:

| source | pool words |
|---|---|
| ROM | `0x2044` `0x1010` `0x04000050` `0x04000052` |
| `int c = 0x2044` | `0x1010` `0x04000050` `0x2044` `0x04000052` |
| `*cnt = 0x2044` | `0x2044` `0x1010` `0x04000050` `0x04000052` |

`0x2044` sorting *after* a symbol address is what "it went wide" looks like.

The park's other two findings survive and are still load-bearing: both register
addresses in pointer locals assigned in the ROM's order, and the step written
`k - i` and `i + k` to match the ROM's operand order.

## The placement rule, now read rather than inferred

Enough of this class has come up that the mechanism is worth having exactly.
From `arm.md`:

| pattern | `pool_range` | note |
|---|---|---|
| `*thumb_movsi_insn` | 1020 | |
| `*thumb_movhi_insn` | 64 | pool entry is still a full `.word` |
| `*thumb_zero_extendhisi2` | 60 | prints **`ldr`** for a pool label, not `ldrh` |
| `*thumb_movqi_insn` | 32 | |

and from `arm_reorg`/`add_minipool_forward_ref` in `arm.c`: gcc puts a pool at
the **last barrier within reach of the pool's first entry**, manufacturing one
with a `b` if none is in range, and stops accumulating when

    fix->address >= minipool_vector_head->max_address - fix->fix_size

Two consequences that decide real functions:

- **A wide first entry pushes the whole pool to the end.** That is why an `int`
  local for a constant is actively harmful when the ROM's pool is mid-body.
- **A short-range first entry can also SPLIT the pool**, leaving later
  references to a second pool after the epilogue.

A HImode `CONST_INT` is emitted into the pool sign-extended, so a HImode mask of
`-0x4000` appears as `.word 0xffffc000`. **The pool word is four bytes wide
whatever the mode, so its value alone does not tell you the mode** — only its
position in the pool does.

## `OvlFunc_962_200816c` stays parked, with a much sharper note

That pair needs the *split*, and the arithmetic now says exactly what it would
take. For the ROM's second pool to exist, the head entry — `0xffffc000` at
`+0x12` — must have a `max_address` of at most `0x54`, so the `0x25d5` fix at
`+0x50` does not fit. That means `forwards ≈ 64`: **a HImode move**.

The bind is that the mask must *be* `0xffffc000` to survive the `(u16)`
comparison, and every C expression holding that value is SImode at the tree
level. Every spelling that makes the AND HImode narrows the mask to `0xc000` and
loses the ROM's `lsl #16 / cmp` pair with it. Eleven were byte-compared against
`orig.bin`; the park lists them and now says not to retry spellings.

## Naming a file-local data label

`OvlFunc_921_2009f24` indexes a sixteen-entry table of target angles that lives
in its own `.s` as `.L23f0`. A C file cannot reference that — local labels do
not cross objects and the name is not an identifier. `split_asm.py` reports it
as `MUST EXPORT`, but `.global .L23f0` only solves the asm-to-asm case.

**Rename it and export it.** A label emits no bytes, so this changes
symbol-table metadata and nothing else:

```diff
-.L23f0:
+	.global gTable_921__0200a3f0
+gTable_921__0200a3f0:
 	.incbin "overlays/rom_7a7298/orig.bin", 0x23f0, (0x2430-0x23f0)
```

Named **by address**, in the shape the script blobs three lines below it already
use (`gScript_921__0200a4f4`) — which asserts nothing about what the data means.
Now a technique section in [elevation.md](../docs/elevation.md).

With the name available the pair was exact on the first screen. Three ordinary
readings did the rest: the counter at `+0x64` is a `short` read **signed** for
`!= 0` and **unsigned** for the decrement (fifth function on that rule); the
slew is a `short`, so the clamp compares after `lsl #16 / asr #16`; and
`a->f5a = 0` compiles to `strb r1, ...` reusing the register that already holds
the tested zero, so no `mov #0` should be written for it.

## A `switch` reproduces the ROM's decision tree

`OvlFunc_882_2008064` matched on the first screen one line out, and that line
was the linker alias. Two rules did the work:

- **The same member read signed and unsigned.** `switch (a->f64)` gets `ldrsh`
  because the sign picks the case; `a->f64--` gets `ldrh` because the result
  goes straight back into a halfword. `short` gives both; `unsigned short`
  breaks the switch.
- **A plain `switch` emits the ROM's BALANCED tree** — `== 2`, then `> 2`, then
  `== 0` over the case values `{0,2,4,6}`. An if/else chain does not. The shared
  tails are cross-jumping, not the source's.

It needed `__umodsi3 = _umodsi3_RAM;` in both overlay scripts. `tryc.py`'s alias
hint now covers all four helpers — it matched the literal string `divsi3` and so
said nothing about this one, whose entire diff was `bl __umodsi3`.

## Two parks

`OvlFunc_898_2008ef4` and its twin are 28 of 30 identical, and the two that
differ are the **twelfth** instance of argument precompute:

    rom    lsl r1, #8 / mov r0, #0  / lsl r2, #7
    ours   lsl r1, #8 / lsl r2, #7  / mov r0, #0

Read off `calls.c`, the ROM's order says it precomputed its *second* argument
and not its third. Both are `0x80 << N`, identical in `arm_rtx_costs`, so no C
expression separates them. Worth recording: `-fno-schedule-insns2` takes this
from 2 differing to 13, so post-reload scheduling is what produces the
near-match and the remaining transposition is a tie broken by generation order.

`Func_80f4100` (ScalePalette, two copies) is a genuine tradeoff and the park
says so rather than picking a side:

| shape | lines | differ | what it gets right |
|---|---|---|---|
| named temporaries for the three products | 64 | 63 | the ROM's operation order — all three masks and muls, then all three shifts |
| one combined expression | 54 | 41 | the register count — r1–r4 plus three high registers, no extra pushes |

Six shapes and two flag sweeps are recorded; nothing gets both. `int` versus
`u32` for the products is settled — they are unsigned, which is what decides
`lsr` against `asr`.
