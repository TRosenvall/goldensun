# Batch 187

Seven elevated, one parked. **Three matched on the first candidate with zero
probing**, all three from the same step: grepping the corpus for a solved
neighbour before writing anything. The round produced a new blocker class (the
static chain), the first source-level equivalent this notebook has for
`-fno-gcse`, and a rule about which duplicate blocks gcc will and will not
merge.

## Function breakdown

| # | function | address | file | what it took |
|---|---|---|---|---|
| 1 | `Func_80b196c` | `0x080b196c` | [rom_b0070_…_a_c.c](src/rom_b0000/rom_b0070_a_a_c_c_c_a_c.c) | the solved `.s` sibling — **zero iterations** |
| 2 | `Func_80a3e28` | `0x080a3e28` | [rom_a1814_…_a_c_b.c](src/rom_a1000/rom_a1814_c_a_c_c_c_c_c_a_c_b.c) | a cross-bank neighbour found by **callee names** — zero iterations |
| 3 | `Func_8015fb8` | `0x08015fb8` | [rom_15e8c_a_c_a_a_b.c](src/rom_15000/rom_15e8c_a_c_a_a_b.c) | **a static chain — the original was a nested function** |
| 4 | `Func_80bd850` | `0x080bd850` | [rom_bbb0c_…_a_c_b.c](src/rom_b5000/rom_bbb0c_a_c_a_a_c_b.c) | the static-chain recipe, generalised |
| 5 | `Func_8078af8` | `0x08078af8` | [rom_78a8c_c_a_b.c](src/rom_77000/rom_78a8c_c_a_b.c) | callee-grep + loop rotation |
| 6 | `OvlFunc_899_2008690` | `0x02008690` | [ovl_30_a_c_a_c_c_a_c.c](src/overlays/rom_794ac0/ovl_30_a_c_a_c_c_a_c.c) | **gcse hashes the alias set**; store placement decides cross-jumping |
| 7 | `Func_808b98c` | `0x0808b98c` | [rom_8b674_c_a_b.c](src/rom_8a000/rom_8b674_c_a_b.c) | **constant hoisting is loop-size arithmetic, threshold 15** |

Parked: `Func_80979a4` (13 of 45).

## A NEW BLOCKER CLASS: THE STATIC CHAIN

`gcc-2.96/gcc/config/arm/arm.h` sets `STATIC_CHAIN_REGNUM` to r8 under ARM and
**r9 under Thumb**. So a Thumb function that *reads* r9 without ever defining it
is reading a static chain pointer, and **the original source declared it as a
nested function**.

The tell is a triple, and all three parts should be present:

1. r9 saved in the prologue and restored in the epilogue;
2. a `mov rX, r9` that no instruction in the function ever defines;
3. a lone stack slot that nothing reads back.

**The caller settles it and gives the corpus-wide grep** — `add rN, sp, #K /
mov r9, rN` immediately before the `bl`. That is gcc handing a callee a pointer
into the caller's own frame.

A standalone translation unit cannot declare a nested function, so the chain is
**transcribed** rather than expressed: an uninitialised `register` bound to r9,
copied into a `volatile` stack slot as the **first** statement. Both details are
load-bearing, measured on the second instance:

| variation | disagreeing |
|---|---|
| local `register` binding, store first | **0** |
| binding moved to file scope | 6 |
| store moved one statement later | 6 |
| plain uninitialised `int`, no r9 binding | 7 |
| non-`volatile` slot | 10 |

The store's position matters because it sets the *next* value's live length,
which local-alloc uses to decide which value wins r0.

**The discriminator against a fifth stack argument**, which is what the shape
looks like at first: `sub sp, #4 / str rX, [sp]` before a `bl` is a stack
argument at seven of its eight sites in this corpus. A real five-argument
indirect call fills r2 and r3 and forces the r4 veneer — measured at 17
differing. **Where the ROM keeps the r3 veneer with r2 and r3 unfilled, the slot
is the static-chain frame slot.**

Both files say plainly that the transcription is **provisional**. For
`Func_8015fb8` the caller is in the same parent `.s`; once that piece is
elevated the pair can be written as the original was, and the register binding
and volatile slot both disappear. Recording that intent in the header matters —
otherwise the next person preserves a workaround that has stopped being
necessary.

## GREP ON CALLEE NAMES AND GLOBALS, NOT ON THE TARGET'S STEM

Three zero-iteration matches came from this, and `Func_80a3e28` sharpened it.
Its stem-sibling is *literally the function it tail-calls* — and that sibling was
**less** useful, because it walks the same array to clear slots rather than to
make this call. The useful neighbour was in a different bank entirely: a solved
function calling **both** of this one's callees over the same global's node
array, with the same post-increment read, the same skip-if-zero guard and the
same descending counter.

