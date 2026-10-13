# Batch 346 — five landings, and a corpus-wide defect

**5 briefs, 20 targets, 5 landings, 8 length gaps closed.** Park files
**623 → 618**. 4,999 / 5,710 matched (87.5%).

Third batch drawn from the gap pool. The ±1 band is thinning at low figures, so
this batch added the untouched **−2 band** (brief C) and pushed into figures of
159–251 that are large purely because the comparison is out of phase.

## Landings

| function | before | how |
|---|---|---|
| `OvlFunc_925_200835c` | 143 (phase) | a dead store to **half** a two-word aggregate |
| `UpdateMusicSettings` | 112 (phase) | **deleting** the local — fewer names, *more* instructions |
| `OvlFunc_926_2008afc` | 34 (−2) | a 7th pool word moves `arm_reorg`'s placement |
| `YesNoMenu` | 34 (−2) | a lever written in its own piece-mate's park, never acted on |
| `Func_809c138` | 182 (phase) | a pooled small value is the `.sym` signature |

**Brief C's −2 band paid immediately** — two landings from four targets. It also
justified being its own brief: a gap of 2 splits both ways, and only per-region
counting separated "two sites of −1" (`UploadIcon`, `YesNoMenu`) from "one site
of −2" (`2008afc`, `Func_80bac6c`).

## Gaps closed

| park | before | after | counts |
|---|---|---|---|
| `UploadIcon` | 42 | **3** | 47/47 insns, 160/160 bytes |
| `Func_80a5788` | 172 | **36** | equal insns *and* equal bytes; 20 → 12 hunks |
| `Func_80799b0` | 108 | **35** | gap and byte size both closed |
| `Func_80a4924` | 132 | **62** | 426/426, 1168/1168; aligned 68 → 59 |
| `Func_80b8c1c` | 140 | **107** | 161/161, 412/412; aligned 103 → 78 |
| `AdvanceMsgText` | 212 | **110** | 610/610; aligned 665 → 684 of 707 |
| `DrawInventoryIcon` | 105 | 197 | equal length reached; figure rose, aligned 74 → 57 |
| `Func_801b810` | 161 | 176 | r8 removed entirely; aligned 53 → **32** |

The last two are the **non-monotonicity rule paying off rather than being
tripped over**: both figures rose while the aligned residue fell, and both
headers carry the pair. That is the correct way to record them.

## THE HEADLINE IS CORPUS-WIDE: 100 parks assert a distance they do not have

A park header routinely says "this figure **IS a true distance**", justified as
*"N == N encodings and objcmp prints no SIZE line"*. That reads `objcmp`'s
**stream** figure as an instruction count. The stream includes 32-bit entries
(pool words and `bl` calls) and pads — so **a pad absorbs exactly one 16-bit
instruction and leaves both the stream length and the byte size equal** while the
instruction counts differ.

`objcmp` prints the counts on the line *directly above* the figure, and prints
its own note that a pad is absorbing the difference. **Four of this batch's
twenty targets asserted the opposite anyway**, in prose, with the contradicting
line in the same output.

Swept tree-wide — **and this count was later corrected downward; see below**:

```
256 files match the bare phrase
180 are a claim about THIS park's own figure
 53 OF THOSE ARE OUT OF PHASE     <- the honest number (first reported as 100)
```

**The original 100 was wrong because the sweep counted the negation as an
assertion** — `grep -i 'true distance'` matches `NOT A TRUE DISTANCE`, a header
that is correct and says so. Batch 347 brief A found this by being handed such a
park as a target. 44 more matches were prose about *other* parks. And **35 of the
real 53 share one provenance**, the sentence *"Re-classified in batch 322; the
figure IS a distance"* — so batch 322's sweep was wrong on 35 of the 59 parks it
touched. `parkcheck`'s cross-check carried the same negation bug and is fixed.

The substance holds — 53 parks assert a trustworthiness they do not have — but
the magnitude was overstated by an instrument I did not check, which is the exact
failure this section documents.

`parkcheck.py` now cross-checks the header's prose against the measured counts.
The claim is **load-bearing** — it is the sentence telling the next agent the
figure can be trusted, and every targeting instrument here ranks on that figure.

This is the **eighth** instrument-level clean negative in this project
(`piece.py` twice, `parkcheck`, `directory`, `shimcount`, `autosweep` with six
causes, `gapscan`, and now `aligncmp` usage). The pattern is identical every
time: **the tool answered the question it was asked, and the question was
narrower than the decision resting on it.**

## One `.sym` entry added

`Func_809c138`'s last encoding needed `_FILE_1b = 0x1b;`. I added it as routine
rather than queueing it: `file_table.sym` already holds **158** entries,
`stage1.ld:16` includes it, 18 landed files use `_FILE_` symbols, and **0x1b is
the hole between the existing `_FILE_1a` and `_FILE_1c`**. Cross-checked against
the comparable case — **163 of the 194** landed files using an `_AREA_` symbol
carry no fakematch row, so a `.sym` value of this kind is not a shim here. One
line, trivially reversible.

## Mechanisms

- **A dead store to HALF of a two-word aggregate is not dead.** `.18.greg`'s
  `32 (2)` is an allocno needing two consecutive hard registers, forced to
  **(r5, r6)** when the counter is in it — that is the reference's
  `push {r5, r6, lr}` plus its apparently dead `mov r6, #0`. **Correction to the
  class file documenting this:** it says the dead half must be *read*; true only
  when the live half is a propagatable constant. With a loop counter as the live
  half, reading it costs a real `orrs` and makes the body one instruction long.
