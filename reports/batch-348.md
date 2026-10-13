# Batch 348 — one landing, and the anchor axis was testing a third of what it claimed

**5 briefs, 20 targets, 1 landing, 7 parks improved.** Park files **618 → 615**.

## The landing

**`OvlFunc_907_2009684` 3 → 0** — byte-identical at `--func` *and* `--whole`,
284 bytes, 111 encodings, 27 relocations. Pin-free, device-free, converts whole.

The lever is allocation **route 4** (a block boundary): the `__GetFlag(0x923)`
arm's two shifted constants assigned to locals above the whole if/else-if chain.
Both hoists are needed — either alone reads 2 or 5.

**And the mechanism bounds the park's entire prior search.**
`precompute_register_parameters` (`calls.c:848-856`) precomputes an argument only
if its value is a non-REG rtx **costing more than 2**. `(const_int 20)` is one
`mov`, so argument 0 fails the cost test, and a *variable* is excluded by the REG
clause — so **no spelling of argument 0 can reach the precompute loop at all.**
That one bound explains the park's eight inert spellings plus five more measured
this round.

## Parks improved

| park | before | after | note |
|---|---|---|---|
| `Func_807905c` | 38 | **8** | gap closed, 264/264, 616/616 |
| `Func_801dd28` | 140 | **106** | anchor axis never run on it before |
| `Func_80bb7c0` | 97 | **94** | instruction counts now equal |
| `OvlFunc_901_2008640` | 16 | **14** | both steps from its own recorded-inert blocks |
| `20082f0`, `20083d0` | 4, 4 | **2, 2** | twins, one line each |
| `ActorCmd_Player_Climb` | 168 | — | gap −2 → −1 |

## THE ANCHOR AXIS WAS TESTING A THIRD OF WHAT IT REPORTED

The session's most consequential tooling finding, upstream of everything the
sweep route claims.

Briefs kept independently rediscovering that blocks of their variants were
**bit-identical** — ten spellings in 346, fourteen in 347, ten in 348 — and each
time those rows were filed as negatives when they had measured nothing. Brief C
tried detecting it by hashing the candidate's **`.s` text**, which cannot work:
gcc writes `.file "<name>.c"`, so variants in different files always differ
textually while being the same program. **That check passes everything**, the
worst direction.

Object fingerprinting (encodings + relocations) went into `sweep_variants.py`,
the shared harness, and re-running the 30 parks whose headers had just been
marked "both axes closed" gave:

```
30 parks re-checked          30 of 30 affected
710 boundaries swept
443 BIT-IDENTICAL -- measured NOTHING     (62%)
267 distinct programs actually tested
```

Per park, bad to total: `2008904` tested **13 distinct programs out of 61
placements**; `807905c` 26 of 56; `200985c` 3 of 18; and **three parks had every
placement bit-identical**, so their anchor axis measured nothing while reporting
a clean sweep.

**What it corrects:**

- The 29 park headers written in the batch-347 harvest pass now carry
  **distinct-program counts** instead of boundary counts, and the three that
  measured nothing say so. A recorded negative that is really a non-measurement
  is worse than no note — it tells the next agent to skip something never tried.
- It **partly explains the harvest pass's zero landings**, which I had attributed
  to those parks being hand-worked. True, but the axis relied on was also
  covering a third of the space its count implied.
- Read the other way: **six landings came from this axis anyway**, so it pays
  despite a low distinct-program count.

**CORRECTION to my first reading, same day.** I wrote that the remaining two
thirds are "unexplored rather than exhausted". That inverts the conclusion: a
byte-identical object is the **strongest** negative, not an untried point — the
compiler emitted the same program, so that placement provably cannot be the
lever. The three parks whose every placement was bit-identical have an anchor
axis closed *more* firmly than a figure-based sweep could close it.

What the 62% corrects is the **instrument**, not the search space: "61 placements
swept" sounds like 61 chances and was 13 programs, so cross-park comparisons must
use the distinct count. And a high bit-identical rate is a steer **away** from a
park — it means gcc's own canonicalisation dominates that body's scheduling, so
placement levers of any kind are unlikely to reach it. Those parks need a
different axis, not more anchors.

**The rule, now in `elevation.md`:** a variant producing a byte-identical object
is **not a negative, it is a non-measurement**. Record the distinct-program
count, never the placement count.

## THE SELECTION CRITERION FAILED, FIVE BRIEFS FOR FIVE

Batch 348 was drawn from "parks with no recorded batch work at all", measured at
259. Every brief reported it false for its targets.

```
259  my count
143  after fixing the regex
 45  parks carrying NO recorded measurement of any kind   <- the real pool
```

Only **2 of the 20 targets** were in that state. The query was
`grep -o 'batch[ -]3[0-9][0-9]'`: **case-sensitive** (headers write `BATCH 341`)
and **3xx-only** (batches 115, 220, 288, 305 all cited and all invisible).

**That is three narrow-regex errors in one session, all the same shape:**

