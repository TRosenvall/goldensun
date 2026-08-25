# Batch 70 — the duplicate clusters, and two things the source says better than masks do

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked
`overlay.elf`. 0 orphaned linker references.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_919_20082a0` | `020082a0` | rom_7a67d8 | [ovl_30_c_a_c_c.c](../src/overlays/rom_7a67d8/ovl_30_c_a_c_c.c) |
| `OvlFunc_common0_d4` | `02008104` | common | [common0_b.c](../src/overlays/common/common0_b.c) |
| `OvlFunc_970_2008f30` | `02008f30` | rom_7fa4ec | [ovl_30_c_c_c_a_c_c_c_c_b.c](../src/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_c_c_c_b.c) |
| `OvlFunc_common0_0` | `02008030` | common | [common0_a_a.c](../src/overlays/common/common0_a_a.c) |
| `OvlFunc_927_20089dc` | `020089dc` | rom_7b4558 | [ovl_30_a_a_c_c_c_c_a_b.c](../src/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c_a_b.c) |
| `OvlFunc_946_20089dc` | `020089dc` | rom_7ced6c | [ovl_30_a_a_c_c_c_a_b.c](../src/overlays/rom_7ced6c/ovl_30_a_a_c_c_c_a_b.c) |
| `OvlFunc_964_20089dc` | `020089dc` | rom_7ed0a0 | [ovl_30_a_a_a_c_c_c_c_a_b.c](../src/overlays/rom_7ed0a0/ovl_30_a_a_a_c_c_c_c_a_b.c) |
| `OvlFunc_965_20089dc` | `020089dc` | rom_7ef4f4 | [ovl_30_a_a_a_c_c_c_c_a_b.c](../src/overlays/rom_7ef4f4/ovl_30_a_a_a_c_c_c_c_a_b.c) |

Five of the eight are one function; another closes the seven-copy set opened in
batch 69. Duplicate clusters are where the leverage is right now.

## A read-modify-write on a few bits is probably a bitfield

Five copies of an eleven-instruction function that does nothing but write two
bits. Spelled out as masking, with every lever the corpus knows applied —
assign back into the parameter, the `K; -K` form for the negative mask,
constant-as-destination — the first **four** instructions come out exactly right
and then:

```
rom    mov r3, #0x3 / and r1, r3 / mov r3, #0xd / neg r3, r3
ours   mov r3, #0x3 / and r1, r3 / sub r3, #0x10
```

`-13` is `3 - 16`, so once 3 is live in r3 gcc gets the second mask in one
instruction. Ten lines against the ROM's eleven, with **gcc strictly ahead**.

Moving the negation ahead of the mask does stop the derivation — and then the
two constants need separate registers, the sprite pointer is pushed out of r0
into r4, and eleven lines match in count with seven different in content.
**Neither ordering can have both.**

Declared as a bitfield:

```c
unsigned char lo : 2, sel : 2, hi : 4;
...
s->sel = v;
```

gcc's `store_bit_field` expands the mask, the shift and the merge itself, and
the two constants are generated as independent RTL that CSE never gets to
relate. Exact on the first screen — and it is the more plausible source anyway.

**Seven flags were probed first and all are negative:** `-fno-gcse`,
`-fno-cse-follow-jumps`, `-fno-cse-skip-blocks`, `-fno-expensive-optimizations`,
`-fno-strict-aliasing`, `-fno-rerun-cse-after-loop`, `-O1`. That is worth
recording as much as the fix — it is not a flag question, and without it this
would have been filed under the 19-file constant-CSE park class, where it does
not belong.

The rule is now in `docs/elevation.md`: try a bitfield whenever a function's
whole body is `x = (x & ~M) | (v << S)`.

## Sweeping the parks with `-fno-strict-aliasing`

Batch 69 found the flag on one function. Rather than assume its reach, all 179
parked files were re-screened with it:

| File | Before | With flag |
|---|---|---|
| `ovl_7a67d8/20082a0.c` | 2 of 22 | **match** |
| `ovl_7ec19c/200816c.c` | already `OK` | still blocked — pool **placement** |
| `rom_a1000/80ad5b4.c` | already `OK` | still blocked — pool **ordering** |
| `rom_b5000/80c24b0.c` | 17 | 16 |

One real unlock out of the scheduling class. **Not a general key** — worth
bounding the claim rather than leaving the flag looking like a master key.

### And it corrected a diagnosis

`OvlFunc_919_20082a0`'s park called the residue *"address-load sinking"*. That
was right about **what** moved and wrong about **why**: the scheduler may sink
that address load past the preceding store only because strict aliasing puts an
`int` store and a `short` load in different alias sets. Deny it that and the
order stands.

**Nothing in the source changed.** The park's statement-order work — every
constant and destination address assigned *before* the load it feeds, three
swaps taking it from 16 of 22 to 2 — was already correct and is kept verbatim in
the file comment. The flag closed the last two.

## `OvlFunc_common0_d4` — closing the seven-copy set

Deferred from batch 69 because its object is named by **nineteen** overlay
linker scripts. Done here: three pieces in all nineteen, with `.data` and
`.data1` following the `_c` piece since the data sits after the last function.
`OvlFunc_common0_0` needed a second cut of the same file later in the batch, so
`common0` is now four objects across those nineteen scripts.

## `OvlFunc_970_2008f30` — a DMA0 raster kickoff

Picks a 0x780-byte scanline table by an index byte, writes its first word
straight to `REG_BG3HOFS` — a 32-bit store, so it sets HOFS and VOFS together —
and hands the rest to DMA0 with the HBlank-repeat control word `0xa6600001`.

`DMA0_SET` was added to [include/dma.h](../include/dma.h): `DMA3_SET` with
`&REG_DMA0SAD` as its base. That matters because `UnknownDMAPrefix()` has
already loaded that address, so gcc CSEs the two and the function loads it once,
as the ROM does.

**The whole remaining difference was one statement's position.** With `dst`
assigned *after* the prefix call, r1 is free during it and gcc uses r1 as the
read-modify-write scratch; the ROM uses r4. Assigned *before*, r1 is spoken for
and the scratch moves to r4 — a free choice for gcc because `GCC296_CFLAGS`
carries `-fcall-used-r4`. Four lines, one reorder.

### A wrong prediction, tested rather than believed

This function has six pool constants and no inline pool of its own; its `=`
forms are resolved by a section-end ltorg shared with two later functions. I
reasoned that splitting it out alone would move those six words from the section
end into the middle of the section, and that `make compare` would fail — and
came close to parking it on that reasoning after batch 69's pool-ordering find
made the theory feel solid.

**It compares clean.** The rule I was about to write down does not hold, and the
ten-minute empirical check was worth more than the theory. Recorded because the
near-miss is the useful part: a fresh blocker class makes every adjacent
symptom look like the same thing.

## One park: `Func_80a3d9c`

A mask gcc will neither leave alone nor keep.

- As a **literal**, `(v & 0xf800) >> 11` folds to a bare `lsr r3, r2, #0xb` —
  gcc knows `v` came from an `ldrh` and is 16 bits, so the mask cannot change
  the result. 28 lines against the ROM's 32.
- Held in a **register** the AND survives, but gcc gets it there by **hoisting**
  it out of the loop, `mov r7, #0xf8 / lsl r7, #8` before the first iteration.
  31 lines, everything inside the loop displaced.

The ROM has it both ways at once: built inline inside the if-body on every
iteration, and the AND kept. Five flags probed and negative.

Three things that did work are kept in the parked source, the reusable one being
`mask & v` rather than `v & mask` — the same constant-as-destination lever as
the ORR in `Func_80ad5b4`. The one thing the source cannot express is the ROM's
`mov r3, r2` right after the `ldrh`, a copy of the loaded value that is compared
and immediately overwritten; `t = v;` is coalesced away. Same redundant-copy
shape as `GetUnit`'s `mov r3, r14`.
