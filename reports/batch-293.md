# Batch 293 — seven functions, nine symbols, and every target opened

Gated on `make -j8 && make compare` → `goldensun.gba: OK` at
`5c4695205413df7db52b9a184815a07783999971`. All 7 landings verified at their exact ROM
addresses. 19 new parks and 1 park update, every one verified by `parkcheck`. 5 commits.
`git status` clean.

| | |
|---|---|
| elevated | **7** |
| parked | **19 new + 1 updated** |
| unopened | **0** — a first |
| `.sym` entries added | **9** (`_MSG_50`, `_MSG_51`, and the seven-id run `_MSG_cae`…`_MSG_cb4`) |
| whole-file conversions | 5 |
| splits | 3 |
| doc corrections | **7** |
| agents | 8 briefs, 27 targets |

**7 + 19 + 1 = 27**, reconciled per brief. Twenty-four consecutive batches reconcile exactly.

| brief | bank | band | targets | landed | parked |
|---|---|---|---|---|---|
| D | `rom_b0000` | 161–233 | 4 | **3** | 1 |
| C | `rom_15000` | 134–176 | 4 | **2** | 2 |
| A | `rom_8a000` | 200–229 | 5 | **1** | 4 |
| E | `rom_77000`/`rom_f2000` | 135–204 | 4 | **1** | 3 |
| B | `rom_a1000` | 149–229 | 4 | 0 | 4 |
| F | `rom_8a000` | 268–321 | 4 | 0 | 4 |
| G | `Func_80191cc` alone | 455 | 1 | 0 | 1 (update) |
| H | `AdvanceMsgText`, two halves | 628 | 1 | 0 | 1 |

**The band evidence held for a second batch.** All seven landings came from the six briefs
at 134–233 instructions. Brief F at 268–321 produced zero landings from four targets, the
same result that band gave last batch. Both deep dives produced large park improvements and
no landing, which is what they were funded for.

## Seven things the docs or my briefs had wrong

Three batches running, the corrections have been the most valuable output. This batch they
came from five different agents.

### 1. `n_refs` is loop-depth weighted, so every hand-counted reference total is wrong

`flow.c:4948`, and again at 4436, 5115 and 5556:

    REG_N_REFS (regno) += (optimize_size ? 1 : pbi->bb->loop_depth + 1);

At `-O2` a reference contributes `loop_depth + 1` — 1 at top level, **3 inside two loops**.
A count read from a `-da` dump is already weighted and correct; a count someone got by
reading the C is not, and any threshold computed from it is wrong by up to the nesting
factor.

**I built this batch's `Func_80191cc` deep-dive brief around a number that was wrong twice
over.** Last batch's park pinned the blocker to local-alloc's caller-save retry
(`4 * calls_crossed < n_refs`), counted three or four source references, and concluded it
needed five. With one call crossed the threshold is `n_refs > 4`, which at depth 2 is **two**
raw references — and the dump said the quantity already had **fifteen**. The inequality was
satisfied by a wide margin; adding references could never have helped. What actually decides
it is that the retry is a *fallback*, reached only when the first `find_free_reg` walk
returns −1, and that walk found r6 free and stopped before r4 was considered.

### 2. `--align` is blind to the literal pool, so it can read zero while bytes differ

`tryc.py:363` drops `.word` and `.align` from the aligned view deliberately — its own comment
says "drop the pool itself". So **objcmp stays mandatory; it is not merely the more
conservative number.** I pushed `--align` into all eight briefs this batch as the working
number, which was right, and incomplete.

The case that shows it: **the literal pool is ordered by the constant's MODE.** A constant
reaching a halfword store stays HImode and gcc places its pool word *first*, ahead of every
SImode word — rotating the pool and shifting every relocation after it. On `Func_808d5dc`
that was the last 15 of 18 differing encodings **while `--align` reported zero differing
instructions.** An `int` carrier fixes it. It runs both ways: elsewhere an `int k = 0x28` was
needed to keep a constant *out* of the pool.

