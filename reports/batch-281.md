# Batch 281 — fifteen functions, five whole files, and the measurement rule earns three more angles

Gated on a clean `make clean && make -j8 && make compare`, green at the target SHA1
`5c4695205413df7db52b9a184815a07783999971`. **All fifteen verified** — fourteen at the exact ROM
address their name encodes, one named function (`UI_Sanctum`) presence-checked the same way. `git
status` clean.

| | |
|---|---|
| elevated | **15** |
| parked | **20** |
| `.sym` entries added | **2** (`_MSG_d27`, `_AREA_32`) — three more reported and withheld |
| whole-file conversions | **5** |
| **fakematch debt added** | 12 |

**Ten subagents, 45 targets, zero duplicate assignments** — verified programmatically before launch,
which is what batch 280 got wrong.

| agent | cluster | landed |
|---|---|---|
| A | overlay 899 | **3** of 4 — two whole files |
| B | overlays 945 / 971 | **3** of 5 — two whole files |
| D | overlays 918 / 887 | **2** of 4 — plus two parks at 4 and 5 encodings |
| E | overlays 921 / 881 | **2** of 4 — a whole-file pair |
| C | overlays 882 / 924 | **1** of 4 — plus a park at 2 encodings |
| F | overlays 956 / 969 | **1** of 5 |
| G | rom_b0000 | **2** of 5 — a whole file with **no scaffolding at all** |
| J | rom_b5000 / rom_a1000 | **1** of 5 — plus parks at 8 and 10 encodings |
| H | rom_c9000 animations | 0 of 4 |
| I | rom_c9000 animations | 0 of 5 |

**A new size record: `OvlFunc_918_2008918` at 588 instructions and 1600 bytes**, against batch 280's
397 and 1148. It ships 63 pin blocks, and that count is *measured* — folding every pin back to a
plain literal call gives 586 of 614 differing and +32 bytes.

## The measurement rule, three more angles — and one of them would have shipped a worse file

Batch 280 learned that a normalised count understates the distance when the pool has moved. This
batch found the other two directions:

- **It can OVERSTATE to the point of calling a perfect match 230 differences.** `tryc` reports the
  byte-identical `OvlFunc_945_200aff0` as "230 differ" — all of it label renumbering, because 54
  jump-table words diverge its positional label map at the first table and it never re-synchronises.
  **`tryc`'s count is noise on any function containing a jump table.** Check for `mov pc, rN` or a run
  of `.word .L` the same way you check where the pool sits.
- **Once the instruction counts disagree, its ORDERING is wrong, not merely imprecise.** On
  `OvlFunc_882_2008434` the normalised count ranked a 433-encoding candidate *best* (64 regions) and
  the actual best, 424, *worst* (76). Picking by it would have shipped the worse file. Every
  subsequent ladder step was re-scored with `objcmp`.
- **A disagreeing REFERENCE count exposes a bad baseline instantly**, which is why the rule is to quote
  it beside the difference count.

## Five whole-file conversions, and two traps in classifying them

`ovl_30_a_c_c_c_a_a_c.s` (both 899 functions), `ovl_30_a_c_c_c_c_c_a.s`, `ovl_30_c_c_c_c_c_a_a_a_c_c_a.s`
(both 921 functions), `ovl_314_c_c_a_a_c.s`, `rom_b0070_c_c_a_c_c_a_a.s` (both, and with no pins, no
barriers, no `volatile` — the only non-plain constructs are two `_MSG_*` references and one struct cast
whose entire purpose is an alias set).

**I misclassified two targets as whole-file-convertible, both the same way.** `OvlFunc_971_20092e0` and
`OvlFunc_924_200a8b0` are each one function, but each carries a `.section .data` — so each needs a
text/data split across two or three linker-script lines. `tools/datacheck.py` reports exactly that and
I did not run it; I counted functions and inferred convertibility. **That check belongs in target
selection.** The second one was recoverable: rehoming `.L60b8` to its own `.s` and repointing the
`.data` line landed the function, after the rehome failed its first gate with an undefined reference and
needed the documented one-line `.global` export.

