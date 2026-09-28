# Batch 290 — eight functions, and `.global` exports are systematically under-counted

Gated on a clean `make clean && make -j8 && make compare` → `goldensun.gba: OK` at
`5c4695205413df7db52b9a184815a07783999971`. All 8 landings verified at their exact ROM
addresses. `git status` clean. 8 commits.

The first local batch after the cloud session that produced batches 286–289, so it is also
the first test of that session's handoff (`docs/web-session-286-289.md`) as a working
document. It held up: its `int`-return lever closed three functions here, its
`objcmp --whole` gave every whole-file verdict, and its `repoint_parks.py` fixed two stale
recipes.

| | |
|---|---|
| elevated | **8** |
| parked | **19 new** |
| unopened | 3 |
| `.sym` entries added | **0** (three reported, all withheld) |
| whole-file conversions | **3 `.s` files / 4 functions** |
| agents | 6 briefs at 5 targets each = 30 |

**8 + 19 + 3 = 30**, reconciled before publishing.

| brief | bank | landed | parked | unopened |
|---|---|---|---|---|
| E | `rom_b0000` | **3** (one `.s` whole) | 2 | 0 |
| F | overlays, smallest in corpus | **2** (one whole, one rehome+split) | 3 | 0 |
| C | `rom_a1000` | **1** | 4 | 0 |
| D | `rom_9000` | **1** (whole) | 4 | 0 |
| A | `rom_b5000` | **1** | 3 | 1 |
| B | `rom_15000` | 0 | 3 | 2 |

Bank-pure briefs, on the handoff's measured finding that single-bank agents cost 23–43k
tokens per landing against 88–140k for mixed. `rom_15000` landed nothing again, consistent
with the handoff calling it one of the two hardest banks.

## The through-line: `.global` exports are under-counted, and it bit twice

Both halves came from agents working different banks, and together they make a rule.

**Agent B, before any landing:** `rom_1ca1c_c_c_c.s`'s `.rodata` defines `.L367dc` with **no
`.global`**, and it is referenced by `ldr r3, =.L367dc` from **two** functions. Batch 285's
park header for that file recorded **six** exported labels; the split needs **seven**. It
resolves today only because it is file-local.

**Agent F, at the gate:** its rehome **failed its first build** with three undefined
references. It had correctly exported the two `.data` labels *its own* function reads —
neither of which was among `datacheck.py`'s 13 reported exports — but three more are read by
the **other** function in the file, which stays in assembly. Five, not two.

> **The rule is not "export what your function reads." It is: EXPORT EVERY LABEL REFERENCED
> ACROSS THE NEW OBJECT BOUNDARY, IN EITHER DIRECTION.**
>
> `datacheck.py`'s EXPORTS line lists only labels that are **already** `.global`, so a label
> the file defines and uses internally is invisible to it. For any split, read the data
> section for label **definitions** and the text for **references**. The link is the backstop
> and a cheap one — it names them all at once, as `split_s.py` did with 11 labels on
> `Anim_Vine` in batch 285.

## The `int`-return lever reached indirect calls

The handoff's biggest lever — a `void` callee does not set r0 in RTL, so a later write to r0
depends on the argument copy and changes its dependent count in `rank_for_schedule` — closed
three more functions here. And agent A extended it:

> **A FUNCTION-POINTER TYPEDEF'S RETURN TYPE SCHEDULES THE ARGUMENT MOVES AT AN INDIRECT
> CALL, AND IT IS THE POINTER'S TYPE THAT MATTERS, NOT THE CALLEE'S.**
>
> ```
> typedef void (*CopyFn)(...)  ->  mov r0,r9 / movs r2,#0x24 / ldr r3 / mov r1,r6
> typedef int  (*CopyFn)(...)  ->  movs r2,#0x24 / ldr r3 / mov r1,r6 / mov r0,r9   (ROM)
> ```
>
> One token took `Func_80c1c54` from 4 differing to exact. **Changing only the typedef while
> leaving `extern void Func_8001af8` is also exact** — the extern's own return type is inert
> at a `bl _call_via_rN` site.

## Two parks upgraded from "unbeaten" to "unreachable by arithmetic"

This is the batch's methodological result. Both used the compiler's own source, which is in
the build container.

