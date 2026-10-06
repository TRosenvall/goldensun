# Batch 329 — eleven landings, and a duplicate search that returned a trustworthy nothing

Ten briefs, 39 targets. **11 functions landed**, 6 parks improved, 1 park figure
corrected, 30 parks re-verified by `parkcheck` at their claimed figures.

**4,883 → 4,894 of 5,710 (85.7%).** 816 remaining, 724 parked, 0 available.
Gated green at every phase; SHA1 `5c4695205413df7db52b9a184815a07783999971`.

## Landings

| brief | function | from | note |
|---|---|---|---|
| B | `Func_8029274` | 2 of 40 | ships an empty-asm barrier — **booked** in `pass3-depin.md` |
| D | `Func_80bf574` | 3 of 24 | the **type constructor** of the sum carrier |
| F | `OvlFunc_969_200b600` | 2 of 44 | |
| F | `OvlFunc_969_200db90` | 2 of 43 | |
| G | `OvlFunc_959_200d0e4` | 3 of 214 | park said **"a FLOOR"** |
| G | `OvlFunc_881_200b95c` | 6 of 72 | |
| H | `OvlFunc_882_20090a4` | 8 of 80 | |
| H | `OvlFunc_890_2008ef8` | 9 of 133 | **+ `200901c` + `2009140`** — one body, three functions |
| J | `OvlFunc_common1_15b8` | 6 of 34 | |

Parks improved: `Func_801c34c` 8 → 3, `DisplayMenuArrowCursor` 6 → 5,
`Func_80b6a60` 5 → 2, `Func_80f0254` 4 → 2, `Func_80f3858` 8 → 2,
`Func_80f7f30` 10 → 9. `OvlFunc_970_2008da4` **corrected** from 7 to 9.

## The organising idea held: a bound is an instrument change, not a verdict

Batch 328 left "organise by residue shape" as the plan. What it actually bought
was narrower and better: **five of the eleven landings came from a park that had
written down a bound and stopped.**

- `200d0e4`'s park said *"a FLOOR"*; the assignment moved to the **dominating
  block** lands it. Fifteen spellings across it and `200b95c` had all been tried
  *inside the guarded arm* — the one place the answer could not be.
- `2008ef8`'s park had tested each stack literal alone and in the entry block
  (137/136, 130/132) and called it unreachable. **Two** separate named locals,
  **block-scoped inside the arm**, read 3 first try.
- `common1_15b8` and `80f3858` had both rejected naming a zero — each having
  named it *together with* something else, which lets gcc reassociate.
- `80bf574`'s park had seventeen int-temp bodies and never varied the **type
  constructor**: `promote_mode` (`explow.c:895-902`) skips `RECORD_TYPE`, so a
  one-member struct of `unsigned char` stays QImode. struct / union / `b:8` /
  `[1]` all **0**; plain `unsigned char` **3**; `int` **9**.

> **The dimension nobody varied, for the eighth batch running.** And the new
> sharpening: when a park's bound is an arithmetic *impossibility*, that is a
> signal to change instrument — brief B found two of its four targets were
> sitting on a remedy this document files under "when priority arithmetic is
> unreachable", having spent five batches on spellings between them.

## `do { } while (0)` does not re-region the scheduler — amended

The method document said the wrapper *"splits one basic block into two
scheduling regions"*. It does not, and the mechanism quoted directly above that
sentence already said why: the note gives the following insn a `REG_DEP_ANTI` on
every prior use and set, which is a **total order through one point inside one
block**.

Brief F measured the difference and it decided two landings: the anchor has a
**position**, and on `200b600` moving it one statement earlier reads 0 where it
stood reads 2 and removing it reads 8. And an anchor **cannot do a region end's
job** — `20082cc` needs a real region end, where the anchor's own dependent caps
it at 3. The two cases look identical in the residue.

## The duplicate search, and a negative worth having

Brief F found that `OvlFunc_925_200b460` had **already landed** and its body
ported onto two parks — and that `dupfuncs.py` could never have said so, because
its file list excludes every piece with a sibling `.c`. A park whose twin has
landed is the cheapest landing there is.

`--vs-landed` now closes that gap and the answer is **none**: 774 remaining
against 4,883 landed, zero shared hashes. The negative is trustworthy because
the cross-half normaliser passes two controls — the 7 known hand-vs-hand groups
all survive the folds (**7/7**), and the same folds find **197 duplicate groups
covering 756 landed bodies**. Getting there needed three folds and a boundary
fix (a generated `.s` has no `.thumb_func_start`, spells `sl`/`lr` for
`r10`/`r14`, writes three-operand decimal, and ends at `.size` with its pool
*before* that).

**What it does not rule out is the live opportunity.** Equality is exact after a
normaliser that placeholders only `Func_` names, `.L` labels and `=` pool
operands — so a pair differing in a data symbol, a non-`Func` callee or one
immediate does not match. F's own case is exactly that: 42 instructions against
41, a near-twin with an extra call. *No free rename-only ports* is established;
*no free ports* is not. **Near-twins need a shape instrument, not a hash.**

## Three guards, each from a tool trusting prose

