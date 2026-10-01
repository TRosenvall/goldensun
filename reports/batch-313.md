# Batch 313 — the unattempted frontier closes, and four of my own instruments inverted

Seven agents, 28 targets — **every remaining unattempted function in the tree**. Gate green at
`5c4695205413df7db52b9a184815a07783999971` at every commit.

**0 landed, 22 parks installed, 6 triage parks created, 5 tools fixed, 3 documented claims retracted.**
State: **4,788 of 5,710 (83.9%)** — **0 available**, 831 parked.

**`AVAILABLE` is now zero.** Every function in the tree is matched, permanently out of reach
(76 hand-asm + 14 ARM + 1 unmatchable), or parked with written analysis. The project has no
unattempted frontier left; all remaining work is residue reduction on parks.

## Candidates

| function | figure | state |
|---|---|---|
| `OvlFunc_884_20097c8` | **276 of 1082** | **97.3% aligned**, 37 diffs in 27 hunks, +1 instruction |
| `Func_8090a5c` | 815 of 849 | **distinct pool set EXACT, 11=11** |
| `OvlFunc_968_200b068` | 998 of 1586 | **call multiset EXACT — 224 calls, every target at identical count**; 85.4% from 68.2% |
| `Anim_Boreas` | 1263 of 1416 | **SIZE EXACT 3156**, relocation symbol sequence correct |
| `Anim_ScreenShatter` | 1042 of 1132 | relocations 69 rows both sides, right entry-for-entry |
| `Anim_Kirin` | 1059 of 1095 | pin-free, no split |
| `Func_8023178` | — | **both jump tables solved, 6 of 6 instructions exact each** |

## Rung 9: every aggregate axis exact, and 146 positions worse

The strongest separated-axis position anyone has built here, and it is **not** the best candidate:

| | count | encodings | opcode histogram | pool words | pool multiplicity | objcmp |
|---|---|---|---|---|---|---|
| `blanket4` | **1048/1048** | **1082/1082** | **perfectly flat** | **exact** | **exact, all 28 values** | 422 |
| kept candidate | +1 | — | ragged | exact | — | **276** |

Rung 8 introduced the per-opcode histogram as the instrument that sees through an exact count. This
shows **the histogram is itself not sufficient** — and rung 2's qualifier (*when the count is not yet
exact*) does not cover it, because here the count **is** exact.

**The reason, which makes the whole ladder legible:** every aggregate axis is a **multiset**
comparison. A candidate can hold exactly the right bag of instructions, pool words and pool loads and
still order them wrongly — and ordering is the whole of a match.

### And brief D added the level the histogram is blind to

On `OvlFunc_968_200b068` the **call multiset** is already exact, and that is what caught a defect
nothing else saw. Written as two self-contained switch arms, `jump.c` **cross-jumps the identical
bodies**, leaving **two `bl` where the ROM has three**. A missing *call* was invisible to size, to
the encoding count, **and to the histogram** — because the histogram counts `bl` as a *class*, so one
call to the wrong target is a perfect wash.

The ladder is now a hierarchy of resolutions, each blind to the next:

    size (a sum)
      → instruction and pool counts (separate sums)
        → the per-opcode histogram (a bag of opcodes)
          → the call multiset (a bag of TARGETS)
            → the positional figure (a sequence)

**Every level can be exact while the next is wrong.**

## And rung 8's instrument had to be fixed: read ENCODINGS, not listing text

Found independently by two agents. **`ldrh rX,<pool>` and `ldr rX,<pool>` on the same pool word are
identical encodings** — Thumb-1 has no PC-relative halfword load, so GAS must encode both the same
way. The listings differ; the objects do not. Proved twice: once through an aligncmp band aligned-equal
*across* the disagreeing mnemonics, and once **arithmetically** — a candidate showing fourteen extra
`ldrh` was only +7 encodings in total, so fourteen extra instructions is impossible.

**This retracts `Anim_Gaia`'s BLOCKER 4**, which attributed 3 encodings to a listing artefact that
costs zero. Two listing-level aliases are now known, both of which *invent* opcode deltas no object
carries: this one, and the `.call_via` macro line (which under-counts `mov` **and** `bx` by one per
site — measured correction factors of 5/9/9/14 across one brief, where the fourteen is large enough
to invent or hide a whole lever).

## Four of my triage columns don't underperform — they invert

