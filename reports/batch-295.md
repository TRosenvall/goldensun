# Batch 295 — fourteen functions, and a relocation form that was never a residue

Seven agents. **14 landed**, gate green at
`5c4695205413df7db52b9a184815a07783999971` throughout.
State: **4,736 of 5,710 elevated (82.9%)**, 664 parked, 220 available.
Reconciliation, exact and the 27th consecutive: 14 landings = 11 from parks
(11 retired) + 3 fresh; parked 673 − 11 + 2 new = 664; available 225 − 5 = 220.

The batch was built on batch 294's finding that **closing briefs on near-exact
parks outperform fresh targets**: 6 of the 7 briefs were closing briefs, and
they produced 11 of the 14. The seventh was a fresh harvest of five small
functions and produced 3 landings and 2 parks — a better rate than batch 294's
fresh targets, which produced nothing, but on materially smaller functions.
Every park that did not close was closed out as a *characterised* negative
rather than left open.

## Landed

| function | was | encodings | what closed it |
|---|---|---|---|
| `OvlFunc_881_200a8e8` | park | — | brief G |
| `OvlFunc_895_2008a24` | park | — | brief G |
| `OvlFunc_924_20099b8` | park | — | brief G |
| `OvlFunc_959_2009150` | park | — | brief D |
| `OvlFunc_959_2009528` | park | — | brief D |
| `SetRegAnimDest` | park | — | brief C |
| `Func_807a664` | 8 of 143 | 143 | three coupled constructs, each a single drop |
| `Func_808ce74` | 2 of 121 | 121 | the residue was a relocation FORM, not a value |
| `Func_80a77a4` | 8 of 76 | 76 | one named local, which forces global allocation |
| `LoadPortrait` | 12 of 71 | 71 | an operand-bearing empty asm — booked as a fakematch |
| `OvlFunc_common1_1078` | 7 of 217 | 217 | a one-member union as an alias-set escape |
| `Func_80e6d3c` | fresh | 166 | indexing instead of post-increment — a preheader lever |
| `OvlFunc_890_2008d9c` | fresh | 158 | per-switch-arm locals; pinned, booked as a fakematch |
| `OvlFunc_945_200b364` | fresh | 177 | a `"+r"` barrier on the *dominating* site |

## The result that matters most: objcmp cannot see the link

`Func_808ce74` sat at 1 differing encoding of 121 with a relocation mismatch,
and **both were the same artefact of comparing unlinked objects.** The reference
leaves 0 at 0xfc with an `R_ARM_ABS32` against `ewram_2020000`
(`wram.sym:137 = 0x02020000`); we place the value directly. The REL addend is 0,
so the linker writes the same four bytes and the linked output is identical.
`make compare` — the project's real gate — proves it.

So when the only residue is a word a relocation would fill, the symbol's value is
known and the addend is 0, **the function may already be exact: take it to the
compare instead of parking it.** The reverse does not hold; a non-zero addend, a
different symbol or a different relocation type is a real difference.

A trap in the same function: the two spellings are *not* interchangeable.
`ldr rN, =ewram_2020000` is a local-alloc pseudo and consumes no step of
reload's spill-register round robin (`reload1.c:5003` starts at
`last_spill_reg`, `4937` advances it); the literal makes reload materialise the
constant, advancing the rotation one step and changing which register a *later*
reload receives. That was worth the last encoding.

## New mechanisms

**A one-member `union` is a per-MEM alias-set escape.** `lang_get_alias_set`
(`c-common.c:3329-3345`) returns alias set 0 as soon as a `COMPONENT_REF`'s
containing type is a `UNION_TYPE`, and only when the access is *directly through
the union* — the pointer-cast spelling `*(int *)&act->f8` measures 5 and gets
nothing, which is the control that proves it. What it buys is a true
load-after-store dependence, the only kind carrying priority here, since
`arm_adjust_cost` returns 0 outright for ANTI and OUTPUT deps. Strictly better
than `-fno-strict-aliasing`, which reaches only 7 → 6 and breaks two other pairs.

**`do { } while (0)` places a sched2 barrier.** `haifa-sched.c:3727-3757` sets
`schedule_barrier_found` on `NOTE_INSN_LOOP_BEG`/`LOOP_END`, and that insn then
depends on every register's last uses and sets plus `reg_pending_sets_all`.
`SET_IO` is a `do{}while(0)`, so choosing it per call site *moves* the barrier —
which is why `common1_1078`'s first and third DMA pushes cannot share one inline
helper.

