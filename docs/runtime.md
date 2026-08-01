# rom_c0 — the shared runtime

Everything else stands on this: the allocator, the task scheduler, the frame
loop, input, maths, decompression and the sound engine. It starts at ROM offset
0xC0, immediately after the header, and contains the boot code. 96 functions are
exported and all 228 are annotated.

Annotating it turned several things the other five writeups had *inferred* into
things that are *checked*. Two of those corrections are noted at the end.

## The allocator is two bump arenas, not a heap

This is the single most important thing to know before decompiling anything that
allocates.

A **64-word table at `iwram_1e50`** holds the whole allocator:

```
[iwram_1e50 + 0]      the EWRAM bump pointer
[iwram_1e50 + 4]      the IWRAM bump pointer
[iwram_1e50 + tag*4]  the pointer handed out for that tag
```

Slots 0 and 1 are the bump pointers themselves, so usable tags are **2..63**.
`Func_4858` resets the table and sets the bases:

| arena | range | size |
|---|---|---|
| IWRAM | 0x3002000 – 0x3007800 | 0x5800 |
| EWRAM | 0x2030000 – 0x2040000 | 0x10000 |

(0x2040000 appears as the literal `0x81 << 18`; 0x3007800 leaves 0x7800–0x7FFF
for the two stacks and the IRQ vector `_start` installs.)

**Four allocators, differing only in arena preference:**

| function | tagged | tries first |
|---|---|---|
| `Func_48b0(tag, size)` | yes | IWRAM |
| `Func_48f4(tag, size)` | yes | EWRAM |
| `Func_4938(size)` | no | IWRAM |
| `Func_4970(size)` | no | EWRAM |

All four round the size up to a word and return 0 when neither arena fits. A
tagged call whose slot is already occupied returns the existing pointer without
allocating — **tagged allocation is idempotent.**

### Func_2dd8 rewinds an arena; it does not free a block

It reads the tag's pointer, clears the slot, and stores that pointer back over
the bump pointer of whichever arena it came from — selected by `(ptr >> 22) & 4`,
since an EWRAM address gives `8 & 4 = 0` and an IWRAM address `0xC & 4 = 4`.

**Everything allocated after the freed block is silently reclaimed too**, so
freeing must happen in reverse allocation order. Every caller in the ROM does
exactly that — `rom_c9000`'s handlers release tag 0x2F then 0x2E, the reverse of
how they were generated. Preserve that ordering when decompiling; it is
load-bearing, not stylistic.

`Func_488c` and `Func_48a0` report the free space left in each arena.

## rom_770 executes from IWRAM, and its names lie

`stage1.ld` places that one section with `rom_770 : { ... } > iwram AT > rom` —
linked for IWRAM, loaded from ROM. `Func_300c` DMA3-copies all 0x500 words
(0x1400 bytes, exactly the 0x770–0x1B70 span) to `iwram_0` at boot and points the
IRQ vector at it.

So the `Func_<offset>` convention **breaks here**. Those names encode each
function's ROM *load* offset; the code runs at IWRAM addresses:

```
Func_770  -> 0x03000000      Func_888  -> 0x03000118
Func_1af8 -> 0x03001388
```

Confirmed with `arm-none-eabi-nm goldensun.elf`. This is the only section where a
symbol's name is not its run address — do not compute branch targets or
PC-relative offsets from names in that file. Everything in it is ARM and heavily
unrolled precisely because it runs from IWRAM at full speed.

## Division, and why it matters

ARM7TDMI has no divide instruction, so every division in the game funnels into
`Func_b6c`, a restoring binary long division returning the quotient in r0 and the
remainder in r1. Four thin wrappers pick which you get:

| | quotient | remainder |
|---|---|---|
| signed | `Func_af0` | `Func_b1c` |
| unsigned | `Func_b60` | `Func_b50` |

`Func_b1c`'s remainder takes the sign of the **dividend**, C-style. Confusing
`Func_af0` with `Func_b1c` changes behaviour silently — it already caused one
misreading in the `rom_c9000` pass, where a wrapping scroll had been written up
as a division.

`Func_8ac` divides in 16.16 by **reciprocal multiply**: `Func_af0(0x40000000, d)`
then multiply, rather than a second long division.

## The task table

20 slots of 8 bytes at `iwram_1a20`: `{function, priority halfword at +4, group
byte at +6}`. `Func_41d8(fn, pri)` registers and then re-sorts the whole table
with `Func_4144`, so **lower priority values run first** — `Func_40e8` fills empty
slots with 0xFFFF so they sort last. The values other modules pass (0x480, 0x4FE,
0xC80) are ordering hints, not frame counts. `Func_4420` runs every task whose
group byte matches `r0 >> 8`.

## The frame loop switches stacks

`Func_30f8(n)` is the heartbeat of every synchronous loop in the game. When the
caller's SP is below `iwram_79ff` it saves the difference at `iwram_1804`,
DMA3-copies the stack out to `ewram_23b0`, and moves SP to `iwram_7a00` for the
duration. That is why deeply nested game code — battle animations, nested menus —
can call it without overflowing the system stack.

While waiting it services the frame: `Func_3538` for input timing, `Func_4420`
for the task groups, `Func_3d04` / `Func_3e10` for the display lists, `Func_5fcc`
for sound.

