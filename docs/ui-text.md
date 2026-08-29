# rom_15000 — text, windows and menus

The UI layer. 135 exported functions, more than any other module, and almost no
hardware footprint (31 DMA3 setups, two BG control writes) — a service layer
rather than a renderer. All 347 functions are annotated. The module header lives
in [rom_15430.s](../rom_15000/src/rom_15430.s).

## Text is Huffman-compressed with an order-1 context model

The tree used to decode a character is selected by **the character before it**,
so `u` after `q` costs almost nothing. `Func_15430` is the decoder and
`Func_15570` seeks it to a string. Both are ARM rather than Thumb because they
are bit-twiddling inner loops.

Decoder state is three words passed by pointer:

```
+0x00  the previous character -- doubles as the tree selector
+0x04  byte pointer into the compressed stream
+0x08  bit buffer, refilled a word at a time
```

`HuffmanTreePointers[prev >> 8]` gives `{base, offsets}` and
`base + offsets[prev & 0xFF]` is where that character's tree starts. String ids
work the same way through `StringPointers`, except the low byte is a *count of
strings to skip*: `Func_15570` walks a byte length table, with 0xFF meaning
"continues into the next byte" so strings over 254 bytes still encode.

## Two more compressors

- **`Func_15afc`** decodes a nibble stream using a **move-to-front alphabet**
  primed from `0xFEDCBA98_76543210`. A decoded symbol rotates to the front, so
  recently used values become cheapest. Output is one nibble per byte —
  unpacked 4bpp.
- **`Func_15d74` / `Func_15e10`** pack 8bpp to 4bpp. They are the same routine
  except that `Func_15d74` builds a per-pixel opacity mask
  (`orr`/`orr`/`and 0x11111111`, then `rsbs x, x, lsl #4` turns each marker into
  a full `0xF`) and skips the read-modify-write when the word is fully opaque.
  `Func_15e10` just stores. It still loads the `0x11111111` constant it never
  uses, so the two were plainly edited from one source.

`Func_1a5a4` DMA-copies `Func_15afc` into a 0x278-byte scratch and runs it from
RAM — the same trick `Func_5340` uses in `rom_c0`.

## The UI block

`Func_15f30` allocates **0x12FC bytes under tag 0xF**; that is what `iwram_1e8c`
points at, so every `+0xNNN` offset in the module is bounded by it.

```
+0x500  eight window records, 0x24 bytes each
+0x620  three message-box slots, 0x28 bytes each   (0x500 + 8*0x24 = 0x620)
+0x698  sixty-four display nodes, 0x1C bytes each  (64 * 0x1C = 0xD98 - 0x698)
+0xD98  node free-list head        +0xD9C  tail cache
+0xEA3  dirty mask, six bits       +0xEA5  mode      +0xEA6  flush suspend
+0xEA8/+0xEAC/+0xEAE  text style: line height, shadow, ink
+0xEB0  a 0x200-entry halfword ring — the layout output
+0x12B2 layout cursor into that ring
+0x12BC eight callback pointers    +0x12DC their ids
+0x12EC/+0x12EE  selection sentinels, 0x3E7 = "none"
```

Each region's size checks out against the next one's offset, which is how the
map was confirmed rather than guessed.

**Window record**, geometry in tiles on a 30×20 grid:

```
+0x00 node list head   +0x04 tail cache
+0x08 width            +0x0A height
+0x0C x column         +0x0E y row
+0x14 busy halfword    +0x16 flags, bit 0 = in use
+0x1A pending count, signed
+0x1C..+0x22 geometry saved at close, in the order x, y, w, h
```

The standard message box is `Func_162d4(0, 0xF, 0x1E, 6, ...)` — full width, six
rows, at row 15.

## The frame loop

`Func_1789c` is the whole per-frame tick: `Func_16f2c` advances the eight
windows, `Func_16868` the three message boxes, `Func_191cc` the menu layer.
`Func_160fc` is the only thing that touches VRAM for the UI — it reads the
six-bit dirty mask at `+0xEA3` and DMA-copies one 0x100-byte block per set bit
to `0x6002000`, bit 0 meaning "all of it".

Input comes from the globals `rom_c0` maintains: `iwram_1ae8` held,
`iwram_1c94` newly pressed this frame, `iwram_1b04` with auto-repeat. Menus poll
these directly — `Func_1999c` reads held state and `Func_199ec` newly-pressed,
which is why navigation does not repeat while a key is down.

`Func_175a0(textId)` is the most-called entry point from outside: show a message
and block until dismissed. `rom_b5000` uses it with ids like 0x816 and 0x847.

## Fonts and layout

`Data_32224` is the font table with a **0x20-byte stride** — width first, then
the glyph bitmap. `Func_17a64` measures a string from it: 0x20 (space) is 4,
anything above 0xFF is 10, and 0xDE/0xDF are two-byte lead-ins that cost
nothing. `Func_155d0` draws from the same entries, plotting 4bpp into
`0x6002500` with the nibble picked by the low bits of x, so glyphs land on
arbitrary pixel columns rather than tile boundaries.

`Func_18038` is the layout engine and the largest routine outside the screens
(839 lines): it breaks lines, formats embedded numbers through `Func_17dd4`, and
fills the `+0xEB0` ring. Callers test that ring's entry for emptiness to decide
whether there is anything to show.

`Func_17dd4` formats decimals with `Func_b1c` (signed remainder) and `Func_af0`
(quotient), pre-filling with spaces so the result is right-aligned rather than
zero-padded.

## Odds and ends

- **Nine functions are bare `bx lr`.** `Func_1c0c8`, `Func_1c0cc`, `Func_1c0d8`
  and `Func_1cf44` are **exported**, so something outside the module holds their
  addresses. `Func_1c468` is an exported "always returns 1".
- `ewram_240+0x205` is the **in-game hour** — `Func_1ca1c` converts it with
  `Func_b1c(hour + 12, 24)`.
- `Func_27114` is the largest function in the module at 2004 lines (the
  top-level in-game menu); `Func_21e6c` is second at 1157 (the shop).
- `Func_1fe2c` builds the menu cursor as a **rom_9000 actor** (`_Func_bc70` /
  `_Func_ba30` / `_Func_b168`) rather than as a display node, unlike the rest of
  the module.

## Depth

The primitives — codecs, allocators, window lifecycle, layout entry points,
input polling — are traced. The large screen bodies (`Func_27114`,
`Func_21e6c`, `Func_23178`, `Func_26080` and the rest) are characterised from
their call graphs and state use and say "traced structurally" in their own
comments. Treat those as a map, not a specification.