### 3. "Derived by `add`" is not symbol evidence on its own

`use_related_value` fires for a `CONST` and never a bare `CONST_INT` — true **of cse**, and
easy to over-read. `reload_cse_move2add` runs after reload and chains bare literals happily;
one candidate derived `0x139` from `0x138` with `add r2, r2, #1` six times with no symbol
anywhere.

The discriminator is **which register** the derivation runs through, because `move2add` is per
hard register:

| the ROM shows | it proves |
|---|---|
| neighbours derived through ONE register, repeatedly | nothing |
| neighbours off a base in a callee-saved register across calls | a symbol |
| each offset materialised fresh in a DIFFERENT register | the ROM *defeating* move2add, which literals cannot do |

None of the nine symbols admitted this batch rests on the weak form.

### 4. `break` and `goto` out of a loop are not interchangeable — worth 228 → 8

`stmt.c:2257` `expand_end_loop` rolls the loop's first exit test to the bottom **only** for a
jump to the loop's own `end_label`, i.e. a `break`. A `goto` to a programmer's label leaves
`last_test_insn` NULL and the roll never fires. The tell is block *order* plus an entry jump
into the middle. Not previously in `elevation.md`; the nearest note is about `goto` *into* a
do/while, a different mechanism.

### 5. ARM's `PROMOTE_MODE` overrides a narrow local's declared signedness

`arm.h:597` gives HImode `UNSIGNEDP = TARGET_MMU_TRAPS != 0`, so `unsigned short saved = …`
emits **`ldrsh`** and an `int` is required. `elevation.md` records the mirror for *fields*
(`signed char` not `s8`); for **locals** it runs the other way, and declaring `unsigned` does
not save you.

### 6. Three smaller doc corrections, each measured

- The indirect-call section says calling through the expression directly "is a different
  shape" from assigning a local. On one function `fp = ev[2]; fp(id);` and `((T)ev[2])(id)`
  are **byte-identical**, reload after the intervening `bl` included.
- The register-allocation class is not all `global.c`. A `-dl` dump — after local-alloc,
  *before* global-alloc — already read `;; Register 51 in 6.`, so HANDOFF's
  `REG_ALLOC_ORDER` hypothesis cannot explain that function.
- The `.call_via` section over-prescribes a bespoke helper per (callee, register) pair. Two
  `.call_via r10` sites reproduced from shipped `math.h` with no helper and no pin.

### 7. The mul lever's *procedure* does not predict either

Last batch established it is a procedure, not a direction. This batch, at both sites in one
brief, the procedure pointed the wrong way — the ROM copies the quantity, so "put the other
operand on the right" says one thing and that spelling **lost**, 71 against 73. Only the
measurement predicts. And the procedure does not carry to other commutative operators at all:
`0x1f & v` and `v & 0x1f` are identical, and five spellings of a commutative `add` all
measured the same (what worked there was changing the expression's **type**, not its operand
order).

## Nine symbols, and the strongest `.sym` case in the tree

**`_MSG_cae` … `_MSG_cb4`** — a seven-id run worth **66 differing → 0**: they *are* the
function. Its in-function control is four-fold: the same function, same run, same sink also
uses `0xcab`, `0xcac`, `0xcb5`, `0xcb6`, and **each of those four reproduces byte-exact as a
plain literal**, measured by dropping each individually. Eleven ids side by side, where
exactly seven must be symbols and four provably must not.

Two mechanisms, both impossibility arguments, both verified by me in the compiler source:

- `0xcb0` is shiftable (`0xcb << 4`), so a literal emits `mov / lsl` where the ROM pools it.
- The other six hit an **if-conversion gate**: `ifcvt.c`'s `noce_try_store_flag_constants`
  requires `CONST_INT` in *both* arms, so a `SYMBOL_REF` bails it. `0xcae`/`0xcaf` have
  `diff == 1` and ARM's `STORE_FLAG_VALUE` is 1 (`arm.h:2516`), so gcc rewrites the arm as
  `0xcae + (flag != 0)` with a `neg/orr/lsr #31/add` idiom the ROM lacks. **The internal
  control is arithmetic**: the function's other three constant pairs have `diff == 2`, fall
  off that chain, and keep the ROM's branches.

