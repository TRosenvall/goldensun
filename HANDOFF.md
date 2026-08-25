# Handoff index

Batches of matching C ready to port into `Coaltergeist/goldensun-decomp`.

Each batch is self-contained: a table of what was elevated, what it replaces,
and anything found on the way that is worth passing upstream. Read them in
order; later ones assume the tooling from earlier ones is already in.

| Batch | Functions | Status |
|---|---|---|
| [batch-01](reports/batch-01.md) | 12 | ready to port |
| [batch-02](reports/batch-02.md) | 7 | ready to port |
| [batch-03](reports/batch-03.md) | 11 | ready to port |
| [batch-04](reports/batch-04.md) | 8 | ready to port |
| [batch-05](reports/batch-05.md) | 7 | ready to port |
| [batch-06](reports/batch-06.md) | 5 | ready to port |
| [batch-07](reports/batch-07.md) | 5 | ready to port |
| [batch-08](reports/batch-08.md) | 10 | ready to port |
| [batch-09](reports/batch-09.md) | 9 | ready to port |
| [batch-10](reports/batch-10.md) | 12 | ready to port |
| [batch-11](reports/batch-11.md) | 12 | ready to port |
| [batch-12](reports/batch-12.md) | 4 | ready to port |
| [batch-13](reports/batch-13.md) | 12 | ready to port |
| [batch-14](reports/batch-14.md) | 8 | ready to port |
| [batch-15](reports/batch-15.md) | 29 | ready to port |
| [batch-16](reports/batch-16.md) | 11 | ready to port |
| [batch-17](reports/batch-17.md) | 18 | ready to port |
| [batch-18](reports/batch-18.md) | 14 | ready to port |
| [batch-19](reports/batch-19.md) | 19 | ready to port |
| [batch-20](reports/batch-20.md) | 5 | ready to port |
| [batch-21](reports/batch-21.md) | 7 | ready to port |
| [batch-22](reports/batch-22.md) | 5 | ready to port |
| [batch-23](reports/batch-23.md) | 6 | ready to port |
| [batch-24](reports/batch-24.md) | 11 | ready to port |
| [batch-25](reports/batch-25.md) | 6 | ready to port |
| [batch-26](reports/batch-26.md) | 5 | ready to port |
| [batch-27](reports/batch-27.md) | 5 | ready to port |
| [batch-28](reports/batch-28.md) | 8 | ready to port |
| [batch-29](reports/batch-29.md) | 5 | ready to port |
| [batch-30](reports/batch-30.md) | 5 | ready to port |
| [batch-31](reports/batch-31.md) | 6 | ready to port |
| [batch-32](reports/batch-32.md) | 9 | ready to port |
| [batch-33](reports/batch-33.md) | 5 | ready to port |
| [batch-34](reports/batch-34.md) | 5 | ready to port |
| [batch-35](reports/batch-35.md) | 8 | ready to port |
| [batch-36](reports/batch-36.md) | 7 | ready to port |
| [batch-37](reports/batch-37.md) | 5 | ready to port |
| [batch-38](reports/batch-38.md) | 10 | ready to port — 7 are fakematches |
| [batch-39](reports/batch-39.md) | 7 | ready to port |
| [batch-40](reports/batch-40.md) | 8 | ready to port |
| [batch-41](reports/batch-41.md) | 6 | ready to port |
| [batch-42](reports/batch-42.md) | 6 | ready to port |
| [batch-43](reports/batch-43.md) | 8 | ready to port |
| [batch-44](reports/batch-44.md) | 8 | ready to port |
| [batch-45](reports/batch-45.md) | 5 | ready to port |
| [batch-46](reports/batch-46.md) | 7 | ready to port — 2 are unparks |
| [batch-47](reports/batch-47.md) | 6 | ready to port |
| [batch-48](reports/batch-48.md) | 7 | ready to port |
| [batch-49](reports/batch-49.md) | 9 | ready to port |
| [batch-50](reports/batch-50.md) | 7 | ready to port |
| [batch-51](reports/batch-51.md) | 11 | ready to port |
| [batch-52](reports/batch-52.md) | 7 | ready to port |
| [batch-53](reports/batch-53.md) | 8 | ready to port |
| [batch-54](reports/batch-54.md) | 6 | ready to port |
| [batch-55](reports/batch-55.md) | 6 | ready to port — 2 are unparks |
| [batch-56](reports/batch-56.md) | 6 | ready to port |
| [batch-57](reports/batch-57.md) | 6 | ready to port |
| [batch-58](reports/batch-58.md) | 6 | ready to port |
| [batch-59](reports/batch-59.md) | 6 | ready to port |
| [batch-60](reports/batch-60.md) | 5 | ready to port |
| [batch-61](reports/batch-61.md) | 5 | ready to port |
| [batch-62](reports/batch-62.md) | 5 | ready to port |
| [batch-63](reports/batch-63.md) | 5 | ready to port — 1 is an unpark |