**`OvlFunc_916_2008098`** closed with *"nothing source-level"* plus a note that
`-fno-loop-optimize` does not exist in this cc1, so the diagnosis could not be confirmed from
the other side. It can be confirmed from `loop.c`: `move_movables` hoists when
`threshold * savings * m->lifetime >= insn_count` (`loop.c:1803`) with
`threshold = (has_call ? 1 : 2) * (1 + n_non_fixed_regs)` (`loop.c:651`). **This inner loop
contains no call**, so threshold takes the doubled form (~30) against `insn_count` ~22.
`30 >= 22` holds **unconditionally**, and lengthening a constant's lifetime only *enlarges*
the product. The only term that could change the outcome is `n_non_fixed_regs`, a target
property — which lines up with `HANDOFF.md`'s `REG_ALLOC_ORDER` hypothesis.

**`Func_80c1afc`** at 20 of 158, with size, count *and* relocations identical, so 20 is a
true distance. `global.c`'s `allocno_compare` is
`floor_log2(n_refs) * n_refs / live_length * 10000 * size`, loop-depth weighted, and the
function's own `.17.lreg` gives counter 7/28 = 5000, offset giv 9/24 = **11250**, table base
5/24 = 4166 — exactly the r5/r6/r7 order gcc produces. The ROM needs the counter *above* the
giv, i.e. **eleven weighted refs, i.e. two more uses emitting instructions the ROM does not
have.** The giv's 9 are forced, including the two `ldrh r0,[r6,r7]` loads the ROM itself
shows (an intervening call makes one cached load impossible).

**And a third, from the same family:** `OvlFunc_924_20097a8`'s giv is recomputed only when
`v->lifetime * threshold * benefit < insn_count` (`loop.c:4544`); here the product is ~64
against ~60, so it *always* reduces and no source shape moves it. Writing the inner loop as a
`goto` loop takes it out of `loop.c`'s reach entirely — no `NOTE_INSN_LOOP_BEG`.

## Levers

- **`i = 0;` WRITTEN EARLY CLOSED TWO FUNCTIONS** — 12 → 0 and 205 → 5. The mechanism is a
  conflict **inherited**, not created: a constant lives in hard r5 across three calls, so an
  early `i = 0` makes `i` inherit that conflict. In the second function it additionally makes
  `i` the eighth value wanting a callee-saved register, so reload spills a different value —
  which is why a later field store reads back as `add r0,sp,#4 / ldrb r0,[r0]`.
  **And the corollary: gcc still SCHEDULES the `mov r7,#0` down to where the ROM has it, so a
  late zero in the ROM is not evidence of a late source assignment.**
- **FIVE LEVERS TRANSFERRED WHOLESALE BETWEEN TWO COUSINS 2 KB APART**, taking the second from
  nothing written to 6 differing on its **first** candidate:
  - **The loop is `i = 0; if (n > i) { ofs = …; do { … } while (n > i); }`, not a `for`** — the
    ROM emits the entry guard *before* the walking offset's init, which puts that init in the
    preheader. 13 → 0.
  - **The condition is spelled COUNT-FIRST, `n > i`.** `i < n` is **83 differing** — the ROM's
    `cmp r9,r10 / bhi` puts the count in operand 0 and **gcc does not commute it.**
  - **`i` and `n` are `unsigned char`** — the `lsl #24 / lsr #24` pairs are the QImode
    zero-extension and also what makes both compares unsigned. `int` costs 89 and four
    instructions.
- **A COUNTED `for` WITH A `break` IS A DIFFERENT LOOP FROM A `while` WITH A COUNTER BAIL-OUT.**
  55 → 35. The `while` form does two wrong things at once: `loop.c` builds a **second walking
  pointer** for the `p + 0x20` address, and `duplicate_loop_exit_test` **peels** the test where
  the ROM enters with a bare `b`. **And the offset decides which form you need** — a sibling
  park gets the ROM's shape from a plain `while` because its field is at +4, which fits the
  `ldrb` immediate and is never a giv.
- **"ONE PRE-CHECK PLUS TWO BOTTOM TESTS, THE LOOP'S OWN CONDITION LAST" IS A GUARDED
  do-while**, and no `while` permutation reaches it. 38 → 13, and it fixed the instruction
  count. Fifth member of the loop-shape family.
- **DO NOT NAME THE SUB-STRUCT.** `st = &u->st;` rebases the whole function on `u+0x10`, kills
  `u`, and buys a fourth stack slot: 159 of 189. Writing `u->st.hp` and passing `&u->st` only
  to the copy is **exact** — cse reuses the `add r6,#0x10` it built for the call argument at
  the one access whose offset is exactly 0x10. **The ROM's MIXED BASES are the tell that no
  intermediate pointer existed.**
