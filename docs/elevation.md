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

This is worth running across the whole parked set. The failure signature —
correct instruction count, every differing line a callee-saved register rename —
currently reads as "register allocation, unreachable from C", which is the
largest blocked class in the corpus.

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

`.pool_aligned` is `.align 2, 0` + `.pool`, so this is pool PLACEMENT, and no
source spelling reaches it. Measured: across every elevated translation unit,
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