**[Fakematch worklist](reports/fakematch-worklist.md)** — seven functions we
matched with inline asm rather than with a construct, all previously parked. The
debt is listed rather than buried, with twenty ruled-out formulations and the one
positive lead: `volatile` gives the right ordering in plain C and fails only on
the stack slot it also forces.

**[Cracking arg-interleave](reports/arg-interleave.md)** retires two blocker
classes that stood for 36 batches, and unblocks 516 functions. The lever is one
line of C: assign an argument's constant to a local in a DIFFERENT BASIC BLOCK
from the call. Read it before attempting anything with a displaced argument --
and read how it was found, because the method (search gcc's own output rather
than generating variants) applies to the classes that are still open.

**[Do large functions break the method?](reports/large-functions.md)** is not a
batch -- it is an experiment, and it is the most important document here for
anyone deciding what to work on next. Short version: length is not the problem,
blocker DENSITY is. 99% of functions over 400 instructions contain at least one
shape we cannot solve at any size, against 23% of those under 20. The binding
constraint is the number of unretired blocker classes, not the number of
unelevated functions.

Every batch is verified the same way, from a clean build:

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'

- **Fourteen `.global` lines have been added to eight existing `.s` files** (four
  in batch 09, three in batch 34, one in batch 35, one in batch 44, one in batch
  50, three in batch 52, one in batch 54) so that functions could be split out
  from the `.incbin` tables and script tables they select between. Every one of those
  files already exported sibling labels the same way, and a `.global` emits no
  bytes -- but it is the only change in these batches that edits assembly rather
  than replacing it. Reverts cleanly if you would rather it did not happen.

## Naming

`docs/names.md` is generated by `tools/name_evidence.py` and carries a proposed
name plus **the basis for it** for every function elevated. The rename pass is
deliberately deferred — names do not survive into the ROM, so a bulk rename is
byte-neutral — but the evidence is captured as it is produced.

Read the `Basis` column before trusting a name: `docs/attribution.md` records
that the inherited annotation corpus gets mechanism right and purpose wrong
often enough to matter.

## Standing items for review

Things surfaced across batches that need a decision from someone who knows the
codebase better than we do. Listed once here rather than repeated per batch.

- **`struct Actor` was 16 bytes short** and is corrected in batch 01, with
  `include/entity.h` folded into it. The five offsets that still have two
  competing readings are documented in the header itself.
- **The annotation corpus gets mechanism right and purpose wrong** often enough
  to matter -- see `docs/attribution.md`. Any annotation ported into this tree
  should be treated as a starting point, not a finding. One was corrected in
  batch 01 (`Func_80b7e7c` does not take the arguments it was documented with).
