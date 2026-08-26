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
| [batch-64](reports/batch-64.md) | 8 | ready to port — 4 are one function in four overlays |
| [batch-65](reports/batch-65.md) | 8 | ready to port — 7 first-screen, via existing .sym symbols |
| [batch-66](reports/batch-66.md) | 5 | ready to port — all via existing .sym symbols |
| [batch-67](reports/batch-67.md) | 5 | ready to port — 8 new area ids added on stated evidence |
| [batch-68](reports/batch-68.md) | 7 | ready to port — 5 new message ids; 1 park unparked in the same batch |
| [batch-69](reports/batch-69.md) | 7 | ready to port — 6 are one function in six places, via a new -fno-strict-aliasing flag group |
| [batch-70](reports/batch-70.md) | 8 | ready to port — 5 are one bitfield store in five places; 1 unpark |
| [batch-71](reports/batch-71.md) | 7 | ready to port — solves the 34-function narrow-constant blocker; 16 stale parks deleted |
| [batch-72](reports/batch-72.md) | 5 | ready to port — the struct-member read lever; a switch reproduces a ROM jump table |
| [batch-73](reports/batch-73.md) | 6 | ready to port — register class measured and narrowed by a compiler experiment |
| [batch-74](reports/batch-74.md) | 5 | ready to port — new -fno-gcse group; three size symbols; a blob that is bigger than its function |
| [batch-75](reports/batch-75.md) | 5 | ready to port — do-while wrappers are load-bearing; tryc's pool warning hole closed |
| [batch-76](reports/batch-76.md) | 5 | ready to port — address vs value; the three-operand lever and its limit |
| [batch-77](reports/batch-77.md) | 5 | ready to port — 2 solves with free twins; one short member read two ways |
| [batch-78](reports/batch-78.md) | 6 | ready to port — 3 solves, 3 free twins; a split that dropped its .data |
| [batch-79](reports/batch-79.md) | 6 | ready to port — `ldrh rD, .L` IS `ldr rD, =v`; pool ORDER reads operand modes |
| [batch-80](reports/batch-80.md) | 6 | ready to port — the pool_range table from arm.md; naming a file-local data label |
| [batch-81](reports/batch-81.md) | 6 | ready to port — tryc called clean matches failures; the 13-member family at 144/144 |
| [batch-82](reports/batch-82.md) | 5 | ready to port — asm labels beat renaming; a named local can COST the preferred register |
| [batch-83](reports/batch-83.md) | 6 | ready to port — which operand becomes the `orr` destination; const.sym |
| [batch-84](reports/batch-84.md) | 5 | ready to port — read the constant's WIDTH off the ROM; split_asm could not see .lcomm |

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

**36 functions are NOT compiler output and are excluded from candidate lists.**
`asm/rom_f9000/rom_f95e0.s` and `asm/rom_f9ef8_a.s` are the MP2K sound driver,
hand-written assembly: `mov r12, lr` / `bx r12` return conventions, an `adr`/`bx`
switch into ARM mode to reach `umull`, and a `bl` to a label inside another
function. `tools/not_c.py` holds the list and the evidence;
`asmfacts.functions()` filters them out by default, so every tool built on it
skips them. They are byte-exact today and should be counted as DONE, not as
remaining work. Corroboration is already in this tree: `src/lib/m4a/m4a0.s` is a
hand-written assembly copy of the same driver, carried in from Coaltergeist's
tree and never built -- upstream ships this driver as assembly.

**Do not clear the smallest size band first.** See
`src/non_matching/tiny_reg_order.c`. Below twenty instructions there is no
structure left for C to express and everything remaining is register birth order
and address-load placement -- the residue that parks larger functions, except it
is the whole function. The 21-40 band is where the hit rate is.


**26 parked files are PARTLY STALE** — they name a group of functions, some of
which have since been elevated, and their notes now describe a mix of solved and
unsolved work. `python3 tools/stale_parks.py` lists them. They need editing, not
deleting, and that has not been done. Sixteen FULLY stale files were deleted in
batch 71; the park count went 180 → 164. Treat any park census taken before that
as counting solved problems.


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

### The pool tell: 209 functions need NO naming at all -- correcting batch 63

Batch 63 measured the pool tell at 271 unelevated functions and concluded they
were "blocked on naming... a maintainer's call". **The second half was wrong.**

`area.sym`, `message.sym`, `file_table.sym` and `wram.sym` already define **611
symbols**. Cross-referencing them against the pool-tell sites:

    unelevated functions with the pool tell    270
      EVERY value already has a symbol         209   <- elevatable NOW
      some values have symbols                  13
      no values have symbols                    48   <- genuinely need naming

So 209 functions are not blocked on a decision by anyone. They are blocked on
nobody having looked in `area.sym`. Two were elevated the day this was found:
`OvlFunc_952_2008070` (parked at 7 of 32, exact once `_AREA_8b` was used) and
`OvlFunc_926_200a574`.

**THE TECHNIQUE**, already established in this tree and documented at the top of
`area.sym`:

    extern int _AREA_3c;                        /* declare  */
    if (v == (int)(&_AREA_3c)) ...              /* compare against its ADDRESS */

