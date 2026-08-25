# Batch 75 — a wrapper shape that looked like noise, and a screen that said yes when the build said no

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `Func_8003e10` | `08003e10` | [rom_3d04_c.c](../src/rom_c0/rom_3d04_c.c) |
| `HuffStr_Start` | `08019bac` | [rom_1908c_c_c_a.c](../src/rom_15000/rom_1908c_c_c_a.c) |
| `ActorAttrOp_waitTimer` | `0800e810` | [rom_e220_c_a.c](../src/rom_9000/rom_e220_c_a.c) |
| `ActorAttrOp_unk64` | `0800e850` | [rom_e220_c_a.c](../src/rom_9000/rom_e220_c_a.c) |
| `ActorAttrOp_unk66` | `0800e890` | [rom_e220_c_a.c](../src/rom_9000/rom_e220_c_a.c) |

## The `do { } while (0)` wrappers are load-bearing

`src/rom_c0/rom_52f4.c` — which already matches — writes every one of its
copy-to-RAM wrappers with the allocation, the DMA and the free inside one
`do { } while (0)`, and the call inside a second nested one:

```c
do {
    u32 (*func)(...) = Func_8004938(SIZE);
    DMA3_SET(routine, func, 0x84000000 | (SIZE / 4));
    do { result = func(src, dst); } while (0);
    free(func);
} while (0);
```

It reads like a leftover macro expansion, and I had written it off as noise.

**It is not noise.** Written flat, the ROM's argument saves and its size load
come out in the other order:

```
rom    mov r8, r0 / mov r10, r1 / ldr r5, =SIZE
ours   ldr r5, =SIZE / mov r8, r0 / mov r0, r5 / mov r10, r1
```

Four differing lines. With the wrappers, nothing else changed, both
`HuffStr_Start` and `Func_8003e10` are exact. The block scope decides when the
incoming arguments become pseudos relative to the size constant.

A named local for the argument does **not** substitute — that was tried first
and gcc coalesces it away.

## A screen that said yes when the build said no

`Func_801edec` screened at **45 lines against 45 with one difference**, and that
difference was only `tryc.py` printing `=_FUNC_80158E8_SIZE` where the reference
prints `=0x214` — the same instruction, the same value. By every measure the
screen can take, it matched.

**It failed `make compare` by 323,730 bytes.**

The ROM keeps a literal pool inside the function, between its two arms. gcc lays
its out differently, the translation unit comes out a different **size**, and
everything after it in the ROM shifts. The first differing bytes were not in the
function at all — they were at `0x0801512c`, in the rom_15000 exports table,
whose veneers carry pooled addresses of functions that had moved.

### The tool had the check and never ran it

`tryc.py` has an inline-pool warning. It fired on two other functions in the
previous batch. **It did not fire here** — because the warning lived only on the
`OK` path, and this was an `XX` with one cosmetic difference.

Fixed: the warning now fires on the near-miss path too, and it was verified
against the function that slipped through. A near-match with an inline pool is
exactly as unproven as a clean one.

**The rule, now in `docs/elevation.md`:** the only test that decides pool layout
is `make compare`. Treat a clean screen on a function whose reference has any
inline `.word` or `.pool` as unproven — warning or no warning.

## Size symbols: two more, and the convention held

Continuing batch 74's find, `_FUNC_8015570_SIZE` (0x60) and `_FUNC_80158E8_SIZE`
(0x214) are now emitted by the existing `.func_end_emit_size` macro, both
exactly the routine's extent with no veneers to account for. Both edits emit no
bytes.

That makes six size symbols across the two batches, all found the same way: a
pooled constant that gcc would have built with `mov`, or `mov`/`lsl`, in fewer
bytes than the pool entry costs.

## One field, three load widths, no casts

The three `ActorAttrOp_*` functions differ only in which halfword they act on.
The ROM uses `strh` to set it, `ldrh`/`add`/`strh` to accumulate, and `ldrsh` to
compare — and declaring the member `short` gives all three. gcc picks the
unsigned load for the accumulate because the sign cannot affect an add that is
truncated back to sixteen bits, and the signed one where the value is actually
compared.

Declaring it `unsigned short` to match the two `ldrh`s would break the `ldrsh` —
the same trap as `OvlFunc_886_2008088` in batch 72.

The `(short)v` cast on the right of the comparison is the ROM's
`lsl #16 / asr #16` and is not optional.

## Parks

| Function | Blocker |
|---|---|
| `Func_801edec` | literal pool layout — one cosmetic line on the screen, 323,730 bytes on the build |
| `Func_80b06c0` | two-operand versus three-operand shift, 2 of 22 |
| `UnpackTilemap` | a symbol address CSEd across two call sites; `-fno-gcse` does not stop it |

`Func_801edec` is otherwise complete: its size symbol, its `int`-local fill
constant and its wrappers are all settled, so it is one blocker away — and it is
the same blocker that parks `Func_80c0e38` and `ovl_7ec19c/200816c`.

`Func_80b06c0` records a useful negative alongside: `unsigned char` for the
stamped value is **worse**, not better — the whole computation narrows to byte
width and three extra instructions appear. `int` is correct.