- **A NAMED LOCAL FOR A TABLE BASE CAN BE A PRESSURE LEVER, NOT AN ADDRESSING ONE.**
  `tbl = L367dc;` in a loop pre-header changes no instruction by itself — it creates a
  short-range high-priority allocno that evicts two other locals and produces the spill the
  ROM has. 152 → 127. Assigned at the top of the function instead: **fully inert.**
- **`field |= 0xffff` FOLDS TO `strh -1` FOR ANY 16-BIT FIELD** — fold distributes the
  truncation and both signed and unsigned spellings collapse it. The ROM's `ldrh / orr / strh`
  with a pooled `0xffff` survives **only** for an `unsigned short` field, and the same offset
  read with `ldrsh` elsewhere then needs an explicit `(short)`. **One offset read unsigned for
  the or and signed for the compares is a real shape**, worth 138 → 98 → 25.
- **A HImode CONSTANT STORE CANNOT BE CHAINED BY `reload_cse_move2add`** — reload writes it as
  `(set (subreg:SI (reg:HI)) (const_int))` and move2add skips a SUBREG destination. Routing the
  value through an `int` unlocks the ROM's whole `mov / lsl / sub / add / sub` chain.
- **BUT `struct HalfWord` CAN BE A FALSE POSITIVE**: one function's `ldr r6, .L10d4 @ 0` feeding
  three `strb` is exactly the documented signature, and applying the carrier **costs 28**.
  Three halfword stores in one function had **three different** ROM materialisations — one a
  genuine pool load where the bare literal is correct, two needing `int` carriers.
- **THE SAME `.rodata` BYTE TABLE IS `signed char` IN ONE FUNCTION AND READ THROUGH AN `int` IN
  ITS NEIGHBOUR**, 200 lines apart in one `.s`. The carrier question is per-function even for
  one table.
- **DECLARATION ORDER IS A SLOT LEVER AND ITS DIRECTION IS NOW MEASURED: LATER DECLARATION
  TAKES THE LOWER SLOT** (34 → 28). Consistent with its being inert where every local is
  register-resident.
- **A DEAD `-1` AFTER A CALL NEEDS A BARRIER BUT NOT A REGISTER PIN.** Every honest dead
  assignment is deleted; `{ i = -1; __asm__ volatile("" : : "r"(i)); }` is exact and **gcc picks
  r2 by itself.**
- **THE ORIGINAL'S OWN BUG IS LOAD-BEARING** — one function's third test reads the *spilled*
  slot rather than the call's result, and spelling it sensibly gives 236 of 253 **and the wrong
  size**. Second time this session a ROM defect had to be reproduced rather than corrected.
- **ARRAY INDEXING vs AN EXPLICIT OFFSET**: `L3a68[i]` gives both the ROM's increment order and
  its addressing, where an explicit `off += 4` gives only the order — and **all four
  pointer-arithmetic operand orders are inert**, because canonicalisation erases them.
- **BUT OPERAND ORDER IS A REAL LEVER ELSEWHERE, AND IT GOES BOTH WAYS IN ONE BANK**: one
  function needs `*(u16 *)(ofs + (int)state)`, its neighbour needs `state + ofs`. **The ROM's
  own `ldrh rD,[rB,rO]` operand order is the readout** — not a rule.
- **A RUNTIME `sub` OF TWO POOLED WORDS PROVES TWO SYMBOLS** (gcc folds two literals), and
  **conversely a runtime derive of one address from another proves ONE symbol plus an offset**,
  because symbol-minus-constant folds into a single pool word — so adding a second name there
  makes things worse.
- **gcse's cprop FOLDS A SHARED TWO-INSTRUCTION CONSTANT THE ROM KEEPS IN A REGISTER**, proven
  because the same file under `-fno-gcse` emits the ROM's `mov r5,#181 / lsl r5,#1`.
  `rtx_cost` of a `CONST_INT` is 0, so the constant always looks cheaper.

## Three symbols reported, all withheld

- **`_MSG_26fa` and `_MSG_2850`**, on the runtime-`sub` argument above — the same argument
  `message.sym` already accepted for `_MSG_ad0`, which is **in the same function feeding the
  same sink**. Withheld because that function is 169 of 187 *and four bytes long*.
- **`_MSG_ae0`**, with unusually strong control: it is `thumb_shiftable_const` so no spelling
  reaches a pool load, while the **five neighbouring ids** in the same function are unshiftable
  and all reproduce as plain literals. Withheld because the register blocker is independent.