- **What are the id namespaces?** No longer blocking — answered mechanically
  in batch 07 — but still worth a real answer.

  Where the ROM pools a constant that would fit in an eight-bit `mov`
  (`ldr r0, =1`, `ldr r2, =0xf`, `ldr r3, =0x1d`), the operand was a **symbol
  reference** in the original source — gcc never pools what it can `mov`, and
  always pools a symbol address. Verified by assembling both forms.

  This was carried as the top blocker for twelve rounds, at a **measured 103
  of 395 overlay candidates**, on the reasoning that naming a namespace we
  could not identify was a bad trade. (It was reported as "three functions" in
  batches 04 and 05, which was what happened to be in front of me rather than
  the real number.)

  **That reasoning was wrong.** Matching needs the operand to be a *symbol*,
  not an *identified* one. Defining it by value in `area.sym` emits no
  bytes and asserts nothing — exactly what `message.sym` already does, per its
  own comment: *"named by value; pending semantic names."*

  So what remains is a naming question, not a blocker: `_AREA_4d` works, and a
  real name would be better. See `docs/elevation.md`, "Tell: the ROM pools a
  SMALL constant".

  **What the values themselves say.** There are **122 ids**, and **121 of them
  are compared in exactly one overlay** — which is what "each area's code lives
  in one place" looks like. That result is structural, read straight out of the
  ROM, and it is the strongest single argument for calling this an area id.
  `0x6a` is the lone exception, compared in two.

  The tables selected are named `MapEntrance_ARRAY_*`, `Events_TolbiSpring`,
  `Events_GameBuildings` — per-location data — and a ROM annotation reads
  *"area 0x13 -> .L1d04"*.

  **What that evidence is not.** The `MapEntrance`/`Events` names arrived with
  the upstream tree and are **not the maintainer's** — they are an earlier
  contributor's inference, so they corroborate nothing on their own. Earlier
  batches counted them as independent confirmation; that was wrong. The
  annotation corpus is documented in `docs/attribution.md` as getting mechanism
  right and purpose wrong often enough to matter. The genuinely independent
  support is the overlay-exclusivity result, and that alone.

  Oddities a correct account should explain: ids `0x00`–`0x0f` never appear,
  and the space is **70% dense over `0x10`–`0xbd`** — 52 unused values inside
  the range, with 11 of 23 overlays showing gaps and two having clusters 56
  apart.

  The names were adopted deliberately on that basis. If it turns out wrong the
  fix is a rename: the values are what matter to the link and they do not
  change. Confirming what `MapEntrance` means would settle it.

  *(Two earlier claims here have been corrected: that the ids came in PAIRS,
  and that they formed "dense, contiguous per-area runs" spanning ~190 values.
  Both were repeated before being checked.)*

- **Two clean-build bugs in the Makefile are fixed in batch 07**, both
  predating our work and both reachable from a fresh clone of your tree: three
  `orig.bin` dependencies naming files that have since been split, and
  `as -MD` recording a directory-less `.c` from a generated `.s`'s `.file`
  directive. Until these, every clean build here needed manual recovery.

- **A third clean-build bug in the Makefile is fixed in batch 31**, also
  predating our work and also reachable from a fresh clone: the dependency
  generator for the ELFs greps linker scripts for `.o` names, so the `.sym`
  files a script `INCLUDE`s are never dependencies. Editing `message.sym` left
  `stage1.o` stale and every overlay referencing the new symbol failed to link
  with `undefined reference`, which reads like a typo in the C. Silent until
  someone adds a symbol.

- **Twenty-three TUs are built with `-fno-rerun-cse-after-loop`**, covering thirty
  functions (first two in batch 25),
  and as of batch 42 one of them is MAIN-ROM code rather than an overlay, which
  weakens the reading that this is an overlay-only property. It
  this needs a decision from someone who knows the original toolchain. Both load
  a save-flag id twice around a call; at -O2 gcc-2.96 hoists it into a
  callee-saved register, spending a push, a pop and two moves to save one pool
  load, and one of them comes out LONGER than the ROM as a result.

  That flag is specifically the pass responsible — `-fno-gcse`,
  `-fno-cse-follow-jumps`, `-fno-cse-skip-blocks` and
  `-fno-expensive-optimizations` all leave the hoist in place.

  **The evidence is thin and is stated as such.** Sweeping all 85 parked files
  with the flag matched only the first two, so it is not a general lever for the
  constant-CSE class -- the nine added since were each found by recognising the
  shape on a fresh candidate, not by the sweep.

  **THE COUNT IS NOW THE ARGUMENT.** Batch 51 searched the whole corpus for the
  shape mechanically -- a pooled flag id loaded for two or more flag calls in
  one function -- and found **19 unelevated functions** carrying it; the worklist is now
  exhausted, 18 elevated and one already parked. Twenty per-file rules is no longer comfortably read as "the original
  build used this flag on these particular files". It reads more like
  **gcc-2.96 running a pass the original compiler did not**, in which case the
  right fix is a compiler-level one and all fourteen rules should eventually be
  dropped together. Flagged here because that is a maintainer's call, not ours,
  and because every batch that adds another rule makes it more pressing.

  **Batch 50 turned the recognition into a rule worth stating: a flag id READ
  IN A GUARD AND WRITTEN IN THE BODY is constant-CSE.** gcc hoists it into a
  callee-saved register across the call, spending a push and a pop to save one
  pool load, where the ROM simply loads it twice. That shape found three of the
  eleven directly. It may instead mean gcc-2.96 runs a pass the original
  compiler did not, in which case the right fix is a compiler difference and
  these two rules should be dropped. See `CSE_CFLAGS` in the Makefile.