One of my own checks *looked* like a contradiction and was not: grepping `toplev.c` for
"if-conversion" returns two hits, so `-fno-if-conversion` appeared to exist. Both are
**comments**, at exactly the unguarded call sites the agent cited, and no entry exists in the
`f_options` table. The agent was precise.

**`_MSG_50` / `_MSG_51`** — the pooling tell (gcc never pools a constant one `mov` can build),
namespace decided by consumer rather than value since both also exist as `_FILE_` and `_AREA_`
entries, and the function calls the sink `message.sym` already names four times.

## I stripped eight measurement-only shims before landing

One brief reported "zero shims in all four". Two of its files carried
`__asm__(".equ _MSG_cXX, 0x0cXX")` lines — seven in one, one in another — each added so the
candidate could compile standalone, each with a comment noting `message.sym` already carries
the row. They are legitimate in a park and **must never land**, since they would duplicate the
real definition.

The lesson for shim counting: **an `.equ` inside `__asm__` is a shim even though it declares
nothing and pins no register.** "Zero shims" needs the `.equ` class checked separately from
the `register` class. After stripping, objcmp reports the correct symbol signature — one pool
word carrying a relocation — and the compare is green.

## Two levers with boundaries worth as much as the levers

**The HImode-literal rule has no direction, and two functions in one bank want opposite
answers.** A thumb HImode store of a literal goes through the pool (`movhi` has no `CONST_INT`
alternative, so expand calls `force_const_mem`); an `int` local gives `mov rD,#K / strh`. One
function needs the int locals, its neighbour needs the bare literals. And **where the
assignment sits decides the register**: written as initialisers at the top of a case block the
locals are born before a call, cross it, and take callee-saved high registers — 142 → 75 by
assigning each immediately before its store, 75 → 60 by moving each shared zero after the
first store of its group.

**`emit_case_nodes` lays case bodies out in SOURCE order while the dispatch is sorted by
VALUE** — independently decisive on two functions: one tail needed `case 4:` before `case 2:`
(27 → 4 aligned, and it fixed the count), another's four two-body switches all needed the
higher-valued case written first (22 → 6). An `else if` chain cannot reach it, because it tests
and lays out in the same order.

## The single-drop blind spot, three more times

Last batch's finding that single drops cannot find a coupled pair recurred three times, and
once it had already cost a wrong conclusion:

- `Func_80191cc`: a pin is worth 171 → 121, and then a block-scoped hoist is worth 121 → 90 —
  **and the previous park had measured that same hoist as a regression and discarded it**,
  because it only pays *with* the pin. An earlier park's inert list was actively wrong.
- `Func_80912b8`: named HImode masks are each a regression alone (125, 118 against 115) but
  are the only route to the ROM's pool order, so the park says retry them *after* the spill
  choice is fixed.
- `LoadMapActors`: a separate test variable buys the ROM's sign extension (positional
  differences 265 → 203) but costs five instructions by pushing a label out of branch range.

## A shim can work and still be the wrong answer

On one function a `"+r"` barrier was a genuine 39 → 30 → 26 and exactly the right class for a
"right instructions, wrong registers" residue — but it was papering over a misreading of which
pseudo the ROM had. There is no second local: the **parameter is overwritten**, and the ROM's
`mov r9,r1` at entry with `mov r9,r3` after the subtract is one pseudo, not two allocnos
sharing r9. That reading reached **0 with no shim**.

## The two deep dives, and what they say about shape

**`Func_80191cc`: `--align` 171 → 57 of 500, with the length now matching.** The
reference-count route is provably dead (above), and the function is retired from the "needs
more refs" class. A pin is the only lever found; `allocno_compare` gives 2.879 against 2.627
and flipping it needs five more source uses or 36 more instructions, neither of which the
ROM's 500 instructions have room for.

