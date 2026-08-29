# rom_b5000 — battle logic

`rom_b5000` owns a battle: the turn order, the action resolution, the numbers.
It reads party data from `rom_77000`, tells `rom_c9000` which animation to play,
and drives the menus through `rom_15000`. If `rom_c9000` is what a battle *looks
like*, this is what a battle *is*.

All 215 functions are annotated. The module header is in
[rom_b5138.s](../rom_b5000/src/rom_b5138.s).

## Entry point and the tag map

`Func_b63c8(encounterId)` sets a battle up and runs it. Its five allocations are
what make the module legible — pointer at `iwram_1e50 + tag*4`, as everywhere:

| tag | symbol | size | what it is |
|---|---|---|---|
| 0x09 | `iwram_1e74` | 0x82C | the battle state block |
| 0x0B | `iwram_1e7c` | 0x280 | |
| 0x0C | `iwram_1e80` | 0x4C | view / camera, shared with `rom_c9000` |
| 0x2C | `iwram_1f00` | 0x20 | |
| 0x36 | `iwram_1f28` | 0x7C8 | **the enemy records** |

That last one settles a question `rom_77000` left open. `0x7C8 / 0x14C` is
**exactly 6**, so the enemy block holds six combatant records — which is why
`Func_77394` accepts enemy ids `0x80..0x85` and no more, and why it returns 0 for
them outside battle: `iwram_1f28` is null until `Func_b63c8` allocates it. The
block is cleared with `Func_8d4` immediately after allocation.

## Two record types, do not confuse them

| | `rom_77000` | `rom_b5000` |
|---|---|---|
| accessor | `Func_77394(id)` | `Func_b7dd0(id)` |
| size | 0x14C | 0x2C |
| lifetime | persistent, saved | one battle |
| call sites | 226 | 88 |

`Func_b7dd0` takes two indirections, and the layout falls straight out of them:

```
ids above 7 fold down by 0x78, so 0x80..0x85 become 8..13
[iwram_1e74] + 0x2DC + foldedId   a slot byte, 0xFF meaning "absent"
[iwram_1e74] + 0x74 + slot * 0x2C the record
```

So `+0x2DC` is a **14-entry id→slot map** — 8 player and 6 enemy, matching the
six enemy records exactly — and `+0x74` is the record array. Known fields:
`+0x0C` and `+0x10` are the field x and z (`Func_b7eb4` reads them and always
zeroes y, so battle positions are planar), `+0x14` is the actor pointer.

## The hand-back to rom_c9000

`Func_bd7dc(code)` is a **one-shot latch**:

```
if [iwram_1e74]+0x800 is already non-zero: do nothing at all
otherwise: set it to 1, and if code != 0 store code at +0x820
```

`rom_c9000`'s animation handlers call it at their hit frame — 85 sites — which
is how "the blow has landed" gets back to the turn logic. Only the first call per
frame wins, so an animation cannot double-trigger. Codes seen from `rom_c9000`
are 0x84, 0x85, 0x86, 0x8F, 0x91 and a few one-offs; 0x86 dominates.

Applying the result goes back out to `rom_77000`: `_Func_783a4(id, -damage)` for
HP, `_Func_783dc` for PP.

## The decompressor that relocates itself

`Func_b5138`'s first loop computes the delta between where its jump table was
assembled and where the code is actually executing, and rewrites eight entries
accordingly. That is necessary because `Func_c08ec` **DMA3-copies the function
into a 0x230-byte scratch under tag 0x31 and calls it there** rather than calling
it in place. Same run-decompressor-from-RAM idiom as `rom_c0`'s `Func_5340` and
`rom_15000`'s `Func_1a5a4`, but this is the only one that has to be position
independent.

## Queues and tables

**The action queue** at `[iwram_1e74]+0x58` is a halfword list walked by
`Func_b6c08`: `0xFF` ends it, `0xFE` separates groups within it.

**The event log** is two parallel arrays filled by `Func_bbabc(code, payload)`:

```
+0x6B8 + n      the code, one byte per entry
+0x6F8 + n*4    the payload, one word per entry   (0x6B8 + 0x40)
+0x7FC          the entry count
```

Nothing bounds that count — the caller is responsible for not overrunning.

**The sprite table** `.Lc7420` is 172 entries of 8 bytes: it runs
`0xC7420..0xC7980`, and `0x560 / 8 = 0xAC`, exactly matching the `0xAB` bound
every accessor checks. `Func_c2384` reads the halfword at +0, `Func_c23c0` bit 0
of the byte at +2, `Func_c2454` the byte at +4.

**Front-line sizes.** `Func_b6a60` caps the player's side at 4, or 3 when
`[iwram_1e74]+0x44` is set — so `+0x44` is the battle-mode flag that shrinks the
active row. `Func_b6ae0` caps the enemy side at 6, matching the record count,
dropping to 3 when save bit 0x16C is set.

## Status chance is literally a percentage

`Func_bf208(id, tier, bonus)` computes

```
threshold = ((record+0x42) * 3 - tier * 5 + bonus) * 0x28F
lands if threshold >= (Func_79bc4() & 0xFFFF)
```

`0x28F * 100 = 65500`, just under `0x10000`. So the scaled value is a percentage
against a 16-bit roll, and the pre-scaling expression is a **percent chance**. A
tier above 5 always fails.

Ten near-identical routines `Func_bf250..Func_bf4c4` tick the per-combatant
status counters, each differing only in which record offset it decrements;
`Func_bf5a8` writes the battle-side status back into the persistent record so it
survives the battle.

## The debug harness

`Func_b56e0` is **not the shipped entry path**. It loops forever starting
battles, and holding **Down** (`iwram_1ae8 & 0x80`) drops into a live encounter
picker driven by the auto-repeat key state:

| key | effect |
|---|---|
| Right / Left | step the encounter id by 1 |
| Up / Down | step it by 10 |
| L / R | cycle party presets through `Func_b5368` |
| Start | `Func_b5534`, a raw record editor |
| Select | `Func_c2a08` — **an empty `bx lr`**, so that feature was stripped |
| A | launch `Func_b63c8` with the id showing |

Without Down held it just calls `Func_b63c8(0x101)`. Confirming also sets save
bits 0x16C, 0x16E (when the id is exactly 0x1C) and 0x162. Treat every bit this
function sets as debug state, not game progression — `Func_b6ae0` reads 0x16C to
cut the enemy count to three.

`Func_b5534` writes menu values straight into combatant 0's record byte by byte,
which is not something the shipped game does.

## Where the formulas live

- **`Func_bae40`** (938 lines) — the battle damage formula. `rom_77000`'s
  `Func_79f10` does generic resolution; this is the battle-specific layer on top.
  Read this before trusting any derived damage number.
- **`Func_bbb0c`** (2874 lines) — the battle main loop, turn to turn.
- **`Func_be378`** (1658 lines) — the player's command input.
- **`Func_b8574`** (189 lines) — enemy action selection, i.e. the AI.
- **`Func_bd424`** (467 lines) — action choice, tying the above together.
- **`Func_bd898`** (902 lines) — HUD and cursor animation.

## An unresolved aliasing question

`Func_b9a44` / `Func_b9a70` index a battle state halfword by the **low nibble**
of the combatant id:

```
player (bit 7 clear)  [iwram_1e74] + 0x58 + 2*(id & 0xF)
enemy  (bit 7 set)    [iwram_1e74] + 0x66 + 2*(id & 0xF)
```

Player id 7 resolves to `+0x66`, and enemy id `0x80` also resolves to `+0x66`.
Either the player side is only ever indexed 0..6 in practice, or this is a latent
aliasing bug. **Unresolved** — worth settling before these two are decompiled.

## Depth

The record layouts, the tag map, the queue and table formats, the latch and the
status-chance arithmetic are traced and cross-checked. The large routines listed
above are characterised from their call graphs and state use and say "traced
structurally" in their own comments. Treat those as a map, not a specification.