Three agents attacked the table I built over batches 312–313, independently, and agreed.

- **`maxreload` is actively misleading, not merely weak.** The batch's highest value, 23, was **23/23
  one array base address** (an 84-byte table read `ldrb [base,index]`). Partitioned, that function had
  the **lowest** true-constant ratio of its group — the **closest to a pure rebuild**, exactly
  opposite to what the figure implies.
- **High-register count measures *pressure*, whatever its cause** — not constant reuse. My
  top-ranked target was dominated by one memory-loaded pointer and its 28 reads; another function had
  58 copies and **zero** constant-rooted ones, so the pin pass had **no domain there at all**. One
  agent automated the partition by tracing each copy to the **root** of its value and **validated it
  against this project's own hand-computed table**, reproducing all four tabulated functions exactly.
  Installed as `tools/hipartition.py`.
- **A mnemonic census counts comparisons, not loops**, and it **ran backwards**. Classified as
  backward edges across eight functions: **95 of 96 conditional back edges are `bne`**, three signed
  closures in all eight. So `Anim_Gaia`'s all-`!=` rule **transfers to every one** — including the
  function the doc said it would corrupt at 48 sites. Worse, the ratio ranked the two functions with
  **zero** signed loop closures as the *least* `bne`-dominated. One agent had written *"the function
  Gaia's lever would damage most: 38 sites"* about a function with zero signed closures, and struck
  the claim in place rather than deleting it.
- **The aggregate grep over-reports, and the inflation grows with the figure** — so the column is
  worst exactly where it matters. Re-derived: 11/12/9/1 → **1/4/0/0**; 6/10/14/19 → **5/7/8/10**;
  3/6/6/14 → **3/3/5/6**; 27 → **11**. **Thumb-1 has no sp-relative `ldrh`/`strh`/`ldrb`/`strb`**, so
  every sub-word stack *scalar* materialises a base register too.

**A separation worth keeping**, from the agent whose candidate was unaffected because it read loop
forms off the reference's actual back edges: **the bad instrument endangered the prose, not the
reconstruction.** A proxy corrupts what you write down before it corrupts what you build.

**And the pattern across all four:** every mechanical proxy I have built for difficulty was defeated
by a shape it could not see, and **each replacement was itself too crude one layer down**. I tried to
fix the aggregate count with a block-aware scan and it still returned 10/11/4/0 against a measured
1/4/0/0. The honest instruments are the ones that count work directly — **unresolved draft lines**,
now confirmed independently twice.

## The frame needs SEVEN resolution classes and FIVE greps

Only one of the seven is fixable by de-duplicating offsets:

1. **sub-word scalars** — the Thumb-1 addressing limitation above;
2. **loop end sentinels** — an address materialised as a termination bound;
3. **hidden register arguments** — one frame-top pointer is handed to a callee **in r9**. A *calling
   convention*, not a local; no declaration work produces it;
4. **re-materialisation** — the same offset formed twice. **The only fixable one**;
5. **address-taken scalars** — appearing as `f(&a)` *and* as `p = &a`, where the slot's *address* is
   stored into another slot which then becomes that function's 28-load maxreload;
6. **the FRAME TOP** — `add rX, sp, #0x300` with a pooled negative then added to reach lower slots.
   **Previously unnamed, and it will always make a large frame look as though it has an object at its
   end**;
7. **phantom load-only slots** — words of an aggregate written through a materialised base the
   `[sp,#imm]` census cannot see.

Greps the recipe was missing: **`(add|sub) sp, rN`** (a frame over 508 bytes, since Thumb-1 `sub sp,#imm`
caps there — a 548-byte frame read as *frameless*); **`add rX,sp,#K`** alongside `mov rX,sp`;
**`str rX,[sp]` unpaired with a load**; **`mov rX,#K / add rX,sp`** and **`mov rX,sp / add rX,#K`**; and
**`add rX, sp` two-operand**, forced when the offset is not word-aligned (`sp+0x14f` is inexpressible
as an immediate form).

One agent settled its four by **closing the byte accounting exactly** — 156/176/284/356, no slack to
hide a miscount. **That is the check a grep cannot have.**

## The switch material: its population is measured, and the lever still doesn't pay