- **`lang_get_alias_set` returns 0 through a UNION** (`c-common.c:3340-3345`), so
  spelling a store as a union member restores a dependence the scheduler needs —
  necessary because `rank_for_schedule`'s first rung is `INSN_PRIORITY`
  (`haifa-sched.c:4061-4064`), and a store with no dependents can never outrank
  the insn feeding a compare.
- **`convert_to_integer` pushes a narrowing INTO the operands**
  (`convert.c:280-320`). A `u8` destination turns `0xf0` into `-16` against the
  ROM's `mov r2, #0xf0`. A **separate-statement** carrier makes the conversion
  see a `VAR_DECL`. This is a *front-end* site, distinct from `force_to_mode`.
- **`ifcvt` declines at two rtl insns.** `find_if_case_1` accepts an arm because
  `count_bb_insns > BRANCH_COST` is `1 > 1` → false (`ifcvt.c:1651`,
  `:1681-1683`; `BRANCH_COST` is 1 for Thumb at `-O2`). `*thumb_iorsi3` takes no
  immediate, so `op |= 0x4000;` *inside each arm* keeps the block at two insns.
- **A pooled SMALL value is the `.sym` signature** — gcc-2.96 never pools what
  `mov` can build. The discriminator can be in the same function. Refinement: a
  `ldrh` pool load means a bare literal; an `ldr` pool load *feeding a halfword
  store* means an `int`.
- **local-alloc coalesces a single-block pseudo into a hard argument register and
  global-alloc does not** (`local-alloc.c:1684-1687`, `:1344-1348`,
  `:2010-2016`). One shared local across two sites makes both global — right at
  one, wrong at the other. The cure is two spellings at once.
- **Move a definition LATER** is a first-class allocation lever; the standing
  table had only the opposite direction.
- A redundant **`& 0xff`** on a byte-loaded value manufactures exactly one
  surviving register move (`simplify_and_const_int` drops the AND on
  `nonzero_bits == 0xff`).

## Two cautions worth as much as the mechanisms

- **A sweep's score is not a proof of equivalence.** A mechanical position sweep
  produced **two wrong programs that compiled and read better on hunk count**.
  Nothing in `autosweep`, `sweep_variants` or `aligncmp` checks that a variant is
  still the same program. A winning row is a *candidate*, not a result.
- **The priority flip is not necessarily the register you want.** One brief
  computed the exact live-length change, **verified in `.18.greg` that the
  allocno order flipped** — and the pseudo stayed on r0. The precondition held,
  the arithmetic was right, the flip happened, and the outcome did not follow.

## Tool fixes this batch

- **`piece.py` now reports exported DATA.** "Converts whole" is about the
  *functions* in a piece; a `.s` can also define `.global` data that siblings
  link against. This is what turned 345's gate red. **32 of the first 200 parks
  are affected** — about a sixth of the tree — and it fired on one of this
  batch's own targets before an agent could trip over it.
- **`parkcheck.py`** cross-checks a header's distance claim (above).
- **`autosweep.py`'s flag axis had one more blind spot**: the aligned check fired
  only on rows reading *lower* than base, so a flag reading **equal** while
  changing the body stayed invisible. `-fno-strength-reduce` on `Func_80bac6c`
  reads the same ndiff 35 at the same stream length while producing 52
  instructions against 55 and taking the aligned residue 12 → 30. Fixing it
  caught a third bad row on the same park.
- **`autosweep.py`'s anchor axis was contradicting its own guard** — every parsed
  row entered `BEST`, so the verdict read `Improved 739 -> 700` off rows labelled
  "MISALIGNMENT, not an improvement" two lines above. Found by running it on the
  three Anim parks the moment they *had* an anchor axis; all three produced a
  false verdict. **Printing a caveat and then contradicting it in the verdict is
  worse than printing nothing, because the verdict is what gets copied.**

## Also

- The seven rom_c9000 Anim parks now have a **fully measured anchor axis for the
  first time** (153/153, 172/172, 185/185 boundaries, zero dead). No zeros, as
  expected at figures of 739–844; the value is the recorded negative.
- `tryc --align` counts label renumbering as difference and read **61** where
  `aligncmp` read 12 hunks on the same body. Prefer `aligncmp` for a residue.
- Two briefs reported their harness refusing to write `FINDINGS.md`/`NOTES.md`.
  Ten briefs across 344–345 wrote them without trouble, so this is not a general
  constraint; both recorded their findings in the candidate headers and manifest
  `notes` instead, which is the project convention anyway.
- I corrected a stale `FINDINGS.md` left on disk by brief E (two byte-size cells
  read 674/638 against the real 688/580) and marked it as a mid-round checkpoint.

## Open

- **Owner decisions still queued:** `_CONST_0 = 0x0;` in `const.sym` (evidence:
  gcc-2.96 cannot pool a literal 0 in any mode whose pool load is `ldr` —
  `arm.md:3840`, `:4625`), and the `__divsi3 = _divsi3_RAM;` alias in the
  common1 linker script that `OvlFunc_common1_78` needs before it can land at
  any figure. Plus the `:: "i"(0)` device question and the 23 undefined-extern
  address parks.
- `UploadIcon` at 3 is byte-identical with one pin on r2; reported as a park
  rather than a pinned landing.
- `Func_80a5388` has a `goto` body reaching **126 of 184** — the best figure ever
  recorded there — **with the aligned residue 48 → 132**. Deliberately not
  installed. That is the rule working as intended.
- `AdvanceMsgText` has three bodies bracketing its last gap and none is both 610
  instructions and 89 pool words; the best aligned residue anywhere on that
  function (692) belongs to a body that is not the lowest figure.