`Func_8078af8` repeated the pattern: intersecting its two callee names found a
solved function in a different split of a different parent, whose header already
carried the two levers this one needed structurally. The stem-adjacent file —
the literal neighbouring split of the same parent, which also calls one of the
helpers — was worth nothing.

**Callee-set identity beats filename adjacency.** Extract the reference, list its
`bl` targets and the globals it loads, and grep those.

One negative, so nobody builds it into a tool: **"has an already-elevated
sibling in the same family" is not a ranking signal** — 726 of 764 remaining
candidates have one. Near-universal habit, useless discriminator.

## A REGISTER ROTATION THAT NO STATEMENT ORDER CAN REACH

`Func_80979a4`'s park is the round's most reusable negative. Structure,
instruction count and order, constants, branch senses and pool contents are all
exact; the entire residue is a three-way register rotation, and the park proves
it is unreachable.

From `-da`: the two rotated pseudos score 0.8 and 0.214 under
`floor_log2(refs) * refs / live_length`, so the hue value is allocated second
and takes r2 after conflicting with hard r0/r1/r3. For the ROM's assignment it
would additionally have to conflict with hard **r2** — and for the constant to
outrank it, the constant would need a live range under a quarter of the hue's,
which is impossible in a 45-instruction function. **Something must occupy hard
r2 across that live range in the original.**

Two probes confirm the mechanism rather than assuming it. Binding the constant
to r2 drops the disagreeing regions from 17 to 8 and makes every scratch
placement exact — but it breaks a range-test fold, so it is a diagnostic.
`-fno-expensive-optimizations` reaches the same assignment for a different
reason and is *affirmatively wrong*: it also replaces the ROM's cheap
two-instruction constant build and unsigned branch, which **proves this file is
built with that optimisation on**.

> **The rule:** when the only residue is a register rotation, compute
> `floor_log2(refs)*refs/live_length` for the two rotated pseudos from
> `.17.lreg`. If the ROM's assignment cannot be reached by any allocation
> *order*, stop sweeping spellings — the value is being pushed off by a
> **hard-register conflict**, and the lever is whatever puts a value there.
> Sixteen unrelated spellings tying at exactly 13 is that signature.

## GCSE HASHES THE MEMORY ALIAS SET

The round's most reusable finding, and the first source-level equivalent this
notebook has for `-fno-gcse`.

`gcse.c`'s `hash_expr_1` folds `MEM_ALIAS_SET` into the expression hash, so two
loads of the **same address** land in different buckets — and are never compared
at all — whenever their alias sets differ. gcc-2.96 assigns a **distinct alias
set per struct tag**: probed, two struct tags over the same address get
different sets, while a bare `short *` and an `unsigned short *` share one.

> **When the ROM RELOADS a field that gcse would otherwise common, reach the two
> reads through DIFFERENT STRUCT TAGS.**

Verified against the flag — `-fno-gcse` on the single-alias-set spelling
reproduces exactly the same reload. Worth **49 instructions**. It depends on
strict aliasing being **on**, so a file using it must never fall under an
`-fno-strict-aliasing` rule; with the flag the same source grows from 316 bytes
to 344.

## WHERE THE STORE GOES DECIDES WHAT GCC CROSS-JUMPS

"Duplicated ROM code means duplicated source" is right, but "let gcc decide what
to merge" is **not passive**. Read from `jump.c`: cross-jumping runs once, after
scheduling; for a simple jump it first tries the block that *falls through into*
the target label, and only then the pairwise search — which is gated on the
label's uid being below a maximum **fixed at pass entry**. Any label the pass
itself creates fails that test and is never searched pairwise.

> **Write the store inside each innermost branch.** The first merge folds the
> arms' identical stores into a *new* join label; from then on the arms can only
> be compared against whatever falls into that join, never against each other.
> The `if` bodies survive as separate copies — which is the ROM.

Writing the store after the `if/else` leaves a **pre-existing** label, the
pairwise search runs, and gcc collapses the duplicates: **93 differing against
0**. The sibling `OvlFunc_899_20085bc` matched by accident of already having the
right shape; this one fails at 93 without the rule.

## A large diff can be several blockers stacked

Two of my own candidates for `OvlFunc_899_2008690` tied at **exactly 91**, which
the notebook's rule correctly reads as "the residue is not in the variables you
are changing". But my follow-up guess — wrong arm order — was wrong, and
reordering the arms alone made it *worse* (93). The deficit was two independent
facts: gcse commoning a reload (49) and cross-jumping collapsing an arm (29).
**An identical tie says the residue is structural; it does not say the structure
is one thing.**

## CONSTANT HOISTING IS LOOP-SIZE ARITHMETIC, AND THE THRESHOLD IS 15