**Agent C applied that bar unprompted.** The rule is holding without restatement.

## Measured negatives

- **`-fno-strength-reduce` leaves the `rom_10424` frame unchanged**, so strength reduction is
  **not** the pressure source there — do not chase the giv.
- **`-fno-schedule-insns2` makes `CreateActor` worse** (100 against 86), so despite its
  file-mate needing that flag, **no Makefile row is warranted.**
- **The `struct Spr *` alias-set cast is NOT load-bearing** at either of two sites — the plain
  `unsigned char *` store is also exact. That lever has been productive elsewhere and is not
  universal.
- **A ROM post-increment load is not evidence of strength reduction** — written as givs it was
  103 worse and two instructions short.
- **All 24 declaration orders inert at 113** on one function, and **all 60 permutations inert**
  on another, consistent with the slot rule.

## Corrections I owe the log

**`census.py` was still over-counting parked, and it was my batch-284 fix that was too loose.**
The handoff warned about it and cited the *pre-284* mechanism, which was already fixed — so the
explanation was stale while **the symptom was live by a different route**: my "subject named in
the first two lines" signal also captured **neighbours** a park mentions in its opening
sentence. Now takes only the **first** identifier, and `match_stem` is **anchored** rather than
length-guarded. 567 parked / 373 unattempted, TOTAL still equal to `funcindex`. **A stale
explanation for a live symptom deserves checking rather than dismissing** — the opposite of the
error I had been making.

**My first `parkcheck` fix broke four parks.** The recipe is now the *docker* invocation, which
wraps with a `\` after `objcmp.py` itself, and no earlier pattern could read it. Requiring the
candidate path to end in `.c` then rejected the literal `<this>` placeholder four parks use —
harmless, because **group 1 is captured and discarded** and check() runs against the park's own
path. Caught by comparing the sweep total to the expected one (100 where 104 was right), not by
reading the output.

**I pointed agent F at the 897-family BLKmode lever by overlay number, and that was wrong.**
The family is 882/883/884/887 plus a *different* 897 function. The agent checked rather than
assumed and said so. What closed its target instead was an alias-set fix: giving two byte
stores struct types rather than `unsigned char *` casts.

**I nearly repeated batch 283's failure in reverse.** Agent E said its landing form was
shim-free; `grep -c '\.equ'` returned 1. Reading the line showed **comment prose** explaining
the shim, and objcmp confirmed it independently, since the extra `R_ARM_ABS32` relocation only
appears *without* the shim. Same false positive as batch 281's bad guard assertion. **The grep
is the right reflex; reading the hit is the other half of it.**

## State

**4,688 from C / 1,022 in asm** against the cloud session's 4,680 / 1,030 — exactly **+8/−8**,
the **twenty-first consecutive reconciling batch**. `census.py` TOTAL 1022 agrees (76 hand-asm,
14 ARM, 590 parked). **590 park files**, `fakematch.txt` **531 rows**, `parkcheck` **104 OK /
0 MISMATCH**.

**342 unattempted** — 74 in 101–200, 117 in 201–400, 84 in 401–800, **65 above 800**, and
**two left below 100**. Reconciles exactly: 373 − 342 = 31 = 8 landed + 23 newly parked.

## Open

- **Two parks at 3 or fewer**: `Func_80b0fa4` at **3 of 138** with relocations identical, and
  `OvlFunc_921_2009fa4` still at 1 of 199. `Func_80c1afc` at **20 of 158** is a true distance
  but now proven unreachable by arithmetic.
- **`Func_8018850` / `Func_8018a50` are twins and should be one job** — they share a run walk, a
  29-entry jump table and a near-identical tail, and their `.s` is 2-of-2 with a clean 32-byte
  `.rodata` tail cut whose label is referenced by **neither** function.
- **`rom_1ca1c_c_c_c.s` needs SEVEN exports** including `.L367dc` — recorded in its park.
- Withheld: `_MSG_26fa`, `_MSG_2850`, `_MSG_ae0`, plus the standing list.
- **Backfill `Verify with:` recipes into the ~469 unchecked parks.** All 19 new parks carry one.
- The cloud session's own open items in `docs/web-session-286-289.md` §6–§7 are untouched:
  `Task_Debug_SpriteTest` (exact, needs a hand-ordered `.rodata` split), the
  `OvlFunc_883_200dd68` BLKmode application measured at 109 → 12, and three nested-function
  jobs.
