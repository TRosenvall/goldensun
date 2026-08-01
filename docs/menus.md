# rom_a1000 — the menu screens

`rom_a1000` is what you see when you press Start: the Psynergy, Item, Djinn and
Status screens, plus the character picker the map scripts borrow. It owns no
data of its own — it reads and writes character records through `rom_77000`,
draws everything through `rom_15000`, and shares its scratch block with
`rom_b0000`'s shops.

All 208 functions are annotated. The module header is in
[rom_a1050.s](../rom_a1000/src/rom_a1050.s).

## Five screens, one scaffold

`rom_15000`'s `Func_1c244` is the field menu. `Func_28920` draws it and returns
which entry was chosen, and `Func_1c244` dispatches five ways:

| entry | function | what it returns |
|---|---|---|
| 0 | `_Func_8ce74` | **not a screen** — the terrain code under the player |
| 1 | `Func_a5b94` | Items — a display id, mask `0x3FFF` |
| 2 | `Func_aa56c` | Djinn |
| 3 | `Func_a24d0` | Psynergy — an ability id, mask `0x1FF` |
| 4 | `Func_a7478` | Status |

`Func_a7380` is the sixth entry point: the character picker on its own. Nothing
in the main ROM calls it — four overlays import it.

The polarity of the dispatch is inconsistent and worth noting: entry 2 loops
the menu again on a return of **zero** while the others loop on **-1**.

Every one of the five opens the same way:

```
Func_48b0(0x37, 0xA70)          take the state block
[iwram_1e68]+4 = 1              tell the field engine a menu is up
_Func_170f8(0, 0, 0x1E, 0x14)   blank the whole 30x20 tilemap
Func_30f8(1) / Func_a1090(0)    one frame, then clear the block
_Func_796c4(state+0x208)        snapshot the roster
Func_a3354 or Func_a8034        build the party strip
_Func_162d4(0xD, 0, 0x11, ...)  open the main window into +0x10C
Func_a2144(0xE) / _Func_219c8   the palette and the sprite grid
Func_a2474()                    arm the Start watcher
  <the screen's own state machine>
Func_a2490()                    disarm it
... teardown mirrors setup and ends in Func_2dd8(0x37)
```

Each state machine is a `switch` on a register, dispatched through a word table
and looped from a shared tail. `Func_a2680` (Psynergy) has thirteen states,
`Func_a5cc0` (Items) five, `Func_a76d0` (Status) four.

## The state block is tag 0x37 — the same one the shops use

`Func_48b0(0x37, 0xA70)` appears five times here and again in `rom_b0000`. Only
one screen is ever up, so this is a shared scratch, not one block per screen;
the same offset means different things depending on who owns it. Since
`0x1F2C - 0x1E50 = 0xDC` and `0xDC / 4 = 0x37`, the symbol name confirms the tag
arithmetically.

The fields that are common to every screen are listed in the module header. The
ones worth knowing here:

```
+0x014  cursor sprite 0        +0x018  cursor sprite 1
+0x048  32 list sprite nodes   +0x0C8  8 element sprite nodes
+0x114  8 party actor pointers, x at +0x134 and y at +0x144
+0x1C8  the compacted list every renderer draws from
+0x208  the roster            +0x219  the roster count
+0x222  the "snap the cursor, do not glide" flag
```

## Two id spaces, and why confusing them is silent

This is the single most important thing to get right before decompiling
anything here.

| | ability id | display id |
|---|---|---|
| mask | `0x1FF` | `0x3FFF` |
| accessor | `_Func_78414` | `_Func_78b9c` |
| record | 0x2C bytes | 0x10 bytes |
| count | — | 0x208 |

`Func_a24d0` returns the first; `Func_a5b94` returns the second. Both pack their
answer the same way — `(memberIndex << 10) | id` — into adjacent halfwords of
the same block, so a mix-up type-checks perfectly and picks the wrong record.

String bases follow the ability id: `0x182 + id` is the item name, `0x53A + id`
the detail line, `0x75 + id` the description. Class names are `0x741 +
record+0x129`.

## Combatant record fields

The menus read more of the record than anything else in the ROM, so this is
where its layout gets pinned down. Established or confirmed here:

| offset | meaning | established by |
|---|---|---|
| `+0x00F` | level, capped at **0x63 = 99** | `Func_a8578` |
| `+0x034` / `+0x038` | max HP / current HP | `Func_a153c` |
| `+0x036` / `+0x03A` | max PP / current PP | `Func_a153c` |
| `+0x03C`, `+0x03E`, `+0x040` | three halfword stats | `Func_a112c` |
| `+0x042` | a byte stat | `Func_a112c` |
| `+0x058` | 32 entries × 4 bytes, ids masked `0x3FFF` | `Func_a68ec` |
| `+0x0D8` | 15 inventory halfwords | `Func_a3d6c` |
| `+0x0F8 + e*4` | djinn the character **has**, 20 bits per element | `Func_ac8fc` |
| `+0x108 + e*4` | the ones that are **set** | `Func_ac8fc` |
| `+0x124` | experience | `Func_a8578` |
| `+0x129` | class id | `Func_a112c` |
| `+0x130`, `+0x131`, `+0x140` | the status flags | `Func_a8b10` |

Two of those cross-check against work already done. `+0x042` is the byte
`rom_b5000`'s `Func_bf208` multiplies by 3 in its status-chance formula. And the
inventory halfword's layout —