## The levers that landed things

- **SPLIT THE PINNED `mov`/`lsl` PAIR IN THE SOURCE** — the lever of the batch, and it supersedes half
  of a batch-280 finding. `{ PIN3; q1 = 0x80; q0 = 3; q1 <<= 7; q2 = 0; f(...); }` defeats the
  repeated-constant CSE *and* gives sched2 the ROM's interleave, because the source order now puts
  `INSN_LUID(mov r0)` between the mov and the lsl. Batch 280 cured the adjacency by *un*-pinning q1 —
  which lets the CSE straight back in when the constant repeats. 14 sites on one function (40 → 18 → 0).
  No flag reaches it.
- **`local-alloc` ORDERS QUANTITIES SHORTEST-LIVED FIRST — THE INVERSE OF `global_alloc`.** In a
  function with no branches at all, `global_alloc` never runs and `local-alloc.c:1480` is the whole
  policy. One reused pointer is one long quantity that sorts last; four separate pointers are four short
  ones that all get the ROM's register. **229 of 247 → 10 in one edit.**
- **AN ALIAS SET OF 0 ON A QImode STORE IS AN UNCONDITIONAL TRUE DEPENDENCE.** `true_dependence` returns
  1 without consulting aliasing at all, and `lang_get_alias_set` returns 0 for any char-precision
  reference — so `char *p; p[5] = 4;` blocks every later load from scheduling above it while
  `s->spr[5] = 4` does not. Found in `alias.c` after nine spellings sat flat.
- **`iwram_3001f2c` IS A STRUCT, NOT AN `unsigned char *` — worth about 290 encodings.** A struct member
  access takes the *record's* alias set, so two loads don't merge across an intervening store; and it
  fixes the addressing form for free, because a hand-cast `state + (0x208 + i*2)` gets reassociated by
  `fold` into `(state+i*2)+0x208` and materialises the address.
- **A SHARED HARD REGISTER IS EVIDENCE OF DISJOINT LIVE RANGES, NOT OF A SHARED VARIABLE.** Merging two
  values because the ROM keeps both in r8 is backwards — disjoint ranges share a register for free, and
  merging inflates refs and reorders `allocno_compare`. Separating them: 34 → 4. Batch 280's
  merge-live-ranges rule needed this converse.
- **COUNT THE ROM'S REGISTERS AGAINST ITS LIVE VALUES** — and this batch has both directions one commit
  apart. `OvlFunc_945_200dd10` needed *merging* (the ROM colours eight pseudos with five registers;
  403 → 10 in one edit); `Func_80b2b10` needed *separating*. Fewer registers than values means merge; a
  shared register with disjoint ranges means leave them apart.
- **WHEN NO PERMUTATION MOVES A FILL, TRY A BARRIER AFTER THE FIRST PINNED ASSIGNMENT.** All 25 mov
  permutations and all 6 shift orders were inert at 2; one `__asm__ volatile ("" : : "r" (q0));` took it
  to exact. Site-specific, not general — a 3×7 sweep of the same barrier found nothing on a sibling.
- **A NAMED MULTIPLIER LOCAL SUPPRESSES ONE LOOP'S giv STRENGTH REDUCTION**, and the mechanism is
  confirmed in `loop.c`: `REG_USERVAR_P` costs `benefit` its `copy_cost`, flipping
  `v->lifetime * threshold * benefit < insn_count`. **`-fno-strength-reduce` is strictly worse** (34 → 59)
  because it kills every giv in the TU including loops that already match — so **no flag row is needed
  anywhere for this class.** Its limit: `loop.c:4502` gates it on `v->dest_reg`, so it is inert on
  DEST_ADDR givs.
- **A MISSING EPILOGUE `mov r0, #0` IS A RETURN-TYPE TELL**, and `thumb_exit` reads the pop register from
  `DECL_MODE (DECL_RESULT (decl))` — so `pop {r1} / bx r1` in an interworking Thumb function that saves
  high registers is direct evidence the function returns a value, with no `return` statement anywhere.
  On one function that reading fixed an eleven-instruction basic-block reordering as a side effect, 53 → 30.