**The local-alloc refusal is a lever, and `STACK_REG` is a gate you cannot
open.** `local-alloc.c:362-368` refuses a pseudo unless it dies exactly once and
its class is not likely-spilled. Forwards: naming an expression so it is read
twice makes it a *global* allocno, which is allocated in `allocno_compare` order
and honours `preferences: 0` from an argument copy over `REG_ALLOC_ORDER`
(`global.c:1103`) — `Func_80a77a4`, 8 → 0. Backwards: an address pseudo set by
`(plus (reg) (const_int N))` and used as a memory base gets `pref STACK_REG`
when N is not a valid `add rd, sp, #imm` operand, isolated on four one-line
variants (`+0x55` and `+0x65` STACK_REG, `+0x54` and `+0x64` BASE_REGS — it is
the **constant**, not the mode). `reg_class_size[STACK_REG] == 1`, so the pseudo
is refused *even when it dies exactly once*, and a walk pointer at a ROM-fixed
odd offset is a global allocno by construction. That closes `Func_8096ddc` and
`OvlFunc_884_200a440` structurally.

**`allocno_compare` is reachable, but usually only through a shim.**
`LoadPortrait` needed `id` to outrank `b` and got there with
`__asm__("" : : "r" (id))`, which emits nothing and moves `id` from 7 refs / 47
insns to 9 / 48 — 0.56 against 0.41 — closing twelve differences at once. The
reason it is *booked* rather than pursued further is itself the result: the
pin-free route was shown structurally closed, because every reference the
thresholds need is already an operand of the ROM's own instructions, and
`regs_may_share` (the one route that merges two pseudos and sums `n_refs`) is
written only at `loop.c:1832`, unreachable without a loop.

## Three more mechanisms, from the fresh harvest

**Making an expression loop-invariant-by-construction is an ordering lever.**
`Func_80e6d3c`'s whole residue — 18 of 166 — was `_UpdateSprite(*list++, ...)`
against `_UpdateSprite(list[i], ...)`, with size, count and relocations exact
either way. Both strength-reduce to the ROM's `ldmia rX!, {r0}`, so the
difference is not in the body: it is in the **preheader**, and it is *which*
invariants `loop.c` gets to hoist. With `*list++` the list init is expand-order,
ahead of `loop_start`, so `move_movables`' insertion point puts the `&pos` hoist
after it; with `list[i]` the base becomes a second invariant and all three hoists
emerge in loop.c's scan order, which is the ROM's. Confirmed *not* sched2 —
`-fno-schedule-insns2` gives the same relative order. This is distinct from every
"where the assignment sits" note already in the method, all of which concern
expand order inside one block.

**"Give each block its own local" is now a rule, confirmed on three functions in
one brief.** One local shared across several sites is as damaging as one constant
shared across two:
- three sites sharing one `int v` → 19 of 179; separate `v`/`w`/`y` → exact roles
- one shared `t`/`q` pair across five loops → 26 of 165; block-scope per loop → correct
- and the boundary: **one shared pair across switch arms is wrong** — bare
  literals 6, one shared pair 8, per-arm exact. That corrects the elevated
  file-mate `src/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_c_a_b.c`, whose note
  "the four `__CopyMapTiles` calls share two named locals" is right for
  straight-line code and wrong across arms.

**To move a giv out of a high register, raise a competing quantity's `n_refs` by
sharing a subexpression.** `Func_80f0678`'s 16×6 nest needs three outer
accumulators and only two can hold low registers, because `add r3, r7, r6` is a
three-operand Thumb add. `global.c:607`'s ratio ranks one above another and
pushes the loser into r14, costing `mov r2, r14` before every add. Naming
`m = j * 2` and using it in **both** terms flips the priority: 19 of 165. Three
algebraic respellings without the sharing are all inert (23 each) — it is the
reference count, not the algebra. The existing "it is the SHIFT, not the algebra"
rule governs *whether* loop.c reduces; it does not extend to giv order or
register choice.

## Two rules that needed splitting, not correcting

**A `"+r"` barrier on a repeated pool constant is POSITIONAL, not inert.** The
method said inert and sometimes destructive. On `OvlFunc_945_200b364` it was the
entire residue — on the **first, dominating** `__GetFlag(0x928)` it is
byte-exact; on the second site it is worth nothing. cse records the equivalence
when it processes the **defining** site, so a barrier there means there is never
an equivalence to propagate. And what makes the shared constant *cost* an
instruction is `local-alloc.c:886`'s `REG_N_REFS (regno) == 2` rule: two pseudos
with two references each both rematerialise into a bare `ldr r0, =0x928`, while
one pseudo with three references misses that test and survives in a callee-saved
register.

