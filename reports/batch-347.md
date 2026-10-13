# Batch 347 — targeting on a wrong premise, and correcting my own

**5 briefs, 20 targets, 2 landings, 7 length gaps closed.** Park files
**618 → 616**. 5,001 / 5,710 matched (87.6%).

## The selection criterion, which was new

Every target was a park whose **own header falsely asserts its figure is a true
distance**. The hypothesis: because that claim is load-bearing — it is the
sentence telling the next agent the figure can be trusted — the park's diagnosis,
blocker class and especially its **recorded-negative list** were all produced
under a false premise, and should be treated as untrusted rather than unverified.

**The hypothesis held.** Of the twenty parks, only two had a diagnosis that
survived intact, and in one of those only the location was lost. What the false
claim had actually caused:

- one park had **the gap's direction inverted** — "four instructions short" when
  the body is one instruction *long* and six pool words short
- another aimed **nine spellings at a site costing zero instructions**
- a third called two shifts "cancelling" when the region gains the **pad**, not
  an instruction
- a fourth measured "two instructions over" off *stream* hunk spans; the real
  surplus was one `mov fp,r3`
- a fifth had ranked the actual cause **last and dismissed it** — "gcc is right
  and the ROM went the long way round"

Brief C sharpened it further: **three of its four were refutable from the header
alone**, each asserting "the figure IS a distance" two lines below its own
contradicting `SIZE ref N, ours M` line.

## I GOT THE SIZE OF THE PROBLEM WRONG, BY THE DEFECT I WAS DESCRIBING

I reported **100** parks. The real number is **53**.

`grep -i 'true distance'` matches **`NOT A TRUE DISTANCE`** — a header that is
correct and says so. Brief A found this by being handed exactly such a park:
`Func_80b5f0c`'s line 6 has read `NOT A TRUE DISTANCE`, correctly, since batch
335. So the criterion was partly handing agents the parks that *least* needed
re-deriving.

    256 files match the bare phrase
    224 survive a naive negation filter
    180 are a claim about THIS park's own figure in its own header
     53 OF THOSE ARE OUT OF PHASE          <- the honest number

The 44 dropped at the last step were prose about *other* parks — one match reads
"312 brief C spent its budget reaching a real distance on its other two".

**And the provenance is the useful half: 35 of the 53 carry one sentence** —
*"Re-classified in batch 322; the figure IS a distance."* 59 parks carry it, so
**batch 322's re-classification sweep was wrong on 35 of 59.** One bad sweep,
fixable in a single pass, rather than the diffuse corpus-wide habit I described.

Corrected in `docs/elevation.md`, `reports/batch-346.md` and the `HANDOFF.md` row
rather than left standing. `parkcheck`'s cross-check carried the same negation bug
for one batch; fixed and verified both ways.

## Landings

| function | before | how |
|---|---|---|
| `OvlFunc_926_2008e94` | 88 (−2) | a union restores the dependence; the park's "unanswerable" was a net-figure artifact |
| `Func_8026e80` | 90 (−2) | **the park tested the wrong base pointer** |

- `2008e94`'s park called its aliasing question unanswerable because it read a
  **net** figure under two flag settings and saw the sign flip. The aliasing
  diagnosis was exactly right for the post-loop block — all four missing
  instructions, one cause (`c-common.c:3340-3345`) — and the +3 was unrelated:
  `calls.c:855` already precomputes argument constants, so naming them only adds
  a live range that loses the low callee-saved register.
- `8026e80`'s two biggest contributors were not length defects at all. The park
  **tested the wrong base pointer** — the last two halfword tests read `e`, not
  `q`, all four being consecutive halfwords of one record — plus a named
  temporary. The final encoding was `optabs.c:658-666`.

## Gaps closed

| park | before | after | counts |
|---|---|---|---|
| `Anim_Spire` | 249 | **36** | 384/384 insns, 956/956 bytes |
| `OvlFunc_883_200aa54` | 215 | **40** | 184/184, 604/604, 59/59 pool |
| `OvlFunc_963_2008124` | 143 | **33** | 119/119, 356/356 — **a real distance now** |
| `OvlFunc_933_2009638` | 245 | **21** | 216/216 |
| `GetLocationName` | 62 | **18** | 58/58, 128/128 |
| `OvlFunc_881_200c058` | 111 | **21** | bytes equal; gap sign **flipped**, header says so |
| `Func_801a66c` | 112 | **76** | 121/121, 268/268 |

## Mechanisms

- **`reload_cse_move2add`** (`reload1.c:8840-8990`) closed two gaps, with
  preconditions **the same hard register** (`:8884-8893`),
  `reg_set_luid > last_label_luid` (`:8872`) and
  `rtx_cost(new,PLUS) < rtx_cost(src,SET)` (`:8905`). Two headers had called this
  `cse` — but `cse.c:2074`'s `use_related_value` tests `GET_CODE (x) == CONST`
  and is documented for symbolic CONSTs, so it cannot relate two `CONST_INT`s.
- **Reload inheritance cannot cross a `CODE_LABEL`**: `reload1.c:3977-3978`
  clears `reg_reloaded_valid` (tested `:5339-5340`), `reload_combine` refuses at
  `:8417-8420`, cselib forgets at a label (`simplify-rtx.c:3180-3181`). Voids a
  park's stated mechanism outright.