- **Four functions are left as assembly that an inline-asm fakematch would
  match** (batch 32, the pool-load-first class). Your tree already contains one,
  `OvlFunc_883_2008fbc`, with that exact shape. We chose assembly over four more
  fakematches; if you would rather have the bytes, the park note lists every
  member and the change is mechanical.

- **Per-file flag rules written as `%` patterns can describe the wrong
  translation unit** (found in batch 45). The `_a`/`_b`/`_c` split chain is a
  POSITIONAL carve of one overlay's assembly, not a TU boundary, so a rule
  anchored on a name prefix can spread one TU's `-O1` choice onto a neighbour
  that merely shares that prefix. Two functions were compiled at the wrong `-O`
  this way, and the symptom was a clean four-line argument-fill diff
  indistinguishable from a real blocker.

  One rule is narrowed in batch 45. Two others in `rom_7ed0a0` still span more
  than one parent; **both build green today**, so they were left alone, but a
  future split under either stem would inherit `-O1` silently.
  `tools/tryc.py` now warns when a mismatch was screened under wildcard-sourced
  flags. Worth a decision from someone who knows which stems are really one TU.

- **Seven sources in your tree are compiled but never linked**, found by
  `tools/asmfacts.py --unlinked` (added batch 59). All seven arrived with the
  base commit, so this predates our work and is reported rather than changed:

      src/rom_b0000/dummy.c
      src/rom_c0/rom_447c_a_b.c
      src/rom_f9000/rom_f9ef8_b.c, _c_a_b.c, _c_a_c_b.c, _c_b.c, _c_c_b.c

  The `rom_f9ef8_*` ones look like an abandoned pass at the m4a engine: the
  linker script takes `src/lib/m4a/m4a.o(.text)` for that region instead, and
  the matching `.s` files are gone.

  **Why it is worth knowing rather than tidy-up trivia:** a `.c` whose `.o` no
  linker script references still compiles, still leaves `make compare` green,
  and still reports clean from `--orphans`. Anyone who elevates a function into
  one of these files gets a passing build and no effect on the ROM. That is
  exactly the failure this check was written for, after it happened once here.

- **SIXTY OF THE PARKED SET ARE WITHIN SIX INSTRUCTIONS OF MATCHING**, measured
  in batch 58 with `tools/near_parks.py`. That number is the most useful one
  here for deciding what to work on, because it separates two populations the
  word "parked" hides: a park at 2 of 24 is a compiler difference nobody has
  cracked, and a park at 30 of 34 is a function whose C is probably wrong. Only
  the second is worth re-reading from scratch.

  **Close does not mean reachable.** Many of the sixty sit on named, settled
  classes where the residual is a FLOOR rather than a gap -- the signed
  lower-bound canonicalisation (batch 55), the pre-header load merge, multiply
  operand canonicalisation.
  Each park note says which. Read it before spending a round.

  What the number DOES say is that the remaining difficulty is concentrated in a
  small number of compiler behaviours rather than spread across the corpus. If
  any one of those classes is ever cracked at the compiler level, it takes a
  large block of these with it.

