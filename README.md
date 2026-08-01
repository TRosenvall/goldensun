# Golden Sun

This is a disassembly of [Golden Sun](https://en.wikipedia.org/wiki/Golden_Sun_%28video_game%29),
in the process of being decompiled to C.

It builds the following ROM:

- [goldensun.gba](https://datomatic.no-intro.org/index.php?page=show_record&s=23&n=0170) (SHA1: `5c4695205413df7db52b9a184815a07783999971`)

## Status

* All known code has been disassembled and symbolized.
* Data has not been disassembled yet.
* Overlays (map code) are not yet built into the output ROM, due to lacking a
  matching compressor. The build system builds and verifies uncompressed
  overlays.
* Editing is not meaningfully possible yet. Data included verbatim from the base
  ROM contains many absolute addresses, so changing the position of any symbol is
  likely to produce a broken ROM.

### Decompilation

**The compiler is patched gcc-2.96** — an arm-elf dev snapshot from 2000-07-31,
the branch between FSF gcc-2.95 and gcc-3.0. Camelot appear to have built their
own rather than using the SDK's.

This corrects an earlier claim here that agbcc was the right compiler. It is
not, and the difference is not cosmetic: agbcc never emits Thumb register-offset
addressing (768 functions need it) and never allocates `lr` in leaf functions
(117 more). Everything currently matching was built with agbcc and **will need
re-verifying** against gcc-2.96.

See [docs/matching.md](docs/matching.md) for the evidence and workflow, and
[docs/attribution.md](docs/attribution.md) for where the identification came
from.

> **macOS users:** the compiler cannot be built or run on macOS. The build runs
> in a Linux container — see [docs/building-on-macos.md](docs/building-on-macos.md).

* **79 functions are fully decompiled and matching** — built from C rather than
  assembly, with the ROM still byte-identical. Most are one-line dispatch
  wrappers converted in bulk; the rest are small accessors and entity-script
  opcodes. See [docs/matching.md](docs/matching.md) for the technique, including
  the `pop {r0}` tell for a void return and the two traps when splitting a `.s`.
* C sources use the structs in [include/](include/) rather than raw offset
  arithmetic — the two compile identically, so matching is no excuse for
  unreadable code. `include/entity.h` names only fields that are actually
  established and leaves the rest as honest padding.
* `Func_b074` is in progress (23 of ~61 instructions aligned).
* `Func_b168` needs rewriting; it is the largest of the split functions and
  calls out to other code, so it cannot be byte-compared without linking.

**The main obstacle is identified and diagnosed.** This agbcc build never emits
Thumb's register-offset addressing mode (`ldr rD, [rB, rI]`), which the ROM uses
in **818 of 2259 functions — 36.2%**. It is a gap in the reconstruction rather
than hand-written assembly: 96.1% of those functions also carry agbcc's own
`-mthumb-interwork` epilogue, so they were certainly compiled. **The fix is to
patch the pattern into agbcc's Thumb machine description**, which would unlock
about a third of the ROM at once. Until then, choose candidates by grepping the
mode out first. See [docs/matching.md](docs/matching.md).

### Annotation

**Every function in the ROM proper is now annotated** — 2259 of them across
fifteen modules — with a comment naming it and describing its register contract
and the struct offsets it touches. Comments are `@` comments in the `.s` files,
which assemble to nothing; annotating cannot change the ROM. Only the overlays
(map code) remain.

Module directories are named for their **ROM offset in hex**, so the ordering
below is offset order, not alphabetical.

| module | functions | what it is |
|---|---|---|
| `rom_c0` | 228 / 228 | **Shared runtime** — allocator, task scheduler, frame loop, input, maths, decompression and the sound engine. Everything else stands on it. See [docs/runtime.md](docs/runtime.md) |
| `rom_9000` | 238 / 238 | **Overworld engine** — sprite/actor pools, entity system and script VM, terrain height and collision, map loading and scrolling, tile/blend/palette animation, player control |
| `rom_15000` | 347 / 347 | **Text, windows and menus** — the UI layer every other module calls into: Huffman string decoding, glyph rasterising, the window and message-box system, and all the in-game screens. See [docs/ui-text.md](docs/ui-text.md) |
| `rom_77000` | 117 / 117 | **Party, character records and event flags** — the data layer: combatant records, derived stats, inventory, and the save variable store. `Func_77394` is the most-called cross-module function in the ROM. See [docs/party-data.md](docs/party-data.md) |
| `rom_8a000` | 418 / 418 | **Field gameplay layer** — NPC interaction and dialogue, cutscene control, camera, field abilities (Psynergy), map transitions, vehicles, screen overlays and fades |
| `rom_a1000` | 208 / 208 | **Menu screens** — the Psynergy, Item, Djinn and Status screens, plus the character picker the map scripts borrow. Shares its state block with `rom_b0000`. See [docs/menus.md](docs/menus.md) |
| `rom_b0000` | 70 / 70 | **Shops and the scrolling-list controller** — buy/sell/inn screens, plus the generic list controller other modules borrow. Still contains a test fixture that grants 200,000 coins |
| `rom_b5000` | 215 / 215 | **Battle logic** — turn order, action resolution and the damage numbers. Reads `rom_77000`, drives `rom_c9000`. See [docs/battle-logic.md](docs/battle-logic.md) |
| `rom_c9000` | 256 / 256 | **Battle animation layer** — draws the animation for a battle action while `rom_b5000` owns the turn logic. See [docs/battle-animations.md](docs/battle-animations.md) |
| `rom_f0000` | 11 / 11 | **The staff credit roll** — a sprite-based scroll with its own 1bpp ASCII font, over 33 crossfading images. See [docs/sound-and-frontend.md](docs/sound-and-frontend.md) |
| `rom_f2000` | 18 / 18 | **Title screen and logo splashes**, plus a palette-fade engine that interpolates on unpacked R/G/B |
| `rom_f4000` | 7 / 7 | **A wagering minigame** — pseudo-area 0x1FD. Stakes money, renders in perspective |
| `rom_f6000` | 15 / 15 | **A prize minigame** — pseudo-area 0x1FC. Costs an item, and carries an unreachable LZW decoder |
| `rom_f9000` | 110 / 110 | **The sound driver** — Nintendo's M4A. `Func_f9080` is the most-called function in the ROM at 1971 sites. See [docs/sound-and-frontend.md](docs/sound-and-frontend.md) |
| `rom_185000` | 1 / 1 | A single exported accessor over a large record table |

Larger routines that were characterised structurally rather than traced
instruction by instruction say so in their comment, and their names are marked
provisional. Treat those as a map, not a specification.

The twelve animation-class handlers in `rom_c9000` **have** been traced
instruction by instruction, and the module writeup records the shared
vocabulary, the allocation-tag map, and four anomalies worth resolving before
those functions are decompiled. The most consequential find there is that
`Func_ed408` is a **runtime code generator** that assembles a specialised sprite
blitter and back-patches its own branches — it had been annotated as a sequence
player, and 216 call sites depend on reading it correctly.

**Four functions in the ROM are declared with the wrong case** —
`.thumb_func_Start` or `.thumb_Func_start`. GAS accepts them and the bytes are
identical, but a case-sensitive scan of the sources misses them entirely, which
is why `rom_8a000` was reported at 416 when it has 418. Any tool that walks
these files needs a case-insensitive match. See [docs/menus.md](docs/menus.md).

## Roadmap

Not in any particular order.

* Annotate the overlays (map code). Everything in the ROM proper is done;
  `rom_320000` is data only and its structure is documented in
  [docs/runtime.md](docs/runtime.md).
* Decompile functions to C, smallest and most self-contained first.
* Develop a matching compressor and build overlays into the output ROM.
* Declare variables in source files and get rid of [wram.sym](wram.sym).
* Isolate modules further. Modules should be partially linked and combined in a
  final link. Functions and data should not be visible, only exported entry
  points.
* Disassemble and document data.

## Prerequisites

The original ROM (USA version) is required. It must be named `baserom.gba` and
placed in the root directory of the repository.

Required software:

* GNU make
* GNU binutils targeting ARM/Thumb (arm-none-eabi)
* C compiler such as GCC or Clang, targeting host architecture (not ARM/Thumb)
* Python 3, for `tools/asmdiff.py`
* **agbcc**, for the decompiled C (see below)

To install the base dependencies on Debian or Ubuntu:

```
apt install make gcc binutils-arm-none-eabi python3
```

### Installing agbcc

agbcc is the reconstruction of the Nintendo AGB SDK compiler, maintained by the
pret project. Build it and install it into this repository:

```
git clone https://github.com/pret/agbcc
cd agbcc && ./build.sh
./install.sh ../goldensun
```

That produces `tools/agbcc/` containing three compilers: `agbcc` (the default),
`old_agbcc` (an earlier variant — pokeemerald uses it for the SDK library files)
and `agbcc_arm` (for ARM rather than Thumb code).

## Build

```
make
```

The default target builds and verifies the ROM and all overlays.

To build only the ROM:

```
make goldensun.gba
```

To verify the ROM:

```
make compare-rom
```

To build a single overlay, for example `overlays/rom_779188/overlay.bin`:

```
make overlays/rom_779188/overlay.bin
```

To verify a single overlay, for example `overlays/rom_779188/overlay.bin`:

```
make compare-overlays/rom_779188
```

To delete all output files produced by the build:

```
make clean
```

## Working on this repository

### File and symbol naming

Functions are named `Func_<rom offset>` and data `Data_<rom offset>`, so a
symbol's name is also its address in the ROM — **with one exception**. The
`rom_770` section is linked for IWRAM and loaded from ROM (`> iwram AT > rom` in
`stage1.ld`), so its names encode load offsets while the code runs at
`0x03000000` onward. See [docs/runtime.md](docs/runtime.md). Source files are named for the
first thing in them: `rom_9000/src/rom_b074.s` begins at `0xb074`.

**Link order is ROM layout.** The order objects appear in `stage1.ld` determines
where their code lands, and the checksum is sensitive to it. When a function is
split into its own file so it can be converted to C independently, the filename
has to encode where it belongs:

```
f<file position>_<function position>_rom_<function address>.s
```

* the `f` prefix keeps `exports.s` sorted to the top of the directory
* the first number is the source file's position in the directory listing —
  `rom_b074.s` was the 9th, so everything split out of it is `f9_*`
* the second number is the function's position within that original file —
  `Func_b388` was the 3rd, so it becomes `f9_3_rom_b388.s`
* the address suffix names the function itself

So alphabetical order reproduces link order, and the directory listing cannot
silently disagree with the linker script.

Rodata that lived at the end of a split file gets its own trailing entry, for
example `f9_6_rom_b074_rodata.s`. Where such a block was a file-local symbol, it
has been promoted to `.global` so the function that uses it can still reach it
across the file boundary.

### Decompiling a function

The workflow, and the reasoning behind the compiler flags, is documented in
[docs/matching.md](docs/matching.md). In short:

```sh
# compare your C against the ROM (offset comes from the function name)
tools/asmdiff.py Func_b684 rom_9000/src/f9_4_rom_b684.c \
    --rom-offset 0xb684 --rom-size 52

# try every compiler and flag combination, ranked
tools/asmdiff.py Func_b684 rom_9000/src/f9_4_rom_b684.c \
    --rom-offset 0xb684 --rom-size 52 --sweep
```

`asmdiff.py` reads `GBA_CPPFLAGS` and `GBA_CFLAGS` out of the Makefile, so the
differ can never disagree with the build. Exit status is 0 only on an exact
match.

When a function matches, delete its `.s` — see the trap below.

### Traps worth knowing

**If `foo.s` and `foo.c` both exist, the `.s` silently wins.** The `%.o: %.s`
pattern rule appears first in the Makefile. Delete the `.s` to switch a function
over to C, and expect the checksum to fail until the C actually matches.

**A `.o` newer than its source is never rebuilt.** For a long time
`make compare` passed while linking objects that had been built from assembly,
which said nothing about whether the C compiled — let alone matched. When a
result surprises you, `make clean` first.

**Declarations must precede statements.** agbcc is a C89 compiler and
`-std=gnu89` is enforced. Declaring a variable mid-block fails with
`syntax error before '<name>'`.

## Acknowledgements

Golden Sun is copyright 2001 Nintendo / Camelot Software Planning.

This is a fan project, not associated in any way with Nintendo or Camelot.