## Input

`Func_3650` reads `REG_KEYINPUT` once and derives the three globals the whole ROM
polls:

```
iwram_1ae8   keys HELD, active-high (0x3FF ^ REG_KEYINPUT)
iwram_1c94   keys NEWLY PRESSED this frame
iwram_1b04   keys with AUTO-REPEAT applied
```

Repeat timing lives in `iwram_1b00`: 0x13 frames for the first repeat, then 6
(`Func_3538`). Key bits are the GBA's — 0 A, 1 B, 2 Select, 3 Start, 4 Right,
5 Left, 6 Up, 7 Down, 8 R, 9 L. So `rom_c9000`'s `& 3` tests mean "A or B held"
and `rom_b5000`'s `& 0x80` means "Down".

## Decompression, and the run-from-RAM idiom

`Func_5340` and its ten neighbours (`Func_5340`–`Func_567c`) all share one shape:
allocate a scratch with `Func_4938`, DMA3-copy a decoder **template** into it,
call it there, release with `Func_2df0`. `Func_2544` is the template `Func_5340`
installs, into 0x2C4 bytes.

The same idiom appears in three other modules with their own templates —
`rom_15000`'s `Func_1a5a4` installs `Func_15afc`, `rom_b5000`'s `Func_c08ec`
installs `Func_b5138`. Only `Func_b5138` has to relocate its own jump table,
because it is the only one with internal branches through a table.

`Func_d30` (LZ77) and `Func_dc8` (Huffman) are the in-place decoders, both in the
IWRAM-resident section.

## The OBJ tile allocator

A **96-slot table at `iwram_1b10`**, four bytes per slot, with the halfword at +2
set to 0xFFFF when free; `iwram_1810` is the companion run-length table.
`Func_3fa4(slot, size, flags)` reserves — rejecting a slot above 0x5F or a size
above 0x2000 — and `Func_3f3c` frees.

Every module's OBJ reservations come from here: `rom_15000`'s glyph nodes,
`rom_c9000`'s effect sprites, `rom_b5000`'s combatants. **A leak in one module
starves the others**, which is worth remembering when a sprite silently fails to
appear.

## The matrix stack

`Func_49ac` (identity) → `Func_4bd4` / `Func_4c1c` / `Func_4c6c` (rotate about
each axis) → `Func_4cb4` (scale) → `Func_5268` (project). `Func_4a28` stores the
current transform as 0x30 bytes — twelve words, a 3×4 matrix — and `Func_4a44`
replays it, which is how `rom_c9000`'s `Func_dc968` pre-builds 384 orientations
and tumbles debris without per-frame trigonometry.

`Func_2322` is sine over a 256-entry halfword table at 0x2344 (exactly 0x200
bytes); `Func_231c` is cosine and reaches it by adding a quarter turn and
**falling through** — both are `_noalign` so padding cannot break that.
`Func_44d0` is atan2 and `Func_948` / `Func_45a4` are integer square root in ARM
and Thumb.

## Two corrections to earlier writeups

**`Func_307c` is the generic interrupt registrar**, not a scanline trigger:
`Func_307c(irq, param, handler)` with the GBA's own IRQ numbering — 0 VBlank,
1 HBlank, 2 VCount. `rom_c9000`'s `Func_307c(2, 0x60, Func_ec0f0)` is arming a
**VCount match** at scanline 0x60. The earlier description was right about the
effect and wrong about the mechanism.

**`Func_41d8`'s second argument is a sort key**, and the table is re-sorted on
every registration. Earlier writeups called it a "priority" without saying that
lower runs first or that the ordering is enforced rather than incidental.

## Depth

The allocator, division core, task table, input, frame loop, OBJ allocator,
matrix stack and the boot-time IWRAM relocation are traced and cross-checked
against the linker script and `nm`. The sound engine (64 functions across four
files) is characterised from its call graph and marked "traced structurally" in
its own comments — the mixer and sequencer inner loops are not yet documented.

---

## Data_320000 — the asset and export dispatch table

`rom_320000` is data only: 1000 words, the table `Func_2f40(id)` indexes with no
bounds check. It is **not uniform** — the first nineteen entries are structural:

```
[0]      __start_rom          the ROM base
[1]      Data_320000          the table itself
[2]      .L2                  a small blob at 0x320FA0
[3..18]  the MODULE EXPORT TABLES:
         9000, 15000, 77000, 8a000, a1000, b0000, b5000, f0000,
         f2000, c9000, f4000, f6000, f9000, f9000, 185000, 185000
[19..]   the assets proper, one blob each; the last entry is 0
```

`f9000` and `185000` each appear twice.

### How cross-module calls actually work

A module cannot reach another with a short `bl`, so each publishes a table of
veneers. `.export_func Func_X` in `macros.inc` expands to
`.thumb_stub _Func_X, Func_X`, which is literally:

```
_Func_X:  ldr r4, =Func_X
          bx  r4
```

Those veneers are collected under `Exports_<module>`, and entries `[3..18]`
above point at them. **That is the whole reason for the `_Func_` prefix
convention** used throughout this project: a leading underscore means "through
the export veneer", the bare name is the real function. It is not a naming
style — the two symbols are different addresses.