- **Narrow constant materialisation** gates 34 functions and is half solved: a
  named `int` mask reproduces the ROM's 32-bit constant, but the instruction
  ordering resists seven attempts. `src/non_matching/overlays/narrow_constant.c`
  has the detail.

### `make clean` cannot be recovered inside Docker

`tools/agbcc/bin/{agbcc,agbcc_arm,old_agbcc}` are **Mach-O x86_64** binaries --
macOS host executables. The Linux container runs them as shell scripts and
fails with `Syntax error: "(" unexpected`. Five objects need them:
`src/lib/m4a/{m4a,m4a_tables}.o` and
`src/lib/agb_flash/{agb_flash,agb_flash_mx,agb_flash_at}.o`.

After any `make clean`, rebuild those five ON THE macOS HOST before returning to
the container. The exact commands are in
[reports/batch-61.md](reports/batch-61.md); the `m4a` pair must be run by hand
because macOS make picks the generic `%.o: %.c` rule for them and reaches for
`tools/gcc296/xgcc`, which exists only inside the container.

`make compare` is byte-exact against `baserom.gba`, so a passing compare after
this recovery is proof the host-built objects are correct -- nothing is taken on
trust. But the report gate's "clean `make clean && make compare`" is a
five-minute recovery, not a no-op. Do not run `make clean` casually mid-round.

### Argument precompute: DIAGNOSED, and it is a compiler difference

Eleven screened functions, every one within six instructions, are held by where
gcc materialises a cheap `mov rN, #imm` among the other argument registers. The
cause is now read out of the compiler source in the build image, not guessed:

    calls.c:805   precompute_register_parameters() copies any argument whose
                  rtx_cost > 2 into a pseudo BEFORE any hard register is loaded,
                  guarded by SMALL_REGISTER_CLASSES && reg_parm_seen.
                  reg_parm_seen is set for argument i before argument i is
                  tested, so it is 1 on the first register argument already.
    arm.h:1061    SMALL_REGISTER_CLASSES is TARGET_THUMB -- always 1 here.
    arm.c:2042    In Thumb, ASHIFT/PLUS/MINUS/NEG/NOT/COMPARE cost 4; pool
                  loads and synthesised constants also exceed 2.
    calls.c:1684  load_register_parameters() then loops FORWARD;
                  LOAD_ARGS_REVERSED is not defined anywhere in this tree.

Expensive arguments get hoisted ahead of the register loads; a cheap constant is
emitted afterwards, and lands last. The ROM's compiler did not precompute -- its
stream is plain forward load order with constant synthesis left in place.

**The predictive rule, tested:** a call misorders when its argument list mixes
cheap constants with TWO OR MORE expensive values and a cheap one is not last.
A call whose arguments are all cheap constants matches. Confirmed on
`OvlFunc_921_20099bc`, which has one call of each kind -- both predictions held
on the first screen. It also explains the matches: `OvlFunc_946_2009624` and
`OvlFunc_932_200aa10` pass only cheap constants.

**This is not fixable from C.** Eight source spellings and eight flags are
byte-identical to the default. These parks should not be retried as source
problems; they need a compiler change, and they sit with the
`-fno-rerun-cse-after-loop` count as the second concrete piece of evidence that
the reference toolchain differs from Camelot's.

**Correction, and it invalidates a test I reported earlier:** `-fno-schedule-insns`
was never a real experiment. arm.c:634 force-disables `flag_schedule_insns`
whenever TARGET_THUMB is set, silently, "since it's on by default in -O2". The
first scheduler NEVER RUNS for any file in this project. Only
`-fno-schedule-insns2` does anything. Any past note claiming sched1 was ruled
out by flag should be read as ruled out by construction.

Affected functions:
`OvlFunc_883_2008dc0/e54/e84/f5c/f8c`, `OvlFunc_884_200881c/20088ac` (2 of 16
each), `OvlFunc_930_2008870` (2 of 24), `OvlFunc_930_20088a8` (5 of 24),
`OvlFunc_909_2009958` (6 of 18), `OvlFunc_921_20099bc` (2 of 15).

