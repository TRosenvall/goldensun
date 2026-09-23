# Batch 282 — seven functions, four of them finished from parks, and a false-green build

Gated on a clean `make clean && make -j8 && make compare`, green at the target SHA1
`5c4695205413df7db52b9a184815a07783999971`. All seven verified at their exact ROM addresses.
`git status` clean.

**Published late.** This report was written during batch 283, after an agent pointed out it
did not exist. The findings were in the park headers and commit messages throughout; only
the report was missing.

| | |
|---|---|
| elevated | **7** |
| parked | **11** (7 new, 4 updated) |
| `.sym` entries added | **2** (`_MSG_820`, `_FILE_e6`) |
| whole-file conversions | **5 files / 6 functions** |
| agents | 5, at 4 targets each = 20 |

| agent | assignment | landed |
|---|---|---|
| B | finish the 8–13 encoding parks | **3** of 4 |
| C | overlays 956 / 927 / 955 | **3** of 4 |
| A | finish the ≤5 encoding parks | **1** of 4 |
| D | overlays 932 / 954 / 935 / 951 | 0 of 4 |
| E | `rom_c9000` animations | 0 of 4 |

**Four of the seven landings came out of parks written in batch 281** — which is the case for
writing parks properly. `Func_80bf678` had 40 candidates already on file; an agent added 20
lines and closed it.

## The round's most useful finding is a mechanism behind something the tree had been doing blind

**`do { } while (0)` plants two *total* scheduling barriers.** Confirmed at
`haifa-sched.c:3714`: a `NOTE_INSN_LOOP_BEG`/`LOOP_END` sets `schedule_barrier_found`, which
makes the next real insn depend on **every prior use, set and clobber of every register** and
calls `flush_pending_lists`. Plain `{ … }` emits only block notes, which the same file
deliberately excludes.

That cuts both ways, and both halves matter:

- It **explains** the deliberate `do { } while (0);` barrier device that 52 landed files use
  and which had been recorded as an observation rather than a mechanism.
- **Any macro body wrapped in the conventional `do { } while (0)` plants two unintended
  barriers**, which is exactly what stopped `OvlFunc_918_2009004` closing — a constant build
  had been combined into a `CALL_VIA_R4` macro's own argument copy and could never be hoisted
  out.

**And the corollary is the reusable part: a residue that looks like a scheduling difference can
be the ABSENCE of scheduling.** That park had recorded "a single four-instruction sched2
interleave"; with `-fno-schedule-insns2` the output was instruction-for-instruction identical
in the window. The one-line test is now standard.

## Three park diagnoses were wrong at the pass level

Each would have wasted the next attempt, and each is corrected in place:

- `OvlFunc_882_2009b18` said the tie fell to `INSN_LUID`. It is settled one step earlier at
  **dependent count** — and the deciding third dependent is *the next call's own `mov r0, fp`*,
  a write-after-write. So no spelling of the call site in question can reach it.
- `OvlFunc_921_2009fa4` blamed reload. The pass is **gcse (PRE)**, located to the insn: at
  `.03.cse` it still holds the `plus`, at `.07.gcse` its source has become the long-lived
  pseudo.
- `OvlFunc_887_2008578` blamed `local-alloc` and `REG_ALLOC_ORDER` for two instructions that
  are **reload-created**, so `QTY_CMP_PRI` never sees them. What decides them is
  `allocate_reload_reg`'s round-robin `last_spill_reg` — function-scoped state — which is why
  112 spellings were inert and why the identical construct 60 instructions later is right. **An
  identical instruction stream up to the divergence does not imply identical reload state**,
  because inherited reloads advance that counter without emitting anything.

## Levers

- **`local-alloc` orders quantities SHORTEST-LIVED FIRST — the inverse of `global_alloc`.** In
  a branchless function `global_alloc` never runs and `local-alloc.c:1480` is the whole policy:
  splitting one reused pointer into four made them all short and they all got the ROM's
  register, **229 of 247 → 10 in one edit**.
- **An alias set of 0 on a QImode store is an unconditional true dependence.**
  `true_dependence` returns 1 without consulting aliasing, and `lang_get_alias_set` returns 0
  for any char-precision reference — so `char *p; p[5] = 4;` blocks every later load from
  scheduling above it while a struct-member byte store does not. Found in `alias.c` after nine
  spellings sat flat. The **load** side landed in batch 283.