- **`install_batch` would have run a split for a PARK.** `do_splits` never looks
  at `figure`, and brief C shipped three park entries carrying aspirational
  splits ("the split you'd need *if* the owner approves"). Applying the phase
  would have consumed three `.s` files, added linker entries and promoted local
  labels to `.global` — for functions that are not landing. Now a validation
  error; the intent goes in `notes`.
- **A park's own PROPOSAL prose, read as its figure.** `2009a3c` *is* the
  batch-316 port, but kept the paragraph arguing for that replacement, which
  described "the installed park" in the third person and quoted
  `"181 of 177 -- SATURATED"`. `parkcheck` said `OK` — it takes the **first**
  claim, the correct 5. Brief A read the 181 and reported a "free improvement
  181 → 5" that batch 316 had banked, and `2009cb4`'s "43 → 40" has been 40
  since **batch 289**. Its three twin ports were **not installed** (no figure
  gain, and they would overwrite the batch-289/316 history); the reason is in
  `scratch_elev/b329/A/WHY-TWINS-NOT-INSTALLED.md`. **Sixth instance of prose
  that looks like machinery**, now a section of its own.
- **A flag-shaped field became a compiler argument.** `parkcheck.py:254` greps
  the header for `OBJCMP_EXTRA=(\S+)`; writing that spelling in a *sentence*
  made `80b6d30` report `TOOLING: Unrecognized option '-fno-gcse'`. First
  instance where the captured text was not a wrong number but a wrong build.

## Counters that read `git ls-files` are blind to an unstaged landing

`directory.py` and `dupfuncs.py` enumerate through git; `census.py` walks the
disk. A pre-staging regeneration reported the **old** 5,006 definitions and
4,484 landed `.c` while census had already moved to 4,894 — and looked entirely
plausible. The generator now says so in its own output. The same arithmetic was
the check in the other direction: the census-vs-`dupfuncs` gap grew by exactly
4, which was 4 functions in **untracked new split parts**.

## Bounds that got a side, without a landing

- **`2009818` — pin-free is REFUTED, not unfound.** Corpus scan of all 4,468
  generated `.s` for the ROM's interleave shape: **0 pin-free / 83 pinned** with
  no branch anywhere, and this function is straight-line. `arm.c:2078` gives an
  8-bit `CONST_INT` in a `SET` cost **0** against a pseudo's **1**, so cse always
  sinks the constant to its use. **The 0 at two pins is the pass-3 landing —
  stop searching.**
- **`200c260` — "why is r0 excluded?" → it is FOURTH IN LINE.** `arm.h:989` is
  `{3,2,1,0,…}` and `find_reg` tie-breaks on `inv_reg_alloc_order`. Free screen
  for the class: compile `-da`, grep `.18.greg` for `Using reg 0`.
- **Brief I's four parks all at 9 share one mechanism** — each is one or two hard
  register choices, every other differing instruction a scheduling consequence
  via the anti-dependence the chosen register creates. Three *different* passes
  make that decision, which is why no single lever fits. Two of the four are the
  same residue shape in different overlays.
- **Brief E refuted one of its own parks' bounds** (`HeightTile_A`'s "the flip is
  necessary and costs +1" — writing the bias early gives the flipped multiply at
  35 instructions) and **closed** two others by invariance rather than by sweep.
- **`80b6d30`: four batches of boundary work were aimed at the wrong pass.** The
  fold is `.07.gcse`, not `.09.cse2`; gcse's cprop is global so no boundary can
  stop it.

## Waiting on the owner

**`Func_80a8f40` is byte-identical** — `OK … 380 bytes, 167 encodings and 16
relocations identical` — spelling its message base as `(int)&_MSG_741`, exactly
as the loop three statements above already spells `0x333`. `_MSG_741` is not in
`message.sym` and **a new symbol entry is an owner call**, so it is parked at its
device-free **6**.

Brief C's argument that this differs in kind from the declined `_MSG_b24`:
necessity on the **register field** rather than the pool word (the ROM's
`ldr r3,[sp,#4]` is unreachable from a `const_int`), the same `id + base` idiom
already using an admitted symbol in the same function feeding the same callee,
and it passes the completion test. Seven device-free spellings are exactly inert
at 6 because cprop folds every one back. **If declined, the park is terminal.**

## Carried forward

- **Aim 330 at parks whose header still reads `"MEASURED, batch 319 recipe
  backfill"` with nothing after it.** Brief J took four and moved three. Two
  carried a claim that actively discouraged working them and **both of those
  moved** — a wrong claim is worth more than a right one, because it names the
  dimension nobody varied.
- **Check the landed count before treating an `upstream_module` listing as a
  plan.** At 5–29 landings it is a readable shortlist; `ovl_30` returns the same
  2,261-landing listing for every target in the bank.
- **Build the shape instrument** the `--vs-landed` negative points at.
- `OvlFunc_968_200c610` is a **landed near-twin of `200c2bc`** the park has never
  named, and its generated `.s` carries exactly the preheader shape `200c2bc`
  needs, twice.
- `OvlFunc_943_200985c` (9) and its piece-mate `OvlFunc_943_2009920` (5) are on
  the **identical** blocker — neither installs alone; work them as one target.
- Still 2 unbooked fakematch-class shims, listed at the top of `02-depin.md`.
