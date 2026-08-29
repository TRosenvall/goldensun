# Batch 11 — 12 functions, and a correction to batch 07

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–10 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean build, unassisted. Every address read back from the linked overlay
ELFs.

## The functions

All twelve are `GetEntrances` four-way members, all from splits, so the sibling
`.s` files must travel with them.

| Function | Address | New source |
|---|---|---|
| `OvlFunc_907_2008088` | `0x02008088` | `src/overlays/rom_79b154/ovl_30_a_b.c` |
| `OvlFunc_920_20080a0` | `0x020080a0` | `src/overlays/rom_7a6ae4/ovl_30_c_a_c_a_b.c` |
| `OvlFunc_927_2008f40` | `0x02008f40` | `src/overlays/rom_7b4558/ovl_30_c_c_a_a_b.c` |
| `OvlFunc_927_200a4ac` | `0x0200a4ac` | `src/overlays/rom_7b4558/ovl_30_c_c_c_a_b.c` |
| `OvlFunc_934_200969c` | `0x0200969c` | `src/overlays/rom_7bdeb0/ovl_169c_a_a_a_b.c` |
| `OvlFunc_935_200808c` | `0x0200808c` | `src/overlays/rom_7bf5a8/ovl_30_c_a_b.c` |
| `OvlFunc_957_2008a00` | `0x02008a00` | `src/overlays/rom_7e3e08/ovl_30_c_c_a_a_a_b.c` |
| `OvlFunc_958_2008d20` | `0x02008d20` | `src/overlays/rom_7e636c/ovl_cc0_c_a_c_a_b.c` |
| `OvlFunc_959_2008af8` | `0x02008af8` | `src/overlays/rom_7e7574/ovl_9dc_a_c_a_b.c` |
| `OvlFunc_965_2008f58` | `0x02008f58` | `src/overlays/rom_7ef4f4/ovl_30_a_a_c_c_b.c` |
| `OvlFunc_965_2008fdc` | `0x02008fdc` | `src/overlays/rom_7ef4f4/ovl_30_a_c_a_b.c` |
| `OvlFunc_965_200a7a0` | `0x0200a7a0` | `src/overlays/rom_7ef4f4/ovl_30_c_a_c_b.c` |

`unknown_id.sym` gains 6 entries. 22 of the family's 24 members are now done;
one is blocked behind its own `.incbin` data and one remains behind a split.

## Correction to batch 07

**Batch 07 said the ids `0x4d`, `0x4f`, `0x51` were "spaced by TWO, which is a
hint about the namespace worth keeping: whatever indexes it appears to have
paired entries."** Batch 09 repeated it when a second family
(`OvlFunc_957_2008a00`, `0x93`/`0x95`/`0x97`) appeared to corroborate it.

**That was wrong.** Surveying all 52 constant groups in the overlays:

| shape | count |
|---|---|
| consecutive | 23 |
| spaced by two | 2 |
| mixed | 27 |

And both apparent pairs are **subsets of consecutive runs other functions use
in full** — `OvlFunc_932_20080e4` compares against `0x4d` through `0x57`, and
`OvlFunc_957_200b598` against `0x93` through `0x97`.

Two data points agreeing is not evidence when both are drawn from the same
unexamined population, and the survey that refutes it takes a minute to run.

### What the survey does show, which is a better lead

The ids form a **dense, contiguous space allocated in per-area runs**:

| Overlay | Run |
|---|---|
| 924 | `0x36`–`0x39` |
| 932 | `0x4d`–`0x57` |
| 957 | `0x93`–`0x97` |
| 968 | `0xb5`–`0xba` |

Observed range `0x10`–`0xba` — roughly 190 ids — with each function comparing
against a subset of its own area's run.

That is what a map or area id space looks like. **If that matches a table you
recognise, naming it retires `unknown_id.sym` outright.**

## What a false-positive family member looks like

`OvlFunc_958_2008d20` was picked up by the family sweep and is not the family
shape. Its first arm checks a story flag and selects between two *further*
tables — five ways, not four — and one arm returns a named global rather than a
local `.L` label.

The generated template produced 25 instructions against the ROM's 31, so
`tools/tryc.py` caught it on sight and it was written by hand instead.

This is worth stating because the sweep's matching criterion is deliberately
loose — three compares and four returns — so that it finds variants rather than
only exact clones. The cost of that looseness is exactly this: close enough to
generate, wrong enough to fail the screen. That is the right trade, but only
because the screen runs before the build.

## Why these files are generated, not copied

Four distinct constant orderings have now appeared in one family:

| Function | Constants |
|---|---|
| `OvlFunc_924_2008f30` | `0x36 0x37 0x38` (ascending) |
| `OvlFunc_924_2008e20` | `0x39 0x38 0x37` (descending) |
| `OvlFunc_942_2008040` | `0x6b 0x70 0x6c` (non-sequential) |
| `OvlFunc_907_2008088` | `0x1e 0x23 0x20` (out of order and non-contiguous) |

Every one of those compiles cleanly if transcribed wrong, and produces a ROM
that differs in three pool words. They are read out of the assembly by script
for that reason.

## Still open, and still only answerable by you

- **Semantic names for the id space**, now with the contiguous-run evidence
  above, which is the most concrete lead so far.
- **Five ambiguous offsets in `actor.h`** (batch 03), documented rather than
  guessed.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'