gcc-2.96 always pools a symbol's address and never pools a constant it can build
with an eight-bit `mov`, which is exactly the asymmetry the pool tell describes.
An absolute symbol definition emits no bytes, so the link is byte-identical and
`make compare` proves it.

**`OvlFunc_926_200a574` is the clean demonstration**: it compares against
`_AREA_3c` (pooled) AND against a literal `3` (`cmp r3, #0x3`) in the same
function, so the tell distinguishes symbols from literals rather than being an
artifact of the disassembly.

**What the remaining 48 need** is genuine naming, and `area.sym`'s own header is
the model: it records why the namespace is called an area id, what the evidence
is, and explicitly what the evidence is NOT. The values still unnamed are
`00 02 05 0c 15 18 19 1a 1b 1f 28 2d 32 3a 3b 3d 69 7e 80 86 8d 8f 90 92 a9 b6
bb f0` -- and note that several of those (0x00 appearing in 54 functions, 0x02 in
21) are almost certainly NOT area ids, since the area space is documented as
never using 0x00-0x0f. They belong to some other namespace that has not been
identified yet.
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
measured, real blocker counts remain argument precompute (12 functions,
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

### The remaining corpus is duplicated, but mostly by SHAPE not by bytes

`tools/twin_finder.py` reports clusters of UNELEVATED twins. **The first version
of this entry (batch 65) reported 102 functions in 12 clusters and implied one
solution would port to all of them. That was wrong, and the correction matters
more than the original claim.**

A signature match is the same OPCODE sequence; operands may differ. Measured
across the 118 shape-matched functions in clusters of four or more:

    EXACT, operand-identical -- one .c ports verbatim      15 functions, 3 shapes
    SHAPE only, constants differ                          103 functions

    18 x 172 insn   shape only, no two identical
    17 x 139        shape only
    17 x 132        shape only
     7 x 220        shape only
     7 x  97        shape only
     7 x  27        EXACT -- all seven identical
     6 x  16        shape only
     5 x  46        shape only, 4 of the 5 identical
     5 x  39        shape only, 4 of the 5 identical

So the free-work figure is **15, not 102**. Reporting the shape count alone
overstates it by nearly 8x. The tool now prints both and labels each cluster.

**Shape-only clusters are still worth real money**, just not for free: one `.c`
ports with the constants substituted. That is exactly how `OvlFunc_916_200836c`
was elevated from `OvlFunc_947_2009578` in batch 63 -- same code, different
VCOUNT bound and tables, three constants changed, matched on the first screen.
A 172-instruction shape with 18 members is still the largest single lever in the
corpus if its shape can be cracked once.

**The one EXACT cluster is the highest-value park.** 7 x 27 instructions, a
shared routine in the main ROM and six overlays, blocked by a SINGLE hoisted
load. It was parked twice independently before the duplication was noticed
(`rom_8a000/rom_9a44c.c` and `ovl_7ced6c/2008ab0.c`, now cross-linked).
`-fno-schedule-insns2` fixes that exact load and breaks four earlier pairs
instead.

The 6 x 16 shape is the `OvlFunc_883/884` family, blocked by argument precompute
(`calls.c:805`) -- a compiler difference, not fixable from C.

### Where the blocked work is concentrated, by function count

Three blockers now account for more blocked functions than everything else
combined, because each holds a whole duplicated cluster:

    4 functions   one register naming (was 8; the 4 x 39 cluster is now
                  ELEVATED -- basic-block placement was solved by inverting the
                  guard, see docs/elevation.md)
                  src/non_matching/ovl_7ced6c/2008a4c.c
    7 functions   one hoisted load, scheduler-related
                  src/non_matching/rom_8a000/rom_9a44c.c (highest value: the
                  seven are operand-identical, so one fix ports verbatim)
    12 functions  argument precompute, calls.c:805 -- a compiler difference,
                  NOT fixable from C

So 11 functions sit behind two behaviours that are still open, and 12 behind one
that is closed. That is a better guide to where a round is worth spending than
the raw park count of 158.

**All three clusters were found by tools/twin_finder.py or by the cluster census
in this file, not by reading the candidate list.** Duplicate-aware selection is
what makes a single screen worth four to seven functions; per-function selection
had been picking singletons for many rounds.

### Naming a symbol is sometimes the elevation -- eight added on stated evidence

Batch 67 added eight ids to `area.sym`: `_AREA_7e 86 8d 8f 90 92 a9 b6`. Each is
**compared against the halfword at gState+0x1C0**, which is area.sym's own
stated criterion for the namespace. Eleven functions were blocked on them.

**Each was previously defined only in `file_table.sym`.** That is not a
contradiction: a file id and an area id may share a number, and 95 small values
already collide across the four `.sym` files. The CONSUMER distinguishes them,
which is why `tools/sym_candidates.py` refuses to classify from the value.

**What the addition is and is not.** It asserts the namespace, not the meaning --
the same "named by value, pending semantic names" convention `message.sym`
already uses. It does not claim to know which place `0x8f` is.

**Weak corroboration, stated as weak:** all eight fall inside 0x10-0xbd, so they
shrink the gap the area.sym header records as an oddity. The gaps shrink; they
do not close.

**An unresolved discrepancy, recorded rather than smoothed over.** The area.sym
header states "52 unused values inside the range". A direct count of the
definitions gave 57 before this change and 49 after. The two may be counting
different things -- defined ids versus ids some function actually compares -- or
one may be stale. Reconcile before citing either as evidence.

### `__SetDestMap` is a second area-id consumer -- resolved, not assumed

Batch 68 recorded the `__SetDestMap` lead as INCONCLUSIVE, naming the check that
would settle it: match each pooled value's destination register against the
callee's signature. Running that check settles it three ways:

- The signature is known from elevated code:
  `extern void __SetDestMap(int map, int entrance);`
- At **all 16 call sites** the pooled value goes into **r0** -- the `map`
  argument -- with r1 set separately as the entrance.
- An elevated file **already** writes `__SetDestMap((int)(&_AREA_01), 1)`. The
  convention predates the investigation.

Seven ids added on that evidence: `_AREA_00 02 04 2d 3a bb be`. `area.sym` now
defines 133.

**A header claim was wrong and is corrected.** It said *"ids 0x00-0x0f never
appear"* — flatly contradicted by `_AREA_01`, which the file itself defines. The
accurate form is that they never appear **in comparisons**; they do appear as
`__SetDestMap` destinations. The comparison census still shows nothing below
0x7e among the ids it was missing, so the underlying observation survives; only
its scope was overstated.

**The consumer rule now has two branches for area:**

    compared against gState+0x1C0     -> area id  (strong)
    passed as arg 0 to __SetDestMap   -> area id  (strong)
    used in arithmetic with the above -> area id  (weaker, read it)

**Caveat on immediate value:** every `__SetDestMap` caller is 69+ instructions
and they are cutscene functions, so these seven symbols unblock nothing on their
own today. They remove a blocker that would otherwise be hit later.

### What the 177 parks are blocked on, roughly

A keyword pass over the park notes, first-match-wins:

     37  register allocation      (naming, elided copies, elided saves)
     35  scheduling               (hoists, load ordering)
     19  constant-CSE
     12  optimiser proved something and removed it
      7  argument precompute
      4  pool tell / naming
      4  basic-block placement
      3  cross-jumping / CSE of two loads
     56  unclassified

**Read the numbers with two caveats.** The classifier takes the FIRST class
whose keywords appear, so a park describing two causes is counted once -- the
argument-precompute figure reads 7 here against a hand count of 12, because
several of those notes mention register allocation earlier in the text. And the
56 unclassified are mostly older parks written before these class names
settled; they are not a separate phenomenon.

**What survives the caveats** is the shape: register allocation and scheduling
together are the largest group by a wide margin, and both are compiler-policy
differences rather than things C can state. The recurring detail is that the
ROM's allocator reaches for callee-saved registers sooner than gcc-2.96 does --
several parks show the ROM using r4-r6 where gcc uses r0-r3, and a few show the
ROM using r4 without saving it at all.

**Planning consequence:** the small-function frontier is now mostly these two
classes. Further progress at this size is likely to need a compiler-level
answer rather than more source spellings, which is the same conclusion the
argument-precompute investigation reached from a different direction.

### A named mechanism for the register-allocation parks: `REG_ALLOC_ORDER`

37 parks are blocked on register allocation, and the recurring detail is that
the ROM reaches for r4-r6 where gcc-2.96 uses r0-r3. That has a named cause in
the compiler source.

`config/arm/arm.h:989` defines the allocation order, with its own rationale in
the comment above it:

    /* ... Allocate r0 through r3 in reverse order since r3 is
       least likely to contain a function parameter; in addition results are
       returned in r0.  */
    #define REG_ALLOC_ORDER  { 3, 2, 1, 0, 12, 14, 4, 5, 6, 7, 8, 10, ... }

So gcc tries **r3, r2, r1, r0 first** -- caller-saved before callee-saved -- then
r12 and r14, then r4 onward. **There is no Thumb-specific override**; the same
order applies, with the high registers simply unusable.

**What this explains.** In `Func_800fa8c` the ROM keeps four loop values in
r4/r5/r6 and saves three of them; gcc puts two in r0/r1 and two in r4/r5. With
no call in the loop, gcc's choice is cheaper and legitimate -- it is not a bug,
it is a different starting point in the same list.

**What it does NOT establish.** That the original toolchain had a different
`REG_ALLOC_ORDER` is the most economical explanation, not a proven one.
Different register pressure, a different pass order, or a different
`CALL_USED_REGISTERS` would produce similar symptoms. Nothing here was tested by
rebuilding a compiler.

**The testable next step**, for anyone who can rebuild gcc-2.96: change
`REG_ALLOC_ORDER` to start at 4 and re-screen the register-allocation parks. If
a meaningful fraction resolve, that is the answer for the largest blocked class
in the corpus. If none do, the class needs a different explanation and this
entry should be struck.