Read from `loop.c`: the move threshold is
`(has_call ? 1 : 2) * (1 + n_non_fixed_regs)`, and a movable is taken when
`threshold * savings * lifetime >= insn_count`. For a Thumb function containing
a call that threshold is **15** — measured, not inferred, since `.08.loop` says
*moved* at 15 real insns and *not desirable* at 16.

A constant `expand` creates for a store has savings 1 and lifetime 1, so:

> **A loop of ≤ 15 RTL insns hoists every such constant into a callee-saved high
> register. A loop of ≥ 16 hoists none.**

That single boundary was `Func_808b98c`'s whole blocker — the ROM keeps one
constant inside the loop and two outside, reachable only at ≥ 16 with the other
two written as pre-loop locals.

Two corollaries, both measured. **The count must clear the threshold on both
loop passes** (`-frerun-loop-opt` is on at `-O2`, and the dump prints two
counts): one candidate hoisted a constant on pass 1, shrinking the loop 16 → 15,
and pass 2 then hoisted the one that should have stayed. And **the cheap,
output-neutral way to add an RTL insn is a narrowing temporary** — reading a byte
field into an int-width local and storing back through an `unsigned char` one
costs two RTL insns at loop time and combine folds both away. Every other way of
adding an insn (an extra pointer IV, a re-read, a dead counter, an extra copy)
was either deleted before the loop pass or survived into the output.

**Regmove's two-operand tie is lost to a subreg**, and the same temporary fixes
it. The Thumb `and` pattern ties its destination to its first input; when that
input is a subreg of a byte register — what masking a byte field directly
produces — regmove refuses the tie and inserts the copy on the *other* operand,
so the mask ends up in the destination. An int-width local makes the operand a
plain register and the tie lands correctly. **An `and` accumulating into the
wrong register is a subreg-versus-register question, not an operand-order one** —
swapping the operands is byte-identical.

**Which of two preheader constants gets which high register is source order**
(the second-assigned wins the lower-numbered register), and the *gap* between
them matters separately: assigning the two adjacently let a later pass build one
of them a byte shorter than the ROM, and putting a third initialiser between them
restored it.

## Other mechanisms worth keeping

**Inline asm cross-jumps.** An emitted veneer tail reached from two arms is
still two written-out source copies — writing it once and jumping costs 12
instructions. gcc compares inline-asm insns by template string. That extends
"duplicated ROM code means duplicated source" to inline asm, which had not been
established.

**Loop rotation is per-function, not per-family.** `Func_8078af8` and its solved
sibling scan the same array the same way, but the sibling matches with a
top-tested `while` and this one requires `do/while`. The difference is what
follows the loop: the sibling has a post-loop statement giving gcc a join to
test into, whereas here every exit goes straight to the return and gcc will not
rotate a `while` on its own. **The latch increment order is source order too** —
the ROM wanted the counter advanced before the pointer, the reverse of the
sibling's.

**One `res` variable beats a direct `return` in every arm** (48 lines → 45):
four returns make four independent r0 pseudos. And the first arm must sit
*outside* the else-chain carrying the default — the tell is a **single** `mov`
where the value is needed on two paths, meaning the source assigned it once,
before the test that separates them. The middle arm's body is then *empty*.

**A high register holding a constant across calls is often not a named local.**
A constant a Thumb immediate cannot encode (`-1`, anything ≥ 256) is
materialised for the comparison anyway, and CSE reuses that register as an
argument. Screen bare literals before naming.

**Two `.s` header comments were wrong** and are corrected in the landed files:
one calls a counting function a slot finder and names the wrong flag bit; the
other describes a hue interpolation as a radius scaler. Both were written from
call traces rather than from the body.

## Corrections to recorded rules

**A gcc pool CAN mix a symbol with integer constants.** A recorded sweep claims
none in this tree do. `Func_8015fb8` is a counterexample on both sides — five
constants and a function address in one pool — and reference-order emission
reproduced the ROM's pool byte for byte.

**`mov r12, r3 / mov r3, rHI / push {r3} / mov r3, r12` is not a four-parameter
signature.** It is `thumb_function_prologue`'s fallback path, taken when the
function pushes no low register but does push a high one.

## State

- **1,856 functions remain in assembly.** 3,500 elevated `.c` files.
- `make clean && make -j8 && make compare` green; SHA1
  `5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
  linked ELF, `.gcc2_compiled.` present in each object.
- Five splits, each verified byte-neutral before any `.c` landed. No new
  flag-group rules — every match is at stock `-O2`. One file
  (`ovl_30_a_c_a_c_c_a_c.c`) **must never** be moved under an
  `-fno-strict-aliasing` rule; its header says so.
- Three functions were attempted by hand and **set aside rather than parked**
  (`Func_80c0700` at 25 of 45, `Func_80ba918` at 29 of 50, `Func_80da24c`
  structurally wrong at one spelling). One to three spellings is not enough
  measurement to justify a park, and a thin park misleads whoever picks it up.
