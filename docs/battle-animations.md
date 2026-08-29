# rom_c9000 — the battle animation layer

`rom_c9000` draws battle actions. `rom_b5000` owns the turn logic and the
numbers and calls into this module through two entry points in
[rom_d6504.s](../rom_c9000/src/rom_d6504.s); everything here is presentation.

All 256 functions carry annotations. The twelve animation-class handlers reached
from `Func_d6578`'s dispatch table have been traced instruction by instruction;
the rest vary in depth and say so in their own comments.

## The allocation table is the key to reading this module

`Func_48b0(tag, size)` allocates and `Func_2dd8(tag)` frees. The pointer lives at
**`iwram_1e50 + tag*4`**, so the symbol a handler dereferences tells you which
tag it wants.

`Func_2dd8` does not free a block — it **rewinds a bump arena** (see
[runtime.md](runtime.md)). That is why these handlers always release 0x2F before
0x2E: reverse allocation order is required, not stylistic. The three allocations `Func_d6578` makes land adjacently:

| tag | symbol | size | what it is |
|---|---|---|---|
| 0x27 | `iwram_1eec` | 0x782C | battle-animation state block |
| 0x28 | `iwram_1ef0` | 0x4000 | destination render buffer (128×128, 8bpp) |
| 0x29 | `iwram_1ef4` | 0x302 | sprite scratch |
| 0x2C | `iwram_1f00` | — | control block |
| 0x2E | `iwram_1f08` | varies | generated blitter A |
| 0x2F | `iwram_1f0c` | varies | generated blitter B |

This is why handlers reach the blitters as `[iwram_1ef0 + 0x18]` and `+0x1c`, or
equivalently `[iwram_1f00 + 8]` and `+0xc`, or `iwram_1e50 + 0xB8` / `+0xBC`.
All three idioms appear; they are the same two pointers.

Notable fields in the 0x782C state block:

```
+0x77A8          screen-shake request (counts down)
+0x77AC/+0x77B0  shake amount and mode
+0x77D8..+0x7807 effect-actor array, filled by Func_dbb24
+0x7818          per-combatant byte array
+0x7824          set to 1 each frame to arm the Func_cd260 effect task
+0x7828          the current action descriptor
```

Action descriptor fields the handlers use: `+0x00` animation class, `+0x04` a
side/direction selector, `+0x08` the acting combatant, `+0x14` the target count,
`+0x24` onward an array of halfword target ids.

## Func_ed408 generates code at runtime

This was previously annotated as a sequence player. It is not. It:

1. Walks its flag word and pixel mode **adding up how many words of code it is
   about to emit** — the long chain of `add r1, #N` is a size calculation, not
   arithmetic on pixels.
2. `lsl r1, #2` converts words to bytes and `Func_48b0` allocates the buffer.
3. DMA3-copies fragments from the templates at `Data_edcc4` / `Data_edcb8`,
   including or skipping each according to the flags.
4. **Back-patches the branches it emitted.** `(target - site - 8) >> 2` masked
   and OR'd into a placeholder is the ARM B/BL offset encoding — this is the
   detail that settles it.
5. Returns 1 if the emitter walked its template table to the end, else 0.

The result is a sprite blitter specialised to one pixel format, blend mode and
size, called as `blit(dst, srcGfx, x, y, w, h)` with **x, y naming the centre**.
Only tags 0x2E and 0x2F are ever passed (111 and 121 sites).

Three independent facts confirm the reading:

- **Flag bits 3:2 are the mirroring mode.** `Func_ea0d8` generates the same
  blitter four times per frame with flags 3, 7, 0xB, 0xF — differing only in
  that field, which is exactly what `Func_ed408` branches on — and blits one
  sprite into four quadrants around a centre near (0x3C, 0x50).
- The generated pointer is read straight back out of the tag's allocation slot,
  which is why the `[iwram_1ef0 + 0x18]` idiom works at all.
- `Func_cef64` picks between already-generated pairs rather than generating new
  ones, and every handler that calls `Func_ed408` frees the same tags at exit.

Most handlers generate two blitters at setup and free them at teardown.
`Func_eb754` generates only one. `Func_ea0d8` and `Func_e15e8` regenerate one
**inside the frame loop**, paying a full code-generation and allocation per
blit.

## The two sprite-size tables

Both are cumulative byte offsets into a decompressed sheet, and both are
self-checking — each entry is the running sum of the previous sprite's area.

**`Data_ede48`** indexes the tag-0x29 buffer. Size *n* is *n* wide by *2n* tall,
so entry *n* is the sum of 2k² for k < n:

```
0, 2, 10, 28, 60, 110, 182, 280, 408, 570
```

Ten sizes ending at 570 + 200 = **770 = 0x302, exactly the buffer size**. That
coincidence is the proof the table and the tag map are both read correctly.

**`Data_ede5c`** indexes `[iwram_1eec] + 0x4E20` with *square* sprites of side
*2n*, so its deltas are 4n²:

```
0, 4, 20, 56, 120, 220, 364, 560, 816, 1140
```

Many handlers also carry small local `{offsets, widths, heights}` triples for
scripted sequences. In every case the offsets are that table's own running
`w*h`, which is a cheap way to check you have paired the tables correctly.

## Shared vocabulary

Helpers used across the module, with what they actually do:

| function | contract |
|---|---|
| `Func_2f40(id)` | asset pointer = `Data_320000[id]` |
| `Func_1af8(dst, src, n)` | ARM fast memcpy |
| `Func_5340(src, dst)` | decompress — DMAs `Func_2544` into a 0x2C4 scratch and runs it from RAM |
| `Func_e0524(id, dst, a, b)` | load and unpack an asset |
| `Func_48b0` / `Func_2dd8` | allocate / free by tag |
| `Func_41d8(fn, pri)` / `Func_4278(fn)` | register / unregister a task |
| `Func_307c(irq, param, fn)` | register an interrupt handler; GBA IRQ numbering, so 2 is VCount and `param` is the scanline |
| `Func_4458()` | 16-bit LCG random, seed at `iwram_1cb4` |
| `Func_2322(a)` / `Func_231c(a)` | sine / cosine, 0x10000 = full turn |
| `Func_44d0(x, y)` | atan2 |
| `Func_948(v)` | integer square root |
| `Func_af0(a, b)` | signed **quotient** |
| `Func_b1c(a, b)` | signed **remainder** |
| `Func_b50(a, b)` | unsigned remainder |
| `Func_8d4(dst, n)` / `Func_8d8(dst, n, v)` | fill |
| `Func_30f8(1)` | advance one frame — every handler's heartbeat |
| `Func_dbb24(n, res, pri)` | spawn *n* effect actors into `+0x77D8` |
| `Func_e3908(e, drag, grav)` | integrate a particle; drag is *k*/64 |
| `Func_e3944(vec3, out)` | project a 3-vector to screen |
| `Func_e3980(id, out)` | a combatant's screen position |
| `Func_d6750(desc)` | collect living combatants |
| `Func_d6888(id, …)` | make a combatant react |
| `_Func_b168(actor, pos, scale, style)` | submit a 3D sprite |
| `_Func_bd7dc(code)` | raise the one-shot flag at `[iwram_1e74]+0x800`, code at `+0x820` |
| `_Func_b8228(id, anim)` | set a combatant's battle animation |
| `_Func_bc70` / `_Func_ba30` / `_Func_bdd4` | create / animate / destroy an actor |

`Func_dbb98` is a bare `bx lr`. `Func_d655c` calls it in a loop to "advance N
frames", so that loop spins without yielding — do not assume callers of it wait.

## How a class handler is shaped

Every one of the twelve is **synchronous**: it runs its own frame loop, calls
`Func_30f8(1)` once per frame, and returns only when the animation is over.
Holding A or B (`iwram_1b04 & 3`, bits 0 and 1 of the GBA key state) jumps the
frame counter forward. The common skeleton:

```
setup      grab the buffers, Func_cd594, generate blitters, unpack assets,
           set display registers, seed particle pools, spawn actors
loop       fast-forward check, sounds by frame, per-frame drawing and physics,
           +0x7824 = 1, Func_30f8(1)
teardown   unregister tasks, free blitters, destroy actors, Func_cdbc0
```

| class | handler | frames | shape |
|---|---|---|---|
| 1 | `Func_e823c` | 320 | seven-piece figure, bouncing trails, final burst |
| 2 | `Func_d2d98` | 124 | particle eruption over a scrolling ground band |
| 3 | `Func_eb754` | 120 + 96 | ascent, then a descending wave front |
| 4 | `Func_dc968` | 220 + 88 | scripted flight, 16 airbursts, then a shower |
| 5 | `Func_d6970` | 366 | wave, scatter-dissolve, reveal |
| 6 | `Func_ec100` | 244 + 144 | a figure flies in, then five beams fall |
| 7 | `Func_d2458` → `Func_d2464` | 208 | downpour, then a burst |
| 8 | `Func_d1714` | 400 | vortex, summon, four impacts |
| 9 | `Func_ea0d8` | 160 + 320 | four-way mirrored figure |
| 10 | `Func_d765c` | 288 + 146 | bombardment on fixed sites, then debris |
| 11 | `Func_e7320` → `Func_e7404` | 192 + 54 | scanline gradient sweep, then a 3D burst |
| 12 | `Func_e15e8` | 170 + 192 | a charge, then a staggered eight-lane strike |