Full derivation:
[src/non_matching/ovl_780898/2008dc0.c](src/non_matching/ovl_780898/2008dc0.c).

### Register-pressure residue: a category, not a set of one-offs

Six parks now reach the same conclusion independently, and it is worth stating
once rather than rediscovering per file. **What registers the ROM uses is a
consequence of pressure in the original translation unit, not of how the C is
written.** Three shapes recur:

- **An elided copy.** The ROM loads a value into one register and copies it
  before use; gcc loads straight into the destination and skips the copy.
  `Func_80bf54c` (4 of 19), `OvlFunc_969_200d9f0` (9 of 27).
- **A dead callee-saved register.** The ROM reserves a register, sets it, and
  never reads it. `OvlFunc_935_2008704` (6 of 24) -- six instructions of
  prologue/epilogue bookkeeping for a value with no consumer.
- **A constant hoisted or not hoisted out of a loop.** `Func_80a9d84`
  (14 of 30) -- gcc hoists three loop-invariant constants, the ROM hoists two
  and materialises the third inside the loop.

**The diagnostic that settles it:** find the function's near-twin that DOES
match. `Func_80a9cbc` matches and `Func_80a9d84`, identical but for a third
constant, does not -- so the third constant is the whole cause.

**CORRECTION (batch 64).** This entry previously also claimed that the
two-named-locals spelling produces the elided copy in `Func_80bf574` but not in
`Func_80bf54c`, and attributed the difference to pressure from a second store.
That was wrong and was never checked: screening `Func_80bf574` shows it emits
`ldrb r3` with no copy, exactly like `Func_80bf54c`. A third sibling,
`Func_80bf3bc`, has strictly MORE pressure -- a parameter held across a
three-argument call -- and elides it too.

So for THAT shape the copy is a plain codegen difference, not a pressure effect.
Pressure remains the right reading for the dead callee-saved register and the
loop-invariant hoist; it is not established for elided copies, and the
near-twin test is what distinguishes them. Run the test before invoking the
category.

**What not to spend rounds on:** the declaration lever, statement reordering,
extra named locals, and the derived-initialiser lever have all been tried across
these six and are byte-identical to the default in every case where the twin
comparison shows pressure is the cause. A copy cannot be requested from C when
nothing competes for the register.

**What might actually move them:** more of the surrounding TU being elevated, so
that the register pressure the original had is reproduced. These are the parks
most likely to fall out for free later rather than to a targeted fix, and they
should be re-screened after their file's neighbours are elevated -- not before.

### The pool tell is a 271-function blocker, and it is a naming problem

A constant below 0x100 fits in `mov #imm8` / `cmp #imm8`. When the ROM spends a
literal-pool word on one instead, the operand was a SYMBOL whose value happens
to be small. That is the pool tell, and it is already recorded as a lever -- what
was never measured is how much it blocks.

**271 unelevated functions load a constant below 0x100 from the literal pool.**
The commonest values are 0x0 (54 functions), 0x2 (21), 0x1f (19), 0x1 (15) and
0x75 (13).

The `=0` cases are the least ambiguous: `mov rN, #0` is always available, so a
PC-relative load of zero is never a compiler choice. These are disassembled ROM
bytes, not our output -- the disassembler prints `ldr r3, =0` because the
encoded instruction really is a PC-relative load, so the tell is genuine rather
than an artifact of how the `.s` was written.

**Caveat on the number:** it counts functions containing at least one such load,
not functions blocked *only* by this. Some will have other blockers too, and the
count mixes Thumb and ARM-mode common code. Treat it as an upper bound on what
naming would unlock, not a promise.

**Why it matters for planning:** this is the largest single identified blocker
in the corpus, larger than argument precompute (11), and unlike that one it is
FIXABLE -- but not by us. Naming is exactly what this effort has deliberately
deferred, so these functions are a maintainer's call. Two worked examples with
the evidence written out:
[src/non_matching/ovl_7d768c/2008070.c](src/non_matching/ovl_7d768c/2008070.c)
(pooled 0x8b) and its sibling `OvlFunc_963_200808c` (pooled 0xaa and 0xa9).

### Dead end: load-then-copy is NOT a blocker signature