Brief D's four were chosen *because* they carry the tree's heaviest dispatch. **All 11 dispatch sites
are TABLES**, so the out-of-range-case lever (which flips a *tree* into a table) has **zero
applicability** — fourth population with no surface for it. **It should stop being briefed as a
lever.**

But both of the *tells* I propagated were wrong, and the corrections compose:

- **"The discriminator is purely numeric" is false — stacking DESTROYS a table.** Stacked case labels
  share one rtl label, so `group_case_nodes` merges them into a **range node**, and the table/tree
  decision counts nodes *after* the merge. The same values stacked emit a three-compare tree; one arm
  per case emits the table. **And the disassembly cannot tell the spellings apart** — separate arms
  with identical bodies produce coincident labels that print as one name repeated. A repeated label
  name is **not** evidence of stacking.
- **The `bcc` tell is wrong, and so is its replacement.** Brief C found the real signal was the
  *absence of a `sub`* (minval 0 ⇒ `bhi`, not `bcc`). Brief D then found a hidden case where **`sub`
  IS present** and the default-sharing case sits at the **TOP** of the range. **The robust tell is
  arithmetic:** derive the node count the table implies and test it against 5 *and* against
  range/10. A mnemonic is a hint; the formula is the test.
- **`minval` needs four forms** — `add r3,#1` gives minval −1, and a **pooled negative**
  (`=0xfffffe84`) gives 0x17c. A `sub`-only screen calls both zero and hunts a lowest case that
  doesn't exist.
- **A table can have a margin of ONE**: 100 entries, 10 nodes, range 99, `10×10 = 100 ≥ 99`. Nine
  nodes and the table vanishes. A check, not a lever.
- **"33 jump tables" was an ENTRY count.** Dispatch *sites* are 2/1/1/0 and 3/3/5/0.

## Levers that paid

- **A TYPE, not a spelling, was the best single edit — 36 bytes and 25 encodings in ONE TOKEN.**
  `unsigned short v = *src++;` emits `ldrsh` + `lsl #16` + `lsr #16`, giving 11 `ldrsh` where the
  reference has **zero**; `int v = *src++;` removes all of it. One grep for `ldrsh` screens it.
- **Rung 8 paid immediately on the same function**: at count +1 of 849 the histogram read
  `bcc −21 | blt +21 | lsr −21 | asr +21` — **42 instructions of pure signedness cancelling inside
  the count**.
- **A histogram localised a 153-encoding residue to ONE variable.** `Anim_Boreas` reads `ldr −48 /
  mov +41` and nothing else above 6: the ROM reloads `frame` from slot `0x28` **fifty times** where
  the candidate keeps it in a register, and that one missing slot shifts the other nine and all three
  aggregates. One added slot is predicted to land the entire frame.
- **`int *p` for a state store** instead of `*(int*)(base+k) =` — **+7.1 aligned points**.
- **A local pair per 5th/6th stack argument**, 61 sites — +4.8.
- **BUILD MULTIPLICITY is the hold-versus-rebuild discriminator**: a wide constant built *exactly
  once* is held in a callee-saved register; built more than once it goes straight to an argument
  register. It needs **no live-range reasoning** and screens the population in the same pass.
- **A tiled index must be a flat sum of `<<` terms, not `*`** (`fold` factors out the common power of
  two into a Horner form, two extra `lsl` per site × 16 plots) — **but the divisions stay `/`**, since
  `x/8` shows the signed bias correction. Opposite directions in one function.
- **A negative array index is not the negative-offset spelling**: `p[-1]` gives 3 instructions,
  `*(u8**)((char*)p - 4)` gives the ROM's 2.
- **The mixed-placement rule** — reference *reloads* ⇒ **pin**; reference *holds* ⇒ **two-step
  computed form** — with the two lists **disjoint and covering every value**, so the zero-sum
  objection does not apply.

## The pin reconciliation needed one more qualifier

Batch 312 concluded *over call sites select, over constants do not*. Here **blanket beat selective on
every axis** — 276 against 845, including on selection's own axis. Not a third contradiction: **when
the sites are chosen BY THE CONSTANTS THEY CARRY, selection over sites IS selection over constants.**
312's agent selected by a *site property*; 313's by *constant identity*. The rule is about the
**selection criterion**, not the unit.

## Bounds

