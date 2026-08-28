# Batch 138 — a linker alias was a whole class, and a diagnosis already existed

Verified on a clean `make clean && make compare` — `goldensun.gba` SHA1
`5c4695205413df7db52b9a184815a07783999971`, all 96 overlays comparing — with
every address below read out of the linked ELFs. The clean rebuild needed the
five-object host recovery documented in [batch-61](batch-61.md); it went as
written.

**remaining 2225 · elevated 3194 · parked 322**

## Elevated (5)

| function | address | ELF | what it took |
|---|---|---|---|
| `OvlFunc_921_20087a4` | `020087a4` | rom_7a7298 | solved twin, exact on the first screen |
| `OvlFunc_957_2008c98` | `02008c98` | rom_7e3e08 | `include/actor.h` fields plus `DMA3_COPY` |
| `OvlFunc_970_20080b0` | `020080b0` | rom_7fa4ec | one missing linker alias, no C change |
| `OvlFunc_968_2009a14` | `02009a14` | rom_7f2f14 | `actor.h` only — `flags`, `interactFlag`, `pos` |
| `Func_8091814` | `08091814` | goldensun | an `unsigned` parameter, one instruction |

## The alias sweep

`OvlFunc_970_20080b0` screened at ONE differing instruction, and it was not the
C: gcc-2.96 emits `__udivsi3` for unsigned division while overlay code calls
`_udivsi3_RAM` through the stub its `imports.s` exports. Different functions,
different addresses.

That was not a one-off. **26 overlay scripts were missing at least one alias,
35 in all**, and every division elevated into any of them would have hit the
same wall while looking like a codegen defect. All 35 are added.

What made a wholesale change safe: an alias only links where that overlay's own
`imports.s` exports the matching `*_RAM` symbol. All 35 did, checked first. The
ROM is byte-identical afterwards, which is the proof they emit no bytes rather
than the claim.

[src/non_matching/ovl_793768/2009754.c](../src/non_matching/ovl_793768/2009754.c)
had recorded this need earlier and named a second overlay as also owed one.
Both are covered; that park drops from 8 differing to 6.

## The thing worth reporting against myself

Three rounds were spent re-deriving the **argument precompute** class, and
HANDOFF.md has carried a full diagnosis of it the whole time — read out of the
compiler sources, not guessed:

    calls.c:805   precompute_register_parameters() hoists any argument whose
                  rtx_cost > 2 ahead of the register loads
    arm.h:1061    SMALL_REGISTER_CLASSES is TARGET_THUMB — always 1 here
    arm.c:2042    in Thumb, ASHIFT costs 4, so a shifted constant is expensive

A cheap `mov` is emitted afterwards and lands last; the ROM's compiler did not
precompute. HANDOFF.md states it is **not fixable from C**, with eight source
spellings and eight flags measured.

Worse than the wasted screens: I invented a competing explanation — that the
lever works "when a callee-saved register is already committed" — and wrote it
into two park files as though it were a finding. `OvlFunc_926_200a68c`
falsified it this round (it commits `r5` and `r6` and the lever still fails),
which is how the existing diagnosis finally got read.

All three parks now open by pointing at HANDOFF.md, and the three functions are
added to its affected list. The invented precondition is marked as invented.

**The discipline change:** check HANDOFF.md's diagnosed-class sections before
attacking a class, not after parking three functions in it. The predictive rule
that was already there — *a call misorders when its argument list mixes cheap
constants with expensive values and a cheap one is not last* — decides these
before a screen is run.

## Parked (7)

| function | class | best | note |
|---|---|---|---|
| `Func_8092a1c` | branch-over-pool | 6/40 | toolchain ceiling, body exact |
| `OvlFunc_948_20091d8` | constant CSE, no boundary | 8/20 | one of 585 in the class |
| `OvlFunc_924_200adcc` | constant derivation | 5/24 | loop byte-exact |
| `OvlFunc_966_200810c` | argument precompute | 5/27 | see diagnosis |
| `OvlFunc_926_200a68c` | argument precompute | 3/30 | falsified the invented theory |
| `Func_80f3858` | constant derivation | 7/28 | mirror image of `200adcc` |
| `OvlFunc_960_2008f50` | DMA-queue inline | 40/131 | earlier round |

Two findings from the parks that generalise. `OvlFunc_924_200adcc`: a loop
counter must be **unsigned** or gcc reverses it into a countdown — and the
ROM's `bls` is itself the tell. `Func_80f3858` is the mirror of `200adcc` on
constant derivation: there gcc derived an address the ROM pool-loaded, here the
ROM derives one gcc pool-loads. The two disagree in **both** directions on the
same kind of address pair, which rules out a single flag for either.

## Tooling

`tools/structmap.py` maps every struct the tree declares (415 files, 66 names)
by name, by layout, and by file. It found `Actor932/935/948/949` were four
invented names for one object; all 14 users now include `actor.h` and the ROM
is unchanged. Five parser bugs were fixed getting there, each caught by
checking a file whose answer was already known — the worst read 405
expression-sized pads as size zero and reported confident wrong offsets.

`tools/unparked_candidates.py` subtracts parked functions from the candidate
list. It leaked twice: first by matching park *headers*, then by matching only
`Func_`/`OvlFunc_` name shapes, which was blind to every function the ROM
annotations gave a real name — `GetWeaponType` was re-derived in full before
its park surfaced. It now matches every identifier in a park; 132 suppressed.

Park integrity, measured rather than assumed: 109 of 322 park files carry no C,
but **84 of those point at a `scratch/` candidate that still exists** and one is
a deliberate class document. Only **24 have no C anywhere**, and none name a
deleted file. A park without its candidate cannot be re-screened — `tryc.py`
runs clean and silent on it.