**"Name the expensive argument at the TOP of the function" is two rules.**
`__MapActor_SetSpeed(9, 0xcccc, 0x6666)` wants top-of-function locals, because
**commoning needs room**; `StartTask(Func_80f0538, 0x90 << 3)` wants an
**adjacent block-scope** local, because lowering one cheap argument's LUID needs
**adjacency**, and top-of-function does not work there at all.

Also found: **the `|` operand-order lever's premise is wrong.**
`0x80 | p[0x59]`, `p[0x59] | 0x80` and `p[0x59] |= 0x80` are byte-identical,
because `commutative_operand_precedence` canonicalises the `CONST_INT` into
operand 1 first. What flips the two-address `orr`'s register roles is whether the
0x80 is a `CONST_INT` or a **pseudo**: `{ int k = 0x80; p[0x59] |= k; }` is worth
2 of 177. The handle is the carrier, not the spelling.

And a boundary on the constant-splitting lever: three separately named `int`
locals for `__Func_8012330(0x10000, 0x10000, 0x10000)` give **identical output**,
no split at all, because `precompute_register_parameters` has already reduced the
three identical `CONST_INT`s to one pseudo before allocation. Pins are the only
remedy found, which is what the elevated file-mate already uses for that callee
family.

## Two tools fixed

**`datacheck.py` was over-attributing data labels.** `split_requirements` ran
each function's body to the next `.thumb_func_start`, which swallows the
following function's `@` doc-comment block — and `LABEL_REF` matches inside
comments, so a label merely *named in prose* was reported as a read.
`Func_80f0678` was asked for three exports where its body references exactly one.
The body now ends at the function's own `.func_end` and `@`-to-EOL is stripped
first; `--all` narrows from 83 files to 81. The error was conservative and cannot
have broken a link, but its output was being handed to briefs as "the exact export
list".

**This is the third bug of one class in this tool family** — a regex matching
inside hand-written disassembly's own prose. The others were datacheck's
generated-file guard (19 files silenced) and `grep -c thumb_func_start` counting
an `@` comment.

**`tryc.py --cflags` could not pass two flags.** The bare-flag argv scan appended
any argv entry starting with `-f` *whole*, and the value of `--cflags` is itself
such an entry, so cc1 got one bogus option and died — and the run reported
nothing varied. That is the same false negative the block exists to prevent,
arriving by the other door: three flag *pairs* were "measured" without ever
reaching the compiler and had to be re-measured. Also recorded:
`-fno-cse-follow-jumps` does **not** stop cse commoning a repeated pool constant,
so it is the wrong reach for that signature.

One structural check worth keeping: gcc-2.96 **can** allocate r12 and lr in Thumb
(`arm.h:773` leaves indices 12 and 14 clear in `FIXED_REGISTERS`, `REG_ALLOC_ORDER`
lists them 5th and 6th, and `CONDITIONAL_REGISTER_USAGE` fixes only the FP regs).
A ROM `mov r12, r14` / `add r14, r3` is therefore not evidence of a different
toolchain.

## Corrections to the method

- **`docs/elevation.md:7402` was right about the operators and wrong about the
  reason.** The boundary is the Thumb pattern's **arity**, not commutativity:
  `*thumb_andsi3_insn`, `*thumb_iorsi3` and `*thumb_xorsi3` constrain operand 1
  to `"%0"` and are genuinely unreachable, but `*thumb_addsi3` alternative 3 is a
  real three-register `add` and IS reachable — through a third destination,
  because `optabs.c:661` swaps the operands back whenever `target == op1`.
  `*thumb_mulsi3` emits `mov %0,%1 ; mul %0,%0,%2`, which reconciles batch 294's
  correction 3. Screen `arm.md` first, then try **one** spelling, not twelve.
- **A park's stated blocker was wrong again — four of the last five batches.**
  `common1_1078`'s was a missing 1-point priority edge, not the sched2 tie it
  claimed. `Func_80a77a4`'s named the wrong pass entirely. `Func_807a664`'s own
  arithmetic did not add up ("two places", three listed, summing to 7 of 8).
  `Func_80a524c` was the exception and the first correct blocker in four batches.
- **A line-level diff of generated `.s` is not an object-level screen.** The
  `dma.h` screen first reported 33 of 33 files changed; at the object level 13
  were byte-identical, differing only by `.L10` → `.L9` renumbering.
- **`--align` can be wrong in both directions.** It read 10 instructions in
  disagreeing regions on a *byte-identical* function (`Func_807a664`), and 53
  against objcmp's 11 on a function with a mid-function literal pool
  (`Func_8096ddc`), where the pool's label naming shifts the whole tail. objcmp
  is the authority.
- **A substring grep over hand-written asm counts prose.**
  `grep -c thumb_func_start` returns 2 on a single-function file because an `@`
  comment discusses the directive. This is the third occurrence of that exact
  class — it is also what silenced 19 files in `datacheck.py`. Anchor it.
