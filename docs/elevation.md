# Elevating assembly to C

How functions get converted here, and the compiler behaviours that stop them.

> ## THE COMPILER IS gcc-2.96. IT IS NOT agbcc OR old_agbcc.
>
> Every function this document is about is built by patched **gcc-2.96**
> (`/opt/gcc296/xgcc`). `agbcc` and `old_agbcc` compile **five objects** in the
> entire tree -- `src/lib/m4a/*` and `src/lib/agb_flash/*`, which are prebuilt
> Nintendo library code, not Camelot's -- and nothing else.
>
> **Never explain an elevation blocker in terms of agbcc or old_agbcc.** If a
> passage below does, it is stale and wrong, and the conclusion drawn from it is
> unsafe. This is not hypothetical: the "branch-over-pool is a ceiling" claim
> was built on old_agbcc's pool behaviour, sat in this file for dozens of
> batches, and in batch 153 caused a function to be written off unscreened. That
> class is several hundred functions and the ceiling was never there.
>
> Where the two compilers genuinely differ is a fact about **which five files**
> get built by which compiler, never about how a decompiled function must be
> written.

## The loop

1. **Pick by shape, not size.** `tools/elevation_candidates.py` ranks the
   remaining hand-written assembly. Small is *not* the same as tractable -- the
   first batch attempted here took the five smallest flag accessors in
   `rom_79338_c_a.s` and matched none of them, because that family is
   scheduling-sensitive and this tree already has its neighbour `ToggleFlag` in
   `fakematch.txt` behind inline-asm barriers.

   The ranker skips ARM functions outright. The ROM is built `-mthumb`, so an
   `.arm_func_start` body cannot have come from C at all.

2. **Read the function.** `tools/showfunc.py <name>` prints it wherever it
   lives, with its annotation, so a 30-instruction target does not require
   opening a 4,000-line file.

3. **Write the C and screen it.** `tools/tryc.py` compiles a candidate and
   diffs the instruction stream against the ROM's assembly:

       docker run --rm -v "$PWD:/work" -w /work goldensun-build \
           python3 tools/tryc.py scratch/candidate.c --ref asm/<...>.s

   `--ref` is what makes it possible to prove a function *before* wiring it
   into the build. Most targets sit inside a multi-function `.s` that has to be
   split and relinked first, and that is a lot of churn to spend on a candidate
   that then fails.

4. **Split, if needed.** A `.o` comes from one source file, so a function
   inside a multi-function `.s` needs `tools/split_s.py <file.s> <FuncName>`,
   which produces the `_a`/`_b`/`_c` shape the tree already uses and rewrites
   the linker script.

   **Verify `make compare` is green after the split and before writing any C.**
   The split is byte-neutral by construction, so if it is not green, the layout
   is wrong -- and a layout mistake looks exactly like a bad decompilation if
   both land at once.

5. **Build.** `tools/tryc.py` is a screen, not a verdict. `make compare` is the
   only authority.

## Working discipline

Three rules learned by breaking them.

**`make -j8 && make compare` does not gate a commit that REMOVES source.**
It answers "does this build" using whatever is on disk, and make treats an
existing `.o` with no rule as up to date -- so a stale object from the previous
build satisfies the link and the commit passes. `make clean` then removes it and
the next build dies with `No rule to make target`.

Batch 19 committed three overlays in that state at once. Before committing
anything that deletes a `.s`, run:

    python3 tools/asmfacts.py --orphans

which answers "is the tree consistent" in a second instead of a five-minute
rebuild. And derive a cleanup list from what was actually WRITTEN, not from what
was attempted -- that batch's loop deleted five `.s` files for three `.c` files,
one of them belonging to a function that had never been elevated.

**The build result must gate the commit, not run beside it.** Running
`make` and then `git commit` as separate steps commits whatever happened,
including a broken link. Chain them, or check the exit code:

    docker run ... sh -c 'make AGBCC_DIR=/opt/agbcc -j8 && make AGBCC_DIR=/opt/agbcc compare' \
        && git commit ...

**Always build in the container. Never run bare `make` on the host.** On macOS
`/usr/bin/make` is GNU make 3.81 (2006) and `sed` is BSD sed, and the two of
them produce failures that look exactly like a corrupted tree:

  * `sed -E -i 's,...,'` -- BSD sed reads `-i`'s argument as a *backup suffix*,
    so the script is eaten and the FILENAME becomes the script. Every `.d`
    under `asm/` then parses as sed's `a` command:
    `sed: 1: "asm/overlays/...": command a expects \ followed by text`.
  * `-T $<` in the `$(ELFS)` recipe -- make 3.81 orders the `elf_deps`
    prerequisites *before* the static-pattern `%.ld`, so `$<` is the first
    object in the linker script and ld tries to parse an ELF as a script:
    `ignoring invalid character '\000' in script / syntax error`.

Both are host-only. `gmake` (Homebrew, make 4.x) fixes the second and not the
first, and the host has no working `tools/pack_overlay` anyway -- those binaries
are Linux x86-64. A whole round went into "fixing" the Makefile for these before
noticing that the documented Docker build was green the entire time. The
Makefile is correct; the invocation was not. `build.sh` uses `gmake`; the commit
gate above uses Docker. Use one of those two.

**Changing a compiler flag requires `make clean`.** make tracks file timestamps,
not command lines, so after editing `GCC296_CFLAGS` or adding a per-file rule the
existing `.o` files are still used and `make compare` reports failures that have
nothing to do with the current tree. A global `-fno-rerun-cse-after-loop`
experiment left three overlays failing for two builds after it was reverted, and
they looked like a regression in unrelated work. `tools/asmfacts.py --orphans`
does not catch this — it checks that sources exist, not that objects are current.

**Write commit messages through a quoted heredoc**, never through
`python3 -c "..."` or any double-quoted shell string. Backticks and `$` in a
double-quoted string are substituted by the shell, which silently eats
fragments of the message -- it has mangled three commits here:

    cat > /tmp/m.txt <<'MSGEOF'
    ...message...
    MSGEOF
    git commit -F /tmp/m.txt

**`git checkout` is denied by this project's permission rules** (the pattern
that blocks branch switching also matches file restores). Use `git restore`.


### The generated `.s` beside the `.c` IS tracked — commit it

Converting a function deletes its hand-written `.s`; the next build writes a
**generated** one to the same path from the new `.c`, so git reports the file as
*modified* rather than deleted. That reads like compiler output leaking into the
corpus, and it is not.

`.gitignore` covers `.o`, `.d`, `.elf`, `.map` — and deliberately **not** `.s`.
The tree carries **2,535 tracked `.s` files bearing gcc's own banner**, all
arriving with the upstream base commit. Tracking the generated assembly beside
the C is the convention, and 391 of 391 elevations here follow it.

So: **stage the modified `.s` along with the new `.c`.** Do not `git rm --cached`
it. Verify with

    python3 tools/asmfacts.py --asm-pairs

which fails if any elevated `.c` lacks a tracked sibling `.s`. This was got
backwards once — three files were untracked on the theory that generated
assembly should never be committed — and the check exists so the question is
settled by the repository rather than by intuition.

## The compiler is NOT deterministic. Rebuild before you investigate.

**gcc-2.96's optimiser depends on the process's address layout.** Same file,
same command, same container:

| condition | runs | result |
|---|---|---|
| as-is | 120 | 114 correct, **6 divergent** |
| `setarch -R` (ASLR off) | 120 | 120 correct |

The divergence is a CSE decision — whether gcc reuses a register that happens
to already hold a pooled constant — which is exactly what a hash table keyed on
pointer values would decide differently under a different heap layout. About 5%
per compile on the file it was measured on.

This corrupts clean builds, and it does so in the most misleading way possible:
the symptom is two wrong bytes and a `.c` that appears not to reproduce its
committed `.s`, which is indistinguishable from a fake match. In batch 151 it
cost most of a round — four optimiser flags and three source rewrites probed
against a file that was already correct.

**So, in order:**

1. On any `make compare` failure, **rebuild the object and re-diff before
   investigating anything**. If the second compile agrees with the committed
   `.s`, it was the compiler, not the source.
2. **Run `git status` after every build.** Every generated `.s` is tracked, so a
   divergent compile appears as a modified file naming the exact object. It is
   free and it is the only cheap way to catch this.
3. For release-grade verification, `setarch -R` makes the build fully
   deterministic. It needs `docker run --privileged`; unprivileged it fails with
   `Operation not permitted`. It is deliberately not wired into the Makefile,
   because that would make the standard build command require `--privileged`.
4. **Treat old single-diff conclusions as provisional.** Any function parked or
   reverted on the strength of one clean-build diff may have been this.

## Two levers from the batch-153 dispatcher family

**PUT THE CALL IN EVERY ARM and let gcc cross-jump it.** When a ROM's arms set
only the differing argument registers and branch to a shared tail holding the
remaining `mov`s and the `bl`, that is NOT a single call after a join. Writing
it as one call with the argument assigned per arm is nine instructions short,
because gcc then materialises the *common* arguments once instead of per arm.
Write the call out in all five or six arms; gcc merges the identical tails and
keeps the differing setup where the ROM has it. This took
`OvlFunc_946_200add0` from 66 differing of 80 to 13, and two siblings written
that way matched on the first screen.

Note this is the same mechanism as the shared-call-tail parks already recorded,
seen from the other side: those are gcc failing to cross-jump where the ROM
did. Either way the source shape to try is the same — put the call in the arms
and let the compiler decide.

**THE COIN FLIP ONLY APPEARS WHEN THE ALLOCATOR HAS SLACK.** Four functions in
one chunk share a source shape. The three that read THREE actor values and
spend three callee-saved registers match byte for byte. The one that reads TWO
and spends two is a pure r5/r6 rename, 13 differing lines and nothing else.

With three live values gcc and the original compiler agree; with two, gcc gives
the higher-priority pseudo the FIRST available callee-saved register and the
ROM gives it the second. That is sharper than "register allocation is
unreachable": **the disagreement shows up in the slack and disappears under
pressure.** Before parking a two-register rename, check whether a sibling with
more live values matches from the same source — if it does, the shape is proven
and the rename is the flip, not a missing lever.

Also tested and negative on that function: the batch-152 birth-order lever does
NOT apply. Swapping the two computations changes which value is computed first
but leaves the variable in the same register, so the `ldr` offsets swap instead
and the count stays at 13. Birth order moves a POINTER's materialisation, not a
value's allocation priority.

## The interleave lever CONFIRMED, with its source signature

Following the survey below, the shape was traced back to source in matched
code. It is the basic-block lever, and this is what it looks like when it works:

    a1 = 0x81 << 1;                    /* named local, assigned early */
    ...
    if (...) {                         /* a branch between */
        __MapActor_Surprise(0x15, a1); /* used at a guarded call site */

produces exactly `mov r1, #129 / mov r0, #21 / lsl r1, r1, #1` -- the other
argument's `mov` scheduled into the gap. From
src/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a_c_b.c.

**The signature to look for, before spending a round:** a two-instruction
constant, assigned to a NAMED LOCAL, in a block that DOMINATES a call site
which lies behind a branch. All three parts are load-bearing. 117 matched
functions carry the shape; in 71 of them the shifted constant is in the HIGHER
register, i.e. a later argument, so argument position is NOT a constraint --
that hypothesis was tested and refuted (46 lower, 71 higher).

**WHERE IT DOES NOT REACH, both measured:**

  - A STRAIGHT-LINE FUNCTION has no dominating block, so there is nothing to
    hoist into. `OvlFunc_945_200dca4` (2 of 43) and `OvlFunc_945_200bdec`
    (2 of 26) are both seven-to-eleven calls in sequence with no condition.
    Confirmed again this round: naming both argument values at the call site
    changes nothing.
  - A SWITCH ARM WHOSE ARMS NEED DIFFERENT CONSTANTS. On
    `OvlFunc_911_200a7ac` the switch is a real branch, so the boundary above
    does not apply -- but hoisting BOTH arms' constants above it and selecting
    one per arm leaves the count unchanged at 7. The lever wants one constant
    dominating one use, not a choice of constants.

## The interleaved constant build is ROUTINE, not a ceiling

Several parks describe an "arg-interleave wall": the ROM builds a
two-instruction constant (`mov`/`lsl`) with another instruction scheduled into
the gap, and our output emits it contiguously. Some of those notes conclude the
shape is unreachable.

**It is not.** Surveyed across every generated `.s` in the tree:

| shape | count in MATCHING code |
|---|---|
| contiguous `mov rX,#imm` / `lsl rX` | 1480 |
| **interleaved** (something between them) | **1003** |
| ...of those, in ordinary C (no `register`-pinning, no dma.h) | **851** |

So gcc-2.96 produces the interleave routinely and in plain source. Any park
that says otherwise is overstating; the honest form of the claim is "no spelling
I tried moved it here".

**A WARNING ABOUT SCANNING FOR THIS.** gcc emits the THREE-operand form,
`lsl r0, r0, #19`, while hand-written ROM `.s` uses the two-operand
`lsl r0, #19`. They assemble identically and tryc normalises them. A regex
written against the two-operand form finds ZERO hits in generated code and
looks like proof of a ceiling. That is exactly the mistake this survey nearly
made -- match `lsl rX(, rX)?, #`.

**ONE MECHANISM IS IDENTIFIED, and it is not general.** In
`src/overlays/rom_78dd40/ovl_30_c_c_b.c` three named locals are each assigned
the SAME two-instruction constant; gcc batches the three `mov`s together and
then the `lsl`s, which produces the interleave as a side effect. Measured
NEGATIVE: naming both argument values as locals at a site where the two
constants DIFFER (`OvlFunc_945_200dca4`) changes nothing -- still 2 differing.
So "name both" is not the lever; batching identical constant builds is one
route to the shape, and the general one is still open.

## A local that only holds an ADDRESS can cost the ordering -- delete it

Twice now, in consecutive batches, a function has come down to a handful of
instruction-ordering differences that no respelling could move, and the fix was
to REMOVE a pointer local rather than to write it differently.

    ours   unsigned short *p = &a->f64;  ...  *p |= 2;  ...  *p &= 1;
    exact  a->f64 |= 2;  ...  a->f64 &= 1;

On `OvlFunc_898_20087ec` the pointer form is 44 lines against 44 with SIX
differing, and all six are one shape appearing twice: gcc hoists the `ldrh`
above the `mov` that saves an earlier field, where the ROM defers it. Dropping
the local is exact. `Func_80c1084` in batch 155 is the same story -- its park
had tried the right offset-clobber form while KEEPING the extra local, which is
why it never fired.

**The tell is that respellings do not move the count.** Four were measured on
20087ec -- the constant on the left of the operator, a narrow named constant,
an `int` named constant, and widening the saved field -- and every one stayed at
exactly six. When several unrelated spellings all give the identical count, the
variable's EXISTENCE is the problem, not its form.

Note this is the opposite of the stack-argument rules, where naming things is
the lever. Those locals carry VALUES that must occupy registers across a call;
this one carries an address gcc can recompute at will, so it buys nothing and
constrains the schedule. Ask which kind you have before adding or removing one.

## The ldrh/ldrsh CSE class is 46 functions, and only ~12 are actually blocked

The shape: the ROM reads the same halfword twice, once unsigned and once
signed, and gcc emits only the `ldrsh` and derives the other value from it.
Swept and measured -- **46 functions, 70 sites**. A looser base-register-only
scan reports 85, but the extra 39 read different offsets off a shared base and
are not this shape.

**IT IS NOT UNIFORMLY A WALL, and that is the point.** Two already-parked
members reproduce the pair perfectly. The discriminator is what the UNSIGNED
value feeds:

| the unsigned value | result |
|---|---|
| feeds a use needing true 32-bit zero-extension | gcc MUST keep both loads -- reproducible |
| used only at 16-bit width, or later sign-extended by `lsl #16 / asr #16` | the two loads are provably the same value, CSE is correct -- blocked |

About **12 sites** fall in the blocked sub-class. The other **~58 are ordinary
candidates** that merely contain the shape and were never attempted. Do not let
the label stop you screening them.

**The blocked sub-class is a clean negative -- stop spending rounds on it.**
Eight spellings were measured on the smallest instance: distinct pointer
objects, reversed source order, an int-typed intermediate, a separate
`__asm__`-aliased symbol, plain casts. All are the same MEM in RTL and all CSE
identically. Only `volatile` keeps both loads, and that is a fakematch.

The mechanism is confirmed positively rather than merely observed: give the
unsigned value a use the sign-extended value cannot satisfy -- a shift count --
and gcc keeps both loads immediately. Where the ROM's `ldrh` result is *only*
ever `lsl #16 / asr #16`, it literally IS the `ldrsh` value, and no legal C can
force a second load.

## SETTLED: the branch-over-pool shape is NOT a blocker

Asked directly of four functions in the class, and the answer is unambiguous:
**gcc-2.96 emits the branch-over-pool instructions itself, at the ROM's
positions, with no help from the source.**

The decisive observation is on `AnimEnd`. Its ROM has two branch-over-pool
sites. While the diff stood at 64 differing both were ABSENT from our output;
once the instruction count converged, **both appeared spontaneously and
matched**. On `StartSnow` the ROM's `b` over its `0xf` pool is present in our
stream and matches from the start.

So the pool branch is a consequence of CODE LENGTH, not of a source construct.
If a reference has one and your output does not, you are the wrong length for
some other reason -- fix that and the branch appears. `.pool_aligned` or a
mid-body `.word` is neither a ceiling nor a signal, and **the 516 functions in
this class should be screened normally.**

This retires the last of the reasoning that made the class look shut. The
earlier claim rested on old_agbcc's pool behaviour and old_agbcc is not the
compiler here; that was corrected in batch 153, and this settles the remaining
question of whether gcc would place them correctly.

## A function can be blocked by FILE STRUCTURE rather than by codegen

Thirty-one single-function `.s` files carry their data as a `.section .data`
full of `.incbin` blobs AFTER `.func_end` -- actor command arrays, scripts,
overlay tables -- under global labels the rest of the overlay references. Those
functions were unreachable for a reason that has nothing to do with how the C
is written: convert the file wholesale and the data goes with it, and the link
dies in a page of `undefined reference`.

**`tools/split_s.py` now handles this case.** It used to refuse every
single-function file that carried data and say "split by hand"; it now checks
whether any of the data sits AHEAD of the function, and if none does, splits
code to `_b` and data to `_c` through the ordinary path. Files with data before
or interleaved with the code still get the refusal, because there is no single
boundary to cut at.

Two things make the clean case safe, and both were already true before anyone
noticed:

- The cut is unambiguous. The function ends at `.func_end` and `.section .data`
  begins a line or two later, so there is exactly one boundary.
- **The linker scripts already list `(.text)` and `(.data)` for these objects on
  separate lines.** Nothing new has to be invented; the two lines just point at
  the two new objects, which `rewrite_ld` has always done.

`OvlFunc_907_2008fa0` was parked one round as "matches byte-exact but cannot be
installed" and is now elevated. If you find a function whose C screens `OK` but
whose file will not convert, check for this shape before parking it -- and
**read the whole of `split_s.py`'s output**, not the line you expected. It
refuses these files with a precise explanation, and piping it through `grep`
for the success line is how one of them got deleted and broke the build.

## What the screen must NOT normalise away

The counterpart to the section below, and the more dangerous direction.

Labels were normalised to `L<n>` in appearance order and the DEFINITIONS were
then dropped, on the reasoning that their position is implied by branch order.
It is not. A function can have the same instructions, in the same order, with
the same normalised branch targets, and still encode a different branch
DISTANCE because the target sits elsewhere.

`OvlFunc_931_2008360` differed from the ROM by exactly one byte that way, and
the C behind it was semantically wrong -- a guard that enclosed one statement
too many. The screen existed to catch that and could not.

Definitions that something branches to are now kept in the stream; ones nothing
references are still dropped, because gcc leaves those behind after pool
resolution and the disassembly does not.

**Every other class in this tool reported a correct function as WRONG, costing
a round. That one reported a wrong function as RIGHT.** When adding a
normalisation, ask which direction its failure runs in.

## A FIFTH SPELLING, found in batch 44

`ldrb r3, [r3]` and `ldrb r3, [r3, #0]` are the same instruction -- the
zero-offset form is an alias -- and the ROM's disassembly writes one while gcc
writes the other. `tryc.py` now folds them.

Only the bare single-register form. `[r3, r2]` is a REGISTER offset and a
different instruction.

## What the screen has to normalise

A FOURTH SPELLING was found in batch 36, and it is a typo rather than a
convention: three lines in the inherited disassembly have **no space after the
operand comma**.

    asm/rom_c9000/rom_d2d98.s                      ldr r0,=.Lee1f5
    asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c.s     ldr r5,=0xffff0000
    asm/overlays/rom_7a8c8c/ovl_30_c_a...c_c_a.s   ldr r0,=.L3058

Collapsing runs of whitespace does not fix that -- there is no whitespace to
collapse -- so a byte-exact translation reports **one differing position in the
middle of an otherwise clean diff**, which reads exactly like a wrong symbol.
`OvlFunc_922_2008f30` is 53 instructions and this was the only difference.

Three lines in the whole tree, and each one is worth a round.



Three spellings differ between gcc's output and the ROM's disassembly without
any difference in machine code. Before these were folded, every function
carrying a constant reported a total mismatch from instruction zero:

| | gcc | disassembly |
|---|---|---|
| destructive Thumb ops | `lsl r2, r2, #2` | `lsl r2, #2` |
| immediates | `#145` | `#0x91` |
| literal pools | `ldr r3, .L14` + `.word` | `ldr r3, =value` |

A fourth was a bug rather than a spelling: label numbering has to be
**per function**, not per file. Shared across a file, a single-function
candidate compared against a multi-function reference gets different numbers
for identical control flow. `ActorCmd_GotoIfZ` read as a mismatch for three
rounds while being byte-exact.

## Codegen facts that decided matches

Established by getting specific functions to match, and reusable:

- **A `u16` field assigned to a `u16` local** loads with `ldrsh` plus a
  redundant zero-extend. Assigned to an **`s32` local** it loads with the
  `ldrh` the ROM uses. The shift is arithmetic either way, because the value
  promotes to `int`.
- **Struct indexing and pointer arithmetic are not interchangeable.**
  `base[i].field` and `*(u32 *)(base + (i << 3) + off)` compute the same
  address but allocate the base pointer and the scaled index to opposite
  registers.
- **gcc pops its return address into r1 instead of r0 when the return type is
  non-void**, whether or not anything is returned. `Func_80b7e7c` only matches
  declared `s32` with no `return` statement.
- **Reading a field twice in the source** is what produces the ROM's redundant
  copy before a compare. Caching it in a local removes the copy.
- **The `if` body becomes the fall-through path.** Where the ROM branches away
  from the code that follows the test, that code was the `if` body in the
  original -- so `ActorCmd_GotoIfNZ` and `ActorCmd_GotoIfZ` are both written
  with the jump as the body and the cursor step as the `else`, rather than one
  being the negation of the other.
- **Taking a pointer to a field and dereferencing it** costs a register and can
  force an extra callee-save push. Access the field through the struct pointer
  each time.

## Tell: the ROM pools a SMALL constant

If the ROM loads a constant from the literal pool that would fit in an eight-bit
`mov` — `ldr r0, =1`, `ldr r2, =0xf` — **that operand was almost certainly a
symbol reference in the original source, not a literal.**

gcc never pools what it can `mov`. It always pools the address of a symbol,
because the value is not known until link time. So a pooled small constant is
the compiler telling you the source said `&_SOMETHING`, and the disassembler
simply resolved it back to a number.

Confirmed by assembling both forms: gas does **not** fold `ldr r0, =1` into a
`mov`, so these are genuinely different bytes and not a disassembly artifact.

**103 of 395 overlay candidates** show this tell — the largest single blocker
in the project.

### FIRST CHECK THAT THE POOL LOAD IS EVEN DIFFERENT (batch 79)

Before spending a round on this tell, confirm the two sides actually differ **in
the encoding**. Twice now they did not.

gcc prints a HImode pool reference as

    ldrh r2, .L0

where the ROM disassembly writes `ldr r2, =0x1f`. **Thumb-1 has no
pc-relative `ldrh` or `ldrb`** — the only PC-form load in the instruction set is
`LDR Rd, [PC, #imm8*4]` — so gas assembles gcc's line to the very same halfword,
`0x4a0d`. `tryc.py` normalised only the `=` spelling and reported these as
differences for as long as they existed; it now rewrites the mnemonic, but the
habit to keep is **objdump both objects before believing a pool diagnosis**.

Two functions sat parked on this for four and eleven formulations respectively
(`BreakItem`, `Func_800c5b4`), and a four-member family was excluded from the
candidate list outright. All six are now elevated with the C essentially
unchanged from what was parked.

The false lead is worth knowing because it is convincing: reading the ROM's
`ldr r2, =0x1f` as this tell gives `(int)&_SYM`, which reproduces the ROM's
assembly **text** exactly. It is still wrong — an SImode symbol load has the
full 1020-byte pool range, so the pool moves to the end of the function and the
jump over it disappears. Right text, wrong bytes.

### Pool ORDER is a readout of operand modes

When every instruction matches and `make compare` still fails, compare the
**order of the pool words**. gcc sorts a minipool by each entry's `max_address`
— the referencing instruction's address plus its `pool_range` — so the order is
a direct statement about the MODE of each operand:

| pattern (arm.md) | `pool_range` | note |
|---|---|---|
| `*thumb_movsi_insn` | 1020 | |
| `*thumb_movhi_insn` | 64 | pool entry is still a full `.word` |
| `*thumb_zero_extendhisi2` | 60 | prints **`ldr`** for a pool label, not `ldrh` |
| `*thumb_movqi_insn` | 32 | |

A HImode `CONST_INT` is emitted into the pool sign-extended, so a HImode mask of
`-0x4000` appears as `.word 0xffffc000` — the pool word is four bytes wide
whatever the mode, and its value alone does not tell you the mode.

An early narrow constant therefore sorts *before* a late wide one, and a
symbol's address always sorts late. `Func_800c5b4` matched instruction for
instruction and failed by fourteen bytes:

    rom   0x1000  Func_800c62c  Func_800c880  0xf1ff
    ours  0xf1ff  0x1000        Func_800c62c  Func_800c880

which says the ROM's `0xf1ff` is SImode and its `0x1000` is HImode. Splitting
the expression so the AND's result stays `u32` fixed it. **No instruction-stream
diff can show this** — `tryc.py` drops pool words on both sides by design — so
when a screen says OK on a function with a pool, the byte comparison is the only
authority.

### A mid-function pool is REPRODUCIBLE, not a disqualification

Batch 30 concluded that a ROM keeping its pool inside the function body behind a
`.pool_aligned` could not come out of a single-function translation unit, and
`pick_candidates.py` excluded 336 references on that basis. **That is wrong.**

gcc emits exactly that shape — `b` around the pool, pool, label — whenever the
pool reference is narrow, because a HImode load's 32–60 byte range cannot reach
the barrier at the end of the function and `dump_table` manufactures a jump over
an early pool. The exclusion is now off by default; `--no-inline-pool` restores
it. What remains true, and is what batch 30 actually caught, is that the SCREEN
cannot see PC-relative offsets, so these must go to `make compare`.

### The placement rule, from `arm.c` rather than by inference

`arm_reorg` (arm.c:5500) walks the fixes and puts the pool at the **last barrier
within reach of the pool's first entry**, manufacturing one with a `b` if none
is in range. It stops accumulating into a pool when the next fix will not fit:

    if (fix->address >= minipool_vector_head->max_address - fix->fix_size)
        return NULL;                       /* add_minipool_forward_ref */

Two consequences worth having in mind before touching a pool-placement park:

- **A wide first entry pushes the whole pool to the end of the function.** This
  is what makes an `int` local for a constant actively harmful when the ROM's
  pool is mid-body — it converts a 64-byte range into a 1020-byte one.
- **A short-range first entry can also SPLIT the pool**, leaving later
  references to a second pool after the epilogue. `OvlFunc_962_200816c` needs
  exactly that and is parked on it: the split requires the head entry's
  `max_address` to be about 64, i.e. a HImode mask, and the mask value it needs
  (`0xffffc000`) is SImode in every C spelling that survives the `(u16)`
  comparison. Eleven were byte-compared; that park says not to retry spellings.

### It does not need the namespace identified — only named

This was treated as blocked for twelve rounds on the reasoning that the tree
has `message.sym` for message ids and `file_table.sym` for file ids, neither
covers whatever `0x4d` is, and inventing a plausible name for a shared linker
fragment is a bad trade — a wrong name propagates, where a missing one waits.

**The premise was wrong.** Matching does not require knowing what the id
*means*. It requires the operand to be a symbol, so that gcc pools it. Naming
the symbol **by value** asserts nothing that could later turn out to be false:

    /* area.sym */
    _AREA_4d = 0x4d;

    extern int _AREA_4d;
    __Func_8091f90((int) (&_AREA_4d), 0x63);   /* -> ldr r0, =0x4d */

An absolute symbol definition in a linker script emits no bytes, so the linked
result is byte-identical to the literal. `message.sym` has done exactly this
from the start — its own comment reads *"named by value; pending semantic
names."* The same move was available here the whole time.

Keep unidentified ids in `area.sym` rather than folding them into
`message.sym` or `file_table.sym`. Those two namespaces are identified, and
putting an unidentified id in one of them asserts something not known to be
true — which is the actual bad trade, and it is avoidable.

First out: `OvlFunc_932_2008388`, with two siblings behind it.

## A `.s` that has been split before has taken the obvious names

`split_asm.py`'s BASENAME WARNING says not to name the `.c` after the `.s`. That
is necessary and not sufficient: a file split in an earlier batch already has
`<base>_b.s` beside it, holding the **generated assembly of an elevated C
file**. Writing a new piece there overwrites it, and the symptom is an
`undefined reference` to a function that plainly exists:

    ovl_30_c_c_c_c_a_b.c:(.text+0x32): undefined reference to `OvlFunc_914_2008b24'

Batch 88 did this to four overlays at once. Nothing warned; `--orphans` passes,
because the linker script is consistent — it is the *contents* of a piece that
were destroyed.

**Choose suffixes by looking at what is already there**, in `asm/` and `src/`
alike, and take the first free letters:

    taken = {suffixes of asm/overlays/<d>/<b>_*.{s,o,d}} |
            {suffixes of src/overlays/<d>/<b>_*.c}
    pick the first len(pieces) letters not in `taken`

The pieces do not have to be `_a`/`_b`/`_c` and nothing depends on their being
in order — the linker script gives the order.

## Several bitfield writes to one byte MERGE -- and their order is the ROM's

Batch 71's rule reads the width of a mask to choose the spelling: a 32-bit
`mov / neg` pair means a bitfield, a bare byte `mov #0xNN` means hand-written
masking. `OvlFunc_883_200db48` extends it in two ways.

**A trailing `& 0xf` can be a bitfield too.** The ROM writes two bytes with one
load and one store each:

    mov r2, #0xd / neg r2, r2 / ldrb r3, [r6, #9] / and r2, r3    ~0xc
    mov r3, #4 / ldrb r1, [r6, #5] / orr r2, r3
    mov r3, #0x21 / neg r3, r3 / and r3, r1 / strb r3, [r6, #5]   ~0x20
    mov r3, #0xf / and r2, r3 / strb r2, [r6, #9]

Three masks, and the last one — `mov r3, #0xf` — *looks* hand-written by the
width rule. It is not: it is a four-bit field cleared to zero, and gcc **merges
adjacent bitfield writes to the same byte** into the single load and store the
ROM has. Written as ordinary masking with `int` locals to keep the constants
wide, the function is 77 differing of 98; written as three bitfield assignments
it is exact.

**The order of the assignments is the ROM's interleave, not source-tidy order.**
`f9_mid = 1; f5_b5 = 0; f9_hi = 0;` matches. The same three regrouped as
`f9_mid; f9_hi; f5_b5` — which reads better, both f9 writes together — is 20 of
98. gcc emits them in the order written and schedules the loads around them.

**So when a byte-width mask appears next to 32-bit ones on the same field, try
it as a bitfield before assuming the width rule applies.**

## The same function can want a named zero and a bare one

`OvlFunc_883_200db48` stores `0` to five fields and `1` to two. The zero needs a
named `int z` assigned at the top of the function, so its pseudo is live across
the calls and lands in a pushed register — the pattern from batches 78, 83 and
85. The one must be written as a **plain literal in both places**: gcc shares it
into a callee-saved register by itself, and giving it a local puts the `mov #1`
on the wrong side of a neighbouring store.

Same function, same kind of constant, opposite answers. Read each one off the
ROM rather than applying the pattern that worked last time.

## A two-way choice of NEARBY constants goes branchless -- put the call in both arms

`__MapActor_GetActor(c ? 0xf : 0xe)` does not compile to a branch. gcc notices
the two values differ by one and emits the "is non-zero" chain:

    ours   neg r0, r3 / orr r0, r3 / lsr r0, #0x1f / add r0, #0xe
    rom    cmp r3, #0 / beq .L0 / mov r0, #0xf / b .L1 / .L0: mov r0, #0xe / .L1:

An `if`/`else` assigning to a variable does the same thing, and so does swapping
the declaration order. **Write the call inside each arm.** gcc cross-jumps the
call itself and keeps the branch for the argument, which is exactly the ROM's
shape:

    if (*p & 1) a = __MapActor_GetActor(0xf);
    else        a = __MapActor_GetActor(0xe);

That took `OvlFunc_898_2008314` from 58 differing lines to 4. It is the mirror
of the `neg / orr / lsr #31` section below: there the idiom is what the ROM has
and a statement-level branch is what produces it; here the branch is what the
ROM has and the obvious C produces the idiom.

## Two initialisers come out in the OPPOSITE order to their assignments

The last four lines of the same function were two constants loaded into
callee-saved registers before any branch:

    rom    mov r2, #0x12 / mov r10, r2 / mov r3, #0x0 / mov r9, r3
    ours   mov r2, #0x0  / mov r9, r2  / mov r3, #0x12 / mov r10, r3

Writing `kind = 0x12; flag = 0;` emits flag first; writing `flag = 0;
kind = 0x12;` emits kind first. **Declaration order does not reach it** —
swapping `int kind; int flag;` changes nothing. Only the order of the
assignments does, and it inverts.

## Find a family by its SHAPE, not by byte identity

`tools/find_twins.py` finds functions byte-identical up to symbol names.
`tools/find_shape.py` finds functions with the same **instruction stream** once
constants, labels and call targets are wildcarded — a much larger net, because
siblings usually differ in ids and in which functions they call.

    python3 tools/find_shape.py <asm/file.s> <SolvedFunction>   # siblings of one solve
    python3 tools/find_shape.py --clusters --min-insn 25        # all families at once

`--clusters` is the one to run when nothing is solved yet: it groups every
remaining function by shape and ranks by payoff. Its top entries are much larger
than `find_twins.py`'s — an 18-member group at 176 instructions, and the
thirteen-member family `find_twins.py` reports is **seventeen** by shape.

The workflow is: solve one member, then fill the captured constants into the
same C. Batch 88 did two families this way, nine functions, every sibling clean
on the first screen.

## A split piece that holds only `.include` is not a piece

When the cut is at the very head of a `.s`, the "head" still contains the file's
`.include "macros.inc"` line, so a naive `if head.strip()` writes an `_a.s` with
no content, an empty `_a.o`, and a linker-script line for it. It links and
`make compare` passes — an object contributing nothing to `.text` changes
nothing — so this is invisible until someone reads the script and wonders what
`_a` is.

Decide on FUNCTIONS AND SECTIONS, not on whether the text is empty:

    body = [l for l in head.split("\n")
            if l.strip() and not l.strip().startswith(".include")]
    if body: write the _a piece

Same for the tail. Caught in batch 82 on `ovl_30_c_c_c_c_c_c`.

## `orr rd, rs` -- which operand becomes the destination

Thumb's two-operand `orr` makes the destination one of the operands, so the ROM
tells you which side of the source expression gcc kept:

    rom    ldrb r2, [r0] / mov r3, #2 / orr r3, r2      the CONSTANT is rd
    ours   ldrb r3, [r0] / mov r2, #2 / orr r3, r2      the VALUE is rd

`*p |= 2` gives ours, and so does every commutative rearrangement of it —
`*p = 2 | *p`, `*p = *p | 2`, naming the loaded value, naming the constant as an
`int`. gcc canonicalises a commutative operator to put the constant second and
none of those reach it.

**Name the constant in a local OF THE WIDTH IT IS COMBINED WITH, and write it
first:**

    unsigned char two = 2;      /* p is unsigned char *  */
    *p = two | *p;

The width matters: an `int` local leaves the order unchanged. Solved
`OvlFunc_920_2008214`.

### IT IS A SPELLING TO TRY, NOT A RULE TO APPLY (batches 85, 86)

`OvlFunc_898_200913c` has both answers four instructions apart:

    rom   ldrb r2, [r5]     / mov r3, #0xfe / and r3, r2      the CONSTANT is rd
    rom   ldrb r3, [r6, #9] / mov r2, #0xc  / orr r3, r2      the VALUE is rd

and the plain `*p &= 0xfe` and `m[9] |= 0xc` give exactly those, with nothing
named. Naming either constant moves the wrong one. Meanwhile
`OvlFunc_903_2008d04` needs the narrow local for **its** `orr` — the plain form,
`2 | *p`, and an `int` local all get it backwards.

So the order is: **write the plain form, look at which operand the ROM makes the
destination, and only then reach for the local.** Applying the lever on sight
costs a screen as often as it saves one.

### When the ROM also POOLS the constant, that is the tell

For a halfword field the same trick gets the operand order but changes the
constant from a pool load to a `mov`, because the widened local is SImode:

| source | operands | constant |
|---|---|---|
| `*p \|= 2` | wrong way round | `ldr r3, =2` ✔ |
| `unsigned short two = 2; *p = two \| *p;` | right ✔ | `mov r3, #2` |
| `two = (unsigned short)(int)&_CONST_2;` | right ✔ | `ldr r3, =_CONST_2` ✔ |

Only the symbol form gives both — which is the pool tell (§"the ROM pools a
SMALL constant") arriving with a measurement behind it rather than an
assumption. `const.sym` holds these; its header records the bar for adding one.

Four functions turned on this. The internal control in `OvlFunc_898_2008cfc` is
worth knowing: it uses the value 2 **twice**, once as a save-flag id where the
ROM writes `mov r0, #2` and once in this OR where it writes `ldr r3, =2` — same
value, same function, one immediate and one pooled.

## A named local has a TYPE and a POSITION, and the ROM says both

Widening a stored constant with an `int` local is batch 84's rule, and it is
only half the answer. `OvlFunc_939_20091d0` stores `0x5b` into a `u16`:

| local | result |
|---|---|
| `int v = 0x5b;` next to the store | 10 of 40 differing |
| `int v = 0x5b;` at the top of the function | **match** |

Assigned at the top, the pseudo is live across the calls and lands in a
callee-saved register — which is where the ROM has it. The same distinction
decided `OvlFunc_901_2008864` (batch 83) and `OvlFunc_928_2008968` (batch 85),
both of which needed a stored zero assigned before the first call so it would be
pushed.

**Read the register off the ROM.** A caller-saved register (r0–r3) means the
pseudo is born after the last call; a pushed callee-saved one means it is born
before the first.

## Stack arguments: name them to keep two registers alive

`f(0, 0, 1, 1, 0x12, 0xe)` with literals gives

    mov r3, #0x12 / str r3, [sp] / mov r3, #0xe / str r3, [sp, #4]

— gcc computes and stores each in turn, reusing r3. Where the ROM builds both
first,

    mov r3, #0x12 / mov r2, #0xe / str r3, [sp] / str r2, [sp, #4]

name them as locals. Two live pseudos instead of one, and the four instructions
come out in the ROM's order. Three of `OvlFunc_920_2008214`'s five differences
were this.

## A named local used ONCE can cost the preferred register

The tree has a standing lever that says naming an intermediate forces the
three-operand form and generally helps. **The converse is also real**, and it
shows up as a clean register transposition rather than as extra instructions:

    base = *(char **)iwram_3001ebc;
    *(int *)(base + (0xe0 << 1)) = 0x201;     /* r2/r3 swapped vs the ROM */

    *(int *)(*(char **)iwram_3001ebc + (0xe0 << 1)) = 0x201;   /* matches */

`REG_ALLOC_ORDER` (arm.h:989) starts `{3, 2, 1, 0, ...}`, so r3 goes to whichever
pseudo the allocator ranks first. Giving the base pointer its own named local
changes the ranking and hands r3 to the offset constant instead.

**So when a block differs only by a consistent register swap, try removing a
local as well as adding one.** On `OvlFunc_953_200960c` four other spellings of
the same store — operand order swapped, the destination named, the offset named,
the offset written as the folded literal — all gave the identical six
differences, and so did `-fno-gcse`, `-fno-rerun-cse-after-loop`,
`-fno-schedule-insns2` and `-O1`. Only deleting the local moved it.

The corollary matters too: **do not delete a local the ROM's own code implies.**
The same function reads `iwram_3001ebc` a second time, and there the local
stays, because the ROM holds `&iwram_3001ebc` in r5 across every call and
re-loads the pointer — two separate reads of a global, not one cached value.

## Technique: reaching a file-local `.L` symbol from C

A function that indexes a table living in its own `.s`, or reads a `.lcomm` slot
the overlay declares, needs a name C cannot spell:

    ldr r1, =.L23f0            @ a data label
    .global .L57fc             @ a .bss slot
    .lcomm  .L57fc, 4

Local labels do not cross object boundaries and `.L23f0` is not an identifier.
`split_asm.py` reports the first case as `MUST EXPORT`, but `.global` on its own
only solves the asm-to-asm half.

**Use gcc's asm-label extension. It is the least invasive form and this tree
already used it before either of the notes below were written:**

    extern unsigned int L57fc __asm__(".L57fc");
    extern short        tbl[]  __asm__(".L23f0");

The declaration gets a link name that need not be a valid identifier, and **no
other file changes** — which matters, because a symbol like `.L57fc` may be
referenced from several `.s` files and from already-elevated `.c` files that use
this same idiom. Renaming it breaks those; batch 81 broke two that way and had
to back the rename out.

### The heavier alternative: rename and export

Renaming the label and adding `.global` also works, and batch 80 used it for
`gTable_921__0200a3f0`:

```diff
-.L23f0:
+	.global gTable_921__0200a3f0
+gTable_921__0200a3f0:
 	.incbin "overlays/rom_7aa430/orig.bin", 0x23f0, (0x2430-0x23f0)
```

A label emits no bytes, so this changes symbol-table metadata only and the link
stays byte-identical. It reads better in the C, and it is worth doing when the
symbol is referenced from exactly one place and a real name is genuinely more
informative. **It is not worth doing to make a reference possible** — the asm
label already does that — and every reference to the old name, in `.s` and `.c`
alike, has to move with it.

Name by ADDRESS in either case (`gTable_921__0200a3f0`, `gOvl_020086dc`), which
asserts nothing about what the data means — the same reasoning as naming ids by
value in `area.sym`.

## Technique: stopping a constant fold with symbol addresses

Where the ROM computes a constant AT RUNTIME:

    ldr r3, =0xc9b / ldr r1, =0xcc6 / sub r1, r3

no C written with literals reproduces it — `0xcc6 - 0xc9b` folds to one
immediate and a pool word vanishes. The operands were the **addresses of
absolute symbols** in the original, and gcc cannot fold the difference between
two link-time addresses:

    extern int _MSG_c9b;
    extern int _MSG_cc6;
    f(first, (int)&_MSG_cc6 - (int)&_MSG_c9b);

Define the ids in `message.sym`, a linker fragment whose definitions emit no
bytes. This unlocked the whole seven-function `OvlFunc_974` family.

Two things that are easy to get wrong:

- **Which ids must be symbols is not arbitrary.** The subtraction's operands
  must be, or the fold returns. Every other argument stays a plain literal
  *unless* the ROM reuses one register for both that argument and a side of
  the subtraction — then it must be a symbol too. Either mistake costs an
  instruction.
- **`message.sym` is not a tracked dependency of `stage1.o`.** After adding
  symbols, delete `stage1.o` by hand or the overlays will not see them and the
  link fails with an undefined reference.

`tools/find_runtime_constants.py` lists every function in the corpus with this
signature and says which symbols each would need.

## Families: 190 functions in 50 groups

`tools/find_families.py` groups hand-written functions by identical shape --
registers, immediates and branch targets abstracted away, call targets kept.
Two stubs that differ only in a slot number land in the same group.

**50 families covering 190 overlay functions**, the largest with **30
members**. The overlays duplicate their cutscene and talk stubs per map rather
than sharing them, so the same sequence recurs across a dozen overlays with
one constant changed.

Once one member matches, the rest are mechanical: same C, different constant.
The three families found by accident before this tool existed gave 7, 4 and 3
functions from one insight each -- 14 of the first 44 elevated.

The converse matters too: whatever blocks one member blocks all of them, so a
family is also the cheapest way to discover that a blocker is expensive. Check
a candidate's family size before deciding how hard to push on it.

## WITHDRAWN: "the declaration lever is an -O2 behaviour"

**This section asserted that the declaration lever does not apply at -O1. That
was wrong, and the way it was wrong is the more useful lesson.**

The evidence was a natural control: `OvlFunc_923_2008ed0` (rom_7aa430, believed
-O1) and `OvlFunc_924_2008f84` (rom_7ac2d8, -O2) are the same forty-one
instructions, and the identical C matched at -O2 and failed at -O1.

Every observation there was accurate. The inference was not, because
`OvlFunc_923_2008ed0` **was never an -O1 translation unit**. A Makefile pattern
rule written for a neighbouring TU captured its file by name prefix (batch 45).
With the rule narrowed, the function matches at -O2 on the C that was parked,
and it is elevated in batch 46.

So there is no -O1 counterexample to the lever, and none was ever observed.

### What to take from it

**Two functions that match on identical C are telling you they are the same TU
shape.** Read that as evidence about the BUILD when their flags differ, not as
evidence that the flags explain the difference. The control was pointing at the
Makefile the whole time and was read backwards.

A per-file flag rule spelled as a `%` pattern is a **claim to check**, not a
fact: the `_a`/`_b`/`_c` split chain is a positional carve of one overlay's
assembly, and an overlay holds many TUs, so a name prefix does not imply a
shared compiler invocation. `tools/tryc.py` prints `(built with: O1)` when a
rule fires, and now adds a warning on a MISMATCH when those flags came from a
wildcard rule. **Read both lines.**

The -O1 units that are named by explicit Makefile targets remain real; the
lever has still never been tested against one of those.

## Declare every callee — argument order depends on it

**A missing prototype changes the generated code.** This is the single
cheapest lever found so far, and it went unnoticed for 52 functions because
most of the tree calls overlay routines with no declaration at all and gets
away with it.

An implicitly declared function returns `int`. gcc-2.96 therefore treats r0 as
holding a live return value across the call and defers writing r0 when setting
up the *next* call's arguments. Declare the callee — with its `void` return
type — and gcc fills r0 first instead:

    no prototype    mov r2,#0x20 / mov r3,#0x20 / mov r1,#0x40 / mov r0,#0
    prototyped      mov r2,#0x20 / mov r3,#0x20 / mov r0,#0    / mov r1,#0x40
    rom             mov r2,#0x20 / mov r3,#0x20 / mov r0,#0    / mov r1,#0x40

Both orders occur throughout the ROM, and both occur *inside a single
function* — `OvlFunc_929_2008524` fills r0 first for two calls and last for a
third. That is not a scheduling accident to be fought with reformulations; it
is gcc reporting which callees the original translation unit had declarations
for.

So: **when the only mismatch is argument fill order, add full prototypes for
every callee before trying anything else.** Return types matter as much as
parameter types — prototyping only the mismatching call is not enough, because
the deferral is caused by the *preceding* call's return type.

This retired the `arg-fill-order` blocker class, which had cost nine failed
formulations while the fix was one line of C.

### The lever reorders ARGUMENT CONSTRUCTION, not just r0

Every use of it below is about where `mov r0` lands, because that is the case it
was first noticed on. It is more general than that.
`OvlFunc_898_2008f3c` has r0 first on both sides and still differs:

    rom    mov r0, #0xcc / mov r1, #0xa0 / lsl r0, #1 / lsl r1, #1 / mov r2, #5
    ours   mov r0, #0xcc / mov r1, #0xa0 / lsl r1, #1 / mov r2, #5 / lsl r0, #1

gcc interleaves the second shift with the third argument and defers the first
shift to the end. Declaring the callee puts the whole block in the ROM's order.

**So try it on any argument-block ordering difference**, not only the ones where
r0 is misplaced.

### There are TWO declaration levers, and they are not the same one

The lever above is about the **preceding** call: its return type decides
whether r0 is live across it, and therefore whether the *next* call fills r0
first or last. Everything above stands.

There is a second, separate effect. **Leaving the mismatching call ITSELF
implicit changes the order gcc fills that call's own argument registers**, and
it puts r0 last:

    prototyped   mov r0, r8    / mov r1, #0x80 / mov r2, r5
    implicit     mov r1, #0x80 / mov r2, r5    / mov r0, r8
    rom          mov r1, #0x80 / mov r2, r5    / mov r0, r8

The two wear identical clothes — both are "a missing prototype changes r0's
position" — and conflating them cost two functions a park each. `LoadStatusIcon`
was parked with a note recording three failed attempts and the conclusion "the
order does not move"; one of the things not tried was deleting the declaration
of the call that was actually wrong. Both it and `Func_8078948` matched on the
first screen once it was.

So the rule is **both directions, on both calls**. When argument fill order is
the only mismatch, there are four things to try, not one:

1. prototype every callee (the documented lever — fixes the common case);
2. make the **preceding** call implicit;
3. make the **mismatching** call implicit;
4. prototype the mismatching call but not the preceding one.

**The lever cannot reach the first call after a control-flow JOIN.** It works by
fixing whether r0 is live across the *preceding* call — so where the preceding
call differs per path, no declaration can decide the question.
`src/non_matching/ovl_793768/2008e0c.c` is 39 of 41 instructions identical with
one misplaced `mov r0`, and seven declaration combinations produce byte-identical
output. Check for a join above the mismatching call before spending screens on
it.

Note this is not a free win everywhere. It moves r0 specifically. Where the
transposition is among the *non-r0* arguments — `mov r1 / ldr r2` against
`ldr r2 / mov r1` — neither lever reaches it, and that residue is still a real
blocker; see `src/non_matching/ovl_77dd1c/2008398.c`, where both directions
were tried before this was understood and neither helped.

## OUR STREAM BEING SHORTER IS A SIGNATURE, NOT A CURIOSITY

If a screen reports fewer instructions than the ROM, gcc found something cheaper
than the original compiler did — and that is almost always a *rewrite* of the
source shape, not a register-allocation difference. It points at a specific
cause, so it is worth sorting the parked set by it:

    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        python3 tools/tryc.py <park> --ref <asm> --quiet     # rom N, ours M

Batch 55 swept all parks this way and found **16 where ours is shorter**. Two
unparked immediately on the first thing tried:

| Cause | Fix |
|---|---|
| the default computed only on the path that needs it | assign it **before** the test, then overwrite in the matching arm |
| a compound condition fused into one comparison | split the bounds into separate statements |

`OvlFunc_964_20092b0` and `OvlFunc_965_2008fac` were both the first case: the
park held `if (area == X) return script; return 0;` at 14 lines against 15, and
the ROM sets `r0` to zero **before** the `cmp` and replaces it in the arm.

**A shorter stream is a failure, and a legible one.** Longer usually means a
blocker; shorter means gcc saved an instruction the original compiler did not.

### But there are TWO causes, and only one is fixable

Refining this after batch 56 walked into the other one:

| Cause | Signature | Fixable? |
|---|---|---|
| gcc **rewrote the source shape** — fused a condition, hoisted a default, replaced a mask with a shift | the missing instruction is somewhere the source can move | **yes** — a statement boundary, or restructuring the arms |
| gcc **cross-jumped** — two predecessor blocks ended in the same instruction, so it sank that instruction into the shared successor | the missing instruction is the LAST one of two blocks that meet | **no known fix** |

The second is the pre-header load merge class
(`src/non_matching/preheader_load_merge.c`), four members, every one short by
exactly one instruction and always the same instruction.

**So check which kind before spending a round.** If the missing instruction sits
at a join, it is cross-jumping and the source cannot reach it.

## Blocker: gcc rewrites a signed LOWER bound and leaves the upper one

    rom    cmp r0, #0xc4 / bgt <out>      upper bound -- MATCHES
           cmp r0, #0xc1 / blt <out>      lower bound
    ours   cmp r0, #0xc4 / bgt <out>
           cmp r0, #0xc0 / ble <out>

gcc-2.96 canonicalises **every** signed lower-bound test to `cmp #(K-1) / ble`:

    v < 0xc1     ->  cmp #0xc0 / ble
    v <= 0xc0    ->  cmp #0xc0 / ble
    v >= 0xc1    ->  cmp #0xc0 / ble     (inverted, for the else arm)

and leaves upper bounds alone. The ROM's compiler does not do the rewrite.

**This is one-directional, which is what makes it a class rather than noise.**
Two functions sit at exactly 2 lines on it — `OvlFunc_899_2008048` and
`Func_80a3ce4` — after every other difference is fixed. No spelling reaches it,
and the operand's type does not change it.

**Check for it before spending a round.** If the only remaining difference is
`cmp #(K-1) / ble` where the ROM has `cmp #K / blt`, there is a 2-line floor.

## A COMPOUND CONDITION FUSES; SPLIT IT INTO STATEMENTS

Two range tests written as one condition get fused into a single unsigned
comparison:

    if (v > 0x11 || v < 0xf)          ->  sub r3,#0xf / lsl r3,#16
                                          cmp r3, 0x20000 / bls

Two instructions longer than the ROM, which does the obvious pair of `cmp`s.
Writing the bounds as separate statements with a `goto` stops it:

    if (v > 0x11) goto other;
    if (v < 0xf)  goto other;

`OvlFunc_899_2008048`: **16 of 22 fused, 2 of 22 split.**

This is the same lever as the `neg/orr/lsr` idiom below — **a branch in the
SOURCE stops a rewrite that no amount of naming reaches.** Two separate findings
now point at it, so treat "gcc replaced my arithmetic with something cleverer"
as a cue to add a statement boundary rather than to rename an intermediate.

(What it does *not* reach: the constant gcc picks for a `<` comparison. It
canonicalises `v < 0xf` and `v <= 0xe` to the same `cmp #0xe / ble`, where the
ROM has `cmp #0xf / blt`, and neither spelling nor the operand type changes it.)

## The `neg / orr / lsr #31` idiom needs a STATEMENT-LEVEL branch

gcc-2.96's branchless "is this non-zero" sequence --

    neg r0, r3 / orr r0, r3 / lsr r0, #0x1f

-- is **not** what `!= 0` compiles to. For `Func_807a2bc`:

| Source | Result |
|---|---|
| `return (x & (1 << bit)) != 0;` | 15 lines — rewritten to `(x >> bit) & 1` |
| `return (x & (1 << bit)) ? 1 : 0;` | 15 lines — same rewrite |
| `v = x & (1 << bit); if (v) return 1; return 0;` | **18 lines, exact** |

Both expression forms let gcc see that the result is a single bit and replace the
mask with a shift, three instructions shorter. Only the **statement-level
`if`/`return`** produces the ROM's sequence.

Naming the mask and the value in their own statements *without* the `if` is 19
lines and worse than either. **It is the branch that stops the rewrite, not the
naming** — which is the opposite of the usual lever, where naming an
intermediate is what preserves structure.

## Tell: `pop {r1}` in a function that looks void names a RETURN VALUE

An epilogue that pops the return address into **r1 instead of r0** is a
statement about the function's return type, not about register allocation.

    rom    ... bl OvlFunc_946_2009a44 / add sp, #0xc / pop {r1} / bx r1
    ours   ... bl OvlFunc_946_2009a44 / add sp, #0xc / pop {r0} / bx r0

gcc-2.96 uses r0 for the scratch register in a Thumb epilogue whenever it can.
It reaches for r1 only when **r0 is still live across the epilogue** — which
happens when the value in r0 is the function's own return value. So the ROM is
saying: this function returns something, and the something is whatever the last
call left in r0.

    return OvlFunc_946_2009a44(actor, pos);

Written as `void` the two streams differ in exactly those two instructions and
nothing else — a 2-of-17 diff that reads like noise if you are not looking for
it. Two functions in batch 46 turned on this (`OvlFunc_946_2009b68`,
`OvlFunc_946_2009b14`), while their three siblings in another overlay genuinely
are `void` and pop `r0`.

**Check the epilogue register before writing `void`.** It costs nothing and it
is not recoverable from the body: a tail call whose result is discarded and one
whose result is returned have identical bodies.

Related: the same reading identified those two functions' argument lists. The
ROM never rewrites r0 before the `bl`, so the pointer returned by an earlier
call is still there and is the first argument — the callee takes the actor AND
the array, where the siblings take only the array.

## `bl _call_via_rN` means the source called through a POINTER

When the ROM loads a function's address and branches through it —

    ldr r3, =Func_8001af8 / ... / bl _call_via_r3

— that is not a veneer the linker inserted and it is not something to work
around. It is what gcc emits for a call through a function pointer. A direct
call is one instruction shorter, so the screen shows *ours* one line short with
the `ldr` missing, which reads like a missing instruction rather than a wrong
call form.

The fix is to route the call through a local of function-pointer type:

    typedef void (*CopyFn)(volatile u16 *dst, void *src, s32 len);
    CopyFn copy;
    ...
    copy = Func_8001af8;
    copy(pal, data, 0x80);

**gcc-2.96 does not constant-propagate that back into a direct call at -O2.** A
modern compiler would, which is presumably why the shape went untried for so
long — it looks like it cannot possibly survive optimisation, and it does.

First matched in `src/rom_c9000/rom_e0524.c` (`LoadVFXFile`).

### The pointer's return type is the declaration

For an indirect call there is no prototype at the call site — **the function
pointer's type IS the declaration**, and its return type drives the same lever
as a direct callee's:

    void (*fp)(int, int)    ->  gcc fills r0 FIRST
    int  (*fp)(int, int)    ->  gcc fills r0 LAST

**Read the order off the ROM; neither one is the default answer.**
`Func_80b63b0`, `Func_801671c` and `Func_8016738` all want r0 last and all need
`int`. `Func_80cd508` wants r0 *first* and needs `void` — and it calls the same
`Func_80008d4` that `Func_80ccbdc` calls with `int`. One callee, two files,
opposite types, both byte-exact. That is the point: the pointer's return type is
a per-call-site fact reporting what the original translation unit declared, not
a property of the callee.

When an indirect call has the wrong r0 position, change the *pointer's* return
type, not where the pointer is stored. Three parked functions had notes
recording several attempts at the storage — "via a typedef'd local, via a plain
local, with the destination in its own local" — and none at the type.

Pair this with the epilogue rule already listed above: `pop {rN}` for N ≠ 0
means the *enclosing* function's return type is non-void. Between them, these
two type choices decided all three of those functions completely, and two of
the three had been filed as permuter seeds.

223 functions in `asm/` still call through `_call_via_rN`, so this is worth
checking against before parking any of them.

## Un-rotated loops: write the control flow with `goto`

gcc rotates a counted loop — body first, test at the bottom — whenever the
initial value provably satisfies the condition, because it can then drop the
entry test. Where the ROM instead **tests at the top and jumps into the test**:

    mov r5, #0 / b .L1 / .L2: add r5, #1 / .L1: cmp r5, #0x59 / bgt exit

no structured spelling reproduces it. `for`, `while`, and -O1 all give the
rotated form. Writing the control flow out directly does:

    i = 0;
    goto test;
    inc:
        i++;
    test:
        if (i > 0x59) return;
        ...
        if (cond) goto inc;

That is legal C and it is what the ROM's control flow actually *is*. It moved
`MapActor_WaitAnim` from 26 of 29 instructions to 28. Try it on any near-miss
where the ROM tests at the top of a counted loop and gcc tests at the bottom.

## Separate variables do not defeat a COPY

Giving a value two names is a real lever — it is what moved
`OvlFunc_945_200d068`'s first divergence by thirty instructions. But it only
works on a value gcc **computed** and would otherwise reuse. It does **not**
stop gcc coalescing a plain copy:

    s = a->sprite;  p = s;  p += 0x24;    /* gcc emits one register, no `mov` */

Three functions have now been tried this way against a ROM that stages a
pointer through a scratch register before moving it to a callee-saved one
(`rom_c00d8.c`, `rom_5868.c`, `rom_91c44.c`), and it fails identically in all
three. Not worth a fourth attempt without a new idea.

## LOOK UP A SHAPE IN THE SOLVED CORPUS BEFORE INVENTING A CONSTRUCT

The tree tracks the **generated** `.s` beside every elevated `.c`. That means the
corpus of solved codegen is on disk and searchable, and it answers *"what C
produces this instruction"* directly rather than by derivation.

    python3 tools/find_solved_shape.py 'stmia'
    python3 tools/find_solved_shape.py --seq 'mul' 'asr'

Batch 53 spent a whole round's thinking on one question — what produces
`stmia r3!, {r0, r1, r2}`? The answer was **two files away**: a neighbouring
elevated function in the same overlay already used `DMA3_COPY` from
`include/dma.h`. Once found, the C took one correction and four functions fell
out of it.

It only searches `.s` files that carry gcc's banner **and** have a sibling `.c`,
because that pair is the proof the C is what produced the assembly. Hand-written
corpus is deliberately skipped: it shows what the ROM does, which you already
have, not what C reproduces it.

**A "no hits" answer is also useful.** It says the shape is genuinely new, so
time spent deriving it is not time wasted looking in the wrong place. `mov r12,
rN` returns no hits — nothing in the elevated corpus uses r12 as a scratch — and
that turned out not to matter, because gcc emits it from register pressure with
no prompting from the source at all.

## Match against the SOLVED corpus, not the unsolved one

`tools/match_shapes.py` reduces every function to a skeleton -- mnemonic plus
operand KINDS -- and reports remaining assembly whose skeleton already has a
matched `.c` behind it. It works because the build writes gcc's output to
`asm/<path>.s` beside the hand-written corpus, so both halves of the tree are
available in the same representation.

**This is a better question than the other three rankers ask.**
`elevation_candidates.py` and `pick_candidates.py` rank by what looks *easy*;
`find_twins.py` groups the remaining assembly against itself. A shape that has
already matched once comes with a finished `.c` to copy, which is a stronger
signal than any tractability heuristic -- batch 32 got five functions that way
by hand, four of them on the first screen, and its first automated run produced
five more, all five on the first screen.

**The destructive-form collapse is load-bearing.** gcc emits `lsl r2, r2, #2`
and the ROM's disassembly writes `lsl r2, #2`. Without folding those together
the two corpora share almost no shapes: the tool found 4 leads before the fold
and 20 after, on the same tree. `tryc.py` has normalised this since its first
version, and it still had to be rediscovered here, because a skeleton looks
nothing like an instruction diff.

## A register swap between two near-identical functions proves nothing

`OvlFunc_952_20080c8` puts the message base in r5 and the actor slot in r6.
`OvlFunc_952_200bfc4` is the same function with a different id and puts them the
other way round. **The C is identical in shape for both** -- the allocation
follows which value the compiler happened to see first, and nothing in the
source controls it.

This is worth stating because the natural reading is the opposite one: the
registers differ, so the sources must differ, so go looking for the difference.
There isn't one. Spend the screen on the constants instead.

## Multiple exits: `goto` the ROM's join points

The first finding that is specific to LENGTH rather than to shape.

A function with more than one exit -- typically one path that runs a teardown
call and one that skips it -- has to be written with `goto` to labels mirroring
the ROM's join points. Written with early `return`s, gcc duplicates the teardown
call at each exit and lays the blocks out in its own order.

`OvlFunc_942_200851c` has two exits and five paths reaching them. Natural
`if`/`else` with `return`s: 78 of 147 instructions in disagreeing regions.
The same code with two `goto` targets: 42. Nothing else changed.

Short functions have one exit and never raise the question, which is why this
did not surface in the first 36 batches. See reports/large-functions.md.

## Use `--align` on anything long

`tryc.py`'s headline count is POSITIONAL -- instruction *i* against instruction
*i* -- so a single extra instruction on one side makes every later position
report as different. At twenty instructions that is survivable because the whole
listing is readable. At 140 it reported "132 differ" for streams that disagreed
in six places totalling 36 instructions, and reported the same number for every
variant tried afterwards, so it could not rank them.

`--align` reports disagreeing REGIONS and a count of instructions inside them,
which does fall as a candidate improves. Default to it above about fifty
instructions.

## Pointer arithmetic: the ROM's `add` says which form to write

Two ways to reach `base + off`, and the ROM tells you which it wants before you
write anything:

| the ROM has | write |
|---|---|
| `add r3, r2` — DESTRUCTIVE, two operands | a walk: `p = base; p += off;` |
| `add r3, r6, r2` — three operands | one expression: `p = base + off;` |

Getting it backwards is not one instruction out. `OvlFunc_923_20091b4` was SIX
positions out with the walk form where the ROM wanted one expression, because
the register roles swap: which register ends up holding the base and which the
offset follows from how the address was built.

**And the two forms can both be right in one function.** That same function
computes an iwram address that wants the single expression and, three lines
later, a gState address that wants the walk — written as one expression the
gState case folds into a single pool constant `gState+555`, which is worse
again. Read each site off the ROM; there is no default.

The walk form has been this tree's habit because it was the first one written
down, and that is exactly the kind of thing that turns into a wasted screen.

## Name the pointer to move a load's base and offset

A register-offset load `ldr rD, [rA, rB]` puts the POINTER in rA and the index
in rB when the C is `table[i]`. Where the ROM has them the other way round,
assign the table to a local and index that:

    ours   ldr r3, [r1, r3]        unsigned int v = L9f0a4[i];
    rom    ldr r3, [r3, r1]        unsigned int *t = L9f0a4; ... t[i];

`Func_8095b8c` was parked for two batches on exactly this one instruction. Its
note carried three restructurings that all made it WORSE -- writing the index as
the pointer base, naming the index, and casting the table to `unsigned char *`
-- because each moved the whole expression. Naming the table moves only which
operand gcc treats as the base.

Same family as the named-intermediate lever, different target.

## Sweep flags ACROSS the parked set, not per function at parking time

`tools/rank_parks.py --flags` screens every park under every per-file build
setting the tree uses and reports the ones a flag improves. Its first run
unparked two functions in a single pass.

One of them, `OvlFunc_939_20087f4`, had cost several screens across two batches
and matches at **-O1 with the parked C unchanged**. `--O1` had been tried on it
-- on three of its four source formulations. It was never tried on the fourth,
because by the time that one was written the flags had already been ruled out on
the others.

**Flags and source forms are two axes and they were being swept separately.** A
park written before a per-file rule existed is never revisited by anything, so
run this whenever a new rule is added -- that is the moment the parked set may
have quietly become solvable.

## Rank the parked set before re-attempting anything

`tools/rank_parks.py` screens every park and sorts by how far out it actually
is. The parked set reads as a flat list of dead ends and is not one: the first
run found a park ONE instruction from matching, and it was matched the same
round. Nothing about it had changed since it was parked; it was never at the top
of anything.

## THE COMPILER'S SOURCE IS IN THE BUILD IMAGE

`/opt/camelot-gcc/gcc-2.96/gcc/` -- 150 `.c` files, the actual 2.96 tree the
`cc1` we run was built from. This was found in batch 38, after thirty-seven
batches of treating gcc as a black box.

**Every blocker class in this document was characterised by probing.** That is
the expensive way. A question like "why does a `volatile` local defer the shift"
took twelve hand-written probes and produced a wrong conclusion; the answer is
four lines of `expand_decl` in `stmt.c` --

    && ! TREE_THIS_VOLATILE (decl)
    && ! TREE_ADDRESSABLE (decl)
    && (DECL_REGISTER (decl) || optimize)

-- which says a `volatile` local never gets a register at all, so its effect on
scheduling is just a consequence of the operand being a MEM. Ten minutes of
reading against a morning of probing, and the probing got it wrong.

The passes worth knowing where to find:

| question | file |
|---|---|
| register or stack slot for a local | `stmt.c`, `expand_decl` |
| which pseudo gets which hard register | `local-alloc.c`, `global.c` |
| rematerialise or keep alive | `local-alloc.c`, `reload1.c` |
| argument set-up order | `calls.c`, `expand_call` |
| instruction ordering | `haifa-sched.c` |
| constant folding into a consumer | `combine.c` |

**Read the pass before probing it.** Probes are for confirming a mechanism you
have already found, not for discovering one.

## WHY THE BASIC-BLOCK LEVER WORKS -- read from the compiler, not inferred

The lever below was found by experiment over three batches and its limits were
found the same way, one park at a time. All of it is four lines of
`local-alloc.c`, in `update_equiv_regs`:

    if (REG_N_REFS (regno) == 2
        && REG_BASIC_BLOCK (regno) < 0
        && rtx_equal_p (XEXP (note, 0), SET_SRC (set)))
      reg_equiv_replace[regno] = 1;

with gcc's own comment above it:

    If the register is referenced exactly twice, meaning it is set once and
    used once, indicate that the reference may be replaced by the equivalence
    we computed above.  If the register is only used in one basic block, this
    can't succeed or combine would have done it.

`reg_equiv_replace` is what makes the allocator REBUILD a value at its use
instead of keeping it live. Both conditions have to hold, and between them they
predict every result we got:

| observation | which condition |
|---|---|
| separate locals work, one local used twice does not | `REG_N_REFS == 2` -- one pseudo used twice fails it, two pseudos used once each pass |
| a branch is required | `REG_BASIC_BLOCK < 0` is `REG_BLOCK_GLOBAL`, set only when the pseudo appears in MORE THAN ONE basic block (`flow.c`) |
| a straight-line function can never use it | one basic block, so no pseudo is ever GLOBAL. Calls do NOT split blocks in gcc-2.96 |
| it fails inside a loop body | `REG_N_REFS` is incremented by `bb->loop_depth + 1`, so inside a loop a set-once-used-once pseudo counts 4, not 2 |

**THE STRAIGHT-LINE CASE IS NOT AN UNSOLVED PROBLEM. IT IS UNREACHABLE.** There
is no C that satisfies `REG_BASIC_BLOCK < 0` in a function with one basic block,
because the condition is about the control-flow graph and not about the source.
The only other pass that rebuilds a constant at its use is `combine`, and
combine can only fold one into its consumer if the target has an instruction
taking it as an immediate -- which a constant needing two instructions, by
definition, does not.

So the three parked shapes that were filed as "one missing construct" --
`src/non_matching/ovl_77dd1c/200c5b8.c`,
`src/non_matching/ovl_7c7b9c/200c218.c`, and the `-1` triple in
`src/non_matching/ovl_787e04/20093e4.c` -- are not waiting on a construct. In
plain C they are unreachable, and register pinning is the only way through.

**This took ten minutes of reading and it settles three parks and four
empirical limits that cost about six rounds to discover.** The passes table
below is not decoration.

## DO NOT APPLY THE BASIC-BLOCK LEVER PREEMPTIVELY

The lever is for constants gcc will **not** place where the ROM has them. If gcc
already places them correctly, hoisting perturbs what was right.

`Func_80a4754` looks like a textbook case — its last call interleaves
(`mov r2,#1 / ldr r0,=0xb86 / neg r2,r2 / mov r1,#0`) and the call sits inside a
guarded block, so the conditions hold on paper. Applying it is **2 of 36**:
hoisting `n = -1;` above the guards moves `mov r1, #0` one instruction early.
The plain literal `-1` at the call site matches exactly.

**Screen the plain form first.** The lever costs a screen to add and a screen to
discover it was the thing that broke you.

## THE BASIC-BLOCK LEVER: assign the constant where the ROM cannot keep it

**This retires two blocker classes**, one of which had been open for thirty-six
batches. Read it before attempting anything with a displaced argument.

Both classes are the same mechanism seen from two angles:

    arg-interleave      rom  mov r1,#imm / mov r0,#imm / lsl r1,#n
                        ours mov r1,#imm / lsl r1,#n   / mov r0,#imm

    pool-loads-first    rom  mov r0, r5 / ldr r1,=X / ldr r2,=Y
                        ours ldr r1,=X / ldr r2,=Y / mov r0, r5

In both, the ROM's expensive operand is materialised in TWO PIECES with another
argument scheduled into the gap, and gcc emits it in one piece.

**The trigger is not at the call site at all. It is where the value is
assigned:**

| the C | what gcc does |
|---|---|
| literal at the call site | materialises in one piece, contiguous |
| named local, SAME basic block as the call | keeps it in a callee-saved register |
| named local, assigned in a **DIFFERENT basic block** | **rematerialises at the call, split** |

Crossing a basic-block boundary is what stops gcc keeping the value in a
register. It then has to rebuild it at the call, and its rebuild of a
two-instruction constant is the split pair, with the other argument scheduled
into the gap. That is the ROM's shape.

    int x;
    x = 0x88 << 18;          /* assigned here */
    a = __MapActor_GetActor(8);
    if (a != 0)              /* <-- a basic-block boundary */
        ...;
    __Func_8012078(0, x, ...);   /* used here: different block */

Either arrangement works: assign before an `if` and use INSIDE it, or assign
before an `if`/`else` and use AFTER the join. A call does NOT create a boundary
-- only a branch does.

### The INVERSE lever (batch 153): naming it in the SAME block makes it contiguous

The table above says a named local in the same basic block as the call "keeps it
in a callee-saved register". That is not the whole story, and the other half is
a lever in its own right.

Where the ROM builds a two-instruction constant CONTIGUOUSLY and gcc splits it
around another argument's load — the mirror image of arg-interleave — naming the
value in a local **in the same basic block** fixes it:

    rom    mov r1, #0xc8 / lsl r1, #0x4 / ldr r0, =Func_80cc960
    ours   mov r1, #0xc8 / ldr r0, =Func_80cc960 / lsl r1, #0x4     (literal)
    ours   ... identical to the ROM ...                             (named local)

On `Anim_UnleashIntro` this fixed both of its `StartTask` sites at once, 6
differing to 2. So the two directions are symmetric and worth knowing as a pair:

| the ROM | write the constant as |
|---|---|
| split around another argument | a local assigned in a DOMINATING block |
| contiguous, other argument after | a local assigned in the SAME block |
| either, and it already matches | a literal at the call site |

Try the literal first, then read which way the diff points before picking.

### A wrong jump table can look like a register problem

`Anim_UnleashIntro` switches on 0..4 with the fifth case doing what `default`
does. Writing only four labelled cases plus `default` makes gcc emit a
COMPARISON CHAIN, not a table: 59 differing of 80. Adding the redundant
`case 4:` produces the ROM's table and takes it to 6 — **and fixes an r5/r6
swap as a side effect.**

That side effect is the part to remember. A two-register rename is not always an
allocation coin flip; it can be downstream of a control-flow shape that is
wrong. Check the branch structure before parking on registers. Count the ROM's
jump-table entries and write out exactly that many cases, redundant ones
included.

**BOUNDARY (batch 152): a function with no branches is out of reach.** Since a
call does not create a block boundary, a straight-line function has exactly one
basic block and there is no "different block" to assign the constant in. The
lever has nothing to bite on. `OvlFunc_945_200dca4` is eleven calls in sequence
with no condition, loop or early return; it is 2 of 43 on a single
arg-interleave and every spelling of the constant gives the same 2.

This matters because the cutscene scripts in these overlays are mostly
straight-line, and they are exactly the shape that produces arg-interleaves. If
a candidate's only defect is an interleave and the function has no branch in
it, park it and move on rather than working through the spellings -- the fix
above is not available.

**WHAT DOES NOT WORK, and why the class survived so long.** Every lever the tree
had was tried against it and all of them are call-site properties: the callee
declared, undeclared, with widened parameters, with eight different return types
on the preceding callee; `-fno-schedule-insns`, `-fno-schedule-insns2`,
`-fno-peephole`, `-fno-force-mem`, `-fno-caller-saves`,
`-fno-expensive-optimizations`, `-fno-cse-follow-jumps`, `-O1`; and naming the
values adjacent to the call, which is the stack-arg-pair lever and is the exact
OPPOSITE of what is needed here. A previous investigation concluded "the C is
not the variable". It was looking in the right place with the wrong axis.

**IT ALSO DEFEATS CONSTANT-CSE, which is the bigger use.** Where the ROM builds
the SAME value twice and gcc builds it once and copies, giving each occurrence
its own named local in a dominating block makes gcc rematerialise both:

    rom    mov r0,#0xc0 / mov r1,#0xc0 / mov r2,#0x80 / lsl r0,#10 / lsl r1,#10
    ours   mov r1,#0xc0 / lsl r1,#10 / mov r0,r1 / ...          (literals)
    ours   ... identical to the ROM ...                          (two locals)

That reaches a class this document had written off. `OvlFunc_922_2009750` was
parked as a counter-example to the `-fno-rerun-cse-after-loop` rule -- the flag
was byte-identical on it -- and two locals holding the same offset match it
outright, with no flag. `OvlFunc_968_20087d8` goes from 19 disagreeing
instructions to 6 the same way, including the `-1` TRIPLE that
src/non_matching/ovl_787e04/20093e4.c is still parked on.

**So before reaching for CSE_CFLAGS, try separate locals in a dominating block.**
The flag is a build-system change that needs justifying upstream; this is one
line of C.

**IT IS NOT ONLY ABOUT SHIFTED CONSTANTS.** Any two-instruction materialisation
gets split the same way. `OvlFunc_943_2008c28` passes -0xa, which gcc builds as
`mov r2,#0xa / neg r2,r2`, and the ROM splits that pair around the other two
arguments exactly as it splits a `mov`/`lsl` pair. The same lever fixes it. Read
the rule as "a constant that takes two instructions to build", not "a shift".

**EVERY REPEATED USE MUST BE IN A DIFFERENT BLOCK FROM THE ASSIGNMENT.** One
use in the assignment's own block defeats it. `OvlFunc_936_20095b4` passes
`0x80 << 2` to `__GetFlag` in an `if` condition and to `__SetFlag` inside the
body; two separate locals assigned above the `if` leave it exactly where the
literal does, because CSE merges them into one pseudo before local-alloc runs
and that pseudo is then referenced three times -- so `REG_N_REFS == 2` fails.

The cases where it works -- `OvlFunc_892_2008054`, `OvlFunc_959_2008ce0` -- have
ALL the repeated uses inside the conditional block.

**THE DOSAGE IS NOT MONOTONIC — lever the sites that are WRONG, not every site
that could take one.** `Task_BlitAnim` has seven uses of the same `0x4000`:
literals everywhere gives 29 differing, two locals gives 5, three gives 3, and
all seven gives 11. That is the `REG_N_REFS == 2` clause biting from the other
side -- past some point CSE merges the locals back into one pseudo and the
pseudo is referenced too often to be rematerialised.

**IT ONLY DECIDES *WHETHER* A VALUE IS REMATERIALISED, NOT *WHAT* FILLS THE GAP.**
`Anim_UnleashIntro` sits at 2 of 80 with gcc already splitting the pair:

    rom    mov r0, #0xa0 / ldr r3, =Func_8001af8 / mov r2, #0x80 / lsl r0, #19
    ours   mov r0, #0xa0 / ldr r3, =Func_8001af8 / lsl r0, #19 / mov r2, #0x80

Three placements give the same two instructions. Which of the remaining
arguments gcc schedules into the gap is a separate question. Related: if the
ROM's gap swallows EVERY other argument -- `OvlFunc_929_2008598` defers its
`lsl` past all three -- that is the scheduler, and four placements leave it
unchanged at 4 of 55.

**AND IT DOES NOT TOUCH REGISTER CHOICE.** `OvlFunc_956_2008b30` is parked on
which register holds a pooled mask. It has a real boundary; the addend, the
mask, and both together were each assigned above it, and all three are 3 of 47.

**AND IT DOES NOT REACH INSIDE A LOOP BODY.** The assignment has to be in a
block that dominates the call, and in a loop every such block is also reachable
across the BACK EDGE -- so the value is live around the loop and gcc keeps it in
a callee-saved register instead of rematerialising. `OvlFunc_935_2008b8c` is
2 of 51 with the literal at the call site, 7 with the value assigned before the
loop, and 9 with it assigned in the block that jumps into the loop.
So the clause is: a dominating block that is **not part of the loop the call
sits in**.

**THE OTHER LIMIT IS REAL. A straight-line function cannot use this**, because there
is no boundary to put between the assignment and the call. `OvlFunc_882_20083cc`
written this way goes from two differing instructions to four. Of 1,861 remaining
functions carrying one of the two shapes, **417 have a basic-block boundary
before their first site** and are reachable; the rest stay parked.

See reports/arg-interleave.md for how it was found.

## Pool loads come first, but the basic-block lever moves them

Within one argument block gcc-2.96 emits every literal-pool load before any
`mov`, whatever order the arguments are written in. The ROM emits them in
source order. It shows up in two disguises that look like different problems:

    rom    mov r1, #0x66 / ldr r2, =0x4b6 / mov r0, #0
    ours   ldr r2, =0x4b6 / mov r1, #0x66 / mov r0, #0

    rom    mov r0, r5 / ldr r1, =0xcccc / ldr r2, =0x6666
    ours   ldr r1, =0xcccc / ldr r2, =0x6666 / mov r0, r5

The second reads as a misplaced `r0` and so looks like the declaration lever's
class. It is not. **Before spending screens on the declaration lever, check
whether the displaced operands are exactly the pooled ones.** If they are, the
lever will not reach it -- neither will any scheduling flag, nor naming the
values as locals adjacent to the call.

`src/non_matching/overlays/pool_load_first.c` has the full negative result and
the four members. The only construct known to reach this shape is register
pinning with inline asm, which is what `OvlFunc_883_2008fbc` does in
`fakematch.txt`.

**CORRECTED IN BATCH 105.** The basic-block lever DOES reach this shape, whenever
the function has a boundary to use. `OvlFunc_943_2008a48` and
`OvlFunc_943_2008af0` had been parked on

    rom    mov r2, #0 / mov r0, #0x15 / ldr r1, =0x103 / bl __MapActor_Emote
    ours   mov r2, #0 / ldr r1, =0x103 / mov r0, #0x15 / bl __MapActor_Emote

since batch 96, and both close with `0x103` named in a block that dominates the
call. The park had tried naming it INSIDE the else-block, which is the case the
lever's own table says keeps the value in a register.

The inline-asm sentence above still stands for the STRAIGHT-LINE members of the
class, which is what its four catalogued examples happen to be. Check for a
boundary before writing one off.

## The stack-arg-pair lever: name BOTH, adjacent to the call

Where a call takes two stack arguments, the ROM materialises both into separate
registers before storing either; gcc reuses one register and interleaves each
build with its own store:

    rom    mov r3, #0x2d / mov r2, #0x2b / str r3, [sp] / str r2, [sp, #4]
    ours   mov r3, #0x2d / str r3, [sp]  / mov r3, #0x2b / str r3, [sp, #4]

**Name both values as locals, assigned immediately before the call, in the
order the ROM stores them.** Three steps, each of which matters:

| | |
|---|---|
| literals at the call site | 3 positions differ |
| the SHARED value named (it is also an earlier argument) | 2 differ |
| both named, first-stored assigned first | match |

The middle step is the non-obvious one — a value that appears both as a register
argument and as a stack argument has to be named once and used twice, and that
is what frees the register the other stack value needs.

**Adjacency is the whole trick.** `OvlFunc_932_20084cc` was parked on this class
having tried named locals and got *thirteen* differing positions, worse than
literals — because the assignments sat at the top of the function with a call
between them and their use, so gcc hoisted both materialisations above it.
Making values live EARLIER is not the same as making them live SIMULTANEOUSLY.

And note the class is **not** "has stack arguments". Several functions pass
arguments on the stack and need no lever, because the ROM fills the slots first
and then the registers, which is what gcc does anyway.


### One local per independent operation (batch 57)

Recycling a local across two operations the ROM keeps apart shifts the whole
register assignment. `OvlFunc_942_200886c` reads gState twice — different
offsets, different tests — and reusing one offset variable and one value
variable for both is **6 of 39**. Giving each read its own offset, pointer and
value matches exactly.

The base pointer IS genuinely held across both (r5, pushed), so that one stays a
single local.

**Read the ROM for which registers are held and which are rebuilt, then mirror
it.** A local is not free: it is a statement that one value spans both uses.

### Batch 52: assign inside the arm when the ROM builds the pair twice

Batch 49 said to hoist a shared value to a dominating block. That is right when
the ROM builds it **once**. When both arms of an `if/else` pass the *same* pair
and the ROM builds it **fresh in each arm**, the locals must be assigned
**inside each arm**:

    if (t == 0x24) {
        __SetFlag(0x335);
        m = 0x23; n = 0x4d;              /* assigned here */
        __Func_8010704(0x23, 0x4e, 1, 1, m, n);
    } else {
        __ClearFlag(0x335);
        m = 0x23; n = 0x4d;              /* and again here */
        __Func_8010704(0x22, 0x4d, 1, 1, m, n);
    }

`OvlFunc_955_200805c`, three spellings:

| | |
|---|---|
| assigned in each arm | 36 lines, **exact** |
| hoisted above the `if` | 35 lines, 26 differ — gcc materialises once and carries it across the branch |
| bare literals | 36 lines, 6 differ — one register reused for both slots instead of two |

The duplication looks redundant and is what the ROM says. **Count the
materialisations in the reference before deciding where to put the assignment.**

### Two refinements from batch 49

**A value in a CALLEE-SAVED register around a call is shared across both calls.**
`OvlFunc_959_200a26c` keeps `0x15` in r5 and pushes r5 in the prologue, then
stores it into `[sp]` before each of two calls. One local passed as the fifth
argument of both reproduces it. The saved register is the tell — gcc has no
reason to spend a push unless the value has to survive the call.

That also settles the store order for free. Where the shared value goes into
`[sp]` last, immediately before the `bl`, and the per-call value into `[sp, #4]`
early, no reordering of the C is needed; it falls out of which local is shared.

**Do NOT reuse an earlier pair's local for a later shared value.** In
`OvlFunc_901_2008e30` the second call stores one register into both slots.
Writing `n = 3;` and passing `n, n` — recycling the first pair's local —
perturbs the FIRST pair's register assignment three instructions earlier and
comes out 3 of 22. A *fresh* local matches, and so do bare literals.

The two failures look nothing alike: the diff lands before the statement that
caused it. Prefer literals where they match; they are shorter and they cannot
collide with anything.

## Splitting: when the refusal is about the FILE, not the function

`tools/split_s.py` refuses when a local label would cross the boundary it is
about to create. `.L` symbols do not survive into an object's symbol table, so
a label defined in one part and referenced from another is invisible to the
linker — and the failure surfaces much later, looking like a bad
decompilation.

Three of the eighteen `GetEntrances` functions hit this. **Two were cleared and
one was not, and the difference is worth knowing before spending a round.**

**Cleared:** the function returns one of two `.incbin` tables defined in the
same `.s`. C cannot carry an `.incbin` into a translation unit, so the tables
stay in assembly and the labels are exported instead:

    .global .L1b10
    .global .L1c9c

A `.global` emits no bytes. In `rom_7b7f1c` four sibling tables were *already*
exported for the same function's elevated neighbours — these two had simply
never been needed. Verify in two separable steps: `make compare` green after
the export and **before** the split, then green again after.

**Not cleared, until it was.** `rom_7eaf28/ovl_314_c_c.s` refused for two more
rounds on the reasoning that it holds nine functions and 54 local labels with
only 8 exported, so clearing it would take "not two exports but dozens".

**That was an estimate from the file's totals, and it was wrong.** Computing
what actually crosses *that specific cut* gives **one** label. Exporting it
cleared the refusal and the function matched.

**The splitter had already printed the answer.** Its refusal lists every
crossing label by name, and for that file it listed exactly one. The park note
claiming "dozens" was written with that output on screen.

So: when the splitter refuses, **read what it printed**. It now leads with the
count and ends with the `.global` lines to paste, because a wall of detail with
no headline is what invited an estimate in the first place.

A splitter that cut on a label-closed boundary rather than a function boundary
would clear the whole class.

## An add/sub chain on a constant may be gcc's OWN arithmetic

When the ROM walks one register through a function, adjusting it rather than
rebuilding it, that looks like a source variable being reused — and often is
not.

    mov r3,#0xe0 / lsl r3,#1 / add r2,r1,r3 / add r3,#0x41 / str r3,[r2]
    sub r3,#0x39 / add r2,r1,r3 / mov r3,#0x18 / str r3,[r2]

Read as source, that says: an offset `0x1c0`, then the stored value built from
it as `0x1c0 + 0x41`, then the next offset as `0x201 - 0x39`. Writing exactly
that — `v = 0x1c0; q = p + v; v += 0x41; *q = v; v -= 0x39; ...` — is **10 of
15** for `OvlFunc_950_200809c`, because gcc then allocates base and offset to
different registers than the ROM did.

The source is just:

    *(int *)(p + 0x1c0) = 0x201;
    *(int *)(p + 0x1c8) = 0x18;

gcc had `0x1c0` in a register, needed `0x201`, and reached for `add #0x41`
because that is cheaper than a second `mov`/`lsl` pair. **The chain is the
compiler's arithmetic on a constant it already had, not the source's.**

### The test that decides it (refined, batch 56)

**Write the literals first and see whether gcc produces the chain.**

- gcc produces it  →  the chain is gcc's, keep the literals
- gcc emits fresh constants instead  →  the chain was in the source, write it

`OvlFunc_common1_15b8` is the case that settles this. The ROM derives its second
constant from the first with `asr r3, #1`. Written as two literals, gcc emits a
fresh `mov`/`lsl` — so the shift belongs to the source, and writing
`v >>= 1;` takes it from 8 of 34 to 6.

**The mnemonic is not the discriminator; who generates it is.** The paragraph
below reads as though `add`/`sub` means gcc and something else means the source.
That is a useful prior, not a test — one screen with literals answers it
directly.

### How to tell this apart from the real reuse lever

The genuine offset-reuse lever (batch 44, `Func_809b648`) looks similar:

    mov r3,#0x91 / lsl r3,#2 / add r2,r1,r3 / mov r3,#0 / str r3,[r2]

The difference is the **final `mov`**. There, the register is *overwritten* with
an unrelated value (`#0`) before the store — gcc will not do that on its own,
because it had no reason to destroy the offset. Where the register is instead
*adjusted* into the next value by `add`/`sub`, that is strength reduction and
the source had plain literals.

**Try the literal form first.** It is shorter, and being wrong costs one screen.

## Technique: naming an intermediate stops gcc folding it

Two matches so far turn on the same lever — gcc folds a computed value into
whatever consumes it, and assigning it to a named local stops that.

**A byte offset folded into the base pointer.** Where the ROM stores with a
register offset:

    rom    lsl r3, r6, #1 / add r3, #0xd8 / mov r2, r8 / strh r2, [r0, r3]
    ours   lsl r3, r6, #1 / add r0, r3 / add r0, #0xd8 / strh r3, [r0]

one instruction longer. Naming the offset produces the ROM's form:

    int off = (slot << 1) + 0xd8;
    *(short *)((char *)unit + off) = value;

Parenthesising the expression, spelling `slot * 2`, and indexing a `short *`
rebased by `0xd8` all give the folded version. Solved `OvlFunc_924_200cf90`.

### The mov/neg pair is not inherently hard

`OvlFunc_924_200cf90` also emits `mov r7, #1 / neg r7, r7` — the shape recorded
as the `narrow-mask` blocker — **with no help at all**, because its `-1` is
compared twice and gcc keeps it live in a register.

So that blocker is about values used ONCE, not about the pair. Worth checking
before spending a round on it: if the constant is genuinely reused, the pair
falls out.

## Technique: a local keeps a shifted constant's mov/lsl pair together

Where the ROM builds a shifted constant and gcc splits the pair around the
next argument:

    rom    mov r1, #0xc8 / lsl r1, #4 / ldr r0, =SomeFunc
    ours   mov r1, #0xc8 / ldr r0, =SomeFunc / lsl r1, #4

**Assign the shifted value to a local before the call.** gcc then finishes
building it before starting the next argument:

    int prio = 0xc8 << 4;
    __StartTask(SomeFunc, prio);

Spelling the same value as one constant (`0xc80`) does not work, and neither
does hoisting the *other* argument into a local. It has to be the shifted one.

Solved `OvlFunc_956_20081b4`.

**This is not the same thing as arg-interleave, and does not solve it.** There
the ROM wedges a plain `mov` INTO the pair and gcc emits the pair contiguously
— the opposite direction. gcc reaches that shape when control flow separates
each assignment from its use, which cannot be transplanted into a function that
has no branches; see `src/non_matching/ovl_78c76c/20095d4.c`.

### Reading generated output as evidence: exclude fakematches first

A sweep for "does gcc ever emit this shape" reported 335 sites and was
**wrong**. It counted `// fakematch` translation units, which force shapes with
inline-asm barriers and register variables — so their listings are evidence
about the barriers, not about the compiler. The tell is `.code 16` directives
leaking into the output.

Filter on `fakematch.txt` and the `// fakematch` first-line marker before
concluding anything from `asm/**/*.s` that gcc generated.

## Blockers

Every function parked in `src/non_matching/` falls into one of these. They are
listed here because the pattern is more useful than any single case, and
because knowing the class tells you whether a retry is worth it.

### 1. Narrow constant materialisation — **SOLVED, batch 71**

This was recorded as "the single highest-value problem in the project", 34
functions across the overlays and the main ROM:

    rom    mov r3, #0xd / neg r3, r3      (~0xc built at 32-bit width)
    ours   mov r3, #0xf3                  (~0xc narrowed to a byte)

**The fix: stop writing the masking by hand. Declare the field as a bitfield.**

    struct Sprite { unsigned char pad[9]; unsigned char lo : 2, sel : 2, hi : 4; };
    ...
    s->sel = v;

gcc's `store_bit_field` builds the mask, the shift and the merge itself, at int
width, and the constants it generates are independent RTL that CSE never gets
to relate. Confirmed on `OvlFunc_927_20089dc` and its four copies, on
`Func_800c548` / `Func_800c570`, and on `OvlFunc_957_200b610` — first screen in
every case.

**Why it took so long is the useful part.** The old reading was that gcc proves
the loaded value is 0..255 and picks the cheaper 8-bit immediate, so the job was
to make the width unknown. Everything tried followed from that reading and
failed: the value in `s32`, `u32` and `u8` locals; the mask as `0xf3`, `~0xc`,
`-13`, `0xfffffff3` and `~(3 << 2)`; a named-constant mask; an explicit `(s32)`
cast; operands both ways round; eleven statement orders; a shared-constant data
dependency; and seven flags (`-fno-gcse`, `-fno-cse-follow-jumps`,
`-fno-cse-skip-blocks`, `-fno-expensive-optimizations`, `-fno-strict-aliasing`,
`-fno-rerun-cse-after-loop`, `-O1`).

A bitfield WAS among the six width-hiding tricks probed, and was written off,
because it was tried as a way to hide the FIELD's width while the merge stayed
hand-written. That is not what makes it work. **The width was never the thing
to fix — writing the merge by hand was.** When every spelling of an expression
fails, the question to ask is whether the expression should be there at all.

Two secondary levers came out of the same functions:

- **A named temporary can decide the allocation, and so can statement order.**
  In `OvlFunc_957_200b610` the sprite pointer had to be read EARLIER in the
  source than the ROM emits it: statement order fixes register birth order, and
  the scheduler then restores the ROM's emission order. Matching the ROM's
  instruction order in the source is not always how you match its registers.
- **`-fcall-used-r4` is in `GCC296_CFLAGS`.** A ROM function that writes r4
  without pushing it is not hand-written; that park note ("gcc will not produce
  that from any source spelling") was wrong.

### 1b. Halfword constant pooling

The same mechanism at halfword width.

    rom    ldr  r3, =0x400          word-sized pool load
    ours   ldrh r3, .L2 / .word 0x400

camelot-gcc's fingerprint list records that gcc-2.96 pools a small constant
**as a halfword when the target is an `unsigned short`**. The ROM's word-sized
load therefore means the original expression was not `unsigned short`-typed at
that point -- but no formulation found so far reaches that state. gcc narrows
based on the width of the eventual store, and it narrows through `s32` and
`u32` locals, through explicit casts, and through `~mask` constants that do not
fit in 16 bits.

Seen in `BreakItem`, `Func_80108c4`, `Func_8092b54`, and (narrowed to *byte*
width, same mechanism) `Func_800c548` / `Func_800c570`.

**Ruled out — do not repeat this experiment.** Reading the field into an
`s32` local before masking, rather than masking inline, *does* change what
gcc puts in the literal pool: inline gives `.word 1023` and a `mov`/`lsl`
pair for the other mask, while via a local gives `.word -1024` — the ROM's
constant. It changes nothing that matters. gcc still loads it with **`ldrh`**:

    rom    ldr  r3, =0xfffffc00
    ours   ldrh r2, .L6      with  .L6: .word -1024

Same constant in the pool, wrong instruction reading it. The pool content is
a red herring; the load width is the thing, and it follows the width of the
eventual store. Checking the pool and not the instruction is how this looked
solved for about ten minutes.

**PARTLY REACHED, batch 182 -- and the above is the reason the escape is
narrow.** The width follows the eventual store, so anything that keeps the value
inside the store's expression narrows. What does NOT narrow is an `int` local
whose live range crosses a basic-block boundary, because the value has to exist
in a register before the block that stores it:

    int inval;
    inval = 0xffff;              /* the function's FIRST statement */
    ...
    if (v != -1) { ... *slot = inval; }      ->  ldr rN, =0xffff / strh

Assigned inside the arm, or inside the guarded body, gcc folds it straight back
to a HImode `const_int` -1, commons it with the `mov #1 / neg` from the `!= -1`
test, and stores from that register instead -- three instructions short. It is
the DOMINATING BLOCK that does the work, exactly as in the arg-interleave lever,
not the `int` type on its own.

Worth 27 aligned down to 9 on `FieldMove_NoTarget`
(`src/non_matching/rom_8a000/8096810.c`). The corpus template that was already
there and unnoticed is `src/rom_9000/rom_ea54_c_b.c`: `rv = 0xfc88;` at the top
of the function, `p[0xc1] = rv;` in a guarded arm.

Separately, the constant-ZERO case has its own escape and does not need a
dominating block -- see "Halfword constant ZERO" later in this file. Between the
two, 1b is no longer "no formulation reaches it"; it is "the value must live in
a register the store cannot narrow into".

### 2. Register birth order

    rom    ldr r2, =0xd98 / add r1, r3, r2
    ours   ldr r1, =0xd98 / add r2, r3, r1

Every instruction is right; two registers are swapped throughout. gcc-2.96
allocates against `REG_ALLOC_ORDER {3, 2, 1, 0, ...}` in birth order, so
whichever pseudo is created first takes r3, then r2, then r1. One difference in
which subexpression is built first flips the whole function.

**This class is solvable.** `Func_808ed4c` was in it and came out: spelling the
scaled index as `(index << 3)` rather than `index * 8` changed the birth order.
Something equivalent probably exists for the others.

Seen in `Func_8015e8c`, `ModifyHP`, `ModifyPP`, `Func_800c548`.

### 3. Comparison canonicalisation

gcc rewrites `x >= C` as `x > C-1` and `x < C` as `x <= C-1`; the ROM keeps the
original constant. It also folds a run of equality tests into an unsigned range
check where the ROM tests each value separately.

Seen in `Func_80a3ce4` (`cmp #0xc1 / blt` vs `cmp #0xc0 / ble`) and
`Func_8078480` (2, 3, 4, 5 folded to `(kind - 2) <= 3`).

Unfolded compare chains *do* occur elsewhere in this ROM, so some C shape
reaches them.

### 4. Constant and address folding

gcc folds a symbol plus a constant offset into one pool entry (`=gState+586`)
where the ROM keeps two words and adds them at runtime; and gcc pools two
nearby offsets separately where the ROM derives the second from the first with
an `add`.

Seen in `Func_808d5a4` and `Func_8091254`. In the first case this probably
means the object being indexed is not the one currently named.

### 5. Scheduling

A load hoisted one or two instructions from where the ROM has it, with
everything else identical. Nothing tried so far moves it -- not a nested block,
not a named temporary, not reordering the statements.

Seen in `Func_809a44c` and in the original `rom_79338_c_a.s` attempt.

## On `decomp-permuter`

Classes 2 and 3 are exactly what `decomp-permuter` automates, and the
checkout is already in the tree. It is the obvious next lever, and it is worth
reaching for before hand-grinding any more of these.

### Splitting a `.s`: the basename must be unique

The Makefile's rule for a C file is `asm/%.o: src/%.c`, and it writes gcc's
generated assembly to `$(@:.o=.s)`. A C file at `src/<path>/X.c` therefore
produces **`asm/<path>/X.s`** and `asm/<path>/X.o`. Linker scripts reference the
`asm/` object for C sources exactly as they do for hand assembly; `src/.../*.o`
appears only for the hand-written `exports.s`.

The consequence: if you split `asm/<path>/X.s` and name the elevated C
`src/<path>/X.c`, gcc's output **overwrites the very `.s` you split**, silently,
during the build. What you get is a link error that reads like a stale object:

    multiple definition of `OvlFunc_968_20085ac'
    undefined reference to `OvlFunc_968_20085e4'

Both halves of that message come from the same cause -- the asm file now
contains gcc's version of the first function and nothing else.

So when splitting, **give every piece a new name and delete the original**:

    src/<path>/X_a.c     <- the elevated function
    asm/<path>/X_b.s     <- what is left, hand assembly
    (delete asm/<path>/X.s, X.o, X.d)

and replace the single linker line with one per piece, both under `asm/`:

    asm/<path>/X_a.o(.text)
    asm/<path>/X_b.o(.text)

This is what the inherited splits already do -- `rom_799abc` carries
`ovl_30_a_a_a_c_c_c_b` (C) and `_c` (asm) with no `ovl_30_a_a_a_c_c_c.s` left.
`asmfacts.py --orphans` does not catch the mistake, because the reference is
not orphaned; only the build does.

### Lever: make the CONSTANT the destination of an AND/ORR

Thumb's data-processing instructions are two-operand and destructive, so
`and rX, rY` overwrites rX. When the ROM writes

    mov r2, #0xc
    and r2, r3          <- the CONSTANT's register is the destination

and gcc writes

    mov r3, #0xc
    and r2, r3          <- the loaded VALUE is the destination

the fix is compound assignment onto the constant:

    m = 0xc;
    m &= u;             instead of    m = 0xc & u;

**Why it is worth more than one instruction.** With the constant as the
destination it is DEAD after the AND. Left live, gcc will reuse it to build
other nearby constants -- `OvlFunc_957_200b610` needs the mask -13 a few
instructions later, and with 0xc still in hand gcc emitted `sub r3, #0x19`
(0xc - 0x19 = -13) where the ROM has `mov r3, #0xd / neg r3, r3`. Killing the
constant killed the derivation too, and unblocked the instruction ORDER as
well: gcc had been hoisting two `ldrb`s together and stopped. One spelling
change, 8 of 25 down to 3.

**When it does nothing.** The lever needs a LITERAL on one side. If both
operands are registers -- a mask that arrived as a parameter, say -- gcc
canonicalises the AND regardless of which side the source names first, and the
spelling has no effect at all. `Func_8006384` in `src/non_matching/rom_c0/`
is that case: `t = m; t &= v;` produced byte-identical output.

**When it makes things worse.** `OvlFunc_930_2009060` has the ROM shape
`mov r3, #0x2 / orr r3, r2` inside each of two mutually exclusive branch arms.
There the compound form went from 11 of 25 to 13 -- it fixes the ORR and
perturbs the surrounding allocation by more than it gains. Straight-line code
is where this pays.

So: try it when the ROM shows a literal as the destination AND the code is
straight-line. Screen it, do not assume it.

### Lever: a DERIVED initialiser forces the pointer copy gcc coalesces away

When the ROM loads a pointer into one register and copies it into another
before walking it:

    ldr r3, [r3, #0x0]
    mov r5, r3              <- the copy gcc will not emit
    add r5, #0x48

writing the obvious two statements does NOT reproduce it:

    q = p;                  /* gcc coalesces q with p and loads straight */
    q += 0x48;              /* into r5, dropping the copy entirely       */

Write the initialiser as a single DERIVED expression instead:

    q = (unsigned char **)(p + 0x48);

Now `p` and `q` are different values, the copy is real, and the `add` follows.
That took `Func_80a9cbc` from 2 of 28 to an exact match with no other change.

**The initialiser must be an expression, not an alias.** `Func_8078550` needs
the same copy and cannot get it, because there the second pointer is the same
address as the first -- a bare `w = buf;` alongside `Func_80796c4(buf)`. gcc has
no reason to keep two names for one value, and the attempt made it worse (8 of
27 to 10). If the two pointers genuinely hold the same address, this lever has
nothing to work with.

Note the pairing with the existing pointer-walk lever: a destructive `+=` gives
you the WALK, but only a derived initialiser gives you the COPY that precedes
it. They are two halves of the same shape and you often need both.

### The constant-as-destination lever applies to MUL as well

`mov r3, #0x64 / mul r3, r0` is the same shape as the AND/ORR case:

    x = 0x64;
    x *= r;                 /* not  x = 0x64 * r;  */

Same reasoning -- Thumb's `mul` is two-operand and destructive, so whichever
operand the source makes the destination is the one that survives.

### Lever: `do/while` puts the conditional on the BACK EDGE

When the ROM's loop ends with the continue-test jumping BACKWARD and an
unconditional jump falling out:

    .L103e:  ...
             bgt .L103e      <- conditional, backward
             b   .L1056      <- unconditional, out

a forward `goto` will not produce it. Written as

    if (w > v) goto loop;
    goto join;

gcc INVERTS the test and emits `ble join / b loop` -- conditional forward,
unconditional backward. Same instruction count, opposite shape.

Write it as a `do/while` whose condition is the continue-test instead:

    do {
        if (i == 0) goto zero;   /* a top-of-iteration exit is fine inside */
        ...
    } while (w > v);
    goto join;

That took `OvlFunc_964_2009038` from 2 of 26 to an exact match, and the same
function from 9 of 26 earlier, because the `while` form also let gcc merge the
in-loop load with the loop-expired one. Spelling both paths out separately keeps
both loads AND gets the branch direction.

Note a loop variable may be genuinely uninitialised on the early-exit path --
`w` is, above. That is not a bug to fix: the ROM leaves through the same path
without reading it. Initialising it to silence the thought adds an instruction.

### Lever: the POINTER-TYPED operand comes first in `[rA, rB]`

Thumb's register-offset addressing prints as `ldr r0, [rA, rB]`. Which register
lands in the rA slot is decided by WHICH SOURCE OPERAND IS THE POINTER, not by
the order of the addition.

    rom   ldr r0, [r5, r7]      r5 = the walking offset, r7 = the loaded base
    ours  ldr r0, [r7, r5]

Reversing the addition in the source -- `off + base` rather than `base + off` --
**does nothing**; gcc canonicalises pointer-plus-integer and the output is
byte-identical. Change the TYPES instead:

    unsigned int   base = (unsigned int)iwram_3001f2c;   /* base as an integer */
    unsigned char *w    = (unsigned char *)0x8a;         /* offset as a pointer */
    ... *(void **)(w + base)

That was the only difference between `Func_801ff14` at 2 of 29 and an exact
match. It reads oddly, but the ROM's addressing says which variable the original
source treated as the pointer, and the walking one usually is.

**The other direction** -- a named pointer instead of register-offset addressing
-- is a separate fix for a separate shape:

    rom   add r3, r2 / mov r2, #0x14 / strh r2, [r3, #0x0]
    ours  strh r3, [r1, r2]

There, naming the sum (`q = g + k; *(short *)q = t;`) makes gcc compute the
address and store at offset zero. `OvlFunc_939_2008ac4` needed this one.

### Splitting again: a `.L` label in `.rodata` needs `.global`

A second split of an already-split file can separate a function from the
`.rodata` it references. Local labels do not cross object boundaries, so the
link fails with

    stage1.o: in function `Func_801ffd8':
    (rom_15000+0xb080): undefined reference to `.L73854'

even though nothing about the source changed. Add `.global` to the label in the
piece that DEFINES it:

    .section .rodata
    .global .L73854

    .L73854:
        .incrom 0x73854, 0x73864

This is the existing convention -- `asm/overlays/rom_7b9cb4/ovl_30_c_c.s` already
carries `.global .L5238` for the same reason, and elevated C reaches such labels
with `extern unsigned char L5238[] __asm__(".L5238");`. Symbol binding is
link-time metadata, so the emitted bytes do not change and `make compare` still
passes.

Check for this BEFORE splitting: if the piece you are cutting away references a
`.L` label defined in the part that keeps the data, export it in the same edit.

### Lever: a plain `return K;` per path, not a result variable

When several paths return different constants and the ROM re-materialises the
value on each path:

    mov r0, #0x1 / cmp r3, #0 / beq .Lexit      <- set, then branch to the shared exit
    ...
    mov r0, #0x1 / b .Lexit                     <- set again on the other path
    .Lzero: mov r0, #0x0
    .Lexit: pop {...}

do NOT write it with a result variable:

    r = 1;
    if (t == 0) goto out;
    ... call ...
    r = 1;
    goto out;
    zero: r = 0;
    out: return r;

gcc sees ONE value live across the call and parks it in a callee-saved register
(`mov r7, #0x1`), which is both wrong and more expensive. Write two plain
`return 1;` statements instead. **gcc merges the epilogues by itself** -- the
shared exit the result variable was trying to build by hand appears anyway, and
the constant is re-materialised per path exactly as the ROM does it.

`Func_80bf3bc`, 8 of 31 down to 2 on that change alone.

The general point: a result variable is a way of describing control flow that
gcc already performs. Reach for it only when the returned value is genuinely
computed rather than chosen from constants -- see the offset-variable-reused
lever, where a variable IS the right answer because the value comes from a
load.

### Lever: assign back into the PARAMETER when the ROM's load is destructive

When the ROM overwrites an argument register with something loaded through it:

    ldr r0, [r0, #0x50]        <- r0 was the parameter, now it is the result

do not introduce a local for the result. A fresh local gets a fresh register,
and under any pressure at all gcc will reach for a callee-saved one and add a
push. Assign back into the parameter instead:

    a = *(unsigned char **)(a + 0x50);

`Func_800c570` went from 8 of 21 to 1 on that single change.

The same function shows the companion rule for arguments that are VALUES rather
than pointers: `f &= 1;` on the parameter, not `m = f; m &= 1;`. The copy is
real -- it emits `mov r2, r1` -- and the ROM ANDs the incoming register in place.

Both are the same idea. A parameter is already in a register the ROM is willing
to clobber; naming a local says "keep the original too", and the ROM usually was
not keeping it.

### Lever: invert the guard so the BODY is the taken branch

When the ROM reaches a short return block by branching FORWARD to the end:

    cmp r5, #0 / beq .Lzero
    <the whole body>
    mov r0, r5 / b .Lexit
    .Lzero: mov r0, #0
    .Lexit: pop

an early return will not produce it. `if (p == 0) return 0;` followed by the
body puts `mov r0, #0` AT THE GUARD, and every label after it shifts. So does
the same shape written with a `goto`, and so does a result variable with a
single exit (11 of 41 on the function below).

Put the body inside the guard instead, with the short return after it:

    if (p != 0) {
        <body>
        return p;
    }
    return 0;

Identical control flow, opposite layout -- and the layout is the one the ROM
has. That elevated four `__CreateActor` wrappers at once in batch 65.

**This corrects an earlier claim.** Three parks recorded that basic-block
placement is decided after the source has had its say and cannot be reached from
C. What had actually been tested was a `goto` spelling of the same early-return
shape, which changes nothing. Inverting the guard is a different edit and it
does reach it.

**It is not universal.** `OvlFunc_945_20080fc` has a comparison chain on one side
of the guard rather than a single return, and inverting there does not move the
block. The lever wants a SHORT return block on one side and the bulk of the
function on the other.

### Refinement: inline the ldrsh zero when another zero is stored later

Thumb `ldrsh`/`ldrsb` have no immediate-offset form, so a zero offset must live
in a register. The usual advice in this file is to name it:

    o = 0;
    v = *(short *)(g + o);

That is right **when it is the only zero in the function**. When a zero is also
STORED later, gcc merges the two: it keeps the offset alive, reuses it for the
`strb`, drops the ROM's separate `mov r3, #0`, and pulls a callee-saved register
into the prologue to hold it. Nine instructions differ on
`OvlFunc_921_200816c` for that one merge.

Inline the cast instead:

    v = *(short *)(g + (unsigned int)0);

That still forces the register-offset form -- the ISA leaves no choice -- but
gives gcc no named value to reuse. `OvlFunc_921_200816c`: 9 of 46 to exact.

**Writing the store as a literal does not help.** `*t = 0;` instead of
`z = 0; *t = z;` is byte-identical, because the merge happens on the OFFSET
side, not the store side.

**And it does not fix a folded address.** `OvlFunc_922_2009a34` looks similar in
the diff -- a register-offset load where the ROM computes an address first --
but there the problem is gcc folding `add r3, r5, r1` into the load, not a
reused zero. The same edit is byte-identical there. Two different defects with
the same shape in the diff; check which one you have before reaching for this.

### The signed lower-bound floor applies only to IMMEDIATE comparisons

Batch 55 established that gcc-2.96 rewrites every signed LOWER bound to
`cmp #(K-1) / ble` where the ROM has `cmp #K / blt`, and that this is
one-directional with a two-line minimum. That is true **for comparisons against
an immediate**.

It does not apply when the bound is a SYMBOL. `(int)(&_AREA_7e)` is not a
compile-time constant gcc can decrement, so the comparison is
register-to-register:

    cmp r2, r3
    blt .Lout

and gcc emits `blt` exactly as the ROM does. `OvlFunc_946_2008d48` range-tests
`_AREA_7e <= area <= _AREA_86` and matches exactly.

**Consequence for candidate selection.** A filter that drops every function
containing a signed range branch is too coarse -- it should drop only ranges
against immediates. That mistake hid this function for a round.

**Note the second half still needs care.** Two out-of-range paths written as two
`return` statements get merged into one block that gcc places differently, six
instructions' worth. Send both to a shared `goto out;` label instead.

### `include/dma.h`: pick the helper by WHERE the fill value is stored

Two helpers are now confirmed to reproduce the ROM exactly, and they differ in
one visible way.

`DMA3_SET(src, dst, cnt)` takes a pointer you have already filled:

    zero = 0;                       /* stored wherever gcc likes  */
    DMA3_SET(&zero, p, cnt);

`DMA3_FILL(dst, value, size)` stores the value THROUGH the register it binds to
r0, which the ROM shows as:

    mov r0, sp / mov r3, #0x0 / str r3, [r0, #0x0]

If the ROM stores through a register it has just set to `sp`, that is `DMA3_FILL`.
Using `DMA3_SET` plus a separate assignment gets the transfer right and those
three instructions wrong -- `Func_80a1090` went from 15 of 25 to 9 on the swap.

**Decoding the size argument:** `DMA3_FILL` builds the count as
`0x85000000 | (size / 4)`, so a ROM count word of `0x8500029c` means
`size = 0x29c * 4 = 0xa70`.

The header itself is not a blocker -- that class was retired in batch 65 after
re-screening all five parks filed under it. Picking the wrong helper is.

### Why register-allocation diffs are usually a floor

gcc-2.96's ARM back end allocates in the order given by `REG_ALLOC_ORDER`
(`config/arm/arm.h:989`): **r3, r2, r1, r0, then r12, r14, then r4 onward**. The
comment there explains the choice -- r3 is least likely to hold a parameter, and
results come back in r0.

The ROM's code repeatedly reaches for r4-r6 where gcc picks r0-r3. Since the
order is a compiler constant and not a function of the source, no spelling moves
it. That is why the following have all been tried across many parks and are all
byte-identical:

- naming extra locals, or removing them
- reordering declarations
- reordering the statements that produce the values
- copying a parameter into a local before use

**Recognising the floor early saves a round.** If the two streams have the same
mnemonics and immediates in the same order, and differ only in which register
each value sits in, stop. Record the count, note that the structure is exact,
and move on. The exceptions worth one attempt are the shapes that are NOT pure
naming: an elided copy that a derived initialiser can force
(`q = (T *)(p + K)`), and a register-offset load that a named pointer can turn
into an add -- both documented above, both with their limits measured.

### The pool tell also appears as an UNFOLDED SUBTRACTION

The familiar form is a small constant pooled where `mov #imm8` would do. There
is a second form worth recognising:

    ldr r3, =0xd2e / ldr r2, =0xd24 / sub r3, r2 / add r0, r3

Four instructions to add ten. **No compiler leaves `0xd2e - 0xd24` unfolded.**
Both operands were symbols, and written as literals gcc folds the difference and
emits `add r0, #0xa` -- so the function comes out several instructions SHORT,
which reads like an optimiser-proved-it floor and is not one.

Same mechanism as the per-area flag run `0x8c8 + (area - 0x7e)`. If a function
is mysteriously shorter than the ROM around some arithmetic on pooled
constants, check whether the ROM is computing something a compiler would fold.

**Finding the namespace: follow the result, not the value.** `Func_80b2884`'s
result flows to `_Func_8017658`'s first argument in an already-elevated caller,
and elevated code elsewhere passes `(int)&_MSG_14` there -- so the operands are
message ids. `_MSG_d21` turned out to be already defined, one of the same run.
Values collide across namespaces (95 of them do); consumers do not.

**A named temporary can decide the allocation.** `d = X - Y; base += d;` puts
the two pooled loads in the opposite registers from the ROM. `base += X - Y;`
matches exactly. Worth trying both when a subtraction's operands land swapped.

### `-fno-strict-aliasing`: a per-TU flag, and how to recognise it

Symptom: everything matches except that a LOAD has been hoisted above a STORE,
usually into a load-use stall a few instructions earlier.

    ours   ldr r3, [r0, #0x1c] / ldr r1, [r0, #0x50] / add r3, r2 / str r3, [r0, #0x1c]
    rom    ldr r3, [r0, #0x1c] / add r3, r2 / str r3, [r0, #0x1c] / ldr r1, [r0, #0x50]

Moving a load across a store is legal only if the two provably cannot alias. At
-O2 gcc-2.96 turns on `-fstrict-aliasing`, so a POINTER load and an INT store to
the same object are in different alias sets and the post-reload scheduler is
free to reorder them. `-fno-strict-aliasing` makes it assume they may alias and
the order stands.

**Do not reach for it first.** `-fno-schedule-insns2` is the wrong knob here:
the same scheduling pass is also what produces the ROM's src-before-dst load
order inside each `a += b`, so turning it off breaks more than it fixes. The
pass is wanted; only its alias information is not.

**It is per-TU, measured.** Adding `-fno-strict-aliasing` to `GCC296_CFLAGS` and
rebuilding all 5336 objects generated from `src/` leaves **2631 bytes** differing
across the ROM. Six TUs want it; most do not.

### Literal pool ORDERING is a blocker the screen cannot see

`tools/tryc.py` normalises pool loads to `=value`, so a function whose pool sits
at a different distance still compares equal. It prints a warning saying so.
**Treat an `OK` carrying that warning as provisional until `make compare`.**

`Func_80ad5b4` is the first case: every instruction matches and the build fails
on ten bytes, because the ROM's pool is `[0xffff8000][iwram_3001f2c]` and gcc
emits `[iwram_3001f2c][0xffff8000]` — reference order, which is what
`add_minipool_forward_ref` (arm.c:4820) produces, since entries are kept sorted
by `max_address` and the earlier-referenced fix is the more constrained one.

A sweep of every gcc-generated `.s` in the tree finds **zero** literal pools that
mix a symbol with an integer constant. Every matched TU's pool is all-symbols or
all-constants, where the ordering cannot be observed. Nothing in the source
controls it — pool layout is decided in `machine_dependent_reorg`, after the
insn stream is final.

### A dead `mov rN, r14` right after `push {lr}` means an uninitialised read

gcc-2.96 emits it when a pseudo is live-in with no reaching definition: the
pseudo takes a hard register from `REG_ALLOC_ORDER` (r14 is sixth) and reload
materialises the copy. The corpus has 211 `mov rN, r14` instructions and exactly
one in that position — `GetUnit`. The other 210 are r14 used as ordinary scratch
after being saved, which is normal.

### r4 is caller-saved BY FLAG, not by nature

Functions that write r4 without pushing it are not hand-written and not
disassembly artifacts: `GCC296_CFLAGS` carries `-fcall-used-r4` (Makefile line
113). Batch 68 recorded the observation with the wrong explanation.

### A read-modify-write on a few bits is probably a BITFIELD

Symptom: a mask/shift/merge sequence comes out one instruction SHORT because gcc
derives the second constant from the first.

    rom    mov r3, #0x3 / and r1, r3 / mov r3, #0xd / neg r3, r3
    ours   mov r3, #0x3 / and r1, r3 / sub r3, #0x10

`-13` is `3 - 16`, so once 3 is live gcc gets the second mask in one insn. Move
the negation earlier and the derivation stops, but the two constants then need
separate registers and the pointer is pushed out of r0 -- the two orderings
cannot both be had.

Written as a bitfield store --

    unsigned char lo : 2, sel : 2, hi : 4;
    ...
    s->sel = v;

-- gcc's `store_bit_field` expands the mask, the shift and the merge itself, and
the two constants are generated as independent RTL that CSE never relates. Exact
on the first screen, and it is the more plausible source anyway.

**No flag substitutes for it.** `-fno-gcse`, `-fno-cse-follow-jumps`,
`-fno-cse-skip-blocks`, `-fno-expensive-optimizations`, `-fno-strict-aliasing`,
`-fno-rerun-cse-after-loop` and `-O1` were all probed and none stops the
derivation. This is not a flag question; the two spellings give gcc different
material.

Worth trying whenever a function's whole body is `x = (x & ~M) | (v << S)`.

### Two flag facts read off the ROM, not guessed

Both are visible in the ROM's own bytes and neither needs a screen to establish.

**`pop {r4, r5, r6, pc}` means the TU was built WITHOUT `-mthumb-interwork`.**
With interwork gcc emits `pop {r4, r5, r6} / pop {r0} / bx r0`. The Makefile's
`COMMON2_CFLAGS` rule already drops interwork for `common2_c%` — but
`common2_a` needs it too, which the wildcard does not cover.

**A prologue that pushes r4 and keeps a value in it ACROSS A CALL means the TU
was built WITHOUT `-fcall-used-r4`.** That flag is in `GCC296_CFLAGS`, so gcc
cannot keep anything in r4 across a call; it reaches past to r8 and spends four
instructions saving and restoring it. `-fcall-saved-r4` took
`OvlFunc_common2_28c` from 33 differing lines to 4.

**Swept, and the sweep bounds the claim.** All 164 parks re-screened with
`-fcall-saved-r4`: it improves **eight** and matches **none**.

    ovl_77dd1c/200c5b8.c   36 -> 21     rom_a1000/80a9d84.c    33 -> 15
    rom_9000/HeightTile_4  22 -> 16     rom_a1000/rom_ad608.c  38 -> 28
    ovl_common/common2_254 22 -> 21     rom_b5000/rom_c00d8.c  33 -> 32
    ovl_7c6bac/200851c.c  150 -> 143    rom_f9000/rom_f92fc.c 143 -> 141

So the r4 question is real and wider than one file, and flipping the flag is not
by itself an answer anywhere else. Read the prologue before reaching for it.

### The mask's WIDTH tells you which spelling the original used

"Use a bitfield" is not a blanket rule. `OvlFunc_927_2008a4c` writes two masked
fields and needs opposite spellings for them:

    sprite +9    mov r3, #0xd / neg r3, r3     32-bit mask  -> BITFIELD
    actor +0x23  mov r3, #0xfe                 byte mask    -> HAND-WRITTEN

gcc's `store_bit_field` works at int width, so a bitfield always produces the
`mov rN, #K / neg` pair. A bare `mov rN, #0xNN` byte mask is what you get from
masking a plain `unsigned char` by hand. Declaring the second field as a
bitfield costs one extra instruction; hand-masking the first costs the whole
constant-derivation problem.

Read the width off the ROM before choosing.

(`| 2` rather than `& ~2 | 2` for a one-bit set is gcc simplifying, and is not
evidence either way.)

### An argument PERMUTATION is not the argument-precompute blocker

`OvlFunc_927_2008a4c` calls `__CreateActor` with its own arguments rotated,
`(d, a, b, c)`, and gcc reproduces the ROM's seven-move shuffle through
r4/r5/r6 exactly, with no help at all. The argument-precompute class
(`calls.c:805`) is about a call whose arguments mix cheap constants with two or
more expensive values — not about permutation. Do not park a rotation on sight.

### Read the state as a STRUCT, not as pointer arithmetic

Thumb `ldrsh` has no immediate-offset form, so a signed halfword read always
needs an index register. That gives gcc a choice, and the two spellings pick
differently:

    gState.area                     ->  add r3, r1, r0 / mov r0, #0 / ldrsh r3, [r3, r0]
    *(short *)(base + k + (u32)0)   ->  ldrsh r3, [r6, r2]

Written as arithmetic, gcc folds the addition into the load and makes the index
carry the offset — one instruction shorter than the ROM. Written as a struct
member, with the offset inside the TYPE, it has to materialise the address and
supply a zero index, which is what the ROM does.

This unparked `OvlFunc_922_2009a34` (18 of 50 → exact) and was first-screen for
`Func_8096ab0`, `OvlFunc_888_2008070` and `OvlFunc_937_20080e4`.

**It is not the same as the inline-zero lever.** Writing the zero as
`(unsigned int)0` settles *which zero wins* when two compete; it does nothing
about whether the address gets folded. Both notes exist because the diffs look
alike.

**gcc derives the second offset by itself.** Two members at 0x16c and 0x1c0 come
out as `mov r1, #0xb6 / lsl r1, #1 / … / add r1, #0x54` with no help — the same
constant-derivation peephole that blocks the bitfield cases. Do not force it.

### A `switch` reproduces the ROM's jump table

`OvlFunc_888_2008070`'s `.s` carries a 35-entry `.word` table and a `mov pc, r3`.
A plain C `switch` produced it exactly — the range check `sub r3, #1 /
cmp r3, #0x22 / bhi`, the `.align 2, 0`, and every entry in order — with nothing
done to provoke it. The case values are dense enough that gcc-2.96 chooses a
tablejump on its own.

Worth knowing before hand-writing an if-chain to imitate one: a jump table in
the `.s` is not evidence of hand-written assembly.

## REGISTER ALLOCATION: it is systematic, and it is not reachable from C

This is the terminal blocker -- once a function's semantics are right, what is
left is which register each value lives in. `tools/reg_map.py` measures it
rather than describing it: for every parked `.c` whose stream is the same
length as the ROM's, it aligns the two, keeps only the instruction pairs that
already agree in mnemonic and shape, and reads off the register correspondence.

**41 same-length parks differ in registers. 26 of them are a clean
permutation** -- every ROM register maps to exactly one of ours and back, so the
function differs ONLY in naming. The other 15 are conflicting maps: same
instructions, different value layout, which is not a renaming and will not
respond to a renaming fix. That split is worth more than the totals.

**The permutation is dominated by ONE adjacent transposition:**

    rom r2 -> ours r3    9        rom r1 -> ours r0    5
    rom r3 -> ours r2    9        rom r0 -> ours r1    3

Eight functions are exactly `r2<->r3` and nothing else.

`REG_ALLOC_ORDER` (arm.h:989) is `{3, 2, 1, 0, 12, 14, 4, 5, 6, 7, ...}`. gcc
gives the first pseudo it allocates r3, the second r2, the third r1, the fourth
r0. **The observed data is what you would see if the original compiler's order
began `{2, 3, 0, 1, ...}`** -- the same registers, the first two pairs swapped.
That accounts for 26 of the ~40 correspondences. The remainder (`r1->r2`,
`r1->r3`, `r4->r1`) do not fit one global order and are probably cases where the
two compilers created the pseudos in a different order to begin with.

### What does NOT reach it

- **Birth order in the source.** It works when the two values come from separate
  STATEMENTS -- that is what fixed `OvlFunc_957_200b610` in batch 71. It does
  nothing when they are operands of ONE statement: naming the pointer, naming
  the zero, or both, in `a[0x5b] = 0`, all give byte-identical output, because
  gcc's expander fixes the order before any pseudo exists.
- **Flags.** Seven probed on a clean `r2<->r3` park -- `-fno-regmove`,
  `-fno-caller-saves`, `-fno-function-cse`, `-fno-defer-pop`, `-fno-peephole`,
  `-fno-delayed-branch`, and the baseline -- all byte-identical. gcc-2.96 does
  not expose allocation order as an option.

### THE EXPERIMENT WAS RUN, AND THE HYPOTHESIS IS REFUTED

`/opt/camelot-gcc/` in the build image ships the gcc-2.96 source with a working
`build.sh`, and the image has a host toolchain. `REG_ALLOC_ORDER` was patched to
`{2, 3, 0, 1, 12, 14, 4, 5, ...}`, `cc1` rebuilt, and the result mounted over
`/opt/gcc296/cc1` in a throwaway container -- nothing in the repo or the image
was changed.

**On eighteen register-allocation parks: 5 improved, 11 got WORSE, 2 unchanged,
and ZERO matched.**

    80c23a0      4 of 16  ->   2      2008d68      2 of 22  ->   6
    rom_c0cc     7 of 20  ->   4      2009458      3 of 36  ->   8
    rom_e3a3c    5 of 39  ->   3      rom_15e8c    7 of 21  ->  12
    800fa8c     20 of 28  ->  17      808ddb8     12 of 26  ->  17

**The control settles it.** Building the WHOLE ROM with the patched order leaves
**724,691 bytes** differing. The stock `{3, 2, 1, 0, ...}` is unambiguously the
right order for this corpus.

### What that means -- the class is SOURCE-shaped, not compiler-shaped

If the allocator's order were wrong we would expect a uniform improvement. We
got a trade: some functions want the swap and more do not. So the `r2<->r3`
transposition is **not** an allocator-configuration difference.

What is left is pseudo CREATION order. gcc hands out registers in the order
pseudos come into existence, so a function whose values are born in the other
order gets the other registers -- and creation order is a property of the
SOURCE, not of the compiler. That is why `OvlFunc_957_200b610` responded to
reading its sprite pointer one statement earlier, and why naming operands
*within* one statement does nothing: no pseudo exists yet to reorder.

**So this class is reachable in principle**, and the lever is to make the value
the ROM allocates first be *created* first -- which needs the two values to
originate in separate statements. Where they are operands of a single
expression, they cannot be separated without changing what the expression is,
and those are the genuine floors.

Do not re-run the compiler experiment. It is recorded here with its numbers so
that it does not get proposed again.


### Putting an offset in the TYPE also stops the constant-derivation chain

Batch 72 used struct members to fix an `ldrsh` addressing form. `Func_80173ac`
shows the same lever doing a second job. Written as pointer arithmetic, five
halfword stores at fixed offsets fail twice:

    ours   ldrh r3, .L0        the constant pooled AS A HALFWORD, because the
    rom    mov  r3, #0xf       store is to a `short`

and, once an int local fixes that, gcc DERIVES each offset from the last --
`sub r0, #0x6` to get from 0xeae to 0xea8 -- where the ROM loads each from the
pool. No arrangement of int locals stops the derivation.

Declared as struct members at their real offsets, both problems disappear at
once: each member's address is generated independently, so there is nothing to
derive from, and the stored constants come out as immediates.

**The general form:** an offset written in the arithmetic is a value gcc can
fold, derive and narrow. The same offset written in the type is not a value at
all. When a function's residue is constants behaving oddly, check whether the
offsets could be members.

### Check the branch SUFFIX on a loop bound

`OvlFunc_911_20080a0` came down to one instruction out of twenty-three:

    rom    bls        ours   ble

`int i` with `i <= 8` gives the signed `ble`; the ROM's `bls` means the counter
was **unsigned** in the source. Invisible unless you read the suffix, and it is
the first thing to check before diagnosing anything else about a loop.


### `do { } while (0)` around a copy-to-RAM wrapper is LOAD-BEARING

`src/rom_c0/rom_52f4.c` -- which already matches -- writes every one of its
copy-to-RAM wrappers with the allocation, the DMA and the free inside one
`do { } while (0)`, and the call inside a second nested one. It reads like a
leftover macro expansion and is easy to write off as noise.

It is not noise. Written flat, the ROM's argument saves and its size load come
out in the other order:

    rom    mov r8, r0 / mov r10, r1 / ldr r5, =SIZE
    ours   ldr r5, =SIZE / mov r8, r0 / mov r0, r5 / mov r10, r1

Four differing lines. With the wrappers, nothing else changed, both
`HuffStr_Start` and `Func_8003e10` are exact. The block scope decides when the
incoming arguments become pseudos relative to the size constant. A named local
for the argument does NOT substitute -- gcc coalesces it away.

### "No pool warning" is not evidence of no pool problem

`tools/tryc.py` warns when the reference keeps a literal pool inside the
function. `Func_801edec` has exactly that shape -- a `b` over a mid-body `.word`
and a `.pool` -- and **the warning did not fire**. The screen reported 45 lines
against 45 with one cosmetic difference, and the build failed `make compare` by
**323,730 bytes**, because the translation unit came out a different size and
everything after it shifted.

The first differing bytes were not in the function at all: they were in the
rom_15000 exports table, whose veneers carry pooled addresses of functions that
had moved.

**The only test that decides pool layout is `make compare`.** Treat a clean
screen on a function whose reference has any inline `.word` or `.pool` as
unproven, warning or no warning.

### A named intermediate forces the THREE-operand form

Thumb has both `add rd, rn, rm` and `add rd, rm`, and gcc picks the short form
whenever the destination is also an operand. When the ROM has the long form and
you have the short one, give the subexpression its own statement:

    DMA0_SET((char *)b + i * 4, ...)     ->  add r0, r4        (ours)

    off = i * 4;
    src = (char *)b + off;               ->  add r0, r4, r0    (the ROM's)
    DMA0_SET(src, ...)

Scaling into the variable itself (`i = i * 4;` then `b + i`) works equally well.
What matters is that the addition is a statement rather than a subexpression of
a call argument.

**It does not always work, and the difference is instructive.** The same trick
fails on `Func_80b06c0`'s `lsl r3, r1, #4` -- there the shift's source is dead
immediately after, so gcc allocates the named local to the same register and the
short form is still correct. The lever needs the two values to be
simultaneously live, which a pointer base and its offset are and a shift's
input and output are not.

### Arithmetic narrows to the width of its STORE unless a local says otherwise

Batch 71 needed an `int` local to stop a CONSTANT narrowing. `OvlFunc_911_20080cc`
needs one to stop the ARITHMETIC narrowing:

    if (--a->f64 == 0)     ->  ldr r3, =0xffff / add r2, r3 / and r3, r2

    t = a->f64 - 1;            (int)
    a->f64 = t;
    if ((unsigned short)t == 0)
                           ->  sub r3, #1 / strh / lsl r3, #16 / cmp r3, #0

Same cause seen from the other side: gcc works in the width of the eventual
store. Do the arithmetic at int width and put the truncation only where the
value is TESTED.

## A switch DECISION TREE means three or more case labels

gcc-2.96 lowers a `switch` two different ways, and which one it picks is a
reliable read on how many case labels the source had.

**Two labels** gives a plain equality chain, one `cmp`/`beq` per case and a
fall-through default:

```
	cmp	r5, #0xc
	beq	.Lcase_c
	cmp	r5, #0xd
	beq	.Lcase_d
	<default>
```

**Three or more** gives the balanced decision tree, whose signature is a
constant compared TWICE in a row -- once against `beq` and once against a
relational branch that leaves for the default:

```
	cmp	r5, #0xd
	beq	.Lcase_d
	cmp	r5, #0xd	<- the same constant again
	bgt	.Ldefault
	cmp	r5, #0xc
	bne	.Ldefault
	<case 0xc, reached by fall-through>
```

So a repeated `cmp` against the same immediate, feeding first an equality
branch and then a `bgt`/`blt`, is not redundant code and is not something to
try to spell away. It says the switch has at least one more case label than
the number of distinct bodies visible in the ROM -- the extra label shares a
body with another case or with the default, which is why it leaves no trace of
its own.

Measured on OvlFunc_971_200906c (src/non_matching/overlays/200906c.c): the
two-case switch gives the chain, and adding a third case whose value equals the
default's reproduces the tree exactly, register for register.

## The QUADRANT facing test -- a fourth spelling for batch 29's table

The three facing tests catalogued in reports/batch-29.md are all RANGE checks,
`f - k <= n`. There is a fourth, and it is an equality on masked bits:

```
	ldrh	r3, [r0, #6]
	mov	r2, #0x80 / lsl r2, #6	<- 0x2000, half a quadrant
	add	r3, r2
	ldr	r2, =0xffffc000
	and	r3, r2
	lsl	r3, #16
	mov	r2, #0xc0 / lsl r2, #24	<- 0xc0000000
	cmp	r3, r2
	bne	...
```

Rotate the angle by half a quadrant, mask it down to its top two bits, and ask
which quadrant it landed in. Two things about the source are forced, and both
were measured rather than guessed:

* **The mask is written `~0x3fff`, not `0xc000`.** gcc pools 0xffffc000 in a
  single `ldr`; `0xc000` costs `mov` + `lsl` and diverges at the first masked
  instruction. Writing a high mask as the complement of a small constant is
  what puts the 32-bit form in the pool.
* **The result is an `unsigned short`.** `lsl r3, #16` against a pre-shifted
  0xc0000000 is the narrowing-cast tell from batch 29, here on an equality
  rather than on a range. Leaving the value an `int` drops both shifts.

```c
unsigned short d = (a->facing + 0x2000) & ~0x3fff;
if (d == 0xc000) { ... }
```

Five functions matched on this in batch 91. Grep `asm/` for `0xffffc000` to
find the rest: twenty-eight files hold it, and ten of the functions that do are
sixty instructions or fewer.

## A rotation in the argument moves: the callee's RETURN TYPE

When everything else in a function matches and what is left is the ORDER of the
hard-register argument moves in front of one call, change that callee's declared
return type between `void` and `int`.

```
	rom	mov r1, #0 / mov r2, #0 / mov r0, r7
	ours	mov r0, r7 / mov r1, #0 / mov r2, #0
```

Declared `extern void f(...)`, gcc-2.96 emits the moves in ascending register
order and puts r0 first. Declared `extern int f(...)`, it emits r0 last.

**Batches 92-94 got this wrong and batch 99 corrected it.** The lever was
originally recorded as "delete the callee's prototype", because deleting the
whole declaration is what was tried first and it worked. That changes two things
at once. Isolating them:

| declaration | result |
|---|---|
| `void f(int, int)` | r0 first |
| `void f()` | r0 first |
| `int f(int, int)` | r0 last |
| `int f()` | r0 last |
| no declaration at all | r0 last (implicit `int`) |

The parameter list is irrelevant. Deleting the declaration worked only because
it also made the return type implicitly `int`. Two files that carried a
deliberately-omitted declaration -- `src/rom_a1000/rom_a47b4_a_b.c` and
`src/overlays/rom_7ed0a0/ovl_30_c_c_c_c_c_a.c` -- now have full prototypes with
`int` returns and still match.

This also explains why the direction looked unpredictable in batch 94: adding a
prototype to a callee that had none moves r0 EARLIER only if the prototype you
add says `void`. `OvlFunc_961_2008120` improved on deleting a `void` declaration
and would have improved just as much on changing it to `int`.

**The batch-93 caveat is withdrawn.** It said the lever only bites when the r0
argument is a value in a register. `OvlFunc_936_20082e8`
(src/overlays/rom_7c097c/ovl_30_c_c_a_c_a.c) has `mov r0, #8` -- a small
constant -- and responds exactly.

**What it does NOT cover:** a rotation between a pool load and an immediate.
`OvlFunc_943_2008a48` (src/non_matching/ovl_7c7b9c/2008a48.c) is two
instructions out with `ldr r1, =0x103` and `mov r0, #0x15` transposed, and
neither the return type nor deleting the declaration moves it. gcc issues pool
loads as early as it can; that is a different mechanism.

## A value that is provably constant inside its branch is NOT evidence

Two claims were written into batch 92's files from reading the assembly, and
both were false when measured. Both are the same mistake, and it is an easy one
because the assembly looks so specific.

`OvlFunc_957_2008cf8` tests `y == 0x14` and then calls a six-argument function
whose second argument is built with `mov r1, #0x14` while its sixth is filled
with `str r4, [sp, #4]` -- r4 still holding `y`. That reads as a clear statement
that one of them is the variable and the other the literal. It is not: passing
`y` for the stack slot only, for both, or for neither all compile to the same
thirty-five instructions. Inside the branch gcc knows `y == 0x14` and picks per
operand whether to re-derive or reuse, and the choice is not reachable from C.

`Anim_Kite` passes `sp + 0xc` and `sp + 8` as two out-parameters, which looks
like it pins two locals to two frame slots. Swapping their declarations changes
nothing; gcc assigns frame slots by use.

**The discipline:** before writing "the ROM does X, so the source must have said
Y" into a file, compile the alternative and check that it actually differs. A
comment claiming a control that was never run is worse than no comment, because
the next person will believe it. When the alternative turns out to match too,
say so in the file -- the negative is the useful part.

## A value in a callee-saved register is NOT evidence the source named it

This has now cost a spelling in three consecutive batches, so treat it as a
standing non-signal rather than re-deriving it.

The ROM does something like

```
	mov	r5, #0x2a		<- once, before the first call
	...
	str	r5, [sp, #4]		<- at every call site
```

and the push list says `{r5, lr}`, so one value demonstrably survives across
calls. That reads as a named local. It usually is not: gcc hoists a repeated
constant into a callee-saved register on its own, and the literal spelling
compiles to the same instructions. Measured on `OvlFunc_948_20099e8` (identical
at 43), and in the opposite direction on `OvlFunc_964_200a52c`, where writing
the two shared values as locals HOISTS them above the first call and costs three
instructions.

**What is forced, and what this is often confused with:** two DIFFERENT
constants in the two stack slots. There the ROM builds both into separate
registers before storing either, and passing literals makes gcc walk one
register through both stores:

```
	rom	mov r3, #9 / mov r2, #0x26 / str r3, [sp] / str r2, [sp, #4]
	ours	mov r3, #9 / str r3, [sp] / mov r3, #0x26 / str r3, [sp, #4]
```

Naming both is what separates them. So the lever is about **two distinct values
needing two distinct registers**, not about a value living a long time.

The general form, which is the same mistake as the constant-inside-a-branch one
above: **the register allocator's output is not a transcript of the source.**
Before writing "the ROM keeps X in r5, so the source named X", compile the
version that does not name it.

## The gState offset must be BUILT, not folded

`*(int *)(gState + 0x1f4)` lets gcc fold symbol and offset into a single pool
entry:

```
	rom	ldr r3, =gState / mov r2, #0xfa / lsl r2, #1 / add r3, r2 / ldr r0, [r3]
	ours	ldr r3, =gState+500 / ldr r0, [r3]
```

Assigning `gState` to a local `unsigned char *` first blocks the fold, because
the local holds the address as a value and the `+ 0x1f4` has to be real
arithmetic. This is needed **even when the base is used only once** — it is
about the fold, not about reuse, which is what
src/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c_a.c settles.

The tell is in the pool: 0x1f4 is reachable as `mov` + `lsl` (0xfa << 1) and 500
is not reachable by `mov` at all, so the folded form has no choice but to pool
it. A `=symbol+N` pool entry where the ROM has arithmetic means a local base
pointer is missing.

## A register-offset load's operand order: subscript or pointer arithmetic

`LDR/LDRH/LDRB Rd, [Rn, Rm]` computes `Rn + Rm` either way but encodes the two
registers in fixed positions, so `[r3, r2]` and `[r2, r3]` are **different
bytes**. Which one you get is decided by how the access is written:

```c
*(unsigned short *)(file + off)      ->  ldrh r0, [r2, r3]   base first
((unsigned short *)file)[idx]        ->  ldrh r0, [r3, r2]   scaled index first
```

Four spellings were measured on `StartMenu_AddOption`
(src/rom_15000/rom_20198_c_c_c_a_a_c_a_b.c): `file + off` and `(int)file + off`
both give base-first; `off + (int)file` and the array subscript both give
index-first. It was the only differing instruction in forty-five.

So a register-offset load whose **first** register holds a scaled index is a
tell for a subscript, and the base-first form is what naive pointer arithmetic
produces. Check this before assuming a one-instruction residue is allocation
noise.

## When a named constant IS forced: it has to cross a call

Batches 92-95 produced six comments claiming the ROM's choice of register proved
something about the source, and four of them were false. The ones that survived
all have the same shape, so here is the discriminator.

**Not evidence.** Two stores in a row sharing a register; a value sitting in a
callee-saved register; a constant that is provably constant inside its branch;
which operand of a commutative `and`/`orr` becomes the destination. Every one of
these has been measured and the alternative spelling compiled identically.
`OvlFunc_common1_17c0` writes zero to two fields from one register and returns a
third zero from another — the local, all-literals, and all-one-local spellings
are the same thirty-five instructions.

**Evidence, but only sometimes.** A value that has to **survive a call**.
`Func_801ef08` keeps a zero in r10 across three calls and writes it afterwards;
spelled as a bare `0` the function is 35 instructions against 39 and diverges at
the first.

**Necessary, NOT sufficient -- batch 100.** `OvlFunc_956_2008274`
(src/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_c_b.c) keeps TWO pooled constants in
two callee-saved registers across a call and re-stores both afterwards, which
passes the survives-a-call test completely. Named as locals the two registers
come out EXCHANGED (6 differing of 51) and no ordering of declarations or
assignments fixes it; written as plain literals at both sites it matches. So
surviving a call raises naming from "no reason to think so" to "worth trying" --
it does not settle it. Measure both. Two
DIFFERENT constants in two stack slots at one call site are the same kind of
thing: they need two registers at once, so naming both is forced, while two
copies of the SAME value are not.

The rule of thumb: ask whether the spelling changes what has to be **live** at
some point. If it does not, the assembly is showing you the allocator, not the
source.

## Two levers that defeat control-flow blockers

Both were found in batches 95-96 and both have now closed or nearly closed more
than one function. They address blockers that had previously been treated as
dead ends.

### Move the call inside the arms to stop if-conversion

When the ROM **branches** over a choice of two nearby constants and gcc insists
on making it branchless -- the `neg / orr / lsr #31` boolean-normalise idiom
followed by an add or subtract -- look for a call that can move inside the arms:

```c
/* if-converted: no branch at all */
if (cond) id = 0xd2c; else id = 0xd2d;
s = f(id);

/* branches, then cross-jumps the two tails back into one bl */
if (cond) s = f(0xd2c); else s = f(0xd2d);
```

gcc will not speculate a call, so the if-conversion cannot happen; it then
cross-jumps the identical tails, which is exactly the ROM's shape -- two pool
loads, a `b`, one shared call. Found on `Func_80b2ed8`
(src/non_matching/rom_b0000/80b2ed8.c), 26 differing to 19; closed
`Func_80b8f08` (src/rom_b5000/rom_b8228_c_a_c_c_a_b.c), 29 to 2.

### Turn the null test around to stop the return-constant hoist

`if (p == 0) return 0;` as an early return lets gcc hoist the `mov r0, #0`
**above** the test. That hoist parked `Func_80bf37c` in batch 89 and five shape
siblings with it. Writing the same thing with the positive test keeps the
constant in the else block, where the ROM has it:

```c
if (p != 0) {
    ...
    return p;
}
return 0;
```

Took `OvlFunc_common0_18` from 30 differing of 40 to 8 of 41
(src/non_matching/ovl_common/common0_18.c). Worth trying on the whole
`Func_80bf37c` family.

## The r2/r3 exchange: a four-member allocator class

Four parked functions now differ from the ROM by nothing but which of r2 and r3
holds which value, with identical instructions in identical order:

* src/non_matching/ovl_7ed0a0/2009458.c   (a masked byte, 3 of 36)
* src/non_matching/rom_b0000/80b2ed8.c    (a pooled constant, 19 of 46)
* src/non_matching/ovl_7b9cb4/200ab58.c   (a walked pointer, 7 of 35)
* src/non_matching/ovl_common/common0_18.c (a masked byte, 8 of 41)

Between them they have absorbed a dozen spellings -- operand order, declaration
order, named intermediates, walked versus indexed pointers, signed versus
unsigned fields -- and none of it moves the pair.

**BATCH 97: THE EXCHANGE IS SOMETIMES REACHABLE, AND THE LEVER IS A TYPE.**
`OvlFunc_946_200985c` (src/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_a_a_c_b.c) has
the shape

```
	ldrb	r2, [r1] / mov r3, #2 / orr r3, r2 / strb r3, [r1]
```

with the CONSTANT in the destination. `*q = 2 | *q`, `*q |= 2`, `*q = *q | 2`
and a named `int two` all put the loaded byte there instead. A named constant of
the FIELD's own type --

```c
unsigned char two = 2;
*q = two | *q;
```

-- puts the constant in the destination and closes the function. `int two` fails
and `unsigned char two` works on the same statement, so it is the WIDTH of the
named constant that is the lever, not the naming.

It does not transfer everywhere: tried on `2009458.c` it gives 4 of 36 against
that park's existing 3, and on `200ab58.c` it changes nothing. The cases it
reached both store in the SAME statement as the mask; the ones it did not
compute into a variable that crosses a join. That is the distinction to test
next.

**Two more orderings that reach the allocator**, both from
`OvlFunc_923_2009bc8` (src/non_matching/ovl_7aa430/2009bc8.c), 26 differing to 7:

* **Two pointer chains have to be computed before the first store**, or gcc
  derives the second from the first with a `sub` and walks one register
  backwards. Naming them as two locals is not enough; both must be LIVE across
  the first store.
* **The store order then decides which chain gets which register.**

And **statement order decides a coordinate pair**: writing two field reads in
the order the ROM loads them took `OvlFunc_946_200985c` from 9 differing to 2.
Declaration order does nothing; the order of the STATEMENTS is what counts.

**THAT LEAD IS REFUTED, batch 98.** The theory was that
`src/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c_c.c` matches because its `and` result
feeds an `orr` before being stored, giving it a longer live range, while
`common0_18` stores immediately. `OvlFunc_922_2008ed8`
(src/non_matching/ovl_7a8c8c/2008ed8.c) has the `orr` **and still splits**, at 8
of 43. So live-range length is not the discriminator and that avenue is closed.

What the class does respond to is narrower than hoped: a named constant of the
field's width, and statement order, and only in cases that store in the same
statement. Six functions have now been through it; two yielded.

## A jump-table switch, and what it needs

gcc-2.96 turns a dense switch into a real table:

```
	cmp	r0, #5 / bhi <default>
	ldr	r2, =.Ltable / lsl r3, r0, #2 / ldr r3, [r3, r2] / mov pc, r3
.Ltable:
	.word ... six entries ...
```

`GetWeaponSpriteID` (src/rom_b5000/rom_b6eb4_a.c) is the first one matched.
Two things had to be right.

**The case values have to be dense enough.** The table has an entry for 4 even
though 4 is not a case — its slot holds the default label. Cases 0, 1, 2, 3 and
5 with nothing for 4 is what makes the range dense enough for gcc to prefer a
table over the decision tree described above.

**The work goes INSIDE each case, not after the switch.** Written as
`case 0: tbl = A; break; ... default: return r;` with one `r = tbl[type];`
after the switch, gcc emits a separate `mov r0, #0 / b` block for the default
and comes out five instructions long. Written as `case 0: r = A[type]; break;`
in every case, gcc cross-jumps the five identical tails into one AND lets the
default fall straight out of the switch to the shared `return r` — which is what
the table's default slot points at.

Those are the same decision seen twice: **give gcc identical tails and let the
default do nothing.** It is the same shape as the duplicated-tail lever for
plain `if` exits.

## A split that says "no data" is a claim to CHECK

`tools/split_s.py` refuses to convert a single-function `.s` that also carries
data, because deleting it would take the data with it and the link would fail
much later with `undefined reference`. Twice now that check has been blind:

* **batch 78** — it did not recognise `.lcomm`/`.comm` definitions;
* **batch 101** — it counted `.incbin` but not **`.incrom`**, and its
  "stranded label" test only looked for labels the function never mentions. A
  jump table's targets are *both* referenced by the function *and* data, so they
  passed. `rom_b6eb4.s` was reported dataless, deleted, and the link died on
  `.Lc2a46`.

It now counts every blob directive, flags any `.section` appearing after the
last `.func_end`, and flags labels *defined* after the code regardless of
whether the function mentions them.

The working lesson survives the fix: **when the tool says a file has no data,
look at the tail of the file yourself.** The failure mode is a link error that
reads like a bad decompilation rather than a bad split, so it costs a
disproportionate amount of time to diagnose.

## gcc-2.96 has no immediate alternative for an HImode constant

Storing a literal `0` through a `short *` gives `ldr r3, =0x0` — a four-byte
pool load of zero — not `mov r3, #0`. Measured on `short`, `unsigned short`,
through a `short *` local, and by indexing a `short *` return value. All pool.

So when the ROM has

```
mov r3, #0x0 / add r0, #0x64 / strh r3, [r0]
```

the right-hand side in the source is **int-typed** and the `strh` truncates it.
`{ int zero = 0; *(short *)(...) = zero; }` reproduces the ROM's sequence
(`OvlFunc_936_2009f14`, 106 differing to 12). The residue there is that gcc
coalesces repeated `zero`s into one pseudo whose live range crosses a call, so
it lands in a callee-saved register — the shape is right, the register class is
not.

The rule generalises: **a pool load of a small constant where the ROM has a
`mov` is a TYPE question, not a scheduling one.** It is BOTH, and batch 107 found
the second half.

**The int-typed value must also be in a DOMINATING block, and each site needs
its own local.** Declaring it beside the store is not enough -- gcc keeps it in
a register and, if several sites share one local, coalesces them into a pseudo
that lives across a call. Measured on `OvlFunc_936_2009f14` (four sites):

| | differing of 103 |
|---|---|
| bare literal `0` | 106 |
| `int zero = 0;` inside each case | 12 |
| ONE `int zero = 0;` at the top of the function | 83 |
| FOUR separate `int` locals at the top | **match** |

and on `OvlFunc_905_20090c8` (one site): `int k = 0x63;` in the store's own
block is 46 of 69, the same declaration at the top of the function matches.

So the HImode rule is the basic-block lever wearing a type: the value must be
int so `strh` truncates it, AND it must be rematerialised at the store. The same way a lone trailing
`mov r0, #imm` the ROM lacks is a return-type question.

## A call result in a store expression must not go through a named local

```c
p = __MapActor_GetActor(n);
*(short *)(p + 0x64) = 0;        /* mov r2, r0 / add r2, #0x64 / strh */
```

keeps `p` live and copies. Inlined —

```c
*(short *)(__MapActor_GetActor(n) + 0x64) = 0;   /* add r0, #0x64 / strh */
```

— `r0` is dead after the add and the ROM's form appears. Two instructions per
site; over five arms of a switch that was ten. This is the *opposite* of the
usual advice (name the value to force a copy), so check which way the ROM went
before reaching for either.

## `bls`/`b` where the ROM has one `bhi` can be a LENGTH symptom

A Thumb conditional branch reaches ±254 bytes. A switch whose default target is
the function epilogue will invert to `bls <table> / b <epilogue>` once the body
plus the jump table pushes the epilogue out of range. On `OvlFunc_936_2009f14`
that happened at 105 instructions and corrected itself when the body came down
to 103. **Do not chase the branch polarity — find the extra instructions.**

## A peeled `case 0` before a jump table means an `if`/`else`

```
cmp r3, #0 / beq <somewhere> / sub r3, #1 / cmp r3, #0xb / bhi
```

is not what one switch with `case 0:` in it compiles to — that gives a table
starting at 0 with no peel (`Anim_Summon`, 90 lines against 92, 77 differing).
The source is

```c
if (c == 0) { ... } else switch (c) { ... }
```

and gcc cross-jumps the if-branch into whichever case shares its body, which is
why the `beq` can land on a jump-table target.

**`else switch` is not interchangeable with an early return.** The form

```c
if (c == 0) { ...; return; }
switch (c) { ... }
```

loses the shared epilogue and runs long by however many instructions the
epilogue holds — five, on `Anim_Summon`.

## Spell the last case explicitly ALONGSIDE `default:`

When a switch's highest case shares a body with the out-of-range case, writing
`default:` alone makes gcc drop the jump table for a comparison tree
(`Anim_UnleashIntro`, 59 differing). Writing

```c
case 4:
default:
    ...
```

restores the table, with slot 4 doubling as the `bhi` target. The apparently
redundant `case 4:` is the lever.

## The statement-form argument lever: where it stops

Writing an argument as its own statements gets a shift ahead of the other
argument setup:

```c
arg = 0xc8;  ...  arg <<= 4;  StartTask(Func_80cc960, arg);
```

This works when the shift is racing a **pool load** (`ldr r0, =Func_80cc960`) —
two independent sites on `Anim_UnleashIntro`, six differing to two.

It does **not** work when the shift is racing a **`mov`**:

```
rom    mov r2, #0x80 / lsl r0, #19
ours   lsl r0, #19 / mov r2, #0x80
```

Seven spellings were compiled against that one site and all seven give the same
two instructions: the constant as a local assigned before the shift, before the
other argument, before the preceding call; the shift folded into the argument
expression; the address staged through a separate pointer local; and an
unprototyped function-pointer typedef.

This is almost certainly the same class as the **r0-against-a-shift rotation**
recorded on `OvlFunc_911_2008304`, `OvlFunc_888_20085cc` and
`OvlFunc_948_2009fd8` — five functions on one unsolved shape.

## A constant hoisted across a call

Two calls in one block taking the same non-immediate constant (`0x4000`, built
as `mov #0x80 / lsl #7`): gcc builds it once and keeps it in a callee-saved
register across the first call, where the ROM rematerialises it. The cost is a
whole extra callee-saved register, so it shows up as a wrong `push` list and a
renumbering of every callee-saved register in the body — a large differing count
from one cause.

**Flags do not reach it**: `-fno-gcse`, `-fno-rerun-cse-after-loop`,
`-fno-cse-follow-jumps`, `-fno-force-mem` all identical;
`-fno-expensive-optimizations` worse. It is local CSE.

The smallest instance is `src/non_matching/rom_c9000/cd260_a.c` (two calls, 29
of 105). The largest is `src/non_matching/overlays/200b4c8.c` (about fifteen
constants, 1027 instructions). Solve it on the small one.

## Constant CSE vs argument scheduling: which fix, and prefer the flag

Two different things both look like "a constant is in the wrong place", and the
basic-block lever fixes both. Only one of them should be fixed that way.

**Constant CSE.** One value used by two calls; gcc builds it once and keeps it
in a callee-saved register, so the function grows a `push` the ROM lacks.

    rom    ldr r0, =0x201 / bl __GetFlag  ...  ldr r0, =0x201 / bl __SetFlag
    ours   ldr r5, =0x201 / mov r0, r5 / bl __GetFlag  ...  mov r0, r5 / bl __SetFlag

**`-fno-rerun-cse-after-loop` fixes this and the C stays literal.** The lever
also fixes it, by giving each use its own named local — and that is the wrong
answer, because a handful of `int` locals whose only job is to hold a flag id is
not source anybody wrote. `OvlFunc_890_2008150` needed five of them; the flag
matches it with no change to the C at all.

**Argument scheduling.** A two-instruction constant split around another
argument, or a pool load issued before the register moves. **The flag does not
touch this.** Measured on the three functions batch 105 closed with the lever:
under `-fno-rerun-cse-after-loop` with their literal spellings,
`OvlFunc_948_2009fd8` stays at 12 of 97, `OvlFunc_911_2008304` at 2 of 85,
`OvlFunc_943_2008a48` at 2 of 57.

The two are independent and a function can need both. `OvlFunc_942_20088cc`:

| | differing of 53 |
|---|---|
| literals, default flags | 46 |
| literals, `-fno-rerun-cse-after-loop` | 3 |
| lever, default flags | 48 |
| lever + `-fno-rerun-cse-after-loop` | **0** |

**So: try the flag first and keep the literals. Reach for the lever only for
what the flag leaves behind.** CSE_CFLAGS is an existing per-file group with
many members, so adopting it costs nothing.

## The complete table of argument orders gcc-2.96 can emit

Compiled as one-line functions under this tree's exact flags (batch 106). This
is the ground truth the return-type lever and the interleave levers are
navigating:

| callee | third argument | emitted order |
|---|---|---|
| `void` | cheap constant | r0, r1, r2 |
| `int` | cheap constant | r1, r2, r0 |
| `void` | pool constant, or `mov`+`lsl` | r2, r0, r1 |
| `int` | pool constant, or `mov`+`lsl` | r2, r1, r0 |
| `void` | a global read | `ldr` base, r0, `ldr` r2, r1 |

`precompute_register_parameters` (calls.c:805) copies any argument whose
`rtx_cost` exceeds 2 into a pseudo before any hard register is loaded — that is
the whole difference between rows 1-2 and rows 3-4.

**A ROM that issues a CHEAP `mov` for a later argument before r0 is
unreachable.** That is row 3's order with row 1's cost, and no spelling of a
value both clears the threshold at expand and assembles to one `mov`.
`OvlFunc_885_20080dc` is parked on exactly that, with the return-type lever, the
unprototyped form, the basic-block lever, narrow parameter types and seven flags
all measured at the same 9 of 56.

Note also what the table does NOT contain: **r0 in the middle**. The
return-type lever moves r0 between first and last, and nothing moves it to the
middle of a three-argument call.

## A LOW-numbered `.L` symbol cannot use the asm-label extension

`extern unsigned char tbl[] __asm__(".L7");` compiles, links, and is **wrong**.
gcc numbers its own labels `.L1`, `.L2`, ... so a low number collides: gcc emits
a `.L7:` of its own in front of the constant pool and the reference resolves to
that instead of the external blob.

The tell is subtle. `tools/tryc.py` reports ONE differing line — a stray label
at the end of the stream — which reads like a pool-placement artifact. The
build then fails `make compare` on the overlay.

The technique's own examples (`.L23f0`, `.L57fc`) are high numbers where this
cannot happen. **For a low-numbered label, rename and export instead** — and
check the precondition first: `grep -rln '^\.LN:' asm/` will usually show
hundreds of files, because `.LN` is gcc's per-file local label everywhere. What
matters is how many files reference it ACROSS an object boundary. For `.L7`
that was exactly one, so the rename to `gOvlCommon1_3fe4` was two lines.

## `neg` of a constant is the two's complement, so read the bits

`mov r3, #0xd / neg r3, r3` is `~0xc`, not `~0xd`. -13 is `0xfffffff3`, which
clears bits 2 and 3 and leaves bit 0 alone.

Reading it as `~0xd` on `OvlFunc_881_200813c` turned one bitfield write into
two and cost three instructions. The batch-71 rule -- a 32-bit `mov`/`neg` pair
means a bitfield -- is right; getting the WIDTH and OFFSET of that bitfield
wrong from a misread mask looks exactly like a codegen difference.

## REBUILT or CARRIED: read which, then place the local accordingly

Three blocker classes in this document are the same rule seen from different
angles, and batch 107 collapsed them. Before naming a value, decide from the
assembly whether the ROM **rebuilds** it at each use or **carries** it there.

| the ROM does | the C | why |
|---|---|---|
| rebuilds the value at the use (a `mov`/`lsl` pair split around another argument; a `mov` where gcc pools; a pool load issued late) | a local in a **dominating** block, **one per site** | crossing a block boundary makes local-alloc rematerialise instead of allocating |
| carries the value into the use (one register holding it across calls, a callee-saved register in the push list) | a local **adjacent** to the first use, **shared** | adjacency is what makes the values live SIMULTANEOUSLY |

**CARRIED HAS A PRECONDITION, added in batch 109: name it only if gcc would
otherwise REBUILD it.** If gcc is already carrying the value, naming it only
moves where it is built, and the literal's position is usually the ROM's. The
`-1` in `src/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_c_a_b.c` is carried in r6
across three calls; named and assigned above the `if` it is 22 differing of 38,
because gcc then builds it before the call the ROM builds it after. As a plain
literal, gcc carries it into r6 by itself. **Screen the unnamed spelling first.**

Both are visible before writing any C. A value in the ROM's push list is
carried. A value built twice from scratch is rebuilt.

`OvlFunc_902_20084e4` needs both in one function: its zero is CARRIED across
three calls (named immediately before the first store — hoisting the same
declaration to the top of the function is 14 differing of 61), and its two
stack arguments are CARRIED (named adjacent, per the stack-arg-pair lever).
`OvlFunc_936_2009f14`'s four zeros are REBUILT (four separate locals at the top
of the function; one shared local is 83 of 101, worse than the park it came
from).

**The failure mode is applying the right idea in the wrong block.** Batch 104
found that an HImode store needs an int-typed right-hand side and wrote the
local beside the store; batch 96 found that `OvlFunc_943_2008a48` wanted its
constant named and wrote it inside the else-block. Both were correct about the
value and wrong about the position, and both sat parked for several batches.

## After a match, GREP FOR ITS PROLOGUE

`tools/find_twins.py` and `tools/find_families.py` compare whole-function
shapes. A cruder search finds things they do not: take the first five or six
instructions of a function you have just matched, escape them, and grep every
`.s`.

```python
pat = re.compile(r"\.thumb_func_start (\S+)\n\tpush\t\{r5, r6, r7, lr\}\n"
                 r"\tldr\tr3, =iwram_3001ebc\n\tldr\tr7, \[r3\]\n"
                 r"\tbl\t__CutsceneStart\n\tmov\tr5, #8\n\tmov\tr6, #0\n")
```

Batch 108 got **five of its seven** this way, from two searches. The reason a
prefix match wins is that these functions are identical only at the start and
diverge freely afterwards — a whole-function shape comparison scores them apart,
but the prologue is where the hard decisions (which locals, which registers,
which flags) are already made. Each hit then costs a constant substitution plus
whatever its own tail does differently.

**Two minutes per match, and it is the highest-yield habit in this document.**

**Batch 109 turned it into `tools/prologue_families.py`**, which clusters every
remaining function in `asm/` on its first N instructions so the families can be
picked BEFORE solving a member. Use `--n 12`; at `--n 6` the largest cluster is
235 functions sharing nothing but the high-register save boilerplate. It
canonicalises immediates, pool symbols and branch labels and keeps register
numbering literal — registers are what does NOT vary inside a real family, so
canonicalising them collides unrelated code.

34 families of 3+ share their first twelve instructions. The three largest are
per-overlay copies of one routine — 18 members at 172 instructions, 18 at 139,
17 at 132 — which is 53 functions behind three solves, and the largest single
lever left in the tree.

## Screening is not wiring

`tools/tryc.py` saying OK means the C compiles to the ROM's instructions. It
does **not** mean the tree changed. Splitting the `.s`, writing the `.c`,
deleting the `.s` and rebuilding is a separate step, and `make compare` cannot
tell you it was skipped — an unwired function is still assembly and still
builds green.

Batch 108 wrote a function into a commit message as elevated when only the
screen had been run. The check that catches it:

    grep -rh "^\.thumb_func_start" asm/ | wc -l

before and after. If the count did not drop by the number of functions claimed,
something was screened and not wired.

## The CSE flag and the constant hoist are NOT the same thing

Batch 106's rule was "try `-fno-rerun-cse-after-loop` first and keep the
literals". Batch 110 found the case it does not reach, and the distinction is
worth keeping straight:

| the ROM's shape | what reaches it |
|---|---|
| a value read then written with CALLS BETWEEN, held in a callee-saved register | `-fno-rerun-cse-after-loop` |
| the SAME pool constant needed by two calls in ONE straight-line block, built once and copied | the basic-block lever ONLY |

`OvlFunc_890_2008150` is the first row and the flag matches it with plain
literals. `src/non_matching/rom_7d30e0/2009838.c` is the second and SIX
CSE-related flags leave it untouched — the hoist happens at expand, and no flag
in this tree's vocabulary disables it.

The lever reaches the second row, and the lever needs a branch.

## Nine functions are parked on "the lever is right and there is no boundary"

Two family parks now sit on the same sentence:

* `src/non_matching/ovl_780898/2008e54.c` — six functions, straight-line
  arg-interleave.
* `src/non_matching/rom_7d30e0/2009838.c` — three functions, straight-line
  constant hoist.

In both, the shape is one the basic-block lever demonstrably fixes elsewhere,
and in both the function has no branch to put between the assignment and the
use. **A construct that produces rematerialisation without a control-flow
boundary would unpark nine functions at once**, and is the highest-value open
question in this document after the eighteen-member family in
`src/non_matching/ovl_780898/20080c4.c`.

## `n += 0xff` is not the same spelling as `n--`

When a value is stored with `strb` and then tested for zero, +255 and -1 agree
modulo 256 — and gcc-2.96 emits the ROM's `add r3, #0xff` only for the first.
`n--` on an `unsigned char` produces `lsl #24 / lsr #24` around the store
instead. On `Func_80bf37c` that is 24 differing against 11.

Read the ROM's `add rN, #0xff` as a decrement written the long way, not as an
addition of 255.

## Pool-constant CSE: the complete rule

A constant needing a pool load (`ldr rN, =0x9999`) used **twice, with one use
dominating the other**, is hoisted by gcc-2.96 into a callee-saved register and
copied to the argument register at each site. The ROM often reloads it. Probed
directly with one-line functions under this tree's exact flags, and the answer
is a TWO-PART precondition:

| between the two uses | flags | result |
|---|---|---|
| nothing — straight line | any combination tried | **hoisted** |
| a branch | default | **hoisted** |
| a branch | `-fno-rerun-cse-after-loop` | **reloaded** — `push {lr}` only |
| mutually exclusive arms | any | **reloaded** |
| — two DISTINCT symbols of equal value | any | **reloaded** |
| — the SAME symbol twice | default | hoisted |

**Neither half alone is enough.** A branch under default flags still hoists; the
flag without a branch still hoists. That is why batch 106's "try the flag first"
rule worked on `OvlFunc_890_2008150` (early returns between the uses) and did
nothing for `src/non_matching/rom_7d30e0/2009838.c` (two consecutive calls, no
branch anywhere).

It also corrects a natural assumption: **the basic-block lever does not defeat
this.** The lever moves where a SINGLE-use value is materialised. A twice-used
value with domination is a different mechanism, and the lever alone leaves it
hoisted — the flag is the other half.

Confirmed against the matched corpus: `src/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_b.c`
reloads `0x201` across two early returns and is built with `CSE_CFLAGS`; its own
header note records 25 instructions against 23 without the flag, exact with it.

**Genuinely unreachable population.** A repeated pool constant with NO label
between the two uses has no boundary to supply the first half, and nothing
reaches it: **316 of the 2,472 remaining functions — 13% by count but 35% of the
remaining instruction mass**, 178 of them 250+ instructions. In the
straight-line script band (250+ instructions, under 2% labels) it is 123 of 138,
because those functions have no branches at all.

The only construct that reaches the no-branch case is an inline-asm barrier
(`__asm__ ("" : "+r" (x))`), which is a fakematch — see `fakematch.txt`, where
`OvlFunc_958_2009080` uses exactly that.

**So the straight-line script band is blocked, and it is a third of the project.**
That is the single most valuable open question in this document. What would
settle it is a clean construct that makes gcc rematerialise a pool constant with
no control-flow boundary available.

## The symbol-address technique, generalised

`§Technique: stopping a constant fold with symbol addresses` records the
SUBTRACTION case (`0xcc6 - 0xc9b`). It is more general than that, and batch 111
retired two parks with it:

> **Wherever the ROM shows gcc FAILING to relate two constants it obviously
> could, try making them symbols.** An `int` constant folds; a `SYMBOL_REF` does
> not, and gcc cannot reach one symbol from another by an immediate.

Three shapes it covers:

* **A base reached later by `add`.** `src/non_matching/ovl_7e636c/2008fd0.c` was
  parked with the note "a named `int base` does not change it — gcc
  constant-folds `base + 8`". `base = (int)&_MSG_23cc;` cannot be folded, so
  gcc holds the symbol in a callee-saved register and does `mov r0, r5 /
  add r0, #8` — the ROM's sequence, push list included.
* **Nearby constants chosen by a switch.** `src/non_matching/overlays/200906c.c`
  was parked on gcc deriving three ids from one pool load; three symbols give
  three loads.
* **A small constant that the ROM POOLS.** Anything that fits `mov` but appears
  as `ldr rN, =K` is a symbol — `_AREA_4b`, `_AREA_7e`.

And a second, independent tell worth knowing: **a pool load of a SYMBOL is not
hoisted, where a pool load of an int constant is.** That fires on values too
large for `mov`, where the "small constant pooled" tell says nothing.

### Batch 149: the `str` operands tell you which stack arguments to name

The rule above says each site needs its own pair. It does not say which sites
need a pair at all, and the assembly answers that directly -- look at what the
`str` reads from:

    str r6, [sp]          <- a HELD register: the ROM spent a callee-saved
                             register on this value, so it is a shared local
    mov r3, #0x18
    mov r2, #0x3e         <- BOTH built fresh into separate registers, then
    str r3, [sp]             stored: this site wants its own pair of locals
    str r2, [sp, #4]

Written as literals, gcc reuses one register for both and interleaves the
stores (`mov r3 / str / mov r3 / str`), which is one register short of the ROM
every time. Written as a fresh pair, each gets its own register.

`OvlFunc_941_20080d4` has eight six-argument calls and needs both answers: two
values are shared locals because the ROM keeps them in r5 and r6 across the
whole body, and two sites build both arguments fresh and need their own pairs.
Six differing to exact. `OvlFunc_922_2009050` is seven sites, one shared and
five pairs, and matched on the first screen from the same reading.

Do NOT share a pair between sites even when the values repeat -- that is the
failure the rule below was written for.

**And where the assignment goes picks the REGISTER CLASS.** A named stack
argument lands in a callee-saved register only if its live range CROSSES A CALL.
`OvlFunc_927_2009454` passes (4, 0) at its last site and the ROM holds the zero
in r5 across the whole register-argument setup, storing it after r0-r3 are
loaded. Declared immediately before the call it gets a SCRATCH register and is
stored at once -- 6 differing. Moving the assignment up so it spans the
preceding call makes gcc spend r5 on it and the function matches. Hoisting it
further, to the top of the function, is wrong again (7).

So: the `str` operands say which values get names; the statement position says
which register class they get. Read both off the ROM before writing either.

**Batch 150: it is every SLOT, not just a pair, and it is all-or-nothing.**
`OvlFunc_888_20084e8` makes an ELEVEN-argument call -- four in registers, SEVEN
on the stack. The ROM materialises every stack value into its own register and
only then issues the stores. Written as literals gcc reuses ONE register for all
seven (`mov r3,#3 / str r3 / mov r3,#7 / str r3 / ...`), 22 differing, and it
never spends the callee-saved register the ROM pushes.

Naming THREE of the seven is worse than naming none (28 differing against 22).
Naming all seven takes it to 2. The rule is all-or-nothing because the register
the extra locals compete for is the one that decides the prologue -- a partial
naming leaves gcc short of exactly one and it reshuffles everything.

Two of the seven were shared, read off the `str` operands as usual: one value
stored to two slots from one register, and one that is both a register argument
and a stack slot. Assignment order follows the ROM's materialisation order.


## Each stack-argument SITE needs its own pair of locals

`§The stack-arg-pair lever` says to name both values adjacent to the call. That
is necessary and not sufficient when a function has several such calls: one pair
of locals reused across sites lands in r2 where the ROM has r3, at every site.
`OvlFunc_936_20098a4` is 4 differing of 58 with a reused pair and exact with one
pair per site; `OvlFunc_911_200a910` has four sites and needs four pairs.

That is the rebuilt-vs-carried rule reaching the stack-argument case. In the
same function a value the ROM genuinely SHARES (`mov r5, #2` once, stored at
both calls) still wants a single local — the two rules land on adjacent
arguments of one call, and the ROM says which is which.

## A DIRTY screen that opens on a LABEL is a false negative

`§Use --align on anything long` and the inline-pool warning both say a CLEAN
screen can be unproven until `make compare`. Batch 112 found the symmetric
failure, and it had a function parked on it.

`OvlFunc_898_2008a4c` screened at **25 differing of 50** and its parked C was
already byte-perfect. gcc puts the pool-skip label immediately before the `if`s
own join label, so two label definitions land at the same address:

    ours   strh r3,[r2] / b .L5   / <pool> / .L5: / .L3: / mov r0, #0xe
    rom    strh r3,[r2] / b .La98 / <pool> / .La98:       / mov r0, #0xe

A label emits no bytes. `tryc.py` keeps branched-to label definitions in the
stream deliberately -- that is correct and must stay -- but one extra label
shifts every later position and the positional count cascades.

**So: if the FIRST differing line is a label definition, check the bytes before
believing the number.** Assemble both sides standalone and `objdump -d` them; it
costs about what a screen costs. `scratch/agent3/bytecheck.sh` is one
implementation.

Worth sweeping the parked set for: any park whose first difference is a label.

## Doing this at volume: operate on a list, not a glob

Two mistakes in batch 112, both from generating file headers programmatically:

* A quote-stripping pass (gcc-2.96 warns on an unbalanced `'` even inside a
  comment) was applied by glob to every `.c` under `src/overlays/` -- **404
  files** -- instead of the new ones. Caught before commit; restored with
  `git show HEAD:<path> > <path>`, which is the tree's way back since
  `git checkout` is not available.
* Body extraction searched for `struct` anywhere in the text and matched it
  inside the word "instruction", truncating a comment mid-word. It failed at
  compile time only because the fragment happened to be invalid C.

**Operate on an explicit file list, and anchor text extraction to line starts.**

## A Makefile target with a doubled slash is a different target

`grep -rln 'pattern' asm/` prints paths as `asm//overlays/...` because the
search path ends in a slash. POSIX collapses `//` when opening a file, so `.c`
files written to such a path land correctly and every screen passes.

**make does not collapse it.** `asm//overlays/X.o` is a different target from
`asm/overlays/X.o`, so a per-file flag rule written that way is silently dead
and the file keeps building from whatever pattern rule matches.

The symptom in batch 113 was **every overlay comparing clean while the ROM SHA1
failed**, which sends you looking at the main ROM where nothing changed. The way
back was `git stash push -u` to prove HEAD was green, then re-reading the six
added lines.

**Normalise paths before putting them in a makefile.** When a rule seems not to
apply, compare the target string character by character before suspecting the
recipe.

## An explicit rule beats a pattern rule -- use that instead of narrowing

Several stems in this tree have `%` wildcard rules applying `O1_CFLAGS`, and
some of them are wrong for particular files: `tools/tryc.py` prints the flag
group it inherited (`built with: O1`), and in batch 113 **two of five such
warnings were wrong for the file in question** -- 20 and 43 differing at `-O1`,
exact at `-O2`.

Do not narrow the wildcard; other files depend on it. Add an **explicit** rule
for the one target, which GNU make prefers over any pattern rule. The cost of
checking a warning is one screen, so check every one.

## Block layout tells you which branch is the `if` BODY

A conditional branch that jumps FORWARD past a block means that block is the
fallthrough, and therefore the `if` body rather than the `else`. Getting it
backwards does not cost one instruction; it rearranges the whole tail.

    OvlFunc_917_200952c   early-return form 42 of 55, if/else form exact
    OvlFunc_959_2009980   passing-case form 10 of 56, failing-case form exact
    OvlFunc_959_2009918   3 of 54 with the polarity swapped

`200952c` reads naturally as `if (n > 0x77) { cleanup; return; }` and that is
wrong -- its `bgt` skips forward over the arc code, so the arc is the body.

On a two-clause condition it means spelling the test as the FAILING case:
`if (dx > 7 || dz > 5) return 0; return 1;`.

On a single boolean result, **which value the ROM presets is the tell**:
`mov r0, #1 / cmp / ble / mov r0, #0` presets TRUE and overwrites on failure,
which is `if (cond) return 1; return 0;`. Forcing it with a result variable
costs an instruction.

## Do not write out signed division or signed ranges

`if (x < 0) x += 0xfffff;` then `asr #20` is `x / 0x100000` on an `int`. gcc
generates the bias-and-shift. Writing the bias by hand adds a branch per site,
and these appear four times in a 54-instruction function.

`v >= -6 && v <= 6` compiles to one unsigned compare (`add r3, #6 / cmp r3,
#0xc / bhi`). Two signed compares give two branches.

And where the ROM spells a test oddly, spell it that way: `ax - 1 < bx &&
ax + 1 > bx` is `ax == bx`, and only the long form reproduces.

## A third family axis: grep for an IDIOM, not a position

`tools/prologue_families.py` clusters on the first N instructions and
`find_twins.py` on whole-function shape. Batch 114's six share neither -- they
share an idiom. Grepping every `.s` for functions under 90 instructions that use
the `=0xfffff` division bias twice or more returns 15 functions.

So the three axes are: same PROLOGUE, same SHAPE, same IDIOM. The third is a
one-line grep and it found a family the other two score apart.

## A `.pool_aligned` INSIDE A LOOP is the other label false negative

Batch 112 found a false negative where the first differing line is a label
definition. Batch 113 built `tools/label_false_negatives.py` for exactly that
signature, swept all 228 parks, got zero hits, and recorded it as a clean
negative. **That conclusion was too broad and the detector was too narrow.**

Batch 115 elevated four functions that screen DIRTY at 48, 28, 25 and 26
differing lines and are **byte-for-byte identical to the ROM**. The sweep could
not have found them:

* it screened only **parks**, and three of the four had never been attempted;
* it flagged only diffs whose **first** differing line is a label, and these
  open on an ordinary instruction — the label shift appears further down.

The producing shape is a **`.pool_aligned` inside a loop body**. The ROM has
`b .LN / pool / .LN:` — one label — where gcc emits two, and every subsequent
line shifts by one position.

> Any DIRTY screen whose ref contains `.pool_aligned` inside a loop goes to a
> byte-level check BEFORE a single spelling is changed.

The check is to assemble the candidate and the ROM function standalone and diff
the `objdump -d` byte column together with `.text` sizes;
`scratch/agent3/bytecheck.sh` does it. Equal size plus equal byte sequence is
proof; `make compare` then confirms it in the tree.

The general lesson, which cost more than the four functions: **a negative from a
detector bounds only what that detector looks at.** "Swept all 228 parks: zero
hits" was true and was reported as though it settled the question.

## `-fno-rerun-cse-after-loop` is not free, and LOOPS are where it costs

This doc has said for many batches to try the flag before contorting the C. The
counterpart, measured across four independent screening runs:

| function | default | with the flag |
|---|---|---|
| `OvlFunc_918_20097ec` | 18 differing | **45** |
| `OvlFunc_881_200a768` | **OK** | 23 differing |
| `OvlFunc_969_20084bc` | **OK** | 35 differing |
| `OvlFunc_925_20088cc` | **OK** | 35 differing |
| `OvlFunc_932_200b9c8` | **OK** | 47 differing |

In a loop whose base address is a `SYMBOL_REF`, the second CSE pass is what
keeps that base in one register; removing it makes gcc rebuild the address at
every access. **Measure it, never assume it neutral, and never widen it to a
file group on the strength of one function in that file.**

## Reading rule: count the `mov r0, #0` sites

For a boolean-returning function built from a chain of tests, the number of
distinct `mov r0, #0` sites tells you how the source is spelled:

* **one** zero site — one combined condition,
  `if (A && B && C && D) return 1; return 0;`
* **two** zero sites, one hoisted above the first `cmp` — the first test is a
  **separate early return**,
  `if (!A) return 0; if (C && D) return 1; return 0;`

`OvlFunc_959_200981c` and `OvlFunc_959_2009880` are the same shape with the axes
swapped and take *opposite* spellings. The second is 16 of 52 written the first
way and exact written the second. Two family members wanting opposite spellings
of the same test is normal; the zero-site count is what separates them.

## Three cheap levers, each one screen

**Declaration order alone, statements untouched.** `OvlFunc_956_20085e0` went 13
of 54 → exact on swapping `int m; int n;` to `int n; int m;`. Swapping the
*assignments* gave 11; four separate locals gave 6. The earlier claim that
declaration order is inert once the values are born in separate statements is
wrong. Try this first on any same-length r2↔r3 park.

**Copy the FIRST parameter into a local to swap the two entry `mov`s.**
`OvlFunc_883_200b45c` differed only in `mov r8, r1` / `mov r6, r0` order.
`f(int first, …) { int slot = first; … }` makes gcc emit the *second*
parameter's copy first. Naming the second parameter instead does nothing.

**`while (1) { …; if (exit) break; i++; }` reaches strength reduction where
`for (i = 0; ; i++)` does not.** `OvlFunc_881_200a768` walks a 12-byte-record
table; the `for` form rebuilds `i*12` at every access (29 of 52 differing), the
`while` form yields the ROM's two byte-offset induction variables and matches.
Semantically identical — only the increment's position relative to the `break`
differs. The second loop in the same function strength-reduces correctly in
`for` form, so this is not about record size.

## Store INSIDE each arm, or gcc will speculate the cheap one

`if (c) { small } else { big }` with one store after the join: written with a
shared result variable and a single trailing store — including the `goto`
spelling — gcc **speculates the cheap arm above the compare**, inverts the
branch, and the small block stops being a block: `OvlFunc_943_20088e0` at 45 of
46. Writing the store **inside each arm** and letting cross-jumping merge the
two identical `strb`s reproduces the ROM's layout: 11 of 46.

> When the ROM's short arm is a real basic block ending in `b`, the source
> stores in the arm. When gcc collapses your short arm, you gave it something
> speculatable.

This is the mirror image of the tail-merge notes above, which are about arms
that *should* merge.

## Read a field twice to get the redundant-looking `mov`

The ROM has `ldrb r3,[r3] / cmp r3,#0 / … / mov r1, r3`. `n = p->f27;
if (n != 0) {…}` coalesces that copy away. Writing the **guard on the field** and
the **body on a local** — two textual reads that gcc then CSEs — reproduces it.
`OvlFunc_882_200a09c`: 23 → 3 differing on this alone. Generalises
rebuilt-vs-carried to a value the ROM reads once but the source names twice.

## Correction: `ldr rN, =0` is NOT a symbol tell

The pooled-small-constant section says gcc never pools a constant it can `mov`,
and sends you to `const.sym`. **gcc-2.96 really does pool a plain literal `0`.**
`OvlFunc_945_2008284` has two `ldr r3, =0` sites and a plain `0` reproduces both
byte-exact; gcc writes them as `ldrh r3, .L11` + `.word 0`, which the assembler
encodes as `ldr r3, [pc, #imm]`.

There are 53 `ldr rN, =0` sites in `asm/`. The shape appears where gcc
**cross-jumps two arms into a shared tail** and needs the constant in a register
on the merged path; in the same function the non-merged arm uses a register that
already holds 0. Do not add a `_CONST_0`.

## `push {r4` is a one-grep test that the file is NOT built with `-fcall-used-r4`

**0 of 2134 generated `.s` files under `asm/overlays/` contain a `push {r4`.**
The flag is global in `GCC296_CFLAGS`, so a *hand-written* `.s` that pushes r4
cannot have been built with it. `OvlFunc_common2_380` is 12 of 52 under the
tree's flags with **every one of the 12 an r4-vs-r5 rename**, and exact under
`--cflags "-fcall-saved-r4"`.

    grep -l 'push\t{r4' asm/overlays/**/*.s

### The sweep was run (batch 151). It does NOT explain the parked set.

**16 functions in the remaining corpus push r4, and ZERO of them are parked.**
Every coin-flip park is a function whose ROM leaves r4 alone, so the flag
cannot be what is wrong with any of them. Do not run this sweep again hoping
it will unblock the register-allocation class; that class stays open, and the
cause remains the preference pass in `find_reg`, not a flag.

What the sweep DID show is that the 16 cluster hard: eight are `common2`
(`_0`, `_254`, `_28c`, `_304`, `_380`, `_41c`, `_44c`, `_618`) and the rest are
m4a/sound. So `common2` — already known to be a non-interwork TU — was also
built without `-fcall-used-r4`, and its r4-pushing functions were unreachable
from C *by construction*, not through any fault of the decompilation.

`COMMON2_CFLAGS` now substitutes `-fcall-saved-r4`. This was verified rather
than assumed: **all nine existing `common2_c*.c` compile to byte-identical `.s`
under both flags**, so the flip is a no-op for the matched corpus and only
opens the r4 functions. `OvlFunc_common2_380` was elevated on the strength of
it. That verification is the pattern to copy before touching any flag group —
compile every file the rule already covers under both settings and diff, which
costs seconds and converts an assumption into a fact.

Note the two flags are not interchangeable as a *screen*: passing
`-fcall-saved-r4` by hand to a candidate whose C shape is still wrong changes
nothing, because the flag has nothing to bite on. The old note on
`OvlFunc_common2_41c` recorded exactly that and drew the right conclusion —
such a function cannot serve as a test of the hypothesis.

## Screening against a `.sym` addition without touching the tree

`tryc.py` resolves `_MSG_*`, `_AREA_*` and friends from `/work/<name>.sym`, and
`ROOT` is fixed at `/work`. Bind-mount a modified copy **over** that one path,
read-only, in the same `docker run`:

    docker run --rm -v "$PWD:/work" \
      -v "$PWD/scratch/message_plus.sym:/work/message.sym:ro" \
      -w /work goldensun-build python3 tools/tryc.py ...

The host file is untouched. This turns "1 differing, and it is the symbol I would
have to add" into a real `OK` **before** anyone edits a linker fragment. Works
for `wram.sym`, `file_table.sym` and `area.sym` too.

## A worklist `ref` path goes stale the moment its `.s` is split

Splitting renames the file. A worklist generated before the split points at a
path that no longer exists. `showfunc.py <name>` resolves the current path in one
call **and** catches the already-elevated case, since `tryc.py` refuses a
generated `.s` as a tautology. Make it step 0 for every function rather than
trusting the recorded path.

## Exporting labels for a split is iterative — drive it from the tool's output

`split_s.py` reports the labels it needs exported, but only the ones it hits
first. `asm/overlays/rom_7795e8/ovl_30_c_c.s` took four rounds (`.L14d4 .L14dc
.L16b0 .L16b2 .L16b4` → `.L16bc` → `.L16b6 .L16ba`). Loop on the tool's output
rather than reading the file, and note that a shell loop passing a
newline-separated label list unquoted will silently do nothing — pass the labels
as separate argv entries.

Exports are byte-neutral: verify `make compare` after the export and **before**
the split, so the two changes stay separable.

## Withholding the prototype is a real lever for ARGUMENT ORDER — and it is narrow

`OvlFunc_927_2009520` makes five 6-argument calls to one callee. gcc emitted
`mov r0, #2` FIRST at every site; the ROM emits it LAST at three of them, second
at one, and third at another — the variation is the scheduler, so the source is
uniform and something upstream of scheduling differs.

Nothing about the call sites moved it: `-fno-schedule-insns` (13 of 76),
`-fno-schedule-insns2` (37, much worse), `-fno-rerun-cse-after-loop` (13), and a
carried `int kind = 2;` local for the first argument (13). **Deleting the
`extern` declaration entirely — so the callee is K&R implicit `int` — matched
exactly.** 13 differing to zero, from removing one line.

The mechanism is that without a prototype gcc has no parameter types to convert
against, so the argument expressions are expanded in a different order. It costs
a warning and nothing else.

**But it does not generalise to the arg-interleave class.** Measured on the two
parks with the closest-looking signature:

| park | best | with no prototype |
|---|---|---|
| `OvlFunc_960_2008d24` | 8 of 65 | **14** (worse) |
| `OvlFunc_948_2009df8` | 18 of 40 | 18 (no change) |

So: try it, it is one line and it sometimes wins outright — but a park that
already records "prototype removed, no change" has genuinely tested it, and the
lever is not a reason to reopen the class.

### Batch 147: the lever is broader than the paragraph above says, and here is how to aim it

The verdict "it does not generalise to the arg-interleave class" was drawn from
two parks. Four functions responded to it in one round, so the scope needs
restating. What changed is not the mechanism; it is knowing **which** declaration
to delete.

**One direction only: `mov r0` moves LATER.** Withholding a prototype puts the
first argument's load at the END of the setup. It cannot put it in the MIDDLE of
another argument's `mov`/`lsl` pair — that is the dominating-block naming lever,
below, and the two pull opposite ways. `OvlFunc_954_20095e0` needs both at once
and its header says which sites want which; `OvlFunc_955_20092f0` needed both as
well. **Read each site off the ROM before reaching for either.**

**No declaration is not the same as an empty parameter list.** `extern void
f();` was screened on three functions here and moved nothing on any of them. The
`extern` line has to be gone.

**The fix is not always at the call that shows the residue.**
`OvlFunc_952_20085a4` had a park note naming its blocker exactly: two arms of a
branch emit `__ActorMessage`'s two arguments in opposite orders, and the ROM
disagrees with us in one of them. Dropping `__ActorMessage`'s prototype does
nothing. Dropping the prototype of the call **immediately before the branch**
fixes both arms. Sweep the neighbours, not only the offender.

**Delete them one at a time.** Over the 62 saved candidates under `scratch/`
with a reference, deleting every `extern void` at once made 37 worse, 21
unchanged and 4 better. `tools/protolever.py` does the per-declaration greedy
hill-climb — score, try each deletion alone, keep the best, repeat — which is
how `20085a4` was found without reading any assembly. It only touches
`extern void` declarations: a callee whose result is used changes type when its
declaration goes.

**Where it will not help.** The dominating-block half of the pair needs a
dominating block. On a straight-line function that wants `mov r0` in the middle
there is nowhere to put the definition, and naming the constant at the top of
the function instead lengthens a live range across a call so gcc allocates a
callee-saved register rather than rematerialising — the function grows a
push/pop pair and gets worse. Measured on `OvlFunc_967_2008308` (60 differing →
78) and `OvlFunc_911_20082b4` (33 lines → 39). Both are still parked.

### Loop-invariant addresses: one lever keeps them IN, another lifts them OUT

A loop that touches two globals can want opposite treatment for each, and
`Func_801b398` needs both in the same loop.

**To keep an address inside the loop, write the loop with `goto`.** The ROM
rebuilds `mov r2, #0xe8 / lsl r2, #2 / add r3, r5, r2` on every pass. Written
`do { … } while (cond)` gcc recognises a loop, lifts the address out, and needs
an extra callee-saved register to hold it — visible as `mov r7, r8 / push {r7}`
in the prologue and a line-count overshoot. An explicit `goto top;` defeats the
loop recognition and the address goes back inside. (Same lever as the batch-145
`check_dbra_loop` note, working here against loop-invariant motion.)

**To lift one out, give it a pointer local before the loop.** The ROM loads
`ldr r7, =gKeyPress` once before the loop and pushes r7 to keep it. Naming the
global directly puts that load inside; `volatile unsigned int *k = &gKeyPress;`
before the loop puts it where the ROM has it.

67 lines/66 differing → 62/46 with the goto → exact with the pointer local.
**Read which the ROM does for each address before reaching for either** — a
prologue that pushes one register too many is the tell for an unwanted hoist.

### A store wants a POINTER local, not an offset local

When the ROM computes a destination into a register and stores at immediate
zero, and we emit a register-offset store, the source is holding an OFFSET where
it should hold a POINTER:

    rom   add  r3, r6, r2 / strh r5, [r3, #0]      <- address in a register
    ours  strh r5, [r6, r2]                        <- register-offset form

`*(short *)(p + o) = z` with a named offset `o` gives gcc the register-offset
store, which is **one instruction shorter every time** and so shows up as a
line-count shortfall rather than a rename. Writing it
`q = (short *)(p + o); *q = z;` recovers the `add`. On `CutsceneStart` two such
stores were four lines of the gap: 58 lines and 48 differing became 60 and 39.

The offset local is still right when the ROM *mutates* it (`add r2, #2` between
two stores off one base) — that pattern needs both, an offset variable to mutate
and a pointer variable to store through.

**Narrower than it first looked (batch 152).** The pointer local forces the
`add` only when something else keeps the offset alive — on `CutsceneStart` the
offset was mutated *between* the address and the store. On `Func_80a8088`
nothing did, and gcc folded `w = (void **)(p + o); *w = r;` straight back into
the register-offset store. What worked there was **dropping the shared offset
variable** and writing both offsets as plain constants: gcc then computes the
first address with an `add` and still derives the second from it unprompted.
33 differing → 3.

So: if a pointer local does not take, the offset variable is the thing holding
gcc to the register-offset form — remove it rather than adding more locals.

### A halfword read's ADDRESSING MODE names its signedness

thumb has `ldrh rD, [rB, #imm]` but **no `ldrsh` with an immediate**. A signed
halfword field must therefore build its offset in a register first. So the
reference tells you the type outright:

    ldrh  rD, [rB, #imm]   ->  the field is UNSIGNED
    ldrsh rD, [rB, rO]     ->  the field is SIGNED

`mov r2, #0xa / ldrsh r3, [r5, r2]` where the ROM has `ldrh r3, [r5, #0xa]` is
not a scheduling difference and not a lever to hunt for -- it is one character
of the struct declaration. On `Func_801b148` that character was 58 differing
lines. The same function's other halfword field genuinely is signed and keeps
its register-offset `ldrsh`, so read each field separately.

This is the cheapest type check in the corpus and it costs one glance at the
operand. Do it before assuming a residue is the allocator's.

### A narrow store of a literal: cast pools it, typed field builds it, named local builds it and costs a register

Measured four times in batch 148 and it settles a recurring confusion.
`*(short *)(e + 6) = 0x80 << 8` compiles to `ldr r3, =0xffff8000` -- gcc puts
the constant in the pool, sign-extended to the destination's width -- where the
ROM has `mov r3, #0x80 / lsl r3, #8`. The same for a byte store. Three
spellings, three different results:

| spelling | constant | register |
|---|---|---|
| `*(short *)(p + K) = V` (cast) | POOLED | -- |
| `e->f6 = V` (typed field) | mov/lsl | SCRATCH |
| `v = V; *(short *)(p + K) = v` | mov/lsl | CALLEE-SAVED |

**Prefer the typed field.** It is the only one that gets the ROM's instructions
without spending a register, and on `OvlFunc_943_20097a0` that mattered: the
function has exactly two callee-saved registers and both are already claimed by
values that genuinely live across calls, so naming the two single-use constants
would have taken registers they need. 37 differing to 6.

Reach for the NAMED LOCAL only when the ROM itself keeps that value in a
callee-saved register -- look at the prologue. `OvlFunc_943_20097a0` pushes
{r5, r6} and holds `0xa0 << 7` and a zero there across the whole body, so those
two are named; the two single-use constants are struct fields.

This also refines "do not name zeros". A zero the ROM keeps in a pushed register
across calls IS a named local; the rule is about zeros that gcc would otherwise
rematerialise beside each use.

**Where it does not reach.** When the destinations are many different offsets
off one pointer and there is no struct to hang them on, neither spelling wins --
see `src/non_matching/ovl_7892c8/200874c.c`, where the ROM pools a value we mov
and movs a value we pool in the same block.

### `if (c) goto X; goto Y;` compiles to `b!c Y; b X` -- the sense INVERTS

gcc expands a conditional goto as "jump-if-false to the next statement", and
jump threading then folds the following unconditional jump into the branch. The
branch you get is the OPPOSITE of the one you wrote:

    if (c) goto X;      ==>     b!c  Y
    goto Y;                     b    X

So to reproduce a ROM that reads `bne loop / b out`, write the test the other
way round -- `if (!c) goto out; goto loop;`. Two sites on
`OvlFunc_941_20091b8` were four of its six differing lines, and inverting both
closed the function exactly.

**This applies only to a conditional goto IMMEDIATELY FOLLOWED BY an
unconditional one.** The six single-branch tests in the same function --
`if (p()) goto L;` with a fallthrough after it -- all came out right written the
natural way, and inverting those breaks them. Check which shape the site is
before flipping anything.

### Explicit gotos are how you control BLOCK ORDER

Two functions in batch 148 needed the case bodies laid out where the ROM puts
them, and neither an if/else-if chain nor a switch produces that layout:

  * an if/else-if chain puts each body INLINE behind a `bne`, so the tests are
    separated by their bodies. `OvlFunc_927_20099b8` scored 99 differing.
  * a `switch` SORTS the cases and emits a balanced compare tree
    (`cmp #9 / beq / cmp #9 / bgt / ...`), which is visible immediately -- a
    `bgt` or `blt` against a case value that is not a range bound means gcc
    built a tree and the ROM did not.

The ROM shape in both was a contiguous run of tests branching FORWARD to bodies
that appear in source order after them. Writing that with labels and gotos took
`OvlFunc_927_20099b8` from 99 differing to 37 in one edit. The remaining 37 were
argument-setup, a separate problem.

A `switch` is still right when the ROM's own compare order is SORTED -- that is
the tell that gcc built the tree from a switch rather than the author writing a
chain.

**BOUNDARY (batch 149): try the ordinary form FIRST.** On
`OvlFunc_971_2008e10` five goto arrangements were screened before the plain
`for (;;) { ... if (c) break; ... }` was tried at all, and the plain form beat
every one of them -- 41, 40, 79, 35 differing against **29**.

The reason matters: **a two-instruction block reached by `goto` is DUPLICATED
INLINE by gcc unless it happens to sit adjacent to the branch.** Move the label
to where the ROM has the block and gcc copies the body to the branch site
instead of jumping to it, which is worse than where you started (79). An
ordinary `break` produces the out-of-line block and the branch to it for free.

So the rule is: reach for gotos when the ROM's block order cannot be expressed
with ordinary control flow -- a case chain whose bodies are all out of line, an
arm that rejoins somewhere the structured form cannot reach. Not before trying
the structured form and reading its diff.


### The declaration is a PER-CALL-SITE choice: two declarations of one callee

When a callee is called more than once and only SOME sites have the wrong
argument order, changing its one declaration moves every site together and can
only ever fix a subset. Give the odd site its own declaration, aliased to the
same symbol:

    extern int  __CloseUIBox(int h, int n);
    extern void CloseBoxV(int h, int n) __asm__("__CloseUIBox");

`OvlFunc_971_20091bc` calls `__CloseUIBox(h, 1)` twice with identical arguments
and the ROM emits `mov r0, r5 / mov r1, #1` at both; with one `int` prototype
gcc gets the second site right and reverses the first. Routing the first call --
the one whose result is discarded -- through the void-returning alias matched it
exactly, and its twin `OvlFunc_971_2009228` with it. The park had recorded this
as unreachable precisely because it had only tried changing THE declaration.

**The return type is what selects the order** at a site whose result is unused.
Screened and rejected on the same function, all unchanged: storing the discarded
result, an alias with an empty parameter list rather than a void return, and the
alias applied to the returned site instead. Deleting the declaration outright
also matches here, but only because that makes both sites unprototyped -- the
same fix by accident, and it would not survive a third call site.

The `__asm__("name")` alias emits nothing and costs nothing; the tree already
uses the same construct for label symbols.

### Invariance under call-site rewrites says WHERE the lever is, not that there is none

`OvlFunc_955_20092f0` was parked mid-round at 123 of 123 lines with 15
differing, on the strength of seven spellings of its calls leaving the count
**exactly** unchanged: constants hoisted, constants inline, a shared zero
variable, per-call-site locals, the decrement folded into the argument, an empty
parameter list, and unprototyped callees. The park note said the residue must
live below the source. It lived in the declarations, and deleting three of them
closed the function within the hour.

A count that does not move under changes to the call site is real evidence, and
what it is evidence **of** is that the call site is not where the lever is. It
says nothing about whether one exists. This is the same failure mode as the
three corpus-count errors recorded elsewhere in this file, in a new place: a
measurement that is sound, generalised one step too far.

## A pooled constant that FITS a thumb immediate is a symbol tell, and `area.sym` is the first place to look

`OvlFunc_960_2008d24` compares a `gState` halfword against `0xa5` and the ROM
does it as `ldr r3, =0xa5 / cmp r2, r3`. Thumb `cmp Rn, #imm8` covers 0–255, so
gcc had no reason to pool it. `_AREA_a5` was already in `area.sym`; spelling the
test `== (int)(&_AREA_a5)` took the screen from **62 differing to 17**.

This is the counterpart to the batch-116 correction that `ldr rN, =0` is *not* a
symbol tell. The distinction is whether the constant is **zero** — gcc really
does pool a literal 0 on a cross-jumped tail — versus any other small value,
where a pool load still means a symbol.

## `-ffixed-r7`: when the ROM spends r8 and you spend r7

`OvlFunc_945_200d6dc` needs three callee-saved registers. gcc takes r5, r6, r7.
The ROM takes r5, r6 and **r8** — which is not free in thumb: it costs
`mov r6, r8 / push {r6}` at entry and `pop {r3} / mov r8, r3` at exit, four
instructions gcc had no reason to spend. Our version was 55 lines against the
ROM's 59, and those four are the whole gap.

`--cflags "-ffixed-r7"` reserves r7 without any other change: **55 → 59 lines,
41 differing → 9**, and the residue was a plain r5/r6 swap that two separate
locals for the two script pointers then closed. It is now a `FIXEDR7_CFLAGS`
group in the Makefile with one explicit rule.

`-fno-omit-frame-pointer` also reserves r7 and is the wrong tool: it reserves
the register *and* emits frame setup, giving 61 lines. What is wanted is the
reservation alone.

**Do not reach for this generally.** Measured against the two register-allocation
parks with the closest-looking signature:

| park | best | with `-ffixed-r7` |
|---|---|---|
| `StartThunder2` | 32 of 74 | 32 (no change) |
| `Func_80f7df0` | 18 of 30 | 18 (no change) |

The tell that it *is* worth trying is specific and mechanical: **the ROM saves a
high register (r8–r11) and your version does not, and the line-count gap is
about four.** Where the counts already agree, reserving r7 has nothing to fix.

## Straight-line call scripts: check for a repeated constant BEFORE writing C

The easiest class left is functions with many `bl`s and no labels —
`tools/draft_script.py` writes the first draft and the work is review. But two
of them in a row failed on the same thing, and it was visible in the assembly
first.

**A constant used at two call sites in a straight-line function is
unreachable.** gcc hoists it into a callee-saved register and reloads it with a
`mov`; the ROM rebuilds it. There is no control-flow boundary to satisfy the
other half of the constant-CSE precondition, and nothing substitutes:
`-fno-rerun-cse-after-loop`, `-fno-gcse`, `-fno-expensive-optimizations`,
`-fno-cse-follow-jumps`, `-fno-force-mem`, `-fno-thread-jumps`, `-O1`, and
spelling the constants as `const.sym` symbols all leave the instruction COUNT
wrong. See `src/non_matching/ovl_77dd1c/200bc48.c` for the one-constant
specimen and `.../ovl_77a7c8/2009c08.c` for the two-constant one.

Two refinements the specimens produced:

* **gcc hoists a POOL LOAD too**, not just a `mov`+`lsl` build — even though a
  reload and the replacing `mov` cost the same one instruction.
* **One symbol used twice is hoisted like an integer.** The rule that "two
  DISTINCT symbols of equal value reload" is about two different symbols;
  `(int)&_CONST_16f` at both sites changes nothing.

`tools/script_candidates.py` ranks the class by repeated *expensive* constant
(pool loads and shifted builds; bare `mov rN, #imm8` is rematerialised for free
and must not be counted — counting it reported 40 of 41 functions as blocked).
In band 30–70 there are 68 such scripts and **40 have no repeated constant**.
The first two picked from the clean list matched on the first screen.

## A THIRD label false-negative shape: two returns cross-jumped to one epilogue

Batch 115 recorded the `.pool_aligned`-in-a-loop shape. Here is another, and it
has nothing to do with pools.

`Func_80b60a0` screens `10 differing of 74, first diff at 54` and is
**byte-for-byte identical** (168/168 bytes). The cause: our C has two distinct
`return 0;` statements, gcc cross-jumps them and emits two labels at the same
address, and the ROM's disassembly has one. Every position after shifts by one.

> If the diff's disagreeing region contains **only label lines**, run the byte
> check before changing a single spelling.

Both false-negative shapes so far fail in the SAFE direction — they report a
correct function as wrong. That is the opposite of the batch-112 hazard, and it
means the cost is wasted screens rather than a bad commit.

## `mov rd, rs` and `add rd, rs, #0` are the same instruction — for LOW registers

`mov r2, r3` and `add r2, r3, #0` both assemble to `0x1c1a`. The ROM's
disassembly writes `mov`, gcc-2.96 writes the `add` form. `Func_80a19a0`
screened 1-of-79 DIRTY while being byte-identical purely because of this.

`tools/tryc.py` now folds them — **only for r0–r7 on both operands**. `mov r8, r3`
is `0x4698`, a genuinely different instruction with no `add` equivalent, so
folding a high register would hide a real difference. This sits beside the
`ldrb [r3]` / `ldrb [r3, #0]` fold.

## `(idx << 1)` and `idx * 2` are not interchangeable in a register-offset store

On `Func_8019000`, `*(unsigned short *)(map + idx * 2)` gives
`strh r5, [r3, r6]` — index first — and `*(unsigned short *)(map + (idx << 1))`
gives the ROM's `strh r5, [r6, r3]` — base first. The array-subscript form and
`(unsigned int)map + idx*2` both give index-first. That one spelling was worth
2 of 73. The existing operand-order note covers subscript versus pointer
arithmetic; it does not cover `<<1` versus `*2` *inside* the pointer arithmetic.

## Hoisting a constant's assignment ABOVE an unrelated load flips high-register allocation

`Func_80a6a00` sat at exactly 5 of 71 with `base` in r8 and `p` in r10 where the
ROM has the reverse. Three declaration-order permutations did nothing; swapping
the two assignments made it worse. Hoisting `base = 0x86 << 2;` to be the
**first statement of the function**, above the unrelated `iwram_3001f2c` load,
was exact on the first screen.

The mechanism is worth knowing because it makes the lever predictable: the
lengthened live range lowers the allocno's priority
(`floor_log2(n_refs) * n_refs / live_length`) and pushes it one slot down
`REG_ALLOC_ORDER`, whose call-saved sequence for Thumb is
**r5, r6, r7, r8, r10, r9, r11** (`arm.h:989`). r10 comes before r9 — so
"the ROM used r10 and we used r9" is a one-slot priority difference, not noise.

## The argument-list case of constant CSE, and a corpus test for it

The constant-CSE blocker also fires **inside a single call's argument list**,
where there is not even a statement boundary to reason about:

* `__Func_80933f8(-1, -1, -1, 0)` — the ROM builds `-1` three times
  (`mov #1` / `neg` ×3); gcc builds it once and copies.
* `__Func_8012330(0x80 << 10, 0x80 << 10, 0x80 << 9)` — gcc commons the two
  `0x20000`s.

**Corpus test: 0 of the generated `.s` files contain two consecutive
`neg rN, rN`.** So two `-1` arguments to one call is unreachable for this
compiler — not merely unreached. Check that before spending screens.

`tools/blocked_cse.py` misses this: it looks at pool constants and
statement-level repeats, not at repeats *within one argument list*, and it let
three such functions through in one 12-function worklist. Widening it to "the
same non-immediate constant appearing more than once in a single call's
argument list" is the outstanding fix.

## `sub sp, #N` + `mov rX, sp` for a stack vector needs a named `int *p = v;`

Both `Field_Growth` and `OvlFunc_883_200d75c` build a three-word vector on the
stack, pass it to `vec3_translate`, then read it back. Using `v[0]`/`v[1]`/`v[2]`
directly puts `mov r8, r3` before `mov r6, sp` (2 of 65); assigning `p = v;` once
and writing everything through `p` was exact. Same family as "naming an
intermediate stops gcc folding it", but the trigger is the order in which the
frame pointer is materialised.

### Narrowing (batch 151): a named pointer does not survive offset 0

`OvlFunc_common2_380` holds TWO stack addresses (`mov r3, sp` and
`add r4, sp, #8`) and stores two words through the first. Naming both pointers
gets the second object's address and the offset-4 store right, and leaves
**exactly one** instruction wrong:

    rom    mov r3, sp / str r0, [r3, #0] / str r1, [r3, #4]
    ours   mov r3, sp / str r0, [sp, #0] / str r1, [r3, #4]

gcc folds the *offset-0* store back to `sp` while keeping the pointer for the
offset-4 store — an asymmetry no amount of pointer naming fixes, because at
offset 0 the pointer and `sp` are the same value and the sp-relative encoding
is equally short.

**The fix is to stop having two locals.** The 8-byte input block and the
20-byte output record are contiguous and exactly fill the frame, so they are
one struct; `&s.in` is then a genuine subobject address rather than "the stack
pointer", and both stores go through it. Exact.

So when a candidate has two adjacent stack objects passed as two pointers to
the same callee, and a named pointer gets you to within one offset-0 store,
**try them as one struct before parking on register allocation.** Check the
frame size first: if `sub sp, #N` equals the sum of the two objects with no
padding, that is the tell.

### Correction (batch 152): one struct, but a pointer PER SUBOBJECT

`OvlFunc_common2_28c` has two 8-byte operands at `sp+0` and `sp+8` and the ROM
holds **two** address registers, storing `[r4, #0]/[r4, #4]` and
`[r5, #0]/[r5, #4]`. Making them one struct and pointing a single pointer at
the struct gives one base with offsets 0/4/8/0xc — the right addresses, the
wrong instructions. What matches is one struct with a pointer local to **each
member**: `pa = &v.a; pb = &v.b;`. So the rule is "the objects must be
subobjects", not "there must be one pointer".

### And it does NOT apply when the object IS the whole frame

`OvlFunc_common2_304`'s record is the only local, so `&d == sp` exactly and gcc
can rematerialise it at will. A named pointer there makes it **worse** (52
differing → 62), because it gives gcc nothing to hold that it cannot recompute.
Park that case on allocation rather than spending spellings on it.

## Frame layout follows DECLARATION ORDER, last-declared lowest

Several same-sized buffers land at offsets in the **reverse** of their
declaration order. On `OvlFunc_common2_28c` the ROM's offsets are `v=0x00,
third=0x10, rb=0x24, ra=0x38`, and the declarations that produce them run
`ra, rb, third, v`.

This is worth reaching for early rather than guessing: read the offsets off the
ROM's `add rN, sp, #imm` instructions, sort them, and declare in the opposite
order. It costs one edit and removes a whole class of "right code, wrong
offsets" diffs.

## POINTER BIRTH ORDER decides which register each pointer gets

Assigning several address locals up front is not the same as assigning each one
where the ROM first needs it. On `OvlFunc_common2_28c`, hoisting both operand
pointers to the top of the body hands the wrong pointer the lower register and
swaps r5/r6 through the **entire** function body — 8 differing lines from one
misplaced assignment. The ROM's birth order is `&a, &ra, &b`, so the second
operand's pointer has to be assigned *after* the first operand's stores.

Read the birth order straight off the ROM: the order in which `add rN, sp, #imm`
and `mov rN, sp` first appear IS the order the source assigns them in. When a
diff is "everything is right but two callee-saved registers are swapped", check
this before concluding it is the allocation coin flip.

## Screening a TU whose flags come from a rule your scratch path does not match

`tools/tryc.py` reads per-file flags from the Makefile by matching the SOURCE
path, so a file in `scratch/` gets the tree defaults even when its real home has
an override. `-mno-thumb-interwork` cancels `-mthumb-interwork`, and a later
`-fcall-saved-r4` overrides an earlier `-fcall-used-r4`, so a common2 function
screens correctly from any path with:

    --cflags "-mno-thumb-interwork -fcall-saved-r4"

The `common2_254` park had recorded that no meaningful screen was possible for
its file. It was, and the function went from "diverging from the first
instruction" to two.

## A 64-bit result wants a union written AFTER the join

Software 64-bit arithmetic returns the pair in r0/r1. Building the result as
`((unsigned long long)hi << 32) | lo` costs five instructions gcc cannot see
through — two `mov #0` and two `orr` — where the ROM simply moves the pair.
Use a `union { unsigned long long q; struct { u32 lo, hi; } h; }`.

WHERE the union is written decides the block structure, and this is the part
that is easy to get wrong:

| union assigned | result |
|---|---|
| inside both arms | gcc threads each arm straight to the epilogue, the join disappears, output is SHORTER than the ROM |
| once after the join, from plain u32 temporaries | the ROM's shape: arms compute into a register pair, one join materialises it |

`OvlFunc_common2_41c` needs the second, plus a `goto` to put the `count >= 32`
arm first. That gets it to the ROM's exact 27 lines with the same block order
and branch senses; what is left is only a register rename, so it is parked on
the coin flip rather than on shape.

Also: `v >> n` for a variable `n` on a 64-bit value emits a call to
`__lshrdi3`. Any ROM function that does the shift inline is doing the word
arithmetic by hand, and the source must too.

## The HImode-literal rule is narrower than stated: only 0 and >= 0x8000

The existing rule ("gcc-2.96 has no immediate alternative for an HImode
constant, so use an `int` local") over-applies. Measured across four functions:

| stored through a `u16`/`short *` | plain literal gives |
|---|---|
| `1`, `0x28`, `0xf0`, `0xfa` | the ROM's `mov rN, #imm` — **no local needed** |
| `0` | `ldr r3, =0x0` — needs an `int` local |
| `0xb000`, `0xfc88` (bit 15 set) | a pool load — needs an `int` local |

So: **plain literals are correct for 1…0x7fff; only `0` and values ≥ 0x8000
need the local.** `Func_801c188` stores four `u16` members with bare literals and
is exact.

The ≥ 0x8000 case has a trap in front of it. Through a *signed* `short *`,
`0xb000` becomes `-0x5000` and pools as `0xffffb000`; making the pointer
`unsigned short *` fixes the pool word but gcc still pools it. Both changes are
needed — `OvlFunc_884_2008674` went 37 differing → 37 → **exact** across the two.

And the local's cost is not only its own instruction: in `Func_8016230` a pooled
zero also produced a spurious `b`/label pair fifteen instructions later, so
`int z = 0;` took it from 51 of 72 to exact. **A stray branch downstream of a
pooled zero is a symptom of the same defect.**

## `volatile` is a reading, not a hack: two signatures

**A global the ROM RELOADS is `volatile`.** The tell is one address load and two
value loads with no call between:

    ldr r6, =gKeyHeld           <- address once, kept in a callee-saved register
    ldr r3, [r6]  ...  ldr r3, [r6]

Two textual reads in C are *not* enough — gcc CSEs them. `OvlFunc_880_20081fc`
went 95 of 100 → 10 on adding `volatile` to `gKeyHeld` alone, and the two
instructions it restored were exactly the second reads.

**A store immediately followed by a load-back of the same stack halfword is a
`volatile` LOCAL.** `volatile unsigned short t;` is what puts the local in
memory, forces the `mov rX, sp / add rX, #2` address register (Thumb has no
sp-relative `strh`), and makes the write/read-back pair real.

Where a `-fno-gcse` rule and a `volatile` both match, prefer `volatile` — it
costs no per-file flag group. `OvlFunc_947_200a230` is wired that way.

## `-fno-gcse` reaches a re-read that no `cse`-family flag does

`OvlFunc_947_200a230` reads a global twice with a branch between; gcc caches the
first load. `-fno-cse-follow-jumps`, `-fno-cse-skip-blocks` and
`-fno-rerun-cse-after-loop` are all identical at 5 differing; **`-fno-gcse` is
exact.** The redundancy is eliminated by *global* CSE, which is why the local-CSE
flags miss it.

## `-fno-schedule-insns2` is an actively misleading probe

On every function this round that reached a scheduling-shaped residue, it moved
the first differing position back to ~1 and multiplied the count: 3 → 15,
17 → 41, 8 → 25, 2 → 23. **It is never the answer to a one-instruction
scheduling difference, and it destroys the evidence you were reading.**

## Name the store's DESTINATION pointer when the ROM computes the address first

Distinct from the operand-order lever. Where the ROM computes `dst = base + K`
as a whole instruction *before* the value:

    ldr r3, =0x604 / add r2, r7, r3     <- destination first
    ... build the value ...  /  str r0, [r2]

`dst = (u8 **)(buf + 0x604); ... *dst = src;` took `Func_801c188` from 13 of 64
to exact — **and dissolved a six-instruction difference twenty positions
earlier.** "Fix the earliest difference first" is the usual advice; this is a
concrete case where the later difference was the *cause* of the earlier one.

## Two more one-screen levers

**`i = 0;` as its own statement is not the same as `for (i = 0; …)`.** The
`for`-init is emitted at the end of the loop preheader; a separate statement is
emitted where you write it. `Func_80798e0`: 3 of 100 → exact.

**Deleting a single-use local reaches an r0↔r4 exchange.** `OvlFunc_880_20081fc`
was 10 of 102, every line one transposition; `t = L16ba; if (t != 0)` →
`if (L16ba != 0)` was exact. Try it alongside the declaration-order lever on any
same-length park whose differences are one clean transposition.

**Deleting a loop-bound local moves the bound into a high register.** With
`n = o->f27;` hoisted, gcc puts `n` in a low register and grows the push list;
`for (i = 0; i < o->f27; i++)` lets gcc hoist the load itself and copy it to r12.
`OvlFunc_888_200a5c4`: 8 differing → exact. The visible symptom is an extra
callee-saved push rather than an r2/r3 swap.

## Strict aliasing can SINK a store, and a `char *` lvalue pins it

The existing `-fno-strict-aliasing` note is about a load hoisted above a store.
The converse happens too: in `OvlFunc_common1_1254` a `e->f7 = 0` store sank
~25 instructions to sit beside a zero it shared with later `int` stores, and no
naming, ordering or scheduling flag reached it. Writing it as
`*((unsigned char *)e + 7) = zero;` — a `char` lvalue is alias set 0 and cannot
move past the other stores — matches on **default flags**. `-fno-strict-aliasing`
also matches; the char-pointer form is strictly better because it needs no flag.

> When a store lands far from where the ROM has it and nothing else differs, ask
> whether alias sets let gcc move it, before reaching for the scheduler.

## Two live pointers to nearby fields: use struct members, not two locals

Where the ROM has `add r3, r5, r2 / add r2, #2 / strh / add r3, r5, r2 / strh`,
naming the addresses as one local, as two locals, computing both before either
store, and an `off += 2` walk all leave the *second* store folded into
register-offset addressing and one instruction short. Casting the base to a
struct and writing `t->f290 = id; t->f292 = style;` is exact — gcc derives the
second offset with `add r2, #2` itself. Same "offset in the TYPE" lever, but the
failure it fixes is *address folding into the store*.

## Negative: the symbol tell does NOT govern argument-setup order

The doc says a pool load of a SYMBOL is not hoisted where an int constant is.
That governs hoisting **across a call**, not the order of argument setup *within*
one call. Measured on `OvlFunc_881_2009a98`: replacing both constants of
`__MapActor_SetSpeed(8, 0x9999, 0x4ccc)` with symbol addresses emits the two pool
loads before `mov r0, #8`, byte-for-byte the same as the literal spelling. Do not
spend a `.sym` addition on argument order.

## A duplicated `.L` label in a ref is not necessarily a defect

`asm/overlays/rom_7ac2d8/ovl_d58_a.s` defines `.Ld90:` on two consecutive lines,
which makes `tryc.py` report 6 differing of 69 for a byte-identical function.
That looks like a corpus defect and it is tempting to delete one.

**Deleting it makes the screen worse — 6 differing becomes 47.** gcc genuinely
emits two labels at that address from the correct C, so the duplication is a
faithful transcription of the original object, not a transcription error. The
function is byte-identical (164/164) *with* the duplicate present. Left as-is.

## The `mov #K / mov #0 / neg / mov #0` argument idiom: 11 functions, unreachable

`f(0, 0, -8)` compiles to `mov r2,#8 / neg r2,r2 / mov r1,#0 / mov r0,#0`; the
ROM has `mov r2,#8 / mov r1,#0 / neg r2,r2 / mov r0,#0` — the `mov r1,#0` slots
between the two halves of the `-8` build, filling the dependency stall.

**Zero of the 2987 generated `.s` files in the tree contain that four-line
sequence.** gcc-2.96 as configured here never emits it. Eleven functions carry
the idiom and every one of them is capped at the same two-line residue; the full
list is in `src/non_matching/ovl_7c460c/2008c74.c`.

Eleven spellings and three flags are measured in that park. The only one that
moves the count moves it the wrong way, and `-fno-schedule-insns` does not touch
it — so whatever reorders the pair is not the pre-reload scheduler.

**The general technique is the point:** when a residue is a fixed short
instruction sequence, grep the *generated* `.s` files for it before spending
screens. If gcc has never produced that sequence anywhere in the corpus, no
spelling will make it. Two classes have now been closed this way — this one and
two consecutive `neg rN, rN`.


## RETRACTED: "the split constant build is unreachable"

I claimed here that `mov rA, #K / mov rB, #K2 / lsl rA, #n` appears 0 times in
the 2987 generated `.s` files and that the class was therefore unreachable.
**That was wrong, and it was wrong because the detector was broken.**

Generated `.s` files are gcc's own assembly output and use a different notation
from the ROM's disassembly: **decimal immediates** (`mov r2, #224`) and the
**three-operand** shift form (`lsl r2, r2, #1`). My regex required the ROM's
two-operand `lsl r2, #1`, so it matched nothing in the generated corpus. The
"0 of 2987" measured my regex, not the compiler.

Re-run with both operand forms accepted, and with a positive control:

| pattern | generated `.s` files |
|---|---|
| adjacent `mov`/`lsl` build (positive control) | **777** of 2987 |
| split `mov` / `mov` / `lsl` | **51** of 2987 |
| any `neg` at all (positive control) | **241** of 2987 |
| two consecutive `neg rN, rN` | **2** of 2987 |

So gcc-2.96 **does** emit the split shifted build — 51 files — and it does emit
two consecutive `neg`s, twice. Neither class is unreachable. The functions parked
on them are parked on "I have not found the spelling", not on "no spelling
exists", and the parks say so.

**The corpus test is still the right idea. Two rules for using it:**

1. **Always run a positive control.** Count how often the *unsplit* or *simpler*
   form of the shape appears. A zero with no positive control is
   indistinguishable from a broken pattern, and that is exactly what happened
   here.
2. **Write patterns against gcc's notation, not the ROM's**, when searching
   generated files: decimal immediates, three-operand shifts. The two corpora do
   not use the same spelling for the same instruction.

This also downgrades agent1's report that "0 of the generated `.s` files contain
two consecutive `neg rN, rN`" — the true count is 2, and with `neg` appearing in
only 241 files to begin with, a low count is weak evidence rather than proof.


## Perturb one copy of a CSE-able constant to see what is underneath it

Before parking anything on constant CSE, change the **second** occurrence to a
nearby different value so gcc cannot common it, and screen. The result says
whether the CSE is the only blocker. One screen, and it changes what the park is
worth:

* `OvlFunc_891_2009b44`: 74 differing → **2 of 66**, and those two are exactly
  the literals that were changed. The function is otherwise exact — a far
  stronger park than "74 differing".
* `OvlFunc_955_2008b38`: 29 → **12 of 63**, revealing a *second*, independent
  arg-interleave blocker the CSE was hiding.

## `--quiet`'s count and `--full`'s count are different numbers

`--quiet` reports raw differing lines; the last line of `--full` reports
*instructions in disagreeing regions* after alignment. The gap can be large —
`OvlFunc_971_2008f8c` reads 17 at `--quiet` and is **2** aligned.

**Ranking parks by the `--quiet` number mis-orders them.** Always take the
aligned number, and record that one in the park.

## CONFLICT: is the declaration-order lever real?

Batch 115 recorded "declaration order alone, statements untouched" as a
one-screen lever, from `OvlFunc_956_20085e0` going 13 of 54 → exact on swapping
`int m; int n;` to `int n; int m;`.

Two later measurements contradict it. All **24** permutations of four locals on
`OvlFunc_932_200b5ac` gave byte-identical output; four orders of seven locals on
`Func_80a6a98` likewise. And on `Func_80f7df0` five declaration orders all gave
18 differing.

Both results stand — the original was a real screen. The honest reading is that
declaration order is **usually inert and occasionally decisive**, so it is worth
exactly the one screen it costs and nothing more. What reliably moves registers
is the order of the **assignment statements**: on `FieldMove_NoTarget` the six
orderings of three header assignments span 9–13 differing.

Do not treat a null result from reordering declarations as evidence about
anything else.

## An HImode constant >= 0x8000 costs a mid-function pool AND a branch

Beyond producing the wrong constant, storing `0xffff` directly through a `u16 *`
makes gcc emit `ldrh r3, <pool>`, which forces a **literal pool inside the
function and a `b` to jump over it** — two extra instructions, and every later
position shifts. `FieldMove_NoTarget` was 139 lines against 137 for that reason
alone; `k = 0xffff;` as an `int` local fixed the constant, the pool and the
branch together.

## The add/sub chain test applies to SYMBOL addresses too

Where the ROM has `ldr r3, =iwram_3001f30 / sub r3, #0x74 / ldr r1, [r3]`,
declaring the second global separately gives gcc a second pool entry rather than
the chain — so by the existing test the chain belongs to the *source*:
`*(T **)((unsigned char *)&iwram_3001f30 - 0x74)` reproduces it. That section
currently discusses numeric constants only.

## Reuse an existing variable as a call's destination to kill a constant derivation

`OvlFunc_971_2008f8c` keeps a default `msg = 0x294e` live into a four-arm `if`;
gcc then *derives* one arm's constant from it (`add r5, #25` for 0x2967) and
cross-jumps the arms. Writing the intervening `__GetFlag` result into **`msg`
itself** — which the ROM's `mov r5, r0` proves — kills the old value in that
region, and both the derivation and the cross-jumping vanish: 63 → 8 differing.

> When the ROM loads a fresh pool constant into a register that already holds a
> nearby constant on your side, look for a source-level reuse that kills the old
> value, not for a way to stop the `add`.

## Negative: `extern int f();` is the same lever as a full `int` prototype

Measured identical on two functions. The **return type** is the whole lever; the
presence or absence of a parameter list is not part of it. (Distinct from the
batch-117 no-prototype lever, which is about removing the declaration entirely.)

## RETRACTED: "`.call_via rN` is a hard wall". It is reachable, and 51 functions were written off

I claimed here that `.call_via rN` — `mov r12, pc / bx rN`, a Thumb-to-ARM call
into an IWRAM routine — could never be produced, because gcc-2.96's machine
description has exactly one indirect-call pattern, `bl _call_via_%0`. **51
functions were removed from the candidate pool on that basis. The claim was
wrong.**

Two mistakes compounded:

1. **The machine description bounds the CODE GENERATOR, not the source
   language.** This tree already reproduces fixed instruction sequences gcc
   would never choose, with inline asm — `include/dma.h` does it for the DMA
   registers. The same technique reaches `.call_via`.
2. **`tryc.py` silently DROPPED the line.** `.call_via` begins with a dot, so
   the ref parser skipped it as a directive — but it is a *macro* from
   `include/macros.inc` that expands to two real instructions. Every function
   using it screened two instructions short per call site, which reads exactly
   like an unmatchable structural difference. Fixed: the parser now expands it.

**`Func_8097a10` is elevated and byte-exact.** The shape that works:

```c
static inline int call_via_r4(int (*f)(int, int), int a, int b)
{
    register int (*_f)(int, int) __asm__("r4") = f;
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile ("\t.align\t2, 0\n\tmov\tr12, pc\n\tbx\tr4"
                      : "=r" (_a) : "r" (_f), "0" (_a), "r" (_b)
                      : "memory", "lr", "r12");
    return _a;
}
```

Note the second lever it needed: the *other* call in the same function goes
through the ordinary veneer (`ldr r3, =F / bl _call_via_r3`), and a direct
`F(a, b)` call will not produce that. Assigning the address to a function-pointer
local first — `g = F; r = g(v, base);` — does.

**The lesson is bigger than the class.** "The compiler cannot emit X" is a claim
about the code generator; the ROM is evidence that *something* emitted it, and
in a matching decomp the source is allowed to use whatever the original used —
including inline asm. Before declaring a shape unreachable, check whether the
screen is even *showing* you the shape.

## The `_call_via_` veneer names follow `reg_names`, and two were missing

gcc-2.96 names the veneer from `reg_names`: r10 → `sl`, r11 → `fp`, r12 → `ip`,
r14 → `lr`. `src/lib/call_via.s` already carried `.global _call_via_fp` for r11
for exactly this reason but had no `_call_via_sl`, so any function calling
through r10 screened one line dirty on the symbol name while being
byte-identical. Adding the label (which emits no bytes) turned two such
functions into matches immediately. `ip` and `lr` are still gaps if they ever
come up.

## Caveat on "two distinct symbols of equal value reload"

That row of the CSE table is correct — two distinct symbols do defeat the hoist —
but it **costs a pool word**. The ROM pools the shared constant once; two
symbols pool twice. Measured on `OvlFunc_916_20088b0`: the hoist really does
stop (67 differing → 6) and the object is 212 bytes against the ROM's 208. The
trick only works where the ROM's pool holds two words anyway, so it does not
rescue the straight-line script band.

## Counter-examples to the narrowed HImode rule

The rule was tightened to "only `0` and values ≥ 0x8000 need an `int` local".
That is still the common case, but it is not exhaustive: on `Func_801b4ec`,
`*(unsigned short *)(p + 0x3a2) = 0x21;` and `… = 8;` both pool, and `int` locals
fixed both. Something else — here an adjacent `ldr rN, =0x3a2` offset load in the
same statement — can drag a small value into the pool too.

**Screen the plain literal first, but do not conclude a small pooled HImode
constant is a symbol tell without trying the `int` local.**

## The `.call_via` inline-asm helper: bind the symbol, do not pass it

The working template is in `src/rom_8a000/rom_97384_c_c_a_b.c`. One refinement
matters at every site:

**Do not pass the callee as a parameter.** gcc materialises the address in some
register and copies it into the bound one, costing an instruction the ROM does
not have:

    helper takes (f, a, b)   ours  ldr r5, =Func_8000888 / mov r4, r5
    symbol bound directly    rom   ldr r4, =Func_8000888

Assign the symbol straight to the register-bound variable inside the helper —
`register int (*_f)(int,int) __asm__("r4") = Func_8000888;` — and write one
helper per (callee, register) pair. On `Func_801cc50` this removed the extra
`mov` *and* freed r5 for the struct pointer, fixing a second difference at the
same time (49 of 57 → 53 of 57 with the length gap closing from 1 to 1 and both
prologue lines matching).

**Do NOT add `"r2"`/`"r3"` to the clobber list.** It looks obviously right — a
real call clobbers them — and it is wrong. `Func_801cc50` went 53 differing /
56 lines to 61 / 62. Argument 3 survives all three call sites in r3, which
proves the routine does not clobber it. Where the ROM moves a value out of r2,
it is to free r2 as the INDEX register for `ldrsh rD, [rB, rO]`, not because of
clobbering. These IWRAM helpers have a narrower convention than a C call.

**Sub-shapes, easiest first.** A single call site with the address bound
directly is the reachable one — `Func_8097a10` matched on that alone. Two or
more sites sharing one pointer is harder: the ROM loads it *late*, after an
intervening call, reusing a register that held something else earlier, and
neither naming the pointer in a local nor inlining the address at both sites
reproduces that live range. See `src/non_matching/rom_b5000/80b84c0.c`.

## Naming one level too many costs a callee-saved register

`OvlFunc_932_200ad58` first drafted at 72 differing of 69, at 74 lines — five
instructions too many and every register renamed. The cause was not a lever
that was missing; it was two locals too many.

The ROM spends r5, r6 and r8. The draft spent r5, r6, r8 and r10, because it
named:

* the global's **address** — `char **pp = &iwram_3001ebc;` — **correct**, this
  gives the ROM's single `ldr r6, =iwram_3001ebc` with two `ldr r2, [r6]`
  reloads across the intervening calls;
* the byte **offset** — `k = 0xe0 << 1` — **correct**, held in r8 and reused by
  both stores, and it is what produces the `[r2, r1]` reg+reg form;
* the dereferenced **value** — `char *q = *pp;` — **wrong**, used once per site;
* and a literal zero stored through a byte pointer — **wrong**, the ROM has the
  plain `mov r3, #0`.

Removing the last two took it from 72 differing to **2**.

> The levers that say "name it" are about a value the ROM demonstrably keeps in
> a register across something. A value the ROM rebuilds at each use should not
> be named, and naming it is not free — each extra long-lived local competes for
> the same callee-saved registers and can push a real one into r9–r11, which
> costs a `mov`+`push` pair at entry and the matching pair at exit.

The diagnostic is the push list: **if your prologue saves a high register the
ROM does not, count your locals before you reach for a lever.**

## The `.call_via` helper, corrected: one site and several sites want DIFFERENT spellings

I circulated "bind the callee inside the helper" as a general improvement. It is
right for **one** call site and wrong for several, and the difference is worth
stating precisely because both forms look reasonable.

| sites | spelling |
|---|---|
| one | bind the symbol inside the helper: `register int (*_f)(int,int) __asm__("r4") = Func_8000888;` |
| two or more sharing one register | pass the callee as a plain argument, constrain it `"r" (f)`, and write `bx %1` in the template |

Binding it costs a **reload per site**: on `Func_801cbd4` (three sites, one
pointer carried in r4) the bound form emits `ldr r4, =Func_8000888` three times,
62 lines / 58 differing. The unpinned `%1` form lets gcc CSE the address into
one register held across all three sites — OK on the first screen after the
change.

For a **high** register (r8–r11), `"r"` accepts r8 but gcc will never *choose*
it: declare `register int (*g)(int,int) __asm__("r8") = F;` in the enclosing
block and pass `g` unpinned. Declared inside a loop body it gives the ROM's
per-iteration rematerialisation; declared once for the function it gives one.

### The clobber list is a per-function READING, not a style rule

I also circulated "never add `r2`/`r3`". That is wrong as a blanket rule.
Measured:

* `Func_801cbd4` — `"r2"` **required**. Without it, OK becomes 60 lines / 61
  differing. The tell is in the ROM's prologue: `mov r6, r2` copies parameter 3
  into a callee-saved register before the first call, which gcc only does if it
  believes r2 dies.
* `Func_80c0a24` — `"r3"` **required**, or gcc picks r3 for the pointer and
  emits `bx r3` where the ROM has `bx r4`. `"r2"` and `"r3"` together is worse.
* `Func_809b86c` — `"r2"` neutral.

**The test: look for a loop-carried or call-crossing value living in r2 or r3.**
If the ROM never parks a value there, the original's macro clobbered it. The two
registers are independent and must be decided separately — `Func_801cbd4` proves
r3 is *not* clobbered (argument 4 survives all three sites in r3) while r2 is.

### Drop `"lr"`

`mov r12, pc / bx rN` never writes lr — the ARM callee returns via r12. Two
functions hold a live value in r14 across the call sites and cannot reproduce it
with `"lr"` clobbered. **`"memory"` and `"r12"` are the only two that are always
right.**

### A correctness trap: never put two call sites in one expression

Both sites' outputs are `register … __asm__("r0")`, so gcc treats them as the
same value. `f(a,a) + f(b,b)` compiled to **one** `bx` followed by `lsl r0, #1`
— it doubled the second result and dropped the first. Nesting silently dropped
the `mov` supplying the outer call's first argument. This is a miscompile, not a
match failure.

**Assign each call's result to its own named local, in its own statement, before
the next site.** Nesting is tempting because it removes exactly the extra `mov`
that some of these functions are stuck on. Do not.

### Inlining the symbol at two sites is how you reach the ROM's stack spill

Where the ROM has `ldr r3, =F / … / str r3, [sp] / … / ldr r3, [sp]`, a named
local does **not** produce it — gcc either allocates a callee-saved register or,
with a pinned register variable, elides the assignment and calls the *wrong
function*. Writing the symbol literally at both sites lets CSE build one pseudo
which then spills, which is the ROM's shape.

## Build a constant AFTER the call if the ROM does — it decides your push list

`OvlFunc_881_20081c4` first drafted with the wrong prologue — r5, r6 **and r7**
against the ROM's r5, r6 — for one reason: a constant was written before the
call and used after it.

    ours   t = 0x80 << 13;                       <- live ACROSS the call
           *(int *)(a + 0xc) = call_via(...) + t;

    rom    ... bx r3 / mov r4, #0x80 / lsl r4, #0xd / add r0, r4

The ROM builds it *after* the call and keeps it in **r4**, which is
call-clobbered under this tree's `-fcall-used-r4` and therefore free. A value
live across a call cannot go there, so gcc reached for a third callee-saved
register and the whole function renamed. Splitting the statement so the
constant is born after the call took it from 24 differing (first diff at
position 0) to 12 (first diff at 24):

    r = call_via_r3(__sin(*p << 3), 0x80 << 11);
    t = 0x80 << 13;
    *(int *)(a + 0xc) = r + t;

> **Read the ROM for where a constant is BUILT, not just what it is.** A
> constant built after a call is a statement that the source computes it after
> the call, and writing it earlier costs a register — which is visible in the
> push list before it is visible anywhere else.

This is the same diagnostic as "naming one level too many", reached from the
other direction: there the cost was an extra local, here an extra live range on
a value that did not need one.

## Operational: check what a generated filter list actually CONTAINS

Round 6's worklists were built with a `blocked_cse.py` pre-filter, and the
agents were told so. The filter did nothing. The extraction was

    blocked_cse.py --list 4000 | awk '/insns/{print $4}'

and on a line like `104 insns 3 repeats OvlFunc_955_2009424 asm/...` field 4 is
the word **`repeats`**, not the name. The exclusion set was 113 identical copies
of that word, so nothing was excluded, and two agents independently reported
spending a fifth of their budget on genuinely blocked functions.

`sort -u | head` on the list would have shown one distinct value in one second.

> **A filter list is data. Look at three lines of it before trusting it, and
> check the distinct count.** The failure is silent: the pipeline runs, the
> worklists are written, and the only symptom is a hit rate that looks like bad
> luck.

Same family as the batch-119 note about a verification loop that read a file
inside the container which had been written outside it — both produced a
confident pass over an empty set.

## `-fno-rerun-cse-after-loop` is not a loop phenomenon

`OvlFunc_942_20086c8` is a straight-line cutscene script with **no loop at all**,
and gcc still hoists the shared `0x8a8` into a callee-saved register and grows
the push list. Of six CSE-family flags — `-fno-gcse`, `-fno-cse-follow-jumps`,
`-fno-cse-skip-blocks`, `-fno-expensive-optimizations`, `-fno-gcse
-fno-cse-follow-jumps`, and this one — **only `-fno-rerun-cse-after-loop`
reaches it.**

The pass runs unconditionally after the loop optimiser; the name describes
*when* it runs, not what it acts on. Both existing notes about the flag frame it
as a loop thing — that it *costs* matches in loops is still true, but its reach
is wider than the name suggests, and it is worth one screen on any
straight-line function whose only fault is a hoisted constant.

## Two more presentations to add to the recognition lists

**`-fno-strict-aliasing` can present as an argument-setup permutation.** The
documented symptom is a load hoisted above a store. On `Func_8028aa8` the entire
residue was a six-position reshuffle *inside one call's argument block*, with no
store visibly displaced — and the flag matched it exactly. Named stack-arg
locals and both return types were neutral against it.

**`-ffixed-r10` is a complete no-op.** gcc-2.96 thumb never allocates r10 by
choice, so reserving it changes nothing. Measured byte-identical on a function
whose residue is a high-register role exchange. Do not spend a screen on it —
unlike `-ffixed-r7`, which is real because gcc *does* pick r7.

## Two constants in DIFFERENT registers means they are simultaneously live

`OvlFunc_930_20090b8` masks two bytes with `0xfd`, then stores a zero, then
passes `0x12` twice. The ROM's registers: mask in **r5**, zero in **r6**, and
`0x12` back in **r5** after the mask dies.

Written in source order — mask, use it twice, then zero — gcc gives the zero the
mask's now-dead r5 and never touches r6, so the push list is one register short.
Four differing of 56, all of it that one choice.

Assigning the zero **before** the mask, so their live ranges overlap, matched.

> When the ROM puts two constants in different callee-saved registers and one of
> them is later reused for a third value, the two are live at the same time.
> Read the register reuse as a statement about the source's declaration point,
> not just its allocation.

This is the counterweight to "naming one level too many": that note is about
values that should not be named at all, this one is about a value that must be
born *earlier* than its first use suggests. The diagnostic is the same — the
push list — but it points the opposite way.

## The ORR-destination lever needs an `unsigned char` local, not an `int` one

Where the ROM has the constant in the destination register —

    rom   ldrb r2, [r1] / mov r3, #2 / orr r3, r2
    ours  ldrb r3, [r1] / mov r2, #2 / orr r3, r2

— naming the constant is the documented fix, and the TYPE is load-bearing.
Measured on `OvlFunc_947_2008ec8`, all against a 69-line ROM:

    *p = 2 | *p;                     2 differing
    *p |= 2;                         2
    int m = 2; *p = m | *p;          2   (byte-identical to the literal)
    unsigned char m = 2; *p = m | *p;   OK

An `int` local is folded away — gcc knows the value fits and treats it exactly
like the literal. A narrow local survives as a distinct QImode pseudo, and that
is what makes it the `orr` destination. The existing note says "a narrow
`unsigned char` constant reaches 'constant is rd'"; the sharpening is that the
`int` spelling is not a weaker version of the same lever, it is *no lever at
all*, so a null result from `int m` says nothing about the technique.

## A volatile `asm` CAN be cross-jumped — put the call in both arms

Round 6 recorded that a volatile `asm` is *not* cross-jumped, so writing a
DMA call in both arms of an `if` gives two expansions and the fix is to select
only the differing argument into a local and put one `asm` after the join.

A four-member family contradicts that, and the contradiction is the answer.
The ROM has **one** `stmia` but **two** `ldr r3, =REG_DMA3SAD` — the operand
setup duplicated into both arms, the asm body merged:

    if (flag) DMA3_COPY(A, d, size); else DMA3_COPY(B, d, size);

gcc cross-jumps the two identical asm bodies into one and leaves the per-arm
`ldr r0, =A` / `ldr r0, =B` and the duplicated base load. Selecting the source
into a local and calling once gives 21 lines against the ROM's 22 — one short,
because the base load then happens once.

Both readings are real; the tell is the ROM's line count. **Two `ldr` of the
same DMA base with one `stmia` means the call is in both arms.** One of each
means the argument was selected first.

Four functions turned on this, one of which was already parked at exactly
"21 against 22, missing one `ldr r3, =REG_DMA3SAD`".

## Splitting a single-function `.s`: remap EVERY section, not the one that errors

`split_s.py` declines a file with one function, so these are done by hand: move
the data half to a new `.s` and repoint the linker script. Three files this
batch carried a `.data` **and** a `.bss` section.

Remapping only `(.data)` links cleanly through the `.data` error and then fails
on `.bss` with a different and much less obvious message —
`defined in discarded section '.bss'`. Grep the linker script for **every**
line naming the object before editing:

    grep -n '<stem>' overlays/<rom>/overlay.ld

`.text` goes to the new `.c`'s object, and `.data` **and** `.bss` both go to the
data half. Getting one of the three right is enough to make the first error go
away and not enough to link.

## Working by FAMILY: `tools/twin_families.py`

Picking candidates one at a time hits diminishing returns — the tractable shapes
get taken and what is left is register-allocation floors. Grouping the
**remaining** functions by identical opcode stream changes the economics: one
solved `.c` is a template and the rest are a search-and-replace on constants.

46 families of 2+ in the 20–120 band cover 96 functions. Results so far:

* a four-member DMA family, elevated in one round, **including a member parked
  for several batches** at "21 lines against 22" whose answer was sitting in its
  three siblings;
* a two-member family where the sibling matched on the first screen after six
  constants were substituted.

Two cautions the tool's docstring carries:

* It groups unelevated functions **against each other**. An earlier version
  compared them against already-elevated ones and returned zero, because
  generated `.s` files use gcc's own `.thumb_func` directive rather than the
  repo's `.thumb_func_start` macro — the scan saw no elevated bodies at all.
* A family can be uniformly *blocked* as easily as uniformly solvable. A
  three-member family of `f(-1, -1, -1, 0)` callers is parked because none of the
  three has a control-flow boundary; the identical C **matches** on
  `OvlFunc_923_2009208`, which does. Read the family before assuming it is a win.

## The constant-CSE rule needs BOTH halves, and a boundary alone is not enough

`OvlFunc_932_2008b3c` uses a flag id once before an `if` and once in each arm —
three uses in three different basic blocks, so the boundary the rule asks for is
present. gcc still hoists the id into a callee-saved register and pays a push.

Of the CSE-family flags only **`-fno-rerun-cse-after-loop`** undoes it;
`-fno-gcse` is byte-identical to the default. Both halves of the precondition
are required, and the boundary is the half that is easy to mistake for
sufficient.

## The constant-as-destination lever: `orr` wants a NARROW local, `and` wants an `int`

Two sharpenings of the same lever, measured the same day, pointing opposite ways:

| shape | spelling that makes the constant the destination |
|---|---|
| `orr r3, r2` with the constant in r3 | `unsigned char m = 2;` — an `int m` is folded and is byte-identical to the plain literal |
| `and r3, r2` with the constant in r3 | `int m = 0xfe;` — an `unsigned char m` puts the LOADED value in the destination instead |

On `OvlFunc_901_2008c1c` the narrow local gives 4 differing and the `int` gives
2. On `OvlFunc_947_2008ec8` the narrow local matches and the `int` is inert.

**The statement form is not the lever.** `*p = m & *p;` and
`m = m & *p; *p = m;` are byte-identical to each other under either type. Only
the type of the local moves it, so a null result from reordering the assignment
says nothing.

Why they differ is not established here; what is established is that the two
operations must be tried separately and that a null result on one type is not
evidence about the lever.

## gcc-2.96 thumb `REG_ALLOC_ORDER`, read from the compiler source

`/opt/camelot-gcc/gcc-2.96/gcc/config/arm/arm.h:989` gives
**`{3, 2, 1, 0, 12, 14, 4, 5, 6, 7, 8, 10, 9, 11}`**. Two consequences that
explain ROM shapes people keep re-deriving:

* For a **call-crossing** value the order is **r5, r6, r7, r8, r10, r9, r11**.
  So `r10` before `r9` is normal, not an oddity, and a fourth long-lived value
  reaches r11 only after r8, r10 and r9 are taken.
* **r12 and r14 are allocatable and come before r4.** A `mov r12, r3` in a ROM
  is a register-pressure readout — "r0–r3 were all busy when this local was
  born" — not a special construct.

## A recurring residue class: the parameter pointer one register too low

Three separate near-misses in one round are 1:1 with the ROM and differ only by
a rotation where our `e`/`m` takes r6/r9 and the ROM takes r7/r8, with
everything downstream shifting. Measured against it and all inert: declaration
reordering, copying the parameter into a local, deleting a named pointer,
`-fno-rerun-cse-after-loop`.

It is **not** the priority formula — `global.c`'s `allocno_compare` is
`floor_log2(n_refs) * n_refs / live_length`, and by that the parameter always
outranks the short-lived value that beats it in the ROM. The cause is in
`find_reg`'s conflict/preference pass. Recognise it in one screen rather than
ten: **same instruction stream, one register rotation, parameter one slot low.**

## A parameter and a loop counter can be the SAME variable

`Func_8092708` sat at 33 of 115 with every difference an r5↔r6 exchange between
an actor pointer used ~20 times (twice inside loops) and an `anim` parameter
used once. The ROM gives **r5 — the first callee-saved register — to the
short-lived parameter.**

`allocno_compare` weights priority by basic-block *frequency*, so merging the
parameter with a loop counter multiplies its priority past a whole-function
pointer's. Reusing `anim` as the counter for both loops, changing nothing else,
matched exactly.

> When the ROM gives the first callee-saved register to a value that looks too
> short-lived to deserve it, look for a later loop counter it could be the same
> variable as.

## Two ways to stop `symbol + K` folding, and they give different `add`s

The doc records the walk (`p = gState; p += K;`), which yields the
**destructive** `add r2, r1`. Naming the offset in a dominating block —
`off = 0xfc * 2; p = gState + off;` — yields
`ldr r3, =gState / mov r1, #0xfc / lsl r1, #1 / add r2, r3, r1`, the
**three-operand** form. On one function the walk was 15 of 61 and the named
offset 5. Read the ROM's `add` to choose, exactly as for ordinary pointer
arithmetic.

## `-fno-gcse`'s signature: visibly redundant work across a call or branch

A repeated `ldr rN, =sym` into the same register in a later block, or an address
rebuilt at two sites. Without the flag, global CSE collapses them **and the
whole allocation shifts**, so the residue reads as a dozen unrelated faults. One
function went 49 → 32 differing on the flag alone, before any spelling change.

## A variable with DISJOINT live ranges should be two variables

One `prev` shared between two mutually exclusive switch arms: the ROM used a
callee-saved register in one arm and the scratch r2 in the other. Splitting it
took 32 → 25 and fixed **three** registers at once.

This is the counterpart to "naming one level too many" — there the fix is fewer
locals, here it is more. The discriminator is whether the ROM gives the two
regions different registers.

## Fix a narrowed negative constant with the LITERAL's spelling first

`p->f1e = s + 0xffffc000;` narrows to `mov #0xc0 / lsl #8`, because the literal
is *unsigned* in C89. `p->f1e = s - 0x4000;` keeps the ROM's
`ldr =0xffffc000 / add`. The documented fix — an `int` local — also works in
isolation but cost six lines in place. **Spelling first, local second.**

## `ble .LCB / b far` is the thumb long-branch expansion, not a source shape

arm.md emits an inverted short branch over a `b` when the target is out of
range. Do not read it as evidence of an odd source structure, and note the
`.LCB` label often coincides with a real `.L` label in the disassembly.

## A function can return a value it never moves into r0

An early exit that branches straight to the shared epilogue with r0 still
holding a compared load. `return -1;` does not produce that;
`v = p[0]; if (v == -1) return v;` does. Costs one instruction and a whole-tail
shift when missed.

## `cmp #K / bge` and `cmp #K / blt` with K>0 are UNREACHABLE — a one-line screening test

The signed-lower-bound residue has been recorded as "no spelling reaches it".
The mechanism is now read out of the compiler, and it makes the class decidable
**before writing any C**.

`combine.c`'s `simplify_comparison` (gcc-2.96, lines ~10150–10180) rewrites,
unconditionally for `MODE_INT`:

    LT  C  (C>0)  ->  LE (C-1)          GE  C  (C>0)  ->  GT (C-1)
    LE  C  (C>0)  ->  unchanged         GT  C  (C>0)  ->  unchanged

So for **K > 0**, gcc-2.96 can emit `cmp #K / ble` and `cmp #K / bgt`, and can
**never** emit `cmp #K / blt` or `cmp #K / bge`.

The ARM hook `arm_canonicalize_comparison` is not involved —
`CANONICALIZE_COMPARISON` fires only when the constant is *not* ARM-encodable,
which never happens for these small bounds. Unsigned behaves the same way
(`LTU`/`GEU` rewritten, `LEU`/`GTU` not). `cmp #0 / bge` is fine, because the
rewrite is guarded by `const_op > 0`.

> **Grep the ref for `cmp rN, #K` followed by `bge`/`blt` with K > 0. Each such
> site is a hard floor of two instructions.** Decide before screening.

**Measured: 33 of the 2274 remaining functions carry one, across 44 sites.** So
it is a small class, not a wall — but it is worth excluding those 33 from a
worklist rather than discovering the floor one screen at a time. The grep:

    cmp\trN, #K   immediately followed by   bge  or  blt   with K > 0

## `goto` loops disable loop optimisation ENTIRELY — a first-class lever

The doc records "write the control flow with `goto`" as a shape-matching note.
The mechanism is much bigger: `loop_optimize` is driven by
`NOTE_INSN_LOOP_BEG`/`END` notes that `stmt.c` emits **only** for `while`, `for`
and `do` statements. A backward `goto` emits no notes, so loop-invariant
hoisting, strength reduction **and** `check_dbra_loop` reversal all vanish
together.

`Func_8090584` was 95 differing of 99 — gcc had hoisted the state pointer, two
mask constants, a base address and three store values out of the loop — and
**no flag touched it** (`-fno-gcse`, `-fno-rerun-cse-after-loop`,
`-fno-move-all-movables`, `-fno-strength-reduce`,
`-fno-expensive-optimizations` all left it at 95). The `goto` rewrite alone took
it to **3**, and then to an exact match.

> **If the ROM rebuilds a loop-invariant constant inside the loop body, the
> source's loop was not a `while`/`for`.**

**Corollary:** once hoisting is off, anything set up in a register *before* the
loop had to be written there. A `mov r7, #0x1f` in the prologue of a `goto`-loop
function is `int mask = 0x1f;` in the source, not gcc hoisting `& 0x1f`.

This also subsumes `-fno-strength-reduce` in these cases, and does it without
that flag's cost — measured at one extra callee-saved register on both functions
it was tried on.

## Apply `volatile` at the USE SITE, not to the declaration

Where one read of a field must not be commoned with an earlier one, qualifying
the *declaration* de-optimises the other reads too — one function went to 52
lines against the ROM's 51. Casting only the offending read matched with no flag
at all:

    *(unsigned short *)(*(unsigned char *volatile *)&s->f28 + 2) = ~sum;

This is the cheap form of "prefer `volatile` over a `-fno-gcse` rule".

## Two more per-site readings

**Name the memory-mapped register's ADDRESS, and declare it before the value.**

    volatile unsigned short *r = &REG_TM3CNT_H;
    int z = 0;
    *r = z;

matches where four spellings of the value local did not — and declaring `z`
first leaves it at 3 differing. Same two statements, two orders.

**Reusing a dead PARAMETER as a local reaches a register the allocator will not
otherwise give.** One function was exact except an index in r2 where the ROM has
r1 — r1 being the second parameter, dead on that path. Assigning into the
parameter itself matched; declaration order, types and statement order did not.

## Selecting for the `goto`-loop lever: grep for a constant rebuilt in the loop

The lever has a mechanical signature, so it can drive candidate selection rather
than being tried after a screen fails. **Find a backward branch, then look for a
pool load or a `mov`+`lsl` build inside the loop body.** If the ROM rebuilds a
loop-invariant there, its source loop was not a `while`/`for`.

Two functions picked this way matched on the **first screen** —
`StartMenu` (a `-1` rebuilt every iteration) and `Func_8019e48`. 18 candidates
carry the signature in the 30–70 band.

## The INVERSE constant problem: the ROM derives, gcc does not

Every constant-CSE entry above is gcc commoning two constants the ROM rebuilds.
`Func_80160fc` is the reverse:

    rom   ldr r2, =0xea6 / ... / sub r2, #0x3 / add r3, r7, r2
    ours  ldr r3, =0xea6 / ... / ldr r3, =0xea3

The ROM's compiler related the two offsets with a `sub`; ours emits two
independent pool loads. **Writing the offset as one variable mutated in place —
`off = 0xea6; ... off -= 3;` — does not reach it**, because both values are
compile-time constants and gcc folds `off` at each use. There is nothing in the
source to stop it.

Worth recognising as its own shape: a `sub rN, #K` or `add rN, #K` applied to a
*pooled* constant, with no intervening load, is the ROM deriving one offset from
another, and it is not currently reachable.

Note the same function shows the useful half of the pointer lever: a **named
destination pointer per access** gets `add r3, r7, r2 / ldrb r3, [r3]` where the
inline expression gives a reg+reg `ldrb r3, [r6, r3]` — worth 8 instructions.

## Address arithmetic in `unsigned int` locals (356 functions carry the shape)

The ROM very often loads a base from the pool, builds a constant offset with
`mov`+`lsl`, and adds them at runtime:

    mov  r2, #0x88
    ldr  r3, =gState
    lsl  r2, #0x2
    add  r3, r2
    ldrh r3, [r3, #0x0]

Every pointer-arithmetic spelling of that folds to `ldr r3, =gState+544` and two
instructions -- `gState + (0x88 << 2)`, `((unsigned short *)gState)[0x110]`,
`(int)gState + (0x88 << 2)`, `0x88 * 4`.  I parked a function claiming the shape
was unreachable.  **It is not.**  The C that produces it does the arithmetic in
`unsigned int` locals, one operation per statement:

    r2 = 0x88;
    r3 = (unsigned int)&gState;
    r2 <<= 2;
    r3 += r2;
    key = *(unsigned short *)r3;

`src/rom_8a000/rom_8a5f8_b.c` (PlayMapMusic) has been in the tree doing exactly
this the whole time.  gcc-2.96 does not constant-fold a `SYMBOL_REF` through a
chain of integer locals, only inside a single expression.

**Two forms of base, and both are reachable.**  Where the pooled value is a
POINTER that gets dereferenced first, the ordinary spelling already works and no
integer locals are needed -- `base = *(unsigned int **)sym;` then
`*(T *)((unsigned char *)base + 0xe6 * 2)`, as in `CutsceneWait`.  Write the
offset as a multiplication (`0xe6 * 2`) and gcc emits the `mov`+`lsl` build.

**Sizing, over the 2273 remaining functions:**

  * 115 have the dereferenced-pointer base -- the easy form.
  * 216 have only the direct symbol base -- these need the integer-local idiom.
  * 25 have both.

Control, over 531 already-matching functions compiled with the production flags:
19 contain the dereferenced-pointer form and 15 the direct-symbol form.  Both are
demonstrably producible.

### The control that caught this, and why the first one lied

The first control run reported **0 of 531**, which would have confirmed the wrong
park.  The detector was matching `add rD, rS` -- the ROM disassembly's
two-operand shorthand -- while gcc writes `add rD, rD, rS`.  `add_rr` came back
0 across the entire corpus, which is impossible for real thumb output and is the
tell that the detector, not the compiler, is the thing that is broken.

This is the **third** time this exact trap has cost something (see the split
shifted build, and the `.call_via` write-off).  The rule is not just "run a
positive control" but: **count the sub-patterns too.** If a component of the
conjunction is zero across the whole corpus, the regex is wrong.  `tryc.py` has
folded this form since early on -- its `DESTRUCTIVE` regex exists for precisely
this -- and any new detector should reuse that normalisation rather than
re-deriving it.

## `ldrh` versus `ldrsh`: the type of the DESTINATION decides

gcc-2.96 thumb loads a HImode *local* with

    mov   r3, #0x0
    ldrsh r2, [r0, r3]        @ ldrsh has no immediate-offset form
    lsl   r3, r2, #0x10       @ ...and tests it for zero by shifting

A plain `ldrh rD, [rB, #0]` only ever comes from loading into an **SImode**
destination.  So whenever the ROM shows `ldrh`, the variable receiving it is an
`int`/`unsigned int` in the source, never a `short`.  Corollary: `>> n` on that
value must be `unsigned int` to get `lsr` rather than `asr`.

Found on `Func_80a3ddc` and immediately decisive on `Func_801c8a0`, where it
took the screen from a three-instruction miss to an exact one.

## A rebuilt loop bound is not always the `goto` tell

The goto lever says: a constant rebuilt inside the loop means the source loop was
not a `while`/`for`.  There is a second cause, and checking it first is cheaper.

Given `for (i = 0; i <= 0x1bf; i++)`, gcc rewrites the test to `i < 0x1c0`, and
0x1c0 **is** a cheap shifted build (`mov r3,#0xe0 / lsl r3,#1`) where 0x1bf is
not.  So it stops keeping the bound in a register and rebuilds it every
iteration, and it rotates the loop, jumping into the middle of the body to reach
the test.  The ROM's `ldr r5,=0x1bf` hoisted once with `ble` at the bottom is a
**do/while**.

Order of diagnosis when a bound is rebuilt in the loop:
  1. Is `K+1` a cheap shifted build while `K` is not?  Then it is a `for` that
     should be a `do/while`.
  2. Only then reach for the goto lever.

## The gap the goto lever cannot express: partial hoisting

`Func_801c8a0`'s second loop wants loop optimisation to run -- the ROM hoists
0x3ff, 0x1bf and a symbol address out of it -- while ONE loop-invariant load
stays inside.  The lever is all-or-nothing: a `do/while` hoists everything
including the load, a `goto` loop hoists nothing.

Six ways of trying to make gcc believe the in-loop store might alias that load
(address-taken local, extern array, load through a cast integer, store through a
cast integer, `volatile`, reordering) all left the output byte-identical.  When a
function needs partial hoisting, park it -- and record it as this class rather
than as an aliasing problem, because the aliasing attack does not work.

## When a derived constant IS reachable (refines the `sub rN,#K` park)

`src/non_matching/rom_15000/80160fc.c` records that the ROM deriving one offset
from another (`ldr r2,=0xea6 ... sub r2,#3`) is unreachable, because
`off = 0xea6; ... off -= 3;` is constant-folded at each use.  That is true only
under a condition worth stating:

**A derived constant is reachable when the base constant has already been forced
into a register by a runtime use.**  On `Func_8093304` the ROM does
`ldr r1,=0x12f4 ... add r1,#0x2`, and the spelling that failed on Func_80160fc
works, because 0x12f4 is first added to a pointer loaded from memory.  gcc must
materialise it, and CSE then derives the second value from the register.

So before parking a `sub`/`add` on a pooled constant, check whether the base
value is used in a runtime computation anywhere earlier.  If it is, write the
offset as a mutated variable and it will derive.  If both values are only ever
folded into addresses, there is nothing to derive from and the park stands.

## The named-pointer lever needs the offset to be mutated afterwards

Giving a store its own named destination pointer gets `add r3, r7, r2 / strh`
instead of a reg+reg `strh r3, [r6, r2]`.  It is not unconditional.

It works when the offset variable is **mutated after the pointer is taken** --
the old sum then has to be materialised, so the `add` survives.  Where the
offset is dead after the store, gcc always folds it into the addressing mode and
no spelling tried recovers the separate `add`: naming the pointer, computing it
through integer locals, making the base an `unsigned int`, or duplicating the
store into both branches to invite cross-jumping.

`Func_8093304` shows both halves in one function -- the in-branch store gets the
ROM's `add`, the join-point store does not, one instruction short of a match.

## Fakematches: what the screen cannot settle

Two candidates this round screened OK and were not matches. Both carried the
inline-pool warning. The escalation ladder, and where each rung failed:

1. **Instruction stream equal** — `DecodeMetatileset`, 78 lines OK. Its .text
   was 0x9c against the reference's 0x98.
2. **Size check** — catches (1), but only runs when the reference holds ONE
   function. A screen that prints `[size check skipped: ref has N functions]`
   has not been size-checked at all. Extract a single-function `.s` and re-run
   before wiring anything into the build. This is the single cheapest habit
   change available: it caught `DecodeMetatileset` in one command.
3. **Size check passed, objdump sizes identical** — `OvlFunc_903_2008fc8`
   passed the size check, and `objdump -h` gave .text 0xbc, .data 0, .bss 0 for
   both objects. The linked overlay still differed in **58 bytes**.

Rung 3 is the one to internalise. Pool loads normalise to `=value` in the
screen, so a pool holding **different values at the same distance** compares
equal, and the sizes agree because the pool is the same length. When the
reference keeps its literal pool inside the function body, an equal instruction
stream and an equal .text size together still do not settle it. `make compare`
is the authority, exactly as this document has always said -- these are the two
concrete cases that show why.

Overlay bytes are embedded in the ROM image, so an overlay fakematch fails the
ROM sha1, not just the overlay `cmp`. Do not read `compare-rom` failing as
"the main ROM is broken".

## Deleting a single-function `.s` that also holds `.data`

`ovl_314_c_c_c_c.s` held one function and a trailing `.section .data` defining
four `gOvl_*` symbols, and the overlay linker script named the object twice --
once under `(.text)`, once under `(.data)`. Replacing the `.s` with a `.c`
broke the link on all four symbols.

`split_s.py` does not help here: with a single function, the trailing data is
part of that function's block. Split it by hand -- text to `_b`, data to `_c` --
and point the two linker-script lines at the two new objects.

**Then verify byte-neutrality with the function still in asm, before any `.c`
exists.** That is what let this round attribute a 58-byte overlay difference to
the C rather than to the layout; without it, the two failure modes are
indistinguishable.

## One integer-local per address chain, not one scratch variable reused

Working the integer-local idiom, the natural thing is to write a single scratch
variable and reuse it, the way the ROM reuses r3:

    r3 = (unsigned int)&iwram_3001edc;
    q  = *(unsigned char **)r3;
    r3 -= 0x20;
    base = *(unsigned char **)r3;
    r3 = (unsigned int)&gState;      /* <-- same variable, unrelated value */
    r3 += r1;

That is 10 of 32 differing on `OvlFunc_923_200a370` -- a pure r1/r3 swap that no
statement reordering fixed. Giving the second chain its **own** variable was an
exact match on the next screen.

Reusing one C variable for two unrelated address chains makes one pseudo with a
long live range; gcc then allocates it against the other pseudos differently
than it allocates two short ones. The ROM's register reuse is the ALLOCATOR
reusing r3, not the source reusing a variable -- do not read one as the other.

Declaration order is not the lever here: swapping the two declarations left the
count at 10.

## `tools/oneref.py` -- always size-check

Extracts one function into a standalone `.s` so `tryc.py --ref` runs its `.text`
size check, which it skips whenever the reference holds more than one function.
Most targets live in a multi-function `.s`, so that check was silently off for
most screens, and batch 123 shipped a candidate whose `.text` was 0x9c against
the reference's 0x98.

    python3 tools/oneref.py <function>
    docker run ... tools/tryc.py cand.c --ref scratch/<function>.s

A screen printing `[size check skipped: ref has N functions]` has not been
size-checked. It is one command to fix, and it is not optional.

## Twins are worth checking for before writing anything

`OvlFunc_881_20084a0` and `OvlFunc_881_20084f0` are the same function against a
different struct field -- offset 0x8 versus 0x10. The second cost one `sed` and
one screen. They were also the only two functions in their `.s`, so the file
became a single `.c` with no split and no linker change at all.

`tools/twin_families.py` finds these; run it before starting anything that looks
like boilerplate.

## `tools/solved_twins.py` -- search the remaining functions against the SOLVED ones

`twin_families.py` groups the remaining functions against each other. That finds
families, but the first member still has to be solved the hard way. This searches
the other direction -- remaining functions against everything already matched --
and a hit is the cheapest elevation there is: copy the `.c`, change the
immediates, screen.

It found 11, and one of them was `OvlFunc_924_200d900`, which had already been
elevated by hand that same round after being spotted by accident. Two more were
elevated from it immediately.

The solved corpus is a **build artefact**: `asm/<path>/X.s` is gcc's output
whenever `src/<path>/X.c` exists, so "solved" is just every `.s` that has a `.c`
counterpart. Build before trusting the output.

Matching is on the **mnemonic stream only** -- no registers, immediates, or
branch targets. That looseness is the point: differing immediates are what make a
twin cheap rather than useless. Every hit still gets screened.

**Check the template's flag group.** `OvlFunc_959_200a38c` is
`OvlFunc_959_200a308` with four immediates changed and needs the same
`CSE_CFLAGS` -- 41 differing at `-O2`, exact with `-fno-rerun-cse-after-loop`.
A twin inherits its template's flags along with its shape.

### The zero-result guard, and why it is not optional

The first run reported **0 solved out of 3095 files** -- which, without the
guard, reads exactly like "there are no twins". gcc emits

    .thumb_func
    .type    NAME,function
    NAME:

and the parser wanted `.thumb_func` on the immediately preceding line. This is
the same class of failure as the two-operand `add` in batch 123 and the
`.thumb_func_start` mismatch recorded in `twin_families.py`. The tool now
refuses to report when either corpus is empty, because on this codebase an empty
corpus has never once been the real answer.

## Working a `solved_twins.py` hit: trust the template's recorded levers

`OvlFunc_924_200b788` is `OvlFunc_923_2009208` with one call target changed.
Reading the target's asm first, I "corrected" two things the template's header
had documented as load-bearing -- the three `-1` locals assigned before the early
return, and `p = a + 0x55;` written after the call rather than before -- because
the ROM appears to build both inside the guarded block.

Both corrections were wrong. Moving the `-1` triple inside the guard let gcc CSE
it (`mov r2,#1 / neg r2,r2 / mov r0,r2 / mov r1,r2`, 67 of 80 differing); moving
`p` gave 4 of 80. The **unmodified template with only the rename** was exact.

The template's header records what was measured. Where the ROM's instruction
ORDER seems to disagree with it, the ordering is the optimiser's, not the
source's -- the dominating-block lever puts the assignments where gcc decides,
which is not where the C statement sits. Rename first, screen, and only start
changing things if that fails.

Cheapest possible working order for a twin:

  1. `sed` the names and any differing immediates. Screen. Often done.
  2. If it fails, diff the two ROM listings for immediates you missed.
  3. Only then start moving statements -- and re-read the template's header
     first, because it usually already says why they are where they are.

## When the named-destination-pointer lever DOES work

Batch 125 recorded that the lever is pressure-dependent and that its stated
precondition (the offset must be mutated afterwards) is not sufficient.
`Func_809ad90` sharpens it from the other side.

Its first store was the function's only defect:

    rom   ldr r1,=gState / mov r3,#0x94 / lsl r3,#2 / add r2,r1,r3 /
          ldr r3,[r0,#0x6c] / str r3,[r2]
    ours  ldr r3,[r0,#0x6c] / ldr r1,=gState / mov r2,#0x94 / lsl r2,#2 /
          str r3,[r1,r2]

Naming the destination -- `d = (int *)(g + off); *d = *(int *)(a + 0x6c);` --
was exact on the next screen, even though `off` is dead afterwards.

What differs from the case where the lever failed is the **stored value**. Here
it is a LOAD, so it needs a register of its own; the offset register cannot
double as it, and the address has to be materialised. In `OvlFunc_881_200808c`
the stored value was a small constant that gcc could put in the offset's
register once the offset was dead, so the addressing mode stayed available.

Practical form of the rule:

  * ROM materialises the address (`add rD, rB, rO` then a `#0` access) --
    look at what is stored or loaded alongside. If it needs a register the
    offset cannot supply, name the destination pointer and it will follow.
  * If the other operand is a bare constant, expect gcc to fold and do not
    spend screens on it.

The same reading explains `OvlFunc_899_20099a4`, whose store the ROM
materialises and where ours matches with no coaxing: the ROM there reuses the
offset register for the stored value, which is only possible once the address
is already in a register.

## Argument-setup order: the zero interleaved into a shifted build

The ROM often emits

    mov r1, #0x80 / mov r2, #0x80 / mov r0, #0x0 / lsl r1, #8 / lsl r2, #7
    bl  __MapActor_SetSpeed

with the zero first argument INSIDE the split build of the other two, while the
obvious C -- `f(0, 0x80 << 8, 0x80 << 7)` -- puts the `mov r0, #0` last. Three
functions were parked on this before it was understood.

**It is reachable.** A control over the 3411 solved functions found 15 carrying
the exact shape. The construct, from
`src/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_b.c`:

    int c, d;
    c = 0x80 << 9;
    d = 0x80 << 8;
    if (__GetFlag(0x84e) != 0) return;      /* <-- the guards are load-bearing */
    if (__GetFlag(0x322) == 0) return;
    ...
    __MapActor_SetSpeed(0, c, d);

Named locals assigned at the top, **with a branch between the assignments and
the call**. gcc will not keep the constants live across the guards, so it
rematerialises each at its use, and the rematerialised sequence interleaves the
way the ROM's does.

**The branch is not optional.** Doing the same in a straight-line function makes
gcc keep the values live instead, which is strictly worse: 33 -> 39 lines on
`OvlFunc_911_20082b4`, 26 -> 29 on `OvlFunc_899_20099a4`. This is the same
dominating-block mechanism as the constant-CSE lever, applied to argument order
rather than to constant sharing, and it fails the same way without a block to
dominate from.

**Sizing.** 248 of the 2244 remaining functions carry the shape. 150 have a
conditional branch before the site and are worth the lever. 98 are straight-line
at every site and are, for now, out of reach.

### The detector needed tightening, twice

A first pass asked only "is `mov r0,#0` not last in the block" and reported 450
remaining / 144 solved -- both inflated by trivial `mov r0,#0 / mov r1,#5` pairs
where r0-first is simply the natural order. The signature that matters is the
zero **interleaved into a split build**: a `mov rX,#imm` before it and an
`lsl rX` after it. That gives 248 / 15, and the 15 are what located the
construct.

A control that is too loose does not just overcount -- it points at the wrong
examples. The smallest "solved example" the loose detector offered was a
two-instruction block with nothing to learn from.

### The argument-order lever, validated — and it covers `neg` too

`OvlFunc_909_2008338` is the first function elevated with it. Naming the two
shifted constants at the top, with the function's existing guards between them
and the call, reproduced the ROM's

    mov r1,#0x80 / mov r2,#0x80 / lsl r2,#7 / mov r0,#0 / lsl r1,#8

exactly, and took the first fifty instructions to identical.

The remaining six differences were **the same shape with a different build**:

    rom   mov r2,#0x10 / mov r0,#0 / mov r1,#0 / neg r2,r2
    ours  mov r2,#0x10 / neg r2,r2 / mov r0,#0 / mov r1,#0

`mov`+`neg` is a split two-instruction build exactly as `mov`+`lsl` is, and the
same lever fixes it: `m = -0x10;` at the top, passed by name. One shared local
across both call sites and two separate locals both matched, so the
constant-CSE concern does not arise here.

**Generalise the detector accordingly**: the shape is a zero (or any
single-instruction argument) landing inside a SPLIT BUILD of another argument,
whether that build is `mov`+`lsl` or `mov`+`neg`. The 248-function sizing above
counts only the `lsl` form and is therefore a floor.

## An added push holding a commoned constant is a FLAG tell, not a source problem

`OvlFunc_883_2008ba8` loads the flag id 0x807 twice — once for `__GetFlag`, once
for `__SetFlag`. At `-O2` gcc commons the two pool loads into r5 and adds a
`push {r5}` the ROM does not have:

    rom   push {r14} / bl __CutsceneStart / ldr r0,=0x807 / bl __GetFlag
          ... ldr r0,=0x807 / bl __SetFlag
    ours  push {r5, r14} / ldr r5,=0x807 / bl __CutsceneStart / mov r0,r5 ...

I spent two screens on source spellings first — two separately named locals
holding the same value (the trick that works on the `-1` triple), a local
assigned inside the guarded block, `-fno-gcse`. None moved it.
`-fno-rerun-cse-after-loop` (`CSE_CFLAGS`) was exact.

**The diagnostic is the push list.** When ours pushes a callee-saved register the
ROM does not, and that register holds a constant used more than once, the
rerun-CSE pass is the cause and the fix is the flag group, not the C. Check the
push list before trying spellings — it is visible in the first two lines of the
screen.

This is the second use of the push list as a diagnostic, alongside the
"naming one level too many costs a register" note. Both read the prologue rather
than the body, and both are cheap to check first.

## The branchless `!= 0` idiom: two spellings, and neither is automatic

gcc-2.96's `neg rD,rS / orr rD,rS / lsr rD,#0x1f` computes "is non-zero" without
a branch. It does **not** emit it from a comparison on its own, and the spelling
that provokes it depends on what surrounds the comparison:

  * `Func_80a5fe0`: `return 1 - (v != 0);` produces it. The subtraction from a
    constant forces value mode.
  * `GetFlag`: `return v != 0;`, `return !!v;`, and assigning to a local first
    ALL give a branch. Only writing the idiom longhand works:

        r = -v;
        r |= v;
        return (unsigned int)r >> 31;

Both are worth keeping. When the ROM shows `neg / orr / lsr #31`, try the
arithmetic form first; if the comparison stands alone in the return, write the
three steps out.

## The derived-constant rule has no per-operand granularity

`Func_80a22f4` makes two DMA3_SET calls whose operands differ by fixed amounts.
The ROM derives ONE of the second call's three operands and pool-loads the other
two:

    rom   add r1, #0x1c / ldr r0, =0x50001e8 / ldr r2, =0x80000001
    ours  sub r0, #0x18 / add r1, #0x1c      / sub r2, #0xf

Writing the destination as a mutated variable gets `add r1, #0x1c` exactly --
the batch-123 rule working as documented, since the first call pins the value
into a register. But the rule then fires on the source and the count too,
because the first call pinned those as well, and CSE relates them.

**There is no way to ask for the derivation on one register-pinned operand and
not its neighbours.** They are all live for the same reason and no flag
separates them. When a ROM sequence derives some operands and reloads others,
that asymmetry is currently unreachable.

## A hard round is worth reporting as one

Eight functions attempted in one round, none matched, best screens 3, 3, 10, 11,
12, 13 and 15 differing. Six of the eight came from the small-function pool
(12-30 instructions), and five of those stalled on **which register a value
lands in** with everything else identical.

That is worth stating plainly rather than presenting as progress: the small-band
pool is dense in register-permutation blockers, because a short function has few
enough pseudos that one allocation choice decides the whole screen and there is
no structure left to vary. The guarded argument-order pool, by contrast, has run
at roughly four matches in five this session.

**Selection lesson:** prefer functions with enough structure that source-level
choices still have leverage. A 13-instruction function that is 3 lines off is
not "nearly done" -- it is out of moves.

## Both split builds must be named, not just the repeated one

`OvlFunc_953_200a820` has six calls of the form
`f(slot, X << 2, 0x93 << 2)` behind a `GetFlag` guard, with the ROM
interleaving the slot constant into the two shifted builds. Getting there took
three separate applications of the same lever, and the order is instructive:

  1. **All inline** -- 61 of 86 differing. gcc hoists the two pooled SetSpeed
     constants into r5/r6 and adds a push.
  2. **Name the two POOLED constants separately per call site** (`s1,t1,s2,t2`,
     all four assigned before the guard) -- 43. The push disappears. Note
     `-fno-rerun-cse-after-loop` did NOT fix this one, unlike
     `OvlFunc_883_2008ba8`; separate locals did.
  3. **Name the repeated `0x93 << 2` six times** (`y1..y6`) -- 12, and the
     remaining twelve differences are all one pair of instructions swapped.
  4. **Name the varying `X << 2` too** (`x1..x6`) -- exact.

Step 4 is the point. After step 3 the second argument was a named local and the
third was inline, and the interleave did not appear. **A call with two split-build
arguments needs BOTH named** for gcc to rematerialise them in the ROM's order --
naming one and leaving the other inline is not a partial win, it is no win.

Twelve named locals in a 40-line function looks absurd and is correct.

## The HImode literal cuts both ways: 0 and 0x8000 need opposite treatment

Storing a constant through a `short *`:

  * `*(unsigned short *)p = 0;` pools the zero (`ldr r3, =0x0`). An **int
    intermediate** -- `v = 0; *(unsigned short *)p = v;` -- gives the ROM's
    `mov r3, #0x0`.
  * `*(short *)p = 0x80 << 8;` pools 0xffff8000, because the value sign-extends
    into HImode. An **unsigned short destination** with the shift written
    inline gives the ROM's `mov r3,#0x80 / lsl r3,#8`.

Two sibling functions in the same `.s` needed one each, which is how the pair
came up. When the ROM builds a halfword constant with `mov`(+`lsl`) and ours
pools it, the fix is one of these two and they are not interchangeable.

## The commoned-constant tell has TWO remedies and they are not interchangeable

An added push holding a constant used more than once is a reliable tell (batch
127). What fixes it is not:

  * `OvlFunc_883_2008ba8`, `OvlFunc_886_20081e8`, `OvlFunc_908_200835c` --
    `CSE_CFLAGS` (`-fno-rerun-cse-after-loop`) is exact; separate named locals
    change nothing.
  * `OvlFunc_953_200a820` -- separate named locals are exact; the flag changes
    nothing.

Both are one screen. **Try both before concluding anything**, and do it in that
order only because the flag needs no source edit; there is no evidence either is
more likely.

**But neither is guaranteed.** `OvlFunc_881_2009c08` shows the tell in textbook
form -- two pooled flag ids, each used twice, hoisted into r5 and r6 behind a
push the ROM does not have -- and BOTH remedies fail on it, as does every other
flag group in the tree. So the tell reliably identifies the CAUSE, but it does
not promise a fix; budget one screen for each remedy and park if neither takes,
rather than spending a round hunting for a third.

A guess at the distinction, offered as a guess: in the three flag cases the
constant is a *flag id* passed to two different functions across a branch, and
in the local case it is a pooled *argument pair* reused at two calls in the same
block. That would make it about whether the commoning happens before or after
the branch, but I have three cases against one and have not tested it.

That guess is now contradicted. `OvlFunc_881_2009c08` is flag ids reused inside a
single straight-line block (br == 0) -- the shape the guess predicts the named
locals should fix -- and they do not. Treat the distinction as unknown.

## Widening the interleave detector: r0 need not be zero

The batch-127 sizing looked for `mov r0, #0` inside a split build. The
single-instruction argument can be **any** constant -- `mov r0, #0xc`,
`mov r0, #0x14`, `mov r0, #0x15` all appear -- and widening the detector took the
fully-guarded pool from 97 functions to **194**, with much smaller members: the
smallest is 18 instructions where the narrow pool started at 85.

Two of this round's four matched on the first screen from that widened pool.
When a sizing looks small, check whether the detector is asking for something
more specific than the mechanism requires.

## `GetFlag(id)` guarding a block that ends `SetFlag(id)` means `CSE_CFLAGS`

Five of the last seven functions elevated from the guarded-interleave pool
needed `-fno-rerun-cse-after-loop`, and they share one shape:

    if (__GetFlag(id) == 0) {      /* or != 0 */
        ...
        __SetFlag(id);
    }

The id is materialised twice -- once for the test, once for the set. At `-O2`
the rerun-CSE pass commons the two into a callee-saved register and adds a push
the ROM does not have. Separate named locals do NOT defeat it in any of the five.

**Screen these with `--no-rerun-cse` from the start.** It costs nothing, and the
shape is visible in the ROM listing before writing a line of C: the same
`ldr r0, =<id>` or `mov r0,#K / lsl r0,#n` appearing once before a conditional
branch and once inside the guarded block.

The counter-example remains `OvlFunc_953_200a820`, where the repeated constant is
a pooled ARGUMENT PAIR used at two calls in the same block, and there separate
locals work and the flag does not. The distinction guessed at in the previous
note now has five cases against one and holds so far: commoning across a branch
wants the flag, commoning within a block wants separate locals.

## Two results of the same call need two pointer variables

`OvlFunc_960_2008dc8` calls `__MapActor_GetActor(0xe)` twice and uses each result
once. Written into one variable, gcc preserves the first pointer across its use
(`mov r2, r0 / add r2, #0x23`) where the ROM destroys it (`add r0, #0x23`).
Two variables took the screen from **50 differing to 14**.

The variable being reassigned is enough to keep the first value's live range
open in gcc's view even when it is dead. Give each call result its own name.

## Consume the pointer, do not index it

`a[0x62] = 0;` makes gcc copy the pointer before adding to it:

    ours  mov r3, r0 / add r3, #0x62 / strb r2, [r3]
    rom   add r0, #0x62 / strb r3, [r0]

Writing the mutation explicitly -- `a += 0x62; *a = 0;` -- consumes the pointer
instead. On `OvlFunc_964_20094ac` that single change took 28 differing to 7.

This only arises for offsets above 31, where thumb's immediate form is
unavailable and an `add` is required either way. Below that the question does
not come up. Combine with the earlier rule (two results of the same call need
two variables) -- they address opposite halves of the same problem: one keeps a
pointer alive that should die, the other kills one that should live.

## Name the stored constant, not just the mask

Naming the constant operand of a store or a bitwise op shifts which register the
POINTER gets. On `OvlFunc_931_200807c`, `z = 0; *p = z;` instead of `*p = 0;` at
three stores took 15 differing to 6. On `OvlFunc_964_20094ac`, `m = 0xf7;
v = *p & m;` took 7 to 4.

Neither reached exact, and in both cases what remained was the same register
swap between the constant and the value it acts on. The lever moves the
allocation but does not choose it.

## A detector bug that cost most of a round

Classifying the remaining functions by the `GetFlag(id)`/`SetFlag(id)` shape
reported **0 of 2227**, twice, through two rounds of "fixing" the wrong thing:

  1. First the operand pattern only accepted `ldr r0, =<id>`, missing ids built
     as `mov r0,#K / lsl r0,#n`. Widened it. Still zero.
  2. The actual bug: the call was matched with `x.strip() in ("bl __GetFlag",…)`
     and the assembly separates mnemonic from operand with a **TAB**, so
     `strip()` yields `bl\t__GetFlag` and nothing ever matched.

With that fixed: **191 functions**. The zero was never plausible -- five had
been elevated from that exact shape in the preceding two rounds.

The lesson is not "check for tabs". It is that when a detector returns zero and
the first fix does not change it, the second hypothesis should be about the
*harness* rather than another refinement of the pattern. Both wrong versions
were about the ROM's spelling; neither was about mine.

Note also the fixed detector has a known false positive: it looks back five
lines for the operand and takes a pool load in preference, so a nearby unrelated
`ldr r0, =X` can be attributed to the wrong call. `OvlFunc_964_20094ac` was
flagged that way and its two ids are actually 0x200 and 0x201. Screening both
flag settings costs one command and settles it.

## Selecting for the interleave lever: prefer call-dense scripts

Two consecutive rounds stalled on register permutation because I picked by SIZE.
The functions that convert are call-dense: a cutscene script is mostly argument
setup and `bl`, so the levers (name every split build, name the stored constant,
consume the pointer) act on nearly every instruction, and there are few enough
locals that allocation has little room to differ.

A usable filter, over the guarded pool:

    calls * 4 >= instructions        and        memory-ops * 4 <= instructions

That is 11 functions in the 30-90 band, and three of the four tried from it
matched. By contrast the small-band pool (12-30 instructions, mostly pointer and
bit work) produced zero matches in eight attempts.

**Density, not size, is the selector.** A 72-instruction script with 23 calls is
easier than an 18-instruction function with three.

## The boundary of the interleave lever

`OvlFunc_921_20082b8` is 2 of 74: one call out of twenty-three has its two
single-instruction arguments in the ROM's order and not gcc's, and **neither is
a split build**. The lever moves arguments around a two-instruction sequence;
where every argument is one instruction there is nothing to move them around,
and the source cannot reach the order.

Together with `OvlFunc_899_20099a4` (a split build exists but no dominating
block) this bounds the class from both sides. The lever needs a split build AND
a preceding branch; missing either, park it.

## The no-prototype lever fixes two-argument order

When the ROM sets up a two-`mov` argument block as `mov r1 / mov r0` and gcc
emits `mov r0 / mov r1`, **remove the callee's prototype**. A call with no
visible declaration gets the ROM's order.

I had parked this shape twice as unreachable ("neither argument is a split
build, so the interleave lever has nothing to work with"). The control refutes
it: **332 of 3428 solved functions** contain a two-`mov` block with r1 emitted
first. The smallest example, `src/overlays/rom_78603c/ovl_30_c_c_b.c`, declares
NOTHING at all -- four calls, no externs -- and produces
`mov r1,#0 / mov r0,#16` from `__Func_8093054(0x10, 0)`.

Applied to the two parks:
  * `OvlFunc_921_20082b8` -- 2 of 74 differing, **exact** on removing one
    prototype. The park is deleted.
  * `OvlFunc_950_20087b0` -- 2 of 61, **exact**, and elevated in the same round.

It does not always apply. `OvlFunc_952_20085a4` calls `__ActorMessage` in both
arms of a branch and the ROM orders its arguments differently in each, so no
single declaration choice satisfies both. Check whether the callee appears more
than once before reaching for this.

**Cost:** the file loses type checking for that callee, and gcc-2.96 warns under
`-Wimplicit`. Remove only the prototype the call needs, not all of them.

## Symbol bases: gcc spends a register for a symbol and not for an integer

The ROM often holds a message id in a callee-saved register and derives
neighbours from it:

    ldr r5, =0x2399 ... add r0, r5, #1 ... add r0, r5, #2

Written `int m = 0x2399;` gcc emits three independent pool loads and does not
keep `m` alive at all -- rematerialising a pool constant is cheaper than a
push/pop pair, so it never spends the register. Mutating the variable
(`m += 1;`) is worse: each arm then folds independently.

Declaring the base as an absolute symbol -- `extern int _MSG_2399;` and
`m = (int)(&_MSG_2399);` -- produces the register and both `add`s. On
`OvlFunc_950_20087b0` that took **73 differing to 3**.

This extends the batch-123 derived-constant rule with the lever it lacked. That
rule said derivation is reachable when the base is already forced into a
register by a runtime use; this says how to force it when nothing else does.
`message.sym` already carried a section comment describing exactly this shape.

## A function with no conditional branch has only the flag group

Both source-level levers for constant placement need a dominating block:

  * the **naming lever** against constant CSE -- separate locals assigned before
    a branch, rematerialised at their uses;
  * the **interleave lever** for argument order -- named split builds, likewise.

`OvlFunc_939_20095bc` is 23 calls, no memory operations and **no conditional
branch anywhere**. gcc commons its one repeated shifted constant into a
callee-saved register and adds a push; neither lever can reach it (naming leaves
the count unchanged, since there is nothing to rematerialise from), and
`-fno-rerun-cse-after-loop` does not either, so the commoning is the main -O2 CSE
pass rather than the rerun.

State it once and check it first: **in a straight-line function the only tool
left is the flag group.** If the flag does not fix it, park it without spending
screens on spellings. Two of this session's parks and two more before them were
straight-line cases where I tried the naming lever anyway -- and on one of them
it cost nine instructions and three extra pushes.

## Name a negated constant as `-1`, not as `x = 1; x = -x;`

For the interleave lever, constants are named at the top of the function so gcc
rematerialises them at their uses. A negated constant must be written as the
negative literal:

    c1 = -1;                 /* rematerialised as mov+neg at the use: exact   */
    c1 = 1; c1 = -c1;        /* a COMPUTED value: gcc holds it in r5 and the
                                prologue grows a push                          */

On `OvlFunc_891_200a244` the two-step form cost `push {r5, r14}` against the
ROM's `push {r14}` and 66 of 67 differing; the literal was exact.

The distinction is that gcc will rematerialise a constant but will keep a
computed value alive. Inside a guarded block the two-step form is fine and is
what several elevated functions use -- it is only wrong in the dominating block,
where the whole point is to be rematerialised.

## `tools/pool.py`

The candidate query had been rebuilt inline nine times across these batches,
twice with a bug that made it report zero. It is now one tool with the
corrections applied, and its columns answer the questions that decide the
approach before any C is written:

  * `br == 0` -- neither naming lever can work; only the flag group is left.
  * `flag2` -- one id feeds both a Get and a Set/Clear, so screen with
    `--no-rerun-cse` from the start. It predicted the flag group correctly for
    all three functions elevated this round.
  * `site` -- guarded interleave sites, the ones the lever can reach.

**Park exclusion is by function NAME.** Park files are named for the low
address, and overlay functions from different overlays share it -- every overlay
loads at 0x02000000. `OvlFunc_971_200808c` was being hidden by a park written
for `OvlFunc_881_200808c`. The names come from the park headers.

## Name CHEAP constants; leave POOL constants inline

The naming lever works because gcc rematerialises a named constant at each use
rather than keeping it live. That holds for constants it can rebuild in one or
two instructions -- `mov`, `mov`+`lsl`, `mov`+`neg`. It does **not** hold for a
constant that needs a literal-pool load: rematerialising a pool load is
expensive enough that gcc prefers to hold the value, so naming it produces the
opposite of what was wanted.

`OvlFunc_946_200967c` has four arms each storing 0x19999 and four each passing
`0xf2 << 18`. Naming all eight gave 65 of 82 differing, with `push {r5, r6, r7,
r14}` against the ROM's `push {r14}` -- the three pooled 0x19999 copies were
held in callee-saved registers. Naming only the shifted builds and leaving the
pooled constant as a literal at each store was **exact**.

Rule of thumb: if the ROM reaches the value with `ldr rN, =...`, do not name it.
If it reaches it with `mov` or `mov`+`lsl`/`neg`, name it once per use site.

**QUALIFIED (batch 133).** That is the right default but not a law.
`OvlFunc_959_200a52c` reloads `0xb333` and `0x5999` from the pool at each of
three `__MapActor_SetSpeed` calls, gcc commons them into r5/r6 and adds two
pushes, and naming SIX separate locals -- one pooled constant per use site --
made gcc rematerialise all six and took 56 differing to 4.

So the distinction is not simply cheap-versus-pooled. Both functions name pooled
constants; in one it costs three pushes and in the other it saves two. What
differs is where the uses are: `200967c`'s four uses are in four EXCLUSIVE
branches, so one live value covers all of them and holding is cheap;
`200a52c`'s three uses are sequential in one block, so holding spans the whole
run and rematerialising wins.

Practical form: name pooled constants when their uses are **sequential**, leave
them inline when the uses are in **exclusive branches**. Either way it is one
screen, so measure rather than reason.

## Adding a Makefile rule does not rebuild the object

`OvlFunc_885_20080dc` screened exact and its linked overlay differed in 18
bytes. The cause was the documented wildcard hazard -- the new file
`ovl_30_c_c_a_c_a_a_b.c` is caught by `rom_78603c/ovl_30_c_c_a_c_a%`, which
applies `O1_CFLAGS`, and all 18 bytes were argument-order swaps at four call
sites.

Adding an explicit `-O2` rule **did not fix it**, and for a few minutes that
looked like evidence the diagnosis was wrong. The Makefile is not a dependency
of the `.o`, so make reported the object up to date and never re-ran the new
recipe. Deleting `asm/<path>.o` (and its `.s`) made the same rule work
immediately.

Check with `make -n <the .o>`: "is up to date" means the recipe you just wrote
has not run. This applies to every flag-group change, so a flag rule that
"doesn't work" should be retested after deleting the object before it is
believed.

## Screening cannot see a wildcard

`tryc.py` compiles with the production flags for the file's own path, but a file
that does not exist yet has no path in the Makefile -- so the screen used `-O2`
while the build used `-O1`. The screen was right about the C and wrong about the
build.

**Before wiring a new `.c` into a directory, grep the Makefile for a wildcard
covering it.** The twelve outstanding cases are listed in HANDOFF.md as owed
work; this is the first one to have actually bitten, and it cost a green screen
followed by a red build.

## Correction: the no-prototype lever is not ruled out by a callee appearing twice

Batch 130 recorded a discriminator for the no-prototype lever: that it cannot
work when the callee is called in two arms of a branch and the ROM orders its
arguments differently in each, because "no single declaration choice can satisfy
both". **That reasoning is wrong.**

`OvlFunc_921_2008974` calls `__Func_80b0278` in two arms -- `mov r0,#0xc /
mov r1,#0xf` in one and `mov r1,#0xe / mov r0,#0xc` in the other -- and dropping
the single prototype matches both. gcc's unprototyped argument ordering is not
one fixed order; it varies per call site the same way the ROM's does.

The lever still fails on `OvlFunc_952_20085a4`, but not for that reason. What
actually decides it is unknown, so the correct rule is simply: **try it, it is
one screen.** The two-arm case is not a reason to skip it.

## A struct passed BY VALUE: `ldmia`/`stmia` into the argument area

    add  r5, sp, #8
    mov  r0, r5 / bl OvlFunc_927_2008474      @ &s
    mov  r3, sp / add r2, sp, #0x18
    ldmia r2!, {r0, r1} / stmia r3!, {r0, r1}
    ldr  r3, [r5, #0xc] / ldr r0, [r5] / ldr r1, [r5, #4] / ldr r2, [r5, #8]
    bl   OvlFunc_927_2008608

The `ldmia`/`stmia` pair copying from a local into the stack argument area, with
the first four words also loaded into r0-r3, is gcc passing a **struct by
value**: six words, four in registers and the tail block-copied. Writing the six
fields as six separate arguments gives `ldr`+`str` pairs instead.

`OvlFunc_927_2008f94` matched on the first screen once the callee was declared
`void f(struct S s)` with a six-int struct. Worth recognising: a block move into
`[sp]` immediately before a call is a by-value aggregate, not hand-written
argument marshalling.

## The HImode-literal rule is not one rule

`OvlFunc_901_200858c` uses the same `unsigned short *` twice and needs opposite
spellings:

  * `*p |= 2;` -- a COMPOUND assignment -- gives the ROM's **pooled** constant
    (`ldr r2, =0x2`). The three-step form `v = 2; v |= *p; *p = v;` gives
    `mov r3, #0x2` and is 45 differing. `*p = 2 | *p;` does not reach it either.
  * `*p = 1;` gives a **pooled** `ldr r3, =0x1` where the ROM has `mov r3, #0x1`;
    an int intermediate (`v = 1; *p = v;`) fixes it.

So on one pointer, one operation wants the pool and the next wants a `mov`, and
the spelling that produces each is different. Treat "pooled or not" as something
to measure per operation rather than derive from the width.

## Two unsolved twins are a two-for-one; `twin_families.py` finds them

`solved_twins.py` searches remaining functions against SOLVED ones, so it cannot
see a pair where neither is done yet. `twin_families.py` groups the remaining
functions against each other, and that is where those pairs live.

`OvlFunc_899_200891c` and `OvlFunc_902_2008204` are 87 instructions with
IDENTICAL opcode streams in different overlays. Diffing the two listings with
labels and branches filtered out showed exactly two differing lines -- one
argument and one callee. The first matched on its first screen; the second was a
`sed` away.

**41 families of 2+ cover 90 remaining functions, and 13 of those families have
no parked member** -- 26 functions in untried shapes, each family a two-for-one.
That is the cheapest queue currently known, and it is worth re-running as parks
close: a family whose members are all parked is a shape already known to be
blocked, and one with none parked has simply not been tried.

Working method for a family: diff the two listings with
`grep -vE "^\.L|\tb\t|bne|beq|bhi|bls"` on both sides. What survives is the
immediates and callees that differ, which is the whole edit.

## Name a POOLED argument to reach an interleave

`OvlFunc_945_200dc48` ends its guarded arm with

    mov r0,#1 / mov r1,#1 / neg r0,r0 / neg r1,r1 / ldr r2, =0xe666

-- the pooled third argument emitted AFTER both `neg`s, where gcc puts it
between the `mov`s and the `neg`s. Naming `e = 0xe666;` in the dominating block
was exact.

That extends the interleave lever: the argument being moved does not have to be
a `mov`-built constant. A pooled one works too, and the same precondition
applies -- the naming must be in a block that dominates the call. Naming it
inside the guarded arm instead leaves the count unchanged.

Note this does not contradict "leave pooled constants inline": that rule is
about a value used at SEVERAL sites, where naming makes gcc hold it. Here it is
used once, so there is nothing to hold and nothing to lose.

## Working a twin family: two `sed`s and two screens

`OvlFunc_945_200dc48` / `OvlFunc_895_2009ac8` are 37 instructions with identical
opcode streams. The filtered diff showed four differing lines -- a data label, a
compare immediate, a sound id, and one shift amount -- and the second matched on
its first screen from a `sed` of the first.

The second's `.s` also held the `.data` and `.bss` its function uses, all three
named separately in the overlay linker script. Split by hand into `_b` (text)
and `_c` (data + bss), pointed the three lines at the two objects, and verified
byte-neutral with the function still in asm before the `.c` landed.

## The epilogue register tells you the return type

    rom   pop {r5} / pop {r1} / bx r1
    ours  pop {r5} / pop {r0} / bx r0

gcc pops the return address into **r0 when r0 is dead** -- that is, when the
function returns void -- and into another low register when r0 carries a return
value. So a ROM epilogue that avoids r0 says the function returns something,
even when nothing in the body looks like a `return`.

On `OvlFunc_971_20091bc` the body ends with a call whose result is otherwise
discarded; declaring the function `int` and writing `return __CloseUIBox(h, 1);`
matched the epilogue exactly. This is cheaper to check than it looks -- the
epilogue is the last three lines of every screen.

Related and already recorded: a redundant-looking copy of a value into a second
register before a loop can mean the function returns that value (batch 127,
`OvlFunc_971_200853c`). Both are the same underlying fact -- r0 is reserved when
there is a return value -- read from two different places.

## Source order of two loads decides which gets r8 and which gets r10

When two values are loaded before the first call and both must survive it, gcc
puts them in high registers -- and WHICH high register each gets follows the
order of the assignment statements in the source, not the order the loads are
emitted in.

On `OvlFunc_969_200db90` the ROM is:

    ldrh  r2, [r6]        @ angle
    ldr   r1, [r5, #0x68] @ pointer
    mov   r8, r2
    mov   r10, r1

The obvious transcription -- assign the angle first, the pointer second -- gave
the opposite pairing (angle in r10, pointer in r8). Swapping the two assignment
statements fixed it, and the emitted load order did NOT swap with them: the
loads are ordered by first USE (the angle is the argument to the first call),
while the allocation is ordered by the source statements. Those two orders are
independent, so you can set them separately.

This cost 20 of 41 differing lines, and it is invisible in the diff as anything
but noise -- the register numbers are simply transposed everywhere downstream.
When a diff shows two high registers consistently swapped and nothing else
structural, reorder the source assignments before trying anything cleverer.

## mul copies the second operand

`mul rD, rS` computes rD = rD * rS, so one operand must be copied into the
destination first. gcc copies the operand on the RIGHT of the C expression:

    r * c   ->  mov r2, r0(c) / mul r2, r3(r)
    c * r   ->  mov r2, r3(r) / mul r2, r0(c)

Multiplication commutes, so both are correct and only one matches. Read which
value the ROM's `mov` copies and put the OTHER one on the right.

## The no-prototype lever works best on a callee used MANY times the same way

Batch 130 recorded that dropping a callee's prototype flips gcc's argument-setup
order, and batch 133 recorded that this fails when the callee appears at sites
with different argument orders -- gcc's unprototyped order matches one site and
breaks the other.

The converse is the strong case, and `OvlFunc_974_2008bb8` is the clean example:
thirty-five calls to one callee, all `(who, item)`, and the ROM sets r1 before
r0 at every one of them. With a prototype gcc set r0 first at all thirty-five --
66 of 116 lines differing, in one perfectly regular pattern. Removing that one
prototype matched the function exactly.

So read the REGULARITY of the difference, not just its shape:

  * one callee, many sites, the SAME swap at every site -> drop its prototype;
    this is close to a certainty rather than a gamble
  * one callee, several sites, the swap at SOME sites only -> the lever will
    trade one set of sites for the other; don't spend the round on it

Drop the prototype for that callee ALONE. The other callees in the same function
keep theirs and are unaffected, so there is no reason to widen the change.

A useful corollary for selection: a flat call-sequence function whose diff is
large but perfectly periodic is usually a ONE-line fix, and is worth picking
ahead of a function whose diff is small but irregular.

## Symbol base: index a typed array, don't cast-and-offset

The symbol-base lever has a specific spelling that matters. Given a ROM store of

    ldr  r3, =L2
    strh r2, [r3, #0x1e]

the transcription that FAILS is the pointer-arithmetic one:

    extern unsigned char L11[] __asm__(".L11");
    *(short *)(L11 + 0x1e) = a * 60;      /* ldr r3, =L2+30 ; strh r2, [r3, #0] */

`L11 + 0x1e` is a link-time constant, so gcc folds the displacement into the
pooled address and stores at offset zero. Declaring the symbol with the type
the access actually uses, and indexing it, keeps base and displacement apart:

    extern short L11[] __asm__(".L11");
    L11[0xf] = a * 60;                    /* ldr r3, =L2 ; strh r2, [r3, #0x1e] */

A struct with explicit padding works identically, but it invents a layout the
assembly does not attest to; prefer the array when the access is uniform.

Both differences in `OvlFunc_common1_ea0` were this one decision -- the pooled
constant and the store displacement are two symptoms, not two problems. When a
diff shows `=SYM+N` against `=SYM` AND a zero store offset against a nonzero
one, that is a single fix.

Also confirmed here: `a * 60` reproduces the ROM's strength-reduced
`((a << 4) - a) << 2` with no help, and the rotated `b test / body / test:`
loop reproduces from a plain `while`.

## Match linker-script references on the full path, never the basename

Overlay directories reuse filenames heavily: ten different directories under
`asm/overlays/` contain a file called `ovl_30_c_c_c_c_c.s`, and they are
unrelated to each other.

Checking "which linker scripts reference this TU" with the basename therefore
lies in both directions. Before splitting `asm/overlays/rom_7f21b8/ovl_30_c_c_c_c_c.s`
a basename grep claimed 36 scripts referenced it; afterwards the same style of
grep claimed 10 scripts still pointed at the deleted object, which looked like
`split_s.py` had missed them. Both numbers were noise. Exactly one script --
`overlays/rom_7f21b8/overlay.ld` -- ever referenced that file, and the tool had
updated it correctly. The other hits were each overlay's own same-named file,
still present and untouched.

So grep for `asm/overlays/<dir>/<stem>.o`, with the directory, and treat a
basename match as evidence of nothing. The cheap confirmation either way is the
byte-neutral `make compare` after the split and before any C lands -- a genuinely
missed linker script cannot survive it.

Also worth checking before reaching for split_s.py at all: a `.s` holding ONE
function and no data needs no split. The `.c` replaces it at the same stem, the
generic `asm/%.o: src/%.c` rule builds it with default GCC296_CFLAGS, and every
linker script keeps working untouched. `OvlFunc_974_2008f14` was this case.

## The symbol-base lever is bounded by the displacement range

Indexing a typed array instead of casting-and-offsetting keeps a symbol's offset
as an addressing-mode displacement, which is how `OvlFunc_common1_ea0` matched
its `strh r2, [r3, #0x1e]`. That lever has a hard boundary: it only works while
the offset FITS the displacement field.

`Func_8094428` reads `gState + 0x1f4`. Thumb word loads cap their displacement
at 124, so 0x1f4 cannot be a displacement at all -- the offset has to be
materialised in a register either way. At that point gcc folds it into the
pooled address (`ldr r3, =gState+500`) while the ROM keeps them apart
(`ldr r3, =gState / mov r2,#0xfa / lsl r2,#1 / add r3,r2`). Every spelling tried,
including the `short[]` index the ROM's own `mov #0xfa / lsl #1` suggests, folds.

So before reaching for the lever, check the offset against the mode:
byte/halfword/word displacements are 31/62/124. Above that the lever is not a
candidate and the difference is a park, not a spelling problem.

## The same call is spelled both ways in the same ROM

`Func_80933f8(-1, -1, -1, 0)` appears in both `OvlFunc_965_2008eac` and
`Func_8094428`. In the first the ROM builds -1 three separate times and our
commoning of it is the blocker; in the second the ROM commons it exactly as gcc
wants to. Same callee, same arguments, opposite codegen.

That is worth holding onto whenever a constant-reuse difference tempts a theory
about the callee or the constant: the choice belongs to the translation unit the
call sits in, not to the call. Do not carry a reuse verdict from one function to
another, and do not park a second function by analogy with the first -- screen it.

## Levers compose: four spellings on one function, 70 differing to zero

`OvlFunc_938_2008264` is the clearest case so far that the levers stack, and
that a large diff can be several small blockers rather than one big one:

    plain transcription                             70 differing
    + int intermediate for the halfword stores       9
    + name the shifted argument (y)                  8
    + name the pooled argument (e) as well           6
    + name both stack arguments (s1, s2)             0

Each step is a documented lever and none of them subsumes another. Two things
worth carrying forward:

  * **Re-diff after every lever.** The count barely moved from 9 to 8, which in
    isolation looks like a dead end; but the first difference had jumped from
    line 49 to line 65, meaning the early blocker was solved and a later one had
    surfaced. Read WHERE the first difference is, not just how many there are.
  * **Naming one argument can be insufficient rather than wrong.** Naming `y`
    alone left the r0/r1 pair transposed; naming `e` alone did nothing at all.
    Together they were exact. When a lever "fails", try it combined with its
    neighbours before recording it as inapplicable.

## Two stack arguments want two named locals

A call with more than four arguments spills the rest to `[sp]`. Passed as bare
literals gcc reuses ONE register for the spill:

    ours  mov r3,#0x4 / str r3,[sp] / mov r3,#0x3 / str r3,[sp,#4]
    rom   mov r3,#0x4 / mov r2,#0x3 / str r3,[sp] / str r2,[sp,#4]

The ROM materialises both values into two registers and only then stores both.
Naming them as two separate locals immediately before the call gives gcc two
pseudos and reproduces it. Both six-argument calls in `OvlFunc_938_2008264`
needed it, and one fix covered both.

## Measure the remaining count, never decrement it

Batch reports 130-133 each carried a "remaining functions" figure, and the series
ran 2224 -> 2219 -> 2214 -> 2208 -> 2202 with every step exactly equal to that
batch's elevation count. That is the signature of a hand-maintained counter, not
a measurement: nothing in it ever re-reads the tree, so a baseline error persists
forever and grows invisible. By batch 134 it sat 46 below the truth.

`tools/remaining.py` measures it. Counting four ways -- raw occurrences and
distinct names, each with and without excluding TUs that already have a `.c` --
gives the same 2248, so there is no definitional ambiguity to argue about.
gcc-generated `.s` intermediates use `.thumb_func` rather than
`.thumb_func_start` and so never contaminate the count; no exclusion rule is
needed at all.

The general point is worth more than the number: **a figure that is only ever
derived from the previous figure is not evidence.** If a report states a
quantity, it should come from re-measuring the thing. This is the same
discipline as the "a zero component means the regex is broken" rule -- both are
about not letting a number pass without something checking it against reality.

## The source-order lever does not reach commutative operands

Assignment order picks registers for two independent values, and that lever has
now paid off repeatedly. It has a boundary worth knowing: it does NOT reach the
two operands of a commutative operator.

`OvlFunc_932_200a5c0` needs

    rom   ldrb r2, [r5] / mov r3, #0x2 / orr r3, r2
    ours  ldrb r3, [r5] / mov r2, #0x2 / orr r3, r2

-- the same `orr` and the same store, with only the two source registers
exchanged. Seven spellings were screened: `2 | *p`, `*p | 2`, `*p |= 2`, both
orderings of two named locals feeding `c | v`, an indexed form with no pointer
advance, and four flag groups. Every one gives the identical two differing lines.

The reason is worth stating because it predicts where else the lever will fail:
for two independent values gcc has a genuine ordering choice to express, so the
source can influence it. For `a | b` the operands are interchangeable in the RTL
before allocation, so there is nothing left in the source for the ordering to
attach to. Expect the same on `&`, `^`, `+` and `*` when the only difference is
which operand landed in which register.

Do not spend a round on this shape. One screen to confirm it is commutative-role
rather than something reachable, then park.

## The interleave lever moves the argument you do NOT name

`OvlFunc_932_200a9dc` was parked with the note "NEXT: nothing. This is the
documented limit of the lever rather than a new shape." It was two lines from
matching, and the lever did reach it.

The park had tried naming the interleaved argument itself -- the `mov r0, #9`
that needs to move into the split build -- and correctly found that the slot is
used both before and inside the `if`, so naming it violates the third clause
(every repeated use must be in a different block from the assignment).

That reasoning is sound and irrelevant. **Name the OTHER arguments.** Naming the
two split builds `x` and `y` in the dominating block, and leaving the slot as a
bare literal, is exact:

    rom   mov r1,#0xb8 / mov r2,#0xa4 / mov r0,#9 / lsl r1,#16 / lsl r2,#17

Naming the split builds is what frees gcc to place the single-instruction
argument between the movs and the shifts. Trying to name the instruction you
want moved constrains the very thing you are trying to let float.

So when a park says "the lever cannot reach this because the interleaved
argument cannot be named", that is not a conclusion -- it is the wrong half of
the call. Check whether the OTHER arguments are nameable before believing it.

## Which interleave parks are worth re-attacking

Of 48 interleave-class parks with a live `.s`, only **9** have a GUARDED site --
a conditional branch before the interleave, which is what gives the lever a
dominating block to name in. The other 39 have the site before any branch (or no
branch at all), where naming hoists constants into callee-saved registers and
adds pushes instead.

That distinction is not "does the function contain a branch" -- `OvlFunc_967_2008308`
has one and is still unreachable, because its site precedes it. `pool.py`'s
`site` column already computes it correctly; use that, not a branch count.

## The register-role swap is now the dominant wall

An audit of the 253 function parks puts the blocker classes in this order:
scheduling/placement 90, interleave 51, constant reuse 14, register-role swap
12, symbol base 10, HImode 8. But that ranking is misleading about where the
work is, because the classes are not equally tractable.

The INTERLEAVE class is effectively closed as a lever target. Of 48 interleave
parks with a live `.s`, only 9 have a guarded site, and working through them
found that in every one except `OvlFunc_932_200a9dc` (recovered, see above) the
interleave was ALREADY solved and something else was blocking. The useful filter
is `site > 0 AND unguarded == 0` in `pool.py`; a function with any unguarded site
is usually blocked at that site, not the guarded one.

What those parks are blocked on instead, repeatedly, is the REGISTER-ROLE SWAP:
the right instructions in the right order with two registers exchanged. Twenty-one
parks describe it. It shows up in at least four dressings:

    base vs offset      ldr r3,=gState / mov r1,#0xe0   (ours: r2 and r3)
    address vs value    mov r3,r5 / mov r2,#0x14        (ours swapped)
    commutative operand ldrb r2,[r5] / mov r3,#0x2      (orr identical either way)
    pointer vs constant  ldr r1,[r3] / mov r3,#0xe0

Nothing in the inventory reaches any of them. Source order picks registers for
two INDEPENDENT values, and that lever keeps paying; it does not pick them for
two values feeding one operation. Declaration order, statement order, separate
locals per chain, and every flag group have been screened across several of these
and none moves the count.

So the honest strategic read: the cheap classes are worked out, and roughly a
fifth of the park corpus is waiting on one unsolved question. A lever for the
register-role swap would be worth more than any number of individual attempts,
and the best test cases are the small ones -- `OvlFunc_923_20091b4` at 2 and
`OvlFunc_932_200a5c0` at 2, both under 110 instructions with a single disagreeing
block.

## A corpus-wide unsolved shape: symbol pool load before constant pool load

    ldr rA, =<symbol>  /  ldr rB, =<constant>  /  add rA, rB

gcc emits the two loads in the opposite order -- constant first -- and no source
form tried has changed that. Worth stating precisely because the scope was
measured rather than guessed:

  * the shape occurs at **25 sites** in the remaining assembly
  * across **3,156 elevated translation units it appears zero times**

So when a park is blocked on it, that is not a lever somebody else already knows.
Nothing in this tree has ever produced it. Registers and instructions are
identical on both sides; only the order of two independent pool loads differs,
which also means it is cheap to recognise and cheap to rule out.

Screened on `OvlFunc_923_20091b4` (2 of 28): nine source forms -- walk, one
expression, array index, named offset before AND after the pointer assignment,
non-compound add, gState as struct and as array -- and six flag groups. SCHED2
and O1 are both worse, so the ROM had them on; `-fno-schedule-insns` changes
nothing, so sched1 is not the pass responsible.

Do not spend a round on an instance of this. Recognise it, record it, move on --
and if a lever is ever found, 25 sites are waiting.

## Naming to reorder only works when the values already cost the same

The source-order lever moves two values between registers. `Func_80a3d6c` shows
where it stops: the two values there are a pooled mask and a zeroed counter, and
naming the mask -- in either assignment order, and in either declaration order --
costs TWO INSTRUCTIONS rather than swapping two registers (20 lines against the
ROM's 22, 19 differing, versus 6 differing for the plain form).

The distinction to check before reaching for it: are both values already going to
exist in registers regardless? If yes, naming only changes which register each
gets, and the lever applies. If naming is what *creates* the value -- a constant
gcc would otherwise fold, sink, or rematerialise -- then naming changes the
instruction count and the comparison is no longer about ordering at all.

Read the line count, not the differ count, to tell these apart. A naming attempt
that changes the length has answered a different question than the one asked.

## A queue worth working: halfword parks that never tried the int intermediate

54 parks mention a halfword store or HImode constant. The int-intermediate lever
-- store through an `int` local rather than assigning straight into the `short`,
which took `OvlFunc_938_2008264` from 70 differing to 9 -- has been tried on only
FOUR of them. The other 50 predate it.

That is the largest untried lever/park intersection currently known. It is not a
promise: several of those parks mention halfwords incidentally and are blocked on
something else, as `Func_80a3d6c` and `SetTextColor` both turned out to be. But
it is a queue built from evidence rather than from guessing at the next function.

## The mid-function literal pool is a translation-unit property

`Func_80b0a20` is one instruction from matching, and the instruction is a `b`
that jumps over an in-function literal pool placed before the epilogue:

    b .Lb0a64 / .pool_aligned / .Lb0a64: / pop {r5, r6} / pop {r0} / bx r0

`.pool_aligned` is `.align 2, 0` + `.pool`, so this is pool PLACEMENT.

> **SUPERSEDED -- DO NOT ACT ON THE PARAGRAPH BELOW.** It says no source
> spelling reaches this and that pools never appear early. Both halves are
> wrong. It also names **old_agbcc**, which is not the compiler for these
> translation units at all -- old_agbcc builds only `src/lib/m4a` and
> `src/lib/agb_flash`; everything else is patched **gcc-2.96**, which DOES emit
> mid-function pools (64 of them in already-matching code). See "The
> branch-over-pool class is not a ceiling" below, where a function was elevated
> straight out of this class with plain literals and no special handling. The
> paragraph is kept only so the reasoning that produced it stays visible.

Measured (and now known to be wrong): across every elevated translation unit,
zero generated `.s` files contain a mid-function pool -- old_agbcc emits at
`.func_end` and never early.

What makes it a lead rather than a dead end is that three of the four functions
in that `.s` carry one. Early pool dumping looks like a property of the original
TU, which means a single-function `.c` may not be able to match any of them, and
the cluster form -- several functions in one `.c`, which the "Cluster X..Y"
headers show is supported here -- is the experiment worth running.

Recognising this is cheap and worth doing early: if the ROM listing has a `b`
immediately before a `.pool_aligned` that is not `.func_end`'s own, the missing
instruction is pool placement and no amount of respelling the body will find it.

## The no-prototype lever does not always trade one site for another

Batch 133 recorded that the lever fails when a callee's sites disagree about
argument order, because gcc's unprototyped order matches one and breaks the
other. `OvlFunc_959_200cbfc` is exactly that shape and the lever worked anyway.

`__Func_8092c40` is called three times. The ROM sets r1 before r0 at the first
site and r0 before r1 at the other two. With a prototype gcc emitted r0 first
everywhere, so one site was wrong. Dropping the prototype fixed that site and
LEFT THE OTHER TWO CORRECT -- 4 differing to 2.

So "the sites disagree" predicts nothing on its own. gcc's unprototyped ordering
is not a single fixed rule applied uniformly; it can land differently at
different sites in the same function. I predicted this would trade one for two
and screened it anyway, which is the only reason it was found. One screen is
cheaper than the reasoning about whether to bother.

## The mid-function pool is NOT a translation-unit property (refuted)

The previous entry proposed that early literal-pool dumping might be a property
of the original translation unit, and that elevating a whole cluster into one
`.c` might reproduce it. **That is refuted**, and cheaply.

Compile a function alone, then compile it again with a second function appended,
and diff where old_agbcc puts the pool. It does not move: both end

    pop {...} / bx r0 / .L4: / .align 2, 0 / .L3: / .word ...

-- pool after the epilogue, no branch, because control never reaches it. The ROM
instead branches over a pool placed BEFORE its epilogue. Both sides emit a label
at the same point; only the order differs.

So this is old_agbcc's constant-pool emission and nothing about source form or TU
composition reaches it. A park blocked on a `b` over a `.pool_aligned` is blocked
for good with the current toolchain.

**The method is the transferable part.** The hypothesis would have cost three
function transcriptions to test the obvious way, and one compile to test the
cheap way. When a hypothesis is about compiler behaviour rather than about a
specific function, construct the smallest input that would show it -- a real
function plus a two-line dummy -- rather than building the full case first.

## The interleave lever works when the guard is a call, and scales to many sites

`OvlFunc_948_200a188` is the strongest confirmation of the guarded-interleave
lever so far: FOUR sites, all the same shape, all fixed at once by naming the two
split builds in the block dominating each guarded call and leaving the slot a
bare literal. 12 differing to exact.

Two things it settles that `OvlFunc_932_200a9dc` left open:

  * **The guard may be a CALL.** There the guard was a memory comparison, so the
    named values never crossed a call and the lever was cheap by construction.
    Here each guard is `__GetFlag(...)`, so the named values cross a call and
    could have cost callee-saved registers and pushes. They do not -- gcc
    rematerialises them per arm and the function still pushes only lr. So
    "naming before a call is expensive" is not a reason to skip the lever;
    screen it.
  * **Naming inside the arm still fails.** The same file with x and y assigned
    inside each `if` body instead of before it gives the identical 12 differing.
    Two functions have now shown this both ways round, so the dominance
    requirement is established rather than suspected.

Selection note: `pool.py` predicted this exactly -- `site 4, unguarded 0`. That
filter (guarded sites present, unguarded sites zero) has now called three
functions correctly and is the cheapest way to find candidates for this lever.

## "r0 in the middle" and "r0 at the end" are different problems

`OvlFunc_954_20095e0` had eight argument-order differences across five call
sites, and they needed OPPOSITE fixes in the same function:

  * **r0 wanted in the MIDDLE** of another argument's split build, at
    `__MapActor_SetSpeed` and `__MapActor_Emote`. Fixed by naming the other
    arguments in the block dominating the guarded region. 13 differing -> 9.
  * **r0 wanted at the END**, after every other argument, at
    `OvlFunc_common1_1078`, `__Func_8092adc` and `OvlFunc_common1_5e4`. Naming
    cannot produce that; DROPPING those three prototypes can. 9 -> exact.

The two levers push in opposite directions, and reaching for the wrong one looks
like the right one failing. Read the ROM at each site and ask which position r0
occupies:

    ... / mov r0 / lsl r1 / ...     -> r0 is INSIDE a split build: name the others
    ... / mov r1 / mov r2 / mov r0  -> r0 is LAST: drop the prototype

A single function can want both, at different sites, for different callees. That
is not a contradiction and it is not a sign the first lever failed.

## The division helper names the signedness

gcc emits four different helpers for `/` and `%`, and which one the ROM calls
says directly whether the operands were signed:

    __divsi3  __modsi3    -> signed
    __udivsi3 __umodsi3   -> unsigned   (in overlays, via the _RAM aliases)

On `OvlFunc_943_2009684` the first transcription declared `extern int __Random(void)`
and wrote `__Random() % 0x5a`, which emits `__modsi3`. The ROM calls the unsigned
helper. Changing the declaration to `unsigned int` took the screen from 55
differing to 17 in a single edit.

This is a free signedness oracle and it is worth reading BEFORE guessing at
types from the values. Note it also survives the overlay alias: `bl __umodsi3`
and the ROM's `bl _umodsi3_RAM` are the same symbol once overlay.ld's
`__umodsi3 = _umodsi3_RAM;` applies, so a screen that shows only that pair is
showing no real difference at all -- check the overlay's linker script before
counting those lines against yourself.

## Something has to cross the call

`OvlFunc_943_2009684` stores a built constant through a pointer that a call
returns:

    rom   bl GetActor / mov r3,#0x80 / lsl r3,#8 / strh r3,[r0,#6]

The ROM computes the value AFTER the call, so it lives in a scratch register and
nothing crosses the call at all. Neither available spelling reproduces that:

  * value in a named int -> assigned before the call, must survive it, gcc gives
    it callee-saved r5
  * pointer in a named local -> the POINTER survives instead and takes r5, and
    the store becomes `strh r3, [r5, #6]`

Both are correct code and both cost a callee-saved register the ROM does not
spend. Recognising the shape is worth a screen or two, not a round: if the ROM
has nothing live across the call and every spelling of yours does, the value is
being computed on the wrong side of it and C gives no way to say which.

## A `sub sp, #N` difference is the local's SIZE, not the body

`OvlFunc_896_200c3bc` screened at 2 of 97 on its first transcription, and the
only real difference was

    rom   sub sp, #0x38        ours  sub sp, #0x50

The function looked expensive -- an eight-argument call with four words spilled
to `[sp]`, 16.16 fixed-point masking, a counter in a high register -- and all of
that was already right. The frame was wrong because the local `struct` had been
padded to 0x40 when it needed to be 0x28.

The arithmetic is worth stating because it is exact rather than a guess:

    ROM frame  -  outgoing stack-argument area  =  size of the local aggregate
    0x38       -  0x10 (four spilled words)     =  0x28

Count the words the largest call spills to `[sp]`, subtract, and size the
aggregate to match. When a screen differs only in `sub sp`, do not read the body
at all -- nothing in the body can change the frame.

Related and confirmed here: `__Random` declared `unsigned` is what makes the
masking come out as `lsr`. Same signedness question as the division-helper note
above, and the same answer -- read the instruction, not the value.

## The interleave lever scales to six sites, including both arms of a branch

`OvlFunc_907_20089cc` is the largest single application so far: SIX call sites,
all the same shape, all fixed by one naming pass. 13 of 126 differing to exact.

Two details worth carrying:

  * **Both arms of an inner `if` are covered by one naming.** Four of the six
    sites are two matching pairs in the then- and else-arms of a nested branch.
    Naming all seven shifted arguments before the OUTER `if` places every one,
    even though only half of them execute on any path. There is no need to name
    per-arm, and doing so would put the assignment in the call's own block.
  * **Read the count of sites from `pool.py` before starting.** This function
    reported `site 6, unguarded 0`, and that was exactly the number of
    differences. When the site count and the differing count agree, the whole
    diff is one lever and the function is worth taking ahead of a smaller one.

## 14% of what remains is unreachable: run tools/poolblocked.py first

`tools/poolblocked.py` scans the remaining `.s` files for a function that jumps
over its own literal pool:

        b .L6a0
        .pool_aligned
    .L6a0:

> **SUPERSEDED -- THIS IS NOT A CEILING.** The paragraph below concludes that a
> function carrying this shape "cannot match however correctly the body is
> transcribed". That is false, and acting on it costs real functions: an agent
> in batch 153 skipped `StartSnow` without writing a line of C, citing exactly
> this text. Two things are wrong with it. **old_agbcc is not the compiler
> here** -- it builds only `src/lib/m4a` and `src/lib/agb_flash`, and everything
> else is patched gcc-2.96. And gcc-2.96 emits these pools readily; the shape
> is `.word` where this text expected `.pool`. `OvlFunc_881_200b8fc` was
> elevated straight out of the class with plain literals. **Screen these
> functions; do not write them off.**

That `b` is a real instruction and old_agbcc cannot produce it -- it emits pools
at `.func_end` and never early. Measured earlier: mid-function pools appear in
ZERO elevated translation units, and the cluster hypothesis was tested and
refuted. So a function carrying this shape cannot match however correctly the
body is transcribed.

**312 of the 2,239 remaining functions carry it -- 13.9%.**

That number matters for planning twice over. It is a real ceiling on what this
toolchain can reach, and it means a candidate-ranking tool that does not exclude
these will keep offering them. `OvlFunc_974_200829c` is exactly that trap: 588
instructions, THREE distinct callees, no conditional branches, no shifts,
reuse 0 -- the most attractive profile in the whole dense queue, and unreachable.
Drafting it would have been 196 calls of transcription for nothing.

Run the scan before drafting anything large. It costs one pass over the tree.

## Whether a sibling's levers transfer depends on the register budget

`OvlFunc_945_2008cc8` matched on its FIRST screen because the two levers its
sibling `OvlFunc_945_2008b84` needed -- `CSE_CFLAGS` for a flag read at the top
and set at the bottom, and a dropped prototype on `__Func_8092c40` -- applied
unchanged.

That is not automatic. `OvlFunc_955_2009424` is a near-twin of the elevated
`OvlFunc_954_20095e0` and its spelling did NOT transfer: the sibling names the
gState base to stop gcc folding it, and naming it there costs a fourth
callee-saved register that the ROM does not spend.

The distinction is the register budget, not the shape. Two functions with the
same structure AND the same number of values live across calls behave the same;
two with the same structure and different pressure do not. Before copying a
spelling from a twin, count what the ROM pushes in each.

## A missing TYPE can masquerade as a register-allocation wall

`OvlFunc_932_200a5c0` sat parked at 2 of 107, filed under the commutative
register-role swap -- the class 21 parks describe, which had survived seven
spellings and four flag groups across two rounds:

    rom   ldrb r2, [r5] / mov r3, #0x2 / orr r3, r2 / strb r3, [r5]
    ours  ldrb r3, [r5] / mov r2, #0x2 / orr r3, r2 / strb r3, [r5]

It was not an allocation problem. Written as pointer arithmetic on an
`unsigned char *`,

    p += 0x23;  *p = 2 | *p;

gcc puts the LOADED BYTE in the orr's destination. Written as a field of a struct
that names the byte,

    p->flags |= 2;

it puts the CONSTANT there, which is what the ROM does. Exact on the first try.

**But this does not generalise, and the counterexample matters as much.** The
`OvlFunc_943_20090a0` twins are parked at 2 on the same-looking shape at actor
offset 0x5a, and the same change makes them WORSE -- 102 lines to 106, 2
differing to 84. Isolating the type change from the compound-assignment form
gives the identical 84, so it is the type itself, not the spelling.

So: when a park is blocked on "the right instructions with two registers
exchanged", and the memory it touches is a field of a known structure, try
declaring that structure before concluding the class is unreachable. It costs one
screen. It will not always work -- but "commutative register-role swap" has now
been shown to be at least two different problems wearing the same diff, and the
type-shaped one is cheap to rule in or out.

## Type-screen any park that touches memory: two of three recovered

Following the `OvlFunc_932_200a5c0` recovery, 73 parks were found to access
memory through raw pointer arithmetic. Screening them by declaring a struct and
using field assignments instead has so far recovered TWO of the THREE tried, on
their first screen each:

  * `OvlFunc_932_200a5c0` -- 2 differing, filed as a commutative register-role
    swap. `p->flags |= 2` instead of `*p = 2 | *p`.
  * `Func_80167ac` -- 14 differing AND TWO INSTRUCTIONS SHORT, filed as "formed
    pointer vs register-offset store". Typing both sides and writing three plain
    field assignments matched it. The offsets (0xea8..0xeae) are far past any
    store displacement, so a struct makes gcc form an address per store, which is
    exactly what the ROM does and what no arrangement of the arithmetic produced.
  * `OvlFunc_931_2008d08` -- unchanged at 7. Typing is not a universal key.

The point for selection: **a park describing an addressing-mode or
register-role difference is a candidate for typing even when the note calls it
scheduling or allocation.** Those notes were written from the diff, and the diff
cannot distinguish "gcc allocated differently" from "gcc was told the wrong
type". One screen separates them.

Where the field is already named in `include/actor.h`, use that name; where it is
not, a local struct with `unk_XX` holes is what the other matched files in this
tree do, and only the offsets the function actually touches need to be right.

## Splitting a .s that holds ONE function plus its data

`tools/split_s.py` refuses a file with a single function and trailing data, and
its refusal is correct: there is no second function to split at, so converting
the whole file would delete the data and the link would fail on undefined
references. It says so explicitly and names the blob and label counts.

When the boundary is clean -- everything through `.func_end`, then
`.section .data` -- the manual split is two files rather than three:

    _b.s   the preamble and the function, no data
    _c.s   the `.include` line and the data section, no functions

Both go in BOTH the `.text` and `.data` lists of the overlay script; an empty
contribution costs nothing and keeps the ordering obvious. Then run the
byte-neutral `make compare` WITH THE FUNCTION STILL IN ASSEMBLY before writing
any C, which is what catches a mis-placed boundary while it is still cheap.

`OvlFunc_958_2009394` is the worked example. Note its C references none of the
fifteen data blobs -- they belong to the overlay, not the function -- so the
split is genuinely clean rather than needing `__asm__` label externs.

## Name the OFFSET, not the base

Reading a global at a fixed offset pulls in two opposing requirements, and there
is a third spelling that satisfies both.

    ROM   ldr r3, =gState / mov r2, #0xe1 / lsl r2, #1 / add r3, r2

  * `g = gState; ... *(short *)(g + (0xe1 << 1))` -- the base becomes a value
    gcc will hoist. If a call sits between the assignment and the use, it lives
    across that call in a callee-saved register and the function gains a push
    the ROM does not have.
  * `*(short *)(gState + (0xe1 << 1))` inline -- gcc folds the whole address into
    one pooled `=gState+450`, which is THREE INSTRUCTIONS SHORTER than the ROM.

Naming the offset gets both halves right:

    off = 0xe1 << 1;
    ... *(short *)(gState + off) ...

The fold cannot happen, because the offset is a separate value; and nothing is
held across the call, because the base is still materialised at its use.

`OvlFunc_932_200a310` is the worked example -- 9 differing with the base named,
93 with it inline, exact with the offset named.

**It does not apply wherever the base appears.** `OvlFunc_955_2009424` was parked
on what looked like the same tension and the lever was retried on it: 111 lines
and 97 differing, against 108 and 78 for its inlined form. The difference is that
the fold has to be COSTING something. On 200a310 the inlined base folds and the
function comes out three instructions short; on 2009424 the inlined base already
gives matching length, so there is no fold to prevent and the extra value only
adds pressure. Check that the inline form is SHORT before reaching for this.

## Check docs/structs.md before inventing a struct

`docs/structs.md` is generated by `tools/structmap.py` and lists every struct
the elevated tree declares: by NAME (how many different layouts share that name,
and the union of every field those layouts pin down), by LAYOUT (the same shape
given two different names), and by FILE.

Read it before writing a new struct. The tree reached 66 names for far fewer
real objects because each function declared its own, and nothing recorded that
they were the same thing. `Actor` alone had 35 layouts across 75 files.

Worse, the same two fields were spelled four different ways -- `f0c`, `fc`,
`f0xc`, `posY` all meant offset 0x0C -- so grepping for one spelling found a
sixth of the users. If you convert a family, enumerate the member accesses
first (`grep -oE '\->[A-Za-z_][A-Za-z0-9_]*'` on each file) rather than
assuming the spelling is uniform. Three separate build failures in one round
came from fixing three spellings and not knowing about the fourth.

When the union of a family's fields matches `include/actor.h`, use that header
instead: `#include "actor.h"` and the real field names. Fourteen files were
converted this way -- `Actor932/935/948/949`, four invented names for one
object -- and the ROM stayed byte-identical. Offsets are what the codegen
depends on, so a wider struct with the same offsets does not disturb a match
as long as the object is used through a pointer.

The map is only as good as its parser, and the parser was wrong four times
before it was right. If a number in it looks implausible, regenerate and check
one file whose answer you already know by hand -- that caught every one of the
four. The last one made 405 expression-sized pads (`pad[0x55 - 0x23 - 1]`)
read as size zero, which put every later field in those structs at a wrong
offset while still looking like a confident table.

## The overlay divide-alias is a whole blocker class, and it is now swept

gcc-2.96 emits `__divsi3` / `__udivsi3` / `__modsi3` / `__umodsi3` for the C
operators and has no flag to rename them. Overlay code calls the RAM-resident
copies -- `_divsi3_RAM` and friends -- through the stub each overlay's
`imports.s` exports. These are different functions at different addresses, so
without an alias in the overlay's linker script a correct C division does not
link to the routine the ROM calls, and the screen shows it as a one- or
two-instruction difference that looks like a codegen problem and is not.

`tools/tryc.py` already recognises the symptom and says so. The point of this
section is that it was worth fixing for the WHOLE tree at once rather than one
function at a time.

26 overlay scripts were missing at least one alias, 35 in total. All are added.
The check that made it safe: an alias is only sound where that overlay's
`src/overlays/<ov>/imports.s` actually exports the matching `*_RAM` symbol --
all 35 did. Aliases emit no bytes, and the ROM was byte-identical afterwards,
which is the proof rather than the claim.

This was found by elevating OvlFunc_970_20080b0, which screened at ONE
differing instruction that turned out to be the missing `__udivsi3`. The park
at src/non_matching/ovl_793768/2009754.c had recorded the same need months
earlier and named two overlays owed one; both are now covered, and that park
drops from 8 differing to 6.

So: if a function that divides screens one or two instructions off, check the
overlay script before touching the C. That check is now expected to pass, and
if it does not, the overlay is new.

A park should carry its candidate C. 2009754.c is comment-only, so the
improvement above could not be verified by re-screening it -- tryc.py runs
clean and silent on a file with no function in it. The C has to be rebuilt
from the .s before that park can be re-attacked at all.

## The signed lower-bound floor is NOT a floor: use a switch

Three park notes and this document all recorded the same "floor": gcc-2.96
canonicalises every signed lower bound to `cmp #(K-1) / ble` where the ROM has
`cmp #K / blt`, and `v < K`, `v <= K-1`, `v >= K` inverted, `!(v >= K)`, an
`int` operand and a `short` one all produce identical output. The conclusion
recorded was "a 2-line floor, not worth another round".

**That conclusion was wrong, and the counterexample was already in the tree.**

A corpus check found 15 sites where ALREADY-MATCHING C emits `cmp #K / blt`.
Fourteen are `cmp #0`, which cannot canonicalise because `-1` is not an
encodable immediate -- those explain nothing. The fifteenth is `cmp #31 / blt`
in `src/rom_b5000/rom_bb588_c_c_b.c`, and it comes from a **switch statement**.
gcc lowers a switch through a different path that emits the bound test
directly and never canonicalises it.

So when the ROM shows `cmp #K / blt` for K != 0 and the test is a RANGE, write
the range as a switch with one case label per value:

    switch (e) {
    case 9: case 0xa: case 0xb: case 0xc:
    case 0xd: case 0xe: case 0xf: case 0x11:
        r = table_a;
        break;
    default:
        r = table_b;
        break;
    }

Three functions matched on the first screen this way, all three previously
parked or blocked on exactly this floor: `OvlFunc_937_200807c`,
`OvlFunc_937_20080e4` (whose park called it "the cleanest example so far
because every other difference is gone") and `OvlFunc_899_2008048`.
`Func_80a3ce4`, the function this document used to STATE the floor, also
matches as a switch; it is not elevated only because its `.s` holds four
functions and the other three are still assembly.

**Scope, measured rather than assumed.** 242 `cmp #K / blt` sites sit in 158
unelevated functions. That is the population where this can apply -- not a
promise that all of them are range tests, which is the precondition. Check that
the values are contiguous or few enough to enumerate before reaching for it.

**The general lesson, which is the more useful one.** "Every spelling I tried
gives the same output" is evidence about the spellings tried, not about the
compiler. All the spellings tried were *expressions*; the lever was a different
STATEMENT form. When a class looks like a floor, ask what construct has not
been tried, and check whether the matching corpus already contains the
instruction that is supposedly unreachable. That check is cheap and it is what
broke this one.

## An early `return 0` guard is not the same shape as a tail `return 0`

`OvlFunc_968_2008098` screened at 26 of 36 written the obvious way:

    n = CreateActor(...);
    if (n == 0)
        return 0;
    ...work...
    return n;

and matched EXACTLY when the same logic was written with the zero in the tail:

    n = CreateActor(...);
    if (n != 0) {
        ...work...
        return n;
    }
    return 0;

Two separate differences collapsed at once, which is why this is worth a
section rather than a line.

  * gcc materialises the early `return 0` BEFORE the comparison -- `mov r0, #0
    / cmp r5, #0 / beq` against the ROM's `cmp r5, #0 / beq`. The constant for
    a path not yet taken is hoisted above the test that selects it.
  * The redundant `mov r0, r5` before the inner call disappeared too. That one
    looked like the "elided copy" register-pressure shape, which HANDOFF.md
    describes as a consequence of pressure rather than of source form -- and
    here it was a consequence of source form after all.

The second point is the general one. A difference that matches a known
pressure-residue shape is not automatically pressure residue. Check the
control-flow shape first: it is cheap, and it moves register allocation. The
ROM's own layout is the hint -- a single exit with the zero assigned in an else
arm and one `b` to a shared epilogue means the source had one exit, not two.

## `docker run` without `-i` silently runs an empty program

    docker run --rm -v "$PWD:/work" -w /work goldensun-build python3 - <<'PY'
    ...analysis...
    PY

This produces NO OUTPUT and EXITS 0. Docker does not attach stdin unless `-i`
is passed, so `python3 -` reads an empty program and succeeds at doing nothing.
It does not look like a failure; it looks like an analysis that found zero
results, which is exactly the shape of a real answer.

This has cost real conclusions. A sweep for structs sharing a layout "found
none" and was nearly written up that way; a park-integrity count came back
empty the same way. Both had simply never run.

**Always write the script to a file first:**

    cat > scratch/analysis.py <<'PY'
    ...
    PY
    docker run --rm -v "$PWD:/work" -w /work goldensun-build python3 scratch/analysis.py

The general rule this is an instance of: a measurement that returns "nothing
found" needs the same scepticism as one returning a suspiciously round number.
Print a total alongside the hits -- `0 of 322` is obviously different from a
script that never executed, and a bare empty list is not.

## gcc DOES emit data mid-function -- for jump tables, never for pools

`tools/poolblocked.py` counts a function as unreachable when it branches over
its own literal pool, and reports 312 of 2212. A wider test looks tempting --
"any data inside the function body with code after it" -- and gives 562, nearly
double.

**That wider number is wrong, and the check that shows it is cheap.** Of the
3494 functions whose C already MATCHES, 85 have data mid-body. Looking at one:

        .align  2, 0
    .L20:
        .word   .L4
        .word   .L3
        ...

A switch jump table. gcc-2.96 emits those mid-function routinely; what it never
emits mid-function is a *literal pool*. So the two kinds of mid-body data mean
opposite things, and only the narrow test measures a blocker.

The general rule, and this is the third time it has paid: **before believing a
class is unreachable, look for it in the corpus that already matches.** If the
construct appears there, the class is not unreachable and the question becomes
what produces it. That check broke the signed lower-bound floor (a switch), and
here it stopped a measurement from being published at nearly twice its true
size.

`tools/census.py` therefore defers to `poolblocked.py` rather than
reimplementing the test.

## A halfword store wants an INT variable, not a literal

`Func_8016758` matched at 45 of 45 once six literal stores became stores of int
locals. Written the obvious way:

    *(unsigned short *)(p + 4) = 0;      ->  ldr r3, =0x0      (a POOL LOAD)
    *(unsigned short *)(p + 4) = z;      ->  mov r3, #0x0      (the ROM's form)

gcc-2.96 materialises a constant destined for a HImode store through the pool,
even when the value is zero and `mov` would do. Assigning it to an `int` local
first and storing the local gives the `mov`.

This is the HImode-literal rule already in this document, read in the other
direction. The existing entry records that a compound `*f |= 2;` REPRODUCES the
ROM's `ldr r3, =0x2`, which is the case where the pool load is what you want.
The same mechanism means that when the ROM has `mov`, a literal in the source
is wrong and an int intermediate is the fix.

Both directions are worth checking whenever a halfword store is involved. In
this function the tell was three `ldr r3, =0x0` against three `mov r3, #0`,
which reads like noise until the rule is applied.

### Why it pools: an ALTERNATIVE-ORDERING artefact, not a value-range one

READ from gcc-2.96, so the rule no longer has to be taken on faith.
`*thumb_movhi_insn` (`config/arm/arm.md:4318`) lists alternative 1, `"l" <- "mn"`,
**before** alternative 5, `"l" <- "I"`. The `n` constraint accepts any
`CONST_INT`, so reload takes alternative 1 at zero reload cost and the constant
goes to the minipool — **even for values `I` would happily encode.**
`CONST_OK_FOR_THUMB_LETTER` (`arm.h:1096`) is `0..255`; `0x63` qualifies, and
still pools.

This matters because it says the rule has **no exceptions to hunt for**. There
is no small-value escape hatch, no threshold below which the literal survives.
Every HImode literal pools, and the `int` intermediate is the only lever.

### A pooled halfword constant FAKES the jump-over-pool false negative

`*thumb_movhi_insn`'s `pool_range` is only **64**, so an `ldrh rN, .LCn` drags
the whole minipool up before the epilogue and gcc emits a real `b .L` over it.

MEASURED on `Func_8015f30`: an intermediate candidate's residue read as
`ldr r3, =0x63` **plus** a bare `b L0 / L0:` pair with nothing between the
labels — which is exactly the shape of the recorded screen false negative. It
was not one. The branch was a *consequence* of the pooled constant, and fixing
the constant deleted it.

**Before filing a trailing `b Lx / Lx:` as a false negative, grep the generated
`.s` for `ldrh rN, .L`.** If one is there, the branch is real and the constant
is the bug.

### The `int` intermediate often needs a SECOND name: the destination pointer

The bare intermediate buys the `mov` and then costs it back. MEASURED on
`Func_8015f30`: `int v = 0x63; *(unsigned short *)(p + 0x12b6) = v;` gets the
`mov`, but the extra pseudo makes sched2 hoist the address pool load above the
preceding `strb` and rotates r3 through the block — **7 to 10 differing across
five placements** (at declaration, after the call, before the preceding store,
adjacent, block-scoped; plus one shared `v` for all three stores, worse still).

Naming the lvalue's pointer as well, **in ROM order — pointer statement first,
then the value** — pins the address computation to its own statement:

    q = (unsigned short *)(p + 0x12b6);
    v = 0x63;
    *q = v;

and the block returns to ROM order. 0 differing.

**Two remedies, and the ROM tells you which.** The recorded remedy from
`src/overlays/rom_7c460c/ovl_314_c_a_c_c_b.c` — *"assign at the top so it is
live across the calls"* — **did not transfer**, giving 9 differing here with
`mov r6, #0x63` and a grown prologue. The discriminator:

- value **live across a call** → assign at the top;
- value **materialised into a just-freed register beside its own store** → name
  the pointer and the value adjacently, pointer first.

In `Func_8015f30` the ROM materialises `0x63` into the r3 just freed by
`add r2, r4, r3`, which is the second case.

## A third DMA-helper signature: ONE SLOT SHARED BY TWO TRANSFERS

The recorded pair is `mov r0, sp / str r3, [r0]` for `DMA3_CLEAR`/`DMA3_FILL`,
versus a fill value stored wherever gcc likes for `DMA3_SET`. There is a third,
and it is what a function does when **two transfers share one stack slot**:

    mov  r5, sp
    str  r3, [r5]
    mov  r0, r5

— the address in a **pseudo**, the store through the pseudo, and only then a
copy into r0. That is a named `volatile` slot object plus a plain pointer local,
handed to `DMA3_SET` twice. Two `DMA3_CLEAR`/`DMA3_FILL` expansions instead give
`sub sp, #8` — two slots — and store through the r0 hardreg, so the prologue's
stack adjustment alone distinguishes them.

Two sub-levers, both MEASURED on `Func_8015f30`:

- **The `volatile` belongs on the OBJECT, not the pointer.** Without it gcc folds
  `*slot` back to `[sp]` (2 differing). `vu32 *slot` over a plain object does not
  help (5 differing).
- **`slot = &value;` has a placement.** Before the `bl` it hoists
  `sub sp, #4 / mov r5, sp` into the prologue (6 differing); after it, exact.

The same edit also fixed WHERE `mov r2, #0xf` landed relative to a store,
because naming the constants gives gcc separate values to schedule rather than
one shared pool entry.

## The pool ceiling is REAL, but not for the reason recorded

`tools/poolblocked.py` justified its class with "old_agbcc emits a function's
constant pool at `.func_end` and never in the middle". Two things about that
are wrong, and the corrected version still leaves the class standing.

**First, old_agbcc is not the compiler.** `/opt/gcc296/xgcc` builds essentially
everything; `old_agbcc` builds five m4a and agb_flash objects on the host. The
claim was about the wrong tool.

**Second, gcc296 DOES place literal pools mid-function.** It never emits the
`.pool` directive -- it writes `.word` tables at its own labels and loads them
as `ldr r3, .L15+8` -- which is why a search for `.pool` in generated output
finds nothing and looks like proof. Counting properly: of 3495 matching
functions, 85 have mid-body data with code after it, and 64 of those are
LITERAL CONSTANTS rather than switch jump tables. `OvlFunc_932_200b9c8` is a
matching function with a pool of five words in its middle.

**So why is the class still real?** `OvlFunc_919_200815c` was screened, reached
`OK` at 27 of 27 lines, and FAILED `make compare`. tryc.py normalises every
pool load to `=value`, so it cannot see WHERE the pool sits; the mnemonics
agreed and the bytes did not. gcc places its pool at a different offset than
the ROM, so the PC-relative `ldr` encodings differ even when every instruction
matches.

The blocker is therefore pool PLACEMENT, not pool capability, and the screen is
blind to exactly that. tryc.py already prints a warning for these functions:

    !! the reference keeps its literal pool INSIDE the function ...
       VERIFY WITH make compare -- this screen cannot see PC-relative offsets.

**Treat `OK` on a pool-bearing function as unverified until `make compare`
passes.** It is the one case in this workflow where the screen and the build
can disagree, and the build is right.

## The pool blocker is ENTRY ORDER, not placement -- and that may be reachable

Following the correction above, `OvlFunc_919_200815c` was compiled and its
output compared against the ROM word by word. The result narrows the class
sharply.

**Placement is not the problem.** gcc dumped the pool mid-function, before the
epilogue, with a `b` over it -- exactly the ROM's shape. The claim that gcc
only dumps at `.func_end` is wrong for this function.

**Contents are not the problem either.** Both pools hold the same eight
constants: 0x3f42, 0xc04, 0x3f3f, REG_BLDCNT, iwram_3001ecc, 0x534, 0x536,
0x52a.

**The ORDER differs, in one position:**

    rom    3f42  c04  4000050  3001ecc  534  3f3f  536  52a
    ours   3f42  c04  3f3f     4000050  3001ecc  534  536  52a

0x3f3f is sixth in the ROM and THIRD in ours. Everything else agrees. gcc
groups the constants destined for HImode stores (0x3f42, 0xc04, 0x3f3f) ahead
of the addresses and offsets; the ROM interleaves them in reference order.

That is the same HImode-literal mechanism recorded elsewhere in this document,
showing up a third way -- it decides not only whether a constant is pooled but
WHERE IN THE POOL it lands.

**Why this matters for the corpus.** 518 functions are classified
branch-over-pool and written off as a toolchain ceiling. If the real blocker is
pool entry order, and entry order follows from the order the source references
its constants, then some unknown share of those 518 is reachable by ordinary
source-level work rather than by a different compiler. Nobody has tried,
because the class was believed closed.

**CONFIRMED, and the fix is a source-level lever.** An `int` intermediate for
the pooled halfword constant moves it out of the HImode group and into general
reference order:

    *q = 0x3f3f;            ->  3f42 c04 3f3f 4000050 3001ecc 534 536 52a
    v = 0x3f3f; *q = v;     ->  3f42 c04 4000050 3001ecc 534 3f3f 536 52a  <- ROM

The second is byte-identical to the ROM's pool -- placement, order and contents
all agree, verified by assembling both and diffing the objects. This is the
same int-intermediate lever that fixes `mov` versus pool load for small
constants; for a constant too large for `mov` it does not change WHETHER the
value is pooled, only WHERE IT SITS in the pool.

So pool entry order IS reachable from the source. `OvlFunc_919_200815c` still
does not match, but its remaining differences are ordinary ones -- the ROM
dereferences the state pointer before loading the offset and holds it in r2
where gcc uses r3. The pool, which was the reason the whole class was written
off, is solved.

**What this means for the 518.** They are not blocked by pool placement. They
carry the same ordinary blockers as everything else, with a pool question on
top that now has a known answer. The class needs re-screening, not retiring.

The screen cannot see any of this -- tryc.py normalises pool loads to `=value`,
so a function can read 27 of 27 and still fail `make compare`. Any work on this
class has to be gated on the build, not the screen.

## Two pointers over one array: give the source the values the ROM carries

`Func_80a1bdc` walks 32 nodes, testing each and passing the live ones to a
helper. Written with ONE pointer it comes out 35 lines against the ROM's 39 --
four short -- and 37 differing.

The ROM carries TWO pointers over the same array: `r7` reads with post-increment
(`ldmia r7!, {r3}`) while `r5` holds the same address for the call argument and
is advanced separately at the bottom of the loop. They are always equal, so one
pointer is the obvious C and it is wrong.

    char *p;  void **q;
    p = base; i = 0; q = (void **)p;
    do {
        if (*q++ != 0)
            helper(p, i, x, y, cols);
        i++;  p += 4;
    } while (i <= 0x1f);

exact at 39 of 39. Adding the second pointer raised the live count from five to
six, which is what makes gcc reach for r8-r10 and restores the missing
prologue -- the same reading as `OvlFunc_957_2008f10` and `Func_801a910`: a
stream N lines SHORT means the source is not carrying enough values.

The last two lines came from assignment ORDER -- `i = 0;` before `q = p;`. That
is now three functions in a row where the final two differences were the order
of two initialisations, and it is worth trying both ways as a matter of course
before concluding anything about a two-line gap.

## Indirect calls: `_call_via_r3` is a solved shape, not a barrier

149 remaining `.s` files call through `_call_via_r3` and it reads like a
compiler-support routine you cannot write in C. It is not: 81 files whose C
already matches emit it, and the source form is an ordinary function-pointer
local.

    int (*fp)(int);
    fp = Func_8000948;
    return fp(new_var);

-- src/overlays/rom_7b4558/ovl_30_a_a_b.c. A pointer loaded from memory works
the same way:

    void (*fp)(void *, char *);
    fp = *(void (**)(void *, char *))(gPtrs + 0xc4);
    fp(arg, rec);

The local matters. Calling through the expression directly is a different
shape; the ROM's `ldr r3, [...] / bl _call_via_r3` is what you get from a
pointer that has been assigned to a variable first.

Worth knowing because the shape looks exotic and had been steered around. It is
in the same category as the overlay divide alias -- something that reads as a
toolchain feature and is really just a C idiom the corpus already contains.

## The argument interleave: settled by probe, and a recorded lead was false

`src/non_matching/ovl_78c76c/20095d4.c` recorded that gcc DOES emit the ROM's
interleave, "probe q8 ... emitted exactly that pattern from
f3(0xe, 0x102, 0x204)". That was the best lead in the class and it is WRONG.
Compiling exactly that call gives gcc's usual form:

    rom     mov r1,#K / mov r2,#K / mov r0,#c / lsl r1 / lsl r2 / mov r3,#0
    gcc     mov r1,#K / mov r2,#K / lsl r1 / lsl r2 / mov r0,#c / mov r3,#0

The ROM puts a cheap constant BETWEEN the two builds; gcc always finishes both
builds first and emits the cheap movs after.

**Eleven source forms were probed and every one is identical:**

    f3(0xe, 0x102, 0x204)                 the recorded lead
    f3(2, 0xd0<<16, 0xe0<<15)             the failing call, three args
    f4(2, 0xd0<<16, 0xe0<<15, 0)          the failing call as written
    f4(0xe, 0x102, 0x204, 0)              lead constants, four args
    f3(0xe, 0xd0<<16, 0xe0<<15)           lead's cheap value
    g4(...) with NO PROTOTYPE
    u4(...) with unsigned parameters
    the two shifted values in named locals
    the cheap value in a named local
    f5(...) with a fifth argument on the stack
    the cheap constants swapped between r0 and r3

This is a much stronger basis than the usual "eight spellings on one function":
it varies argument count, prototype presence, signedness, locals, and stack
arguments, and the output does not move. For this shape -- two expensive
argument values and one or more cheap constants -- **the ROM's order is not
reachable from C with this compiler.**

Two consequences. The class is a genuine compiler difference rather than a
lever nobody has found, and `tools/census.py`'s `open` count is optimistic,
since its filter passes calls whose cheap constant is last and those still miss
(see the twins in that park).

The method is the one batch 135 used to kill the cluster hypothesis: when the
question is about compiler behaviour rather than a particular function, build
the smallest input that would show it. Eleven probes in one file cost one
compile.

## The branch-over-pool class is not a ceiling: first elevation from it

`OvlFunc_881_200b8fc` is the first function elevated out of the 521-function
branch-over-pool class, and it needed no special handling at all -- plain
literals, screened, split, and verified with `make compare` rather than the
screen.

That matters because the class was written off on a rationale that has now
failed twice: first the claim that gcc cannot emit a mid-function pool (it
emits 64 of them in already-matching code), and now the assumption that the
remaining functions are therefore unreachable.

**What the class actually contains, measured.** Of 521 pool-blocked functions,
198 also match the precompute shape, 43 also match const-remat, and 3 are ARM.
That leaves **277 where the pool is the only recorded blocker** -- and those are
the ones worth screening. They are mostly LARGE: 176 are over 120 instructions,
64 are 61-120, and only 7 are 35 or under. So the reachable part of this class
is real but expensive per function, not a pile of quick wins.

**A refinement to the const.sym tell.** That file says gcc never pools a
constant an eight-bit `mov` could build, so a pooled small value means the
source named a symbol. There is an exception: gcc pools a small constant when
it is an operand of a HALFWORD expression. Here `0xc | *(unsigned short *)p`
gives `.word 12` loaded with `ldrh`, matching the ROM, from a plain literal.
Check for a halfword operand before concluding a symbol is needed.

**The discipline that made this safe:** tryc.py normalises pool loads, so its
`OK` on a pool-bearing function proves nothing about placement. This one was
gated on the build throughout -- split first, `make compare` green on the split
alone, then the C, then `make compare` again.

## Naming a shifted value works against a POOL LOAD, not against a cheap mov

Two functions this round ended on the order of two argument setups, and the
same lever fixed one and not the other. The difference is what the shifted
value is competing with.

`OvlFunc_common1_1490`, 2 of 32:

    rom    lsl r1, #4 / ldr r0, =OvlFunc_common1_1354
    ours   ldr r0, =OvlFunc_common1_1354 / lsl r1, #4

Both operands are EXPENSIVE -- a shift and a pool load. Writing
`n = 0xc8 << 4;` as a statement before the call reorders them and the function
matches exactly.

The interleave parks -- ovl_794ac0/2008428.c, ovl_79c738/2008150.c and others
-- look similar and the same edit does nothing:

    rom    mov r1, #0x80 / mov r0, #0xf / lsl r1, #8
    ours   mov r1, #0x80 / lsl r1, #8   / mov r0, #0xf

There the shifted value competes with a CHEAP `mov`, and that ordering is
decided by calls.c:805 precomputing expensive arguments ahead of the register
loads -- settled by eleven probes and not reachable from C.

So before reaching for the named local, check what the misordered instruction
is. Against another expensive value it is worth one screen; against a cheap
`mov` it is the documented wall and the screen is wasted.

Both pool-class elevations this round also confirm the const.sym exception
above: every small constant the ROM pooled here -- 0xc, 3, 0 -- came out right
from a plain literal, because each meets a halfword.

## An explicit address computation means the OFFSET gets clobbered

When the ROM computes an address into a register where register-offset
addressing would have done, look for a later instruction that destroys the
offset. `OvlFunc_919_2008200` writes 0x100 to `[ptr + 0x1c0]` and makes the
value with `sub r2, #0xc0` on the register holding 0x1c0 -- so the address had
to be materialised first.

    *(int *)(p + off) = off - 0xc0;     ->  str r3, [r1, r2], no address at all

    d = (int *)(p + off);               ->  add r3, r2 / sub r2, #0xc0
    off -= 0xc0;                            / str r2, [r3]      -- the ROM's
    *d = off;

Reusing the offset variable as the value kills it after the subtraction, which
forces gcc to compute the address while the offset is still live. The block
goes from wholly wrong to instruction-for-instruction identical.

The general form: a value derived from an offset by a destructive operation is
a hint about REGISTER LIFETIME, not just arithmetic. Write the destruction, not
the expression.

## A "dead" register can be a TYPE problem, not a pressure problem

`UIDrawText` was read as having a dead callee-saved register: the ROM does
`mov r8, r3` immediately before a call and never reads r8 again, which is the
shape docs and HANDOFF both describe as unreachable prologue bookkeeping.

It was not dead. With `int` parameters the function screened at 17 of 45 and
gcc emitted `asr` where the ROM has `lsr`; changing the two shifted parameters
to `unsigned int` and shifting them in place matched at 45 of 45 -- and gcc
emitted the `mov r8, r3` itself, because with the right types that register
assignment is what its allocator wants.

**So check the signedness tell before diagnosing a dead register.** `lsr`
against `asr` on the same value says the operand is unsigned, and getting the
type wrong shifts the whole allocation, which can leave an instruction looking
like bookkeeping for a value nobody reads.

The genuinely dead ones -- `OvlFunc_945_200b7b4`, where the ROM sets a register
to zero and never touches it -- differ in that the value has no consumer at
all, not merely no consumer in our version.

## Which operand is the POINTER decides the addressing base

`ldrsh r3, [r5, r7]` and `ldrsh r3, [r7, r5]` are the same access with the
roles swapped, and gcc picks by which C operand has pointer type -- not by the
order of the addition, which it normalises away.

`Func_80b6e30` walks a byte offset over a struct pointer. Written the natural
way it comes out reversed from the ROM:

    char *p; int off;        ->  ldrsh r3, [r7, r5]   (pointer is the base)
    *(short *)(p + off)

    int p; char *off;        ->  ldrsh r3, [r5, r7]   the ROM's
    *(short *)(off + p)

Writing `off + p` instead of `p + off` does nothing; declaring the OFFSET as
the pointer and the base as an `int` is what moves it. The result reads
backwards, so it is worth a comment wherever it is used -- but the ROM's
register assignment is the only evidence of which role gcc assigned, and this
is the only lever that reaches it.

## Finish the OFFSET before the base, to get register-offset addressing

The mirror of the offset-clobber lever. That one forces gcc to COMPUTE an
address; this one stops it.

`Func_8020b14` stores through `[base + n*2 + 0xeb0]`. The ROM builds the whole
offset first and then uses register-offset addressing:

    lsl r3, r1, #1 / mov r2, #0xeb / lsl r2, #4 / add r3, r2
    / strh r2, [r4, r3]

Written as one expression, `*(unsigned short *)(p + (n * 2 + 0xeb0)) = 0;`,
gcc folds the base in early -- `add r3, r4` right after the shift -- and ends
up storing through `[r3, #0]` with the address materialised. Computing the
offset into its own variable first:

    off = n * 2;
    off += 0xeb << 4;
    *(unsigned short *)(p + off) = 0;

keeps the base out until the store and matches exactly.

So the pair is: **name the offset and finish it** for register-offset
addressing; **clobber the offset after taking the address** for an explicit
one. Which the ROM wants is visible in the store -- `[rB, rO]` against
`[rA, #0]` -- and it is worth checking before writing the expression.

## A short `.LN` extern can be captured by gcc's own labels

`extern unsigned char L3[] __asm__(".L3");` compiles to `.word .L3` in the
literal pool. gcc also names its own branch targets `.L3`, `.L4`, `.L11` and so
on, and when the numbers overlap the assembler resolves the pool entry to the
LOCAL label. The code then loads the address of a branch target instead of the
data it meant, compiles clean, and is wrong.

`OvlFunc_common1_e10` needs five tables named `.L2`, `.L3`, `.L11`, `.L12`,
`.L13`; gcc generated `.L3`, `.L11` and `.L12` for its own branches in the same
function. Three of five references were captured.

Nine elevated files use short `.LN` externs and match, so this is not general --
it depends on how many labels gcc happens to emit for that function, which the
source cannot predict. **Check the generated assembly whenever the symbol name
is short:**

    grep -oE "^\.L[0-9]+:" out.s       gcc's own labels
    sed -n "/^\.L8:/,/^$/p" out.s      the pool it built

A name in both lists is a captured reference. In `tryc.py` output the symptom
looks like harmless label renumbering -- `rom ldr r1, =L9 / ours ldr r1, =L3` --
so it is easy to read past.

**THE FIX: alias the symbol in the linker script.** The data labels ARE
exported -- `.global .L3` appears in the `.s` that DEFINES them, which is a
different file from the one that references them, so grepping the referencing
file suggests they are local and they are not. Give each an alias gcc cannot
collide with:

    _TBL_L3 = .L3;

and reference `_TBL_L3` from C. Absolute assignments emit no bytes, so the ROM
is unchanged, and `make compare` proves it. `OvlFunc_common1_e10` matched this
way after failing three of its five references to capture.

The alias belongs in every linker script that lists the referencing `.o` --
three, for a file shared by three overlays. Missing one is a link error, not a
silent wrong answer, so the failure mode here is safe.

## Two globals a fixed distance apart: derive the second address, don't declare it

When the ROM reaches two globals by loading one address and subtracting:

    ldr r3, =iwram_3001eec / ldr r2, [r3] / sub r3, #0x6c / ldr r1, [r3]

two separate `extern`s give two pool loads and never reproduce it. Write the
second as an offset from the FIRST symbol's address:

    st   = iwram_3001eec;
    view = *(char **)((char *)&iwram_3001eec - 0x6c);

`Task_SpinCamera` matches on that opening exactly. It is the offset-clobber
lever applied to a symbol address rather than a struct offset -- the address
register is reused for the second load, so it must be modified in place.

The tell is a `sub` or `add` on a register that just held a symbol address.
Worth trying wherever two globals are reached a fixed distance apart, which in
this tree usually means they were one object in the original source.

## Naming an expression: a lever with three outcomes and one stated limit

Giving a subexpression its own local variable is the single most-used lever
here, and it points in three directions. Measured cases:

**It is the whole fix**, when it blocks a reassociation. `GetNumDjinn`'s
`u[(0x8c << 1) + which]` gets reassociated by gcc into `(u + which) + 0x118`,
finishing with a plain immediate-offset load; the ROM computes
`which + 0x118` into a register and does `ldrb r3, [r2, r3]`. Assigning the
index to an `int` first and subscripting that matches exactly. Source operand
order is IRRELEVANT -- `u[which + (0x8c << 1)]` is byte-identical to
`u[(0x8c << 1) + which]`. Only the name moves it.

**THE LIMIT, and without it the lever looks unreliable rather than
conditional:** naming blocks reassociation only when gcc cannot see the
definition. In `SetDjinni` the same trick on `i = k + 0xf8` changed NOTHING,
because `k = entry * 4` is computed locally two lines up and gcc sees straight
through the name. `GetNumDjinn`'s index was an opaque parameter. If the
compiler can constant-fold its way back to the original expression, the name
buys you nothing.

**It is catastrophic**, when it promotes a value gcc was content to
rematerialise. Naming the `0x200` mask in `CheckEquipmentCritBoost` -- the
obvious fix for a prologue ordering problem -- took it from **2 differing to
47**: the name earns the value a callee-saved register of its own, drags r10
into the frame and moves the argument to r8. A loop-invariant that gcc hoists
by itself must stay unnamed.

**It does nothing**, when the value already lives in a register.
`Actor_SetAnimAndSpeed`'s animation index is a parameter; naming it produced
byte-identical output.

## The halfword exception, and the two directions it runs in

`const.sym`'s header records that gcc pools a small constant when it is an
operand of a HImode expression. In practice this is common enough to check on
every function that stores through a `short *`, and it runs BOTH ways.

*The ROM has `mov`, we pool:* name an `int`. `Func_801eea0` hit this twice in
one function. `*(short *)(p + 4) = 0x1e - x` pools the `0x1e` because the
subtraction is a HImode operand -- assigning it to an `int` first gives the
ROM's `mov r3, #0x1e`, and took 13 differing to 7. The remaining 7 were the
literal zero in `*(short *)(p + 6) = 0`, the same trap; naming that closed the
function. `Func_80175c0` is a third instance.

*The ROM pools, we have `mov`:* leave the literal alone. `Func_80b0070` ends
with `ldr r2, .Lb00e0` where `.Lb00e0` is a `.word 0` -- the ROM itself pools
the zero for a halfword store, so there the plain literal is the right source
and naming it would be the error.

## Early return versus else-return is worth four instructions

Only when a return VALUE is involved, and that qualifier was measured.

In `SetDjinni`, written as an early `if (...) return 0;` gcc hoists
`mov r0, #0` above the compare and jumps straight to the epilogue, collapsing
four instructions. Written as `if (cond) { ...body... } else { return 0; }` --
identical semantics -- it emits the ROM's separate `mov r0, #0 / b` block. That
one restructure took 32 differing to 24.

It does NOT help a void function. `Func_808e0b0` and `Func_8096b88` both open
with early `return;` guards; rewriting both as nested `if` blocks changed
neither one by a single instruction. There is no value to hoist, so there is
nothing to move.

## Counter initialisation wants to come first

Three functions where the only remaining difference was an adjacent pair in the
prologue, and moving the loop counter's `= 0` EARLIER in the source fixed or
improved all three. `Func_8012d70` matched outright once `i = 0;` was written
before the offset computation rather than after -- that was the entire
difference between 2 differing and a match. `Func_80b0070` went 17 -> 16 -> 14
differing as `n = 0` and then `i = 0` moved ahead of the output-pointer
computation.

Do not over-apply it: in `Func_80b0070` swapping the two counters relative to
EACH OTHER (`i = 0` before `n = 0`) made it far worse, 14 differing to 42,
because gcc then merged the two zeros into one register.

## Statement order does not transfer between near-identical functions

`Func_8096b88` and `Func_808e0b0` are the same shape on the same field layout
(+0x25 flag, +0x27 count, +0x28 actor array), and both are parked on the same
register-role swap. Swapping the order of the `list` and `n` assignments
**helps one and hurts the other**: 5 differing to 3 on `Func_808e0b0`, 7 to 8
on `Func_8096b88`. Same two statements, same family, opposite sign. Measure it
per function; there is no rule to carry across.

Both also need the count read TWICE in the source -- once in the guard, once to
initialise the counter. Reading it once is one instruction SHORT, because gcc
keeps the loaded byte in the counter register and never emits the ROM's copy.

## Increment and decrement tells

The assembly says which form the source used, and the equivalent-looking
rewrite does not match.

`ldrh / add #1 / strh` followed by a SEPARATE `lsl #16 / asr #16` on the value
that was loaded is a POST-increment, `v = (*b)++`. The shift pair exists only
because the old value is still live after the store; `n = *b; *b = n + 1;`
reuses one `ldrsh` and never emits it. (`Func_809b5dc`.)

`ldrh / sub #1 / strh` followed by the same shift pair is a PRE-decrement whose
new value is used, `n = --(*c)`. (`Func_8099340`, matched on the first screen
because of it.)

## Deriving a nearby global, continued

The section above covers two globals a fixed distance apart. Two more uses, and
one spelling that does NOT work.

`Func_801eea0` reaches the global 4 bytes below `iwram_3001e90`, and
`Func_80974d8` reaches the one 0x4c below `iwram_3001ebc`. Both want

```c
*(unsigned char **)((char *)&iwram_3001e90 - 4)
```

Declaring the two symbols separately cannot produce the ROM's `sub r3, #4` --
gcc has no reason to believe two externs are a fixed distance apart, so it
emits a second pooled address.

**An array with a negative index does not work.** Declaring
`extern unsigned char *sym[] __asm__("iwram_3001e90");` and writing `sym[-1]`
looks like the same thing and is worse: gcc declines to fold the `-1` into the
address and emits a runtime `mov r3, #4 / neg r3, r3` register-offset load.
Take the address and subtract.

## A mask applied to a byte gets NARROWED unless it is named

`p[9] = (p[9] & -13) | 8;` looks like it must produce the ROM's
`mov r2, #0xd / neg r2, r2`. It does not. gcc sees that the result is stored
back into a byte, truncates the mask to its low 8 bits, and emits a single
`mov r2, #0xf3` -- one instruction shorter than the ROM, and every register
after it shifts.

Assigning the mask to its own `int` first stops the narrowing:

```c
int mask = -13;
p[9] = (p[9] & mask) | 8;
```

On `OvlFunc_947_200a1ac` that was 53 lines and 42 differing against 54 and 9.

Two things this is NOT. It is not the value being masked: an `int` intermediate
for the loaded byte (`t = p[9]; p[9] = (t & -13) | 8;`) scores 15 differing,
worse than naming the mask. And it does not extend to the OR constant -- naming
the 8, leaving it a literal, and reordering its assignment against the mask's
all produce byte-identical output.

This is a third distinct case for the naming lever, alongside "blocks a
reassociation" and "promotes a value gcc would rematerialise": here the name
prevents a WIDTH narrowing that the destination type would otherwise justify.

## When both arms end with the same expression, WRITE IT TWICE

The tidy C -- assign the common value to a local in each arm, then use it once
after the join -- costs an instruction, because a named variable spanning the
merge point forces gcc to copy the merged value out of the register the arms
left it in.

`OvlFunc_948_2009edc` ends both arms of a branch by writing the same field of
the same actor. The ROM duplicates the CALL in each arm and shares only the
`add r0, #0x23 / strb` tail. Written as

```c
if (...) { ...; p = __MapActor_GetActor(0xb); }
else     { ...; p = __MapActor_GetActor(0xb); v = 0; }
p[0x23] = v;
```

gcc emits `mov r3, r0 / add r3, #0x23 / strb r3, [r3]` -- one instruction more
than the ROM's `add r0, #0x23 / strb r5, [r0]`. Writing the whole store inside
both arms and letting gcc tail-merge it gives the ROM exactly. 71 lines to 70.

gcc's tail merging is reliable and does not need to be arranged in the source;
see also Func_80974d8, where writing the shared store in both branches or once
through a temporary produces byte-identical output. The rule is only about not
introducing a NAMED variable that outlives the join.

## Stack arguments must be named PER CALL SITE

The existing note says the two stack arguments of a six-argument call want to
be named locals. `OvlFunc_954_20081a8` sharpens it: sharing one pair of locals
across three such calls is much worse than giving each call its own pair.

    one shared pair (s1, s2) for all three calls  -- 20 differing
    a separate pair per call site                 --  2 differing

Sharing makes gcc keep one pair of registers alive across the whole function
and reload them; separate locals let each call materialise its own into two
registers and store both, which is what the ROM does.

## THE SYMBOL TELL, generalised — check the halfword exception FIRST

`const.sym`'s header says a pooled constant that an eight-bit `mov` could build
means the source named a SYMBOL. Two refinements, both measured:

**It holds all the way down to zero.** `OvlFunc_951_2008dd0` emits `ldr r2, =0`
and uses it for two BYTE stores. Byte stores have NO QImode analogue of the
halfword exception -- compiling `p[0x55] = 0; q[0x26] = 0;` emits `mov r2, #0`
and reuses it -- so a pooled zero is a symbol. Substituting one moved that
function's first difference from line 17 to line 29.

**But check for a halfword before reaching for a symbol.** `OvlFunc_898_2008acc`
emits `ldr r3, =0x2`, which looks identical to the tell. It is not: the constant
is ORed with an `unsigned short` lvalue, making it a HImode operand, and gcc
pools it from the plain literal. That check is what kept a spurious `_CONST_2`
out of const.sym. Note the same function ANDs 1 into the same halfword and gets
`mov r3, #1` with no pool -- OR pools, AND narrows.

**Two pooled values and a `sub` means two symbols, or a symbol and a literal.**
`ldr r3, =0x7e / ldr r2, =0x8d2 / sub r2, r3` cannot be two literals, because
gcc folds that at compile time. It is `0x8d2 - (int)&_AREA_7e`. And write it
INLINE at every use: naming the difference hoists its two pool loads above the
block that should precede them (14 differing against 2 on
`OvlFunc_946_20092b4`).

Symbols are referenced as `(int)&_SYMBOL`, declared `extern int _SYMBOL;`.
`_CONST_1` was added and VALIDATED this way -- it closed `Func_809b5dc`, which
had been parked one instruction short.

## Naming a value, resolved: it asks for a REGISTER, and the class matters

This supersedes the three-outcomes framing above; that section's cases are all
instances of this one.

  * **Name stack arguments** -- they get a low register and a store, the ROM's
    shape. But PER CALL SITE: one shared pair across three six-argument calls
    is 20 differing where separate pairs are 2.
  * **Name a stack BUFFER's address** when the ROM holds it in a register.
    `OvlFunc_934_20090e0` is three instructions short without it -- exactly
    `mov r7, r8`, `push {r7}` and the restore -- because gcc otherwise
    addresses the buffer through `sp`. Conditional, though: the same lever does
    nothing for `OvlFunc_946_2008e00`, retried with the assignment moved
    immediately after the call.
  * **Name a constant that costs more than one instruction to rebuild**
    (pooled, or `mov`+`lsl`, or `mov`+`neg`). Removing the named delta from
    `OvlFunc_927_2009078` makes it two instructions SHORT.
  * **Do NOT name zeros.** A named zero is promoted to callee-saved, costing
    push/mov/pop; gcc rebuilds a zero in one instruction and always will.
    `OvlFunc_926_200a5b8` matched only after the local was removed.
  * **Do NOT name a value built from pooled symbols** -- see above, it hoists.
  * **Name a mask that a narrower destination would let gcc truncate**
    (`p[9] = (p[9] & -13) | 8` becomes `mov r2, #0xf3`). The mask, not the
    value being masked.

## Two shapes that are NOT reachable from source

Recorded so rounds are not spent on them.

`cmp rN, #<nonzero>` followed by `bge`. gcc-2.96 canonicalises `x >= 8` to
`x > 7` when inverting a branch. MEASURED: across 3205 generated `.s` files the
sequence appears exactly ONCE, and that instance is switch dispatch, not a
comparison expression.

Scheduler interleaves -- the ROM splitting a two-instruction constant build
around another operation, where we emit the pair together. Three independent
spellings on `OvlFunc_932_20082cc` and two on `OvlFunc_946_2009494` are all
byte-identical to each other.

## Levers found by the parallel screening pass

Measured independently of the sections above, and several sharpen rules already
recorded here.

**A value used TWICE wants `int x = (unsigned short)f(...)`, not `unsigned
short x`.** The existing note says an angle has to be an unsigned short local.
That holds when it is used once. Used twice -- two entities, two comparisons --
gcc re-emits the `lsr #16` at every use and narrows the register; an `int`
holding an explicit cast keeps one zero-extend. 6 differing against a match on
`Func_8092878`.

**Split one variable into two with disjoint live ranges to flip a register-role
swap.** On `Func_80289e8`, `sel = Func_8028574(r)` instead of reusing
`r = Func_8028574(r)` took 17 differing to 2 by flipping the r5/r6 roles.
Reordering statements and reordering declarations were both inert on the same
function (19 and 19). When a role swap resists ordering levers, look for a
variable doing two jobs.

**Give each mutually-exclusive arm its OWN locals.** `OvlFunc_944_20080c0` has
four arithmetic arms; sharing one `n`/`t` pair across them makes gcc pick a
common destination register and lose the destructive `sub r3, r0` form. Naming
`n1..n4`, `t1..t4` took 16 differing to a match. This is the per-call-site
stack-argument rule generalised: shared locals across arms that never run
together still cost.

**Declaration order, not assignment order, flips a pointer pair.** With
everything else right on `OvlFunc_898_2009674`, `bp` and `ap` were swapped;
swapping their DECLARATIONS closed it, and swapping their ASSIGNMENTS made it
much worse (87 lines, 78 differing). Both orderings exist as levers and they
are not interchangeable.

**The statement POSITION of a naming decides the register it takes.** On
`OvlFunc_943_200b1a8` the pooled `0xffff` had to be named AND the assignment had
to sit between two specific statements: in place, a match; one statement
earlier, 6 differing; one statement later or unnamed, 58 differing and one
instruction short, because the ROM re-materialises a symbol address whose
register the constant had taken. Naming is not a boolean.

**Interposing an unrelated `= 0` between two pool loads can fix their order.**
`PrintBattleText` sat at 8 differing with a pooled `0xea5` hoisted above
`ldr r3, =iwram_3001e8c` and a zero in the wrong register. Nine spellings of
naming and reordering the two values were all inert at 8. Moving an unrelated
`v = 0;` so it sat BETWEEN them fixed the load order and the zero's register at
once. This is a counter-example to the older note that pool-load order is out of
reach when there is no basic-block boundary.

**A struct MEMBER and a `unsigned char *` SUBSCRIPT emit a byte STORE's address
computation and value in opposite orders.** `a->height = info->unk_01;` matched
`InitSprite` where `a[0x21] = info->unk_01;` was two instructions out of place,
after statement order, named temporaries and three scheduler flags had all
failed. MEASURED NOT to extend to LOADS: the same substitution on
`Func_808e0b0`'s `o[0x27]` read is byte-identical to the subscript form.

**`-fno-schedule-insns2` separates "wrong order" from "wrong register".** On
`OvlFunc_896_200c260`, disabling sched2 reproduces the ROM's instruction ORDER
exactly and leaves only a temp-register difference, which identified the two
visible symptoms as one cause -- gcc picking r2 rather than r0 for a
`mov high, #imm` staging temp, which removes the anti-dependence that would
otherwise pin the order. Useful as a diagnostic even though the flag is not
usable in the build.

## More from the parallel pass — frames, exits, and where levers stop

**An unused local array reproduces a bare `sub sp, #N`.** gcc-2.96 does NOT
eliminate the frame slot for an unreferenced array: `int v[3];` emits
`sub sp, #12` / `add sp, #12` and nothing else. Probed five ways --
`volatile int v[3]`, a struct-by-value return, a seven-argument call -- all give
the identical bare frame. This is the cheapest way to reproduce a ROM
`sub sp, #N` that has no visible user, and on `Func_80e3994` it fixed a
displaced `mov r8, r1` as a side effect.

**Two textually separate `return -1;` statements ALWAYS merge.** gcc-2.96 folds
them into one shared block; probed three ways, including with distinct
constants. So a ROM that MATERIALISES the same constant at two different exits
is telling you the source used ONE VARIABLE, not two returns -- and that
variable has to be assigned inside the loop that kills it, not in the preheader.
That is what produces the interleaved `mov r0,#1 / neg r0,r0` in
`Func_8003e58`.

**Naming a call's ARGUMENT fixes the `_call_via_rN` register.** The interleave
lever is recorded for constants; it applies to an indirect call's function
pointer too. On `Func_809397c`, writing `s = dx*dx + dz*dz;` as its own
statement before `fp = F;` pushes the pointer load after the argument
computation and selects the ROM's `_call_via_r3`. Inline, the pointer lands in
r2 and you get `bl _call_via_r2`.

**A derived constant needs the first one added to a POINTER FROM MEMORY.**
`off = 0x28b; q0 = b + off; off += 1;` derives 0x28c and 0x28d with `add r2,#1`.
With plain literals gcc emits `mov r2,#0xa3 / lsl #2` each time -- 42 differing
of 60 on `Func_80978c4`.

**The base-first/index-first lever does not reach a BARE GLOBAL ARRAY.** Four
spellings of the same store through a global -- `G[i]`, a named offset,
`*(G + i)`, `*(G + o)` -- are byte-identical to each other. It only works
through a pointer local. Measured on `Func_8003e58`.

**A `goto` loop defeats strength reduction.** Where the ROM recomputes
`start + j` every iteration, a `for` or `do` gets turned into a pointer
induction (`add r3, #1`). Writing the loop with an explicit `goto` keeps the
recomputation. Same function also needs the outer scan as a `goto` loop so the
constants are rebuilt each iteration rather than hoisted.

## CORRECTION: `cmp rN, #K / bge` with K > 0 IS reachable

An earlier section of this file, and batch 143, record this shape as unreachable
from an `if`, on the evidence that it appears exactly once across 3205 generated
`.s` files and that the one instance is switch dispatch. **That inference was
wrong.** A corpus count says what the tree currently contains, not what the
compiler can emit, and every function in the tree was written by someone who had
already concluded the shape was out of reach.

A direct nine-way probe settles it. For `x < 8`:

    if (x < 8) x = 8;                 ->  cmp r0, #7 / bgt     (the rewrite)
    return x < 8 ? 8 : x;             ->  cmp r0, #8 / bge     <-- the ROM's form
    int k = 8; if (x < k) x = k;      ->  cmp r0, #8 / bge     <-- the ROM's form

    early return, short, !(x>=8), x<=7, char, x-8<0   ->  all cmp #7 / bgt

So the `<` to `<= K-1` rewrite is applied to an `if` statement's comparison but
NOT to a conditional expression's, and NOT when the bound arrives through a
named local.

`Func_8093168` was parked on exactly this at 4 of 57 and had been declared a
dead end. Naming its two bounds as locals closed it outright: 4 differing, then
2, then a match. **Whenever a residue is `cmp #K-1 / bgt` against the ROM's
`cmp #K / bge`, name the bound.**

## More measured behaviours from the parallel pass

**`ldrb rD,[rB] / lsl rD,#24 / cmp rD,#0` is a `volatile signed char` read.**
Probed twelve spellings: plain `signed char` and a `signed char` bitfield both
give `mov r3,#0 / ldrsb`; plain `char` is UNSIGNED in this toolchain; `*p << 24`,
`(c<<24)!=0` and `unsigned char` all fold the shift away. Only
`volatile signed char` keeps the `lsl #24` and drops the `asr #24`. 6 differing
to a match on `Func_8097194`.

**The scheduler-interleave wall has a key: the TYPE of the memory operand.**
`OvlFunc_964_2008cd0`'s last residue was two independent chains emitted in the
wrong order. Nine source spellings and four scheduler flags were inert at 2
differing. Declaring the object as a struct and reading the halfword as a named
`unsigned short` FIELD instead of `*(unsigned short *)(e + 6)` fixed it --
even though both spellings emit the identical `ldrh r1,[r6,#6]`. The alias set,
not the instruction, is what sched2 sees.

**A pool constant rebuilt inside a loop has a SECOND cause.** It is recorded
here as the selection signature for the `goto`-loop lever. It is also what
reload does when it rematerialises under register pressure instead of spilling.
`Func_8021a18` has two such constants and matched with plain nested `for` loops;
the `goto` rewrite made it far worse (71 differing against 4) by killing the
strength reduction that produces the ROM's induction variables. Count the live
values before reaching for `goto`.

**The `goto` lever's other job is defeating `check_dbra_loop`.**
`Func_80197c4` had no hoisted invariants at all, yet gcc reversed all three of
its counters into countdowns. A ROM loop counting UP with `add / cmp #N / bne`,
where the counter has no use but the exit test, is itself a goto signature --
and a cheaper one to spot than a rebuilt constant.

**A stack scratch area wants a `struct`, not a `char[]`.** With an array gcc
re-addresses each field from `sp` (`mov r3,sp / add r3,#14`) even when it has
just materialised the buffer address in r0 for a DMA; with a struct field it
uses `[r0, #imm]`. Two instructions per field on `Func_8005b64`.

**"Finish the offset before the base" needs one named variable PER OFFSET.**
Reusing a single `off` across two consecutive register-offset stores costs a
`mov` copy on the second.

**Nested `if`s with the `return 1` in the tail, not early returns.** Where two
guards both return the same value, early returns let gcc cross-jump them into a
block placed right after the guard; the ROM puts `mov r0, #1` at the end.
66 differing to 4 on `OvlFunc_964_2008cd0`.

## A constant REBUILT across calls: what it does and does not mean

**A first version of this section claimed it means the source used two distinct
symbols. That was wrong, and the control that catches it is cheap: 76
already-matching functions in this tree rebuild the same `mov`+`lsl` constant
twice across a call, so the shape is plainly reachable from ordinary C.**

What is true, and probed directly. Three functions each calling `f(x)`, `g()`,
`f(x)` in a straight line:

    same literal twice     ->  push {r5,lr} / ldr r5 ... mov r0,r5   (CSEd)
    the SAME symbol twice  ->  identical -- CSEd into r5
    two DIFFERENT symbols  ->  push {lr} / ldr r0,.L6 ... ldr r0,.L6+4

So in a STRAIGHT-LINE sequence gcc will CSE a repeated constant into a
callee-saved register and pay push/pop for it.

But that is not what the matching functions do. In
`src/overlays/rom_78b2ac/ovl_30_c_c_a_a_a.c` the literal `0x80 << 2` appears
three times with calls between, and gcc rebuilds `mov r0,#128 / lsl r0,r0,#2`
every time -- because the three uses sit in DIFFERENT CONDITIONAL BRANCHES, not
in one straight-line run.

**So when the ROM rebuilds a constant and you CSE it, suspect the CONTROL FLOW
first, not the symbols.** Either the source has a branch you have not
reproduced, or register pressure differs. Reach for a symbol only after the
control flow matches and the rebuild still will not appear -- and remember that
one symbol used twice CSEs exactly like a literal, so the symbol theory requires
TWO distinct names, which is a strong claim about the id space.

`OvlFunc_881_2009c08` and `OvlFunc_882_200bc48` are parked on this, three and
one instructions over. Both are genuinely straight-line in the ROM, which is
what makes them puzzling rather than solved.

## Identical constants in ONE basic block: a controlled blocker

The section above retracts a claim about constants rebuilt ACROSS basic blocks.
This is the tighter case, and it survives the controls that one failed:

  * Across basic blocks -- routine. 76 matching functions rebuild a constant
    across a call; 31 rebuild one with no call between. In every case checked
    the uses sit in different conditional branches.
  * Within ONE basic block -- never. ZERO of 3235 generated `.s` files build the
    same constant twice inside a single block.
  * Probe: only two DISTINCT symbols avoid the CSE. A repeated literal and a
    repeated single symbol behave identically.

So a ROM that builds the same constant twice with no label or branch between the
two builds -- most visibly, the same value passed as several arguments of one
call -- is not reachable by any spelling of that constant.

It is a small class: three functions in the whole remaining tree
(`OvlFunc_924_20090c0`, `Field_Carry_Target`, `InitWorldMap`). Recorded so the
next person recognises it in one screen rather than twenty.

## Repeated constants: SEPARATE LOCALS PER USE SITE, and when it works

The section above establishes that gcc CSEs a repeated constant inside one
basic block. There is a lever, and it comes from reading matching code rather
than from guessing.

`src/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_b.c` calls `__MapActor_SetSpeed`
three times with the same pair of pooled constants, and the ROM reloads both
from the pool at every call. Its source declares SIX locals:

```c
int s1,t1,s2,t2,s3,t3;
s1 = 0xb333; t1 = 0x5999;
s2 = 0xb333; t2 = 0x5999;
s3 = 0xb333; t3 = 0x5999;
...
__MapActor_SetSpeed(2, s1, t1);
__MapActor_SetSpeed(3, s2, t2);
__MapActor_SetSpeed(1, s3, t3);
```

One pair per call site, all with the same value. gcc keeps them as distinct
pseudos and emits a separate pool load for each. This is the per-call-site
naming rule extended from STACK arguments to REGISTER arguments.

**It closed `OvlFunc_948_20095f0` outright** -- two `SetSpeed` calls, three
instructions over from the CSE, exact with two pairs of locals.

**MEASURED LIMIT.** It does NOT work when the uses are far apart.
`OvlFunc_882_200bc48` repeats a constant across about twenty instructions and a
dozen calls, and `OvlFunc_881_2009c08` repeats two constants across a similar
span; separate locals change neither, whether assigned at the top of the
function or immediately before each use (both byte-identical to the literal
form). The lever appears to work while the two live ranges are short enough
that gcc does not think a callee-saved register is worth it.

Class size, measured both ways: 257 of the 2113 remaining THUMB functions
repeat an ordinary pooled constant inside one basic block, against 4 matching
functions that do it in ordinary C and 13 more that do it only inside DMA
inline asm. So the shape is reachable but rare, and this lever is the only
known route to it.

## The method that has been closing these: read code that ALREADY does it

Three residues in recent rounds were declared unreachable on the strength of
reasoning about what gcc must do, and all three were wrong. The correction each
time came from the same move, and it is now a tool:

    python3 tools/whodoesthis.py 'add\tr3, r3, #39'
    python3 tools/whodoesthis.py --multiline '<pattern spanning lines>'

It searches the GENERATED assembly of every already-matching function and
prints the C that produced each hit. A count over the remaining hand-written
assembly tells you what the original authors wrote; a count over generated
assembly tells you what this compiler will actually emit from C, which is the
only question that matters when stuck.

It closed `Func_808e0b0` -- parked four rounds on an address-temp register
choice -- in one pass: searching for "address computed into a register and
loaded back into the same register" found 18 matching functions, two at offset
39, and reading one gave both halves of the fix.

TWO CAUTIONS, both learned the hard way.

**It reports the shape, not the reason.** The C it points at may differ from
yours in several ways with only one that matters. On `Func_808e0b0` the matching
function's loop GUARD was the entire fix and copying its loop BODY scored 29
differing.

**A count of zero is weak evidence.** Generated assembly samples what people
have managed to write, not what the compiler allows. The `mov`+`lsl` form of one
search returned zero and was written up as a hard blocker; the pooled form of
the same search returned seventeen, and reading one of those produced a working
lever.

## The argument interleave is reachable -- but it depends on register pressure

Recorded across several batches as a scheduler wall on the strength of
byte-identical spellings. `whodoesthis.py` shows it is not: 27 matching
functions emit `mov rA,#i / mov rB,#i / mov rC,#i / lsl rA / lsl rB / bl`, and
16 of those push only `lr`, so a callee-saved frame is not what buys it.

The spelling in all of them is a NAMED LOCAL per shifted value, assigned near
the top of the function and used much later, with calls in between -- and gcc
REMATERIALISES the `mov`+`lsl` at the use point rather than keeping it live.
That rematerialisation is what produces the interleave.

**The condition is register pressure, not the spelling.** Those functions carry
six or more named locals competing for registers. Applied to a small function
with almost nothing else live -- `OvlFunc_944_2008468`, `OvlFunc_927_2009818` --
the same named local simply WINS a register: `push {r5}` appears and the
function comes out two or three instructions over. Naming two or three locals
instead of one does not tip it either.

So the lever is real and the two functions above are still parked, which is the
honest state: the shape is reachable, and not from their shape.

## A zero survives in a callee-saved register only inside a LOOP

The rule "gcc will not spend a callee-saved register on a value it can rebuild
in one instruction" is recorded several times here, and it has an exception
worth knowing, because four parked functions sit on it.

`whodoesthis.py` finds 21 matching functions that DO put a zero in a high
callee-saved register (`mov r3, #0 / mov r8, r3`), so it is plainly reachable.
Reading one -- `Func_80ad658` in src/rom_a1000/rom_ad274_c_b.c -- the source is
a plain literal `0`, and what buys the register is that the store happens
INSIDE A LOOP:

```c
for (i = 0x89; i <= 0x8c; i++) {
    if (*(unsigned int *)(base + (i << 2)) != 0) {
        _DeleteSprite(...);
        *(unsigned int *)(base + (i << 2)) = 0;   /* every iteration */
    }
}
```

Several dynamic uses beat one rebuild, so the allocator keeps it.

**Outside a loop it does not.** `OvlFunc_895_20087d0` stores its zero twice, at
adjacent fields, at the very end of a long straight-line body -- and gcc rebuilds
it, costing six instructions of high-register save and restore against the ROM.
`OvlFunc_948_2009ac8` and `Func_80b0070` are the same shape.

So do not reach for a named local to force this: naming is what fails. The
condition is dynamic use count, and no spelling changes it for a value used
twice on one path.

## SMALL functions are hard for a specific reason: no register pressure

A run of eleven rounds on functions of 8 to 20 instructions produced no
matches, and the parks all say the same thing. It is worth stating as a class
rather than as eleven separate observations.

Most of the levers in this file work by changing what the register allocator
does. Naming a value, splitting a live range, reordering statements -- all of
them only bite when there is competition for registers. In a function with a
two-instruction body and nothing else live, gcc's choices are forced, and no
spelling moves them:

  * `OvlFunc_959_2008c78`, 3 of 10. The argument interleave. Naming the two
    shifted arguments, the slot, and all three are byte-identical. The 27
    matching functions that DO emit this interleave all have six or more live
    values; here there are none, so a named local always wins a register
    instead of being rematerialised.
  * `Func_80a3ce4`, 1 short. gcc folds a two-sided range test into an unsigned
    `sub`/`cmp`; three spellings do not prevent it.
  * `SetTextColor`, 5 of 8. Both operands of an AND are dead afterwards, so the
    allocator is free to pick either as the destination and picks the other one.
  * `Func_8019d0c`, 2 short. Register-offset stores where the ROM computes
    explicit addresses -- and the explicit-pointer lever that fixed exactly this
    in `SetTextColor` does nothing here.

**Practical consequence: prefer 40-to-120 instruction functions.** They have
enough live values for the documented levers to have something to act on. The
smallest remaining functions are not the easiest; they are close to the hardest.

## Duplicate groups: 74 of the remaining functions come free

See `tools/dupfuncs.py`. 101 of the 2110 remaining THUMB functions are
byte-identical to another one after normalising symbol names, `.L` labels and
pool operands -- 27 groups, so 74 functions follow from 27 solutions.

It is very top-heavy. Three groups carry FIFTY functions:

    x18  OvlFunc_883_20080c4   parked at 7 differing of 176
    x17  OvlFunc_883_200834c   parked
    x15  OvlFunc_883_20088c0   parked at 101 differing, length exact

Those three are the highest-value targets in the tree and their parks carry
mapped residues. Members of a group differ only in which data tables they name,
so one C file with the labels as parameters serves the whole group.

## Two search tools

`tools/whodoesthis.py <pattern>` -- searches the GENERATED assembly of every
already-matching function and prints the C that produced each hit. Use it the
moment a residue looks like a wall. It corrected four "unreachable" conclusions
in six rounds and closed three functions. Its docstring carries the two
cautions: it reports the shape and not the reason, and a count of zero is weak
evidence.

`tools/dupfuncs.py` -- lists the duplicate groups above.

## When gcc HOISTS a repeated constant, exactly: dominance

Probed directly, five variants, and the rule is clean:

    both uses in mutually exclusive branches   -> REBUILDS, push {lr}
    three uses, all in branches                -> REBUILDS, push {lr}
    one use dominating + one in a branch       -> HOISTS to the top, push {r5,lr}
    the same with eight calls in between       -> HOISTS
    the same with the second use deep in an arm-> HOISTS

So it is **dominance, not distance and not the number of intervening calls**.
If every use sits in a branch that no other use dominates, gcc rebuilds the
`mov`+`lsl` at each one and pays no callee-saved register. If one use dominates
another, gcc computes it once at the top of the function -- ahead of even the
first call -- and holds it, costing a push and a pop.

This is partial redundancy elimination, a different pass from the cse described
above, and it explains why the per-use-site naming lever does nothing against
it: gcc folds the initialisers to one rtx before the pass runs, so five named
locals and one literal are the same input.

**A CASE THAT CONTRADICTS THE RULE, recorded because it is unresolved.**
`OvlFunc_952_200be40`'s ROM rebuilds `0xa0 << 7` at a dominating use and a
branch use both, which by the probe above it should not. Under our flags gcc
hoists. The likeliest reading is that the two constants are DIFFERENT SYMBOLS in
the original source and only coincide in value -- the same explanation the
repeated-flag-id case wanted -- but that is inference from a contradiction, not
a measurement, and this tree has no symbol space to write it with.

## A SELECTION FILTER that works, built from the blocker conditions

Thirteen rounds of near-misses produced one useful thing: enough measured
conditions to pick targets that avoid them. Encoding all of them as a filter
found a match on the first try.

Reject a candidate if ANY of these hold:

  * **Under 40 instructions.** Every lever here works through the register
    allocator, and a tiny function has no pressure for it to act on. See the
    small-functions section above -- four parks, none reachable.
  * **Over 120 instructions.** Too many independent residues to converge on.
  * **Uses r8-r11.** Allocation-priority residues, the wall that holds
    OvlFunc_883_200d64c, OvlFunc_901_2008350 and OvlFunc_949_200807c.
  * **Repeats an expensive constant** (a pooled value, or a `mov`+`lsl` pair)
    anywhere in the body. That is CSE if the uses are close and PRE hoisting if
    one dominates another, and neither yields to any spelling.
  * **Fewer than 8 calls.** Arithmetic-heavy bodies hit instruction selection
    rather than the documented levers.

107 of the remaining functions pass. The first one tried, `GetJupiterDjinni`
(118 instructions, 35 calls), matched at 121 lines after a single documented
lever -- the gState offset needed a local pointer so it is built at runtime
rather than folded into `ldr =gState+500`.

Detector note: count a `mov rN, #imm` followed by `lsl rN, #k` as one constant
even when other instructions sit between them. The first version of this filter
required them adjacent and passed the very function whose PRE hoisting motivated
writing it.

## The split-constant interleave IS reachable — for LIVE locals only

The shape, which the parks have called arg-interleave for a long time: the ROM
starts a two-instruction constant build, does one instruction of unrelated
work, then finishes the build.

    rom    mov r5, #0x80 / mov r0, #0x0 / lsl r5, #0x1
    gcc    mov r5, #0x80 / lsl r5, #0x1 / mov r0, #0x0

**The lever.** Split the build across two source statements and put the
independent work between them. gcc keeps the halves where they are written:

    flag = 0x80;        instead of      flag = 0x80 << 1;
    i = 0;                              i = 0;
    flag <<= 1;

This is what landed `Func_80b9a70` (0x080b9a70), which had been sitting at 5
differing with the shape appearing twice, once per arm. Both arms fixed by the
same edit.

**The boundary, measured, and it is sharp.** The lever only works when the
constant is a LIVE LOCAL — a value used after the sequence completes. In
`Func_80b9a70`, `flag` survives into `i | flag`.

It does NOT work when the constant is a call argument that dies at the call.
gcc rematerialises argument temporaries during argument fill and discards
whatever statement structure the source imposed. Tried on
`ovl_7cb2c0/200dca4.c`: residue unchanged, character for character, first
divergence at the same instruction.

That matters because it re-explains a whole park class. Those parks concluded
that the missing ingredient was a basic-block boundary and that straight-line
cutscene scripts were therefore unreachable. The real discriminator is
lifetime, not control flow. Every park in that class is an argument temporary,
which is why they resist — and a branch would not have saved them.

**Before spending a screen, ask: does the constant outlive its construction?**
If no, this lever cannot reach it and neither can the basic-block one.

A known non-generalisation: the same edit on `ovl_79e5c0/200a7ac.c` made it
worse (7 → 17), where the gap work was a pool load rather than a one
instruction constant. Cheap work in the gap is part of the precondition.

## Corpus scans over `asm/**/*.s` MUST use `\s+`, not a literal space

A scan for the shape above returned **0 sites**, which was a detector bug. The
`.s` files separate mnemonic from operands with a **TAB**:

    mov\tr1, #0xd0          not     mov r1, #0xd0

A regex written as `mov (r\d+), #...` matches nothing in the entire asm tree.
Corrected to `mov\s+(r\d+),\s*#...` the same scan returns **3846 sites across
555 functions**.

This is the second false zero from this cause. Two defences, both cheap:

1. **Validate the detector against a case you have seen with your own eyes**
   before believing any zero. The site that exposed this bug was one being read
   on screen minutes earlier.
2. **`tryc.py` output is not file text.** Both of its columns are rendered
   through a disassembler, so the ROM column shows space-separated operands
   that do not exist in any file. Never lift a pattern out of a tryc diff and
   grep the asm tree for it verbatim.

**Generated output is affected too — verified, not assumed.** gcc 2.96 emits
the same tab separator:

    \tsub\tr0, r0, #8

So scans over the build's generated `.s` files carry the identical hazard. The
first draft of this section claimed they were a different corpus and immune;
that was wrong, and checking took one compile.

Any earlier conclusion of the form "zero of the N .s files contain X" that was
produced by a hand-written regex is suspect and should be re-run before it is
relied on — over either corpus. `src/non_matching/ovl_7c460c/2008c74.c` rests
on such a zero and is flagged there.

## The selection filter was rejecting a class that WORKS: loops with no calls

`tools/pickable.py` rejects any function with fewer than 5 calls, on the
grounds that arithmetic bodies hit instruction selection rather than the
documented levers. That rule is too strong, and it was hiding the best
remaining candidates.

The rule exists because of the small-function class above — 8-to-20
instruction bodies with no register pressure. But the SIZE rule (prefer
40-120) already excludes those. The call-count rule was additionally excluding
**51 loop-carrying functions in the 40-120 band**, and that is precisely where
the levers in this file work best:

**A loop body is full of LIVE locals; a cutscene script is full of ARGUMENT
TEMPORARIES.** The distinction measured over the last three rounds is exactly
the one that decides whether the statement-split lever bites. Call-heavy
straight-line functions are the ones blocked by argument fill order; loop
bodies are the ones where naming, ordering and birth order still work.

Evidence: `UpdateRespawnMap` (51 instructions, ZERO calls) matched on the
FIRST screen. `Func_80b9a70` (52 instructions, zero calls) matched a round
earlier. Both would have been rejected by the call-count rule.

The scan that finds them: 40-120 instructions, contains a branch to an
earlier label (a loop), 4 or fewer calls, no r8-r11.

## Change ONE thing at a time, or you will discard three correct fixes

On `Func_8029274` four edits were derived from one diff and applied together.
The result went from 12 differing to **25**, which reads as "all four were
wrong". Applied one at a time from the same baseline:

    name the mask local, born before the buffer pointer     12 -> 10
    invert the digit test so the ROM's fallthrough is taken 12 -> 10
    rewrite the copy-back loop in int arithmetic            12 -> 26   <- the one
    (the fourth was folded into the third)

Three of the four were correct and one was catastrophic; bundling them hid
that completely. The combination of the three good ones reached 6 of 47.

**The corollary is a real lever, not just process.** The bad edit converted a
pointer loop to integer arithmetic in order to obtain the ROM's SIGNED compare
(`bge` where a pointer compare gives unsigned `bcs`). Casting only the
COMPARISON — `while ((int)p >= (int)buf)` — buys the same `bge` and leaves the
loop body as pointer dereferences, which is what the ROM has. When a cast is
needed for a comparison, cast the comparison, not the loop.

## The loop-body class: what separates the matches from the stalls

Six functions from the loop-body class have now been worked. The split is
clean and gives a screening signal to apply BEFORE writing any C.

MATCHED, on first or near-first contact:

    UpdateRespawnMap    51 insns   table search, 3 live values
    Func_801fda8        58 insns   clipped rect fill, 4 live but short-lived
    Func_80907b0        45 insns   two DMA blocks + two counted loops
    Func_80b9a70        52 insns   table search, 3 live values

STALLED, all on the SAME thing -- a whole-function register rotation:

    Func_80f4100        39 of 54   allocation rotated, length exact
    Func_8029274         6 of 47   copy-back pointer in the wrong register
    Func_80c0228        36 of 55   v/tile/base/counter all rotated
    DecodeMetatileset   41 of 78   count/src/dst rotated, switch chain exact

**The signal: count the values live ACROSS the whole body.** At three or so,
gcc's allocation is forced and matches. At four or more, the ROM and gcc
disagree about which registers to spend -- typically the ROM spends one more
callee-saved register than gcc needs, and since these functions have no calls,
gcc is right and cannot be argued out of it by any spelling.

This is not a reason to avoid the class; four of eight matched, which is a far
better rate than the call-heavy candidates. It is a reason to read the ROM's
register usage first and stop early when the rotation is pervasive, rather
than spending six screens rediscovering it.

## A COPY-then-modify in the ROM means two named values, not one expression

On Func_80c0228 the ROM had

    mov r2, r3 / add r2, #0xd        (copy, then add)

where ours had `add r3, #0xd` in place. The source was `row = v / 8 + 0xd`, one
expression. Splitting it into two named locals --

    r = v / 8;
    row = r + 0xd;

-- produced the copy and took the function from 40 differing to 36.

Generalise it as a reading rule: **where the ROM copies a register before
modifying it, the original had two live names.** Where it modifies in place,
one. This is cheap to check and it is a genuine lever, though a site-dependent
one -- naming the mask constant on the same function bought nothing, while the
same mask lever moved Func_8029274 by two lines.

## tryc is wrong in BOTH directions on a pool-carrying function

When the reference keeps its literal pool inside the function, tryc prints a
warning and its count becomes unreliable **in both directions**. Two functions
in adjacent batches, same warning, opposite failures:

    Func_801fda8    screened 6 DIFFERING   -> byte-identical
    Func_80d66cc    screened OK, 3 runs    -> build failed the checksum

The first is the double-label artifact: gcc emits two labels at one address and
the streams misalign, putting the divergence at the very END where it reads
like an epilogue bug. The second is pool PLACEMENT: every instruction matched,
but the ROM splits its pool (one constant dumped mid-function) where gcc emits
one block after the epilogue, so the object is a different SIZE and the whole
bank shifts. That showed up as 93,746 differing bytes starting BEFORE the
function, values off by 2 and 4 — a bank-head pointer table recording a moved
layout.

**Rule: an `OK` that carries the pool warning is a match of the INSTRUCTION
STREAM, not a match.** tryc cannot see pool placement and says so. Install it
and let `make compare` decide, and expect either verdict to flip.

**Reading a size shift:** thousands of differing bytes whose values are off by
2 or 4, beginning before the function you touched, is a layout shift, not a
code error. Diff the generated `.s` pool against the reference's before
suspecting the body.

## Register allocation follows ASSIGNMENT position, not declaration order

On `Func_80d66cc` the last 8 differences were a single r0/r1 swap between a
pointer and a loop counter. Measured:

    permuting the DECLARATION order of the two locals      no change
    assigning `i = 0` BEFORE the pointer assignments,
      with an empty `for (; ...)` init                     closed all 8

Declaration order is recorded elsewhere in this file as a lever, and it is one
— for frame layout, and for values born at their declaration. But for a value
whose first use is an assignment, it is the **position of that assignment** in
the statement stream that decides which register it gets. Try moving the
assignment before reordering the declarations.

## Before every elevation commit: `git status --porcelain`

`split_s.py` produces FOUR changes — the original `.s` deleted, two halves
created, and a `stage1.ld` edit. Committing explicit paths (which is correct,
after `scratch/` was swept into history) makes it easy to list two of them and
omit the sibling half.

Five elevations did exactly that. The committed tree could not link; it built
locally only because the untracked files sat on disk. `make compare` cannot
see this, because make reads the working tree.

**A clean BUILD is not a clean CHECKOUT.** `??` under `asm/` or `src/` means a
split is half-committed. This check is the last step before any commit.

## The copy-then-modify tell is ONE-DIRECTIONAL, and it has a boundary

The tell — *where the ROM copies a register before modifying it, the original
had two live names* — closed three functions in recent batches. Two limits,
both measured:

**It does not run in reverse.** An in-place modify in the ROM is NOT evidence
of a single name. On `Func_80c0f98` the ROM masks its parameter in place
(`and r5, r3`); writing `val &= 3; v = val << 2;` to match is byte-identical to
`v = (val & 3) << 2`, because `val` is dead afterwards and gcc folds both to
one rtx. Only the copy direction carries information.

**A copy of an UNCHANGING value is unreachable.** On `Func_80a8b10` the ROM
copies the output base into r12 purely to compare against it
(`mov r12, r5 ... cmp r3, r12`), leaving us exactly one instruction short with
everything else exact. Any local initialised from `out` is provably the same
value, so gcc coalesces it and emits no `mov`. In the three functions where the
tell worked, the two names held *different* values at some point — a count and
a loop counter, a division result and an offset. **A pure duplicate of a live
value is an allocator artifact, not a source construct.**

## `switch` and `if / else if` are not interchangeable

On `Func_80c0f98`, an `if (t == 1) ... else if (t == 2)` chain made gcc fall
through into the first body:

    rom    cmp r2,#1 / beq L1 / cmp r2,#2 / beq L2 / b L0
    ours   cmp r2,#1 / bne L1     (first body inlined as the fallthrough)

A `switch` evaluates both tests up front and branches to separate blocks, which
is what the ROM does. The single edit was worth 20 differences and fixed the
length (50 of 62 -> 30 of 64). This file already warns that a wrong case count
can look like a register problem; the wrong control construct can too.

## Where the offset-inside-the-loop lever stops

Declaring an address's offset inside the loop body moves the preheader below
the guard — it closed `Sprite_DeleteLayer`. It does NOT stop gcc from creating
an induction pointer in the first place. On `Func_80c1f50` the ROM addresses
register-offset off a base with one index variable, and gcc builds a parallel
cursor; four loop shapes, including the offset-inside-the-body form, are
byte-identical on that point.

Different passes: the lever moves where `move_movables`/`strength_reduce`
INSERT a value; it has no purchase on `strength_reduce` deciding to CREATE one.

## r8-r11 IS NOT A WALL — the reject was hiding a quarter of the corpus

`tools/pickable.py` rejected any function touching r8-r11, citing three named
functions as an "allocation-priority wall". Measured directly:

    matching TUs that use a high register:  232
      of those, fakematches:                 30
      GENUINE matches:                      202

High registers are reachable from ordinary C, and by a wide margin. The reject
was generalised from three functions to a class, and it cost:

* **Every unparked duplicate group.** All ten of them use high registers, which
  is why 72 functions that "come free" from 25 group solutions have gone
  untouched. That backlog was invisible to the filter, not hard.
* **The candidate pool.** Removing the reject took the filter from **5
  candidates to 125**.

**What actually puts a value in r8-r11** is having more call-crossing locals
than r5/r6/r7 can hold — nothing more exotic. `OvlFunc_common1_1608` reached 16
of 84 from plain C on its second attempt, with r8 holding a zero used by five
later stores, r9 an argument across a call, and r10 a status byte across four.

The high-register column is still printed, because these functions are harder
on average — but harder is not unreachable, and it is now a ranking signal
rather than an exclusion.

**The general lesson, and this is the third time:** a reject generalised from a
handful of parks quietly deletes a whole class from view. The "fewer than 5
calls" rule hid the loop-body class that produced four elevations; this one hid
the duplicate groups. Before trusting any reject in this file, check whether
MATCHING code already does the thing it forbids — `whodoesthis.py` and a grep
over the generated `.s` answer it in one command.

## Ad-hoc candidate scans MUST reuse `pickable.parked()` — third repeat

A hand-written scan in a scratch script re-offered `Task_SpinCamera`, which was
already parked at exactly the residue it was re-derived to. A whole round's
effort was spent reproducing a park note that already existed — including the
same symbol-address-subtraction lever, already written down.

The cause: park files are named by ADDRESS (`80d6504.c`), and the ad-hoc filter
matched candidates by the trailing component of the SYMBOL NAME. That works for
`Func_80b9a70` and never for `Task_SpinCamera`. Measured: **134 functions in
asm/ have an address matching a park file**, so any name-suffix filter re-offers
a large slice of already-worked functions.

`tools/pickable.py`'s `parked()` already handles this — it matches on filenames
AND scans park CONTENTS, because a park covering a class is named for the class
and lists its members inside. Its docstring records that two rounds were
previously lost to exactly this. This was the third.

**Rule: a throwaway scan is still a candidate filter. Import `parked()` from
`tools/pickable.py` rather than re-deriving exclusion, and if a scan needs a
different ranking, change pickable.py instead of writing a parallel one.**

The cheap check that catches it after the fact is already in the workflow:
`git status` before committing. A park file that appears as `M` rather than `??`
means the function was already parked — stop and read the existing note before
overwriting it.

## A `volatile` local reproduces a value the ROM keeps in a stack slot

`Func_8092504` allocates a four-byte frame for ONE value, writes the entry byte
into it, and re-reads it on every loop iteration — while pushing r5, r6 and r7,
so it is not short of registers. Written as a plain local, gcc keeps the value
in a callee-saved register across the call and the function comes out FIVE
LINES SHORT (33 differing at 29 lines against 34).

Declaring it `volatile` makes it memory-resident and re-read at each use: 8
differing at exactly 34 lines.

**The tell:** `sub sp, #N` for a frame holding one value, stored once and
re-loaded inside a loop, in a function that has spare callee-saved registers.

**State it as an inference.** A plain local that gcc happened to SPILL produces
the same shape, and the two cannot be distinguished from output alone. Reach
for this when the frame is small, the value is loop-invariant, and register
pressure does not explain the spill — and say so in the park rather than
claiming the original said `volatile`.

## Name the COMPLETE offset, not a partial one

The name-the-offset lever has a precision requirement that cost three screens
on `Func_80b6cdc`. The ROM addresses a table as base plus a computed index,
with the field displacement folded INTO the index:

    rom    lsl r2, r1, #1 / add r3, r2, #4 / ldrsh r3, [r4, r3]
    ours   lsl r3, r1, #1 / add r2, r3, r4 / mov r5, #4 / ldrsh r3, [r2, r5]

Written as `s[i + 2]`, gcc folds the base into the pointer and uses the
displacement as the index — the mirror image. Naming the PARTIAL offset
(`off = i * 2;` then `s + off + 4`) changes nothing: gcc re-associates it to
`(s + off) + 4` and the output is byte-identical.

Naming the COMPLETE byte offset is what works:

    off = i * 2;
    a = off + 4;          /* the whole displacement, in its own local */
    ... *(short *)(s + a) ...

That matched outright. The ROM's own shape shows why the partial form is not
enough: it computes `i * 2` once and derives BOTH displacements from it with
separate adds, so `i * 2` is a real value in the original and each full offset
is another. Write both.

## Where the complete-offset lever stops: the base fold

Naming the complete byte offset restores the ROM's `[base, index]` form ONLY
when the base register gcc chose is already the right one. Both sides measured
in one round:

* `Func_80a3c98` — the ROM has the base in r6 and gcc had put it there too;
  the only difference was that gcc folded the base into the offset chain.
  Naming the complete offset (`off = 0x8a * 2 + i * 4`) matched outright, and
  the operand order INSIDE the offset expression made no difference.
* `Func_80f7df0` — the ROM holds the base in r4 across the whole body, reaches
  three fields with `[r4, rX]`, and pushes r5 to keep the offsets live. gcc
  folds each base-plus-offset into a pointer instead, needs one register fewer,
  and does not push r5. Naming every offset completely is worth only two
  differences (29 -> 27) and leaves it two lines short.

**The distinction: the fold and the register spend are one decision.** Holding
several complete offsets live costs a callee-saved register; folding the base
into a pointer avoids it. When the ROM pays that cost and gcc has no reason to,
the source cannot ask for the more expensive form — naming the offsets does not
create the pressure that would justify it.

So check the ROM's `push` list first. If it saves a register more than a
natural C version needs, the addressing difference is downstream of that and
the lever will not close it.

## The pooled-small-constant tell applies to SINGLE-USE constants only

The rule recorded here — Thumb-1 gas will not fold `ldr rX, =imm8`, so a pooled
small constant is a genuine symbol tell — is right but needs a qualifier, and
the qualifier matters because it decides whether a residue is worth chasing.

**A repeated literal pools too.** `src/overlays/rom_7ac2d8/ovl_2dcc_b.c` masks
with `0x1f` three times; CSE hoists it into a register and its generated `.s`
carries `.word 31`. That is a plain literal in matching code, not a symbol.

**A single-use small constant does not.** Probed with the project's flags on
three forms of `*p - 0x1f` — inline as a call argument, through an `int` local,
and as a bare return — all three emit `sub r0, r0, #31`. gcc never pools it.

So before treating a pooled small constant as a symbol, count its uses in that
function. Several uses is CSE and tells you nothing; one use is the tell.

`Func_801b9a8` and its twin `Func_801b9ec` use theirs once, and naming that
constant would close both.

## The int-local fix for a pooled constant needs a SPARE REGISTER

Routing a stored constant through an `int` local turns a pooled HImode literal
into a `mov`. It closed `Func_8011b00`, `Func_80173f4` and `Func_80c01bc`.

It is not free, and on `OvlFunc_common1_148` it BACKFIRES — the function sits
at 1 differing of 30 with the literal left alone, and every route through a
local is worse:

    a new `int` local                 1 -> 4 differ
    reusing the already-dead `t`      1 -> 4
    reusing the already-dead `off`    1 -> 4
    reusing the already-dead `v`      1 -> 5

Reusing a local that is already dead does not help, so this is not about how
many variables are declared. The constant has to be LIVE ACROSS THE STORE, and
that costs a register at exactly that point.

**Precondition: apply it when the function has a spare register at the store,
and re-check the count afterwards.** On a function that is already tight, the
pooled literal is the cheaper of the two wrong answers.

## Duplicate-constant CSE into a callee-saved register (blocker class)

A function calls the same routine twice, or two routines, passing the SAME
constant. gcc computes it once into a callee-saved register and holds it across
the intervening call. Holding a value across a call means saving the register,
so our prologue grows a push the ROM does not have, the epilogue grows a pop,
and every argument-fill site in between rotates. One hoist, a diff touching most
of the function. The ROM recomputes the constant each time.

Two instances, both parked:

  ovl942_2008144.c   0x94 << 1 twice   constant costs mov + lsl   18 of 30 differ
  ovl903_200843c.c   0x3333, 0x1999    constant costs one ldr     58 of 51 differ

The second one settles what the class actually is. In 942 the constant took two
instructions to build, so hoisting at least saved something. In 903 each use is
a single pool load and a register copy is also a single instruction, so the
hoist saves nothing and costs four. gcc hoists anyway. The rule is "repeated
constant", not "expensive constant".

**Cheapest tell: compare the prologues before diffing the body.** A push the ROM
does not have, in a function with a repeated constant, is this class.

Source spelling cannot reach it. Four spellings of the shared constant in 942 --
`0x128`, `0x94 << 1`, `0x94 * 2`, and the two mixed -- compile to BYTE-IDENTICAL
output. Constant folding runs before CSE, so by the time the hoist is decided
every spelling has collapsed to the same value. This is the opposite of the
pooled-constant tell, where `X << 1` versus `0x128` genuinely selects a
different instruction sequence; there the form is observable, here it is not.

Ruled out for both: `-fno-rerun-cse-after-loop` and `-O1`. Makefile:192 already
carries a per-file rule using that flag for two overlay TUs whose description --
"load a save-flag id twice around a call, and at -O2 gcc hoists it into a
callee-saved register" -- reads exactly like 903, and the flag does not help
either function. That Makefile comment's own caveat is now confirmed from the
other side: the flag is not what separates hoisting from not hoisting in
general, and whatever distinguishes those two TUs is narrower than this pattern.
Do not widen that rule on the strength of a shape match.

## Naming a value only works if gcc cannot fold it

Hoisting a subexpression into a named local is one of the highest-yield levers
here, and it has a precise precondition that three functions this round pinned
down: **the value must not be computable at compile time.** If gcc can fold it,
the local evaporates before code generation and the edit is a no-op.

  Func_80175c0   named `t = (idx << 1) + 0xeb0`, a RUNTIME expression
                 survived; produced the ROM's register-offset `ldrh r3, [r5, r3]`
                 in place of an incremental fold into the base. 26 -> 18.

  Func_801d94c   named `o = 0x5a4` and `q = base + off`, both compile-time
                 constants. Folded away, zero change.

  rom_79338_a.c  named `p = gFlags`, a link-time constant ADDRESS. Folded away,
                 zero change.

A symbol address counts as foldable. So before reaching for the lever, ask
whether the value depends on anything only known at run time. If it does not,
the lever does not apply and the difference is coming from somewhere else.

One further warning from that same TU: naming is not merely neutral when it
fails, it can be harmful. Routing GetFlag's mask through a named local cost
five instructions, because gcc's branchless `!= 0` idiom
(`neg / orr / lsr #31`) is reached only when the AND feeds the test directly.
Two functions in one file wanted opposite spellings. Screen per function, not
per file.

## CORRECTION: the duplicate-constant class is the DOMINANCE rule, already recorded

The section "Duplicate-constant CSE into a callee-saved register" above was
written as a new blocker class. It is not new. "When gcc HOISTS a repeated
constant, exactly: dominance" already had the mechanism, probed with five
variants: uses in mutually exclusive branches REBUILD, a use that dominates
another HOISTS. Both functions parked under the newer heading are straight-line,
so every use dominates the next, and the hoist is exactly what that rule
predicts.

What the two new parks do add is the COST side. The dominance entry does not
say whether gcc weighs what recomputing would cost, and OvlFunc_903_200843c
settles it: each use is a single pool load and a register copy is also one
instruction, so the hoist saves nothing and costs four. gcc hoists anyway.
Repetition plus dominance is the whole trigger; expense does not enter.

Read the two sections together. The dominance rule tells you WHETHER to expect
the hoist from the ROM's control flow, before writing any C.

## `parked()` was blind to 210 of 467 park files -- fixed by not parsing headers

Four header conventions had been added to `PARK_HDR` one at a time, each after a
parked function was re-offered as a candidate, and the last entry on this
promised the problem was closed. It was not. Auditing every park file at once
instead of reacting to one miss showed the regex recognising 257 of 467. The
tree also uses `NAME [path]`, `NAME -- NON-MATCHING.`, `NAME -- NOT MATCHING`,
`NAME [path] -- 0xaddr`, `NAME and NAME2 [path]`, files whose first line is a
bare `/*`, and class write-ups that name no function at all.

Nearly half the parked set was invisible. The selection filter dropped from 21
candidates to 10 once this was fixed, so roughly half of every candidate list
for an unknown number of rounds was work already done -- which is the real cost
of the four previous one-at-a-time patches, and it was never visible because a
re-offered function looks exactly like a fresh one.

**The fix is to stop keying on the header format.** `parked()` now builds the
universe of real function names from the .s corpus and looks for any of them in
each park file's leading comment, plus any C function the file defines. A sixth
convention cannot break that, because it keys on the NAME rather than on the
punctuation around it. `parked()` went from 687 entries to 1143, and the audit
that found the gap now reports zero unrecognised files -- run it again after
adding parks in bulk.

The general lesson, which cost four rounds to learn: when a filter is patched
repeatedly for the same class of miss, MEASURE ITS COVERAGE instead of adding
another case. One audit answered what four patches did not.

## `tools/filtered.py`: the selection filter, finally as code

The filter described under "A SELECTION FILTER that works" had been used once
and never committed, so every round re-derived it by hand. It is now a tool.
Same five rejects (size 40-120, no r8-r11, no repeated expensive constant, at
least 8 calls), reusing `pickable.parked()`, and it flags which candidates have
kin. Ten functions currently pass.

## Deleting an address-only local, confirmed on a LOOP base

"A local that only holds an ADDRESS can cost the ordering -- delete it" is an
old entry, and `UpdatePoison` is a clean second instance with a measurement
attached, because the local looked mandatory.

The ROM builds the gState offset rather than folding it:

    ldr r3, =gState / mov r2, #0xfc / lsl r2, #1 / add r6, r3, r2

The documented way to stop the fold is a local pointer, and it works -- 78
differing to 17. But the residue was then a register-role swap, `worst` and the
array pointer exchanged, plus the base materialising ABOVE the loop guard where
the ROM has it below. Both symptoms had ONE cause: a local assigned before the
loop is born before the guard, which lengthens its live range, and by
`global.c`'s priority formula a longer range loses the earlier register.

Indexing the array directly -- `gState[(0xfc << 1) + i]`, with the local gone --
matched outright. The index expression carries `i`, so there is nothing for gcc
to fold into `ldr =gState+504`, and the base is now born in the loop preheader
BELOW the guard, which gives it the shorter live range and the ROM's register.

So the two levers are not alternatives to choose between. **If the offset varies
with a loop induction variable, indexing already prevents the fold and the local
is pure cost.** Reach for the local pointer only when the offset is constant.

Also measured on the same function, and both worth knowing:
  * assigning the base INSIDE the loop body is much worse (79 lines to 87) --
    gcc reloads it every iteration rather than hoisting.
  * declaration order is inert here, three permutations byte-identical, which
    is what "register allocation follows ASSIGNMENT position" already predicts.

## The no-prototype lever: two independent hits in one round

The lever is recorded as narrow, with the important caveat that `extern int f();`
behaves identically to a full prototype -- so it is the ABSENCE of a declaration
that matters, not a weakened one. Two functions in one round were closed by it,
on two different callees, and in both the residue was an argument fill order and
nothing else:

    __Func_8092c40(0x10, 0)      rom mov r1,#0 / mov r0,#0x10   ours reversed
    OvlFunc_913_2008244(...)     rom sets r0 LAST among r0-r3   ours earlier

Deleting the declaration gave the ROM's order exactly in both cases -- 4
differing to 2 on the first, and 2 to a match on the second.

**So promote it in the order you try things: when the residue is purely which
register gets filled when, and the instructions themselves are right, delete the
callee's declaration before reaching for anything structural.** It costs one
screen. It is still narrow in the sense that it does nothing for register
ASSIGNMENT -- only for fill ORDER -- but that distinction is easy to make from
the diff, because a fill-order residue shows the same instructions transposed
while an assignment residue shows different register names.

## A stack argument in a callee-saved register means it crossed a call

`OvlFunc_913_2008a68` had a two-argument stack pair where the ROM built the
second value in r5 and stored it after all four register arguments were filled,
while we built it in a scratch register and stored it before them.

r5 is callee-saved, and gcc only spends one on a value that survives a call. So
the ROM's register choice is itself the reading: that value was live across the
preceding call in the original. Assigning it before that call rather than
adjacent to its own reproduced the register AND the deferred store, 6 differing
to 2.

**The live range wants to be the shortest one that still crosses a call.**
Assigning the same value at the top of the function instead is worse (6 again),
because a range that crosses several calls is not what the ROM's allocation
shows. This refines the per-call-site stack-argument rule rather than replacing
it: name both arguments per site, and then place the assignment by which
register the ROM spends.

## A second instance of the dominance contradiction

The dominance section ends with `OvlFunc_952_200be40` recorded as a case where
the ROM rebuilds a constant that, by the measured rule, it should have hoisted --
with the note that the likeliest reading is two different symbols coinciding in
value, and that this is inference from a contradiction rather than a
measurement.

`OvlFunc_891_2008098` is a second instance, and a cleaner one because everything
around it matches. Instructions 0-53 are exact; the ROM then builds a fresh 2
into a scratch register for a stack argument while the loop's stack value -- also
2, in r7 -- is still live and would have served. gcc commons them, which is what
dominance predicts. Naming the later constant is byte-identical, the same way
per-use-site naming fails against any dominating use.

Two instances is not proof, but it is enough to stop treating the first as a
one-off. If a third turns up, the symbol reading is the thing to test.

## Ranking a residue: first-divergence beats the differing COUNT

Splitting two disjoint live ranges into separate variables on
`OvlFunc_891_2008098` moved the first divergence from instruction 13 to 54 while
RAISING the differing count from 15 to 24. The 24-differing version is the
better one: its residue is a single localised call, where the 15-differing
version's divergence runs through the whole body and the count is low only
because the two streams keep re-synchronising by accident.

`--quiet`'s count is a similarity score, not a distance to a match. When two
versions disagree, compare where they FIRST diverge and how localised the
residue is, and keep the one whose remaining problem you can name.

## The `neg` interleave family is SOLVED as a class -- it was the missing guard

`src/non_matching/ovl_7c460c/2008c74.c` carried an eleven-function family with
one two-line residue, eleven failed spellings, three failed flags, and a corpus
zero that a later batch flagged as probably a tab-versus-space artifact. Re-run
correctly with `\s+` over the 3336 SOLVED .s files, the shape appears at **13
sites in 13 already-matching files**. The zero said nothing.

One of the 13 is `src/overlays/rom_7ef4f4/ovl_30_a_c_a_c.c` -- which IS
`OvlFunc_965_2009030`, a member of that family's own list. It was solved and the
family note was never updated. What it does is the documented argument-order
lever: name every split two-instruction build as a local in the function's ENTRY
BLOCK, which dominates the sites. Its `n1 = -0x10` is a `mov`/`neg` pair.

Counting conditional branches before the `neg` site for all eleven members:
**the solved one has its site inside two nested guards, and all ten unsolved ones
are straight-line.** That is a complete explanation, and it also explains the one
anomalous number in that park -- naming the constant at the top scored 43 instead
of 2, because with no guard to cross the local stays live and costs a register,
exactly as the argument-order section records for straight-line functions.

So the family is not its own blocker class. It is the straight-line half of the
argument-interleave class, already sized at 98 functions and already known to be
out of reach. Eleven functions did not need eleven more spellings.

**Generalise the lesson, not just the family:** a park that lists members and a
shared residue is claiming those members are equivalent. Check whether ANY of
them has been solved since -- by name, not by path, because the paths go stale
the moment a `.s` is split. Six of that family's eleven paths were stale.

## `tools/guarded_interleave.py`

Separates the two populations for the whole tree: functions whose interleave
sites are ALL dominated by a conditional branch (the lever works) from those with
any straight-line site (nothing reaches it). 83 unparked functions have only
guarded sites. Two elevations came from that list the day it was written --
`OvlFunc_959_2008bec` and `OvlFunc_922_2009d78`.

It counts `mov`+`lsl` and `mov`+`neg` alike, because the docs generalise the
shape to any two-instruction build and a filter that implemented only the `lsl`
example let a `neg` case through the round before.

## A POOLED argument also needs naming in the entry block

`OvlFunc_959_2008bec` came to 2 of 59 with the three shifted constants and both
`neg` builds named at the top. The last two instructions were a pool load and a
`neg` transposed:

    rom   neg r1, r1 / ldr r2, =0xe666
    ours  ldr r2, =0xe666 / neg r1, r1

Adding `e = 0xe666;` to the same entry-block group matched outright. So the rule
is not "name the two-instruction builds" -- it is **name every argument that the
ROM materialises inside the interleaved run, pool loads included.** A pool load
is one instruction and looks like it needs no help, but it participates in the
same scheduling decision and leaving it as a literal pins it in the wrong slot.

## A global RE-READ across a branch is a `volatile` tell

`OvlFunc_922_2009d78` reads `iwram_3001e40` twice -- once for `& 3` before a
guard, once for `& 7` after it -- and the ROM emits a second `ldr`. gcc commons
the two loads, which cost exactly the one missing line and left everything after
it shifted by one.

`extern volatile int iwram_3001e40;` matched outright. This is the documented
"volatile is a reading, not a hack" signature, and the version to reach for when
the residue is a load the ROM performs and we do not.

Measured alongside, and worth keeping: adding a pointer local for the global's
address -- `int *g = &iwram_3001e40;` -- is inert on its own AND still fails when
combined with `volatile`. The address is a link-time constant, so the local is
folded away and only costs ordering. That is the third confirmation of "a local
that only holds an ADDRESS can cost the ordering, delete it".

## Reading rule: check whether the constant is ALREADY the destination

Before reaching for the constant-as-destination lever on an `and` or `orr`,
compare the operand order in BOTH streams. On `OvlFunc_968_2009150`:

    rom   ldrb r2, [r0, #0] / mov r3, #0xfe / and r3, r2
    ours  ldrb r3, [r0, #0] / mov r2, #0xfe / and r2, r3

The constant is the destination in both -- r3 in the ROM, r2 in ours. What
differs is only which register each value got, so the lever has nothing to do and
this is the register-role swap. Screened anyway before that was noticed: `int m`
for the `and`s is inert, `unsigned char n` for the `orr` is inert, and applying
both at once is much worse. The two shapes look identical at a glance because
both show a `mov` of a constant beside a load; only the operand order of the
`and` tells them apart.

## The per-call-site stack-argument rule has to be PAID FOR

Naming both stack arguments per call site is a reliable lever, and
`OvlFunc_924_20096c4` measures its boundary. The same edit buys the post-loop
call outright and backfires inside the loop:

    16  naive
    13  name both stack arguments of the post-loop call        <- best
    13  the above plus a local for the first in-loop site      (inert)
    94  the above plus a local for the second in-loop site     (92 lines -> 97)
    94  all four sites named at once                           (92 lines -> 97)

The lever works by keeping BOTH values live simultaneously so the ROM's two
registers appear instead of one register reused. That is a purchase of two
registers at the call, and the surrounding block has to have them spare. This
loop already keeps six values live and the ROM spends r8, r9 and r10 on them; one
more named local is free, a second forces a spill and costs five lines.

**So apply it freely in straight-line code and cautiously inside a loop that is
already spending high registers** -- and add the sites ONE AT A TIME, because the
failure is not gradual. Going from one extra local to two took the function from
13 differing to 94.

## Working a guarded-interleave candidate: the flag id is the one constant NOT to name

The entry-block naming lever and the `GetFlag(id)`/`SetFlag(id)` rule collide on
the same functions, and the order to apply them in is now clear.

`OvlFunc_923_200996c` is the guarded-interleave shape -- a store interleaved into
two position builds, all behind a flag guard -- and naming those builds in the
entry block reproduced them on the first screen. It came to 7 differing, and all
seven were the flag id:

    rom   mov r0, #0x94 / lsl r0, #2   ... twice, rebuilt
    ours  mov r5, #0x94 / lsl r5, #2   once, held in r5, `mov r0, r5` at both

Naming the id made it worse in the sense that mattered, but so did the literal --
both spellings are byte-identical, because the id's FIRST use is the `__GetFlag`
call in the entry block itself. There is no guard between the assignment and that
use, so the entry-block mechanism has nothing to work with: the value is simply
live across a call and gets a callee-saved register. `CSE_CFLAGS`
(`-fno-rerun-cse-after-loop`) is exact, which is what the GetFlag/SetFlag rule
already says.

**So when working a candidate from `tools/guarded_interleave.py`:**

  * name every constant whose uses are all AFTER a guard -- that is the lever;
  * do NOT name a flag id used by the guard itself -- it cannot help, and
  * screen with `--no-rerun-cse` from the start whenever the ROM shows the same
    id materialised once before a conditional branch and once inside it.

The distinction is the position of the FIRST use relative to the guard, not the
constant's kind. A value first used inside the guarded block rematerialises; a
value first used in the entry block is live across the call and commons.

## The guarded-interleave routine, in order

`tools/guarded_interleave.py` has produced nine elevations. Working its output is
now mechanical, and the order matters because the steps interfere:

1. **Name every constant whose uses are all AFTER a guard** -- shifted builds,
   `mov`/`neg` builds, and pool loads alike, assigned in the entry block.
2. **Never name a flag id used by the guard itself.** Screen with
   `--no-rerun-cse` from the start when the ROM materialises one id both before
   and inside a conditional branch, and add a `CSE_CFLAGS` rule if it lands.
3. **Delete the callee's prototype** when the residue is the same instructions
   transposed (fill order). Not for different register names.
4. **Delete any local that only holds an address.**
5. **Write the arms in the ROM's fall-through order.**

Steps 1 and 2 look contradictory and are not. The discriminator is the position
of the FIRST use relative to the guard: a value first used inside the guarded
block rematerialises and wants naming; a value first used in the entry block is
live across a call, commons, and wants the flag. Three functions in one batch
were predicted correctly from that rule alone.

Steps 3, 4 and 5 are each independently confirmed multiple times now -- the
no-prototype lever three times, the address-only local four times on four
different shapes, and the fall-through reading again on `OvlFunc_953_200839c`
where swapping the arms took 58 differing to 4.

## The entry-block naming lever has a BUDGET -- name only the interleaved site

Every previous use of this lever named a handful of constants and matched.
`OvlFunc_968_2008b98` shows what happens past the limit, and it is not a
graceful degradation.

Six constants named in the entry block -- the shifted argument, two struct
initialisers, two pooled values and an argument -- and gcc kept ALL of them
live from the entry block instead of rematerialising them after the guard.
That means high registers: our prologue grew `mov r7,r11 / mov r6,r10 /
mov r5,r9 / push {r5,r6,r7}` against the ROM's single `mov r7,r8 / push {r7}`,
and the function came out 98 lines against 85 with 89 differing.

Naming ONLY the constant at the actual interleave site -- the one whose build
the ROM splits around `add r5, sp, #0x10` -- and leaving the other five as
literals took it to 85 lines and 4 differing, two of which were the divide-alias
false negative. Widening the stack struct by one word closed the rest.

**So the rule is not "name the constants in the entry block".** It is: name the
constant whose two-instruction build the ROM interleaves, and nothing else. The
mechanism only pays when rematerialising is cheaper than holding, and each extra
name pushes gcc toward holding. Read the ROM for which build is actually split
before naming anything -- the other constants in the same block are not
participating and naming them is pure cost.

The failure is visible in the prologue before any diff is read: high registers
saved that the ROM does not save.

## The INVERSE of the disjoint-live-range rule: one register means ONE variable

"A variable with DISJOINT live ranges should be two variables" has a mirror
image that is just as useful, and `OvlFunc_954_2008490` turns on it.

The ROM computes a field into r6, tests it, and then reuses r6 for an unrelated
result:

    asr r6, r3, #0x14      /* y = field >> 20 */
    cmp r6, #0xb           /* tested three times */
    mov r6, #0xa0          /* n, a different value entirely */

Written as two variables `y` and `n`, gcc puts y in r0 and n in r6 and the whole
dispatcher rotates -- 38 differing. Written as ONE variable reused for both, it
matched outright.

So read the ROM's register reuse in both directions. **Two registers for what
looks like one value means two variables; one register for what look like two
values means one variable**, even when the two roles are semantically unrelated.
The second reading feels wrong to write -- reusing `n` to hold a coordinate
before it holds an offset is not how one would author it -- but it is what the
original did, and the allocator will not be argued into it otherwise.

## CSE_CFLAGS has a counter-example: check what ELSE the unit commons

"`GetFlag(id)` guarding a block that ends `SetFlag(id)` means `CSE_CFLAGS`" has
been right on five-plus functions. `OvlFunc_952_2008264` is the first case where
applying it makes the function WORSE, and the reason generalises.

The unit carries two constants with opposite needs:

    0x966    GetFlag guard + SetFlag inside      wants the pass OFF
    0x2241   message-id base held in r6, with the ROM deriving
             `add r0, r6, #1` and `add r0, r6, #2`   wants the pass ON

    default                     33 differ   0x2241 derives correctly, 0x966 commons
    -fno-rerun-cse-after-loop    57 differ   0x966 correct, 0x2241 stops being held

The flag is per translation unit, so it cannot give one constant the pass and
deny it to another. No source spelling separates them either -- both are
compile-time constants, and naming the base folds under the flag exactly as the
"INVERSE constant problem" section records.

**Before adding a CSE_CFLAGS rule, look for a pooled base that the ROM HOLDS and
DERIVES from.** That is the thing the flag costs, and it is easy to miss because
it is working correctly in the unflagged build.

## RE-ATTACK PARKS WITH LEVERS FOUND AFTER THEY WERE WRITTEN

`OvlFunc_946_20092b4` sat parked at TWO differing on this residue:

    rom   ldrb r2, [r1] / mov r3, #0x2 / orr r3, r2
    ours  ldrb r3, [r1] / mov r2, #0x2 / orr ...

Its park lists two spellings tried and rejected. Neither is the documented fix.
"The ORR-destination lever needs an `unsigned char` local, not an `int` one" is
in this file, it is exactly this shape, and `unsigned char m = 2; e[0x23] |= m;`
matched outright. The park predated the lever and nobody went back.

**A park is a snapshot of what was known the day it was written.** The inventory
grows every batch, so before spending a round on fresh candidates, grep the park
corpus for the residue shape of whatever lever was learned most recently. Two
lines from matching is common in that corpus and the cost of a re-screen is one
command.

The counter-example, so this is not read as a blanket instruction:
`OvlFunc_947_2009fd4` is also at 2 differing on an `orr` operand-role residue,
and its park HAS tried the narrow local -- 18 differing, much worse -- along with
five other spellings. A thorough park stays parked. Read what was tried before
re-screening, not after.

## The park corpus goes STALE: 12 files named only already-elevated functions

Sweeping every park file for whether the functions it names are still unsolved
found twelve that were not -- including one for `Func_80b86ec`, elevated from a
solved twin two batches earlier with its park left in place. One of them cost a
round's effort: `OvlFunc_919_2008200`'s park describes a live register-role swap,
and the function has been elevated since.

They are moved to `toDelete/stale_parks/` rather than deleted, since the analysis
in them may still be worth reading. Re-run the sweep after any batch that
elevates from the park corpus; the check is cheap and a stale park is worse than
no park, because it reads as a measured dead end.

## `tools/park_retry.py` -- and what the first sweep with it found

Ranks parks that are CLOSE (<=20 differing) and whose notes never mention a
lever whose residue shape they carry. It does not claim the lever works; it says
the lever was never written down as tried. 123 parks qualify.

**Two elevations came from one lever in two rounds**, both on the same residue:

    rom   ldrb r2, [r1] / mov r3, #K / orr r3, r2
    ours  ldrb r3, [r1] / mov r2, #K / orr r3, r2

`OvlFunc_946_20092b4` and `OvlFunc_903_2008d68`, both closed by
`unsigned char m = K;`. The second park is the instructive one: it recorded
trying "the constant as a named local" and concluded "nothing in
docs/elevation.md reaches it". The local was an `int`, and the doc's own
sharpening is that an `int` local is folded and is *no lever at all*. **When a
park says a named local was tried, check whether it records the TYPE.** If it
does not, it has not tried the lever.

**The boundary, measured the same round.** `OvlFunc_947_200a1ac` carries a
residue that looks identical -- `mov r4, #0x8 / and r3, r1 / orr r3, r4` against
ours -- and `unsigned char` is MUCH worse there: 47 differing and one line long,
against 9 for the int. The difference is what the ROM needs from the constant.
In the two that closed, the constant is the ORR DESTINATION. In 947 the ROM's
requirement is that the constant stay LIVE across two flag updates, and a narrow
local will not do that. Same-looking residue, opposite remedy.

## A park that records its attempts is a result -- re-deriving it is not

`Func_80170c4`'s park lists four attempts, including "a single `return d;`
reached by `goto out;` ... THE LENGTH BECOMES CORRECT, 24 against 24 ... BUT THE
COUNT GETS WORSE, 8 of 24, because r4 and r5 then swap roles".

Restructuring it as `if (n > 0) { ... } return d;` -- the natural spelling of the
same shared exit -- reproduces exactly that: 24 lines, 8 differing, r4 and r5
exchanged. Two more screens on the priority formula (a local copy of the counter,
and declaration order) are both 8, which the park also already records.

The park was right and the effort was wasted. `park_retry.py` ranks by what is
NOT mentioned, which is a heuristic over prose; the sweep is for generating
candidates, and the park itself is still the thing to read before screening.

## A RANKING TOOL CAN HIDE THE THING IT WAS BUILT TO FIND

`tools/protolever.py` scored a MATCH as the worst possible result for as long as
it has existed. `tryc` prints `  OK <name> (N lines)` indented; the tool did
`out.strip()` and then tested for `" OK "` with a leading space, which the strip
had just removed. An OK row therefore fell through to the "no differing count
found" branch and scored 10**9.

On `Actor_SetAnimAndSpeed` it printed

    drop Sprite_SetAnim      OK Actor_SetAnimAndSpeed  (51 lines)
    ...
    best: 4 differing, as written

The match was in the transcript and invisible to the summary. It was only caught
because the printed rows were read rather than the verdict.

**Generalise it: a tool that ranks its own screens fails SILENTLY, because the
rows it prints stay correct while the conclusion inverts.** Any sweep whose
summary disagrees with its own output is reporting a bug, not a result — check
that before believing either. The fix here matches on the leading token rather
than on surrounding whitespace, and is unit-tested against four output shapes,
including the `[size check skipped]` suffix that tryc appends on multi-function
references.

How many candidates this cost is not knowable from here; the tool's docstring
says it was run over 62 saved candidates, and any of those that matched on a
single deletion would have been reported as a failure.

## A MISSING RELOAD after a store of a different width is an ALIASING tell

`Func_808d828` sat at 68 differing with everything after one point shifted. The
cause was a single load:

    rom   strh r3, [r2, #0] / ldr r1, [r5, #0x8] / ...
    ours  strh r3, [r2, #0] / (no reload -- the old r1 is reused)

At `-O2` gcc-2.96 has strict aliasing on, so a `short` store provably cannot
alias an `int` read and the reload is commoned away. `-fno-strict-aliasing`
restores it and takes the function from 68 differing to 7, at exact length.

**Recognise it by the missing load, not by the instructions around it.** Where
the ROM re-reads a field after storing through a pointer of a DIFFERENT WIDTH
and we do not, test `ALIAS_CFLAGS` before spending screens on the surrounding
code — the divergence it causes is unbounded, because everything downstream
shifts, so it looks like a much larger problem than it is.

## The derived-constant rule: unreachable only when both values are DEAD

"The INVERSE constant problem" says a `sub rN, #K` applied to a pooled constant
is the ROM deriving one offset from another and is not reachable, because
`off = A; ... off -= K;` folds at each use. That is true of the case it was
written from, and too broad.

`Func_808d828`'s ROM does the same shape:

    mov r2, #0x80 / lsl r2, #2 / and r3, r2 / ... / sub r2, #0x64

and the mutate-in-place spelling REPRODUCES it exactly:

    m = 0x80 << 2;
    if ((f & m) != 0) { m -= 0x64; ... }

The difference is that the value has a REAL USE — as the `and`'s mask — between
its definition and the subtraction. It is one live variable being mutated, not
two dead constants gcc can fold independently.

**So: deriving is unreachable when both values are dead constants, and reachable
when the first is genuinely consumed before the arithmetic.** Check for a
consumer before writing the shape off.

## The aliasing tell is a RECOGNISER, not a lever to sweep with

`-fno-strict-aliasing` has now closed two functions in two rounds, and a
systematic sweep of it found nothing. Both facts are useful.

Swept across the thirty closest parks that carry candidate C and a live
reference and do not already mention the flag: **zero improvements, every one
unchanged.** So it is not a flag to try speculatively. What it IS is the remedy
for one recognisable shape:

    the ROM RE-READS a field after a store through a pointer of a
    DIFFERENT WIDTH, and we do not

`Func_808d828` (68 differing to 7) and `Func_80935d4` (54 to 4) both show exactly
that and nothing else distinguishes them from the thirty that did not respond.
Read the ROM for the missing load; do not sweep.

The reason it is worth catching early is the SIZE of what it causes. Losing one
reload shifts everything downstream, so a single commoned load presents as fifty
instructions of divergence. On both functions the diff looked like a structural
misreading and was one flag.

## `pop {r1} / bx r1` in a void-looking function: `int` with NO return statement

Recorded before as "names a RETURN VALUE"; the spelling matters and is worth
pinning. On `Func_80935d4`:

    declared void                                   4 differing, epilogue wrong
    declared int, `return st;` at the early exits  74 differing, 90 lines
    declared int, NO return statement anywhere      2 differing, epilogue exact

Adding explicit returns makes gcc materialise a value at each exit, which the
ROM does not do. The function falls off the end and whatever is in r0 is the
result — so declare the return type and write no `return` at all.

## The aliasing tell, sharpened: a CHAR store does not qualify, nor does a CALL

`tools/aliastell.py` finds functions whose ROM re-reads a field across a store.
Its first version returned 48 candidates and the first two worked were both
false positives, each for a different reason. Both are now excluded and the
output is 11.

**A call between the store and the re-read means nothing.** gcc cannot assume a
pointed-to field survives a call, so it reloads regardless.
`OvlFunc_888_2008848` was offered on exactly that and `-fno-strict-aliasing`
changes nothing; its real blocker is a dominance hoist of a repeated mask.

**A store through a CHARACTER type does not qualify either.** A character type
aliases everything under the standard, so gcc must reload after it whatever the
flag says. `OvlFunc_947_2009938` re-reads `a[0x50]` after `a[0x23] &= 0xfe`, and
the re-read already matches without the flag.

So the shape that pays is narrower than "a re-read across a store": it is a
re-read across a store whose type **cannot** alias the loaded type — `short`
against `int` on `Func_808d828` and `Func_80935d4`, both of which went from
~60 differing to single digits.

## A large unsigned literal makes the COMPARISON unsigned

On `OvlFunc_947_2009938`:

    b[2] + 0xfff00000 >= a[2]      ->  cmp / bcs   (unsigned)
    b[2] - 0x100000   >= a[2]      ->  cmp / bge   (signed)

Both emit the identical `ldr r2, =0xfff00000 / add r3, r1, r2` — the pool word
is the same and only the branch differs. So a ROM `bge` where we emit `bcs`,
with the same pool constant, is a tell about how the literal was SPELLED in the
source, not about the operand types. Write the negative.

## Read the UNSIGNED value before the SIGNED one: confirmed, and it closes functions

Found on `OvlFunc_931_20086f0` (24 differing to 17) and now decisive on
`Func_809b804`, which it took from 2 differing to a match.

The shape: the ROM reads the same halfword twice, once signed and once
unsigned, and emits `ldrsh` before `ldrh` regardless of what the source says.
Writing the UNSIGNED read first anyway changes the register assignment:

    s = *(short *)(a + 0x3a);            /* signed first  */
    v = *(unsigned short *)(a + 0x3a);   ->  mov r2, #0x3a / ldrsh r3, [r5, r2]

    v = *(unsigned short *)(a + 0x3a);   /* unsigned first */
    s = *(short *)(a + 0x3a);            ->  mov r1, #0x3a / ldrsh r3, [r5, r1]

Nothing else moves — the emission order is identical both ways. **Source order
picks the REGISTERS for two independent values; it does not pick the emission
order.** Those are separate effects and worth holding apart, because the
emitted listing looks unchanged and it is easy to conclude the edit did nothing.

On `Func_809b804` the offset constant landing in r1 rather than r2 was the whole
residue. Try this whenever a halfword is read both ways and the only difference
is which scratch register holds the offset.

## The `multi` population is a real pool, and it had never been worked

`tools/census.py` classifies 692 remaining functions as `multi` -- they share a
`.s` with others, so their blocker is UNKNOWN until a split, and they are
excluded from the `open` worklist for that reason. That is not the same as being
blocked, and after several rounds of thin returns from the worked-over pools it
was the obvious place to look.

Selecting from it with the criteria that have been paying -- loop-free, no
r8-r11, no repeated expensive constant, at least three calls, 25 to 75
instructions -- leaves six functions. Three were tried and two matched, each on
one edit after the first screen:

    Func_809b804   2 differing on the first screen, matched by reading the
                   UNSIGNED halfword before the signed one
    Func_80970f8   4 differing on the first screen, matched by naming the
                   loaded byte before its store

Both first screens were already at exact length. The `multi` label costs a
`split_s.py` run and nothing else, and the split is byte-neutral by
construction — so treat that population as ordinary candidates, not as work
deferred behind an obstacle.

## Initialise the RESULT before the value it is compared against

`GetMoveDisplayEffect` is a chain of independent `if`s, each overwriting a
result. Written the obvious way -- compute the key, zero the result, then test --
gcc IF-CONVERTS the first test into a branchless `eor / neg / orr / lsr #31`
sequence and the whole function diverges: 37 of 36 differing and two lines long.

Moving the initialisation ABOVE the key computation:

    r = 0;
    t = m[1] & 0xf;
    if (t == 1) r = 1;

is 14 differing and one line short. Nothing else changed. The result being live
before the condition exists is apparently enough to stop gcc treating the pair
as a settable predicate.

**So when a ROM has a run of `cmp / bne / mov` and we emit branchless
arithmetic, try hoisting the result's initialiser above everything the
conditions read.** It costs one screen and it is not the same as the
assignment-position lever for register choice -- here it changes the CONTROL
FLOW gcc emits.

## `volatile` at the use site reaches a redundant load that `-fno-gcse` does not

The same function ends with the ROM reading `m[3]` TWICE -- once into a register
compared three times, once again as the call argument -- with no store between.
gcc commons them and is one line short.

Measured, all inert: two separate `int` locals for the two reads;
`-fno-gcse`, which docs/elevation.md records as reaching re-reads no cse-family
flag does; and passing the already-loaded local instead.

`*((volatile unsigned char *)m + 3)` at either use site matches outright.

So `-fno-gcse` and a use-site `volatile` are not interchangeable: the flag did
not restore this load and the cast did. Reach for the cast when the redundant
load is at ONE identifiable site -- it is also narrower than a flag, which is
the right shape for a whole-TU decision that only one expression needs.

## A stack argument equal to a register argument is ONE named local, used twice

`OvlFunc_916_2008150` calls the same six-argument routine from both arms of a
branch. In the first arm the ROM builds 4 once and uses it for BOTH the fourth
argument and the stack slot:

    rom   mov r2,#9 / mov r3,#4 / str r2,[sp,#4] / mov r0,#0 / mov r1,#0
          / mov r2,#1 / str r3,[sp,#0]        <- r3 is still argument 4

Writing `f(0, 0, 1, 4, 4, 9)` with two literals gives two materialisations and
23 differing. Writing `v = 4; f(0, 0, 1, v, v, 9)` matches. The other arm needs
them distinct and gets two locals.

This is the per-call-site stack-argument rule with the extra step: check whether
the ROM's stack value IS one of the register arguments before giving it a local
of its own.

## The aliasing class has a SECOND form: a load that SANK, not one that vanished

`Func_8096d2c` sat at 4 of 41 with everything else exact. The residue:

    rom   ldrh r3,[r2] / add r3,#1 / ldr r6,[r5,#0x68] / strh r3,[r2] / ...
    ours  ldrh r3,[r2] / add r3,#1 / strh r3,[r2] / ... / ldr r6,[r5,#0x68]

gcc SANK the `int` load past the halfword store, which is legal only because
strict aliasing says the two cannot alias. `-fno-strict-aliasing` matched
outright; three source orderings, including assigning the pointer first, were
all inert.

So the class is not only "a reload that vanished" -- it is also **a load that
moved to the wrong side of a store of a different width.** Same cause, opposite
symptom.

**This form is not detectable from the ROM listing** and `tools/aliastell.py`
does not look for it: the ROM's order is the natural one, so there is nothing
anomalous to scan for. It is visible only in the DIFF. The practical rule is to
try `ALIAS_CFLAGS` whenever a small residue is a load sitting on the wrong side
of a store of a different width, whether or not the detector offered the
function.

## Mirror the ROM's control flow, including which arm falls through

Two functions this round turned on it, in opposite directions.

`Func_8099070`: the ROM's `beq` jumps to a one-line arm and FALLS THROUGH to the
main block, so the main block is the `if` body. Written as an early return for
the small case -- which reads far more naturally -- the arms swap and everything
after diverges. **29 differing to 7 on that alone.**

`Func_80b27b0`: a chain of four `if (kind == N) { if (test) goto set; }` blocks
converging on one assignment, with the last arm branching out rather than
falling through. Transcribed as `goto` exactly as the ROM lays it out, it matched
on the first screen -- 47 instructions, no iteration.

The reading is not new but the emphasis is: **when a function is a chain of
guarded tests converging on one result, write the gotos.** Restructuring it into
early returns or `else if` is a different program to gcc even when it is the same
program to a reader, and the cost shows up as a whole-function divergence rather
than a local one.

Note this is the same rule the `Func_80ae99c` park bounds: layout tells you which
arm falls through WHEN GCC EMITS A BRANCH, and says nothing when gcc if-converts
instead. Chains of tests around calls and loads always branch; two bare constant
assignments may not.

## `add rOff, #1` then a recomputed address can be a plain POINTER INCREMENT

`Func_80a8034` writes two adjacent bytes and the ROM does:

    mov r2, #0x88 / lsl r2, #1 / add r3, r6, r2   /* address */
    add r2, #0x1                                  /* offset  */
    ... / strb r5, [r3, #0]
    add r3, r6, r2                                /* address again */
    strb r5, [r3, #0]

Modelled literally -- a live `off` incremented, the address recomputed from it
each time -- that is 9 differing, exact length, with `off` and the pointer in
each other's registers and no source ordering moving them.

Written as what it actually is, `p = g + off;` once and then `p += 1;`, it
matches. gcc chooses to keep the offset live and rebuild the address; that is
its allocation decision, not a shape the source has to spell out.

**So do not transcribe an offset-plus-recompute pair literally.** Two adjacent
accesses are a pointer increment; the ROM's extra `add` is how gcc spends
registers, and asking for it directly gets the registers backwards.

## The merge lever, fourth confirmation: the RESULT can be the input variable

`Func_80b19cc` computes a value, then overwrites it in each of three arms. The
ROM keeps one register (r5) for the loaded value and then for the result of
whichever arm ran. Written with a separate `r`, gcc materialises the result in
r0 ahead of the first compare and the whole tail diverges -- 27 differing.
Assigning back into the input variable matches.

That is four functions now where "one register for two unrelated values means
ONE variable" closed or nearly closed the diff, and one (`Func_8099070`) where
the ROM genuinely uses two registers and merging is worse. The discriminator is
the ROM, not a preference: count the registers before choosing.

## An address local's BIRTH STATEMENT decides whether it gets its own register

`OvlFunc_886_20090c0` sat at 4 differing of 60 with everything aligned except a
scratch register: the ROM builds `0xc0 << 13` in **r2** and then reuses r2 for
the byte pointer, while ours built the constant in r1 because r2 was already
claimed by the pointer.

The ROM's own order is unambiguous:

    ldr r3, =0x6666 / mov r2, #0xc0 / str [r5,#0x18] / str [r5,#0x1c]
    ldr r3, [r5,#0xc] / lsl r2, #0xd / add r3, r2
    mov r2, r5                        <- the pointer is born HERE
    str r3, [r5,#0xc] / str r3, [r5,#0x3c] / add r2, #0x64

Three placements of `q = a + 0x64;` were measured, changing nothing else:

| where `q` is assigned | differing |
|---|---|
| before the `+0xc` update | 4 |
| **between the two int stores** | **1** (and that one is the linker alias) |
| after both int stores | 10 |

So this is finer than "POINTER BIRTH ORDER decides which register each pointer
gets", which is about the order of several pointers relative to each other.
Here there is ONE pointer, and what matters is the **statement gap** it is born
in. Born too early it holds a callee-saved register through the constant's live
range and pushes the constant elsewhere; born too late gcc rebuilds the address
after the stores instead of before them.

**Read `mov rN, <base>` in the ROM as a statement position, not as a detail of
addressing.** When a diff is "one scratch register is wrong and nothing else
is", try moving the address local's assignment one statement at a time.

## A THIRD signature for the aliasing class: an address computation that FLOATS

The two recorded signatures are a reload that vanished and a load that sank.
`OvlFunc_886_20090c0` adds a third, and it is not about a load at all.

With strict aliasing on, gcc knows the later `*(short *)q = ...` stores cannot
touch the `int` fields at +0xc and +0x3c, so it is free to hoist `add r2, #0x64`
above them; the ROM keeps it below. `-fno-strict-aliasing` reinstates the
dependence and the `add` lands where the ROM has it -- 7 differing to 4, with no
source change. `-fno-schedule-insns`, `-fno-schedule-insns2`,
`-fno-rerun-cse-after-loop` and `-fno-gcse` all leave it alone or make it worse.

So the tell is not only "a load that should be there is missing". It is also
**an address computation scheduled across stores of a different width**. That
widens what `tools/aliastell.py` should be looking for; the tool's docstring
already says it cannot find the sunk-load form, and this form is invisible to it
too, because there is no re-read to key on.

## The interleave is narrower than "this ROM likes r0 early"

`OvlFunc_927_200a1b0` is a 108-instruction cutscene script that comes out at
exactly 108 with SIX differing, all one shape: `mov r0, #0x12` has to land
before the `lsl` that finishes r1's split build.

The useful observation is what does NOT differ. The same ROM issues four calls
to a FOUR-argument callee with the same split-constant arguments, and every one
of them puts `mov r0, #0x12` last, after all the shifts -- exactly as ours does.
Only the two- and three-argument calls interleave.

That rules out the reading "the ROM was compiled with a different scheduler" and
points at argument LOADING order, which is consistent with no scheduler flag
touching it: `-fno-schedule-insns`, `-fno-rerun-cse-after-loop`, `-fno-gcse`,
`-fno-strict-aliasing`, `-fno-defer-pop` and `-fno-expensive-optimizations` all
leave it at 6, and `-fno-schedule-insns2` takes it to 52.

It also confirms the guarded/straight-line split from the other direction: this
function has NO conditional branch, so the naming lever has nothing to
rematerialise across, and naming the split builds -- the spelling that works on
guarded sites -- is exactly inert. **`tools/filtered.py` should not have offered
it**: the filter counts calls and instructions and says nothing about guards.

## A symbol-relative load needs BOTH halves named: the base AND the complete offset

`Func_80b26cc` reads a record out of the file-local table `.Lb41ac` at
`(id*33)*2 + 0x30`. The ROM keeps the symbol in one register and the whole
computed displacement in another:

    lsl r3, r5, #5 / add r3, r5 / lsl r2, r3, #1
    ldr r1, =.Lb41ac / mov r3, r2 / add r3, #0x30 / ldrsh r0, [r1, r3]

Written the obvious way, `*(short *)(Lb41ac + off + 0x30)`, gcc reassociates to
`(Lb41ac + 0x30) + off` and pools the FOLDED symbol — `ldr r2, =.Lb41ac+48` —
which is 37 lines against 39 and 27 differing.

Two edits, each worth measuring separately:

| source | result |
|---|---|
| `*(short *)(Lb41ac + off + 0x30)` | 37 lines, 27 differing |
| `n = off + 0x30; *(short *)(Lb41ac + n)` | **39 lines**, 8 differing |
| `base = Lb41ac; n = off + 0x30; *(short *)(base + n)` | 39 lines, **2** |

Naming the complete offset stops the fold and fixes the length. Naming the BASE
as well is what puts the symbol in the addressing base position — without it
gcc emits `ldrsh r0, [r3, r1]` with the computed value as the base and the
symbol as the offset, which is the same instruction with the operands the wrong
way round. The named base is also reused for the second address the ROM builds
(`add r3, r2, r1`), so one local fixes both sites.

**So "name the offset, not the base" is half the rule.** When the base is a
SYMBOL, name it too — the fold and the operand order are two different defects
and each has its own remedy.

## `do { } while (i != N)` stops gcc reversing a counted loop

`Func_8019a54` walks three slots. Written `for (i = 0; i < 3; i++)` gcc reverses
the loop — `mov r6, #2 / sub r6, #1 / bge` — because the index is dead in the
body. The ROM counts UP and exits on `cmp r6, #3 / bne`.

Rewriting the same loop as

    i = 0;
    do { ... i++; } while (i != 3);

gives the ROM's up-count, and moved the first difference from instruction 6 to
instruction 14 with nothing else changed. Indexing the array with `i` so the
counter is a real induction variable does NOT prevent the reversal (23 → 20
differing, still descending); only the `!=` exit test does.

**Read the loop's exit test in the ROM before writing the loop.** `cmp rN, #K`
with `bne` is an up-counting `do`/`while (i != K)`; a `bge` against zero on a
descending counter is what a `for` with a dead index compiles to.

## Write the counter bump AFTER the call, even when the ROM has it before

`Func_80b26cc`'s loop body is a call, then a bound check on a counter the ROM
increments BEFORE the call:

    mov r1, #1 / add r6, #1 / bl _Func_8078ad0 / cmp r6, #7 / bgt

Written that way in source — `k++;` then the call — gcc emits `add r6, #1`
first and the argument second, which is 2 differing. Writing `k++` AFTER the
call statement matches: gcc hoists the increment above the call on its own and
the argument setup wins the earlier slot.

That is the whole remaining diff on this function, so it is worth stating
plainly: **the ROM's instruction order for a loop-carried increment is not the
source's statement order.** gcc will move the increment up; what the source
controls is what gets the slot before it.

## The birth-statement lever governs a COMPUTED pointer, not a COPIED one

The new address-local rule (an address local's birth statement decides its
register and placement) does not generalise to every misplaced pointer move.

`OvlFunc_901_2008c1c` sits at 2 of 75 because a `mov r2, r8` lands one
instruction late — the ROM issues it before an unrelated byte store, gcc after.
That is a pointer copy landing in the wrong statement gap, which is exactly the
shape the lever describes, and the lever does not reach it: a copy born between
the two candidate statements, the same copy born one statement earlier, and
re-deriving the pointer at the use site all leave it at 2 or make it much worse
(74 differing). `-fno-schedule-insns` and `-fno-strict-aliasing` are inert;
`-fno-schedule-insns2` gives 41.

The distinction: on `OvlFunc_886_20090c0` the local was **computed** from a base,
and its birth statement decided which register it got. Here the value is already
live in `r8` across the calls and every use rematerialises a copy out of it, so
what differs is where post-reload scheduling puts a register-to-register move —
and source position does not reach that.

## Negative: `ldrb` + `lsl #24` before a `cmp #0` is not reachable

`Func_801f730` tests a byte in a loop and the ROM spells it
`ldrb r3, [r2] / lsl r3, #24 / cmp r3, #0`. Ten shapes were compiled directly
under this tree's flags and none produces it: plain and signed `char *`, a cast
at the test, a `signed char` local, five bitfield widths and containers, a
`volatile` byte, and an explicit `(p[0] << 24) != 0`.

The last is decisive — gcc-2.96 knows `p[0]` is 0..255 and folds
`(x << 24) != 0` to `x != 0`, so the shift cannot be requested. Every signed
spelling instead reaches for `ldrsb`, which in Thumb-1 needs a register offset
and so costs a `mov rN, #0` first. Both are two instructions, so the length is
right and only the encoding differs.

**Do not spend screens hunting a narrowing spelling for a `!= 0` byte test.**
Probe the compiler once; if the fold happens, the shape is a property of the
source's TYPE somewhere upstream, not of the comparison.

## The merge lever CHAINS: one variable can be counter, result AND index

`Func_8005810` scans 16 slots, collects the free indices into a stack array, and
then returns one of them. The ROM keeps **r5** for the loop counter, then for
the result, then for a modulo result, then for the final array read. Four roles,
one register.

Merging them one at a time is measurable at each step:

| source | lines | differing |
|---|---|---|
| separate `i`, `r`, and an inline `v[Random() % cnt]` | 43 | 35 |
| drop the named base pointer (see below) | 43 | 21 |
| single exit instead of three `return`s | 42 | 21 |
| counter and result merged into one `unsigned int r` | 42 | **13** — first diff moves from instruction 3 to 30 |
| the modulo index merged into `r` as well | 43 | **match** |

The last step is the one worth remembering: `r = Random() % cnt; r = v[r];`
matches, while `idx = Random() % cnt; r = v[idx];` — naming the index in its own
local, which is the ROM's `mov r5, r0` read literally — is 26 differing, WORSE
than not naming it at all.

**So the merge lever is not a single swap.** When the ROM runs one register
through several unrelated values, try merging all of them into one variable, and
re-measure after each. And naming a value the ROM demonstrably holds is only
right if it goes into the variable that already owns that register.

## A named base pointer for a stack array costs a callee-saved register

The same function shows the inverse of "name the pointer". Written with an
explicit walking pointer over a stack array —

    int v[16]; int *q;
    q = v;
    ... *q++ = i;
    ... later, v[0] and v[idx]

— gcc materialises the frame address once into a callee-saved register and walks
with a *copy*: `mov r6, sp` at entry and `mov r0, r6` before the loop, with r6
held live across two calls. The ROM has neither; it re-derives `sp` at each of
the three sites (`mov r1, sp` for the loop, `ldr r5, [sp, #0]`, `mov r2, sp`).

Writing `v[cnt] = i;` instead and deleting `q` took 35 differing to 21 and
removed both moves. `v[cnt++] = i;` is byte-identical to the two-statement form.

**A stack array does not need a named base.** `sp` is always available, so gcc
re-derives it for free; naming it creates a pseudo that competes for r4-r7. This
is the opposite of the heap/global case, where naming the base is what produces
the ROM's single pool load — the discriminator is whether the base costs
anything to rematerialise.

## Reading the solved sibling first: DeleteActor matched on the first screen

`DeleteActor` shares a `.s` with `Actor_SetAnimAndSpeed`, which was elevated in
an earlier batch and has the identical opening — a null guard, then
`switch (*(unsigned char *)(e + 0x54) & 0xf)` with cases 1 and 2, case 2 walking
a four-pointer list backwards with `*list++`. Copying that structure and adding
the tail gave an exact match with no iteration at all.

The tail is `DMA3_CLEAR(e, 0x70)` from `include/dma.h`: the ROM's
`ldr r2, =0x8500001c` decodes as `0x85000000 | (size / 4)` with `size/4 = 0x1c`,
and the macro's own `u32 value` local is the function's otherwise-unexplained
`sub sp, #4`. **When a ROM tail is `stmia r3!, {r0, r1, r2} / sub r3, #0xc`,
decode the control word before writing anything — the macro already exists.**

## A ref built from a LINE RANGE can be short, and it lies quietly

`Func_8006088`'s reference was extracted with `sed -n '225,265p'` on the `.s`.
The function is longer than that range, so tryc compared against a truncated
reference and reported "rom 40 lines, ours 45" — a five-instruction excess that
does not exist. Two spellings were screened against that phantom before the
ref was rebuilt with

    awk '/^\.thumb_func_start NAME/,/^\.func_end NAME/' file.s

which gave "rom 45 lines" and inverted which spelling looked better.

**Never build a screening ref from a line range.** Use the function markers.
A short ref does not error; it just moves the length disagreement somewhere
plausible.

## Two defects can CANCEL, and then the length agrees for the wrong reason

On the corrected reference, `Func_8006088` screens two ways:

| bit extraction | lines | differing |
|---|---|---|
| `(sio >> 4) & 3` — three instructions, WRONG | 45 (exact) | 12 |
| `(sio << 26) >> 30` — two instructions, the ROM's | 44 | 21 |

The wrong spelling has the ROM's exact length and the better count, because its
extra instruction cancels a `mov r0, r2` that gcc does not emit anywhere in the
function. The correct spelling exposes the missing move and the one-line offset
cascades through everything after the join.

**A better differing count from a compensating pair of errors is not progress,
and length agreement is not evidence when two independent defects can cancel.**
Read what the ROM's instructions actually ARE for each region before trusting
either number.

## A counter the ROM masks ONCE is `unsigned int`, not `unsigned char`

`Func_80bf4c4` decrements a byte field twice and the ROM writes
`add r3, #0xf8` and `add r3, #0xff` — adding 248 and 255 — then masks to a byte
exactly once at the end with `lsl #24 / lsr #24`.

| local type | what gcc emits |
|---|---|
| `unsigned char` | a `lsl/lsr` truncation after EVERY store — two extra instructions per site |
| `int` | `sub r2, #8`, and a signed `ble` where the ROM has `bls` |
| `unsigned int`, written `v = v + 0xf8` | the ROM's `add #0xf8` and `bls`, no truncation |

So when the ROM accumulates in a wide register and masks once, the source's
variable is WIDE and the mask is an explicit cast at the point the ROM applies
it. The additive spelling matters too: `v - 8` on an `unsigned int` gives `sub`,
`v + 0xf8` gives the ROM's `add`.

## The copy-then-modify tell is PER SITE, and the instruction count decides

The same function has two decrement sites. The ROM spells them differently:

    site 1   add r3, #0xf8 / strb r3 / mov r2, r3            (three)
    site 2   mov r3, r2 / add r3, #0xff / strb r3 / mov r2, r3 (four)

Site 2 is the recorded copy-then-modify shape and `t = v + 0xff; *p = t; v = t;`
matches it instruction for instruction — 29 differing down from 38. Applying the
same spelling to site 1 OVERSHOOTS to 49 lines against 47, because site 1 is
three instructions: r3 already holds the value from the entry compare's
`mov r3, r2`, so there is no copy-in to ask for.

**Count the instructions at each site before applying the tell.** The shape
looks identical at both; the arithmetic does not.

## The birth-statement lever: scope narrowed by a second negative

`Func_80a3354` computes a loop's base address, and the ROM forms it AFTER
setting up the loop's stored value and counter while ours forms it immediately.
That is exactly the birth-statement shape, and neither moving the assignment
after `i = 3;` nor naming the offset first changes anything (54 lines, 27
differing, both).

With `OvlFunc_901_2008c1c` that is two negatives. The lever's demonstrated scope
is a pointer **computed from a base in straight-line code**, where its birth
statement decides which register it gets. It does not reach a pointer copied out
of a high register, and it does not reach a loop's base address.

## The gState offset build is worth 50 differences on its own

`Func_8095fcc` screened at 51 lines against 54 with 53 differing — a diff that
reads like a wrong function. The entire cause was one expression:
`*(int *)(gState + (0xfa << 1))` folds to a single pooled symbol,
`ldr r3, =gState+500`, where the ROM builds the offset and adds it:

    mov r1, #0xfa / lsl r1, #1 / add r3, r1

Naming the offset first — `off = 0xfa << 1; ... gState + off` — restored the
three instructions and took the function to **54 lines and 3 differing**, with
the whole body already exact underneath.

The rule was on record; what is new is the SIZE of the symptom. Three missing
instructions at the top shifted every subsequent line, so the screen reported a
near-total mismatch for a function that was three instructions from done.

**When a screen's differing count is close to its line count, check the first
few instructions for a folded symbol offset before reading any further.**

## One function can need BOTH register-offset operand orders

`Func_80bac6c` has three search loops over the same base. The ROM encodes two of
them offset-first and one base-first:

    loops 1 and 3   ldrsh r3, [r2, r5]   r2 = offset, r5 = base
    loop 2          ldrsh r3, [r0, r2]   r0 = base,   r2 = offset

The recorded rule — `base + off` gives base-first, `off + (int)base` gives
offset-first — applies per access, not per function. Writing loops 1 and 3 as
`*(short *)(off + (int)g)` and leaving loop 2 as `q + off` moved the first
difference from instruction 16 to 24 and took 36 differing to 34.

**Do not pick one spelling for a whole function.** Read the operand order at
each site; a function that mixes them is not evidence that the rule is wrong.

## A counter-example to the `goto`-loop lever, and what separates the cases

The `goto`-loop lever is recorded as taking `Func_8090584` from 95 differing to
3 where no flag helped. `Func_80bac6c` is the opposite: its loop 2 keeps an
index and rebuilds `i * 2 + 0x64` every iteration while gcc strength-reduces to
a walking pointer — exactly the shape the lever is for — and the rewrite is
**much worse**, 67 lines against 62 and 51 differing, applied to that loop
alone.

Flags do not help either: `-fno-strength-reduce` is 65 lines and 51 differing,
and `-fno-move-all-movables` is byte-identical to no flag at all.

The distinguishing feature is how much the loop hoists. `Func_8090584` had a
pointer, two mask constants, a base address and three store values hoisted out;
the `goto` rewrite recovered all of them at once and its own cost was small
against that. Here exactly one constant and one induction variable are involved,
and the rewrite's overhead — five instructions — exceeds what it recovers.

**Count what the ROM rebuilds inside the loop before reaching for the rewrite.**
One induction variable is not enough to pay for it.

## SELECTION IS THE LEVER: rank by similarity to a SOLVED function

Two consecutive rounds produced zero elevations. Both picked candidates on size
and call count, and both landed on register-allocation and scheduling walls.
The rounds that produced elevations had something else in common, and it was not
the size of the function: `DeleteActor` matched on the first screen because its
own `.s` already held the solved `Actor_SetAnimAndSpeed` with the same opening.

**The exact-skeleton tools are exhausted, and that is now measured.**
`tools/match_shapes.py` reports 0 leads, and `--near 1`, `--near 2` and
`--near 3` all report 0 as well; `tools/solved_twins.py` reports 0 across 0
templates. Every remaining function that is a constants-only variant of an
elevated one has been taken.

But "no exact twin" is not "no usable template". `tools/fuzzy_solved.py` ranks
every remaining function by the best difflib ratio between its skeleton and any
solved function's, bucketed by length with a mnemonic-overlap prefilter so the
quadratic part stays small. It reports **26 leads at ratio >= 0.80**.

The first three were taken in one round and **all three matched on the FIRST
screen with no iteration at all**:

| function | ratio | exemplar |
|---|---|---|
| `TextBox` | 0.986 | `DialogueBox`, same `.s` stem — same body, one argument differs |
| `OvlFunc_924_2009bf0` | 0.977 | `OvlFunc_924_2009420`, same overlay — same cutscene, different slot and constants |
| `OvlFunc_926_2008484` | 0.971 | `OvlFunc_939_2008764`, another overlay — including its recorded two-spellings-for-two-increments detail |

Compare that against the two dry rounds: eight functions screened, every one
reaching exact length, none matching.

> **A ratio above 0.95 is worth more than any tractability heuristic.** Size,
> call count and branch count predict how hard a function is to READ. Similarity
> to a solved function predicts whether the answer already exists.

The ratio is a lead, not a proof — the exemplar's `.c` still has to be read, and
the third of these needed its branch polarity inverted against the exemplar's.
But the reading is minutes rather than a round.

**Run `tools/fuzzy_solved.py` before any other candidate scanner.**

## `x <<= 16; x >>= 2;` is not the same spelling as `(short)x << 14`

`Func_80782a0` stores a clamped value as a halfword and then uses it shifted.
The ROM does the widening in place, after the store:

    strh r0, [r5, #0x38] / lsl r0, #0x10 / ... / asr r0, #0x2

Written as a cast — `r0 = (short)r0 << 14;` — gcc computes the shift into a
SECOND register and hoists it above the store: `lsl r3, r0, #0x10 / strh r0,... /
asr r0, r3, #0x2`, five differing at exact length. `-fno-schedule-insns2` takes
that to four and still does not close it.

Writing the two shifts as two statements on an `int`

    r0 <<= 16;
    r0 >>= 2;

matches outright. The cast asks for a value; the shift pair asks for an
operation on a register, and only the second gets the in-place form.

**When the ROM widens a just-stored halfword with a shift PAIR, write two shift
statements.** A cast to `short` is the natural reading and it is the wrong one.

## `-fno-schedule-insns2`: read the SIGNATURE, not the flag

The recorded warning is that this flag is "an actively misleading probe", from a
round where on every function it moved the first differing position back to ~1
and multiplied the count (3 → 15, 17 → 41, 8 → 25, 2 → 23).

`Func_8078320` is the opposite case and it is worth naming, because the warning
as written would have stopped the elevation. Its residue was three lines — a
`strh` and the `ldrsh` after it in the wrong order — and the flag closed it
**outright**, with no other line disturbed and no change to where the diff
began. `SCHED2_CFLAGS` already exists for exactly this, on one prior function.

So the discriminator is the shape of what the flag does, not the flag:

* first difference jumps back toward instruction 1 and the count multiplies →
  the flag is destroying the evidence; put it down.
* the residue closes or shrinks with nothing else moving → it is the answer,
  and post-reload scheduling really was the difference.

The sibling `Func_80782a0` shows the other half of the rule: the same flag on a
similar-looking residue took 5 differing to 4 and did NOT close it, and the real
fix was a source spelling. **A flag that improves but does not close a small
residue is still the wrong answer.**

## Solved-sibling pairs come nearly free, and `fuzzy_solved.py` finds them

`Func_80782a0` and `Func_8078320` sit in one `.s`, share a solved exemplar
(`UpdateStatBarPercent`), and appeared in `tools/fuzzy_solved.py` as two
separate leads at 0.906 and 0.897. Solving the first gave the second for two
edits — the clamp's field offsets and the forwarded store.

That is the second round in a row where the fuzzy ranking produced elevations at
one or two screens each, against two prior rounds that produced none. Six of the
seven elevations across those two rounds came off its list.

## The `goto`-loop lever, and what actually separates it from its counter-example

`Func_80aac84` is the clearest instance yet, and it settles the discriminator
that `Func_80bac6c`'s counter-example left open.

Its inner loop screened at 74 lines against 77 with **71 differing** — gcc had
both REVERSED the loop (counting down from 15) and STRENGTH-REDUCED the address,
where the ROM counts up from 0, keeps the base in r14 and recomputes
`base + j` and then `idx * 2` every iteration. Rewriting both loops with `goto`
took it to **77 lines and 2 differing**, and a statement-order swap closed it.

Set against `Func_80bac6c`, where the same rewrite cost five instructions and
went from 34 differing to 51:

| | hoisted / transformed by gcc | goto rewrite |
|---|---|---|
| `Func_8090584` | a pointer, two masks, a base, three store values | 95 → 3 |
| `Func_80aac84` | loop reversal AND strength reduction of the address | 71 → 2 |
| `Func_80bac6c` | one constant, one induction variable | 34 → 51 |

**The rewrite pays when gcc has applied a loop TRANSFORMATION — reversal or
strength reduction — not merely hoisted one value.** Reversal and strength
reduction restructure the whole body, so disabling loop optimisation recovers
many instructions at once; a single hoisted constant is cheaper to live with
than the rewrite's own overhead.

The corollary from the original note held here too: with hoisting off, the ROM's
`mov r7, #0x1f` before the outer loop is an explicit `int mask = 31;` in the
source, not gcc lifting `& 31` out.

And once the loops are `goto` loops, **the order of the two initialisers is
observable**: `mov r6, #0x0` (the counter) before `mov r14, r3` (the base) is
`j = 0;` written before `base = row << 4;`. Reversed, it is 2 differing.

## The per-use-site-locals lever needs a DOMINATING BOUNDARY, like everything else

`OvlFunc_948_200938c` and `_200949c` each call `__MapActor_SetSpeed` twice with
the same pooled pair. The ROM reloads both constants at each site; gcc hoists
them into r5/r6, which shows in the prologue as `push {r5, r6, r14}` against the
ROM's `push {r14}`.

The recorded remedy is separate locals per use site, with a worked instance in
`src/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_b.c` — six locals for three calls.
Copying that spelling here is **exactly inert**, byte for byte identical to the
plain literals, and `-fno-rerun-cse-after-loop`, `-fno-gcse` and
`-fno-cse-follow-jumps` are all inert too.

The difference is where the guards are. In the working instance three
`if (a != 0)` blocks sit BETWEEN the assignments and the uses. In these two the
only guard is AFTER both calls, so assignments and uses share one straight-line
region.

**So "separate locals per use site" is the dominating-block mechanism under
another name, not a property of having several names.** It joins the argument
interleave and the constant-CSE levers in needing a branch to rematerialise
across, and the check is the same one: is there a boundary between the
assignment and the use?

## `split_s.py` refusing on local labels is a two-step, and the steps must stay separable

Splitting `ovl_314_c_c_c.s` was refused: two `.L` labels would have crossed
files, and a local label does not survive into the object's symbol table, so the
link would have failed. The tool names the labels and the fix.

The discipline that matters is its last line — export FIRST, verify
`make compare`, and only then split. A `.global` emits no bytes, so the export
is provably byte-neutral on its own; done together with the split, a layout
mistake and a bad export are indistinguishable at the end.

## Why container builds must pass `AGBCC_DIR=/opt/agbcc`

`AGBCC_DIR` looks like it selects the compiler and does not. It is referenced
only by `src/lib/m4a/%.o` and the three `src/lib/agb_flash/*.o` rules
(Makefile:5583-5614), which build those library objects with `old_agbcc`. Every
elevated `.c` in this tree compiles with `$(GCC296_CC)` — gcc-2.96 — and no
elevation has ever gone through agbcc.

The override is still required, and the reason is not obvious from the
Makefile. Its default is `AGBCC_DIR ?= tools/agbcc`, and that path DOES exist
inside the container because the repo is mounted there — but:

    tools/agbcc/bin/old_agbcc   Mach-O 64-bit executable x86_64      (host)
    /opt/agbcc/bin/old_agbcc    ELF 64-bit LSB pie executable        (image)

The checked-in copy is the macOS build. Running `make` in the container without
the override reaches it, the shell cannot exec a Mach-O binary and falls through
to parsing it as a script, and the build dies with

    tools/agbcc/bin/old_agbcc: 1: Syntax error: "(" unexpected

which names the compiler and looks like a corrupted toolchain rather than a
wrong path. So: **build in the container with `AGBCC_DIR=/opt/agbcc`, and know
that it feeds five library objects only.** If those objects are already current
the variable never gets used, which is why an incremental build appears to work
without it.

## The duplicate-constant hoist has a THIRD presentation: the PRE-SHIFT base

Two parks this batch showed gcc commoning a repeated argument value. A third,
`OvlFunc_959_200a06c`, shows it commoning something the earlier descriptions
would not catch.

Its three `__MapActor_SetPos` calls pass `0xac<<18, 0xb0<<15`, `0xb0<<18,
0xb0<<15`, `0xb4<<18, 0xc0<<15`. gcc does not hoist a shifted value — it hoists
`mov r5, #0xb0`, the eight-bit immediate underneath two *different* shifted
results, and shifts copies of it at each site. The ROM writes `mov r1, #0xb0`
and `mov r2, #0xb0` separately.

So the repeated thing need not be an argument, or even a value that appears in
the ROM twice. **Check for a repeated 8-bit immediate across argument sites, not
just a repeated argument.** All three specimens were found only from the
prologue after screening, which is a poor way to learn it.

## A `// fakematch` exemplar must not be copied, and the tool now says so

`tools/fuzzy_solved.py` offered `OvlFunc_925_200aeb8` as the exemplar for
`OvlFunc_959_200a06c` at ratio 0.865. That file is marked `// fakematch` and
gets its match by pinning thirteen locals with
`__asm__ volatile ("" : "+r" (x))`.

Copying it would have produced a match and propagated the hack into a target
that needs nothing of the kind — the target is a nine-call straight-line
cutscene. The tool now tags such exemplars `<- FAKEMATCH, do not copy`.

**A high ratio against a fakematch is worse than no lead at all**, because the
spelling it suggests will work.

## `(unsigned short)(v - 1) <= 1` is not `unsigned short w = v - 1; w <= 1`

`OvlFunc_922_20095dc` guards on a halfword minus one. The ROM subtracts as a
full word and narrows only at the comparison:

    ldrh r3, [r3] / sub r3, #1 / lsl r3, #16 / mov r2, #0x80 / lsl r2, #9 / cmp r3, r2

Declaring the intermediate `unsigned short` makes gcc materialise the wrap
instead — `ldr r2, =0xffff / add r3, r2`, plus a separate `add r2, #1` for the
bound — three differing at exact length. Keeping the value an `int` and casting
inside the condition gives the ROM's `sub` and its `lsl #16` against `1 << 16`.

Same family as the `x <<= 16; x >>= 2;` reading: **a narrow TYPE asks gcc to
produce a narrow value; a cast inside a comparison only asks it to compare
narrowly.** The ROM's `lsl #16` immediately before a `cmp` is the second form.

## NAMING a repeated constant can be worse than leaving it inline

`OvlFunc_927_200a078` passes `0xc0 << 11` to two calls, and the ROM builds it
once into r8 and holds it across the intervening work — the exact shape that
usually means a named local, and its solved exemplar
(`OvlFunc_927_2009ef0`) does name it.

Here naming it is 2 differing at exact length, and the difference is
argument-setup order:

    rom    mov r2, #0x86 / mov r3, #0xc0 / lsl r3, #11
    ours   mov r3, #0xc0 / mov r2, #0x86 / lsl r3, #11

Writing the literal at BOTH call sites matches outright. gcc's own CSE produces
the single build in r8 by itself; the named local additionally fixes when that
build happens, and it fixes it wrongly. Moving the assignment earlier is much
worse again (125 lines, 122 differing, both placements tried).

**So a constant held in a callee-saved register across calls is not by itself
evidence for a named local.** gcc will common two plain literals into exactly
that. Name it only when the plain spelling fails — and note this is the inverse
of the duplicate-constant parks in this same batch, where gcc's hoist is what
has to be prevented. Same mechanism; whether it is right depends on the ROM.

## `tools/fuzzy_solved.py` now warns before the C is written

Two failure modes cost rounds and both are now flagged on the lead list.

**FAKEMATCH exemplars.** Some elevated files match only by pinning locals with
`__asm__ volatile ("" : "+r" (x))` and are labelled `// fakematch`. Copying one
works, which is the problem — it propagates the hack into a target that may not
need it. Two of eleven current leads carry such an exemplar.

**DUP-CONST targets.** Batch 169 lost three functions to the duplicate-constant
hoist, and `tools/filtered.py` already computed exactly the notion of
"expensive constant" that predicts it. The detector reuses that function
verbatim rather than reimplementing it, reading the real instruction text —
`match_shapes` collapses every immediate to one letter, so the skeleton the
ranking runs on cannot see this at all. Three of eleven current leads are
flagged.

A flagged lead is not worthless; the rest of the function may still be exact.
But expect a park unless there is a guard between the repeats.

## Two functions, one exemplar, two-line residues pointing OPPOSITE ways

`OvlFunc_928_2008d0c` and `OvlFunc_957_2008de8` are both variants of the solved
`OvlFunc_946_2009a44`, both come out at the ROM's exact length, and both sit at
exactly **2 differing** — on the placement of one constant build relative to a
neighbouring memory operation.

    2008d0c   rom  mov r3, #0x80 / ldrh r1, [r5, #6]     constant EARLIER
              ours ldrh r1, [r5, #6] / mov r3, #0x80

    2008de8   rom  str r3, [r6, #8] / mov r1, #0xf0      constant LATER
              ours mov r1, #0xf0    / str r3, [r6, #8]

Between them, six spellings were measured — operands swapped in the sum, the
constant named in a local, the mask named, the expression written inline in the
call, the halfword read named first, and the whole statement moved above the
vector stores. Every one is inert or worse, in both functions.

**The pairing is the finding.** If a source construct controlled where an
independent constant build lands, one of the two would have yielded to it —
they want opposite things and the same spellings were available to both. So
this is the scheduler, not a missing lever, and a two-line residue of this shape
is worth one probe and then a park.

Note also that `2008d0c`'s exemplar contains the identical angle expression and
matched with it. The spelling is not the variable; the surrounding register
pressure is.

## Loop initialiser order, confirmed a second time

`Func_80aac84` (batch 168) established that once a loop's initialisers are both
visible, their ORDER is observable in the output. `Func_80a9b94` confirms it on
a plain `do`/`while` with no `goto` rewrite involved:

    rom    mov r5, #0x0 / add r6, #0x48        counter first
    ours   add r6, #0x48 / mov r5, #0x0        pointer first

Swapping the two source statements — `i = 0;` before `q = ...;` — is the whole
difference between 2 differing and an exact match, on a 33-line function that
was otherwise correct on the first screen.

So this is not a `goto`-loop phenomenon. **When a diff is two lines and both are
loop setup, try the other order before anything else.** It costs one screen.

## A global read must be NAMED to keep its load above a following branch

`Func_80a1cb0` selects a constant with an `if`, then walks a list based at a
global. The ROM loads and dereferences the global BEFORE the branch and keeps
the value across it:

    ldr r3, =0x3001f2c / mov r2, #0x38 / ldr r3, [r3] / mov r8, r2
    cmp r0, #1 / beq L0 / mov r2, #0x28 / mov r8, r2
    L0: mov r5, r3 / add r5, #0x48

Written with the global used only after the branch — `k = 0x38; if (...) k =
0x28; p = iwram_3001f2c + 0x48;` — gcc sinks the whole load below the branch,
which is 12 differing at exact length.

Reading it into a named local first, and forming the pointer afterwards:

    s = iwram_3001f2c;
    k = 0x38;
    if (mode != 1) k = 0x28;
    p = s + 0x48;

matches. The name is what pins the load above the branch; the `+ 0x48` still
happens after it, exactly as the ROM has it.

**This is the read counterpart of the address-local birth rule.** For a store
the question is where the address is COMPUTED; for a global read it is where the
value is FETCHED, and a branch between the fetch and the use is what makes the
difference observable.

## The fuzzy lead pool: what the tags are actually filtering

At ratio >= 0.70 there are 26 leads and exactly ONE is fully clean — the rest
carry DUP-CONST, FAKEMATCH, or both. That is the tags earning their keep rather
than the pool being empty: dropping the threshold to 0.66 surfaces five more
clean leads, and they still close in one or two screens.

So when the clean list empties, **lower the ratio before changing method**. A
0.70 lead that is clean has been more productive than a 0.90 lead that is
flagged.

## A reassigned local sometimes has to be SPLIT, not merged

The merge lever says one register running through two unrelated values means one
variable. `OvlFunc_882_20090a4` is the counter-case, and the discriminator is
the push list.

Its ROM runs r5 through `0x35` and then `0x36` — the merge lever's usual shape.
Written as one reassigned local it is 78 lines against 80 with 64 differing;
written as two separate locals it is **80 lines and 8**. gcc coalesces the
single variable's two live ranges into one register and then needs one FEWER
callee-saved register than the ROM, and the two missing lines are that
register's save and restore.

**So when the ROM spends a callee-saved register the source has to force, two
variables can be what creates the demand.** The merge lever still holds when the
register count already agrees; check the prologue first. A push the ROM has and
we lack is the signal to split, exactly as a push we have and the ROM lacks is
the signal to merge.

That function also needed `-ffixed-r7`: the ROM holds four stack-argument
constants in r5, r6, r8 and r10, skipping r7, and gcc's `REG_ALLOC_ORDER`
reaches r7 before r8. Reserving it fixed the prologue outright and moved the
first difference from instruction 0 to instruction 1.

## Initialiser order, third instance -- promote it to a first check

`Func_80a9cf8` came in at the ROM's exact length with 5 differing and closed in
two steps, both of them ordering:

    p = iwram_3001f2c;  n = 0xa8;  i = 0;  q = p + 0xc8;      2 differing
    p = iwram_3001f2c;  i = 0;  n = 0xa8;  q = p + 0xc8;      MATCH

That is three functions now:

| function | shape | what moved |
|---|---|---|
| `Func_80aac84` | `goto` loop | counter before base |
| `Func_80a9b94` | plain `do`/`while` | counter before pointer |
| `Func_80a9cf8` | plain `do`/`while` | counter before the held constant |

It is not a `goto`-loop phenomenon and it is not specific to pointers. **When a
small residue is confined to a loop's setup block, permute the initialisers
before reaching for any lever.** Each permutation is one screen, and there are
usually only two or three worth trying.

`Func_80a9cf8` also needed the global read NAMED before the pointer was derived
from it — `p = iwram_3001f2c; ... q = p + 0xc8;` rather than
`q = iwram_3001f2c + 0xc8;` in one expression. That took it from 9 differing to
2, and it is the same rule `Func_80a1cb0` established: the name pins the LOAD,
and the displacement still happens where the source puts it.

## Elevations compound: today's match is tomorrow's exemplar

`Func_80a68a8` was elevated one round ago off `tools/fuzzy_solved.py`. This
round it appeared as the EXEMPLAR for `Func_80a3d24` at ratio 0.710, and that
function matched on the first screen with no iteration.

That is worth naming because it changes how the lead list behaves. The solved
corpus grows every round, so a ratio computed today is not the ratio that will
be computed next round — functions that were below threshold can rise as their
nearest neighbour gets solved. **Re-run the ranking every round rather than
working from a saved list**, and do not write off a function because it ranked
poorly before its family had a member.

## The `goto`-loop lever, fourth instance -- and it fits the recorded rule

`Func_80a9d3c` walks a sprite array and tests a parallel flags byte with the
loop counter. gcc strength-reduced the flags access into its own walking pointer
with a precomputed end address (`add r3, r5, #4`), where the ROM keeps the
counter and indexes `ldrb r3, [r2, r6]` with it. Rewriting the loop with `goto`
matched.

That is the batch-170 discriminator behaving exactly as stated: **the rewrite
pays when gcc applied a loop TRANSFORMATION -- reversal or strength reduction --
not merely a hoist.** Here it was strength reduction of a second induction
variable, and the rewrite recovered it.

The same function also needed an `int` intermediate for a halfword store of `8`:
`*(unsigned short *)(x + 6) = 8;` POOLS the constant (`ldr r3, =0x8`) where the
ROM has `mov r3, #0x8`. The narrowed HImode rule says only `0` and values
>= 0x8000 need the local; this is a third exception to that, alongside the two
already recorded, so **check the halfword exception on any small constant stored
through a `short *`, not just on 0.**

## Changing three things at once cost a round's worth of signal

`Func_80df9d0` screened at 5 differing. Three fixes were obvious from the diff,
all three were applied together, and the result was **9** — worse. Measured
individually afterwards:

    named loop bound      11   worse
    offset-first pointer   4   better
    split load/store       3   better
    the two good ones      2

The recorded rule "change ONE thing at a time, or you will discard three correct
fixes" has a second failure mode worth naming: applied together, a change that
HELPS and a change that HURTS cancel, and the combined number tells you nothing
about either. A batch of three that scores worse than the baseline is not
evidence that all three are wrong.

## SCRATCH-REGISTER SELECTION is a distinct wall, and it is now three deep

Three functions parked in one batch share a residue that none of the recorded
levers touches: they reach the ROM's exact length AND its exact instruction
sequence, and differ only in which of r1/r2/r3 carries a value.

| function | lines | differing | shape |
|---|---|---|---|
| `OvlFunc_882_20090a4` | 80 of 80 | 8 | `mov r2, #0xf` against `mov r3, #0xf` |
| `OvlFunc_968_200c968` | 90 of 90 | 22 | struct address built in r2 against r7 |
| `SystemMsgBox` | 79 of 79 | 23 | the zero and the -9 one register across |

This is worth separating from the register-ROLE swap (which is about
callee-saved allocation and shows in the prologue) and from scheduling (which
moves instructions). Here the prologue matches, the order matches, and the
work matches.

**What does NOT reach it, measured across the three:** naming a value, naming a
global's address, naming a struct pointer, reordering the statements that
produce the operands, operand order within an expression, and every flag group
tried. Those levers change what is computed or when; none of them changes which
scratch register receives the result.

**Recognising it early is the practical value.** When a screen is at exact
length with the instruction sequence aligned and the diff is a column of
`mov rN` against `mov rM`, stop. One probe, then park -- the three above cost
between four and seven screens each to arrive at the same place.

## The fuzzy lead pool is exhausted at 0.60 -- what the next round should do

`tools/fuzzy_solved.py` at `--min-ratio 0.60` now returns **two** clean leads,
and both were worked this round and parked. Everything else on the list carries
DUP-CONST or FAKEMATCH.

That is not the tool failing; it is the tool having done its job. Batches
167-171 took **eighteen** elevations off it, most at one to three screens each,
and the compounding effect means the ranking still improves every round as
exemplars land. It should keep being re-run.

But it can no longer carry a round on its own. The pools that remain, from
`tools/census.py`:

* **`multi`, 655 functions** -- excluded from the worklist only because their
  blocker is unknown until a split, and a split is byte-neutral by construction.
  Batches 164-166 took roughly a dozen elevations out of it. This is the obvious
  next place.
* **`branch-over-pool`, 495** -- of which ~277 have the pool as their only
  predicted blocker; mostly large.
* **`open`, 270** -- the nominal worklist.

**Start the next round with a `multi` scan, not with `fuzzy_solved.py`.** Run
the ranking as well, since it is cheap and still rising, but do not expect it to
supply the round.

## The narrowing-shift fold is about the FOLD, not about `signed char`

`Func_801f730` was parked on `ldrb` + `lsl #24` + `cmp #0`, with ten spellings
probed directly against gcc-2.96 and none producing the shift. The decisive
probe was `(p[0] << 24) != 0`, which gcc folds to `p[0] != 0` because it knows
the loaded value's range.

`Func_80788c4` shows the same wall one width up: `ldrh` + `lsl #16` + `cmp #0`,
in a compaction loop that is otherwise exact at the ROM's 65 lines.

So the earlier park should not be read as something about `signed char` or about
byte loads. **Any narrowing shift placed before a zero test is unreachable,
because constant-range folding removes it before the shift can be emitted.**
Recognise it from the ROM side -- a `lsl #24` or `lsl #16` whose result is only
compared against zero -- and do not re-probe; the byte case's ten measurements
cover the halfword case by the same argument.

### CORRECTION: unreachable only when gcc can RANGE-ANALYSE the value

The sentence above is too broad, and `Func_80bf2b4` is the counter-example. Its
ROM has `add r3, #0xff / strb r3, [r5] / lsl r3, #0x18 / cmp r3, #0`, and
`(unsigned char)t == 0` on an `unsigned int t` produces the `lsl #0x18` on the
FIRST screen.

The difference is what produced the value:

| the tested value is | gcc knows | the shift |
|---|---|---|
| a freshly loaded byte or halfword | range 0..255 / 0..65535 | folded away, unreachable |
| arithmetic that widens past the type, e.g. `v + 0xff` | range 0xff..0x1fe | REAL, and emitted |

In the second case the narrowing genuinely changes the answer -- `(u8)t == 0` is
not `t == 0` -- so gcc has to keep it.

**Look at what produced the value, not at the shift.** If it came straight from
a load of the same width the shift is unreachable; if arithmetic widened it,
`(unsigned char)x == 0` is the spelling and it works on the first screen.

## A run of constant-offset stores: gcc keeps ONE offset, some ROMs keep two

`Func_80bb588` zeroes 24 contiguous bytes, fully unrolled in the ROM. Written as
24 explicit `u[K] = 0;` statements the instruction COUNT is exact -- 98 against
98 -- and the whole residue is the address bookkeeping between stores:

    rom    add r2,r1,r0 / strb / add r0,#2 / add r2,r1,r4 / strb / add r4,#2 ...
    ours   add r2,r1,r0 / strb / add r0,#1 / add r2,r1,r0 / strb / add r0,#1 ...

Two offset registers each stepping by 2, alternating, against one stepping by 1.

That pattern is the signature of a strength-reduced loop over a stride-2 pair,
so the loop forms are the obvious hypothesis. **They are wrong, and it is worth
recording so nobody re-derives it:**

| source | lines (rom 98) | differing |
|---|---|---|
| 24 explicit `u[K] = 0;` | **98** | 56 |
| two alternating named offset locals | 72 | 94 |
| `for (i = 0; i < 12; i++) { u[0x131+i*2]=0; u[0x132+i*2]=0; }` | 34 | 94 |
| the same loop with `-funroll-loops` | 75 | 89 |

Unrolling gives 75 lines, not 98, so the ROM is not an unrolled loop. And named
offset locals are worse than literals, because both values are compile-time
known and gcc folds them back into immediates -- the same fold recorded for
`OvlFunc_882_20090a4`'s bound, arriving from the other direction.

**So the explicit form is right and the bookkeeping is not reachable through
it.** When a run of constant-offset stores matches on count but not on offset
registers, do not spend screens on loop shapes; the count agreeing is the signal
that the statement form is already correct.

## Two leads with the SAME ratio and the SAME exemplar are twins of each other

`tools/fuzzy_solved.py` listed `OvlFunc_915_2008aac` and `OvlFunc_913_2008b1c`
adjacently, both at ratio **0.612** against the same exemplar. That is not a
coincidence to skim past: an identical ratio against an identical exemplar means
the two targets have the same skeleton as each other.

They turned out to be shape-identical across two DIFFERENT overlays --
108 instructions each, differing only in the function name and its labels. The
first cost one screen plus a branch-polarity flip; the second was `sed
s/name1/name2/` on the finished file and matched on the first screen.

**Read the ratio column for repeats before picking.** Equal ratios with a shared
exemplar are a free second elevation, and the pair may live in different
directories where neither a same-`.s` nor a same-overlay heuristic would find
them. `tools/twin_families.py` groups by exact opcode stream and would also have
caught this one; the point is that the ranking already shows it, at no extra
cost, if the column is read as data rather than as an ordering.

## Branch polarity, confirmed again: the success arm is the `if` BODY

`OvlFunc_915_2008aac` screened at 78 differing of 108 with the whole residue
downstream of one branch. The ROM's `bne` skips FORWARD to the `return 0`, so
the cutscene is the fall-through and therefore the `if` body:

    if (r != 0) return 0;  <cutscene>          78 differing
    if (r == 0) { <cutscene> return 1; } return 0;   MATCH

That is the third function closed by this reading. The tell is in the ROM and
costs nothing to check: **which way does the conditional branch jump?** Forward
over the long block means the long block is the body.

## A family is only a two-for-one if its SHAPE is reachable

`tools/twin_families.py` offered `OvlFunc_954_2008974` / `OvlFunc_956_2008c5c`
as a fully-unparked 61-instruction pair -- the ideal-looking lead. It came out
at the ROM's exact 63 lines and 29 differing on the duplicate-constant hoist,
which batch 169 had already measured as unreachable in straight-line code.

The list had no warning, while `tools/fuzzy_solved.py` has carried exactly that
check since batch 169. Both lists now do, sharing
`filtered.expensive_constants`. Running it immediately flags several of the top
families, including two of the three-member ones.

**Two members multiply whatever the shape costs, in both directions.** A family
whose shape is blocked is not a two-for-one, it is two parks -- so the
reachability checks belong on the family list at least as much as on the
ranking.

## Initialiser order, FOURTH instance

`Func_8096cdc` came in at the ROM's exact length with 2 differing, and closed on
`i = 0;` before `pw = &ewram_200048a;` rather than after. That is the same
reading as `Func_80aac84`, `Func_80a9b94` and `Func_80a9cf8`, now across a
`goto` loop, two plain `do`/`while` loops, and here a `do`/`while` whose other
initialiser is a global's ADDRESS rather than a pointer or a constant.

Four for four, and every one of them was two lines from a match. It stays a
first check.

## A named constant cannot create register pressure

Three functions this batch tried to reproduce a ROM that spends a register on a
literal, by naming that literal in a local. All three failed the same way, and
the failure is worth stating once rather than rediscovering:

| function | the ROM holds | naming it gave |
|---|---|---|
| `OvlFunc_882_20090a4` | a loop bound in a callee-saved register | 11 differing, worse than the literal's 5 |
| `Func_80bb588` | two offset registers stepping by 2 | 72 lines against 98 -- folded to immediates |
| `Func_8019944` | a zero parked in r12 across the loop | no change; folded at both stores |

Constant folding runs before register allocation, so by the time allocation
happens a named compile-time constant and a literal are the same RTL. **A ROM
that spends a register on a literal is not asking for a local** -- it is showing
you an allocation decision the source has no vocabulary for, which is the same
conclusion the scratch-register and callee-saved-copy walls reach from their own
directions.

The corollary is useful in the other direction: when naming a value DOES change
the output, the value was not compile-time known -- so the lever is really about
blocking a fold or pinning a live range, never about "asking for a register".

## The epilogue tell, refined: it names the DECLARATION, not a value

**This is a correction to how batch 172 first reported it.** The epilogue rule
was already recorded twice -- "Tell: `pop {r1}` in a function that looks void
names a RETURN VALUE" (batch 46) and "The epilogue register tells you the return
type" (`OvlFunc_971_20091bc`). Batch 172 presented it as new; it is not, and the
batch-172 report and its HANDOFF entry have been corrected.

What `Func_80b6378` does add is a case neither earlier entry covers. Both of
them explain the r1 epilogue as the function returning *the value the last call
left in r0*, and both fix it by writing an explicit `return f(...)`. This
function has no `return` statement, no trailing call, and no value to return --
its body ends in a store inside a loop. Changing `void` to `int` and adding
nothing else matched.

So the mechanism is one step earlier than the earlier entries state. gcc marks
r0 live at exit from the **declared return type alone**, whether or not any
value ever reaches it, and the epilogue scratch register falls to r1 as a
consequence. The tell therefore reads:

    pop {r0} / bx r0    ->  declared void
    pop {r1} / bx r1    ->  declared non-void

and NOT "returns something." When the earlier entries' fix -- writing
`return <call>;` -- has no candidate call to apply to, the declaration alone is
still the whole lever.

Also worth keeping from those entries: a two-line epilogue residue looks exactly
like the scratch-register-selection wall named in batch 171. **Check the
epilogue pair before adding a function to that park class.**

## An index into a pointer must be NAMED to stay an index -- correcting "offset first"

Batches 170 and 171 recorded the offset-first lever as being about *operand
order*. That is wrong, and `Func_80b6378` measures it:

| spelling | result vs a 27-line ROM |
|---|---|
| `*(p + buf[i] + 0x48) = v;`, `char *p` | 28 lines; base folded in first |
| `*(char *)(buf[i] + 0x48 + p)`, `int p` | 28 lines, byte-identical to the above |
| `k = buf[i] + 0x48; p[k] = v;` | **matches** |

Operand order is inert because `+` is commutative and gcc reassociates before it
selects addressing modes. What produces the ROM's `strb r3, [r6, r2]` is giving
the index a NAME, which forces the sum to exist as one quantity before the
memory reference is formed -- and therefore to occupy an index register.

**The general form, which subsumes the batch-170 global-read rule:**

> Naming a value does not tell gcc where to *put* it. It tells gcc the value
> must *exist*. A name is a materialisation point, not a placement hint.

That is why naming a global's value pins its load above a branch (batch 170),
why naming an index pins a sum ahead of an address computation (here), and why
naming a compile-time constant does nothing at all (the constant-folding entry
above) -- in the last case the value already existed.

## A reassigned accumulator may need a SECOND variable

`Func_8098184` came out one instruction short, the ROM's extra being a bare copy
inside the loop:

    rom    add r3, r2, r1        ours   add r2, r1
    rom    mov r2, r3

The in-place `while (v <= lim) v += step;` will not produce it. Two variables,
with the post-loop stores reading the *destination*, will:

    do { w = v + step; v = w; } while (w <= lim);
    *(int *)(a + 0x18) = w;

gcc does not coalesce the copy because the live ranges genuinely differ -- `v` is
dead at the loop exit and `w` is not.

This is batch 171's split-versus-merge discriminator inside a loop body, and its
tell is different. Batch 171's tell is the **push list**, which is silent here
because both registers are caller-saved and neither is pushed. **The loop-body
tell is a bare `mov rN, rM` between the update and the test.** Both point the
same way from different evidence: a copy the ROM makes and we do not means the
source had two names where we wrote one.

## A stale object can make a wrong elevation look green

The tree compiles `src/<path>.c` -> `asm/<path>.s` -> `asm/<path>.o`, and it is
the **`.o` that the linker script names**. When a hand-written `asm/<path>.s` is
replaced by an elevated `src/<path>.c`, the old `.o` remains and `make` has no
reason to rebuild it. Two of batch 172's elevations built green incrementally
while their `.o` was still the one assembled from the hand-written asm that had
just been deleted -- the C had never been compiled at all.

The already-recorded rule "check every address against the linked ELF" does NOT
catch this: the symbol is at the right address either way.

> **After removing a hand-written `.s`, an elevation is not proven until a
> `make clean` build, and the proof is the `.gcc2_compiled.` local symbol at the
> function's address in the linked ELF** -- that marker is what distinguishes a
> compiled translation unit from an assembled one.

The visible symptom, if you are looking for it, is a missing generated
`asm/<path>.s`: it is a tracked build product, and its absence means the object
beside it was never regenerated.

## The selection filters drained before the corpus did

Every ranking returned nothing usable at the top of batch 172: `fuzzy_solved`
4 leads at >= 0.80 with all four flagged, `match_shapes` / `solved_twins` /
`--near` at 0 for the third round running, and `pickable` 99 candidates of which
**every one uses r8-r11**. That last number is the diagnostic. The filter's
survivors have become homogeneous -- every remaining admission carries the one
wall the filter was never built to score.

`pickable` and `filtered` are tuned for 40-120 instructions and >= 5 calls, and
that band is worked out. Scanning **below** it -- 14-44 instructions, no r8-r11,
no repeated expensive constant, call count unconstrained -- returned 19
candidates, of which three were tried and three matched in 1, 4 and 2 screens.

The rejection of functions under 40 instructions is justified in `pickable.py`
as "a tiny function gives the allocator nothing to act on." That is sound
reasoning about which LEVERS apply and a false statement about which functions
MATCH; a short function often needs no lever at all.

> **A filter tuned to predict which functions need documented work will
> systematically hide the functions that need none.**

## Check siblings inside one .s family by hand

`Sprite_DeleteLayerIndex` matched on the first screen because the already-solved
`Sprite_DeleteLayer` in the same `.s` family is the same routine reached by
pointer instead of by index, and its entire second half -- the count loop, the
zero test, the byte store at `+0x27` -- transferred verbatim.

`fuzzy_solved.py` never surfaced that pair: it scores whole-skeleton similarity,
and the two functions differ across their entire first half. The tree's
`_a/_b/_c` splits keep genuinely related routines adjacent, and every ranking
here is blind to that adjacency. **Before ranking anything, look at what is
already solved in the target's own family.**

## `ldmia rN!, {rM}` for ONE word, outside a loop, is an explicit `*p++`

`Func_8011590` reads two adjacent pointer globals and the ROM does it like this:

    ldr   r3, =iwram_3001e6c
    ldmia r3!, {r5}          <- one word, post-incrementing
    ldr   r7, [r3, #0x0]

An `ldmia` with a single register in the list is not a block move; it is a load
with writeback, and gcc-2.96 emits it when the source **walks a pointer**.
Indexing the same two slots gives the plain form and, here, the opposite order:

| spelling | result vs a 46-line ROM |
|---|---|
| `a = p[0]; base = p[1];` | 46 lines, 2 differing -- `ldr r7,[r3,#4]` then `ldr r5,[r3,#0]` |
| `q = p; a = *q++; base = *q;` | **matches** |

Two things follow. The obvious one is the spelling. The less obvious one is that
the two globals had to be reached through **one** declaration
(`extern unsigned char *iwram_3001e6c[];`) rather than two separate `extern`s at
their own addresses -- gcc cannot know two independent externs are four bytes
apart, so it would pool both. Adjacent globals that the ROM reaches from a
single pool entry are one array, and `wram.sym` confirms the addresses.

## An offset belongs in the LOAD, not in the address, when the ROM keeps a bare base

`Func_80058ac` reads a halfword out of a 16-byte stack buffer:

    rom    mov r3, sp / ldrh r3, [r3, #0x8]
    ours   add r3, sp, #0x8 / ldrh r3, [r3, #0x0]

Same two instructions, but the ROM's base register is the buffer itself and the
offset rides in the load. `*(unsigned short *)(buf + 8)` on an
`unsigned char buf[0x10]` folds the offset into the address. Declaring the
buffer with the type it is actually read as -- `unsigned short buf[8]` -- and
writing `buf[4]` matches.

This is the same materialisation question as the named-index rule, pointed the
other way. There, naming a sum forced it into an index register. Here, letting
the ACCESS carry the offset keeps it out of the address computation. The
discriminator is in the ROM: **if the base register holds a bare pointer and the
constant rides in the load, the source indexed a typed array; if the constant is
added into the register first, the source did pointer arithmetic on bytes.**

## A caller and a callee can disagree about the prototype, and the ROM shows it

`Func_80058ac` masks its argument to 16 bits at entry (`lsl #16 / lsr #16`),
which says its parameter is a `u16`. Its caller `Func_8005a78` passes the value
**unmasked**. Declaring the callee `unsigned short` at the call site adds a
mask the ROM does not have, two extra instructions; declaring it `int` matches.

So the two translation units did not share a prototype -- the caller either had
none, or had one with a wider parameter. That is ordinary for this era, and it
means **a callee's parameter type is a per-call-site fact, not a global one**.
Do not propagate a definition's narrow parameter type into the `extern` at a
call site unless the caller's assembly actually narrows.

The tell is cheap to read: a mask at the *callee's* entry and no mask at the
*caller's* call site is exactly this disagreement.

## The loop-order lever applies to the INCREMENT, not just the initialiser

Batch 171 recorded initialiser order as three-for-three and made it a first
check. `OvlFunc_899_200c704` is the same lever in the increment block:

    rom    add r0, #0x1 / add r2, #0x10
    ours   add r2, #0x10 / add r0, #0x1

Written `for (i = 0; i <= 0x24; i++) { ...; t += 0x10; }` the pointer bump lives
in the body and lands first. Moved into the increment clause,
`for (i = 0; i <= 0x24; i++, t += 0x10)`, it matches -- 2 differing to exact, at
the ROM's exact length both ways. General form:

> When a small residue sits in a loop's SETUP or its INCREMENT and the
> instructions are right but ordered wrong, the source controls it. A bump
> written in the body and the same bump written in the increment clause are not
> the same program to gcc.

## A pooled small constant through a halfword store is NORMAL

Two independent confirmations now: `Func_8091eb0`'s `ldr r2, =0x21` and
`OvlFunc_970_20092ac`'s `ldr r3, =0x30`. Both are eight-bit immediates a `mov`
could build, both are stored through a halfword pointer, and in both cases
**gcc pools the plain literal by itself** -- our output has the same `ldr` the
ROM does.

The recorded pooled-constant tell ("`ldr r0, =0x89` for a value an eight-bit
`mov` builds means the source names a linker symbol") therefore has an
exception. Before adding an entry to `const.sym`, check whether writing the
plain literal already pools it. `Func_809b364` is the case where the symbol IS
needed -- there the constant feeds a `cmp`, not a `strh`.

## Adjacent globals from one pool entry work backwards too

Batch 174 established that two globals the ROM reaches from a single pool entry
are one array. `Func_80c0130` is the same thing at a NEGATIVE offset: it reaches
`iwram_3001e78` as `mov r3, r2 / sub r3, #0x88` off `iwram_3001f00`'s pool
address. `extern unsigned char iwram_3001f00[];` with
`*(unsigned char **)(iwram_3001f00 - 0x88)` produces it exactly, and this is the
same spelling already used elsewhere in the tree
(`*(unsigned char **)(iwram_3001f30 - 0x64)`).

Two more things that function reproduces for free, both worth knowing before
reaching for a lever:

- **A second DMA's base derived from the first.** The ROM's DMA3 transfer uses
  `add r3, #0x24` off `&REG_DMA0SAD` rather than a fresh `&REG_DMA3SAD` pool
  load. Writing `DMA0_SET(...)` followed by `DMA3_SET(...)` produces it -- gcc's
  constant CSE finds `0x40000d4 = 0x40000b0 + 0x24` on its own.
- **A register destination derived from another register's address**, likewise:
  `add r1, #0x14` off `&REG_BG2CNT` comes from plainly writing `&REG_BG2PA`.

## `-fno-gcse` reaches a SUNK LOAD but not a SHARED CONSTANT

Batch 175 parked three functions on duplicate-constant CSE and found `-fno-gcse`
inert on all three. `Func_807a550` matched **with** `-fno-gcse`. Both results
are right, and together they separate two passes that share a flag name:

| symptom | pass | `-fno-gcse` |
|---|---|---|
| one constant materialised once and kept in a callee-saved register | `cse.c`, local | inert |
| a redundant LOAD sunk onto the only path that needs it | global CSE / PRE | **fixes it** |

`Func_807a550` re-reads its loop bound through a pointer each iteration because
a byte store inside the loop may alias it. At -O2 gcc is *smarter* than the
original build: it sinks that reload onto the branch where the store actually
happens and skips it otherwise. The ROM reloads unconditionally. 7 differing
lines to exact.

**Read the diff before reaching for the flag.** A shared constant does not yield
to `-fno-gcse`; a load that appears on fewer paths than the ROM's does.

## Naming the SCALED BYTE offset flips a register+register load's operands

`ldr r0, [r6, r3]` and `ldr r0, [r3, r6]` are different bytes. `Func_80c1fa8`'s
last instruction was the second where the ROM has the first, and it was the only
differing line in an otherwise exact 42.

| spelling | result |
|---|---|
| `return base[idx];` | `ldr r0, [r3, r6]` -- the SCALED INDEX lands in Rn |
| `return *(base + idx);` | identical |
| `return *(int *)((char *)base + idx * 4);` | identical |
| `k = idx * 4; return *(int *)((char *)base + k);` | **`ldr r0, [r6, r3]`** |

Batch 172 established that naming an index materialises it and therefore
produces an index register. This is the next question down: *which* operand
becomes the base. Array subscripting hands gcc `(plus (mult idx 4) base)` and it
takes the first term as Rn. Naming the already-scaled byte offset hands it
`(plus base k)` instead.

So the rule has two levels. **Name the index to get a register+register load at
all; name the SCALED BYTE offset to control which register is the base.**

## An accumulator, again -- and now with a second confirmation

`Func_807a550` and `Func_80c1fa8` both needed `count = 0;` / `n = 0;` moved
earlier than the natural place, and for two different reasons that are worth
keeping apart:

- `Func_807a550`: moved above the CALL, so the live range crosses it and the
  accumulator lands in a callee-saved register. This is batch 173's rule, second
  instance, and the tell is the same -- the ROM's `mov r6, #0x0` sits *after*
  the `bl`, which argues against the fix.
- `Func_80c1fa8`: no call is involved. Moving `n = 0;` above an `if` that
  rewrites the function's argument took 17 differing lines to 10, purely as
  statement order.

Together: **an accumulator's initialiser wants to be as early as the source will
allow**, and it is worth trying at the top of the function before anything else.

## The addressing forms are a PREFERENCE, not a guarantee

Batch 174 gave a discriminator for register+register loads: a bare base with the
constant in the load means a typed array; the constant added into the register
first means byte pointer arithmetic. `TestCollision` is the case where the
recorded fix stops working, and the reason is worth having.

`Func_8011f54` and `TestCollision` read the SAME two tables through the same
idiom, and both ROMs split the ewram read into
`add r0, r1, r3 / ldrb r0, [r0, #0x0]`. In `Func_8011f54`, writing a named
pointer (`q = ewram_202c000 + idx; ... *q ...`) produces exactly that. In
`TestCollision` the identical spelling is folded back into `ldrb r2, [r0, r3]`.

The difference is register pressure. `TestCollision` is also juggling a function
pointer, two masked coordinates and a table base; `Func_8011f54` is not. A named
pointer only survives as its own value while gcc has a register to keep it in.

> **When a spelling that worked on one function goes inert on a near-identical
> one, look at what else is competing for registers before concluding the rule
> is wrong.** These levers express a preference to the allocator; they do not
> override it.

## A constant-folding opportunity the ROM did not take means the constants are not adjacent

`Func_8077cb8` parses three two-digit decimal fields. Written the obvious way,
`(p[0] - '0') * 10 + (p[1] - '0')`, gcc folds -480 and -48 into a single -528
and the function comes out **ten lines short of the ROM**. The ROM applies
-0x1e0 at the multiply -- pooled, and shared across all three fields -- and
-0x30 at the point of use.

Writing `a = (*p++ - '0') * 10; a += *p++;` and then consuming `(a - '0')` where
`a` is used reproduces both, and takes 46 lines to 54 (of 56).

The general form: **if the ROM leaves two compile-time constants unfolded, they
were not adjacent in the source.** gcc will always fold `x*10 - 480 + y - 48`;
it cannot fold across a statement boundary that the value has to survive.

## A chain of arms that each STORE AND RETURN is not the same as one that assigns and joins

`OvlFunc_880_20082f4` maps a character code through fifteen arms and writes one
byte. The ROM's shape is a plain chain: `cmp / bgt <next> / mov / add / b <join>`
per arm, with a single `strb` at the join. Written the way that shape reads --

```c
    if (c <= 7) v = c + 0x41;
    else if (c <= 0xc) v = c + 0x42;
    ...
    out[0] = v;
```

-- gcc comes out at **58 lines against the ROM's 86**. It speculates each arm's
computation ABOVE its test (`mov r3, r0 / add r3, #0x41 / cmp r0, #7 / ble
<join>`), which is legal because the assignment has no side effect, and saves an
instruction per arm.

Putting the store inside each arm matches exactly:

```c
    if (c <= 7) { out[0] = c + 0x41; return; }
    if (c <= 0xc) { out[0] = c + 0x42; return; }
    ...
    out[0] = 0x3d;
```

A store cannot be speculated above the test, so the computation stays in the
arm; and cross-jumping then merges the fifteen identical `strb / pop / bx` tails
back into the one the ROM shows.

> **A ROM whose arms all branch to a single store does NOT mean the source had a
> join variable.** Cross-jumped store-and-return arms produce the same tail. The
> discriminator is whether the arm's COMPUTATION sits before or after its test:
> before means a join variable, after means the arm stores for itself.

An explicit `goto` chain reproducing the ROM's control flow exactly is
byte-identical to the join-variable form -- gcc canonicalises it away -- so this
is not reachable by rearranging control flow, only by giving each arm a side
effect. Six flag groups, including `-fno-thread-jumps` and
`-fno-cse-follow-jumps`, are all inert.

## Naming is a floor, not a ceiling

Batch 172 established that naming a value forces it to exist, and batches 174
and 176 used that to pin loads and select addressing forms. `Func_801d014` is
the limit case. Its residue is that gcc forms a copy's DESTINATION address
before its source, where the ROM does the reverse, five times over. Naming the
loaded byte costs eight lines; naming the source address costs five.

> Naming can stop gcc sinking something past a point. It cannot stop gcc
> hoisting something above one. When the fix you need is "move this LATER", no
> name will do it.

## The two directions of pool-load motion are different passes

| the ROM has | gcc does | flag |
|---|---|---|
| the pool load OUTSIDE a loop | sinks it in, reloading each iteration | `-fno-gcse` fixes it (`rom_f0254_a_b.c`) |
| the pool load INSIDE a loop | hoists it out into a callee-saved register | no flag exists (`rom_9000/8011164.c`) |

`Func_8011164` reloads `gBuffer` every iteration and bumps it by two between two
reads. Writing that literally -- `g = gBuffer;` as the loop body's first
statement, `g += 2;` between the reads -- is the right shape and narrows the push
list from `{r5, r6, r7, lr}` to `{r5, r6, lr}`, but gcc still recognises
`g = gBuffer` as loop-invariant and hoists the initialisation. `-fno-gcse` is
**inert**, which is the diagnostic: the motion belongs to `loop.c`, not global
CSE, and gcc-2.96 has no switch for it.

## gcc will ADD a callee-saved register to share a constant

Batch 175 parked three functions on duplicate-constant CSE, and all three had
callee-saved registers already committed. That suggested a reading: gcc shares a
repeated constant only when a register happens to be spare, so a function whose
ROM pushes nothing but `lr` should be safe.

`OvlFunc_953_200a904` was screened to test exactly that -- ten calls, `push
{lr}` alone, one constant (`0xd6 << 1`) passed to three consecutive calls -- and
the reading is wrong:

    rom    push {r14}       ... mov r2, #0xd6 / lsl r2, #0x1   (three times)
    ours   push {r5, r14}   mov r5, #0xd6 / lsl r5, #0x1 ... mov r2, r5  (x3)

gcc adds r5, its push and its pop, to avoid rebuilding a two-instruction
constant three times. The length comes out IDENTICAL -- the hoist plus the wider
prologue exactly pays for the three rebuilds -- and 25 of 34 lines differ.

> **A zero-pressure ROM push list is no protection.** The duplicate-constant
> reject is about the constant, not about pressure, and is right to be a hard
> skip in every ranking.

All five flag groups are inert, as they were on the batch-175 three.

## Pool scaffolding cuts both ways

`Func_80f6148` is two lines SHORT of its ROM because the ROM contains two `b`
instructions to the immediately-following label -- jumps over a literal pool
emitted mid-function. `Func_801bd98` is one line LONG for the same reason with
the sign flipped: OUR output has the `b`, stepping over the pool gcc placed
before `=0x3ff` and `=0xfffffc00`, and the ROM's pool is elsewhere.

> A one-line difference in EITHER direction, adjacent to a `b` whose target is
> the next label, is constant-pool placement and not a source problem. `tryc.py`
> normalises pool loads and cannot see where a pool sits; this is the shape that
> costs.

## A run of separate constant ANDs on one loaded byte is the BITFIELD tell

`Func_801bd98`'s tail does one `ldrb [r0, #5]`, four separate ANDs with `~0xc`,
`~0x20`, `~0x10` and `0x3f`, and one `strb`. Written as four `&=` statements gcc
folds all four masks into a single `& 3` -- it does NOT fold them when they are
four assignments to four different bitfields of the same byte, because each is a
separate insert.

The same function shows how gcc picks the access width: a 16-bit field at +8
declared `unsigned short f8a : 10, f8b : 2, f8c : 4;` is reached as
`ldrb [r0,#9] & 0xf`, an `ldrh [r0,#8]` read-modify-write, and
`ldrb [r0,#9] & ~0xc` -- **the smallest load that covers each field**, which is
why one struct produces a mix of byte and halfword accesses that looks
inconsistent in the disassembly.

## The m4a bodies can never be C

`asm/rom_f9000/*` contains functions with NO prologue that read r4 and r5 as
implicit inputs (`Func_80f9f3c` is the clearest: it opens with
`ldrb r1, [r4, #0x12]` and ends `bx lr`, never having written r4). They are
hand-written assembly with a private calling convention, which is why the census
keeps `audio` as its own class. Every scan in this tree kept offering them;
`tools/lowpressure.py` now rejects any body that reads r4-r7 before writing it
and never pushes it.

## The arg-interleave wall IS reachable -- the basic-block lever, first success

`OvlFunc_921_2008384` reached the ROM's exact 133 lines with eight differing,
and all eight were four copies of the recorded arg-interleave shape:

    rom    mov r1, #0x80 / mov r0, #0x8 / lsl r1, #0x1 / mov r2, #0x28
    ours   mov r1, #0x80 / lsl r1, #0x1 / mov r0, #0x8 / mov r2, #0x28

The ROM splits the two-instruction build of an argument around another
argument's `mov`. Batch 42 read `local-alloc.c` and concluded the split comes
from `update_equiv_regs` declining to keep an equivalence when the pseudo is
live in more than one basic block -- so **assigning the constant in a block that
DOMINATES the call, and holds none of its uses, forces the split**. Several
functions have been parked on this since, every one of them straight-line, and
`src/non_matching/ovl_7cb2c0/200bdec.c` says explicitly that a call does not
create the block the lever needs.

This function has two early-return `if`s before the affected calls, so the entry
block genuinely dominates them. Declaring four locals, assigning the four
constants at the top, and passing the locals matches:

```c
    e1 = 0x80 << 1;  e2 = 0xd0 << 8;  e3 = 0x19999;  e4 = 0xc0 << 6;
    if (__GetFlag(0x881) != 0) { ... return; }
    if (__GetFlag(0x82b) != 0) { ... return; }
    ...
    __MapActor_Emote(8, e1, 0x28);
```

Eight differing to zero. **The wall is not the interleave -- it is the absence
of a dominating branch.** When a function has one, the lever works, and it is
worth checking the control-flow graph before parking anything on this class.

Note this does NOT contradict "naming a constant cannot create register
pressure" (batch 172). That entry is about a ROM holding a literal in a register
for its own sake; this is about where gcc is willing to REBUILD one.

## Naming a stored value stops the halfword-store pooling -- and can delete a pool skip

Batch 175 recorded that gcc pools a small literal stored through a halfword
pointer, and that this is normal output rather than a `_CONST_*` tell.
`OvlFunc_921_2008384` is the case where the ROM does NOT pool it:

| spelling | result |
|---|---|
| `*(short *)a = 0xa;` | `ldr r3, =0xa`, 135 lines against 133, 66 differing |
| `*(unsigned short *)a = 0xa;` | identical |
| `n = 0xa; *(short *)a = n;` | **`mov r3, #0xa`, 133 lines, 10 differing** |

Naming the value takes it out of the pool. The two-line gain is the interesting
part and it is not the `mov`/`ldr` swap: removing the pool ENTRY removed a whole
pool, and with it the `b <next label>` gcc had been emitting to step over it.

> A pooled constant costs more than its own instruction. If a function is one or
> two lines long next to a pool skip, look for a pool entry to eliminate.

Together with batch 175's entry the rule is: gcc pools a small literal at a
halfword store **when it is written as a literal**, and does not when it is
written through a named local. The ROM tells you which.

## Extending the band is cheaper than mining the parks

Two consecutive rounds returned one elevation and then none, because the clean
20-120 instruction band was worked out -- eight functions left, four of them
m4a. Raising the ceiling to 260 instructions returned **thirteen fresh clean
candidates**, most of them zero-callee-saved cutscene scripts, and the first one
tried matched.

Long functions are not harder in proportion to their length. A 130-instruction
script with 41 calls is 41 easy transcriptions and one or two real questions;
the work scales with the number of DISTINCT residues, not with size. **When a
band empties, raise the ceiling before changing method.**

## The callee's RETURN TYPE can fix every call site at once

`OvlFunc_974_20088c4` is a 232-line debug-setup script: 53 calls to two
functions, each with three constant arguments. It screened at the ROM's exact
length with **159 of 232 lines differing** -- every call, the same way:

    rom    mov r1, #E / mov r2, #j / mov r0, #S / bl __GiveDjinni
    ours   mov r0, #S / mov r1, #E / mov r2, #j / bl __GiveDjinni

The ROM sets r0 LAST. Declaring the two callees `int` instead of `void` --
nothing else changed, and neither result is used -- matches all 53.

| declaration | result |
|---|---|
| `extern void __GiveDjinni(int, int, int);` | 159 differing |
| `extern void __GiveDjinni();` (withheld) | 159 differing |
| `extern int __GiveDjinni(int, int, int);` | **exact** |

The recorded form of this lever (batch 93, refined in 94) is about the presence
of a prototype. The return type is a separate knob and it is the stronger one:
withholding the prototype did nothing here. gcc reserves r0 for a value-returning
call and so evaluates the r0 argument last; for a `void` call r0 is just another
argument register and goes first.

> **When EVERY call in a function has its arguments in the wrong order, look at
> the return types before anything else.** One declaration fixed 53 sites; no
> amount of statement reordering would have.

Related and already recorded from the other direction: the epilogue tell
(`pop {r1}` means the enclosing function is declared non-void). Both are the
same fact -- gcc treats r0 as reserved whenever a return value exists, declared
or not.

## Order switch cases by the ROM's LABEL ADDRESSES, not numerically

`OvlFunc_917_20092f4` dispatches two `switch` statements through gcc-generated
jump tables and then cross-jumps their tails into each other -- case 0 of the
first switch ends `mov r0, #8 / b .L1488`, and `.L1488` is in the middle of case
0 of the SECOND switch. Two arms also fall through into later cases.

All of that comes free from a plain `switch`, but only if the cases are written
in the order their labels appear in the ROM. The first switch's arms are laid
out 0,1,2,3,4,5,6,8,9,10 then 7 and 11 sharing; the second's are 0,2,3,4 then 1
then 5. Written numerically, the fall-throughs are wrong and the tails do not
merge.

Do NOT hand-write the dispatch: the `ldr r3, [r3, r2] / mov pc, r3` with an
inline `.word` table is what gcc emits for contiguous cases.

## The basic-block lever is bounded by the register file

Batch 176 broke the arg-interleave wall by assigning four constants in a
dominating block. `OvlFunc_932_200a6c0` needs eight, and gcc spills all of them:
136 lines and 28 differing becomes 144 and 142.

> The lever's cost is a live range. A `push {lr}` function has nowhere to put
> more than two or three, and past that the spill costs more than the
> interleave. Apply it to the specific constants the diff names, never to every
> constant in the function.

## Stack-argument materialisation

A call with more than four arguments passes the rest on the stack. When the ROM
materialises two stack arguments into two registers before storing either, gcc
will not follow:

    rom    mov r3, #0x14 / mov r2, #0x29 / str r3, [sp] / str r2, [sp, #4]
    ours   mov r3, #0x14 / str r3, [sp] / mov r3, #0x29 / str r3, [sp, #4]

gcc accumulates outgoing arguments and emits a move-then-store per argument,
reusing one register; the original build evaluated all arguments into pseudos
first. Three specimens: `ovl_7b9cb4/200a6c0.c` (six sites),
`ovl_78b2ac/2008ef8.c` (two), `ovl_7b8cb0/2008904.c` (one).

**The discriminator is what the first stack argument is**, and it explains why
most sites match:

- both stack arguments the SAME value -- matches (the ROM uses one register too)
- the first already in a register (a variable) -- matches; gcc has nothing to
  move, so its habit never shows
- both distinct literals -- FAILS

`ovl_7b8cb0/2008904.c` is the case worth copying from: seven of its eight
six-argument calls match because the original code passes a FLAG VARIABLE -- one
that is zero on that path -- as both stack arguments rather than a literal zero.
When a ROM stores the same register to both stack slots and a nearby flag is
provably zero there, pass the flag.

Naming the literal in a dominating block makes it much worse (137 lines against
140, and 130 against 140 for the other operand): the local needs a register the
function does not have.

## Four levers that took one function from 158 differing to 7

`OvlFunc_970_2008da4`, in order of what each was worth:

1. **A write-only local the ROM stores to the stack is `volatile`.** The ROM
   computes three BG control words and stores each to a stack halfword nothing
   reads, as well as to the register. Declared plainly, gcc keeps the value in a
   register and drops the frame -- 158 differing. `volatile unsigned short t;`
   restores the frame and the stores: **158 -> 35**.

2. **Naming a stored value stops the pooling** (batch 176's rule, twice more):
   `n = 0x81 << 4; REG_BLDALPHA = n;` gives `mov`/`lsl` where the literal pools,
   **35 -> 26**, and the pool entry it removes takes a skip jump with it.
   `n = 0x2648; REG_BLDCNT = n;` is **15 -> 7** by the same mechanism.

3. **The offset belongs in the load.** `*(int *)(b + (0x9a << 1) + 0xc)` folds
   to one 0x140 offset and gcc addresses `[r2, #0]`; naming the base keeps the
   ROM's `[r1, #0xc]`. **26 -> 15**.

Worth noting that (2) fired twice and each time the gain was larger than the one
instruction it changed, because removing a pool ENTRY can remove a whole pool
and its skip jump. When a function is near-exact and carries a `b` to the next
label, the cheapest thing to try is naming whatever literal is in the pool.

## gcc narrows a mask that the ROM kept wide

`OvlFunc_970_2008da4` writes `(x & ~0xc) | 4` to a byte field ten times. The ROM
builds `~0xc` as -13, deriving it from a 2 that is already live (`sub r5, #0xf`).
gcc emits `mov r5, #0xf3` -- the same mask narrowed to byte width, legal because
the result is stored with `strb`, and one instruction either way.

`& -13` instead of `& ~0xc` is byte-identical: the narrowing happens on the
value's mode, not on how the constant is spelled. This is gcc doing an
optimisation the original build did not, and there is no flag for it.

## The duplicate-constant search is CLOSED -- thirteen flags

`OvlFunc_941_2009394` is 53 lines with exactly one residue: `0x81 << 1` passed
to two calls eleven apart, which the ROM rebuilds and gcc hoists into a
callee-saved register. That makes it the ideal probe, and it was used to finish
the flag search batch 175 started:

    -fno-gcse  -fno-rerun-cse-after-loop  -fno-strength-reduce
    -fno-strict-aliasing  -fno-cse-follow-jumps  -fno-cse-skip-blocks
    -fno-force-mem  -fno-caller-saves  -fno-function-cse  -fno-inline

all inert; `-fno-schedule-insns2`, `-fno-expensive-optimizations` and
`-fno-omit-frame-pointer` all WORSE. Thirteen flags, no reach. The sharing is
cse.c's local constant propagation, gcc-2.96 exposes no switch for it, and
batch 175 already ruled out separating the two uses by parameter mode.

**The selection consequence is the important half.** A scan for call-dominated
functions with no stack arguments and no high registers returns 158 candidates
and EVERY ONE repeats an expensive constant. The pure-cutscene-script class --
four elevations across batches 176 and 177 -- is exhausted, and what remains of
it is all this wall. The DUP-CONST reject in `pickable.py`, `filtered.py`,
`family_siblings.py` and `lowpressure.py` is correct and is a hard skip.

## Scope a name as tightly as the ROM's live range

Naming a stored value to keep it out of the constant pool is a recorded lever
(batch 176; twice more in `ovl_7fa4ec/2008da4.c`). `Func_8095938` shows it has a
cost that depends entirely on scope. The function needs three halfword stores of
zero to use `mov` rather than a pooled constant:

| where the name is declared | result vs a 136-line ROM |
|---|---|
| one `int z = 0;` at function scope | 143 lines, 142 differing |
| `int z1/z2/z3 = 0;` inside each arm that uses it | 137 lines, 25 differing |

At function scope the local outlives every call in the body and gcc spills it.
Declared inside the arm, its live range is two instructions and the lever is
free. **The name is not the lever -- the live range is.** The same bound applies
to the basic-block lever (`ovl_7b9cb4/200a6c0.c`: four locals fit, eight spill).

## A duplicate LABEL costs no bytes, and tryc counts it as a line

`Func_8095938` screened at "137 against 136, 23 differing", which reads as one
missing instruction plus a shifted tail. It is neither. The extra line is
`.L20: .L21:` -- two labels at one address, emitting nothing -- so the
instruction counts are equal, and the 23 are ONE store scheduled two
instructions late plus the shift it causes.

`make compare` was the only way to find that out: the file was placed, built,
and failed, and the generated `.s` had to be read beside the reference.

> When a screen is within a line or two and the diff looks like a pure shift,
> check for a duplicate label before believing the count, and read the generated
> asm rather than the normalised diff.

That test also re-confirmed the batch-172 stale-object trap FROM THE OTHER SIDE:
after removing the `.c` and restoring the `.s`, `make compare` still failed
until the `.o` was deleted by hand. The object is built from whichever source
existed last, and make does not notice the swap in either direction.

## The branch-over-pool class was never blocked -- 501 functions reopened

`tools/poolblocked.py` reported 501 of 1979 remaining functions -- a quarter of
the corpus -- as CERTAIN, and `census.py` counted them the same way. The premise:

> "old_agbcc emits a function's constant pool at `.func_end` and never in the
>  middle. That `b` is a real instruction we cannot produce."

**That premise is about old_agbcc**, which in this tree builds five m4a and
agb_flash objects and nothing else. Every elevated function is compiled by
gcc-2.96, and gcc-2.96 emits mid-body pools with skip jumps routinely.
Compiling a plain transcription of `Func_80bad7c` -- no lever applied -- gives
two of them:

        b       .L10
    .L19:
        .align  2, 0
    .L18:
        .word   256
        .word   iwram_3001e74
    .L3:

The supporting measurement ("mid-function pools appear in ZERO of the elevated
translation units") was true and circular: the tool rejected every candidate
that would have one, so none was ever attempted, so the count stayed at zero.

**A third of the class has no pool at all.** `.pool_aligned` is a macro that
flushes pending literals and emits nothing when there are none. The test looked
for the directive with code after it and counted the MARKER, not the data.
`CreateSpriteLayer` was reported blocked on two empty markers whose `b`
instructions are a loop-entry jump and a branch to the epilogue. Splitting the
501 by whether data actually precedes the marker gives roughly 167 empty and 334
real (the boundary moves with how far back you look; a hand count with a wider
window gave 235/266).

So: **zero certain blockers in the class.** Treat a reported function as a
candidate that will need pool placement watched, and remember that placement
follows the pool's CONTENTS -- removing one entry by naming a stored value
removed a whole pool and its skip jump in `OvlFunc_921_2008384` and twice in
`ovl_7fa4ec/2008da4.c`.

**The general lesson is about how a blocker class gets recorded.** This one was
adopted on a compiler-behaviour claim plus a corpus measurement, and the
measurement could not have come out any other way once the claim was acted on.
When a class is declared CERTAIN, the test that would falsify it is to
transcribe one member and look at the generated asm -- not to count the
already-matching corpus, which by construction contains only what was attempted.

## gcc-2.96 pushes lr in ANY Thumb function with a conditional branch

A third flag/toolchain fact readable off the ROM without screening, alongside
the `pop {r4, r5, r6, pc}` and `-fcall-used-r4` entries above.

    int  Leaf(int *p)   { return p[3] + 1; }        ->  ldr / add / bx lr
    void A(int **p)     { if (p[3]) p[3][2] = 0; }  ->  push {lr} / ... /
                                                         pop {r0} / bx r0

One `if`, no calls, three instructions of work, and the prologue appears. It is
not an interwork artifact -- without `-mthumb-interwork` the push is still there
and only the return changes, to `pop {pc}`.

> **A ROM function that contains a conditional branch and NO `push` was not
> compiled by gcc-2.96.**

`RealClearChain` at 0x080f9a30 is the specimen: a sixteen-instruction
doubly-linked-list unlink, `ldr r3, [r0, #0x2c]` first and `bx r14` last.
old_agbcc compiles the same C to the ROM's shape -- no prologue, `bx lr`, every
branch and store in order -- differing only by a leading `add r2, r0, #0` and a
rotation of which temp holds which pointer.

## The `audio` class is C, but NEITHER compiler in the tree produces it

**This entry replaces the one written the round before, which said the class
"needs old_agbcc". That was wrong, and the correction is the useful part.**

Two things are established about `asm/rom_f9000`:

- **Some of its functions are ordinary C.** `RealClearChain` is a textbook
  doubly-linked-list unlink; `ply_patt` is a three-line dispatcher.
- **Neither of the tree's compilers reproduces the ROM's form.** gcc-2.96 gets
  the register usage right (`ldr r1, [r0, #0x2c]`, r0 used directly) and the
  prologue wrong (it pushes lr, see above). old_agbcc gets the prologue right
  (`bx lr`, no push) and inserts a copy.

The old_agbcc copy is systematic, not incidental:

    void F1(int *p) { p[3] = 0; }             -> mov r1, #0 / str r1, [r0, #0xc]
    void F2(int *p) { if (p[3]) p[4] = 0; }   -> add r1, r0, #0 / ldr r0, [r1, #0xc] ...
    int  F3(int *p) { return p[3] + p[4]; }   -> add r1, r0, #0 / ldr r0, [r1, #0xc] ...

> **old_agbcc copies an incoming pointer argument to another register whenever
> it is used more than once.**

`RealClearChain` uses its argument four times and the ROM uses r0 directly, so
old_agbcc is excluded. Five spellings were tried against it -- a named-field
struct, plain casts, an early `return`, and two load orderings -- and all five
produce the copy.

So the class stays open as a question about which compiler built that region,
and it is NOT a per-file Makefile rule away. What the previous round got right
is the free test: a conditional branch with no `push` rules out gcc-2.96.

**The general lesson, and it is the second time this month.** The
branch-over-pool class was declared CERTAIN on a compiler-behaviour claim that
was never tested against the compiler actually in use. This entry was written
the same way -- old_agbcc produced the right prologue on the first try and the
conclusion was drawn from that one observation, without checking whether the
rest of its output could be reached. **One matching feature is not a matching
compiler.** Compile the whole function and diff it before naming a toolchain.

## The accessible pool, measured

After the branch-over-pool correction, applying every reject that has been
established -- parked, r8-r12/r14 outside the push/pop, a repeated expensive
constant, `const_remat`, `precompute` -- to the whole tree leaves **twelve**
candidates in 12-160 instructions, and nine of those are `asm/rom_f9000`.

The three that are not are `OvlFunc_957_2008f94`, `Func_801a66c` and
`Func_8026e80`; the last two are parked here at 105 and 87 differing, both on
allocation or layout.

That is the honest state of the worklist and it is why recent rounds return one
elevation or none. **The next real gains are in relaxing a reject, not in
working the list**: the duplicate-constant reject alone removes 85 of the 173
precompute-class candidates and every one of the 158 call-dominated scripts.

## r8-r11 is NOT a blocker -- and treating it as one hid 109 functions

Every ranking in this tree rejected any function mentioning r8-r11 outside the
push/pop. That reject is wrong, and it was costing more than any other.

**gcc-2.96 allocates r8-r11 freely once r4-r7 are committed.** `-fcall-used-r4`
takes r4 out of the callee-saved set, so a function with four values live across
a call has only r5, r6, r7 -- and the fourth goes to r8, with the
`mov r7, r8 / push {r7}` prologue and `pop {r3} / mov r8, r3` epilogue that the
reject was reading as a wall. Our own screens have been emitting r8 and r10 for
rounds; the reject was never tested against that.

Relaxing it to a reported column left **122 candidates** where the full reject
left twelve, and three of the first four tried matched:

| function | screens | what it took |
|---|---|---|
| `Func_80a1814` | 3 | the derived byte below |
| `Func_800d924` | **1** | nothing |
| `Func_800d98c` | **1** | nothing -- twin of the above |

In all three the r8 came out with no lever: a zero live across two calls, a
pointer argument live across a loop's call. **Write the C and let the allocator
find the high register.**

r12 (ip) and r14 (lr) holding a value ARE a real wall -- no C expresses either --
so the reject is kept for those two and only those.

**How this happened is the same shape as the branch-over-pool correction.** The
reject was a heuristic ("avoid allocation fights") that hardened into a
certainty, and the corpus measurement that would have falsified it -- our own
generated asm, which contains r8 and r10 all over -- was never compared against
it. Two rejects have now been found wrong this way in three rounds.

## Write a derivation as its own STATEMENT, not as an expression

Batch 178 recorded that when a ROM derives a small constant from a value that is
already live, the source writes the derivation rather than the constant.
`Func_80a1814` sharpens it: the derivation has to be a compound assignment.

    rom    mov r3, #0xfe / strb r3, [r0, #0xf] / ... / sub r3, #0xff
                                               / strb r3, [r2, #0xf]

| spelling | result vs a 44-line ROM |
|---|---|
| `r[0xf] = 0xfe;` and `t[0xf] = 0xfe - 0xff;` | 43 lines, 16 differing |
| `n = 0xfe; r[0xf] = n; t[0xf] = n - 0xff;` | 44 lines, **1** differing |
| `n = 0xfe; r[0xf] = n; n -= 0xff; t[0xf] = n;` | **matches** |

`n - 0xff` inside the store is folded to -1 and materialised with a `mov`;
`n -= 0xff` as its own statement survives as a `sub` on the register the
previous store just used. **The lever is the statement boundary, not the name.**

## The derivation lever has no mirror

Three functions now need the source to write a derivation the ROM performs:
`n -= 0xff` rather than `n - 0xff` (`Func_80a1814`), `t--` rather than
`t = -1` (`Func_8026e80`), and the `0x3e7` that gcc derives for free in
`Func_801a66c` and that naming BREAKS.

`Func_801ed40` is the same question pointed the other way and it has no answer.
Two field offsets four bytes apart are tested one after the other; the ROM pools
both, and gcc pools the first and subtracts 2:

    rom    ldr r2, =0x12ee ... ldr r2, =0x12ec
    ours   ldr r2, =0x12ee ... sub r2, #0x2

One differing line of 63, at exact length. Three spellings (`p + 0x12ec`,
`p + (0x12f0 - 4)`, each address named into its own pointer) and three flags
(`-fno-gcse`, `-fno-cse-follow-jumps`, `-fno-expensive-optimizations`) are all
byte-identical.

> **You can ask gcc to derive a constant. You cannot ask it for a second pool
> entry.** When the ROM materialises two nearby constants separately and gcc
> derives one from the other, that is the end of the road.

## Our code being SHORTER than the ROM is not always a bug to fix

`OvlFunc_888_200a6f0` comes out 43 lines against the ROM's 45. Both use r8 and
r10; they disagree only about which value goes where. The angle is used twice
and early, the `+0x64` pointer twice and late; gcc gives the cheap low register
to the pointer and needs no moves, while the ROM puts the pointer in r8 and pays
`mov r2, r8` and `mov r1, r8`.

There is nothing to write. Three spellings of the pointer's construction were
measured and none moves it, because the difference follows from which register
the value is heading for, not from the arithmetic. **A two-line deficit where
the ROM's extra lines are `mov rN, r8` reads are an allocation choice, not a
missing statement** -- check what the moves are before looking for one.

## A local constant used twice can be folded past a macro

`Func_8012388` allocates a buffer and DMAs into it, passing the same size to
both:

    rom    ldr r5, =0x27c / ... / lsr r5, #0x2 / lsl r2, #0x18 / orr r2, r5
    ours   mov r1, #0x9f / lsl r1, #0x2 ... ldr r2, =0x8400009f

`DMA3_COPY` builds `0x84000000 | (size / 4)`. With `size` a local initialised to
a literal, gcc folds the whole count word at compile time; the ROM's compiler
kept the size in a register and did the shift and or at runtime.

This is the batch-174 shape -- a fold the ROM declined -- but the recorded fix
(separate the two constants into different statements) does not apply: here it
is ONE constant that has to stay a variable, and nothing in this notebook
currently achieves that. Six lines of 39.

## Two plain local inits are emitted in SOURCE ORDER

Not the argument-block rule above -- that one is about pooled loads inside a
call's argument list, and it says gcc *ignores* source order there. This is the
opposite case and the opposite answer.

When two locals are initialised to constants in the same basic block and neither
depends on the other, gcc-2.96 emits the two `mov`s in the order the statements
are written.

    rom     mov r7, #0x0        i    = 0
            mov r5, #0xd8       off  = 0xd8

    ours    mov r5, #0xd8       off = 0xd8;   <- written first
            mov r7, #0x0        i   = 0;

Swapping the two source lines swapped the two instructions and took
`Func_807882c` from 4 differing of 35 to 2. It is the cheapest lever in the
notebook -- one line move, no restructuring -- and it is worth trying FIRST on
any residue that is a pure adjacent transposition of two constant loads.

**The boundary.** It holds for independent constant inits in one block. It does
NOT hold once the values reach an argument list (the pooled-load rule wins), and
it does not hold for the `mov r8, rN` copies reload emits for high registers --
those are placed by the allocator and no source order reaches them. See
`src/non_matching/rom_77000/8078870.c`, where the residue is exactly such a copy
and nine spellings left it at 5 differing.

## A byte-sized zero can be served by a register whose LOW BYTE is already zero

The recorded naming lever says: when the ROM materialises a constant separately
and gcc reuses a register that already holds it, give the value a name. That
note is written for EXACT duplicates. It also fires on a case it does not
mention -- a **subword** store whose register only has to agree in the bytes
being written.

`Func_809a3c4` builds `0xc0 << 10` = 0x30000 into a register for two word
stores, then does `*(char *)(p + 0x5a) = 0`. The low byte of 0x30000 is zero, so
gcc reused that register for the byte store and emitted nothing at all:

    rom     mov r3, #0x0 / strb r3, [r5, #0x5a]     two instructions
    ours    strb r2, [r5, #0x5a]                    one -- r2 still holds 0x30000

That is why the body came out ONE LINE SHORT of the ROM, which reads like a
missing statement and is not. Assigning through a named local first --

    z = 0;
    *(char *)(p + 0x5a) = z;

-- restores the separate `mov` and took the function to exact length.

> **Read a one-line-short body at a `strb`/`strh` as a possible subword reuse
> before assuming a statement is missing.** Check whether any live register's
> value agrees with the stored constant in just the bytes being written. Word
> stores cannot do this; byte and halfword stores can, which makes 0 and small
> constants the ones to watch.

## Discipline: a family's PARK files carry levers, not just its matches

`tools/family_siblings.py` ranks a candidate by how much of its `_a/_b/_c`
family is already ELEVATED, on the theory that a matched sibling spells the
symbols and shapes the next one needs. That is true and it has paid off. But it
misses the other half of the family's record.

`GetEquippedItem` and `Func_807882c` both matched this round on the
pointer-typed-operand lever. Neither has an elevated sibling that demonstrates
it. The lever was written down in
`src/non_matching/rom_77000/8078588.c` -- a PARK, in the same address range,
which had found the register-offset form, used it, and still failed for an
unrelated reason. A park that fails on residue B can be the only record of
working lever A.

> Before screening a candidate, `ls src/non_matching/<its range>/` and read the
> parks whose addresses are near it. The tool now prints them; see the
> `parks near` column.

## A "redundant" register copy is usually a SECOND READ gcc has CSEd

This one retired six parked functions and elevated four more in a single round,
and it was sitting behind a misreading that had been written down twice.

Ten near-identical status-counter routines at `0x080bf250..0x080bf524` all open
with:

    ldrb r2, [r5]
    mov  r3, r2
    cmp  r3, #0

r2 is dead immediately after -- the next read of that same byte is a fresh
`ldrb r1, [r5]` -- so the `mov` was read as a redundant copy, filed under the
copy-into-a-register class, and six of the ten were parked on it. The park even
records that splitting the value across two named locals does not produce it,
because gcc coalesces them. That was all true and all beside the point.

**The copy is not redundant. It is a second read of `*p` that CSE turned into a
register copy.** Write the function with a local and there is only ONE read:

    n = *p;
    if (n == 0) goto fail;
    n += 0xff;
    *p = n;                    31 instructions vs the ROM's 32, 26 differing

Write it entirely through the pointer and there are two, the second becomes the
`mov`, and it matches on the first screen:

    if (*p == 0) goto fail;
    *p = *p + 0xff;            32 vs 32, MATCH

> **A body that is ONE INSTRUCTION SHORT with a `mov rA, rB` in the ROM's
> version of the missing line is a CSEd reload, not an allocation artifact.**
> Count the reads of the value in your C against the reads the ROM makes before
> it stores. If the ROM loads once and copies, the source read twice.

The general form is more useful than the case: **naming a value is not free.**
Every lever in this notebook that says "give it a name" buys a statement
boundary at the cost of collapsing repeated reads into one. When the ROM shows
more traffic than your version, not less, try REMOVING the name before adding
another.

This is the third reject in four batches found wrong by retesting rather than by
new technique -- after branch-over-pool and r8-r11 -- and the same shape as
both: a correct observation ("that copy does nothing useful") promoted to a
conclusion about what the source could not have been.

## `.gcc2_compiled.` is a PER-TU symbol, not a per-function one

The standing verification is: after a clean build, every elevated address must
appear in the linked ELF with a `.gcc2_compiled.` local symbol at the SAME
address. That works because this tree has been elevating one function per file.

A translation unit with ten functions in it emits the marker ONCE, at the first
function's address:

    080bf250 t .gcc2_compiled.
    080bf250 T Func_80bf250
    080bf2b4 T Func_80bf2b4      <- no marker, and this is correct
    ...

> For a multi-function TU: the marker at the first address proves the TU was
> compiled from C, and the remaining symbols at their exact ROM addresses prove
> the layout. Do not read a missing marker on functions 2..N as a failure.

## The CSEd-second-read shape is a SELECTOR, and it is read off the assembly

Batch 178's lever came with something no other lever in this notebook has: a
signature visible in the reference itself, before any C is written. A load into
rA immediately followed by `mov rB, rA` is the tell.

`tools/cse_reload.py` reports it. At the time of writing, **344 remaining
functions show it and 89 of those are already parked** — and the parked ones are
the better hunting ground, because a park that recorded "a redundant copy we
cannot produce" was usually one spelling away from matching.

Two confirmations outside the family that produced the lever, both first-screen:

- `Func_80b06ec` — four `ldrb r2, [r0] / mov r3, r2 / cmp r3, #0` groups, parked
  at four lines short. Its park had even written down *"that `mov` is the
  signature of the value being read into one variable and TESTED THROUGH
  ANOTHER"* and then not acted on it.
- `Func_801cee0` — three of them, one per switch arm, parked at three lines
  short.

The count is a hint, not a promise: a load-then-copy also appears at a jump
table's `mov pc, r3`, at a return value reloaded from the stack, and wherever
the allocator simply wanted a different register. Read the function. What the
shape is *reliably* good at is the opposite direction — **a body that comes out
short against a reference showing it is missing reads, and the fix is to remove
a name rather than add one.**

## A COUNT IS NOT A DISTANCE

`Func_801cee0` produced two candidate spellings and the better one has more
differing lines:

    shared join via `break`         48 lines, 4 differing
    store-and-return arms           48 lines, 17 differing

The 4 are two inverted conditional branches — `bls join / b exit` where the ROM
has `bhi exit / b join`, in two of three arms. That is a real difference in
control flow, and it was source-inert across the plain `break`, an explicit
`goto inc`, an explicit `goto out`, and three jump-optimisation flags.

The 17 are one register rotation: the ROM holds the pointer in r1 and the
offset-then-value in r2, and we do the reverse, consistently, in every one of
the seventeen. The control flow is exact.

> **A residue of N identical differences is closer than a residue of two
> different ones.** Rank a screen by how many DISTINCT causes its diff has, not
> by the line count. The park keeps the structurally exact version.

The corollary is about which lever to reach for. The 4-differing version had
already consumed every branch-shape probe available; the 17-differing version
has one open question (the rotation) and it is a known class with known
specimens. The second is where a future round can make progress.

## Cross-jumping is a SYMPTOM of argument order

`Func_801bcd4` dispatches through a jump table to five loaders, two of which are
the same function called with different constants. gcc merges those two arms and
the ROM does not:

    rom    mov r2, r4 / mov r0, r5 / mov r1, #0x3a / bl LoadInventoryIcon
           mov r2, r4 / mov r0, r5 / mov r1, #0x2a / bl LoadInventoryIcon

    ours   mov r0, r6 / mov r1, #0x3a / b L11
           mov r0, r6 / mov r1, #0x2a / L11: mov r2, r4 / bl LoadInventoryIcon

jump.c merges common SUFFIXES. The ROM sets r2, then r0, then r1, so the only
suffix the two arms share is the `bl` itself and nothing merges. We set r0, r1,
r2, which makes `mov r2, r4 / bl` a two-instruction common suffix — long enough.

> **Do not look for a way to disable cross-jumping. Look at the argument order
> that made the suffix long enough to merge.** The merge is downstream.

Here the fix is not available: the recorded return-type lever moves r0 relative
to the other argument registers when the callee returns a value, and declaring
the callees `int` — all five, or only the merged one — is byte-identical to the
`void` version. The lever cannot make r2 be chosen first.

## The gState fold has a cosmetic half and a real half

Two different things wear the same appearance and only one of them costs
instructions.

**Cosmetic.** `gState` is an absolute symbol (`wram.sym`: `gState = 0x02000240`),
so a reference to it canonicalises as either `=gState` or `=0x2000240` depending
on whether gcc emits a relocation or the folded value. `tryc` counts that as a
differing line and it is worth nothing.

**Real.** Written `*(int *)(gState + (0xfa << 1))` gcc folds base and offset into
ONE pool entry, `ldr r3, =gState+500`. The ROM builds it at runtime:

    ldr r3, =0x2000240 / mov r1, #0xfa / lsl r1, #0x1 / add r3, r1

That is three instructions, and a local `g = gState;` restores them.
`Player_ExitStairs` went from 69 lines to the ROM's exact 72 on that one change.

> Before chasing a pool-entry difference, check the LENGTH. If the bodies are
> the same length the pool spelling is canonicalisation and there is nothing
> there. If yours is three short beside it, the base needs naming.

**And naming the offset undoes it.** Adding `k = 0xfa << 1;` alongside the named
base folds the address again — 71 lines and 65 differing. The base wants a name;
the offset must stay an expression.

## ONE EXPRESSION, NOT TWO STATEMENTS, when a large add follows an operation

This is the mirror of "write a derivation as its own statement", and getting the
two the wrong way round cost a park for four batches.

    rom    lsl r3, #0x6 / mov r5, r3 / add r5, #0xe6

The `mov` is not about the shift. **Thumb has no three-operand add for an
immediate above 7**, so `+ 0xe6` must be destructive, which means the shift
result and the biased value have to be two separate pseudos. Whether they are is
decided by the statement structure:

    ang = X << 6;
    ang += 0xe6;              /* ONE pseudo -- `+=` reassigns; gcc folds the copy */

    ang = (X << 6) + 0xe6;    /* TWO pseudos -- the ROM's three instructions */

> When the ROM has `<op> rX, #n / mov rY, rX / add rY, #K` with **K > 7**, write
> it as ONE expression. The `+=` form collapses to a single pseudo.

It generalises past shifts to any value feeding a large add. The precedent was
already in the tree and unnoticed: `src/rom_9000/rom_1219c_b.c` writes
`off = ((layer & 3) << 2) + 0x28;` and emits exactly `lsl / mov / add`.

`OvlFunc_939_20092a4` was parked on this in batch 177 with the opposite reading
-- that the named-intermediate lever could not reach it because "a shift's input
and output are not simultaneously live". True, and irrelevant: the add is what
needs them live, not the shift.

## Halfword constant ZERO: the pooling class has a fix

The notebook lists halfword constant pooling as unsolved. For the constant-zero
case it is not. Storing a literal `0` to a `short` lvalue is ALWAYS a pool load:

    short *p;  *p = 0;                    ->  ldrh r3, .Ln  /  .word 0

Seven spellings measured on `Func_80b8db8`, every one producing the pool load:
`short *`, `unsigned short *`, a struct field, an `int h : 16` bitfield, `&= 0`,
and both signednesses of the pointer. **Only a register-allocated local
escapes:**

    short zero = 0;  *p = zero;           ->  mov r3, #0  /  strh

`int v = 0; *p = (short)v;` works identically. The mechanism is that the `movhi`
expander `force_const_mem`s a CONST_INT when the destination is memory; a local
gives it a register source instead.

**And watch what else that pool drags in.** The `ldrh` needs its pool within
range, so `arm_reorg` dumps the pool mid-function and inserts a branch over it --
which reads as an unrelated redundant jump.

> A stray jump-to-next-label in Thumb output is usually a POOL DUMP, not a
> jump-optimisation failure. Find the early pool user and remove it.

## PROMOTE_MODE: no type on a HImode local can give you `lsr`

`arm.h` contains

    else if (MODE == HImode)  UNSIGNEDP = TARGET_MMU_TRAPS != 0;

and `TARGET_MMU_TRAPS` is 0 in this configuration. **Every HImode local promotes
to SImode SIGN-extended, whatever it was declared.** So when the ROM has
`lsr rD, #0x10` and we emit `asr`, no amount of `unsigned short` on the local
will fix it -- `OvlFunc_960_2008ce4`'s park had recorded "declaring it unsigned
is not enough" and was right without knowing why.

The fix is on the READ side: assign the halfword into an `unsigned int` before
the use, which forces a zero-extending read.

    u = n;  *(volatile unsigned short *)ADDR = u;      /* lsr */

`(t << 16) >> 16` at the point of use is equivalent.

## A REGISTER SHARED BETWEEN TWO STORES IS NOT EVIDENCE OF A SOURCE VARIABLE

Three parks in one batch had kept a local because the ROM reused one register
across two nearby stores, and in all three the local was the whole problem.

`OvlFunc_931_2008d08` is the cleanest: its park read `mov r2, #0x14` serving two
stores as "a CARRIED value -- one `int k` shared". Six spellings of that local --
`int`, `unsigned char`, `short`, `unsigned short`, and both declaration orders --
measured **exactly 7 aligned every time**. Deleting it and writing the literal
twice matched.

That identical-count pattern is itself the tell, and the notebook already states
the rule: when unrelated spellings all give the same count, the variable's
EXISTENCE is the problem, not its form. gcc will reuse a materialised constant
across nearby stores on its own.

The same deletion closed `OvlFunc_931_200807c` (two locals holding only
addresses) and half of `OvlFunc_964_20094ac` (a named value carrying a
read-modify-write).

## Two smaller shapes from the same batch

**A dead four-byte stack local survives -O2 as a `char` array, not as a struct.**
With `struct { u8 a,b,c,d; }` gcc merges the four zero stores into one
`str r3, [sp]`; with `char buf[4]` it emits `mov r2, sp` and four `strb`, which
is the ROM. The `mov rX, sp` is forced because Thumb `strb` has no sp-relative
form, so it is the byte-granular analogue of the recorded address-taken tell.

**When gcc reuses a compared-against-zero register as a stored constant and the
ROM does not, overwrite that variable before the stores.** Writing
`if (p == 0) { buf[0] = 0; ... }` lets CSE reuse the compared pointer register as
the zero -- it knows `p == 0` on that path -- which keeps p's pseudo live to the
last store and inverts the allocation. Assigning `p = buf` FIRST kills the
equivalence.

## RANK PARKS ON ALIGNED REGIONS, NOT POSITIONAL COUNTS

A positional diff compares instruction *i* to instruction *i*. The moment one
side has an extra instruction, everything after it reports as differing, so the
one number the screen prints is useless on exactly the functions where reading
the whole listing is least practical.

`tryc.py --align` reports disagreeing REGIONS. `tools/realign.py` re-measures the
whole parked set with it. The difference is not cosmetic:

    Func_8020b64      recorded 47 of 61   ->  aligned  6
    HeightTile_4      recorded 22 of 28   ->  aligned  3
    OvlFunc_939_20092a4  recorded 38 of 55 -> aligned  2, and then MATCHED

It also **inverts recorded flag comparisons**. `OvlFunc_952_2008264`'s park
measured the default at 33 and `-fno-rerun-cse-after-loop` at 57 and concluded
the flag was a net loss; on aligned counts the flag is BETTER, 8 against 9, and
it makes all three of that function's pool loads exact.

> Any park quoting a positional count on a function with a length mismatch is
> overstating its distance, possibly by an order of magnitude. Re-measure before
> writing anything off.

## GREP A .s FOR `.section` BEFORE CONVERTING IT WHOLE

`asm/rom_8a000/rom_92950_c_c_c_c.s` held one function AND a `.section .rodata`
block exporting `.L9ed80`, which `stage1.ld` pulls on its own line and another
translation unit references. Replacing the file with a `.c` dropped the data and
the link failed with an undefined reference.

**The screen cannot catch this.** `tryc.py` compares one function's instruction
stream; it has no idea another object needs a symbol the file also defined. Only
`make compare` found it.

`tools/split_s.py` does not solve it either: it cuts at FUNCTION boundaries, so
trailing data stays with the function it follows. Split such a file by hand into
a `.c` for the code and a `.s` for the data, and give each its own linker-script
line.

## A re-loaded immediate after a join is TWO locals, not one

`OvlFunc_941_2008210` has a guarded prologue and then a join, and both halves
pass 0x15 as a stack argument. One shared local for it is the obvious reading
and it is wrong by 18 instructions. The shared pseudo's live range spans the
whole function, so `allocno_compare` ranks it below the short-lived pair locals,
gcc gives it r10, and each of the eight stack-argument sites pays

    ours   mov r3, r10 / str r3, [sp]
    rom    str r5, [sp]

The reference states the answer plainly, in a way worth learning to see:

    rom    ...
           mov  r5, #0x15        <- inside the if body
           ...
           b    .Ljoin
    .Ljoin:
           mov  r5, #0x15        <- AGAIN, on a path where r5 already holds it

**gcc does not re-materialise a value it kept live across a branch.** A second
`mov rN, #imm` on a path where rN already holds imm therefore means the original
had a SECOND VARIABLE, whose live range begins at that assignment. Splitting the
if-body's uses into their own pair shortened both ranges, lifted them above the
pair locals, and put the hot values in r5/r6 with one `str` per site. Exact.

This is the read-count rule turned around. The read-count rule says a load
followed by `mov rB, rA` is a CSEd second read of ONE object. This says a
re-materialised CONSTANT after a join is a second OBJECT. Loads collapse toward
one variable; constants split into several. In both cases the count of
materialisations in the ROM is the count of source-level things, and the
register rotation around it is the symptom, not the cause.

Cheap check in the other direction: if you have written one local for a value
the ROM materialises twice, you have merged two. Live range picks the register,
not value identity.

## The join-split lever, bounded from both sides

Batch 182 found that a constant re-materialised after a join is two locals.
Batch 183 spent five functions finding out exactly when that is true, and the
boundaries matter more than the rule, because the rule is cheap to try and its
failure modes all look like near-misses.

**IT HAS A SWITCH-ARM FORM.** `FieldMove_Target` repeats `gState + 0x24a` in two
mutually exclusive `switch` arms. All three spellings are distinguishable:

| spelling | result |
|---|---|
| folded inline | one pool word `=gState+586`; the ROM has two words and an `add` — 11 |
| one shared local | range spans the switch, priority drops, diff moves into the prologue — 13 |
| **one local per arm** | defeats the fold AND keeps the range short — **exact** |

Note the cost runs backwards from the usual dominance rule: naming the value
ADDS an instruction pair here, and that pair is what the ROM has.

**IT IS INERT WHEN BOTH USES ALREADY LEFT THE ASSIGNMENT'S BLOCK.**
`OvlFunc_898_20084a0` was flagged on `0xcccc`, but both uses already sit in
different basic blocks from the assignment, so one local is rematerialised at
each site anyway — one local and a split pair compile BYTE-IDENTICALLY. The
split is only needed when the shared range spans the join and forces a
high-numbered register. **Check the push list before splitting**: the signature
is r10 (or another high register) plus an extra `mov` at every use.

**IT CANNOT SEPARATE TWO VARIABLES HOLDING THE SAME CONSTANT.** `Func_8021390`
wants `0` before a call and `0` inside the guard, materialised twice. Constant
propagation folds two source variables holding 0 back to one `const_int` long
before allocation, so no spelling reaches it — five assignment positions, two
separate named zeros, and five types all measured exactly 36. Proved by
construction: change only the pre-call argument from `0` to `9` and gcc's whole
allocation flips to the ROM's two-materialisation shape. **The split is about
VARIABLES; the fold is about VALUES, and the fold wins.**

**AND IT IS NOT THE ONLY THING THAT LOOKS LIKE IT.** See the next section.

## Two shapes that look identical: [split] and [cse]

Both are "a constant built more than once with a label in between", and the fix
for one is inert on the other. `tools/filtered.py` now separates them, and the
test is WHAT is repeated:

    a repeated `mov rN, #imm8` feeding a STACK-ARGUMENT slot
        -> two source VARIABLES; split the local          (OvlFunc_941_2008210)

    a repeated POOLED id consumed as a register argument by a `bl`
        -> one source LITERAL that rerun-CSE commons;
           unreachable from source, needs CSE_CFLAGS      (OvlFunc_920_2008304)

On the second, four constant-facing spellings — separate named locals, the equal
spelling `0x181 << 1`, explicit `== 0`, and goto-raised label use counts — all
left the SAME 6 instructions in 4 regions, because constant propagation folds
any name back to the same `const_int`. **For a pooled constant there is no
source-level split.**

Two refinements to the recorded guard/set note while we were there. The
`GetFlag(id)` / `SetFlag(id)` pair does NOT have to be in one block or even one
arm — `OvlFunc_920_2008304` has the guard before a join and the set after it,
twice, once per arm of the outer if/else, and the pass commons both. And
`-fno-gcse`, `-fno-cse-follow-jumps`, `-fno-thread-jumps` and
`-fno-expensive-optimizations` all leave the `mov r0, rN` copies in place, so
**`-fno-gcse` not helping is positive evidence FOR `CSE_CFLAGS`**, not evidence
that the shape is unreachable.

## A dominating-block local can beat the flag

The recorded advice for a CSEd constant is to reach for
`-fno-rerun-cse-after-loop` and keep the literals. `OvlFunc_898_20084a0` is the
counter-example. With literals gcc CSEs `0xcccc` into a callee-saved register
and grows a `push` the ROM lacks — 12 aligned. The flag removes the hoist (6)
and leaves three argument-scheduling residues it cannot touch. Naming
`s = 0xcccc` in the dominating block fixes BOTH, with no flag at all, because a
rematerialised pseudo has low `rtx_cost` and drops out of
`precompute_register_parameters` — which is what lets `mov r0, #2` land between
the two pool loads.

**When one constant is both CSEd and mis-scheduled, try the local before the
flag.** The flag only addresses the first half. Two more applications of the
same lever on the same function (`t`, then `u`/`v` assigned before an `if` and
used inside it) took it 4 → 2 → exact.

## Identical counts across unrelated spellings: now look at a SIGNATURE

The notebook already says identical counts across unrelated spellings indict the
variable's existence. `OvlFunc_901_20088a8` extends where to look next. Four
spellings — a named zero, a narrower zero, a hoisted zero, a named slot — all
measured EXACTLY 2. The residue was not in any variable. It was in a
**declaration**: `__Func_8092c40` returns a value, and marking the callee `int`
makes r0 live out of the previous call, which changes what
`precompute_register_parameters` may reorder across. The two instructions were
an argument fill order, and the fix was one word in an `extern`.

So the ladder, when several unrelated spellings tie exactly:
1. delete the variable — it does not exist;
2. **check every callee's return type** — `void` versus `int` moves argument
   ordering, not just the epilogue;
3. check the build flags.

Only after all three is it a wall.

## Working note: tryc.py needs the container

`tools/tryc.py` resolves the compiler from `GCC296_DIR`, defaulting to
`/opt/gcc296`, which exists **only inside the build image**. There is no local
gcc-2.96 in this tree. Screening therefore runs as

    docker run --rm -v "$PWD:/work" -w /work goldensun-build sh -c \
      'python3 tools/tryc.py <cand.c> --ref <file.s> --align'

which is still the screen and still must not touch the build. The failure when
run outside is a bare `FileNotFoundError` on `/opt/gcc296/xgcc`, which reads
like a broken tool rather than a wrong working directory.

## A NEGATIVE multiplier is what selects the shift chain

When the ROM expands a constant multiply as a chain of shifts and adds where a
pool load and a `mul` would plainly be cheaper, that is not gcc preferring
shifts. `expand_mult` calls `synth_mult` on the ABSOLUTE value and negates
afterwards, and the cost budget it passes down comes from the NEGATIVE `MULT`.
So `x * 6553` gives `ldr r3, =0x1999 / mul` and only `x * -6553` gives the
seven-instruction chain ending in a `neg`.

**Read the sign off the ROM: a `neg` closing a shift chain means the source
multiplier was negative.** `OvlFunc_964_20090c4` has both spellings in one
function, which makes it a clean internal control.

Two cautions from the same expression:

- `C - x*k` and `x*-k + C` are indistinguishable in an ISOLATED probe — both
  fold to `ldr C / sub`. Only under the real function's register pressure does
  the negative-multiplier form keep the ROM's `neg` + `add` pair. **Isolated
  probes are not safe for sign questions**; put the expression back in the
  function before believing a tie.
- **A statement break stops the distribution.** `(r - 5) * 0x3332` folds to
  `r * 0x3332 - 65530` and emits `mul` then `ldr =0xffff0006 / add`. Written as
  `t = r; t -= 5; vx = t * 0x3332;` it gives the ROM's `sub #5 / mov / mul`.
  This is the mirror of the one-expression-not-two rule, and which direction to
  go is decided by whether the ROM distributed — not by which reads better.

## `mov rLow, rHigh` before a store is a SECOND reload

A `mov rLow, rHigh` immediately before a store whose base is that same high
register looks like a register-allocation quirk. It is a re-read of a pointer
already committed to a callee-saved register, and gcc emits it **only when a
call sits between the pointer's definition and its use**. With the definition
adjacent to the store, reload inherits the scratch and stores through it
directly, one instruction shorter.

So the instruction is evidence about CONTROL FLOW, not about statement order:
it says a `bl` intervenes. That is worth checking before spending a sweep on
declaration orders, as `OvlFunc_964_20090c4`'s park did — 34 placements, 28
declaration orders and 8 flags all left the same floor, because the question was
never where the assignment sat.

## The call-saved allocation order is NOT monotonic: r10 comes before r9

Every register-birth-order note in this file up to batch 183 reads as if gcc
hands out call-saved registers in numerical order, so that an r8/r9 diff is
"adjacent allocnos" and an r9/r10 diff is one step. That is wrong, and it has
been mis-sizing every estimate of how far a register diff is from closing.

Settled twice, and the second time corrected a working assumption worth more
than the fact itself. First with a probe (`scratch/regorder/p.c`): seven values, each born at a distinct point and
each live across a call, so the allocator must rank them and hand out call-saved
registers strictly in priority order. Compiled with the production flags, the
assignment by birth order came out

    a1 (longest-lived, lowest priority)  -> r11
    a2                                   -> r9
    a3                                   -> r10
    a4                                   -> r8
    a5                                   -> r6
    a6 (shortest, highest priority)      -> r5
    a7                                   -> stays in r0, dies into the call

Reading that highest-priority-first, the order gcc hands them out is

    r5, r6, [r7], r8, r10, r9, r11

**r10 is handed out BEFORE r9.** So a diff showing r8 and r9 swapped is allocnos
four and six trading places, not adjacent ones, and an r9/r10 difference is a
two-place move in the priority sort. When a park says "the registers are one
apart so this is nearly closed", check which two.

**AND THE COMPILER SOURCE IS IN THE BUILD IMAGE, which this notebook has been
treating as unavailable.** `/opt/camelot-gcc/gcc-2.96/gcc/` and
`/opt/camelot-gcc/agbcc/gcc/` are both present — `config/arm/arm.h`, `global.c`,
`local-alloc.c`, `combine.c`, `thumb.md`, all of it. So every citation in this
file carried on trust from a fingerprint list can simply be read:

    docker run --rm goldensun-build sh -c 'sed -n "989,996p" \
      /opt/camelot-gcc/gcc-2.96/gcc/config/arm/arm.h'

`REG_ALLOC_ORDER` is `{3, 2, 1, 0, 12, 14, 4, 5, 6, 7, 8, 10, 9, 11, 13, 15,
...}`. The call-saved run is **4, 5, 6, 7, 8, 10, 9, 11** — matching the probe
exactly, and placing r7, which the probe could not. (r4 is call-used here under
`-fcall-used-r4`, so it is skipped.)

**Check the source before recording a compiler claim.** Several notes in this
file cite `arm.h:989`, `global.c:allocno_compare` and `calls.c:805` from memory
of a fingerprint list. They happen to be right; that was not guaranteed, and the
check costs one command.

(The probe could not place r7: with a frame pointer live it was reserved
throughout. Its position above is carried over from the existing notes and the
corpus, both of which are consistent with it sitting between r6 and r8.)

## Constant CSE inside ONE basic block: closed, with a number

The notebook has called repeated-constant CSE its single most valuable open
question. One sub-case of it is now closed, not by another spelling sweep but by
reading two cost functions in the build image.

`arm_rtx_costs`, Thumb branch, `CONST_INT` with `outer == SET`, returns **0** for
a value below 256, `COSTS_N_INSNS(2)` for a shiftable value at or above 256, and
`COSTS_N_INSNS(3)` otherwise. In this tree `COSTS_N_INSNS(N)` is `N * 4 - 2`, so
those are 6 and 10 — *not* 8 and 12, which is what you get from the more common
`N * 4` definition and is worth checking before quoting.

`cse.c`'s `COST` macro scores a pseudo `REG` at **1** and sends anything else to
`notreg_cost`, which returns `rtx_cost (x, SET) * 2`. So:

| the constant | rtx_cost | CSE COST | against a pseudo's 1 |
|---|---|---|---|
| below 256 | 0 | **0** | constant wins — always rematerialised |
| shiftable, ≥ 256 | 6 | **12** | register wins — always CSEd |
| other, ≥ 256 | 10 | **20** | register wins — always CSEd |

Two things follow.

First, this is the mechanism behind the corpus rule that a bare `mov rN, #imm8`
is rematerialised for free and must not be counted as a repeat. **The boundary is
literally `< 256`**, and `filtered.py`'s duplicate detector is right to treat a
`mov`+`lsl` pair as expensive and a bare `mov` as free.

Second, and this is the closure: the COST is a property of the `const_int`, and
every C spelling of the same value folds to the same `const_int` before cse runs.
**Within a single basic block, a repeated constant of 256 or more is unreachable
from C for this compiler.** Not "we have not found the spelling" — there is no
spelling. Stop sweeping and park it.

This does NOT touch the cases that are reachable, and the distinction is the
whole practical point:

- repeats separated by a control-flow boundary are the `CSE_CFLAGS` shape when
  one use dominates the other, and need no flag at all when the uses are in
  mutually exclusive arms;
- repeats of *different* values are the split lever;
- repeats below 256 are free and were never a blocker.

Only "same value, 256 or more, one straight-line block" is closed. That is the
shape `OvlFunc_959_200cf60` is parked on, measured at `-da`: by `.03.cse` — the
FIRST cse pass — the repeats are already collapsed, and `.09.cse2`, the rerun
`-fno-rerun-cse-after-loop` disables, is byte-identical to it. The flag cannot
help because the damage is done before it runs.

## Blocker 1b, the actual mechanism — and READ THE RIGHT COMPILER TREE

Two corrections, one of which invalidates how 1b was explained earlier in this
file (not what to do about it — the recipe was right — but why).

**THERE ARE TWO COMPILER SOURCE TREES IN THE IMAGE AND THEY DISAGREE.**
`/opt/gcc296`, which is what `tools/tryc.py` and the build actually drive, is
built from **`/opt/camelot-gcc/gcc-2.96/`**. The other tree,
`/opt/camelot-gcc/agbcc/`, is a different compiler: it does not define
`REG_ALLOC_ORDER` at all, and its halfword-move pattern has different
constraints. **Reading the agbcc copy gives the wrong answer.** Some passes are
byte-identical between them — `local-alloc.c`'s `update_equiv_regs` gate is the
same rule at line 886 in gcc-2.96 and 868 in agbcc, so the `Func_80a5b94` park's
claim stands — but do not assume it. Cite `gcc-2.96`.

**1b IS A CONSTRAINT-ORDERING FACT, NOT A LIVE-RANGE FACT.** In
`gcc-2.96/gcc/config/arm/arm.md`, the Thumb halfword-move pattern's source
operand constraint string is `"l,mn,l,*h,*r,I"`. Alternative 1 accepts `n`, any
`CONST_INT`, and emits a load from the pool. The `I` alternative — the one that
would give `mov rN, #imm` — is LAST. recog takes the FIRST matching
alternative, so **for a HImode `const_int` the `mov` alternative is
unreachable**, and every halfword store of a literal goes to the pool. That is
why it reaches down to the value 1, and to 0.

So the escape is not "assign it in a dominating block". The escape is **making
the value SImode** — an `int` local — so that it is set by the *word* move
pattern and stored through a subreg. Measured in isolation: `*(short *)p = 1;`
and `*(short *)p = 0;` both pool; `int one = 1; *(short *)p = one;` gives
`mov r3, #1`.

The dominating-block effect recorded earlier in this file is real but it is a
SECOND, separate question — once the value is an `int`, *where* it is assigned
decides which register it lands in, which is ordinary blocker 2. Conflating the
two made 1b look mysterious. Restated:

1. **Is the stored constant an `int` local?** If not, it pools. Always. This is
   the whole of 1b and it is not negotiable from source in any other way.
2. **Then** place the assignment to get the register the ROM used — that is
   birth order, and `.17.lreg` will tell you what happened.

And one case that needs neither: `OvlFunc_951_200973c` reproduces
`ldr r3, =0xffff / strh` from the plainest possible spelling on a `short` struct
field, because there the ROM *wants* the pooled form. Check which side you are
on before reaching for the `int`.

## `mov rB, rA` after a pool load decides whether a global wants a pointer local

Two directions of the same lever, both now attested, with the ROM printing the
answer.

`loop.c`'s `move_movables` hoists a loop-invariant only when
`threshold * savings * lifetime >= insn_count`, and `threshold` is
`(loop_has_call ? 1 : 2) * (1 + n_non_fixed_regs)`. A global read ONCE inside a
loop has `lifetime == 1` — the address set and its use are adjacent — so in a
large loop it can never clear the bar and gcc rematerialises the pool load at
the use. **When the ROM holds a global's address in a callee-saved register
across a loop but dereferences it only once, LICM did not put it there; the
source did, as a pointer local initialised before the loop.**

When LICM *does* hoist, it inserts a copy — `ldr rA, =sym` then `mov rB, rA` —
because the pre-loop uses and the in-loop uses are separate pseudos. A pointer
local makes them one pseudo and the copy disappears.

So the tell is the copy itself:

| ROM shows | means | write |
|---|---|---|
| `ldr rA, =sym` reloaded at each use | LICM declined | a pointer local |
| `ldr rA, =sym` then `mov rB, rA` | LICM hoisted and copied | leave it a bare global |

`OvlFunc_951_200973c` has both in one function and needed both spellings — the
`-da` `.08.loop` dump prints the decision per insn ("move-insn savings N",
"not desirable"), so this is readable rather than guessable.

Related qualification to the recorded un-rotated-loop entry: `stmt.c`'s
`expand_end_loop` carries a Cygnus-local transform that turns
`start: if (test) goto end; body; goto start` into the ROM's un-rotated shape by
itself. So **`for (init; ; inc)` with a trailing `break` produces it with no
`goto` spelling at all** — and the hand-written backward `goto` is actively
worse, because it is not a natural loop to `loop.c` and gets no invariant motion
whatsoever. Try the `for` first.

## Halfword addressing: `ldrsh` and `ldrh` want OPPOSITE spellings

Two functions in batch 185 landed on the same question from opposite sides, and
together they replace the vague "the offset belongs in the load" advice with a
rule that says which way to go.

**Signed** halfword loads have no immediate-offset form in Thumb-1. The
sign-extending load pattern therefore takes a generic memory operand plus a
scratch and decides at output time: an address that is a **PLUS of two
registers** emits a single `ldrsh`; anything else must materialise a zero into
the scratch and emit a `mov rZ, #0` beside it. Two extra instructions for a
bare-register address, none for a register pair.

**Unsigned** halfword loads *do* have a register-offset form, and gcc takes it
whenever the address is a PLUS of two live pseudos. So the same source shape
that saves two instructions on `ldrsh` costs two on `ldrh`.

    ROM has  add rA, rB, rC / ldrsh rD, [rA]      -> offset as a LITERAL in the
             i.e. mov rZ,#0 beside it                address expression
    ROM has  bare ldrsh rD, [rB, rC]              -> offset as a NAMED VARIABLE

    ROM has  add rA, rB, rC / ldrh rD, [rA]       -> address as its own pseudo
                                                     (hard: gcc folds it back)
    ROM has  bare ldrh rD, [rB, rC]               -> offset as a NAMED VARIABLE

`OvlFunc_899_200a564` is the `ldrsh` case and closed on it.
`src/non_matching/rom_15000/801b424.c` is the `ldrh` case and is parked on it:
three spellings, including a pointer local assigned either side of the offset
increment, all measure exactly 12, because gcc folds `q = p + off; *(u16 *)q`
straight back into the register-offset mode.

One trap worth naming: `mov rN, #K / lsl rN, #n` appears in the ROM **either
way**, because Thumb `add` immediate stops at 255 and gcc must build any larger
offset in a register regardless. **That pair is not evidence of a named offset
variable.** Read the load's addressing form, not the offset build.

## When a backward target has extra predecessors, write the `goto`

The recorded advice is now three-way, and the tell for each is different.

- **To get the un-rotated loop shape** — `b body / inc: / body: ... bne inc` —
  write `for (init; ; inc)` with a trailing `break`. `expand_end_loop` carries a
  Cygnus-local transform that does exactly this rewrite. A hand-written `goto`
  here is WORSE (27 against 11 on `OvlFunc_951_200973c`) because it is not a
  natural loop and gets no invariant motion at all.
- **To DENY loop-invariant motion** — write the `goto`, for that same reason.
  `Debug_TransferTest` and `Func_801b424` both needed it: gcc hoisted a poll
  address out of the loop, took an extra callee-saved register, and dropped the
  per-iteration rebuild the ROM has.
- **The structural tell for the second case**: count the predecessors of the
  backward target. `Func_801b424`'s loop head is branched to from THREE places —
  the latch plus two `continue` paths at the bottom — and no single `for` or
  `while` produces that shape. More predecessors than just the latch means the
  original had a label.

## `-fsched-verbose=6` is the standard probe for blocker class 5

Class 5 has always been recorded as "nothing I tried moved it". It does not have
to be. `-fsched-verbose=6` prints the ready list with each insn's priority, and
`rank_for_schedule` returns on priority first — so the table says directly
whether a scheduling gap is one tie-break away or structurally impossible.

`Func_8077f70` is the first park in the class backed by that table rather than
by an exhausted search. The store that must move has priority 34; the shift that
takes its slot has 36. The only way our store reaches 36 is an ANTI-DEPENDENCE
on that shift — reading the register the shift writes. The shift writes exactly
one register, and our store's source is a different one, so **no C spelling that
keeps this instruction set can create the dependence**. `.20.ce2` already holds
the ROM's order; sched2 sinks the store afterwards.

Run it before writing "nothing moves it". A proof and an exhausted search read
the same in a park and are worth very different things to the next reader.

## Loop-invariant motion: the SECOND pass is what gets you

`loop.c` moves a movable when `threshold * savings * lifetime >= insn_count`,
and then subtracts 3 from the threshold **after each move**. So hoisting one
invariant makes the next one cheaper, and gcc runs the loop optimiser twice.

`Func_8077f70`'s `.08.loop` dumps show a pooled constant refused on pass 1 in
both the inner and the outer loop, then moved on pass 2 — because pass 1 had
hoisted a mask first and shrunk the loop by two instructions. Those two verdicts
bracket the threshold at 15..17, so defeating the hoist by growing the loop would
need 18 instructions, which a 13-instruction loop cannot reach. Confirmed by
construction: `-fno-rerun-loop-opt` on the plain `for` reproduces the `goto`
version exactly.

**Amendment to the `goto` note: a backward `goto` denies ALL invariant motion,
not only the motion you wanted stopped.** Where the ROM keeps something outside
the loop, the `goto` must be paired with hoisting that thing by hand. On this
function `goto` alone is 15 and `goto` plus the hand-hoisted mask is 7.

## Two qualifiers on tells that were stated too broadly

**A pooled small constant whose consumer is a HALFWORD STORE is blocker 1b, not
a symbol.** The recorded tell says a pooled value that `mov` could build means
the source named a linker symbol. `Func_8077f70` pools a 16 — and so does our
own compiler, for the same store, with no symbol involved. Check the consumer
before adding anything to a `.sym` file.

**The `[offset]` question is inert at `ldrsh` sites.** Thumb `ldrsh` has no
immediate-offset form, so the offset must reach a register whichever way it is
written; bare literals and per-block offset locals give byte-identical output,
measured A/B on six sites. The recorded warning about per-block offset locals is
about offsets that could otherwise fold into a load *immediate*. At an `ldrsh`
site there is nothing to police.

## Do not disable sched2 while testing the declaration lever

Leaving a callee implicitly declared was worth 18 → 9 on `Func_8077f70`, across
four calls where the ROM fills r1 before r0. But under `--no-sched2` **both**
forms come out wrong and equal. The ROM's argument order is produced by sched2
*fed* the implicit declaration's operand order — not by the declaration alone.
Testing the two together hides the lever.

## Two shift statements beat a `(short)` cast

For an in-place sign extension, `x = (short)x;` builds a sign-extend pattern
with a clobber and reload hands it a scratch register, giving a three-register
`lsl / asr`. Written as `x <<= 16; x >>= 16;` on the same variable, gcc emits the
ROM's destructive two-register pair. Worth 7 → 4 on `Func_8077f70`, and it also
stopped an unrelated store from being sunk.

## Write the redundant compare — the `else if` chain IS the spelling

When the ROM re-tests a scalar against a value it has already tested in an
earlier arm, that reads like redundant codegen and invites a tidier nesting. It
is not redundant: it is the literal shape of

    if      (y == K && !a && !b) { ... }
    else if (y == K && a)        { ... }

Nesting the second arm under one shared `y == K` — the version a reviewer would
prefer — **loses the compare**. `OvlFunc_924_200a1cc` matched on its first
spelling because the chain was written out longhand. The shared tails cross-jump
on their own afterwards; no `goto` and no lever is needed.

## Check the immediates before naming a shifted constant

The reflex when the ROM interleaves `mov / mov / lsl / lsl` for two shifted
arguments is to name both as locals. That is right only when the two immediates
DIFFER. `OvlFunc_924_200a1cc` got the interleave free from bare shifted literals
because both constants share the same `mov` byte and gcc batches identical
values; a sibling calling the same helper needed the locals precisely because
its immediates differ. One glance at the two bytes decides it.

## Grep the corpus for a solved neighbour before writing anything

`Func_8077f70`'s first screen was 26 of 123 because a solved function in the
same bank turned out to be verbatim its middle two-thirds. That is the single
largest first-screen improvement recorded in this file, and it cost one grep.

## The selection filter was calibrated on the wrong functions

`tools/filtered.py` returned FOUR candidates out of 1,251 unparked functions,
which read like the pipeline was exhausted. It was not. The filter's thresholds
had never been checked against this project's own record, and measured over the
3,474 compiler-output `.s` files in the tree they are wrong in the large:

| the filter rejects | share of ALREADY-MATCHED functions it would have rejected |
|---|---|
| fewer than 8 calls | **85%** |
| outside 40–120 instructions | **77%** |
| uses r8–r11 | 8% |

Median matched size is **21 instructions** — below the filter's own floor of 40.
So `calls >= 8` would have thrown away five sixths of this project's successes,
and the size band nearly as many. Those numbers describe the functions whoever
wrote the filter happened to be working on, not the ones that yield.

`--wide` is a second ranking that keeps only the checks that survive contact
with the record: hand-written assembly excluded, a same-block repeated expensive
constant excluded (proved unreachable — within one basic block a constant ≥ 256
always loses to a pseudo in `cse.c`'s cost model), everything else ranked by
size, smallest first. r8–r11 and the call count are *reported*, not rejected.
That is **768 candidates** where the old filter offered four.

**The general lesson is about tooling, not about this filter.** A screen whose
thresholds are set from the cases in front of you at the time will quietly
narrow to those cases and then report the work as finished. Check a selection
rule against the outcomes it is supposed to predict before believing it.

## Hand-written assembly is not an elevation candidate, and the test is PER FILE

Two patterns that gcc-2.96 cannot emit:

    mov r12, lr  ...  bx r12     saving the link register in ip instead of a
                                 push/pop frame
    bl .Lnnnn                    branch-and-link to a LOCAL label, usually
                                 inside another function

Verified against ground truth: **zero** of the 3,474 compiler-output `.s` files
contain either, while 38 unparked functions do. Twelve are the MP2K sound driver
in `asm/rom_f9000/rom_f95e0.s`, which ships as hand-written assembly in real GBA
titles and was never C to begin with.

**Test the FILE, not the function.** A `.s` builds one object, and an object is
either compiled or assembled — never both — so one hand-written routine condemns
its whole translation unit. Checking per function lets the small helpers
through, and they are exactly the ones that hurt: a five-instruction multiply
helper with no calls and no `r12` idiom of its own sorts to the very TOP of a
size-calibrated ranking while being just as unreachable as the driver around it.
File-scoping the test removed 28 such entries from the head of `--wide`.

## Duplicated ROM code means duplicated SOURCE — write it out

Three results this round say the same thing from different angles, and it runs
against the instinct to factor.

**A shared third block in a two-arm ROM is evidence of CROSS-JUMPING, not of a
shared source block.** `OvlFunc_899_20085bc` is a plain two-level `if/else` with
both inner arms written out in full. gcc cross-jumps the two identical B copies
— same block, same successor — producing exactly the ROM's layout, and it does
**not** merge the two A copies even though they are byte-identical too. One
merge happened and one did not. Hand-performing the merge with a `goto` is
measurably worse. Write the duplicate and let gcc decide.

**A `static` helper is not the same as duplicated source.** Factoring
`OvlFunc_969_200b7c4`'s repeated three-field test into a function called twice
went to **37** differing. gcc-2.96 inlines it but shares the two copies'
structure differently than it shares two written-out copies.

**And a dead statement is source too.** That function opens with a compare, a
branch, and a single load whose destination is redefined on the very next
instruction — the residue of a complete test whose result is overwritten by the
next statement. gcc does not delete the dead non-volatile load. **An isolated
compare-and-branch whose only guarded instruction is a load into a register that
is immediately redefined means a whole statement was dead in the source.**
Transcribe it; do not explain it away as an optimisation artefact.

## Branch polarity has a THIRD face: boolean MATERIALISATION

For one predicate gcc-2.96 emits three shapes that are not interchangeable:

    ok = (A && B && C);                    false-first — the 0 is hoisted to the
                                           TOP of the compare chain, 1 out of line
    ok = 1; if (!(A && B && C)) ok = 0;    a third shape again
    if (A && B && C) ok = 1; else ok = 0;  true-first — the 1 sits INSIDE the last
                                           compare's block, 0 out of line

So the recorded "which side of the `if` carries the exit picks the condition"
extends to flag variables: **which literal is OUT OF LINE tells you whether the
source was an expression-assignment or an `if/else` statement.** The expression
form puts the default out of line; the statement form puts the else arm out.

Two cheap things that are *not* levers on this shape, both measured: De Morgan
rewriting compiled byte-identically to the and-form, and the flag's type is free
(`int` and `unsigned char` both match). Do not spend a round on either.

## A large early deficit is NOT diagnosed by branch polarity

Worth recording as a negative, because it was my working hypothesis and it cost
a round. On `OvlFunc_899_20085bc` I assumed a 55-of-88 deficit starting early
meant the arms were in the wrong order. Reordering them bought **one**
instruction. The deficit was the tail.

**Polarity errors show up as isolated single-instruction REPLACEs.** A large
contiguous deficit means a structural misreading — the wrong statement shape,
not the wrong condition.

## Correction: literals stored through a halfword lvalue ALWAYS pool

The recorded 1b table says small values in roughly 1..0x7fff need no local
because they emit `mov`. Measured on `OvlFunc_899_20085bc`, that is wrong for
anything stored through a halfword lvalue: storing a literal `5` through a
`short *` pools it and costs four bytes of pool, and only `int five = 5;` gives
the ROM's `mov`. Both the `5` site and the `0` site needed it.

The honest rule for this compiler: **every literal stored through a halfword
lvalue pools, and the escape is always an `int` local.** The recorded
counter-examples were `u16` *struct members of a wider object*, which may reach
a different pattern than a `strh` through a pointer — that wants a corpus
re-check before the section is rewritten, so it is flagged here rather than
edited there.

Related, and needing no construct at all: the pooled HImode **zero** falls out of
the natural store-then-retest, because cse store-forwards the second read to an
HImode constant. The `unsigned short` struct-or-array trick recorded elsewhere is
unnecessary when the ROM itself re-reads the field.

## `tryc.py` has a fourth label false-negative: the jump-over-pool join

When a jump-over-pool's target lands on the **same address** as an existing join
label, gcc emits two labels there and the reference — being a disassembly —
can only show one. `--align` reports two differing instructions and the output
is byte-identical.

**When the entire residue is a `b Lx / Lx:` pair with no instruction between the
two labels, assemble both sides and `cmp` before treating it as a diff:**

    docker run --rm -v "$PWD:/work" -w /work goldensun-build bash -c '
      arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o /tmp/a.o <ref-only.s>
      /opt/gcc296/xgcc -B/opt/gcc296/ -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi \
        -fno-builtin -nostdinc -ffreestanding -fcall-used-r4 -Iinclude -S -o /tmp/f.s <cand.c>
      arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o /tmp/f.o /tmp/f.s
      arm-none-eabi-objcopy -O binary -j .text /tmp/a.o /tmp/a.bin
      arm-none-eabi-objcopy -O binary -j .text /tmp/f.o /tmp/f.bin
      cmp /tmp/a.bin /tmp/f.bin && echo IDENTICAL'

`make compare` remains the authority, but this settles it without touching the
build.

## A read of r9 with no defining write is a STATIC CHAIN — the original was nested

New class, read out of the compiler and confirmed from the caller.
`gcc-2.96/gcc/config/arm/arm.h` sets `STATIC_CHAIN_REGNUM` to r8 under ARM and
**r9 under Thumb**. So a Thumb function that *reads* r9 without ever defining it
is reading a static chain pointer, and the original source declared it as a
**nested function**.

The tell is a triple, and all three parts should be present:

1. r9 saved in the prologue and restored in the epilogue;
2. a `mov rX, r9` that **no instruction in the function ever defines**;
3. a lone stack slot that nothing reads back.

**The caller settles it, and gives the corpus-wide grep:**

    add rN, sp, #K
    mov r9, rN
    bl  <target>

That is gcc handing a callee a pointer into the caller's own frame.
`Func_8016018` does it before each of its three calls to `Func_8015fb8`.

**A standalone translation unit cannot declare a nested function**, so the chain
has to be transcribed rather than expressed: an uninitialised `register` bound
to r9, copied into a **`volatile`** stack slot as the first statement. The
`volatile` is load-bearing — without it gcc dead-store-eliminates the slot *and*
the whole r9 save/restore with it (12 differing → 2), and pointer indirection
does not substitute for it.

**Treat that transcription as provisional.** If the caller is in the same parent
`.s` — as it was here — then once that piece is elevated the pair can be written
the way the original almost certainly was, with the callee nested inside the
caller, and both the register binding and the volatile slot disappear. Record
the intent in the file header so the next person does not preserve a workaround
that has stopped being necessary.

Distinguish this from the recorded note that a dead `mov rN, r14` after
`push {lr}` means an uninitialised read. That one is about r14 and about garbage;
this is about r9 and about a real, caller-supplied value.

## Correction: a gcc pool CAN mix a symbol with integer constants

A recorded sweep claims that no gcc-generated literal pool in this tree mixes a
symbol with integer constants. `Func_8015fb8` is a counterexample on both sides —
five constants and a function address in one pool — and gcc's reference-order
emission reproduced the ROM's pool byte for byte.

That weakens the "pool layout is uncontrollable" worry for the mixed case, at
least when reference order and pool order coincide.

## Grep on CALLEE NAMES and the GLOBAL, not on the target's stem

The habit of looking for a solved neighbour before writing anything paid twice
this round with zero-iteration matches, and `Func_80a3e28` sharpened *how* to
look. Its stem-sibling is literally the function it tail-calls, and that sibling
was **less** useful — it walks the same array to clear slots rather than to make
this call. The useful neighbour was in a different bank: a solved function
calling **both** of this one's callees over the same global's node array, with
the same post-increment read, the same skip-if-zero guard and the same
descending counter.

**Callee-set identity beats filename adjacency.** Grep for the callee names and
for the global, then read what comes back.

One negative worth recording so nobody builds it into a tool: "has an
already-elevated sibling in the same family" is **not** a useful ranking signal —
726 of 764 remaining candidates have one. It is a near-universal working habit,
not a discriminator.

## gcse hashes the MEMORY ALIAS SET — different struct tags defeat commoning

The most reusable finding in batch 187, and the first source-level equivalent
this notebook has for `-fno-gcse`.

`gcse.c`'s `hash_expr_1` folds `MEM_ALIAS_SET` into the expression hash. Two
loads of the **same address** therefore land in different buckets — and are never
compared at all — whenever their alias sets differ. And gcc-2.96 assigns a
**distinct alias set per struct tag**: probed, two different struct tags over the
same address get different sets, while a bare `short *` and an `unsigned short *`
share one.

> **When the ROM RELOADS a field that gcse would otherwise common, reach the two
> reads through DIFFERENT STRUCT TAGS** — or through a struct tag and a bare
> pointer of another type.

Verified against the flag: `-fno-gcse` on the single-alias-set spelling
reproduces exactly the same reload. Worth 49 instructions on
`OvlFunc_899_2008690`.

**It depends on strict aliasing being ON.** A file using this must never fall
under an `-fno-strict-aliasing` rule — with the flag the same source grows from
316 bytes to 344. Check the Makefile before relying on it.

## Where the store goes decides what gcc cross-jumps

"Duplicated ROM code means duplicated source" is right, but "let gcc decide what
to merge" is **not passive** — store placement decides it.

Read from `jump.c`: cross-jumping runs **once**, after scheduling. For a simple
jump it first tries to merge against the block that *falls through into* the
target label, and only then runs the pairwise `jump_chain` search — which is
gated on the target label's uid being below a maximum **fixed at pass entry**.
So any label the pass itself creates fails that test and its chain is never
searched pairwise.

> **Write the store inside each innermost branch.** The first merge folds every
> arm's identical store into a *new* join label; from then on the arms can only
> be compared against whatever physically falls into that join, never against
> each other. The last arm's `else` body absorbs the other `else` bodies and the
> `if` bodies survive as separate copies — which is what the ROM has.

Writing the store *after* the `if/else`, or after the switch, leaves the arms
jumping to a **pre-existing** label, the pairwise search runs, and gcc collapses
the duplicates. 93 differing against 0 on the same function.

The sibling `OvlFunc_899_20085bc` matched by accident of already having the
right shape; this one fails at 93 without the rule.

## A large diff can be several blockers stacked

`OvlFunc_899_2008690` had two candidates tie at **exactly 91**, which the
notebook's own rule reads as "the residue is not in the variables you are
changing". That was right, but the follow-up guess — wrong arm order — was
wrong, and reordering the arms alone made it *worse* (93).

The deficit was two independent structural facts: gcse commoning a reload (49)
and cross-jumping collapsing an arm (29). **An identical tie across spellings
says the residue is structural; it does not say the structure is one thing.**

## Blocker 1b also protects an ADDEND

With a store written inside its branch, a written `+ 0xffff` is converted to the
halfword type and `convert_to_integer` distributes it into HImode, where it folds
to a subtract. The ROM's pooled load and add needs the value parked in an `int`
local first — the same SImode escape as 1b, but protecting an **addend** rather
than a stored literal. The sibling arm's `+ 1` needs no local.

## Constant hoisting out of a loop is arithmetic, and the threshold is 15

`loop.c` computes `threshold = (has_call ? 1 : 2) * (1 + n_non_fixed_regs)` and
moves a movable when `threshold * savings * lifetime >= insn_count`. For a Thumb
function containing a call that threshold is **15** — measured from `.08.loop`,
which says *moved* at 15 real insns and *not desirable* at 16.

A constant that `expand` creates for a store has `savings 1, lifetime 1`, so:

> **A loop of ≤ 15 RTL insns hoists every such constant into a callee-saved high
> register; a loop of ≥ 16 hoists none.**

If the ROM keeps a constant *inside* a loop while hoisting others, count the RTL
insns before trying spellings.

Two corollaries. **The count must clear the threshold on BOTH loop passes** —
`-frerun-loop-opt` is on at `-O2` and the dump prints two counts, so anything
that moves an insn out on pass 1 buys nothing; one candidate shrank 16 → 15 on
pass 1 and pass 2 then hoisted the constant that should have stayed. And **the
cheap, output-neutral way to add an RTL insn is a narrowing temporary**: read a
byte field into an int-width local and store back through an `unsigned char`
one — two RTL insns at loop time, both folded away by combine. Every other way
of adding an insn was either deleted before `loop` ran or survived into the
output.

## An `and` accumulating into the wrong register is a SUBREG question

The Thumb `and` pattern ties its destination to its first input. When that input
is a `(subreg:SI (reg:QI ...))` — which is what masking a byte field directly
produces — `regmove` refuses the tie and inserts its copy on the *other* operand,
so the mask lands in the destination instead of the value.

Loading the byte into an **int-width local first** makes the operand a plain
`REG`, the tie lands on the value, and the ROM's order comes out. Swapping the
operands in the source (`mask & p[k]` instead of `p[k] & mask`) changes nothing —
byte-for-byte identical. **This is a subreg-versus-register question, not an
operand-order one.**

Related, and separate: **which of two preheader constants gets which high
register is source assignment order** — the second-assigned wins the
lower-numbered register — and the *gap* between the two assignments matters on
its own. Assigning them adjacently let a later pass build one of them an
instruction shorter than the ROM; putting a third initialiser between them
restored the ROM's form. A full permutation sweep of the preheader initialisers
is cheap and was what closed the last two instructions.

## The block-duplicate test is a SELECTION filter, not an unreachability proof

Batch 184 proved that a repeated constant of 256 or more **within one basic
block** is unreachable from C, because `cse.c` scores a pseudo at 1 and any
larger constant at 12 or 20, so the register always wins. That proof is sound.

`filtered.py`'s `duplicate_class` returning `"block"` is **not** evidence that a
function is in that class, and an attempt to classify the remaining corpus on it
failed completely. Of 1,234 unparked functions it flagged 406; after three
rounds of tightening, the number that survived scrutiny was **zero**. Every
single one dissolved. The false-positive classes, in the order they were found:

**1. A call between the two sites.** A call clobbers the argument registers, so a
constant used as an argument is rebuilt at each site whatever cse would prefer.
Three consecutive calls taking the same two constants look exactly like a
straight-line triple materialisation. 250 of the 406.

**2. The same value needed in several registers AT ONCE.** `f(-1, -1, -1, 0)`
emits three `mov`/`neg` pairs into r0, r1 and r2 with nothing between them. They
are simultaneously live, so cse *cannot* merge them — this is the most ordinary
code there is. 139 of the remaining 156.

**3. An INDIRECT call, which a `bl` test does not see.** gcc-2.96 calls through a
register with `mov r12, pc / bx rN`. Two sites reported as separated by neither a
label nor a call turned out to be arguments rebuilt for two successive indirect
calls. `filtered.py` now recognises this (see `CALL`), but the lesson is the
general one.

Also dissolved: seven `REG_DMA3SAD` repeats, which are simply `include/dma.h`'s
macro invoked twice — each invocation binds the base register afresh.

**The rule to take from this:** the marker says "this shape is worth a look", and
nothing more. **Unreachability is established per function, by measurement, not
by a pattern match over the listing.** The only class that can honestly be
excluded without an attempt is hand-written assembly, which is a different
question — those files were never C.

A corollary for planning: **there is no cheap way to predict which of the
remaining functions will need parking.** Parking is the outcome of an attempt.

## Constant rematerialisation needs a DOMINATING BRANCH, because cprop is cross-block

The ROM often rebuilds the same two-instruction constant (`mov rN, #C / lsl rN,
#n`) at each of several call sites, where gcc builds it once, parks it in a
callee-saved register and copies. That shape is sometimes reachable and
sometimes not, and **which one it is can be decided by inspection, before any
spelling is tried.**

Two passes are involved and only the second can undo the commoning.

**cse1 commons the repeat unconditionally.** MEASURED on the solved
`OvlFunc_953_200a3e0`, whose source names `y1..y6` all `= 0x93 << 2`: only ONE
`(set (reg) (const_int 588))` survives `.03.cse`; the other five become copies
carrying `REG_EQUAL`. **So separate named locals do NOT defeat CSE.** The
folklore that they do is false, and it is worse than useless — on
`OvlFunc_953_200a5f0` the named-local spellings measured 40 and 41 against 29
for plain literals.

**What restores the constants at their uses is gcse's constant propagation, and
cprop is strictly CROSS-BLOCK.** Read from `gcse.c`: `cprop_insn` skips a use
when `! oprs_not_set_p (reg_used->reg_rtx, insn)`, with gcc's own comment *"If
the register has already been set in this block, there's nothing we can do."*
And `find_avail_set` only accepts a set available at the START of the block
(`TEST_BIT (cprop_avin[BLOCK_NUM (insn)], ...)`).

So the test is one question: **is there a branch that DOMINATES the repeated
uses?**

- `OvlFunc_953_200a3e0` — yes. Its coordinate assignments sit in block 0, above
  a leading `if (__GetFlag(5))`, and its uses sit in later blocks. cprop
  restores six separate constant sets; each pseudo then satisfies
  `REG_N_REFS == 2 && REG_BASIC_BLOCK < 0`, `update_equiv_regs` marks it
  replaceable, and all twelve coordinate pseudos vanish. That file gets
  `push {lr}` alone.
- `OvlFunc_953_200a5f0` — no. Its only branch is *after* every constant use, so
  all three uses live in block 0. MEASURED: `.17.lreg` reads *"Register 35 used
  4 times across 28 insns in block 0"*, and `.18.greg` says `;; 0 regs to
  allocate` with `35 in 5`. Parked.

**The rule: if the ROM rebuilds the same multi-instruction constant at two or
more sites, look for a branch that dominates them. If the only branch is after
them, or there is none, the shape is unreachable — park immediately and do not
sweep spellings.**

This is the second consequence of the batch-152 straight-line boundary. That
entry noted only that `REG_BASIC_BLOCK < 0` never holds in a branchless
function. The constant-remat consequence is the larger half.

Checked so nobody repeats it: a scan of all 3105 generated `asm/**/*.s` for a
function that rebuilds the same `mov #C / lsl #n` pair twice inside one basic
block finds exactly three, and all three are high-register-pressure spill cases,
not constant remat. There is no solved precedent for the shape.

## CORRECTED: the "same-value movs" class is really MOV ORDER SLAVED TO SHIFT ORDER

Batch 192 closed `OvlFunc_881_200b2f0` and explained a transposed pair of `mov`s
by saying that **all the registers receive the same value, so nothing orders
them**. That explanation has been carried since and it is WRONG. Measured on a
minimal reproducer -- an `extern void f(int,int,int)` and nothing else:

    q1 = 0x80; q2 = 0x80; q2 <<= 9; q0 = 9; q1 <<= 10;   ->  mov r2 / mov r1 / lsl r2 / mov r0 / lsl r1
    q1 = 0x81; q2 = 0x80; q2 <<= 9; q0 = 9; q1 <<= 10;   ->  mov r2 / mov r1 / lsl r2 / mov r0 / lsl r1

**DIFFERENT VALUES BEHAVE IDENTICALLY.** Equality of the constants has nothing
to do with it, so any park whose reasoning rests on "the same value, so nothing
orders them" is resting on a coincidence of the case it was written from.

THE ACTUAL RULE: **gcc emits the two `mov`s in the order their consuming SHIFTS
appear.** Shift r2 first and `mov r2` goes first; shift r1 first and `mov r1`
goes first. Source assignment order, declaration order, a `do { } while (0)`
barrier, `__asm__ volatile("")`, building one constant in two steps, and
deriving one operand from the other were all measured and are all INERT --
seven forms, byte-identical.

    q1 = 0x80; q2 = 0x80; q1 <<= 10; q2 <<= 9; q0 = 9;   ->  mov r1 / mov r2 / lsl r1 / lsl r2 / mov r0

### The trap: the mov order and the shift order cannot be set independently

The ROM shape that stalled two functions wants the `mov`s in one order and the
shifts in the OTHER:

    target   mov r1 / mov r2 / lsl r2 / mov r0 / lsl r1

Writing the third argument INLINE while the second stays pinned does break the
coupling -- `f(q0, q1, 0x80 << 9)` gives `mov r1` first -- but it takes the tail
with it:

    f(q0, q1, 0x80 << 9)                     ->  mov r1 / mov r2 / mov r0 / lsl r1 / lsl r2
    f(q0, q1 << 10, q2)  and three others    ->  mov r2 / mov r1 / lsl r2 / mov r0 / lsl r1

So there are TWO reachable states and they are mutually exclusive: the `mov`
pair correct with the tail wrong, or the tail correct with the pair swapped.
The ROM is a third. This is structurally the same two-state trap measured on
`src/non_matching/ovl_7ac2d8/200cf44.c`, reached from a different direction.

**Diagnostic value:** the cheap test is to compare the ROM's mov order against
its shift order. If they AGREE, the site is ordinary and needs no lever. If they
are CROSSED, no arrangement of pins, ~~barriers~~ or statement order reaches it,
and the function should be parked on that basis rather than swept. Two
functions, `src/non_matching/ovl_7c460c/2008ff0.c` at 2 of 157 and
`src/non_matching/ovl_7d30e0/2008b68.c`, stalled here in consecutive rounds.

**THE SENTENCE ABOVE IS WRONG ABOUT BARRIERS AND ABOUT PARKING, and it is left
standing only because `tools/crossed.py` was built from it. Read the next
subsection before acting on it.** Both functions it names are now elevated.

Still open: why the batch-192 interleave DID close `OvlFunc_881_200b2f0`. That
site interleaves each `mov` with the `neg` that consumes it, which is the same
shape as a shift consuming a mov, so it should be subject to this rule too. The
`__Func_8012330(-1, -1, 0xe666)` call in `2008b68` is written in exactly that
interleaved form and still comes out swapped. One of the two cases has a
property the other lacks and it has not been identified.

### CORRECTED AGAIN: a VOLATILE ASM ON THE FIRST MOV reaches the crossed case

The two-state trap above is real and every measurement in it holds. The
conclusion drawn from it -- park, do not sweep -- does not, and it cost two
functions for two rounds. Both are now elevated:
`src/overlays/rom_7c460c/ovl_314_c_a_c_a.c` (2 of 157 to exact) and
`src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_a_c_a.c`, plus
`src/overlays/rom_7eaf28/ovl_314_c_a_c_c_c_c_c_c_c_c_c_c_c_a_b.c`, which
`crossed.py` reports as `AVOID` and which matched anyway.

The lever is one line, placed after the FIRST mov the ROM issues:

    q1 = 0xdc; __asm__ volatile ("" : : "r" (q1)); q2 = 0x9d; q2 <<= 3; ...

**Why the seven forms in the 2008ff0 park could not find it.** Every one of them
varied something the SOURCE controls -- operand order, pin presence, pin scope,
how the two mov/shift chains interleave. The mov order is not decided there. It
is decided in the post-reload scheduler, which orders the pair by which shift
consumes first, exactly as that park worked out. An operand rewrite cannot
express "materialise this one first"; a volatile asm can, because it CONSUMES
the register (so the mov must precede it) and PRODUCES nothing (so there is no
value for gcc to copy forward instead of rebuilding the immediate). That second
half is the point -- the park had already measured that introducing a real
dependence emits `mov rN, rM` rather than the immediate
(`src/non_matching/ovl_793768/2008e0c.c`), and correctly rejected every
construct that would. It asked in its closing paragraph for a construct with
both properties. This is that construct; nobody tried it.

So the inspection test keeps its diagnostic value and loses its verdict:
**CROSSED means go straight to a volatile-asm barrier on the first mov. It does
not mean park.** `tools/crossed.py` prints `BARRIER` rather than `AVOID` for
this reason; its verdict is a route, not a rejection.

This also answers half the "still open" question below it. The batch-192
interleave closed `OvlFunc_881_200b2f0` and did not close the `-1, -1, 0xe666`
call in `2008b68` because the interleave is an operand-level device and the
constraint is a scheduling one -- when it appears to work it is because the
schedule happened to agree, not because it was expressed. The barrier closes
that call too.

**THE BARRIER IS ONLY AVAILABLE WHERE THE ROM DOES NOT USE r8-r11.** This is
the sharpest boundary on it and it is worth checking before writing the line.
Measured on five functions, and the split is total:

    OvlFunc_960_2008838   hi-reg insns 0   barrier reaches the site
    OvlFunc_939_2008ff0   hi-reg insns 0   barrier reaches the site
    OvlFunc_948_2008b68   hi-reg insns 0   barrier reaches the site
    OvlFunc_961_2008120   hi-reg insns 8   barrier REWRITES THE FUNCTION
    OvlFunc_901_2008c1c   hi-reg insns 5   barrier REWRITES THE FUNCTION

The mechanism follows from what the barrier is. Splitting the block into two
scheduling regions shortens every live range that crossed the split. Where the
ROM's own allocation DEPENDS on those ranges being long -- which is exactly what
a `mov r5, r8 / push {r5, r6}` prologue records -- shortening them removes the
spill, and the function comes out two instructions SHORT with the whole
allocation renumbered. On `2008120` that is 2 of 48 becoming 45 of 46; on
`2008c1c`, 2 of 75 becoming 65 of 74. In both, the residue the barrier was
aimed at is real and the cure costs an order of magnitude more than it buys.

Note which way round this runs, because it is the opposite of the usual reading
of high registers. Elsewhere r8-r11 traffic is a tell that GCC hoisted something
it should not have, and pinning removes it -- that is what happened in both
`2008ff0` and `2008b68`, whose parks record an eight-instruction spill that the
pins deleted. By the time the barrier went in, those functions had no
high-register traffic left to disturb. Here the ROM ITSELF spills, the spill is
correct, and it must be preserved.

**So: read the ROM's high-register count first. Zero means the barrier is
available. Non-zero means it is not, whatever the residue looks like.**
`tools/templated.py` already prints this as its `hi` column, computed for a
different reason -- it ranks candidates and warns that high-register traffic
predicts an intractable allocation residue. The same number answers this
question, so no new screen is needed.

The pin is unaffected by any of this: on `2008120` an r2 pin alone leaves the
score exactly where the plain literal had it, and on `2008c1c` a plain copy
local does the same. Pins are cheap in a high-pressure function; barriers are
not.

**Scope, measured, so this does not get over-read in the other direction.** The
barrier does not go on every site: on `2008b68` the two barriers alone, applied
while an earlier divergence was still open, scored WORSE than the park's
baseline. That was an artifact -- everything after the first difference is
misaligned, so a correct downstream fix counts as noise -- and it is a trap
worth naming on its own. **The differing count is only meaningful for the FIRST
divergence. Fix that, then judge anything applied after it.** With the earlier
residue closed, tearing the same two barriers back out gives 5 differing at
precisely the two sites they serve.

### CORRECTED: a CALL-CLOBBERED PIN reaches this class without any branch

The analysis above is right about the passes and wrong about the conclusion.
"Park immediately and do not sweep spellings" cost real functions, and the
boundary is not where it says.

MEASURED on `OvlFunc_963_2008334` (elevated, `src/overlays/rom_7ec968/
ovl_30_c_c_a_c_c.c`): sixty instructions, **not one conditional branch
anywhere**, and the ROM rebuilds `mov #0x80 / lsl #9` at each of two calls plus
three more constants at four others. By the rule above this is unreachable.
The plain spelling behaves exactly as predicted -- 35 of 60, length already
exact, the constants hoisted into r5/r6, `push {r5, r6, lr}`, later calls fed
by `mov r1, r5`. Inline literals instead of named locals are BYTE-IDENTICAL,
confirming the cse1 half of the analysis.

Pinning the argument registers matches on the first try:

    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");
    p1 = 0x80; p2 = 0x80; p0 = 8; p1 <<= 9; p2 <<= 8;
    __MapActor_SetSpeed(p0, p1, p2);

repeated per call, with the assignment order read off the ROM.

**Why the rule mis-stated its own boundary.** What the shape needs is for the
constant to be DEAD at the next use, so nothing can be copied forward. A
dominating branch is one way to arrange that, and it was the only way anyone had
tried, so it got written down as the requirement. It is not. `r0`-`r3` are
CALL-CLOBBERED: a constant pinned there cannot survive a `bl`, so gcc has no
choice but to rebuild it, and no branch is involved. The pin arranges deadness
directly.

So the inspection test keeps its value but flips its verdict: **no dominating
branch means the ordinary spellings are hopeless -- go straight to a
call-clobbered pin, do not sweep.** The branch case still works and needs no
pin, so prefer the plain form where a dominating branch exists; a pin is a
fakematch and costs an entry in `fakematch.txt`.

The scan quoted above -- three instances, all spill cases -- was looking for
solved precedent among GENERATED asm and could not have found this one, because
until now no such function had been solved.

Scope, so this is not over-read. The pin makes the pinned register's own value
dead across a call. It does NOT order two other movs the post-allocation
scheduler may swap (`src/non_matching/ovl_7ac2d8/200cf44.c` measures that
boundary over seven structurally distinct forms), and it does NOT buy a live
register where there is none -- against register pressure it pays exactly what
an ordinary `int` local pays (`src/non_matching/overlays/common1_148.c`
measures that one: the pin lands on the same 4 differing as the int local).

### A SECOND CURE FOR THE CROSSED CASE: write the SHIFTS in the MOVS' order

The volatile-asm barrier is the general answer and has closed a dozen crossed
sites since batch 206. It is not free -- it is unavailable where the ROM spills
r8-r11 (batch 207), and on `OvlFunc_959_200a5f8` it is actively wrong for a
different reason. That function passes stack arguments, so its prologue carries
a `sub sp, #8` with SIX body instructions hoisted above it, and a volatile asm
at the top of the body is a full scheduling barrier that stops the hoist. The
two defects are in tension: barrier gives 7 of 65, no barrier gives 3.

**What works costs nothing.** The movs are slaved to the shift order AS WRITTEN
IN THE SOURCE, so write the shifts in the order the MOVS need:

    rom     mov r0 / mov r1 / mov r2 / lsl r1 / lsl r2 / lsl r0
    source  q0 = K; q1 = K; q2 = K; q0 <<= a; q1 <<= b; q2 <<= c;
                                    ^^^^^^^^ ROM's MOV order, not its shift order

The movs then come out r0, r1, r2 as required, and sched2 lands the shifts in
the ROM's order on its own. Nothing blocks the frame adjustment. Exact.

**TRY THIS BEFORE REACHING FOR A BARRIER.** It cannot reach every crossed site
-- the barrier is still what closes the three-register fills where two movs are
out of place, and the negation forms -- but it has no side effects, and a
function that carries a `sub sp` in its prologue is a positive reason to prefer
it. The tell for the conflict is a residue that gets WORSE when the barrier goes
in, with `sub sp` leading the diff.

### HAZARD: a pin assigned BEFORE a call and used AFTER it is silently dropped

The rematerialisation lever works because r0-r3 are call-clobbered, so a
constant pinned there cannot survive a `bl` and gcc must rebuild it. The same
fact is a BUG GENERATOR when the ROM does NOT rebuild it. Measured on
`OvlFunc_883_20092bc`:

    { PIN2;
      q1 = (int)gScript;                                  <-- assigned here
      *(void **)(__MapActor_GetActor(0x16) + 0x6c) = fn;   <-- call clobbers r1
      q0 = 0x16;
      __MapActor_SetBehavior(q0, q1); }                    <-- used here

The `ldr r1, =gScript` IS NOT EMITTED AT ALL. The function comes out one
instruction short and the callee is entered with r1 never written. gcc sees a
dead store to a hard register and deletes it; unlike a pseudo, a pin is not
something it can reload, so nothing takes its place.

**The tell is a function one instruction SHORT with an argument register never
written.** A length undershoot is normally read as gcc commoning something; here
it means an argument vanished. Read WHICH instruction is missing, not the count.

The rule: **a pinned register's live range must not cross a call.** Assign it
after the last call before its use, or do not pin that site at all -- on the
function above, plain literals with no pin are exact.

### The fakematch escape, and where it goes

Where the tree accepts a fakematch, the idiom is
`register unsigned int rq __asm__("rN") = K;` plus
`__asm__ volatile ("" : : "r" (rq));`. Two things about it, both MEASURED on
`OvlFunc_938_2009450`:

**It does not go on every site, and putting it everywhere is worse.** All four
sites barriered: 3 differing. The parameter barriered instead: 2. Site one left
plain: exact. The reason is that the first call already gets the ROM's
interleave for free — the parameter copy `mov r5, r0` is itself the instruction
that lands in the `mov r1,#0xc0 / lsl r1,#8` gap, and forcing r0 there pushes
the copy to the top. **Barrier sites 2..n, never site 1, when a parameter is
saved to a callee-saved register.**

**Both halves are load-bearing.** The `register __asm__("rN")` declaration ALONE
is inert: without the volatile barrier it measured fourteen, identical to plain
literals. The barrier is what defeats cse1; the register pin is what places the
`mov` in the gap.

**Correction to the existing note that "the hoist happens at expand".** That is
true of the POOL-LOAD form only. For the `mov`+`lsl` form, `.00.rtl` shows
expand emitting four *independent* `(set (reg) (const_int 49152))` — one per
site, because Thumb cannot encode the constant and each argument is
`force_reg`'d — and the hoist is cse1's, via `cse.c`'s `COST` macro at line 509:
a pseudo costs 1, a `const_int` goes through `notreg_cost` and costs more on
Thumb, so at a copy insn CSE always prefers the register.

**There is no flag to find, and that is READ, not swept.** The responsible pass
is the FIRST `cse_main`, which in `toplev.c:2917` sits inside the plain
`optimize > 0` block with **no `-f` flag gating it at all**;
`flag_rerun_cse_after_loop` guards only the *second* call, at line 3095. The
dumps agree: `.02.jump` still carries one `const_int` per use, `.03.cse` has
already demoted the later sites to `REG_EQUAL` notes, and `.07.gcse` changes
nothing further. Eleven flags have now been swept across two functions
(`-fno-rerun-cse-after-loop`, `-O1`, `-fno-expensive-optimizations`,
`-fno-gcse`, `-fno-strength-reduce`, `-fno-schedule-insns2`, `-fno-peephole2`,
`-fno-cse-follow-jumps`, `-ffixed-r5`, `-ffixed-r5 -ffixed-r6`,
`-fno-schedule-insns`) with no effect on the class — but the sweeps were never
the argument. **Stop sweeping flags on this class.**

### Anchor EVERY argument of a call you anchor any argument of

The launder is **per call site, not per constant** — counterintuitive, and it
decides how many launders a fakematch needs. MEASURED on
`OvlFunc_927_200a004`, whose clean floor is 13 of 43:

| laundered | disagreeing |
|---|---|
| nothing | 13 |
| the second site only | 13 |
| the two repeated constants, both sites (the CSE-necessary minimum) | 4 |
| minimum + the `1` | 2 |
| minimum + the `-1` | 2 |
| **every argument of both affected calls** | **0** |

Laundering the CSE-necessary minimum removes the CSE residue and a **sched2**
residue takes its place, because gcc's natural interleave was already the ROM's
— the function's third call site comes out exact with no help at all. Anchoring
*some* arguments of a call perturbs that interleave, leaving the un-anchored
`neg r1` and `mov r3, #1` floating above the anchored shifts. **A partially
laundered argument list is strictly worse than none.** `--no-sched2` on the
partial forms gives 14, confirming sched2 is the reorderer and that suppressing
it is not the fix.

**Launder the FIRST occurrence, never the second.** Laundering only the later
site left 13 — bit-identical to no launder. From `.03.cse`: CSE substitutes into
the launder's own *initialiser* (`set (reg _t) (const_int …)` becomes
`set (reg _t) (reg 32)`) before the asm ever sees the value. The asm makes the
value opaque *downstream of the assignment*, which is too late. Anchor the first
site and every later occurrence rebuilds naturally.

Six launders on a 43-instruction function is within house norms, not an outlier:
47 of the 97 fakematch files carry five or more, and the precedent in this same
overlay carries seven.

## The LICM lever has TWO HALVES and both are load-bearing

Hoisted expressions land at the **END** of the preheader. When the ROM emits a
plain copy *after* a hoisted computation, promoting that computation to an
explicit local is **necessary but not sufficient** — with the local written in
the wrong place the residue is byte-identical to leaving it inline.

MEASURED on `OvlFunc_945_2009144`, a clean three-point curve:

| spelling | disagreeing |
|---|---|
| four box bounds inline in the `if` | 2 |
| bounds as locals, written AFTER the pointer setup | 2 |
| bounds as locals, written BEFORE the pointer setup | **0** |

Once the expression is a source statement, the preheader keeps **source order**
through sched2: the five computations are mutually independent, so
`rank_for_schedule` falls through to the original insn number.

**Promote AND sweep the position.** This is the same shape as the general "every
name-the-value lever has a placement" rule, but it is worth stating separately
because the failure mode is silent — the promoted-but-misplaced spelling scores
exactly what the inline spelling scored, which reads as "the lever did nothing"
rather than "the lever is half-applied".

## A named local also shows up with NO CALL in sight

The existing rule reads: *one load kept across a call is a named local; two
loads of the same field are direct field reads.* There is a third face, and it
needs no call at all.

**Eager versus lazy issue across a short-circuit chain.** MEASURED on
`OvlFunc_945_2009144`: the ROM issues

    mov  r7, #0xa
    ldrsh r2, [r0, r7]
    mov  r7, #0x12
    ldrsh r4, [r0, r7]

back to back, **before any `cmp`**. Written inline inside the `&&` chain, gcc
sinks the second load past the x-tests — which is exactly what the short circuit
licenses — and 22 positions differ. Naming both loads takes it to 2.

**A load issued before its own guard has been evaluated is a named local.** The
discriminator is not a call; it is that the ROM did work the short circuit would
have skipped.

## r4 used but NOT pushed is `-fcall-used-r4` working, not a wall

The existing entry says a prologue that pushes r4 and keeps a value in it across
a call means the TU was built **without** `-fcall-used-r4`. The converse is also
a shape you will meet, and it is not a blocker.

MEASURED on `Func_80f377c`: `push {lr}` with r4 unsaved (`00b5`, `041c`), while
r4 holds the allocated block pointer across three inline-asm blocks that bind
r0–r3. It is legitimate because **r4 is dead before the next `bl`** — under
`-fcall-used-r4` gcc may use it at no prologue cost.

**Check the liveness before filing a missing push as a flag-group problem.**
Only r4 kept live *across a call* implicates the flag.

## Not a source tell: `neg`+`add` instead of `sub`

Thumb-1's three-address `sub rd, rn, #imm` only encodes an imm3. A subtraction
of a constant ≥ 8 with the source operand still live therefore has to route the
constant through a register, giving `mov rN, #12 / neg rN, rN / add rN, r0`
where the C plainly said `x - 12`. Likewise `x + 12` gets the two-address
`mov r6, r0 / add r6, #12`. Neither is a spelling signal — do not chase either.

## `do { } while (0)` IS A SCHEDULING BARRIER

READ from `haifa-sched.c`, `sched_analyze_insn` (~line 3714): if any
`NOTE_INSN_LOOP_BEG`, `LOOP_END`, `EH_REGION` or `SETJMP` note was collected
before an insn, `schedule_barrier_found` fires and that insn gets a
`REG_DEP_ANTI` on **every** prior register use and set.

So a macro body wrapped in `do { } while (0)` — this tree's `SET_IO` and
`SET_PALETTE` — **splits one basic block into two scheduling regions without
emitting a single instruction.** Everything before the macro is scheduled to
exhaustion first.

**This is the lever for "sched2 put my prologue filler in the wrong hole".**

### And it reaches a residue that source order cannot

The existing rule says a sched2 residue of two adjacent independent insns is a
source-order tie-break. There is a second case that looks the same and is not.
MEASURED on `Func_80c0700`, from `.23.sched2` with `-fsched-verbose=6`:

    prio(sub sp,#4)     = prio(str [r6]) + 2     via mov r6, sp
    prio(add r5,r0,r3)  = prio(str [r6]) + 4     the add reads r3; the IME
    prio(ldr =REG_IME)  = prio(str [r6]) + 4     pool load writes it

`rank_for_schedule` compares `INSN_PRIORITY` **first**, so `add r5` beats
`sub sp` by a **fixed +2 gap that no source rewrite can close** — the WAR is
unavoidable because local-alloc puts both pool constants in r3. The ROM's order
was therefore proof of a *barrier*, not of a tie-break.

**If a sched2 residue is NOT two adjacent independent instructions but a FIXED
PRIORITY GAP, look for a missing barrier — a `do/while(0)` macro — not for a
source-order swap.**

## `strh r3, [r3]` is a SOURCE IDIOM: grep before theorising

A volatile store whose **value register is the same register that holds the port
address** is not an allocator artefact. It is `SET_IO(REG_IME, REG_ADDR_IME)` —
storing the port's own address into the port. `REG_ADDR_IME` has 0 in its low
bit, so this disables interrupts while saving the `mov rN, #0`, reusing a
register that already holds the address.

This tree already had it, with a comment, in `SetIntrHandler`
(`src/rom_c0/rom_2e00_c_c_b.c`). It still cost hours of theorising about the
register allocator on `Func_80c0700`.

**When an instruction looks like a compiler quirk, grep the tree for a solved
instance of the shape before reasoning about a pass.** The corpus is the
cheapest oracle available and it is consulted last far too often.

## An ARGUMENT LIST is an ordering device

`REG_ALLOC_ORDER` is `{3,2,1,0,12,14,4,5,6,7,…}`, so local-alloc hands a
store-address pseudo **r1** whenever r1 is free. `set_preference` in `global.c`
then unwraps `(set (reg 51) (plus (reg 34) 1604))` — `GET_RTX_FORMAT(PLUS)[0]`
is `'e'`, so `src` becomes `reg 34`, whose `reg_renumber` is already 1 — giving
the base pointer a **hard-reg preference for r1**. That costs a callee-saved
register.

When the ROM needed r1 *busy* across an address computation, the natural
spelling is **putting the assignment inside the argument list**:

    f(pal, (void *)0x50000c0, *(int *)(g + 0x644) = v, 0x80);

gcc precomputes the earlier arguments into pseudos before expanding the later
one, so the earlier argument's pseudo stays live across the store, the address
pseudo falls through to r0, and one fewer callee-saved register is needed.

**An assignment written as an argument keeps the earlier argument pseudos alive
across it. That is a register-pressure lever, not a style choice.**

## A `*/` inside a comment silently turns the rest of the file into code

Writing `EH_REGION_*/SETJMP` in a header comment closed the comment 600
characters early. The compiler then reported four *"missing terminating `'`
character"* errors — because possessive apostrophes in the remaining prose were
now being lexed as character constants — and a *"malformed floating constant"*
on the text `.23.sched2`.

**None of the reported errors were at the actual fault, and all of them pointed
at innocent punctuation.** The diagnosis that fits every symptom at once is
"the comment ended early"; chasing the apostrophes individually would have
mangled the prose and never fixed it.

Pass names, dump suffixes and RTL note names get written into these headers
constantly, and `EH_REGION_*` is exactly the kind of token that ends in `*`.
**Before landing a header comment, check it contains no `*/` other than its
own terminator.**

## `x |= 0xff` on a byte field is DELETED outside a loop, and survives inside one

MEASURED on `Func_80ba918`. The same source text behaves differently in two
places in one function:

- **Outside the loop**, `q->f16 |= 0xff` compiled to `mov r3,#255 / strb r3`.
  Combine folded `x | 0xff` to `0xff` and dropped the `ldrb` and the `orr`
  entirely — six instructions short of the ROM.
- **Inside the loop**, the identical text survived as `ldrb / orr / strb`,
  because `loop.c` had already hoisted the literal into a pseudo before combine
  ran, so combine only ever saw `(ior reg reg)`.

**So a ROM read-modify-write with an all-ones mask OUTSIDE a loop proves the
mask was a NAMED VARIABLE in the source. Inside a loop it proves nothing.**

That also explains the high register: a named `int mask` is live across the
`bl`, so global-alloc gives it a callee-saved register and every use costs a
`mov rLow, r10`. The `r10` is a consequence of the naming, not a blocker.

### The mirror of the orr-destination lever

The documented cures for a constant landing in the destination — `*p = K | *p`,
`*p = *p | K`, a narrow local, an `int` local — were all tried here and **all
four scored identically**. When the second operand is a **register-held mask**
rather than a literal, the fix is to name the **loaded value** instead of the
constant:

    t = q->f16;
    q->f16 = t | mask;

13 → 6 in one step, with `int t` and `unsigned char t` interchangeable. The
existing entry covers only naming the constant; this is the case that applies
whenever the ROM's mask arrives in a register.

## A reload SCRATCH register is a statement-order tell

The last residue was `mov r1, r10 / orr r3, r1` against the ROM's
`mov r4, r10 / orr r3, r4`. READ from `.18.greg`: *"Spilling for insn 48. Using
reg 1 for reload 0"* — **reload takes the lowest free register**, so the ROM
choosing r4 means **r1 was already occupied at allocation time**. The only thing
that could occupy it was a pointer assignment that sched2 had moved below the
`orr`. Hoisting that statement above the read-modify-write took 6 → 0.

**When the only residue is a scratch-register rotation, do not reach for
allocation-order arithmetic. Ask which source statement the ROM must have
evaluated EARLIER to make the low register busy, and hoist it — sched2 will put
it back where the ROM shows it.** This is the LICM "promote AND sweep the
position" rule one level down. Only the ordering constraint is real: three
placements satisfying it all matched.

## `pop {r1} / bx r1` names a return value — but `return x;` is not the spelling

Second face of that entry. Writing the obvious `return s;` makes gcc
const-propagate `s == 0` off the loop-exit edge and emit an extra `mov r0, #0`.
What reproduces the ROM is an **`int` return type with no `return` statement at
all**: r0 stays live at the epilogue and nothing is materialised.

## The `sub` after the hoisted constants means an upward `for`, not a `do/while`

An explicit `c--` statement lands in the preheader **before** the LICM hoists.
A plain `for (j = 1; j < c; j++)` lets `check_dbra_loop` synthesise the
countdown **after** them — which is the ROM's order. So the position of the
loop's decrement relative to the hoisted constants distinguishes the two source
forms.

## Try the BARE register pin before the barrier

The fakematch idiom has two strengths and the weaker one is often enough.
Measured on two functions that pull in opposite directions:

| function | bare `register __asm__` pin | pin + `__asm__ volatile` barrier |
|---|---|---|
| `OvlFunc_938_2009450` | 14 (inert — same as plain literals) | **0** |
| `OvlFunc_884_2008780` | **0** | 8 and 6 (a regression) |

The difference is **what the pin does to the pseudo**, read from the `-da`
dumps. With plain literal arguments, expand puts any multi-insn constant into a
pseudo already at `.00.rtl`, and `.03.cse` rewrites the later site to it. A
hard-register declaration **removes the pseudo**, so there is nothing left to
common and no barrier is needed. Where the constant still flows through a
pseudo, the pin cannot help and the barrier is what works.

**Try the bare pin first; reach for the barrier only if it is inert.** And note
the barrier's cost is not local — on `OvlFunc_884_2008780` it perturbed the
scheduler three instructions *upstream*, inside a call that plain literals had
already matched.

### Declaration order is argument-setup order

READ from `.23.sched2`: three argument insns tie at priority 70; one wins on
`INSN_DEPEND` count; the remaining pair ties 3-against-3 and falls through to
**`INSN_LUID`** — the RTL chain order. **So the order pinned register variables
are declared in is the order the argument setup comes out in.**

### The return-type lever's mechanism

An implicitly-declared (int-returning) callee carries
`(set (reg:SI 0 r0) (call ...))`. That set is the next *real* write of r0, so it
**truncates the dependent list** of the `mov r0,#0` feeding it — two dependents
instead of three — and it loses a scheduling tie it would otherwise win.
Declaring the callee `void` restores the third dependent (the next call's own
`mov r0,#0`) and hands the decision back to LUID.

Two callees in one function needed `void` for **two different reasons**: one for
its own argument order, one for the r0 dependent count at the *previous* call
site. That is why the lever has to be swept per callee rather than applied as a
policy.

**Corollary: when two ROM call sites with identical source shape schedule
differently, do not hunt for a source difference.** It is the
dependent-count/LUID tie resolving differently because of what *follows* each
call. Reproduce one and the other usually falls out.

## AN ARM FUNCTION IS NOT ATTEMPTABLE — there is no ARM build path

**This build compiles no ARM code at all.** Every one of the 935 gcc-2.96 rules
passes `-mthumb`, and **none of the 3,517 solved files is ARM**. So a
`.arm_func_start` function cannot be elevated without first establishing a
compile path that does not exist, and there is no precedent to copy.

That is a structural fact rather than a heuristic, which is why `census.py`
gives ARM its own column instead of folding it into hand-written assembly.

**14 ARM functions across 4 files** were being reported as *available*, and they
are the entire reason the 1–20 and 21–40 bands looked like they still had work
in them. They did not: both bands are now **0 available**.

### The ARM ones are hand-written anyway — the tells

Checked by hand, all six ARM functions under 40 instructions are plainly
hand-assembled, and the tells are worth keeping because the Thumb-oriented
`HANDASM` test sees none of them:

- **Three-way width selection by predication.** `UploadPalette_ROM` picks byte,
  halfword or word from one `cmp` with `ldrccb` / `ldreqh` / `ldrgt`, then
  stores the same way. gcc-2.96 never emits that.
- **`rrx`.** `Func_8015570` uses rotate-right-extended. gcc essentially never
  generates it.
- **A constant table read via `adr` + `ldm`.** `Func_8015d74` and `Func_8015e10`
  load three masks from a hand-laid `.word` table next to the code. gcc uses
  `ldr rN, =const` and a literal pool.
- **Self-modifying / relocating code.** `FixupRamCode_ROM` walks memory
  rewriting **Thumb BL instruction pairs** to relocate code copied to RAM, and
  `Func_80f0024` rewrites its own branch table after computing a load offset.
- **`_ROM` suffix plus `func_end_emit_size`.** These are routines copied to RAM
  at runtime, with their sizes exported for the copier — the whole family is
  deliberately hand-written.

### `Func_80f0008` is the sharp case, and it is worth reading

Seven instructions, `smull` + two `smlal`, then `(hi << 16) | (lo >> 16)`. That
is exactly a three-term fixed-point dot product returning `acc >> 16`, and it
looks like ordinary C:

    long long acc = (long long)b * a;
    acc += (long long)c * d;
    acc += (long long)e * f;
    return acc >> 16;

**gcc-2.96 compiles that to `smull`/`smlal` under `-marm`** — so the shape is
reachable in principle. It still cannot match, and the reason is one register
pair:

    ROM:   smull r12, r0, r1, r0      <- accumulator in (r12, r0)
    ours:  smull r4,  r5, r1, r0      <- accumulator in (r4, r5), so r5 must be saved

The ROM's pair is call-clobbered, so it needs **no prologue**; gcc's is
callee-saved, so it emits `stmfd`/`ldmfd` and the stack argument load shifts by
8. `HARD_REGNO_MODE_OK` (`arm.h:965`) puts **no** alignment constraint on ARM
DImode, so that is not the obstacle — but a DImode value still occupies
**consecutive** registers, and r12's successor is r13/sp, not r0. **A gcc DImode
value can never live in (r12, r0)**, so the ROM's `smull` is not gcc output at
whatever allocation. Hand-written.

The general lesson: `-marm` compiling cleanly is not evidence the original was
C. Check whether the register assignment is one the machine description can even
express.

## The return-type lever is about WHETHER the call writes r0, not what width

`BuildDraw2DFuncs` matched only once its callee was declared to return a value.
Declared `void` it is 8 differing, with `mov r0, #0x2e` emitted one slot too
early in both arms of an `if`/`else`; declared to return anything at all it is
exact.

MEASURED, and the indifference is the finding: **`int`, `unsigned int`, `char`,
`short`, `void *`, `long`, and leaving the callee undeclared entirely all
match. Only `void` fails.**

That is the mechanism recorded on `OvlFunc_884_2008780` seen from the other
side. An int-returning callee carries `(set (reg:SI 0 r0) (call ...))`, which is
the next *real* write of r0 and therefore truncates the dependent list of the
`mov r0` feeding it, changing how the argument-setup scheduling tie resolves.
There `void` was what was needed; here it is anything but.

**So do not read meaning into which non-void type wins — pick the type the
callee actually has.** The only bit that reaches the scheduler is whether the
call writes r0.

## A register live across a call is NOT always a named local

The recorded rule — *one load kept across a call is a named local* — has a
false-positive shape, and it is worth knowing because the instinct runs the
wrong way.

`BuildDraw2DFuncs` loads `gPtrs`' address in each arm of an `if`/`else` and
keeps it in a **callee-saved register across the second call**, which is exactly
the named-local silhouette. It is not one. MEASURED:

| spelling | disagreeing |
|---|---|
| `void **p = gPtrs;` hoisted above the branch | 33 |
| the same local assigned separately inside each arm | 28 |
| **no local at all — `gPtrs[...]` read directly** | **8** |

The register genuinely is live across the call, but that is gcc's own doing once
both arms need the address. **Check whether the liveness is forced by the
control flow before concluding the source named the value.**

## How much of a duplicated tail has to be duplicated in the source

"Duplicated ROM code means duplicated source" is already recorded; this
quantifies where to stop. In `BuildDraw2DFuncs`, both arms of the `if`/`else`
end with the same call, load and store:

- sharing the trailing store between the arms → **28** of 52
- writing the second call **and** the trailing store inside **both** arms → **8**

gcc then cross-jumps the common suffix back into a single block on its own,
which is why the ROM shows one shared tail even though the source repeats it.
The pointer bump ahead of it stays duplicated because the two arms order it
differently against the stack-argument store, so the merge cannot begin any
earlier. **Duplicate the whole tail and let cross-jumping take back what it
wants — do not pre-share it.**

## PICK TARGETS BY TEMPLATE, NOT BY SIZE — `tools/templated.py`

Two consecutive rounds went **0-for-6** choosing targets by size, smallest
first. That heuristic is spent: with ~3,520 functions matched, the size-ordered
head of the list is uniformly structural — allocation-order disagreements and
scheduling residues — because the ones that fall to a spelling are gone.

The heuristic that *has* worked, seven rounds running, is **callee-set
identity**: `neighbour.py` finds a solved file sharing the target's callees and
globals, and that file hands over the struct layout, the argument order and the
idiom. But it was only ever run **after** a target was chosen.

`tools/templated.py` inverts it — score every remaining function by its best
available neighbour *first*, and work the ones that already have a worked
example. `score = |shared symbols| / |target's symbols|`, so **1.00 means every
callee and global the function touches already appears together in one solved
file**. 931 candidates have some neighbour; 14 score 1.00.

The first 1.00 candidate tried, `OvlFunc_959_200c704`, matched. **Size is
reported but deliberately not ranked on** — a 90-instruction function with a
perfect template beats a 50-instruction one with none, and if that stops holding
the tool should be struck rather than tuned.

### `do { } while (0)` used deliberately, and it earned its keep

The barrier finding from `Func_80c0700` was recorded as an observation; this is
the first time it was reached for on purpose. The shape: a pool load that the
ROM emits **after** a call, which gcc hoists **above** it because the target
register is callee-saved and survives the call.

    do { msg = 0x2411; } while (0);

emits no instruction, splits the scheduling region, and stops the hoist — 4
differing down to 2. **When the ROM materialises a constant after a call and you
cannot keep it there, the construct that costs nothing is the loop note.**

### Measure a fakematch's scaffolding by REMOVING it, not by adding it

`OvlFunc_959_200c704` ended with three pin blocks, a barrier and a two-step
constant, which looks like a lot. Each was then deleted from the *finished* file
in turn:

| removed | differing |
|---|---|
| the pin on the repeated `0xb0 << 8` call | 32 |
| the `do{}while(0)` barrier | 4 |
| the pin on the unrelated `__MapActor_Emote` call | 2 |
| nothing (as landed) | **0** |

Every piece is load-bearing, and the sizes say what each is *for* — the first is
the actual CSE defeat, the rest are scheduling. Building a fakematch up
lever-by-lever tells you what helped; tearing it down afterwards tells you what
is still needed, and those are different questions. **Do the teardown before
landing one.**

### The largest step was ordinary C, not the fakematch

Worth stating because the fakematch is the eye-catching part: the single biggest
gain, 30 differing down to 6, came from noticing that the ROM holds `0x2411` in
a callee-saved register and reaches the second message with `add r5, #1`. That
is a **named local, incremented** — two separate literals give two pool loads
and no `push {r5}` at all. Nothing to do with inline asm.

## A call between two uses does NOT guarantee the constant is rebuilt

The recorded false-positive list for the repeated-constant blocker says: *"A
call between the two sites — calls clobber the argument registers, so a constant
used as an argument is rebuilt at each site whatever cse would prefer."* That
was the largest of the three classes, 250 of the 406 originally suspected.

**It is only true when the constant lives in a call-clobbered register.** If gcc
has spare callee-saved registers, it will spend them to keep the constant across
the call and feed the later site with a copy.

MEASURED on `OvlFunc_891_2009b44`, which has `0x3333` and `0x1999` at two
`__MapActor_SetSpeed` calls with a `bl` between them:

    rom     ldr r1, =0x3333   ldr r2, =0x1999      -- rebuilt at BOTH sites
    plain   ldr r5, =0x3333   ldr r6, =0x1999      -- once, then mov r1,r5 / mov r2,r6

I read the intervening call as excusing the rebuild and wrote plain C. It came
out **seven instructions LONG with two extra callee-saved registers**.

### The marker is a WIDER PROLOGUE, not a `mov`

This is why the case is easy to miss. The familiar symptom of the constant-hoist
blocker is `mov rN, rM` where the ROM rebuilds. Here the copies are there too,
but the *loud* signal is the prologue: `push {r5, r6, r7, lr}` plus an r11 save
against the ROM's `push {r5, r6, lr}`.

**When a straight-line candidate comes out LONGER than the ROM with extra
callee-saved registers, look for a hoisted constant before looking anywhere
else.** Three spellings were tried first — naming the coordinates, transforming
them in place, reordering the statements — and all three made it worse, because
none of them addressed the actual cause.

### Anchor the arguments that participate, not the whole list

The recorded rule is *"anchor every argument of any call you anchor any argument
of"*, learned where partial anchoring perturbed an already-correct interleave.
It is not unconditional. Here, pinning the two constants gave 24 differing and
adding a third pin on the same call's `r0` argument gave **exactly 24** — the
slot argument does not participate in the perturbed interleave, so pinning it
buys nothing and only adds scaffolding a teardown would have to justify.

Read the rule as: *anchor every argument that participates in the interleave you
are fixing.* Where that is the whole list, anchor the whole list; where it is
not, the teardown will say so.

## A hoisted constant and a named local look IDENTICAL — read the first use

A constant living in a callee-saved register across several calls is the
recorded signature of *"one load kept across a call is a named local"*. It is
also what gcc produces on its own when it hoists a literal it sees reused. The
two are indistinguishable from the register allocation alone, and guessing wrong
is expensive: on `OvlFunc_927_2009c34`, naming them measured **80 differing of
92** where plain literals measured **8 of 90**.

**The tell is where the FIRST use goes.**

    ROM      mov r3, #1 / str r3, [sp] / ... / mov r8, r3
    named    mov r3, #1 / mov r8, r3   / ... / mov r3, r8 / str r3, [sp]

The ROM stores the literal **straight to its destination** and only afterwards
copies it into a high register for the later sites. A named local is
materialised into its register *first*, and every use — including the first —
is fed from there.

So: **if the first use is direct and the register copy comes after, the source
had a literal and gcc hoisted it. If the first use already goes through the
register, the source named it.**

This is the same question as the `Func_80b280c`-style pressure reading seen from
the other side, and it now has three data points pulling in different
directions — `BuildDraw2DFuncs` (liveness forced by control flow, do not name),
`OvlFunc_959_200c704` (genuinely named and incremented), and this one (hoisted).
The order of the first use separates all three.

## Register swap: sweep ASSIGNMENT order when the two values start together

Two functions this round ended with a two-register swap and nothing else, and
they are **not** the same class — the difference is how far apart the two values
are initialised.

**`OvlFunc_926_2008db4` — adjacent initialisation, source has a vote.** The ROM
opens with `mov r6, #0 / mov r5, #8`, the counter first. Measured:

| spelling | differing |
|---|---|
| `n = 8; for (i = 0; …)` | 6 |
| `i = 0; n = 8; for (; …)` | **0** |
| `for (i = 0, n = 8; …)` | **0** |
| swapping the two **declarations** | 6 — no effect |

**`OvlFunc_932_200b668` and its twin — far-apart initialisation, allocator
wins.** One value is set before a call and the other after; six spellings tie at
5 and declaration order is equally inert. Parked.

Both have inert declaration order, because local-alloc orders by priority rather
than pseudo number. But where the two initialisations sit **adjacently in one
basic block**, their order is still visible and the assignment order decides.

**So: for a register swap between two values initialised adjacently, sweep the
assignment order before concluding allocation order. For values initialised far
apart, do not bother — that is the allocator.** The cheap discriminator is
whether the two `mov`s are neighbours in the ROM.

## Teardown found ONE pin where four pieces had been added

`OvlFunc_882_200be48` matched on the first candidate but for a single
instruction — `lsl r1, #5` one slot early, because the ROM defers the shift past
every other argument setup and emits it last before the `bl`.

The first form that worked pinned all three argument registers **and** used a
two-step constant. Removing pieces one at a time from the *finished* file:

| scaffolding | differing |
|---|---|
| r0 + r1 + r2 pinned, two-step | 0 — the first thing that worked |
| r1 + r2 pinned, two-step | 2 |
| r0 + r1 pinned, two-step | 0 |
| r0 + r2 pinned, no two-step | 0 |
| **r0 pinned alone, no two-step** | **0** — landed |
| r2 pinned alone | 2 |

**r0 was the whole lever; the other three pieces were habit.** Pinning the slot
argument forces it into place early, which leaves the shift as the only work
left before the call, so gcc emits it last on its own.

Neither plain two-step reaches this: assigning `v = 0x80;` before the two
intervening stores and `v <<= 5;` after them measures the same 2, and so does
the compact form. Argument evaluation order is not something the source can
state, which is what the pin is standing in for.

**This is the third consecutive round in which the *anchor every argument* rule
has had to be bounded.** It came from a case where partial anchoring perturbed a
correct interleave, and it keeps being read as unconditional. The reliable form
is: **anchor the argument that participates, and let the teardown find which one
that is.** Adding pins is cheap and always "works"; the teardown is what tells
you which one you actually needed.

## Screen candidates on HIGH-REGISTER USE, not just template quality

`templated.py` reliably gets the *data model* right — struct layouts, argument
orders and extern blocks come free from a good neighbour. It does **not**
predict whether the residue will be tractable, and two strong templates in a row
went nowhere before this was noticed.

Counting `r8`–`r11` references in the target's body separates them cleanly.
Measured across one round's candidate list:

| function | syms | insns | r8–r11 uses | outcome |
|---|---|---|---|---|
| `OvlFunc_955_2009898` | 9 | 123 | **27** | abandoned, 117 of 126 |
| `OvlFunc_968_2008cc8` | 11 | 138 | **17** | parked, 104 of 140 |
| `OvlFunc_925_200b060` | 7 | 148 | 17 | untried |
| `OvlFunc_935_20088a8` | 7 | 59 | **0** | **elevated, first candidate** |
| `OvlFunc_943_2009a98` | 7 | 75 | **0** | — |

High-register traffic means the function needs more values live than the low
registers hold, and that is exactly the pressure the allocation-order parks are
made of. A count of zero, with a narrow `push`, says the residue will be about
spelling rather than about the allocator.

**Rank on the template, then filter on `hi == 0`.** The two are independent
questions and both are cheap to ask before writing a line of C.

## A "60 of 60" screen can be three instructions

`OvlFunc_935_20088a8` reported *60 differing of 60* at plain -O2 — an apparently
total mismatch — when exactly **three** instructions disagreed. One extra
instruction near the top shifts every later line by one, and difflib then aligns
almost nothing.

**Read the itemised regions, not the headline number.** The trailing
`N instruction(s) in disagreeing regions` line is the honest figure; the count in
the header is an alignment artefact whenever the lengths differ. That candidate
was correct on the first try and could easily have been thrown away.

## Pin DECLARATION order is argument order

Recorded before as "declaration order is argument-setup order" for pinned
registers, and worth restating precisely because the wording invites the wrong
reading: it is the order of the **declarations**, not the order the arguments
appear in the call.

MEASURED on `OvlFunc_932_2008c9c`, the last two instructions of the match. The
ROM sets `mov r0, #0xa` first and the two pooled constants after:

    register int v1 __asm__("r1") = 0x3333;   /* r0 lands three slots late */
    register int v2 __asm__("r2") = 0x1999;
    register int v0 __asm__("r0") = 0xa;

    register int v0 __asm__("r0") = 0xa;      /* exact */
    register int v1 __asm__("r1") = 0x3333;
    register int v2 __asm__("r2") = 0x1999;

The call site is written the same way in both. Only the declaration order moved.

## Read each condition code separately — one guard can mix signedness

The recorded rule is that a condition code is a signedness statement:
`bls`/`bhi`/`bcs`/`bcc` mean unsigned. What is easy to miss is that **adjacent
tests in the same guard can differ**, and assuming one signedness for the whole
guard costs an instruction and, worse, silently changes the semantics.

`OvlFunc_924_20094cc` guards on three comparisons in a row:

    sub r5, #0x54 / cmp r5, #7  / bhi   <- UNSIGNED: x in [0x54, 0x5b]
    cmp r3, #0xd3              / ble   <- signed
    cmp r3, #0xdb              / bgt   <- signed

The first is the standard one-sided range check — subtract the low bound and
compare **unsigned**, so a value below the range wraps to a huge number and
fails. Written with a plain `int` it compiles to `bgt`, which does not check the
low end at all: the code still builds, still looks right, and is wrong.

The two that follow are ordinary signed comparisons on a different field. So one
guard, two signednesses, three lines apart.

## An uninitialised pin moves its assignment

Companion to "pin declaration order is argument order". Declaration order sets
*where* each `mov` lands, but a `register` declaration **with an initialiser**
pins that `mov` to the declaration point — which is sometimes too early and
cannot be moved by reordering the declarations alone.

MEASURED on `OvlFunc_901_2008d84`, the last instruction of the match. The ROM
sets r2 last, after the shift:

    mov r1, #0x80 / mov r0, #0x12 / lsl r1, #7 / mov r2, #0x14

`register int p2 __asm__("r2") = 0x14;` puts that `mov` before the shift no
matter where the declaration sits relative to the others, because the shift is a
*statement* and the initialiser is not. Splitting them —

    register int p2 __asm__("r2");
    p0 <<= 7;
    p2 = 0x14;

— places it where the ROM has it.

**So the pin has two independent knobs: the declaration's position sets the
register's place in the ordering, and the assignment's position sets when the
value is materialised.** Reach for the second when the first cannot go late
enough.

## Two pool loads and a SUBTRACT mean two symbol addresses

gcc folds literal arithmetic. So a ROM sequence like

    ldr r3, =0xcc6 / ldr r2, =0xc9b / sub r3, r2 / add r5, r3

— two pool loads and a runtime subtraction to compute a compile-time 0x2b —
cannot have come from two integer constants. It is the **difference of two
symbol addresses**, which gcc has no way to fold.

In this tree the message ids are absolute symbols in `message.sym`, and the
established idiom is `extern int _MSG_xxx;` with `(int)(&_MSG_xxx)`. Written
that way the two pool loads and the subtract fall out exactly.

**The reading matters beyond the instructions.** `msg += _MSG_cc6 - _MSG_c9b`
says the source shifts whatever the caller passed by the distance between two
known lines — an offset from a base, not an assignment. Reading it as
`msg = 0xcc6` compiles to a single pool load and loses the meaning as well as
the match.

Missing ids are a one-line addition to `message.sym`; that is existing practice
for this family.

## Two signed byte reads, two instruction sequences — the source picks

`ldrsb` has no immediate-offset form, so a signed byte read through it needs its
offset in a register first (`mov r1, #0` and then `[r3, r1]`). But that is not
the only way gcc reads a signed byte, and one function can use both:

    +0x3a9   mov r1, #0 / ldrsb r1, [r3, r1]          <- straight into an int
    +0x3ac   ldrb r3, [r3] / lsl r3, #24 / asr r3, #24 <- via a signed char local

**gcc picks on where the value lands.** Read straight into an `int` it uses
`ldrsb`; assigned to a `signed char` **local** first, it emits the unsigned load
and widens afterwards. Both are signed byte reads and both compare the same, so
the difference is invisible in the semantics and shows up only as two extra
instructions — easy to dismiss as noise.

## "N spellings tie" is only evidence if the spellings differ in STRUCTURE

`OvlFunc_881_200b2f0` was parked at 4 of 99 with the claim that the `-1`
triple's `mov` order was unreachable, on the strength of **six spellings tying
at exactly 4**. The park was wrong, and the reason is worth more than the
function.

All six kept the three assignments together and the three negations together,
and varied only the order *within* those two groups:

    p0 = 1; p1 = 1; p2 = 1; p3 = 0;    /* six permutations of this half */
    p2 = -p2; p1 = -p1; p0 = -p0;      /* and of this one */

Interleaving them matches:

    p0 = 1; p0 = -p0;
    p1 = 1; p1 = -p1;
    p2 = 1; p2 = -p2;
    p3 = 0;

Six permutations inside one shape are not six unrelated spellings — they are one
spelling tried six times. **A tie is evidence of a wall only when the spellings
differ structurally**: different grouping, different statement boundaries,
different variables, not merely a different order over the same skeleton.

The practical test before writing a park: can you name the structural assumption
every attempt shared? If you can, that assumption is the next thing to vary, and
the park is premature.

## The `-1` triple is fully reachable

Batch 148 recorded it as an unbroken class; that was amended once to "reachable
by pinning" when the negations were reproduced, with the `mov` order still
believed out of reach. Both halves are now reachable, by pinning the four
argument registers and interleaving each assignment with its own negation.
`pickable.py`'s three-or-more-`neg` rejection has no remaining basis beyond
cost.

## `-fno-gcse` has a THIRD shape: a load HOISTED in front of a switch

The two `-fno-gcse` rules already in the Makefile are a shared CONSTANT hoisted
into a callee-saved register and a sunk LOAD. Batch 215 adds a third symptom on
`OvlFunc_945_200812c`: a thirteen-way switch whose arms each re-load the value
they bump, where at `-O2` the global pass loads it ONCE in the entry block above
the jump table. Every arm then shifts by one instruction — **147 of 157 lines
differ for that one hoisted load**.

**The bar for calling something a flag rather than a spelling.** Six
structurally different sources were measured against the hoist and none moved
it: a separate tail per case instead of a shared `goto`; an inverted guard in
the one arm that reaches the tail with no intervening call; recomputing the
pointer inside each arm; a `__asm__ volatile ("" ::: "memory")` immediately
before the switch; and the value typed both `short` and `unsigned short`. One
failed spelling is not evidence. Six that share no structural assumption is.

**Do not reach for `-O1` because it also works.** It did here — and it settled
EIGHT LINES WORSE, because it additionally reorders the entry block and the
jump-table setup. Measure both before writing a rule. Six further flags
(`-fno-rerun-cse-after-loop`, `-fno-strict-aliasing`, `-fno-strength-reduce`,
`-fno-expensive-optimizations`, `-fno-cse-follow-jumps`, `-fno-cse-skip-blocks`)
layered on top changed nothing, which is what pins the choice to `-fno-gcse`
alone rather than to a family.

## The narrow-store table gains a fourth row: a PINNED `short` local pools

The narrow-store section above ends with a "where it does not reach" note about
a block where the ROM pools a value we `mov` and `mov`s a value we pool.
`OvlFunc_945_200812c` is exactly that shape — a byte store of zero the ROM
POOLS (`ldr r2, =0x0`) and, in the same function, a halfword store of zero the
ROM builds with `mov r3, #0`.

Twelve spellings were measured on the pooled one. The bare literal, casts
through `char *` / `unsigned char *` / `volatile unsigned char *`, named locals
of type `int` / `char` / `unsigned char` / `short` with and without a barrier, a
local assigned in a dominating block, a local assigned in every predecessor of
the join, and a `static const` — **eleven give `mov`**. The twelfth is exact:

    register short hz __asm__("r2");
    ...
    hz = 0;
    __asm__ volatile ("" : : "r" (hz));
    p3 = a + 0x62;
    *p3 = hz;

**The pin is what does it, not the width.** The same `short` unpinned still
`mov`s, and an `int` pinned to the same register also `mov`s. So the table's
exclusion is not wrong — it was about the unpinned spellings, and it still holds
for those. Add the pinned-narrow-local row beside it.

## Try PINS before the crossed-shift barrier — and know where they stop

The crossed mov/shift cure on record is the volatile-asm barrier, with the
statement-reordering cure as a no-side-effect alternative. There is a third, and
it should be tried first when the crossed values are CALL ARGUMENTS.

On `OvlFunc_932_20087e8` two locals pinned to `r0` and `r1` were added to
rematerialise a constant passed twice to one call. They also produced the ROM's
crossed order — `mov r0 / mov r1 / mov r2` against `lsl r2 / lsl r0 / lsl r1` —
**with no barrier anywhere**. Seven instructions exact in one step.

**Where pins stop.** `OvlFunc_945_200812c` has the same crossed shape in two
argument fills and pins are not enough: gcc re-fuses each `mov` with its own
`lsl` and the barrier is still needed. The difference is what sits inside the
crossing. In `20087e8` every crossed value is an argument being BUILT. In
`200812c` a plain register copy — `mov r0, r5` — is wedged between the first
shift and the second, and a pin cannot order a value that is merely COPIED
against values that are BUILT.

So: **pins first, barrier only if the pairs re-fuse.** Cheaper than measuring a
barrier, and strictly safer, since pins do not perturb the schedule elsewhere.

## Keeping a register-to-register COPY needs BOTH ends pinned

Batch 214's `20089c0` park recorded that a pin says *where* a value lives and
cannot say it must be **copied** rather than used in place. That is true of ONE
pin. It is not true of two.

`OvlFunc_932_20087e8` materialises a data label into `r2`, stores through `r2`,
and only then copies it to `r5` for the loop that follows:

    ldr r2, =.L5238 / ldr r3, =0x0 / strh r3, [r2] / mov r5, r2

One pointer local is loaded straight into `r5` and the copy vanishes — one
instruction short, everything after it shifted. Two locals, the source pinned to
`r2` and the destination pinned to `r5`, keep it:

    register unsigned short *t __asm__("r2");
    register unsigned short *q __asm__("r5");
    t = (unsigned short *)L5238;
    *t = 0;
    q = t;

**Both pins are load-bearing.** Free either end and gcc coalesces the pair back
into one pseudo and the copy goes again. Naming both ends of a copy is how you
say "copy this", and it is the general form of the one-instruction-short tell.

## BLOCK LAYOUT FOLLOWS SOURCE ORDER — spell the ROM's order with `goto`

gcc lays basic blocks out in the order the source expands them. When the ROM's
order is not the order nested `if`/`else` would produce, no rearrangement of the
conditions reaches it; the blocks have to be named.

`OvlFunc_971_200808c` is 64 instructions whose arithmetic was right almost
immediately and which still missed by 45. All 45 were layout. The ROM's order is

    entry -> [set arm] -> [BODY] -> [clear arm] -> [join] -> return 0

with the join's `bne` reaching BACKWARD into the body — the body sits BETWEEN
the two arms of an `if`/`else` although it is textually after both. Writing the
two arms and the body as `goto` targets in the ROM's own order took 45 to 27,
and two entries already on file closed the rest:

  * `if (x == y) return 1; return 0;` is the **return-a-boolean** idiom and gcc
    if-converts it to seven branchless instructions (`eor / neg / orr / lsr /
    sub`). Invert to `if (x != y) goto out0; return 1;`.
  * Every zero return must reach ONE label. Separate `return 0` statements let
    gcc hoist a `mov r0, #0` above the first test so a path can fall through
    with the value already set.

**The tell that a residue is layout and not arithmetic**: the differing lines
are whole blocks appearing in a different order, with the instructions inside
each block already matching. Diff by block before trying to respell anything.

## A pooled constant may be a ROUTINE'S LENGTH — check the neighbouring symbols

`area.sym`, `message.sym` and `file_table.sym` cover id spaces; `const.sym`
covers pooled values belonging to none of them. Batch 216 adds a fourth kind and
`size.sym` to hold it: **the SIZE of a routine the game copies into RAM and runs
there**, which appears in the source as a link-time value and therefore always
pools.

The shape to recognise: allocate a scratch buffer, DMA a blob into it, call it,
free it. If the byte count is pooled where gcc would build it, check whether it
is the **gap to the next symbol**. On `DecompressIcon` and `LoadIcon` all three
counts are:

    Func_8015afc @ 0x08015afc   0x278 to the next
    Func_8015d74 @ 0x08015d74   0x9c  to the next
    Func_8015e10 @ 0x08015e10   0x7c  to its own .func_end

**WRITING THE SUBTRACTION IN C DOES NOT WORK.** `(unsigned)(Func_8015d74 -
Func_8015afc)`, both declared `extern unsigned char []`, emits TWO pool words and
a runtime `sub` -- gcc cannot fold a difference of two external symbols, and the
assembler never gets the chance when they live in a separately assembled file.
A single symbol carrying the size is what reproduces the ROM.

**THE CONFIRMATION TO LOOK FOR is the ROM disagreeing with itself.** 632 is
BUILT with `mov #0x9e / lsl #2` at four unrelated sites and POOLED only in the
two functions that copy this decoder. Same value, two treatments, split along
the meaning boundary -- that is what turns a pooled constant from a puzzle into
a tell. Look for it before adding any symbol.

## A PIN CAN FORCE gcc INTO A HIGH REGISTER, and sometimes that is the match

`templated.py` ranks on `hi`, the count of r8-r11 references, because high
register traffic predicts an intractable residue. That still holds as a ranking
heuristic, but it must not be read as "r8 means unreachable".

`LoadIcon` keeps the DMA control word `0x84000000` in **r8** across three
transfers, with `mov r7, r8 / push {r7}` to save it. Writing the transfers with
`dma.h`'s `DMA3_COPY`, which rebuilds the word per call, is **six instructions
SHORTER than the ROM** and needs no r8 at all. Hoisting the shared word into a
local pinned to r8 and passing the whole control word through `DMA3_SET` is what
makes gcc reach for the high register and emit the ROM's prologue.

So when the ROM uses a high register and we do not, the question is not how to
relieve pressure but **what value the ROM is keeping alive that we are
rebuilding**. Pin that value to the high register the ROM uses.

## THE FRAME SIZE IS A DIAGNOSTIC, and dma.h's inlines each cost a stack word

`sub sp, #N` disagreeing is not a scheduling residue -- it counts the function's
stack objects, so it identifies a WRONG NUMBER OF LOCALS before any instruction
is compared. Check it first.

`Func_8091494` screens with `sub sp, #0x8` against the ROM's `#0x4` because
`DMA3_FILL` and `DMA3_CLEAR` each declare their own `u32 value`; using both
inlines allocates two words. One local plus two `DMA3_SET` calls is the shape.
Two further rules came out of that local, both measured:

  * **Assign it late.** At the top of the function it emits `mov r5, sp` before
    the first call; the ROM emits it immediately before the first store.
  * **PIN IT, do not barrier it.** Unpinned, gcc keeps the pointer in r5 for the
    DMA argument and still writes the store as `str r3, [sp]`, folding an
    address it can prove equals the frame pointer. Pinning forces the store
    through the register. A barrier was tried instead and cost five lines.

**gcc will fold a pointer back to `sp` whenever it can prove the equality.** A
pin is the only thing measured that stops it.

## DO NOT NAME AN INTERMEDIATE THAT IS CONSUMED IMMEDIATELY

The tree's usual advice is to name a value to pin down where it lives or to stop
a fold. The reverse case is real and `OvlFunc_948_200a290` is the clean example.

It calls `__MapActor_GetActor` nine times and writes through each result.
Assigning each to a local gives `mov r3, r0 / add r3, #0x59 / strb` at every
site -- **the name is what buys the value a register of its own**, so the return
value gets copied into it. Using the call expression directly,
`__MapActor_GetActor(8)[0x59] = v;`, lets gcc advance the return register in
place, which is the ROM's `add r0, #0x59 / strb r5, [r0, #0x0]`.

The test is whether the value outlives its use. A result consumed by the very
next operation should stay anonymous; one that must survive a call, or land in a
particular register, gets a name.

## A GREEN SCREEN SAYS NOTHING ABOUT HOW THE TEXT IS TRANSPLANTED

`tryc.py` compiles the WHOLE scratch file. Batch 216 assembled a matched
function's `.c` from its scratch file with `tail -n +2` to drop a leading line,
which silently dropped `#include "dma.h"` as well; the screen had been exact and
the link failed on three undefined `DMA3_SET` references.

Copy the screened body verbatim and prepend the comment, rather than slicing the
screened file by line number. The build catches this one, but it costs a cycle
and it looks like a decompilation error when it is a transcription error.

## A PIN CANNOT ASK FOR A SPILL

Batch 210 recorded that a pin assigned BEFORE a call and used AFTER it is
silently dropped. Batch 217 found the corollary that matters: that hazard also
blocks the one use that would make a pin a spill tool.

`CreateSpriteLayer`'s residue is a variable the ROM SPILLS (`mov r4, #0 /
str r4, [sp]` before a call, `ldr r4, [sp]` after) and gcc keeps in a register.
Pinning it to r4 -- the very register the ROM spills from, and one that is
call-clobbered under this tree's `-fcall-used-r4` -- changes nothing, because
the pin spans the call and is dropped. Measured against the parked body: 62
differing with the pin, 45 without.

Two neighbouring tools were measured on the same site and neither substitutes:

  * **A BARRIER proves liveness, not spilling.** It forces the value to exist
    across the call and gcc satisfies it in a callee-saved register. 63.
  * **`volatile` IS NOT A SPILL.** It does produce the frame and the
    store/reload, but every later access goes through memory too, so the
    function grows accesses the ROM does not have. 56, and LONGER than the ROM.

So a spill remains the one allocation outcome no source construct commands.

## Do not put TWO COMPETING PINS on one register

The one-variable-two-ranges entry says a register carrying two values of
different types can be named by two `register` declarations on the same
register. `InitSprites` looks like that case -- r5 holds a zero across two DMA
writes, then a buffer size -- and pinning BOTH is worse than pinning one.

With both pinned the allocator puts a third value in r4 and shifts the size out
of place (`lsr r2, r5, #0x2` against the ROM's destructive `lsr r5, #0x2`), four
lines. Pinning only the first and leaving the second free lets gcc pick r5 by
itself and the block comes out exact.

**One pin plus a free choice beats two pins** unless both ranges are
simultaneously constrained. A pin is a constraint on the allocator, and two
constraints on one register leave it less room, not more.

## COMPUTE the bounds of a folded range -- reading them off the digits fails

The unsigned range idiom encodes `lo` as a NEGATED constant and `hi - lo` as a
span, so recovering the C bounds is arithmetic:

    lo = 2**32 - neg          hi = lo + span
    source:  x > lo - 1  &&  x < hi + 1

On `OvlFunc_913_200a88c` three of five boxes were first written with an upper
bound read off the hex digits and all three were wrong, because adding the span
carries into the next digit: `0xC00001 + 0x51FFFE` is `0x111FFFF`, not
`0xC51FFFF`.

**THE TELL IS UNAMBIGUOUS AND CHEAP.** A wrong bound shows up as a wrong SPAN
constant, not as a wrong branch -- `ldr r1, =0xb91fffe` against the ROM's
`=0x51fffe` says the bound is wrong while the idiom is right. Check the span
constant first; if it matches, the bounds are right.

Write the upper bound with the operator the ROM compares with: `<= 0x248ffff`
and `< 0x2490000` are the same in C and different in the emitted `cmp`.

## A dispatch that branches with `bhi` is switching on an UNSIGNED value

`cmp r3, #2 / bhi` in a switch tree means the switch expression is unsigned;
`bgt` means signed. On `OvlFunc_913_200a974` the counter declared `int` shifts
the whole function, and `unsigned int` fixes it.

One condition code identifies the type of a global. Check it before treating a
dispatch mismatch as a structural problem -- it is the cheapest possible fix and
it is invisible in the instruction mnemonics alone.

## A CROSS-JUMPED TAIL MAY SHARE LESS THAN IT LOOKS

When two arms jump to a common label, the shared block is only what is
*identical*. On `OvlFunc_913_200a974` both arms load the counter themselves and
only the compare-and-decrement is shared; writing the load once in the shared
block leaves the function two instructions short.

The test is to read what the shared block USES rather than what it does: if it
consumes a register the arms each set, that setting belongs in the arms.

## The two crossed-site cures, BOTH NEEDED IN ONE FUNCTION

`OvlFunc_913_200a974` settles the question of which cure to reach for by
containing both, four instructions apart:

  * One site falls to the **barrier-free reordering** -- writing the counter
    load between the constant's `mov` and its `lsl` puts the load where the ROM
    has it, with no side effects.
  * The other does **not**. Reading the byte into a local first still leaves
    `mov r3, #0xd` ahead of the `ldrb`; only a barrier on the loaded value holds
    the order.

So the ladder stands as recorded -- pins, then reordering, then barrier -- and
the presence of both in one function is the reason none of the three can be
promoted to a default.

## tryc.py HAS TWO BLIND SPOTS, and tools/objcmp.py exists for them

`tryc.py` compares INSTRUCTION STREAMS and normalises every PC-relative load to
`=value` so that pool PLACEMENT cannot cause false diffs. That normalisation
hides two real things, and batch 218 hit both in one round:

  * **POOL ORDER — a false PASS.** `Func_80982dc` screens with 86 instructions
    against 86, 196 bytes against 196 and ZERO differing mnemonics, and fails
    `make compare`. Its nine pool words are the ROM's nine, ROTATED: gcc emitted
    the last one first. A ROM `.s` reaches constants with `ldr rX, =value`, so
    the ASSEMBLER pools them in INSTRUCTION order; gcc emits its own `.word`
    list in ITS order. Every `ldr [pc, #N]` then points four bytes further.
  * **DUPLICATE LABELS — a false FAIL.** `OvlFunc_928_2008500` screens at "6
    differ" and is byte-identical: gcc put its pool-dump target and its epilogue
    label at one address, and a disassembly can only show one label there.

    python3 tools/objcmp.py <candidate.c> <reference.s> [--func NAME]

compares size, every encoding and every relocation. **Run it whenever tryc's own
`!!` warning says the reference keeps its pool inside the function** -- that
warning marks precisely the cases where the normalisation can hide something. It
does not replace tryc (which tells you WHICH instruction is wrong while you
iterate) or `make compare` (which sees layout and linker-script mistakes).

**WRITE AND BUILD ONE FUNCTION AT A TIME.** Batch 218 wrote two into the build
together and went red; because both screened clean, neither could be blamed and
it took a bisect to find which. The minute saved is not worth it.

## THE TWO NAMING RULES ARE ONE RULE

Batch 216: do not name an intermediate consumed immediately -- the name buys it
a register. Batch 218's `OvlFunc_897_200935c` is the exact inverse: its four
switch arms store through one global, and unnamed, the address is computed AFTER
the call into a scratch register, which makes each arm's store textually
identical to the early-return path's and lets cross-jumping merge them -- two
lines short. A named pointer per arm puts the address in a callee-saved register
before the call and the merge stops.

**NAME A VALUE THAT MUST SURVIVE SOMETHING** -- a call, or a register choice you
need. **DO NOT NAME ONE WHOSE ONLY ROLE IS TO BE CONSUMED IMMEDIATELY.** Both
batches are the same rule seen from its two sides.

## A POOLED SMALL CONSTANT IS NOT AUTOMATICALLY A SYMBOL

The `area.sym`/`const.sym` tell says a pooled value an eight-bit `mov` could
build names a symbol. `OvlFunc_928_2008500` shows the tell firing and being
WRONG: `(int)&_AREA_00` reproduces the pool TEXT, and a bare literal `0` is what
matches -- it gives a NARROW pool reference (`ldrh` against `.word 0`) encoding
the same halfword, and that single token fixes three residues at once (the pool
dumps inside the function with the ROM's branch over it, `.word 0` sorts ahead
of a symbol whose instruction comes earlier, and the address temporary is forced
into `ip`).

Ten spellings around that temporary tied at EXACTLY 27 differing; the exact tie
is the tell that the variable was never the problem. MEASURE the literal before
reaching for a symbol.

This contradicts `src/non_matching/ovl_7d6418/2008dd0.c` ("byte stores have no
QImode analogue of the halfword pooling exception. MEASURED: they do not"). One
plainly does; that park needs re-measuring with a bare `0`.

## LET gcc CROSS-JUMP A SHARED TAIL -- do not write the share as a `goto`

Where two arms end in the same code, writing the share explicitly as a `goto`
into a common label CONSTRAINS THE ALLOCATOR in a way cross-jumping does not.
On `OvlFunc_911_200a6cc` the explicit form never beat 2 differing across six
configurations -- limit pinned, limit free, bound named and pinned, bound named
free, bound assigned early, and a barrier keeping the limit live -- because the
bound's pool load kept landing in the register the limit had just vacated.
Writing both arms out in full and letting jump.c merge the identical tails is
exact. gcc merges them either way; only one way lets it choose registers first.

## `do { } while (0)` IS A SCHEDULING BARRIER THAT EMITS NOTHING

On `OvlFunc_880_2008154` the last five differing lines were purely the order of
two independent pool loads. Wrapping a pair of statements in `do { } while (0)`
splits the block into two scheduling regions and closes it, at zero instruction
cost. The volatile-asm barrier was tried at both plausible points and lands the
load too early (9 differing) or still needs help (3).

Reach for this before the asm barrier when the residue is the ORDER OF TWO
INDEPENDENT SETUP SEQUENCES rather than a mov/shift crossing.

## DO NOT PASS A COMPUTED VALUE AS AN INLINE'S ARGUMENT

gcc evaluates an inline's arguments BEFORE the inlined body. On
`OvlFunc_880_2008154` that hoisted an expression above the interrupt guard the
inline installs -- an argument is not "at the call site", it is before
everything the inline does. Specialising a second inline that builds the value
at its point of use took 50 differing to 38.

## WIDEN A VARIABLE SO AN EXPLICIT CAST SURVIVES

Held as `u16`, a counter has its narrowing folded away at tree level and gcc
compares the register directly. Where the ROM keeps the UN-folded zero extension
(`lsl rX, #0x10 / lsr rY, #0x10`), the variable must be `int` with explicit
`(u16)` casts at the use sites. 38 differing to 5 on `OvlFunc_880_2008154`.

## Three smaller entries from batch 218

  * **A NAMED BYTE OFFSET BLOCKS REASSOCIATION.** `off = idx * 4 + 0x14;` then
    `*(T **)(p + off)` keeps the ROM's separate `add`; written as one expression
    gcc folds the constant into the load's displacement instead.
  * **STATEMENT ORDER DECIDES REGISTER NAMING.** On `Func_808bd24`, fetching one
    global before another rather than after is 12 differing against 24 -- a
    reordering that changes nothing semantically.
  * **A MAIN-ROM FUNCTION CAN REACH `divsi3_RAM` LEGITIMATELY** when the ROM
    calls it INDIRECTLY: a function-pointer local holding `divsi3_RAM` emits
    `bl _call_via_rN` and side-steps the `__divsi3` alias blocker that parks
    `src/non_matching/rom_8a000/809088c.c`. Writing `/` would not. Also on that
    function: **the callee's RETURN TYPE decides r0 fill order for a DIRECT
    call**, not only an indirect one -- `extern int StopTask(void *)` fills r0
    last and matches where `extern void` fills it first.

## When a ROM folds a constant into the INDEX, the BASE is a real variable

`Func_80b5a0c` keeps `s + 2` in `ip` across three paths and folds `0x64` into
the index register (`strh r2, [r5, r3]`, r3 = 0x64 + 2*idx). Collapsing that to
the obvious single pointer `(short *)(s + 0x66)` is TEN INSTRUCTIONS SHORT;
writing the base out as its own variable and indexing `p[0x32 + ...]` recovers
all ten.

The general form: if the ROM could have folded a constant offset into the base
and did not, the base is being held for another reason -- usually because it is
live on a path where the index is not.
