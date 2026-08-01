# The rom_f* modules — sound, title, credits, minigames

Five modules, 161 functions, all annotated. They are grouped here because they
sit next to each other in the ROM, not because they belong together: `rom_f9000`
is the sound driver every other module leans on, and the other four are
self-contained screens that only the boot overlay and the area dispatcher reach.

| module | functions | what it is |
|---|---|---|
| `rom_f0000` | 11 | the staff credit roll |
| `rom_f2000` | 18 | title screen, logo splashes, palette-fade engine |
| `rom_f4000` | 7 | a wagering minigame — pseudo-area 0x1FD |
| `rom_f6000` | 15 | a prize minigame — pseudo-area 0x1FC |
| `rom_f9000` | 110 | the sound driver |

## rom_f9000 is Nintendo's M4A driver

Not a lookalike — the actual SDK driver, with a thin game layer on top. Three
pieces of evidence pin it down, and none of them is a guess:

- The driver block at `iwram_7ff0` begins with **`0x68736D53`, the ASCII
  `'Smsh'` identifier**, and every public entry point checks it before touching
  anything.
- **The track command base is `0xB1`.** `Func_f9c90` subtracts it to index the
  36-word jump table at `.Lfb7a0`, so `0xB1` is FINE, `0xB2` GOTO, `0xB3` PATT,
  `0xBB` TEMPO, `0xBD` VOICE, `0xC0` BEND — the published M4A command set,
  confirmed against the field offsets each handler writes.
- `Func_f9a80` copies that whole table into the player, which is how the engine
  calls its own commands and helpers by index rather than by address.

That means the record layouts are known rather than inferred, and they're
written into the module header: the player at `+0x1C`/`+0x1E`/`+0x20` (base
tempo, scale, effective rate), the track at `+0x0A` key shift, `+0x0C` tune,
`+0x0E` bend, `+0x12` volume, `+0x14` pan, `+0x40` command pointer, `+0x44` a
three-deep pattern stack.

The flag byte at track `+0x00` is worth singling out: **bits 0 and 1 mean
"volume or pan changed" and bits 2 and 3 "pitch changed"**. Every command
handler raises the pair matching what it wrote, and `Func_f9f6c` is what acts on
them. That's why `Func_f9ba4` (volume) sets `|= 3` and `Func_f9bcc` (bend) sets
`|= 0xC`.

### The game's interface is one function

`Func_f9080(id)` has **1971 call sites** — more than any other function in the
ROM, by a wide margin. It routes by range:

```
0x011        fade the main player out over 7 steps
0x121        fade the secondary player out over 3
0x012        ignored outright
0x050..0x063 jingle: fade the music, play, count down, restore
0x064..0xFFF sound effect
otherwise    music, skipped if already playing
```

**Bit 12 of the argument makes music start silent** so it fades in rather than
cutting in. That one flag explains a lot of call sites that otherwise look like
duplicates.

Two tables drive it: `Data_fc624` is the player table (8 entries × 12 bytes) and
`Data_fc684` the song table (8 bytes each — header pointer at `+0`, player index
at `+4`). **Player 7 is the shared effect player**: `Func_f9080` scans players 7
down to 4 for one whose status word is zero and takes the first free one, falling
back to 7 when they're all busy.

### Two robustness measures worth copying

`Func_f9a98` is a **guarded byte read** — it returns zero for a pointer outside
the expected range, and every stream reader goes through it. A corrupt song
reads as zeros instead of faulting.

**Eleven of the 36 jump-table entries point at `Func_f9a50`**, which simply ends
the track. An unimplemented command byte stops the track rather than running off
into the data.

`Func_fa9a4` does something similar from the other direction: when it stops the
FIFO DMAs to reprogram the timer, it *damages the ident* by subtracting 10 from
it, so every guarded entry point refuses to run until it's restored. The engine
locks itself with the same check that validates callers.

### One thing that never runs

`Func_f92fc` is a **debug sound test** — it steps through ids on the d-pad and
plays each. Nothing calls it; it's reachable only through its export veneer.
That's the fourth development leftover found in this project, after
`rom_b5000`'s encounter picker, its stubbed-out Select handler, and
`rom_b0000`'s 200,000-coin fixture.

## rom_f0000 — the credits have their own font

`Func_f07f0` rasterises 1bpp 8×8 glyphs out of `.Lf1770`, advancing by the
widths in `.Lf11bd`, and indexes both by `character - 0x20` — **plain ASCII**.
`rom_15000`'s Huffman text system is not involved at all, which is why the
credits are English-only.