- **`reload1.c`'s `Using reg R` lines do not predict the emitted register**
  (insn 92 dumped as `reg 2`, emits r1).

## Decided

**`dma.h`'s helpers stay `static inline`.** Screened at the object level across
all 82 landed users: a macro form changes **51** of them (`DMA3_CLEAR` 20 of 33,
`DMA3_FILL` 1 of 4, `DMA3_COPY` 33 of 53), so it cannot be made in place. A
future function needing the macro form must introduce a second, differently named
macro adopted per site. This is why `SomethingSaveHeader` stays parked at 2 of
156.

## Housekeeping closed

**The fakematch ledger is now complete.** `shimcount.py` had found 507 landed
files carrying a fakematch-class shim against only 455 booked rows. Each of the
52 was tested rather than booked on sight — de-pin the file and objcmp against
the tracked generated `.s`:

- **49 were load-bearing and are booked**, with the function name taken from
  objcmp's output rather than guessed from the path. Residues run from 2 of 38 to
  820 of 947, so most are not marginal.
- **3 were vestigial** — byte-identical with the pin gone — and the declarations
  are removed. These were reserving a register for nothing, which is what the
  standing "sweep for unused `register … __asm__`" item was after.

`shimcount` now reports zero unbooked files; `fakematch.txt` holds 591 rows.

## The int-return sweep: a tree-wide negative

A park declaring `extern void F(...)` for a function the tree defines as `int` is
both a documentation error and a known lever, so it was swept mechanically.

**The documentation debt is large: 250 parks carry 445 declarations that
disagree with the tree's own definitions**, 146 of them the void/non-void case.
**As a lever, blind application is worthless.** Of the 36 parks that both carry a
runnable recipe and declare `void` for a non-void function (67 sites), flipping
every one moved exactly **one** park by **one** encoding (`Func_80ba2c0`,
205 → 204); one spot check went the other way to 502 differing with a size
change. That is consistent with the mechanism rather than a refutation — the
lever acts through a dependent count and only breaks a `rank_for_schedule` tie,
so with no tie there is no effect. Try it when the residue is *already known* to
be a sched2 ordering at a call; not otherwise.

Two tooling traps came out of that sweep, both of which cost a wasted run:
`objcmp --func` prints `differ in N place(s)` where `--whole` prints
`N of M differ`, so a parser written against one silently returns None on the
other and a sweep that drops None reports a **false negative**; and **only 120 of
664 parks have a recipe an automated sweep can run at all**, 4 of which name a
reference `.s` that no longer exists.

## Open

- `Func_80a524c` (7 of 134), `Func_8096ddc` (11), `OvlFunc_884_200a440` (12),
  `Func_809b450` (2), `SomethingSaveHeader` (2) — all five updated, all
  parkcheck-verified at the count they state, all with the mechanism closed out.
- Two new parks, both parkcheck-verified and both with the blocker named by its
  pass: `Func_80f0678` at 6 of 165 (loop.c's `strength_reduce` — `record_giv`
  *prepends* to `bl->giv`, so giv initialisations emit in reverse scan order, and
  every spelling that would reorder them also reclassifies the constant from an
  inner giv base to an outer invariant that gcc hoists out of the nest; four
  spellings, all identical at 167 encodings and 105 differing, so the two
  properties are coupled and it is unreachable by construction) and
  `OvlFunc_924_200cfcc` at 5 of 179 (`cse.c:3652` canonicalises the `SYMBOL_REF`
  into position 1, then `local-alloc.c:1131` ties the output to the first input it
  can combine with; reversing the source addition is inert because `fold` puts a
  `TREE_CONSTANT` address second before cse sees it).
- Withheld symbols: `_MSG_c30`, `_MSG_970`, `_LEN_2c4`, `_CONST_50`, and now
  `_MSG_ad4` / `_MSG_b2c` — computed, not asserted: all three *admitted*
  neighbours are shiftable (`_MSG_182` 193<<1, `_MSG_ad0` 173<<4, `_MSG_b20`
  178<<4) and neither proposal is, so they lack the pool tell `message.sym`'s
  criteria are built on. Their evidence is positional only, and they complete no
  function either way.
- 250 parks with a wrong return-type declaration, to correct as documentation.
- ~469 parks with no `Verify with:` recipe; 4 recipes name a missing `.s`.
- `datacheck.py` reports exports for text/**data** splits but not text/**text**.
- Fold `.call_via` into `census.py` as a column beside ARM and hand-written asm.
- The available pool is 225, but only **20** remain in the 101–200 band that
  reliably produces landings; 17 of the 225 are `.call_via` and unelevatable.
