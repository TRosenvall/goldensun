# Elevating assembly to C

How functions get converted here, and the compiler behaviours that stop them.

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

### 1. Narrow constant materialisation — **34 functions, solve this first**

The single highest-value problem in the project. Counted, not estimated:

    python3 - <<'PY'
    import glob, re
    for p in glob.glob("asm/**/*.s", recursive=True):
        L = open(p, errors="replace").read().split("\n")
        if not L or "Generated by gcc" in L[0]: continue
        for i, l in enumerate(L):
            if re.sub(r"\s+", " ", l.strip()) == "neg r3, r3" and \
               re.sub(r"\s+", " ", L[i-1].strip()) == "mov r3, #0xd":
                print(p); break
    PY

**34 hand-written functions** contain the shape below, across the overlays and
the main ROM. Several are the same function duplicated per map, so one fix
lands many at once.

    rom    mov r3, #0xd / neg r3, r3      (~0xc as 0xfffffff3, 32-bit)
    ours   mov r3, #0xf3                  (~0xc narrowed to a byte)

The ROM masks a byte field in 32-bit width; gcc proves the loaded value is
0..255 and picks the cheaper 8-bit immediate. Same for halfword fields, where
the pool entry is loaded with `ldrh` instead of `ldr`.

Tried and failed: the loaded value in `s32`, `u32` and `u8` locals; the mask as
`0xf3`, `~0xc` and `~(3 << 2)`; the mask via a named constant; an explicit
`(s32)` cast on the load; operands in either order. gcc narrows through all of
them because the `ldrb`/`strb` pair tells it the width.

What has NOT been tried, and is where the next attempt should go: making the
loaded value's width genuinely unknown to gcc — a `volatile` field, a union, a
bitfield, or a read through a pointer gcc cannot trace to the byte field.

Blocked functions include `Func_800c548`, `Func_800c570`, `MapActor_Emote`,
`DisplayMenuArrowCursor`, and four copies of `OvlFunc_*_20089dc`.

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