```
bits 0..8    the item id, the same 0x1FF _Func_78414 masks with
bit 9        locked: equipped, or a key item
bits 11..15  the quantity, less one
```

— explains why `rom_77000`'s `_Func_788c4` decrements by `0x800`: that is one
unit, `1 << 11`.

The ability record's `+0x02` doubles as the equipment slot type. `Func_a9aec`
puts kind 1 on row 0x08, kind 2 on 0x38, kind 3 on 0x28 and kind 4 on 0x18 —
four kinds, four slots, matching the four labels `Func_a9a5c` draws.

## Djinn are stored as two bitmasks

`Func_ac8fc` walks four words at `record+0xF8` and four more at `+0x108`, twenty
bits used in each — so the structure allows **twenty djinn per element across
four elements**. Each entry comes out as `(element << 5) | index`, with bit 15
added when the djinn is set.

`Func_ae7fc` counts them per member with an OR of the two masks, which means it
is counting how many a character *owns*, not how many are set.

That count feeds a rule I did not expect to find: **`Func_ae778` refuses any
transfer that would leave two party members' djinn counts differing by more than
one.** It builds the counts, applies the proposed move, compares every pair, and
returns 0 the moment it finds a gap of 2 or more. `Func_ae714` uses it to mark
which members may not donate.

`Func_ab5e4` — 2322 lines, the largest function in the module — is the screen
that drives all of this, and it is also the only place outside a battle that
calls `rom_b5000`'s `_Func_bf5a8`, the routine that writes battle-side status
back into the persistent record.

## Small things worth keeping

**`Func_a4754` breaks items.** When the ability record's target kind is 2 it
rolls `Func_4458` and, if the 16-bit result is below `0x2000`, destroys the item.
That is exactly **one chance in eight**.

**`Func_a153c`'s low-HP colour** switches the ink to 4 when current HP has fallen
below a quarter of the maximum and to 2 when it is zero. The quarter is the
`lsl #16 / asr #18` pair, not a stored threshold.

**`Func_a2444` is how Start closes the menu from any depth.** It is a per-frame
task; on Start it plays sound `0x71`, sets save bit `0x150` and unregisters
itself. Every loop in the module polls `0x150` and unwinds.

**`Func_a8c2c` generates tiles rather than loading them.** It fills each 0x40-byte
tile in VRAM with `0x44444444` and XORs a diagonal in from `.Laf23c`, building
the stat-bar graph at runtime.

**`Func_a8904` is a busy-wait** — 255 iterations of nothing, with no `Func_30f8`,
so it burns cycles inside a frame rather than waiting for one.

**`Func_a1fd4`'s return value is tri-state**: 1 for a vertical move, 0 for a
horizontal one, -1 for no input. Callers test `> 0` to mean "the page changed".

## Corrections to earlier writeups

Four annotations from previous passes turned out to be wrong, and all four are
load-bearing.

**`Func_19000` is a tile plotter, not a clipper.** It was annotated as
`ClipToWindow`, "returns the clipped tile address". It actually writes one
tilemap halfword inside a window's interior, with the mode argument selecting a
palette bank to OR in — 0xE000 for mode 2, 0xF000 for 3, 0x1000 for 4, and mode
1 writing nothing at all. `rom_a1000`'s `Func_a21b0` draws its entire page bar
through it, one tile per call. 36 call sites.

**`Func_8ce74` is a query, not an action.** It was annotated as
`CheckPartyTouchTriggers`, "firing any that the party has stepped onto". It fires
nothing: it returns the terrain code under the player, gated by a height check
through `_Func_11f54` for codes `0xF2..0xF7` and by `Func_8d48c` for everything
else. Entry 0 of the field menu stores that code and leaves.

**`Func_28920` is the field menu, not a save screen.** It appends panels 1,
0x0F, 2 and 7 — dropping 0x0F when `_Func_7a5bc` reports an empty party, so the
menu is three rows instead of four — and translates between screen row and entry
number through the two byte tables `.L37403` and `.L373f7`.

**`Func_1c244`'s dispatch was described but not read.** The five arms and their
differing loop polarity are now written down.

## A source quirk that hides functions

Four functions in the ROM are declared with the wrong case:

```
rom_a1000/src/rom_a1814.s     .thumb_func_Start Func_a1c6c
rom_a1000/src/rom_a1814.s     .thumb_Func_start Func_a1f74
rom_8a000/src/rom_93304.s     .thumb_func_Start Func_942e0
rom_8a000/src/rom_97b54.s     .thumb_Func_start Func_97f80
```

GAS accepts them and the bytes are identical, but **a case-sensitive scan of the
sources misses them entirely**. That is how `rom_8a000` was reported complete at
416/416 when it actually has 418 functions, and why `rom_a1000` looked like 206
rather than 208. Every tool that walks these sources needs `re.I`.

Separately, `Func_a23f4` has no `.func_end`, so it gets no ELF `.size`. Harmless
— the next `.thumb_func_start` realigns — but a size-based tool will report it
as zero length.

## Depth

The scaffold, the state block layout, the two id spaces, the record fields, the
djinn masks and the balance rule, and every helper under about 200 lines are
traced and cross-checked. The eleven screen loops over 300 lines say "traced
structurally" in their own comments: their state graphs and the data they touch
are read, the individual drawing arms are not. Treat those as a map.