**`AdvanceMsgText`: `--align` 23 of 724 on a first candidate**, for a 628-instruction function
— everything structural right, including a 31-entry jump table that materialised as a table on
the first try. Its agent's verdict on the two-halves shape is the useful part: phase 1 was
worth doing for the frame, table, block order and callee signatures, but **its absolute number
was nearly uninformative** — half the 387 was the absent case body and most of the rest was
register allocation that only resolves once the missing instructions restore the pressure. So
split the *transcription*, don't expect phase 1 to rank variants, and put any second large
block in phase 1 rather than stubbing it.

Two corrections to that function's earlier notes, both load-bearing: it **is a loop** (the slot
read as a plain variable is the do-while trip count), and the switch has **no `default:`** —
the range check is an explicit `if (op > 0x1e) goto glyph;` in long-branch form, because a
default body cannot be emitted after the break label. Writing a real `default:` there is wrong.
Its table also needed no filler case, which scopes last batch's `CASE_VALUES_THRESHOLD` note:
that was needed for a switch with too *few* nodes.

## State

`census` TOTAL and `funcindex` agree at **993**.

| | batch 292 | batch 293 |
|---|---|---|
| functions elevated | 4,710 of 5,710 (82.5%) | **4,717 of 5,710 (82.6%)** |
| still in `asm/` | 1,000 | 993 |
| — parked | 635 | 654 |
| — **unattempted and attemptable** | 275 | **249** |
| — hand-written assembly | 76 | 76 |
| — ARM | 14 | 14 |

Available fell by exactly 26 = 7 landed + 19 parked.

**The 100–260 band that produces landings is nearly exhausted: 25 functions left in 101–200,
against 80 in 201–400, 78 in 401–800 and 65 above 800.** 248 of the 249 available are over 100
instructions.

## Open

- **The productive band is almost gone.** Next batch should expect either a lower landing rate
  or a shift in method. The two parks worth reopening on arithmetic rather than allocation are
  `ScreenTransitionIn` (`--align` 60 at exact length and count, with 28 of the 60 in one
  identified cluster) and `Func_807a664` (31 with size, count **and** all ten relocations
  exact, blocked on `combine`'s `simplify_comparison` rewriting `(v << 16) != 0` back to
  `v != 0` — eleven variants measured).
- `Debug_PaletteEditor` needs its own brief with a `.08.loop` read, not a slot in a four-target
  batch.
- **Settle the `.call_via` question with objdump against the real ROM address.** `macros.inc:64`
  expands `.call_via r3` to an inline `mov r12, pc / bx r3` where gcc emits `bl _call_via_r3` —
  both 4 bytes, different encodings, and no generated `.s` in the tree contains `.call_via`. If
  they are genuinely distinct, every function-pointer call at such a site carries a hard
  one-instruction residue.
- Two stale reference comments found: `rom_8ba38_a_c_a_c.s` names its function
  `UpdateFollowerPositions` with a "~180-instruction body" when it is `Debug_PaletteEditor` at
  321, and `rom_8a5f8_a_c_a.s` claims `GameStart` takes no arguments when it takes r0. Noted in
  the parks rather than edited.
- Unchanged: the `make clean` churn; ~469 parks with no `Verify with:` recipe; 161 parks with
  stale prose `Source asm:` lines; and the standing owner decisions (`-ffixed-r7` for
  `CamelotLogo`, `OvlFunc_930_20091b0`'s three, `-fcall-saved-r4` for `OvlFunc_970_2008f80`,
  `ALIAS_CFLAGS` for `OvlFunc_882_200c41c`, a `DMA3_SET` `"r2"`-clobber variant, and the
  withheld symbols `_MSG_26fa`, `_MSG_2850`, `_MSG_ae0`, `_FILE_18`, `_FILE_19`,
  `_SIZE_80b5138`, `_LEN_2c4`).