## Two things the tree already knew and this file did not

- **A pooled zero reaching a `strb` is the `struct HalfWord` case.** `*thumb_movqi_insn`'s alternative 1
  is `"l" <- "m"` with no `n`, so **a QImode constant can never pool** — only HImode can, and
  `PROMOTE_MODE` widens a plain `short` local to SImode but *not* a struct field. This was recorded in a
  landed `src/` header in batch ~275 and never reached `docs/elevation.md`, which cost the agent that
  rediscovered it real time. Now documented, with the one-line search that finds the class.
- **The `.call_via r4` clobber list in the landed template is wrong in both directions**, and one half is
  a real miscompile. `mov r12, pc / bx r4` puts the return address in r12, so `lr` **survives** — the ROM
  proves it by keeping a pointer in `lr` across both calls, and listing `"lr"` makes gcc refuse to use it.
  Meanwhile the ARM callee *does* clobber r0–r3, and without `"r2","r3"` gcc kept a base pointer in r2
  across the call.

## The HImode constant-store rule has no single direction

Five instances across two batches now, and they only look contradictory until you read them per site:

| function | what the ROM does | what the source needs |
|---|---|---|
| `OvlFunc_943_200a618` (280) | pooled zero for byte stores | **bare literal** |
| `OvlFunc_899_200a758` | `mov r2,#5 / strh` | dedicated `short *` **plus** an `int` carrier |
| `OvlFunc_971_20092e0` | four pooled, two `mov` — **in one function** | both, per site |
| `Func_80a90bc` | wants the early pool dump | **bare literal** (`hv[i] = -16`) |
| `Func_80ba6ac` | wants one end pool | `int` carrier, shared across both arms |

The discriminator is **not magnitude** (`0x54` is 84, also under 256) and which `*thumb_movhi_insn`
alternative reload picks was not determined by any of the five. **Read the ROM's instruction at each
halfword store site individually**, and don't generalise from a sibling store — let alone a sibling
function. A cheaper signal for the same question: **pool order is a readout of each constant's mode**,
since sorting by `max_address` only reproduces the ROM's order for one assignment of modes.

## rom_c9000 opened structurally, and landed nothing

Agents H and I were a deliberate bet on the battle-animation bank. **They landed zero of nine** — but
they returned the idiom, which is what the bet was for. `iwram_3001eec` is the head of a pointer table,
and `((char **)&sym)[0]` reproduces `ldmia r3!, {r1}` exactly with no walking pointer: a 16-instruction
prologue of that shape matched **on the first compile, in every register, across five functions**. The
particle/draw/descriptor struct shapes are recorded and reusable.

**And I briefed both agents wrongly.** I told them `rom_c9000` was "the largest untouched cluster,
essentially none ever attempted". It holds **137 landed `.c` files** and 20 parks. What is actually true
is narrower — every landed file is a small split-tail (2,738 lines total, largest 186), so the *large
entry points* are untouched. I inferred "nothing attempted" from my selector's count of 74 *available*
functions, which says nothing of the kind. Batch 280's lesson was "when a tool says something has never
been done, check the tool can see it"; **this is the adjacent error, asserting a negative the tool never
claimed.** The agent still running got the correction and the three donor files mid-flight — though I
misrouted it to the wrong agent first, which that agent caught and reported.

The bank's dominant blocker is now named: **`base` loses its high register**, because it is live across
~650 instructions with only ~15 references, and its def is `add rd, sp, #12` which requires LO_REGS in
Thumb — so every reference picks up a `mov rX, r8` reload copy. A high-register pin is the only handle,
and the precedent is already in the tree.

## Corrections I owe the log

**I published a claim that my own code in the same file disproved.** Batch 280 said `Func_808d9a4`'s
dot-prefixed byte table was "one permanent fakematch row nobody can remove… which no C identifier can
name" — while the park's C, committed in the same breath, contained
`extern const unsigned char tbl[] __asm__(".L9e680");`. Both halves false; the construct is in six
landed files and is not consistently booked as fakematch debt. Retracted in the report, the HANDOFF row
and the park, with the original text struck through rather than deleted. **The check this needs is to
read a park's header against its body before committing — the body is evidence about the header.**

