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

## What the screen has to normalise

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

## The declaration lever is an -O2 behaviour

The rule below decided functions in nine batches and every one of them was
compiled at -O2. It does **not** apply at -O1.

Confirmed by a natural control: `OvlFunc_923_2008ed0` (rom_7aa430, -O1) and
`OvlFunc_924_2008f84` (rom_7ac2d8, -O2) are the same forty-one instructions.
The identical C matches at -O2 and fails at -O1, with neither declaring nor
withholding the prototype moving the argument pair.

Sixteen `.o` prefixes build at -O1; the Makefile lists them explicitly, and
`tools/tryc.py` prints `(built with: O1)` when it picks one up. **Read that
line.** Two of the parked functions sit in -O1 units and their diagnoses were
written before this was known.

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