- **The `(plus reg reg)` fold is two-way and per site.** A *variable* offset
  makes a byte address `(plus reg reg)` — a legal Thumb `ldrb` address — so
  combine folds the add away; a literal makes it `(plus reg const_int)`,
  unencodable, so the reference's separate add survives. One brief closed a gap
  by **removing** an offset variable and measured the same lever firmly negative
  on another park.
- **Name the ADDRESS, not the offset** — `p = g + 0x242;` as its own statement
  took a park 57 → 33. The opposite of the fold above, and a different effect:
  it takes the arithmetic out of the addressing mode entirely.
- **A HImode store of any literal ALWAYS pools it** — `arm.md:4269-4271`, with
  alternative 1's `n` tested before alternative 5's `I` (`:4319-4320`),
  `pool_range` 64 against SImode's 1020. Closed one park's pool words *and* its
  `b` over a mid-function pool.
- **`find_reg` blocks r12/r14 only for call-crossing pseudos**
  (`global.c:986-991`), so moving `r = NULL;` across the call closed one gap.
- **Deleting a name** closed `Anim_Spire`: `cnt - 1` was gcc's own CSE temporary,
  not a source variable. Named, it takes a low-numbered DECL pseudo in the
  spilled frame-map group and each use pays its own reload; unnamed it becomes a
  high-numbered cse temporary in the same slot with the ROM's register roles.

## The dominance contradiction is resolved

`elevation.md` recorded two parks where the ROM rebuilds a constant it "should
have hoisted", noting the likeliest reading was **two different symbols
coinciding in value** — explicitly flagged as inference, not measurement.

That inference is **refuted**: gcc-2.96 always pools a symbol address, and the
ROM builds its value with `mov`+`lsl`. What it needed instead:

- **the hoist is `cse` pass 1, not PRE** — measured with `-fno-gcse` in force,
  `.00.rtl` holds five `const_int` sets and `.03.cse` holds one, so it survives
  `-fno-gcse` entirely, which is why every gcse-aimed probe measured inert;
- **the gate is `LABEL_NUSES (JUMP_LABEL (p)) == 1`** in
  `cse_end_of_basic_block` — **`cse.c:6642`** (the brief cited `:6534`, which is
  where the function opens; checked against the compiler source). A block reached
  twice rebuilds the constant *correctly*, with no second symbol involved.

## Tool fixes, including a correction to my own first attempt

- **`objcmp` now states the pool gap when both dimensions move.** I first added a
  standalone pool-gap report and then found it **duplicated a check that already
  existed** for the equal-instruction case. The real gap was narrower: when
  instructions *and* pools both move, the pool delta sat in a parenthetical. It
  now reads e.g. **"+1 instructions but −6 32-bit entries"** — exactly the park
  whose header had inverted the direction off the instruction count alone.
- **`parkcheck`'s distance cross-check** no longer fires on the negation.
- **`autosweep`** no longer claims "Both cheap axes are CLOSED" when run with a
  single axis flag — a false completeness claim, and precisely the sentence that
  gets copied into a park header as a recorded negative.

## Cautions about our own measurement

- **A wrong program scored 21 and compiled clean**: a `short` at an odd struct
  offset pads to the next even one, so it wrote to `a+0x65`. **Second batch
  running** in which a sweep-scored variant was semantically wrong. A score is
  not a proof of equivalence.
- **Ten variants on one park were bit-identical to the base**, so its recorded
  "163 → 181" harm cannot be reproduced from any spelling — it never measured
  anything. The mirror of this appeared too: a recorded *positive* (`j = 0;` at
  the top "produces the ROM's frame") is bit-identical as written, because gcc
  deletes the dead store, and placed properly makes the frame *smaller*.
- **`-fno-cse-follow-jumps` and `-fno-cse-skip-blocks` are each bit-identical
  alone and decisive together.** `autosweep`'s flag axis tries one group at a
  time, so it structurally records **two false clean negatives** on the mechanism
  they jointly control.
- **The rung that equalises the count is not always the rung that helps.** On one
  park the count-equalising rungs left aligned at 100 while the rung that
  *reopened* the count took aligned 78 → 57.
- **`shimcount` does not detect the never-read-union-member device class** — one
  landing carried one while reporting clean.
- **120 variants on one park hit the reference's instruction count with a
  *better* aligned residue while being wrong**, separated only by the prologue's
  high-register save count.

## For the fifth and sixth batch running

The decisive lever was found **inside the park's own recorded-negative list**
again — twice in one brief, and one such negative is *bit-identical* on the
closed body. Another park's "own bracket contained the answer and was read as
proving no answer existed."

A negative is a measurement of **one body** and expires silently when the body
changes.

## Open

- One park's anchor axis is **not** closed (`autosweep` capped at 40 of 43).
- `-fno-gcse -fno-strict-aliasing` together reach +1 with equal bytes on one
  park, decomposing exactly along its three regions — but no combined Makefile
  group exists. Recorded, not proposed.
- Owner decisions still queued: `_CONST_0` in `const.sym`, the
  `__divsi3 = _divsi3_RAM;` alias for `OvlFunc_common1_78`, the `:: "i"(0)`
  device question, and the 23 undefined-extern address parks.
- **35 parks carry the bad batch-322 sentence and should be corrected in one
  pass** — a cheap, mechanical cleanup that removes a standing trap.