| instrument | narrowness | cost |
|---|---|---|
| `gapscan` separator class | no `*` for comment continuations | 550 of 606 parks skipped; pool "99", really 192 |
| the distance-claim sweep | matched the **negation** as an assertion | "100" false claims, really 53 |
| the batch-work proxy | case-sensitive, 3xx-only | "259" unworked, really 45 |

Each produced a confident number written from the one spelling in front of me,
applied to a corpus with several — and **each came out favourable**, a bigger
pool or a bigger problem, which is exactly why none looked wrong.

**The rule this earns:** before a count becomes a targeting criterion, test the
query against a case it must catch **and** a case it must reject. All three would
have died on one such test.

And the deeper error: **"carries no batch stamp" is not "has never been worked."**
Two parks hold recorded sweeps of 220 and 115 variants with no batch number
anywhere. Measure the thing, not a proxy for it.

What *did* pay was the brief's other instruction, which brief A singled out: **go
straight to the mechanism, do not re-screen closed axes.** Twelve of twenty
targets were at equal instruction length — a property measured correctly — so no
brief lost its round closing a gap.

## Mechanisms

- **`optabs.c:663`'s `rtx_equal_p(op1, target)` forces the operand swap** for
  `*f = X | *f`, because the store target *is* one operand — which is why the
  whole `|=` spelling family is **bit-identical** rather than merely inert.
  `local-alloc.c:1146-1155` can only ever tie operand 1 there, so no
  register-level lever exists; `optabs.c:640-642`'s HImode `force_reg` pools
  the 2.
- **`loop.c` can never hoist an ordinary load out of a loop containing a call.**
  `:2444-2447` sets `unknown_address_altered` for any non-const `CALL_INSN`, and
  `:3265-3267` returns 0 for every non-`RTX_UNCHANGING_P` MEM **before** the
  store walk at `:3269-3277`. A park blamed `true_dependence` and an alias set;
  `true_dependence` is never reached. The ROM's sequence is a read written
  *before* the loop, not a hoist.
- **The minipool is held in order of increasing `max_address`** (`arm.c:4681-4682`),
  `max_address = fix->address + fix->forwards` (`:4826`), and `forwards` is
  `pool_range` — **1020** for `*thumb_movsi_insn` (`arm.md:3294`) against **64**
  for `*thumb_movhi_insn` alt 1 (`:4353`). On `WaitTextPrompt` that single split
  produces the five `[pc,#N]` encodings, three relocations, two transposed pool
  words and two instructions on the wrong side of the barrier — **its entire 33
  is one defect**, and the ROM's shape already byte-matches in
  `src/rom_8a000/rom_944ec_a_a_a_a_c_c_a_c_b.c`.
- **A cluster can be a PREFERENCE, not a priority.** `.18.greg` reading
  `;; 53 preferences: 0` with no hard register in the conflict list means
  `find_reg` hands it r0 on the preference pass (`global.c:1103-1110`) whatever
  its rank — so chasing a priority ratio could never work. The preference arrives
  via `set_preference`'s `src = XEXP (src, 0)` (`global.c:1585-1650`, called at
  `:1440`).
- **The gate on r1/r2/r3 is `spill_cost`** (`reload1.c:1495-1512`), **not
  `bad_spill_regs`** as batch 336 recorded — a much weaker condition, and
  `.18.greg` shows pseudos already in r0–r3, so the barrier is reload
  *placement*.
- **`local-alloc` owns a pseudo `.17.lreg` reports as `in N`** — on one park
  `;; Register 41 in 5.` with zero call crossings, so `find_reg`'s
  call-clobbered exclusion was never the cause; `QTY_CMP_PRI`'s `death − birth`
  denominator was.

## Cautions about measurement

- **A wrong program scored better than base — third batch running.** One
  generator produced `xx = xx;`, which compiled clean, read an uninitialised slot,
  and beat base on both metrics. Caught by reading the emitted asm.
- **A recorded negative was scored on the wrong metric.** `p0 = gBuffer; p0 += off;`
  is filed as "WORSE (214 of 218)" — raw ndiff. On aligned residue it is the best
  state ever measured on that function (91 against 123).
- **`tryc --align` is structurally blind to one axis**: its `DESTRUCTIVE` regex
  folds `add r0, r0, r3` into `add r0, r3`, so it cannot see the
  destructive-vs-three-operand distinction one function turns on. It also counts
  label renumbering as difference, reading 61 where `aligncmp` read 12 hunks.
- **Four placements tied at one figure with four distinct md5s** — same number,
  four different programs.
- **A park's split shape can be stale.** Two were wrong this batch: one named a
  three-member `.s` that holds two, another asked for a split `split_s.py` says
  is unnecessary.

## Open

- `WaitTextPrompt` is one defect away, with the ROM's shape already in the tree;
  six declaration shapes were measured and none triggers it, so the divergence is
  a combine-order / `LOG_LINK` property of the surrounding function.
- **The remaining two thirds of the anchor axis are unexplored** on every park
  swept before this batch.
- 35 parks still carry the bad batch-322 distance sentence.
- Owner decisions unchanged: `_CONST_0`, the `__divsi3` alias, `:: "i"(0)`, and
  the 23 undefined-extern address parks.