`ldrb rA, [..]` immediately followed by `mov rB, rA` is the shape behind the
four-function `Func_80bf*` family, where gcc loads straight into the destination
and drops the copy. It looked like it might be a large blocker class, and the
raw numbers encouraged that:

    unelevated   508 of 2702  (18.8%)
    elevated      72 of 2915  ( 2.5%)

**That 7.5x enrichment is a SIZE ARTIFACT.** Elevated functions are small,
unelevated ones are large, and a longer function has more chances to contain any
given pattern. Controlling for length removes it:

    size band     elevated      unelevated
    12-25         19/980  (2%)  10/149  (7%)
    26-40         29/485  (6%)  28/351  (8%)
    41-70          4/94   (4%)  67/666 (10%)
    71-200         2/45   (4%) 191/988 (19%)

In the 26-40 band -- where both populations are well represented -- the rates are
6% and 8%. gcc emits load-then-copy routinely in functions that match byte for
byte, so its presence says nothing about whether a function is reachable.

**Do not build a candidate filter on this.** The `Func_80bf*` family is blocked
by a specific instance of the pattern, not by the pattern itself. The only two
measured, real blocker counts remain argument precompute (11 functions,
mechanism traced to compiler source) and the pool tell (271, upper bound).

### What the park corpus actually looks like, measured

A claim was made in batch 64 that the remaining small-function pool is
"dominated by cases where gcc's output is shorter than the ROM's". **That was an
overstatement.** Measured across the 53 park files that record a length
comparison:

    ours SHORTER than ROM    21  (40%)
    same length              28  (53%)
    ours LONGER               4  ( 8%)

**The majority are same-length**, which means register allocation and
instruction ordering -- differences the levers are built for and which have been
cracked repeatedly. The shorter-than-ROM group is real and substantial at 40%,
and it is the genuinely hard one: where the optimiser proved something (a value
the guard pinned, a provably-dead instruction, a mask the store makes
redundant), no source spelling puts the longer form back.

**Caveat on the denominator:** only 53 of 159 park files record a length
comparison at all. The newer park format includes it; older notes do not. The
proportions are from the files that have it, and newer parks are biased toward
the near-misses that got the most attention, so the shorter-than-ROM share is
probably an over-estimate for the corpus as a whole.

**What follows for planning:** the same-length majority is still worth working,
and the biggest shortfalls are listed below so they are not re-attempted as
spelling problems:

    -6  src/non_matching/ovl_7bf5a8/2008704.c   dead callee-saved register
    -5  src/non_matching/rom_b5000/80c23c0.c    branchless bit extract
    -3  src/non_matching/rom_c0/8006384.c       register-register AND
    -3  src/non_matching/rom_b5000/80c2410.c    provably dead mov

### Retired: "dma.h register binding" was not a blocker class

Batches 54-55 named `include/dma.h` register binding as a blocker and five parks
were filed under it. Re-screening all five in batch 65 shows the label was
wrong, and no park is actually held by it:

- **`Func_80170c4`** matches the ROM's `stmia r3!, {r0, r1, r2} / sub r3, #0xc`
  EXACTLY through `DMA3_SET(&buf, d, cnt)`, with the halfword staged at `sp+2`
  by a plain local. Its residue is an unrelated copy elided at a shared exit.
- **`Func_80a22f4`** has the binding as ONE of two defects, and its own note
  records a helper variant (`"+l" (_dst)`) that removes it -- the spill goes
  away and the length drops from 13 to 12. What remains is gcc strength-reducing
  the second transfer's constants off the first, which is about constants and
  has nothing to do with registers.
- **`OvlFunc_914_2008c0c`** needs gcc's partial tail merge; the count of base
  loads is the count of calls, which no helper shape reaches.
- `Func_80198dc`, `Func_80bd7a4` are held by other classes.

**Practical consequence:** `include/dma.h` works. Reach for `DMA3_SET` /
`DMA3_COPY16` when a function does a DMA transfer, and do not treat the header
as a known-lost cause. If a DMA function does not match, the reason will be
somewhere else, and the park note should name that reason rather than the
header.