**Two misclassifications from not running `datacheck.py`** (above), and **one misrouted agent message**.
I also hit the backtick-in-unquoted-heredoc bug again in a commit message, losing three fragments;
caught on read-back and amended.

## State

**4,513 from C / 1,197 in asm** against batch 280's 4,498 / 1,212 — exactly **+15/−15**, the
**sixteenth consecutive reconciling batch**. `census.py` TOTAL 1197 agrees with `funcindex` exactly.
**514 park files** (+20). `fakematch.txt` **500 rows** (+12). `dupfuncs.py` unchanged at 8 groups / 29
functions / 21 free, still a floor.

**454 available**, of which **124 in 201–400**, **79 in 401–800** and **61 above 800**. Only 21 remain
under 100 instructions.

> **Corrected after publication.** This section first said 510 parks and 460 available, because I
> published the batch without writing parks for four functions that had been attempted with real
> measurements — `Func_80b3050` (10 of 193, size *and* relocations *and* count identical),
> `Func_80b24e4` (126 of 204), `OvlFunc_956_200876c` (115 of 297, size and count exact) and
> `OvlFunc_969_200c23c` (616 of 677). Since `census.py` matches park files by name, all four were being
> counted as never-touched. **The arithmetic is what caught it:** 15 landed + 16 parked against 45
> targets leaves 14, and I had named only 10 as unopened. That reconciliation belongs *before*
> publishing, not after. With the four parks written it is 15 + 20 + 10 = 45 exactly.

## Open, and all of these are the user's call

- **Three `.sym` entries reported with in-function control and withheld** because none COMPLETES its
  function: **`_MSG_820`** (`Func_80bf678`) — whose control is the strongest yet recorded, the only
  shiftable id among seventeen, all sixteen others reproducing as plain literals — plus **`_MSG_cc3`**
  (`Func_80b24e4`, four controls) and the batch-280 three (`_MSG_2644`, `_MSG_920`, `_MSG_970`).
  `_MSG_820` and `_MSG_cc3` are the cleanest illustration yet that **evidence quality and completion are
  separate tests**: both are better-evidenced than `_MSG_d27`, which went in because it completed a file.
- **One symbol that measurement RETIRED:** `_MSG_c64` looked like the classic message-base shape on
  `OvlFunc_881_2009ca4` and reproduces from a plain literal. Measure before reporting a tell.
- **Two `.s` export lines, one-line and zero-byte each:** `.global .L2430` would put
  `OvlFunc_921_2009fa4` one instruction from landing; `Func_808d9a4`'s naming question is now retracted
  and needs nothing.
- **Three long-standing entries:** `_MSG_2080`, `_SIZE_80f0024`, `_TBL_7a828`.
- **Per-file flag rows:** `-fcall-saved-r4` for `OvlFunc_970_2008f80` and `ALIAS_CFLAGS` for
  `OvlFunc_882_200c41c` remain open; **`-fno-rerun-loop-opt` and `-fno-strength-reduce` are now argued
  AGAINST** — the latter explicitly, since it is strictly worse than the source cure. Agent C confirmed
  no `ALIAS_CFLAGS` row is needed for the two 882 functions it read.
- **The fakematch convention question**, ~20 of 181 PIN-using files unbooked.
- `OvlFunc_948_2009308`'s one pin, from batch 272.
- **Ten targets were never opened** (`OvlFunc_899_200afd4`, `OvlFunc_956_2009474`, `OvlFunc_956_2008da4`,
  `Func_80b0aac`, `Anim_Bind`, `Anim_PsyphonSeal`, `Anim_AstralBlast`, `Anim_PlanetDiver`,
  `BaseAnim_ParticleCloud`, `Anim_Unused_Fizz`) — recorded as unattempted rather than parked, since a
  12%-complete transcription carries no finding. Batch 280's carried-forward `OvlFunc_945_200aff0` landed
  this round **with zero pins**, so "unattempted" is not evidence of difficulty.