It draws each glyph twice: colour 1 at an offset of `0x101` — one pixel right
and one row down — and colour `0x0F` at the glyph position. That's a drop
shadow, done in the rasteriser rather than with a second pass.

The scroll is **sprite-based, not tilemap-based**. `Func_f0678` lays out a 15×6
grid of OBJ entries, `Func_f0538` rebuilds the whole OAM every frame from the
scroll position at `ewram_4c00`, and `Func_f0614` renders the next line into a
32-row ring buffer as each row scrolls off. One 8-pixel row every 32 frames.

Behind it, 33 images crossfade with `BLDALPHA` between two background pages —
one showing while the next decompresses into the other.

**`Func_f0024` is byte-for-byte `rom_b5000`'s `Func_b5138`**, differing only in
that the per-word bias is a parameter rather than the constant `0x60606060`.
Same relocating jump table, same bit stream, same block walk. That makes **four
independent copies** of the run-decompressor-from-RAM idiom:

| module | installer | template |
|---|---|---|
| `rom_c0` | `Func_5340` | `Func_2544` |
| `rom_15000` | `Func_1a5a4` | `Func_15afc` |
| `rom_b5000` | `Func_c08ec` | `Func_b5138` |
| `rom_f0000` | `Func_f02b0` | `Func_f0024` |

## rom_f2000 — a fade engine that works on unpacked colour

The title screen and the two logo splashes, but the reusable part is the
palette-fade engine (`Func_f377c` through `Func_f3898`).

**It expands 512 packed colours into 1536 halfwords, one per channel**, so an
interpolation can carry a fractional value per channel that 5-bit packed colour
cannot represent. `Func_f2ebc` computes `(target - current) / frames` per
component, `Func_f3078` converts in both directions with clamping, and
`Func_f2f10` pushes the result out through the `ewram_2090` transfer queue at
VBlank rather than writing palette RAM directly.

Both splash screens animate through **an HDMA on `BG0HOFS`** fed from a
per-scanline offset table — a horizontal shear, with nothing actually moving.

## rom_f4000 and rom_f6000 — the pseudo-areas

`rom_8a000`'s `Func_8a8e4` (EnterArea) treats **area ids above `0x1FA` as
special screens rather than maps**:

| id | what it does |
|---|---|
| `0x1FB` | nothing |
| `0x1FC` | `_Func_f6008` |
| `0x1FD` | `_Func_f4008` |
| `0x1FE` | `_Func_b63c8` — starts a battle |

That's a clean piece of engine structure that neither module's own code makes
obvious. The caller saves and restores the palette at `0x50001C0` around both
calls.

The two minigames differ in what they cost. **`rom_f4000` stakes money** —
`_Func_79700(-bet)`, clamped against the balance first so it can never go
negative, with `_Func_77348` (party average level) scaling the difficulty.
**`rom_f6000` costs an item** — `_Func_78b60(0xE4)` checks the party has item
`0xE4` and `_Func_789dc(0xE4)` consumes one.

`rom_f4000` renders in perspective, building its transform in the camera block
at `iwram_1e80` — the same block `rom_b5000` allocates under tag `0x0C` and
shares with `rom_c9000`. Three modules agree on that layout; now four.

`rom_f6000` calls `rom_c9000`'s `_Func_ed408` twice — the **runtime code
generator** that assembles a specialised sprite blitter and back-patches its own
branches. Outside `rom_c9000`, this module is its only user.

### 1150 lines of dead code

**`Func_f7f78` is a dictionary decoder in the LZW family — and nothing
references it.** No `bl`, no load of its address, anywhere in the ROM. Its five
helpers (`Func_f7db4`, `Func_f7df0`, `Func_f7e34`, `Func_f7e60`, `Func_f7f30`)
are reachable only from it.

It is a genuinely different algorithm from the `Func_2544` family above: 0x400
codes chained through a doubly-linked structure, versus a bit-stream decoder with
no dictionary at all. Someone wrote a second compression scheme and it never got
wired up.

## Depth

The M4A identification, the command base and jump table, the record layouts, the
song and player tables, `Func_f9080`'s ranges, the credit font, the fade engine's
unpacked representation, the pseudo-area dispatch and the dead-code finding are
all traced and cross-checked. The mixer (`Func_f9674`), the PSG driver
(`Func_fae58`), the sequencer body (`Func_f9c90`) and the two minigame loops say
"traced structurally" in their own comments — their control flow and the state
they touch are read, the inner loops are not. Treat those as a map.
