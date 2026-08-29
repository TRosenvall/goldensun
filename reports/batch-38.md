# Batch 38 — the lever in production, and seven debts taken on deliberately

*Status: ready to port, with one caveat. Read "Seven fakematches" below before
merging.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–37 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare` — 96 overlays compared
byte-for-byte and `goldensun.gba: OK`. Every address read back from the linked
overlay ELF with `nm`.

## Three by the basic-block lever

Batch 37's lever, now driven by [`tools/find_bb_lever.py`](../tools/find_bb_lever.py),
which lists the functions it reaches — `pick_candidates.py` *rejects* both shapes,
correctly for the ~500 straight-line ones and wrongly for the 516 with a branch.

| function | address | overlay |
|---|---|---|
| `OvlFunc_908_2008124` | `0x02008124` | rom_79c0c4 |
| `OvlFunc_907_2008890` | `0x02008890` | rom_79b154 |
| `OvlFunc_926_200a484` | `0x0200a484` | rom_7b2078 |

`OvlFunc_926_200a484` has **three sites in one function**, all fixed by
assigning their constants before the first branch — the best evidence the lever
is a rule and not a coincidence. It also carries the counter-example that keeps
it honest: a *fourth* shifted constant in the same function is **not**
interleaved in the ROM, so it must stay a literal at the call site. **Read each
site off the ROM; do not apply the lever everywhere.**

## Seven fakematches — taken on knowingly

The remaining seven are matched by **pinning a register with inline asm**, not by
finding the construct. This was an explicit decision, not a slide.

**All seven were already parked**, so this is a net reduction in unconverted
assembly rather than new debt on functions that were fine. Two class-park files
are now empty and retired.

| function | overlay |
|---|---|
| `OvlFunc_967_2008030` | rom_7f21b8 |
| `OvlFunc_973_200804c` | rom_7fc720 |
| `OvlFunc_921_20085dc` | rom_7a7298 |
| `OvlFunc_908_20081a8` | rom_79c0c4 |
| `OvlFunc_882_2008398` | rom_77dd1c |
| `OvlFunc_882_20083cc` | rom_77dd1c |
| `OvlFunc_882_2008400` | rom_77dd1c |

Each is marked `// fakematch`, listed in `fakematch.txt`, and carries in its own
header what the real blocker is and the twenty formulations already ruled out.
[reports/fakematch-worklist.md](fakematch-worklist.md) is the worklist.

**If you would rather not take these, reverting them is mechanical** — the seven
`.c` files go back to `.s` and the two park notes come back. Nothing else in the
batch depends on them.

## The compiler's source is in the build image

`/opt/camelot-gcc/gcc-2.96/gcc/` — 150 files, the tree the `cc1` we run was
built from. Thirty-seven batches of treating gcc as a black box, and it was
sitting there.

It immediately overturned a conclusion. A `volatile` local produces the ROM's
exact argument ordering in plain C, which looked like a lead — and `expand_decl`
in `stmt.c` gates the register case on `! TREE_THIS_VOLATILE (decl)`, so a
`volatile` local **never gets a register at all**. The ordering is a consequence
of the operand being a `MEM`; the two effects are not separable. Twelve probes
had produced a confident wrong answer; four lines of source settled it.

`docs/elevation.md` now carries a table of which pass answers which question,
and the rule: **read the pass before probing it.**

## Two tools, and a measurement that withdrew its own claim

`tools/find_construct.py` makes the "search gcc's own output" method repeatable.

`tools/find_fragments.py` matches a function's **blocks** against the solved
corpus — the granularity that can reach large functions where whole-function
matching cannot. Its first ranking said 104 large functions were ≥80% composed
of already-solved blocks, worth 15% of the project.

**That was wrong and is withdrawn.** 217 of the 1,331 distinct block skeletons
in the corpus — 16% — come *only* from fakematches, and the correlation runs
backwards: a function scores high largely by being full of the arg-interleave
shape, whose only "solutions" are fakematches. Counting real exemplars only, it
is **6 functions and 2,357 instructions — half a percent, not 25%.**

The general rule, which matters beyond this tool: **a measurement over a corpus
containing 104 fakematches will silently measure the fakematches.** Exclude them
by default. This was caught because the tool's `--show` output was visibly full
of `[FAKEMATCH]` tags, which is luck.

## Counts

336 functions elevated in total, of which 7 are fakematches. 2,959 hand-written
functions remain in `asm/` of 5,714. 95 parked functions and the two
large-function experiments.
