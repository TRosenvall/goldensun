# Batch 311 — eight parks, and the sweep that caught two park headers lying

Five agents, 25 targets. Gate green at `5c4695205413df7db52b9a184815a07783999971` at every commit.

**0 landed, 8 parks installed, 8 pre-existing park figures made checkable for the first time.**
State: **4,788 of 5,710 (83.9%)** — 37 available, 794 parked.

No landings. The batch's value is in two places instead: one function two encodings from
byte-identical, and a tooling change that turned an uncountable risk into a counted one and
immediately found two false figures in the tree.

## Parks installed

| function | figure | state |
|---|---|---|
| `OvlFunc_969_200cbec` | **2 of 1068** | size exact, count exact, **relocations identical** |
| `CalcStats` | 834 of 927 | size exact, count exact, prologue/epilogue byte-identical, 69 relocs in identical order |
| `OvlFunc_969_20088b4` | 724 of 962 | count exact, pool-word set exact |
| `Anim_Gaia` | 921 of 990 | size exact **by coincidence**, count saturated — pin-free |
| `FieldMain`, `MenuBar`, `Func_8023e70`, `Func_8024934` | — | triage, no candidate, no figure claimed |

`200cbec` is the headline. Both axes exact and the relocation sequence identical entry for entry,
so its **2 is a true distance, not a saturated one** — and the two encodings are *one adjacent pair
swapped*: sched2 hoists `ldr r1,=0xffee0000` one slot ahead of the `str r3,[r7,#8]` before it, both
loads resolving to the same pool word. Five devices were tried against the swap; three measured
worse (an `__asm__ __volatile__("")` barrier worst, at 410 differing) and two were byte-identical.
The next step is `-fsched-verbose=8` on that one block, **not more spellings** — the remaining lever
is sched2's dependent-count tie-break, which is the one term a spelling can still reach.

## The sweep: a rule only binds where something re-measures it

`tools/parkcheck.py` had one verdict, `UNCHECKABLE`, covering two unrelated situations: a **triage
park** with no candidate (legitimate — there is no figure to check) and a park that **claims an
"N of M" figure with no way to re-measure it** (dangerous — its number can never be caught lying).
Thirteen parks once hid in that verdict. Splitting it into `NOFIGURE` and `UNCHECKABLE` made the
dangerous class countable for the first time. A static sweep of all 823 park headers:

    parks with a runnable recipe          354
    triage / no figure claimed            461
    no header comment at all                0
    FIGURE CLAIMED, NO RECIPE               8

Recipes added to all eight, and run. **Six confirmed exactly** — including `ovl_77dd1c/2009154.c` at
**2 of 160**, the closest park in the tree, which is reassuring precisely because it was the single
figure most worth doubting. **Two were lying, and both are saturated**, so their real distance is
unknown:

| park | claimed | production | why |
|---|---|---|---|
| `ovl_7f6e64/200a200.c` | 90 of 138 | **148** | the 90 is a `-fno-gcse` figure, and **no `-fno-gcse` Makefile row exists for that object** |
| `ovl_7aa430/2009a3c.c` | 90 of 177 | **181** | no flag cited, no Makefile row — no flag explains the gap |

`200a200` is the per-flag claim-line defect — a rule already written down, and one I have rejected in
agent reports seven times — sitting in a park's **first line** for several batches. It survived for
one reason: nothing could run it. That is the general lesson and the argument for making the recipe
requirement mechanical rather than editorial.

