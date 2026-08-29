# rom_77000 — party, character records and event flags

The game's data layer. It owns the character records, the derived stats, the
inventory and the save variables, and hands them out to everyone else. 100
exported functions and essentially no hardware use (four DMA3 setups) — nothing
here draws, it answers questions. All 117 functions are annotated; the module
header is in [rom_77320.s](../rom_77000/src/rom_77320.s).

`Func_77394` is the **most-called cross-module function in the ROM**, at 226
call sites. Seeing `_Func_77394` anywhere else means "fetch a combatant record".

## Combatant records — 0x14C bytes

`Func_77394(id)` resolves them:

```
ids 0x00..0x07   ewram_500 + id * 0x14C              the player's side
ids 0x80..0x85   [iwram_1f28] + id * 0x14C - 0xA600  the enemy side
anything else    0
```

The enemy bias is exact: `0x80 * 0x14C = 0xA600`, so enemy `0x80` lands at
offset 0 of the block `iwram_1f28` points at. That is the whole reason the ids
are split `0..7` / `0x80..0x85` rather than being contiguous. Enemy ids also
return 0 when `iwram_1f28` is null — i.e. outside battle.

Fields established from the accessors:

```
+0x0F   per-character byte, averaged across the party by Func_77348
+0x14   current HP as a 14-bit fraction, (curHP << 14) / maxHP, 0..0x4000
+0x34   max HP          +0x36  max PP
+0x38   current HP      +0x3A  current PP
+0x58   32 entries of 4 bytes, searched by Func_78bc0 on a 0x3FFF mask
+0xD8   15 inventory slots, 2 bytes each
+0x118  status byte array, entries validated against a bound of 9
+0x128  class index, 0..7 — indexes the ability class mask
+0x129  eligibility byte checked by Func_79008
```

`+0x14` is a fraction, not a percentage, so UI bars scale from it directly.
`Func_7822c` recomputes it and is called after every HP or PP change.

**`Func_783a4(id, signedDelta)` is the damage and healing entry point** — clamps
to `0..maxHP`, refreshes the fraction, returns the new HP. `rom_b5000` calls it
as `_Func_783a4(id, -damage)`. `Func_783dc` is the PP twin.

## The inventory halfword packs three fields

Each of the 15 slots at `+0xD8` is one halfword:

| bits | meaning | written by |
|---|---|---|
| 0..8 | item id (mask `0x1FF`) | `Func_78708` |
| 10 | equipped | `Func_78a34` sets, `Func_78a60` clears |
| 11..15 | quantity − 1 | `Func_788c4` decrements by `0x800` per unit |

`Func_784b0` returns `(h >> 11) + 1` as the quantity, or 0 when the id field is
empty. `Func_784d8` counts slots by scanning until the first empty one, so **the
inventory is kept compacted** — `Func_787dc` shifts the remainder down on
removal to preserve that.

Searches match on the low 9 bits only (`(slot ^ wanted) & 0x1FF`), so quantity
and the equipped bit never interfere.

## The save variable store at ewram_40

512 bytes, addressed at **four granularities** by the accessors in
[rom_79338.s](../rom_77000/src/rom_79338.s):

| view | functions | notes |
|---|---|---|
| bit | `Func_79338` test, `Func_79358` set, `Func_79374` clear, `Func_79390` toggle | 288 sites between them |
| byte | `Func_793b8` read, `Func_793c8` write | clobbers all eight bits |
| counter | `Func_793d8` inc, `Func_793f8` dec | **saturating** at 0xFF and 0, not wrapping |
| nibble | `Func_79418` read, `Func_79434` write | bit 2 of the index picks low or high |

All index the same byte as `(idx & 0xFFF) >> 3`, written `(idx << 20) >> 23` to
mask and shift in one pair. Because the views overlap, a "flag" and a "counter"
can share a byte — **do not assume an index is only ever used one way.**

**Indices 0..7 are party membership.** `Func_795fc` counts exactly those eight
bits to get the active party size, so bit N means "character N has joined". That
also matches the eight entries in the `Func_78ed8` base-stat table.

## Data tables

| accessor | table | stride | entries |
|---|---|---|---|
| `Func_773d8(id)` | `Data_80ec8` | 0x54 | ids 8..0x101, out-of-range → entry 0 |
| `Func_78414(id)` | `.L7b6a8` | 0x2C | ability records, id masked to 0x1FF |
| `Func_78b9c(id)` | `.L7ee58` | 0x10 | 520 display records, bounds-checked |
| `Func_78ed8(i)` | `.L844ec` | 0xB4 | **exactly 8** — one per playable character |
| `Func_797d4(i)` | `.L84a9c` | 8 | 16 element records |
| `Func_79ad8(i)` | `.L84b1c` | 0x54 | effect records — same stride as items, different table |
| `Func_7a0cc(r,c)` | `.L8926c` | 12 | a 4 × 20 grid |

`Func_78ed8`'s table runs `0x844EC..0x84A8C`, which is exactly 8 × 0xB4. It has
**no bounds check**, so the index must already be 0..7 — the one accessor here
that trusts its caller.

Ability record `+0x04` is an eight-bit mask of which classes may use it;
`Func_7842c` tests `(mask >> classIndex) & 1` and returns 0 for any class index
above 7.

## Two independent RNGs

`Func_79bc4` is a 16-bit LCG with the **same constants** as `rom_c0`'s
`Func_4458` (`0x41C64E6D`, `0x3039`) but a **different seed location**:
`ewram_23a8` rather than `iwram_1cb4`. So the two streams are independent, and
because this one lives in ewram it is part of the save state.

## Money

`Func_79700(delta)` adds to `ewram_240+0x10` and clamps to `0..0xF423F` —
**999,999, the gold cap**. Negative results clamp to zero rather than
underflowing.

## Where the formulas live

- `Func_77428` (1023 lines) builds the character summary — **derived stats are
  computed here**, so this is the function to read when a displayed number does
  not match a stored one.
- `Func_7905c` (293 lines) applies a level-up, rolling variance with
  `Func_4458`. Growth curves live here.
- `Func_78bf0` (320 lines) recomputes equipment effects, re-run by `Func_78708`
  after any inventory change.
- `Func_79f10` (220 lines) resolves an action — the damage-formula entry point.
- `Func_79ae8` is the standard "something changed, recompute everything" call:
  `Func_798e0` for derived stats, `Func_799b0` for elemental bonuses,
  `Func_78bf0` for equipment.

## Save block

`ewram_240` holds the save data. Known offsets:

```
+0x10   money, capped at 999,999
+0x1F8  the active roster, one byte per member
+0x205  the in-game hour (see rom_15000's Func_1ca1c)
```

## Depth

The accessors, the record and inventory layouts, the save-store views and the
table strides are traced and cross-checked. The five large formula routines
above are characterised from their call graphs and state use, and say "traced
structurally" in their own comments. Treat those as a map, not a specification.
