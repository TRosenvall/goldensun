# Batch 334 — eight landings, and the length gap closed on all 25 targets

Five briefs, 25 targets, **8 landings**. **692 → 684 parked.**
**4,926 → 4,934 of 5,710 (86.4%).** Gated green; SHA1 unchanged.

Third batch run entirely on the length-gap criterion. Rate across the three:
**8 of 15, 7 of 25, 8 of 25.**

| brief | landed | parks improved |
|---|---|---|
| A | 1 | 2 |
| B | **2** | 3 |
| C | 1 | 4 |
| **D** | **3** | 2 |
| E | 1 | 3 |

## Landings

| function | from | what closed it |
|---|---|---|
| `Func_8006384` | 24 | **four crossed levers**, none working alone |
| `Func_80b1bd0` | 28 of 238 | a sibling's landed source had a **three**-parameter signature |
| `Func_80f7df0` | 30 | association order, not naming |
| `Func_8092b54` | 27 | three causes: `QTY_CMP_PRI`, a pool word, register numbering |
| `OvlFunc_948_2009ac8` | 44 | `fold_range_test` merging three `==` into a range check |
| `OvlFunc_909_200809c` | 30 | name the **array**, not the pointer |
| `OvlFunc_947_2009938` | 41 | **one TU, two functions** — genuine twins |
| `OvlFunc_947_20099f0` | 37 | same candidate |

## Two grouping changes, both from batch 333's mistakes

**Grouped by piece, not figure.** I computed which shortlist parks share a TU
*before* assigning, and gave each multi-park piece to one brief. Batch 333 split
a piece across two briefs and wasted part of a round.

**Dropped the equal-length pairing.** Where two targets looked paired I told the
brief explicitly *not* to assume twins and to resolve the actual `.s` first.
That was right both ways: brief D's pair **were** real twins (identical
86-instruction streams modulo an r5/r6 swap) and landed as one TU — and its
park's title named **an `.s` that no longer exists**, which only checking found.

## The mechanism of the batch: mode, not naming

`*thumb_movhi_insn` (`arm.md:4318-4320`) constrains operand 1 `"l,mn,l,*h,*r,I"`
— `mn` at alternative **one**, the immediate `I` at **five**, and `recog` takes
the first match. `*thumb_movsi_insn` (`:3838-3840`) reverses that order. So
**HImode constants always pool and SImode constants never do**, and the
destination's mode is the lever.

That resolved three separate parks: `Func_8005fcc`'s four "pooled constants
needing names" were two-thirds a **mode** problem and free once routed through
an `unsigned int` temp (77 → 74, lengths now equal); `OvlFunc_957_2008b30`'s
extra instruction was a **second literal pool and the branch over it**, because
`pool_range` is 64 for HImode against 1020 for SImode — and the mode is
*invisible in the instruction text*, since gas assembles `ldrh rX,.L5` as a
16-bit `ldr`.

## Three blocker classes corrected, all cited

- **`Func_80c00d8`** — the pass is **cse1**, not gcse or the allocator. `.00.rtl`
  emits *two* pseudos for 0x100 and insn 27 is gone by `.03.cse`. Forced
  arithmetic: `cse.c:509-514` charges a pseudo 1 against
  `rtx_cost(const_int 256, SET)` = 6 (`arm.c:2077-2080`), so no spelling loses it.
- **`Func_80c1014`** — **not a copy at all**, which corrects batch 333's own
  gcse-cprop re-diagnosis. `mov r2, r8` is a *reload*; `.18.greg` prints the
  allocation order outright.
- **`Func_8006088`** — `find_avail_set` (`gcse.c:3608-3671`) reaches only one of
  three accumulator uses; the copy dies to global-alloc's preference coalescing.
  The requirement is a live-range **conflict**, which is why the whole two-name
  family is inert.

New for the notes: **`gcse.c:1546-1551` refuses to merge two MEMs in different
alias sets**, and `long` vs `int` *does* split the set here while `unsigned int`
and a `COMPONENT_REF` do not.

## Four errors of mine, recorded

1. **I asserted "all pin-free" for brief E's five targets.** `20086dc` carries
   **15 pins** via four `PIN3` macros. Asserted, not measured.
2. **I said a `rom_9000` piece held "7 functions, two of them parked."** Brief C
   found **all seven are parked**, and the two targets are not adjacent — so
   landing both needs two cuts plus a linker change.
3. **My audit flag had a false positive.** Brief C showed `Func_8093af8`'s
   "116/115, gap 1" row is the 16-bit count with a 32-bit entry absorbing the
   difference — its **stream lengths are equal**, so positions line up. The flag
   now reports the two cases separately instead of asserting phase.
4. **I gated while agents were still writing** and got a spurious link failure;
   a clean re-run was green. Do not build mid-batch.

And one I caught at install: brief A's improved `8005fcc` body **replaced the
park and took its header with it** — recipe, claim and every measured negative —
and I installed it, leaving `parkcheck` reading `UNCHECKABLE`. Header
reconstructed from the previous revision. **A park body may be replaced; its
header must be carried forward.**

## Carried forward

- **~145 unworked audit-shortlist parks remain at gap ≤ 2.**
- **`Func_80063bc` at 3 and `Func_8006408` at 7** share a TU with nothing else
  now that `8006384` has landed — *both* must close for either to ship, and
  `80063bc`'s residue is **structurally bounded** (`arm.c:2425-2427` zeroes every
  anti/output dependence, fixing a priority gap at exactly one).
- **`Func_80c0130` at 6 and `Func_80c00d8` at 33** likewise share a TU; batch
  327's claim that a landing there "needs a split" is **wrong** — two members,
  both parked, so one TU converts it whole.
- `OvlFunc_957_2008b30` is at **0** but cannot ship: its piece-mate `2008bc8`
  has an `_AREA_92`/`_AREA_97` symbol relocation difference.
- `OvlFunc_882_200bc48` is byte-identical **at one pin**, parked pin-free at 41.