- **A `|=` on a struct byte picks its destination by the MODE OF THE CONSTANT CARRIER, not by
  source operand order.** Four spellings differing in operand order or `int`/`unsigned int`
  carrier are **bit-identical** (`iorsi3`'s `%` lets reload swap either way); an
  `unsigned char` carrier took 22 → 6. So "write the constant first" is measurably *not* the
  cure.
- **An `int` return type on a `void` callee is an "r0 last" lever, distinct from dropping the
  prototype**: full prototype 2, prototype dropped 3, `extern int` **0**, while fourteen other
  spellings at the same site floored at 2.
- **A shared hard register is evidence of disjoint live ranges, not of a shared variable.**
  Separating two values the ROM keeps in r8: 34 → 4. Batch 281's merge rule needed this
  converse — and **both directions landed one commit apart**, since `OvlFunc_945_200dd10`
  needed *merging* (403 of 411 → 10).
- **`check_dbra_loop` is the escape when a source statement cannot outrank a `loop.c`
  preheader insn.** `move_movables` and `strength_reduce` both `emit_insn_before(loop_start)`,
  so every preheader insn outranks every source one. Writing two loops as up-counting
  array-indexed loops made `check_dbra_loop` re-emit the counter init last and reverse them,
  **fixing three of four windows at once**.
- **A missing epilogue `mov r0, #0` is a return-type tell, and both polarities are attested** —
  `pop {r1}` means it returns a value, `pop {r0}` means void. On one function that reading also
  fixed an eleven-instruction block reordering, 53 → 30.

## Two `.sym` entries, and the contrast between them is the lesson

**`_MSG_820` was added because a park was wrong about its own reason for withholding.** That
park recorded the strongest in-function control on file — the **only shiftable id among
seventeen**, with all sixteen others reproducing as plain literals in the reference's exact pool
order — then declined it "because 8 encodings remain … it does not COMPLETE the function."
Those 8 closed. **"Does not complete its function" is a statement about the current candidate,
not about the symbol**, so any withheld entry whose park is later advanced should be
re-examined rather than left withheld by inertia.

**`_FILE_e6` was added with its namespace checked rather than assumed** — the callee passes its
argument straight to `__GetFile`, so this is the file space, and `_FILE_e7`/`_FILE_e8` are
adjacent values already admitted on the same evidence. `_FILE_e4` and `_FILE_e5` are the same
tell at the other two call sites and are withheld, neither completing its function.

## Corrections I owe the log

**I broke the tree and the gate did not catch it.** A guard assertion in a write script tripped
while the `rm` of a hand-written `.s` sat on the next shell line, so the delete ran and the
write did not. `make -j8 && make compare` then reported **`goldensun.gba: OK`** — because with
no source there is no rule to rebuild that object, and make silently linked the `.o` from the
previous build. **A tree missing the source of its own ROM passed both gates.** `git status`
cannot catch this; only reading the build log can. Restored, then landed properly by writing the
`.c` first and confirming from the log that `xgcc` actually compiled it.

**I also removed a redundant `.equ` shim I had left in a landed file in batch 281.** Both
definitions were live — `nm` reported the symbol twice, once from the linker script and once
from the object — which meant **the `area.sym` line was never actually exercised**. A
verification shim belongs in the scratch candidate only.

**And I misclassified a target as whole-file-convertible when it carries a `.section .data`.**
`datacheck.py` reports exactly that; I counted functions and inferred convertibility. Recovered
by rehoming the blob, after the rehome failed its first gate with an undefined reference and
needed the documented one-line `.global` export.

## State

**4,520 from C / 1,190 in asm** against batch 281's 4,513 / 1,197 — exactly **+7/−7**, the
**seventeenth consecutive reconciling batch**. `census.py` TOTAL 1190 agrees with `funcindex`.
**517 park files**, `fakematch.txt` **500 rows**.

**444 available** at the batch's close, of which 122 in 201–400, 78 in 401–800 and 61 above 800.

> A figure in this section was corrected after the fact. It first read 451, because four
> functions had been attempted with real measurements and never parked — `census.py` matches park
> files by name, so all four were counted as never-touched. The arithmetic is what caught it:
> 7 landed + 16 parked against 20 targets does not reconcile. **That check belongs before
> publishing.**

## Open

- **Three `.sym` entries reported and withheld**, none completing its function: `_MSG_cc3` (four
  in-function controls), `_FILE_e4`, `_FILE_e5`. Plus batch 281's three.
- **Three per-file flag rows**, each a claim about how the original was compiled:
  `-fcall-saved-r4` for `OvlFunc_970_2008f80`, `ALIAS_CFLAGS` for `OvlFunc_882_200c41c`, and
  `-fno-rerun-loop-opt` / `-fno-strength-reduce` now argued **against**.
- **Three long-standing entries:** `_MSG_2080`, `_SIZE_80f0024`, `_TBL_7a828`.
- **Two data decisions:** `OvlFunc_935_2008ca0`'s four-byte blob (rehome recommended) and
  `OvlFunc_971_20092e0`'s text/data split.
- The fakematch convention question, ~20 of 181 PIN-using files unbooked.
- `OvlFunc_948_2009308`'s one pin, from batch 272.
