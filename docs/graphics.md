# Assets: where they live, how they are packed, and what editing one would take

Written in batch 332 from a direct investigation, not from how other GBA
decomps do it. Every claim below is either measured here or cites a file and
line. The two places it says "not established" are the ones to attack first.

## One indirection for every asset in the game

    void *GetFile(int index) { return gFileTable[index]; }

That is the whole API — `src/rom_c0/rom_2e00_c_b.c:27`, declared in
`include/file_table.h`. `gFileTable` is built by
`asm/rom_320000/rom_320000.s` as a pointer table (`.word file_N`) followed by
the blobs, via **957 `.ft_include` entries**.

`macros.inc:70` expands `.ft_include` to a plain `.incbin`, so the **860
`file_table/*.raw` files ARE the ROM's bytes** — and `make compare` passing is
the proof. Edit one and the ROM changes.

Two properties that matter for editing:

- **The pointer table is assembler-computed.** A blob may change SIZE and every
  offset after it follows automatically. Only **58 of the 957** entries pin a
  size with explicit trailing padding bytes.
- **Appending is mechanical.** A new `.ft_include` line yields a new index that
  `GetFile` can return.

### The other 1.8 MB is NOT extracted

534 `.incrom` sites pull **1,820,610 bytes straight out of `baserom.gba` by
offset** (`macros.inc:42`: `.incrom s,e` is `.incbin "baserom.gba", s, e-s`).
Nothing there is editable until someone carves it out into files. Whether any
graphics live in that region is **not established**.

## The container: byte 0 is a mode tag

| tag | blobs | decoder |
|---|---|---|
| `0x00` | 535 | **not written.** `pack_overlay`'s "mode 0" — bit-packed, custom, optimised for short distances (`compress_mode0` `:170`, `encode_mode0_length` `:245`, `encode_mode0_distance` `:271`). Derivable the same way mode 1 was |
| `0x02` | 271 | **unknown third format.** `pack_overlay` only ever writes 0 or 1, and these are measured not to be uncompressed either. The real research task |
| `0x01` | 15 | **done** — `tools/lzdecode.py` |

`tools/pack_overlay.c` is this project's **compressor** — the build already uses
it to produce `overlays/*/overlay.lz` — and its header comment documents both of
its formats. Mode 1 is classic LZSS, 4-bit length / 12-bit distance with an
optional 8-bit length; `tools/lzdecode.py` is its exact inverse, derived from
`encode_mode1()` at `:359`.

Note `pack_overlay` also runs `encode_thumb()` first, rewriting BL offsets to be
absolute. That is **overlay-code preprocessing** and does not apply to data
blobs.

## These really are graphics

All 15 mode-1 blobs decompress cleanly to an **exact multiple of 32 bytes** — no
remainders anywhere:

    file_88.raw    4,040 -> 14,400   = 450 units
    file_108.raw   2,112 ->  6,912   = 216 units
    file_198.raw   1,072 ->  3,456   = 108 units
    file_776.raw     228 ->    448   =  14 units

32 bytes is **both** one 4bpp GBA tile **and** one 16-colour BGR555 palette, so
the count does not say which — and `lzdecode.py` deliberately does not claim.
`file_776` is almost certainly palettes: 224 consecutive 16-bit values with
**zero** of them setting bit 15, exactly as BGR555 requires.

**A GBA image is three things** — a tile sheet, a palette, and a tilemap saying
which tile goes where. "An image" is therefore not one blob, and an editor must
handle all three. Sprites add OAM shape/size on top.

## What a PNG workflow would need

The `gbagfx` model from pokeemerald: indexed PNG where the PNG's palette *is*
the GBA palette, each 8x8 pixel block one tile.

| step | estimate | why tractable |
|---|---|---|
| mode 0 decoder | ~half a day | format documented in `pack_overlay.c` |
| mode 2 decoder | unknown | no reference implementation exists |
| tiles+palette <-> PNG | 1-2 days | well-trodden |
| **asset cataloguing** | days to weeks | **nothing records which index is what.** `file_table.h`'s symbols are "named by value; pending semantic names" |

**The round trip is self-verifying**: decompress, re-compress, require the
original bytes, then `make compare`. The ROM hash is the authority — the same
discipline the C work uses.

## Maps are a different subsystem, and partly understood already

`include/map.h` is the place to start; it is careful about what it has measured
and what it has not. Established there:

- **the cell array** — rows `0x200` apart, cells 4 bytes, so a cell is
  `layer_base + z * 0x200 + x * 4`, with **four layers** `0x30` apart from
  `MAP_LAYERS` (`0x130`);
- **the metatile index is the low 12 bits** (`MAP_CELL_INDEX_MASK`);
- **collision is one byte at cell + 0x02** — `0xFF` solid, `0` clear, written
  without touching the index, which is why a pushable log can sit on a tile
  without changing how it looks;
- **map bounds** (`MAP_BOUNDS_MIN_X/Z`, `MAX_X/Z`) and a camera record;
- **spawn records**, `0x18` bytes — id (`-1` terminates), tag, and x/y/z in
  16.16 fixed point. Measured: 66 of 66 slot-3 blocks divide by `0x18`.

Explicitly NOT established, and these are what a new room would need:

- **warp / exit records.** No structure found. `Player_ExitStairs` exists as a
  function name but the data format is unknown.
- **the map-id space**, i.e. what maps an id to a file index.
- **`MapObject` fields.** `map.h` records the `0x0C` stride as measured and says
  the fields are not — a single reading does not hold across overlays yet.
- **scripts.** `gScript_*` symbols and `MapActor_WaitScript` show events are
  scripted data; the format is unexamined.

### The best lead for anyone attacking maps

**`Debug_WarpMenu_UI`** — `src/non_matching/rom_15000/8029094.c`, parked at 17
of 163, pin-free. The game shipped with a developer warp menu, which means it
can already teleport to arbitrary maps **and something in it enumerates the map
id space.** Reading that function is the cheapest route to the one table a new
room would have to be registered in.

## Honest summary of "could I add a new entrance and my own map?"

- **Changing what a room looks like**: the tile grid and collision are
  understood, so editing metatile indices and solidity is reachable once the
  right blob is decompressed and identified.
- **Adding a blob**: mechanically easy — append `.ft_include`, get an index.
- **Making the game REACH a new room**: blocked on the warp records and the
  map-id table, neither of which is known yet.

So the graphics are the easy part, the map grid is half-solved, and the
connective tissue — warps, id tables, scripts — is the unknown. `make compare`
gates all of it, which means every step is verifiable rather than hopeful.