Class 0 falls through to the class 11 arm, so `Func_e7320` is the default
animation as well.

## Techniques worth knowing

- **Pre-built brightness ramps** (`Func_ea0d8`, `Func_e15e8`). The 0x302 sprite
  sheet is copied eight times to `[iwram_1eec] + 0x2710 + 0x302*k`, each byte
  clamped to a ceiling of `0x40 - 7k`. Fading is then a pointer change.
- **Shuffled permutation dissolve** (`Func_d6970`). Eight 128-byte blocks are
  filled 0..127 and shuffled by 128 random swaps, then used as the scatter order
  for erasing the render buffer.
- **Palette desaturation** (`Func_d6970`). Every fourth frame, each palette
  entry's R, G and B are stepped one unit toward their mean.
- **Split-scale background** (`Func_ec100`). `Func_ec0e0` restores
  `REG_BG2PA = 0x100` per frame while `Func_ec0f0`, armed on a **VCount-match
  interrupt** via `Func_307c(2, line, fn)`, drops it to 0x80 — so BG2 squashes
  below the split. Moving the split moves the fold.
- **Scanline gradient** (`Func_e7404`). A 160-entry halfword table at
  `[iwram_1eec]+0x1F80` is rewritten each frame and fed to hardware by the task
  `Func_e72e0`; the frame counter is folded into the colour so the band sweeps.
- **Recorded motion paths** (`Func_dc968`). Asset 0xD2 is read as one absolute
  16-bit pair followed by signed byte deltas, two per frame.
- **Pre-built 3D transforms** (`Func_dc968`). 384 random orientations are built
  once with `Func_49ac`/`Func_4c6c`/`Func_4bd4`/`Func_4c1c`, stored 0x30 bytes
  each at `ewram_13800`, and replayed with `Func_4a44` — no per-frame trig.
- **HBlank register queue** (`Func_ea0d8`). Entries of `{value, register,
  control}` are pushed to `ewram_2090` under an `REG_IME` guard, capacity 0x20.

## Anomalies to resolve before decompiling

- **`Func_dc968` never calls `_Func_bd7dc`** — alone among the twelve. Whatever
  resumes `rom_b5000` after class 4 must do it another way.
- **`Func_ea0d8` contains unreachable code.** A 128-entry 3D swarm sits behind
  `if (frame < 0)`. The frame counter is written in exactly seven places — `0`,
  `+1`, `0`, and the fast-forward targets `0x96`, `0xD6`, `0x118` — none
  negative. It is still seeded during setup.
- **`Func_e15e8` calls `Func_e0524(0xBC, …, 1, 1)` twice in a row** with
  identical arguments. Redundant unless that function is not idempotent.
- **`Func_e823c` aborts on A/B** rather than fast-forwarding; there is no
  "skip to frame N" arm at all.
- **`Func_e7404` registers `Func_cd260` and unregisters it immediately**, with
  only `Func_cd104(0, 0)` between the two calls.
- The six-frame splash tables appear **twice in the ROM at different addresses**
  — `.Lee18e`/`.Lee1A0` in `rom_d244c.s` and `Data_ede9f`/`Data_edeb2` used by
  `rom_dbbdc.s` and `rom_e0564.s` hold the same values.

## Verifying an annotation pass

`@` comments in GAS assemble to nothing, so annotating provably cannot change
the ROM — but check it rather than assume it:

```sh
cp rom_c9000/src/rom_x.o /tmp/ref.o
gmake rom_c9000/src/rom_x.o
cmp /tmp/ref.o rom_c9000/src/rom_x.o
```

Then `gmake clean && gmake compare` for the whole build. Two traps, both hit
during this pass:

- Deleting a `.thumb_func_start` line while editing the comment above it fails
  as `.size expression for Func_x does not evaluate to a constant` — the
  function's symbol is gone, so `.func_end` cannot compute its size.
- **zsh does not word-split an unquoted `$FILES`.** A loop over a variable
  holding a space-separated list passes one giant string; `gmake` then tries to
  link a bogus target. Use a literal list.