`2009a3c` is worse in kind. Its blocker diagnosis ("one allocno too many, plus an unsolved
pooled-zero construct") **rests on the withdrawn figure**, so the diagnosis is now unconfirmed — and
a saturated body cannot support an allocno-counting claim at all, because saturation means the
instruction stream has diverged far enough that per-encoding attribution is meaningless.

Three checker bugs were fixed to get there, all of them silent-unverifiability bugs **in the checker
itself**, which is the one place they must not live: a stray `// fakematch` on line 1 pushed the
header block off offset 0 and reported "no header comment"; a recipe wrapped in `sh -c '...'` left
the closing quote glued to the function name so objcmp searched for `NAME'`; and the verdict split
above. A tree-wide sweep confirmed the first affected only the two files from this batch.

## Two more rungs on the figures-that-lie ladder

**Rung 6 — a missing input can print the best possible verdict.** Brief E caught this in its own
harness and it is the worst of the set because it fails in the *safe-looking* direction. A generator
crashed, so the candidate `.c` never existed; objcmp emitted no verdict; and the harness's
empty-string fallback printed **`SIZE EXACT / ENCODINGS EXACT`**. Every probe script now fails loudly
on a missing file or a verdict-less run. This is the same failure as `tools/split_s.py` silently
ignoring `--dry-run`: the tool did nothing and said something reassuring.

**Rung 7 — an exact size can be a coincidence, from two opposite errors in different units.**
`Anim_Gaia` is size-exact at 2,188 bytes on both sides while its count saturates at 992 against 990.
objcmp prints no SIZE line, which reads like the strongest signal available. It is arithmetic:
`+2 instructions (+4 bytes)` against `−1 pool word (−4 bytes)`. Rungs 3 and 4 were cancellations
*within* one unit; this is a cancellation **across** units, text against pool, which no single figure
can separate because **size is the one number that adds them together**. Compare the instruction count
and the pool-word count separately, never their sum.

## Levers and bounds

- **A pooled constant gcc could never pool IS A SYMBOL.** `*thumb_movsi_insn` decides pool-versus-`mov`
  **on the value**, so anything loadable with one `mov #imm8` never reaches the pool by that route —
  therefore a reference that pools an eight-bit-movable word has a **relocation** there, not a literal.
  `z2 = (int)&_AREA_00;` is what bought `20088b4`'s exact count. `_CONST_0` is byte-identical to it, so
  the encodings cannot say *which* symbol it is; only the relocation sequence can. Same gate, different
  mode: the pooled-halfword defect is **not about zero** — `*(short *)x = 0xa` pools `.word 0x0000000a`
  where the reference emits `movs r3,#10`.
- **A pin forces the register, not the rebuild — and it fights naming.** Where a pinned value is also a
  named long-lived quantity, cse1 replaces the pin's own `q1 = K` with the commoned pseudo and the pin
  degrades to a copy. The two levers cannot stack at one site. One **blanket pin pass over every literal
  argument outside 0..255** took `200cbec` from −28/−14 to both axes exact in a single step; that is the
  documented step 1 for a **pure-rebuild** reference. The screen that tells you which kind you have costs
  two greps and comes before writing a line: the **pooled-constant multiset plus the reference's
  `mov rlo,rhigh` count**. Nothing reloaded more than ~3× with few reuse copies means pure rebuild; a
  value reloaded seven or eight times with ~30 reuse copies means mixed, and mixed will not go exact
  under either lever alone — which is exactly where `20088b4` sits.
- **`REG_N_SETS` again, in two more passes.** `int q = 0x80 << 7;` was inert at all four of its sites:
  one set earns a `REG_EQUIV` and reload rematerialises instead of allocating. The two-step form
  `q = 0x80; q <<= 7;` has two sets, carries no note, and gets its register. `expand_divmod` keeps its
  dividend copy on the same gate.
- **gcse/PRE invents allocnos, and an inner parenthesis is load-bearing.** `s + 0x28` occurs three times
  in `CalcStats`'s reference and is never commoned there. Spelling it as an explicit pointer lets
  `pre_insert_copies` hoist it function-wide and invent a third high register the ROM does not have.
  `q = s + (0x28 + k * 8)` rebuilds the *identical* pointer via loop.c strength reduction, which runs
  **after** gcse. A fourth member of the allocation family — *deny gcse the expression* — lowering your
  own count through a different pass than the other three.
- **`(signed char)*p` folds to `ldrsb`; `(signed char)p[0]` does not.** Largest single gain in `CalcStats`
  (strict 526 → 432), and the reference uses both forms 40 instructions apart. The two spellings are
  identical C.
- **`(1 << d) & mask` must be split across two statements**, or `fold` rewrites it to `(mask >> d) & 1`.
- **Three induction variables, not two, is what stops `check_dbra_loop` reversing a loop** — and reversal
  is directly observable, so this is cheap to confirm.
- **An extra use is the discriminator.** `&=` narrowed to a byte picks the cheaper constant (`mov #0xf6`
  over `mov #0xa`/`neg`) and a second int local blocks it; the sibling `& -4` block needs no help because
  its result has three further uses. Same shape as the donor-needs-a-use-of-its-own precondition.
- **Both `mul` operand orders are byte-identical** — so the "second operand lands in the destination" rule
  **cannot be read backwards** at a commutative site to recover which operand the source named first.
- **A third bound on initialise-at-declaration**, and it sharpens the rule rather than merely limiting it:
  `for (j = 0; …)` makes the declaration's store **dead**, so it is removed before `global.c` ever sees it.
  With the earlier bound, the rule is now *initialise-at-declaration lowers your own priority exactly when
  the initialiser is LIVE — and a loop that re-initialises the same variable kills it.*

## Three corrections to my own briefing

**The spill-slot narrowing.** `docs/band-800plus.md` §3 carried the unqualified heading
*"Declaration order — inert"*, measured on LOCAL-alloc-placed constants. It does **not** cover reload
spill slots, where declaration order **does** set the order. On `Anim_Gaia` a reorder moved the aligned
figure by two encodings — noise, and on the figures alone it would have been written off inert a second
time — while correcting a **seven-encoding slot misassignment**, after which the slot map matched the
reference entry for entry. Two consequences: for a spilling function the ranking instrument is the
**slot access-count table, not the aligned figure**; and the lever has **no purchase on a function with
no spills**, which is why putting it in all eight briefs of batch 310 on two functions' evidence was
wrong. Read the frame first, then the slot map. Fifth instance of an unqualified heading excluding
reachable work.

**The switch material was inert where I predicted it would pay.**

| target | insns | high-reg mentions | jump tables |
|---|---|---|---|
| `CalcStats` | 864 | **17** | **3** (27/8/6 entries) |
| `Func_8024934` | 946 | 84 | 0 |
| `FieldMain` | 965 | 102 | 0 |
| `MenuBar` | 1037 | 115 | 0 |
| `Func_8023e70` | 1201 | 96 | 0 |

All four menu/field targets have **zero** jump tables; `MenuBar` has zero unsigned branches of any kind
in 1,037 instructions. The only target with dispatches is the one with the **fewest** high-register
mentions, so on this population the two axes are **inversely** related. "Menu code is switch-heavy" is
what put the switch material in the brief, and it is false here. Second over-promotion of that lever
(after the "transfers to 36 functions" claim a tree-wide screen reduced to one), and the pattern is the
same both times: the *mechanism* was sound and the *population* was assumed. **Screen the population
before writing a lever into a brief.**

**Instruction count was the wrong difficulty axis.** My ordering of brief B's five was wrong about which
is hardest. `Anim_ScreenShatter` is 1,051 instructions — 115 **more** than `Anim_Gaia` — yet has frame
0x38, **no aggregates** and 8 spilled scalars, and should have been second. `Anim_Kirin` is shorter but
carries frame 0xb0, **6 aggregates with three `mov rX,sp`** and 22 scalars, and should have been last.
**Rank by the frame triad, not by length**: each aggregate is a quantity whose slot you must place, and
whose order is reversed relative to the scalars.

## A census that must not travel between functions

`Anim_Gaia`'s loops are **all `bne`** — every loop is `!=`, not `<`, worth 505 → 528.
`Anim_ScreenShatter`, in the same family, has **48 signed comparisons against 16 `bne`**. Importing
Gaia's rule there would corrupt 48 sites. Run the comparison census **per function** before applying
any loop-shape lever — the same discipline as the one-function int-carrier lever that measured 3.6
points negative elsewhere. **Mechanisms travel; per-function shapes do not.**

Related discipline: `-fno-rerun-cse-after-loop` **was not tested** on either of brief E's two and must
not be cited for them — **neither function has a loop.** It remains unsettled on `OvlFunc_882_200b1ac`,
which does have one and is the place left to settle it.

## Tree claims corrected

- **`FieldMain` takes no arguments.** The `GameStart` park declares `extern void FieldMain(int a)` and
  calls `FieldMain(k)`; r0 is overwritten with `0x1b` before any read, so the parameter is fiction. Its
  `.s` annotation also says "~600 instructions"; it is **965**.
- **`MenuBar`'s annotation reports "1157 lines" as a size** — that is the dump's line count, not an
  instruction count. It is **1,037** instructions, and its `bl .gcc2_compiled.` entries are disassembler
  noise, not calls.
- **Both files that matched `FieldMain`/`MenuBar` by name are about other functions** — the name-match
  trap the brief warned about, walked into anyway.
- **`Func_8023e70`, `Func_8024934` and `Func_8023178` are a three-member family** in one `.s`, all
  unattempted, with identical prologue and opening sequence. One three-way split serves all three, and
  solving any one supplies the other two's first ~40 instructions.
- **`Func_8024934`'s sp+0x58 is loaded once and never stored** — not a local. Three quarters of its
  372-byte frame is aggregate invisible to the first frame grep, and `add r2, sp, #0x174` points **one
  past the frame end**.

## Next

**`FieldMain`** is the recommended next target of the four triage parks: 3 spill slots in 965
instructions, the only one needing no split, and two *landed* callees at −0x38 and −0xac opening with
the same `galloc_ewram(0x1b, 0xccc)`.

`200cbec` at **2 of 1068** is the closest unresolved function in the tree after `Anim_Froth`, and its
remaining work is a single dump read rather than a search.

## Owed

- `OvlFunc_889_2008074`'s 233 pins are still not minimised to a fixpoint (owed since 310).
- `BufferString` needs `.func_end_emit_size Func_8015430, _FUNC_8015430_SIZE` in `asm/rom_15000/rom_15430.s:92`.
- `Anim_TitanBlade` needs 12 `.global` exports in one gated commit.
- Promote `DMA3_COPY_RW` to `include/dma.h` — two functions now satisfy the standing condition.
- `_FILE_bf` is missing from `include/file_table.h`.
- `2009a3c` needs re-measuring before its blocker diagnosis is trusted again.
- Owner decisions still open: the `.L4`/`.L5` rename in the shared common1 data file; `_CONST_1f`/`_CONST_200`.