- **Neither operand order nor associativity at a commutative site is readable from the output** —
  `mul`, `and`/`orr`, and add grouping (the last measured **50 encodings negative** when read back as
  source parenthesisation). `Anim_Gaia`'s lever 9 is unsafe; the real discriminator is a **use count**,
  since a cost-1 mask used twice in a call-containing loop is hoisted by `loop.c` from a plain literal.
- **Two axes matching does not mean the object is close.** Nine address pins were size- **and**
  count-identical while the object carried ~60 `.s` differences **including a permuted spill-slot
  map**. Sharper than rung 3: both axes are **blind**, not merely insufficient.
- **A frame-size match can be a coincidence** — `0x38` both sides, ROM at 9 live slots + 3 dead words
  against 12 live + 1 dead.
- **Rung 3 at its cleanest**: a probe gave **count exact at 1095/1095** while aligncmp *collapsed* 4.8
  points and the frame stayed 8 bytes over. The agent: *"had I ranked on objcmp alone I'd have
  installed it."*
- **Hoisting `int j = 0;` out of a loop was inert on all four figures and the slot map**, despite its
  precondition holding — a fourth bound on initialise-at-declaration.
- **Gaia's two-aggregate layout rule doesn't generalise to three**: the first-declared takes the
  **lowest** offset and the remainder are reversed **above** it.
- **A low back-edge count is a positive result** — one function has ONE backward edge against 40
  `cmp`, another 8 edges in 2,476 instructions, ruling the whole loop-form family out in one pass.
- The **eight-bit-movable pooled screen** is inert again — **fifth consecutive population**. Demoted
  from a lever to a conditional screen.

## My own errors this batch

- **Three functions mislabelled "no split."** I counted `thumb_func_start` occurrences and never ran
  `datacheck.py`. **A one-function file can still need a TEXT/DATA split**, and two of those targets
  **could not have landed at any candidate quality** because their exports did not exist. Run
  `datacheck.py` while *assigning* work, not while installing it.
- **I cited one of three `docs/ANALYSIS_*` files.** The one I missed says in its own headline *"THE
  LEVER THIS ONE WANTS IS NOT PINS"* — and my brief for that function led with the pin pass. It was
  also **low by three** on its named-constant count, from a high-register-only survey that missed
  three held in r5/r6/r7.
- **A rule I briefed as the batch's open question was already solved** — written down and measured in
  a park I installed last batch. The document carried both halves separately and said only that the
  function "stays mixed," which reads as unsolved. **Extracting a park's rule is part of installing
  it**; second time a park has held an answer the doc didn't.
- **The "shared family prologue" I briefed buys nothing** — it opens **338 of the 871** remaining
  functions, 39% of the tree, with 93 already in generated output. A family claim needs a shape that
  is **rare**, and rarity is a tree-wide count.
- **There is no single split serving a family.** `split_s.py` cuts one named target, so first → 2-way,
  middle → 3-way, last → 2-way. I repeated "one three-way split serves all three" in batch 312's
  report. Third instance of that symmetry error.

## Housekeeping

- **A SUFFIX COLLISION**: five parks plus one recon all name `rom_e7320_c_c_b` as their own output
  stem, and only one can have it. All six annotated.
- **14 parks had recipes naming references that no longer exist** — twelve repaired by re-indexing
  `asm/`, and **two whose functions had already landed**, retired to `toDelete/`.
- **Eight parks claimed a figure with no runnable recipe.** Six confirmed exactly; **two were lying**,
  both saturated.
- **`parkcheck.py`** gained `NOFIGURE`, `NOBODY` and shell-quote handling; **`datacheck.py`** now sees
  `.lcomm`-defined symbols (its old recipe would have failed to link).
- **The decimal-immediate trap was hit independently by three agents** — `str rX,[sp,#4]` is invisible
  to a `#0x`-keyed regex.

## Next

All remaining work is **residue reduction on 831 parks**. The closest, ranked:

| park | distance |
|---|---|
| `rom_b5000/80b9554.c` | **0 of 81** — deliberate "do not land", waits on two siblings |
| `ovl_7a7298/2009fa4.c` | 1 of 199 |
| `ovl_77dd1c/2009154.c` | **2 of 160** |
| `ovl_7f6e64/200cbec.c` | **2 of 1068** — size, count *and* relocations all exact |
| nine more at 2, thirty within 10 | |

`Anim_Boreas`'s one-slot prediction and `OvlFunc_884_20097c8` at 97.3% are the best-placed new work.
