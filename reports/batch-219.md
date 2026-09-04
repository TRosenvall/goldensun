# Batch 219

Six elevated, one parked. Two themes: **`objcmp.py` earned its place three
times over**, and three of the six were fixed by changing *where* something
sits — a basic block, a control-flow polarity, a label counter — rather than by
respelling any expression.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `Func_8099678` | `0x08099678` | [rom_97b54_a_c_c_c_b.c](src/rom_8a000/rom_97b54_a_c_c_c_b.c) |
| 2 | `Func_801b5c0` | `0x0801b5c0` | [rom_1aeec_…_c_b.c](src/rom_15000/rom_1aeec_a_a_c_a_c_c_b.c) |
| 3 | `OvlFunc_936_200b864` | `0x0200b864` | [ovl_30_c_c_c_c_a_c_c.c](src/overlays/rom_7c097c/ovl_30_c_c_c_c_a_c_c.c) |
| 4 | `Func_808c30c` | `0x0808c30c` | [rom_8ba38_…_c_b.c](src/rom_8a000/rom_8ba38_a_a_a_c_a_c_b.c) |
| 5 | `OvlFunc_948_20097ac` | `0x020097ac` | [ovl_30_…_c_b.c](src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_c_c_c_c_c_b.c) |
| 6 | `OvlFunc_common1_88c` | `0x0200a124` | [common1_…_a_b.c](src/overlays/common/common1_a_a_a_a_c_c_a_a_a_b.c) |

Parked: `OvlFunc_879_2008454` (86 of 83, an array address cached across a call).

Gated on a clean `make clean && make compare`, addresses verified against the
linked ELF with `tools/checkaddr.py`.

**A caveat on row 6.** `OvlFunc_common1_88c` lives in a SHARED common object
that is linked into three overlays at three different load addresses —
`0x0200a124`, `0x0200a3bc` and `0x0200ae54`. The table lists one of them
because the row format takes one. It is not a single-address function, and
`checkaddr` verifies whichever it is given rather than proving the other two.

## `objcmp.py` WAS LOAD-BEARING THREE TIMES

Added last batch on the strength of one function; this batch it caught three
distinct things, two of which `tryc` reports as clean:

  * **`Func_8099678`** — the obvious spelling of its final store SCREENS CLEAN
    and is wrong. gcc hoists the stored constant to the front of the literal
    pool, ahead of a symbol whose instruction comes earlier, so every relocation
    shifts by four and twelve encodings differ. It would have gone into the
    build and turned it red.
  * **`OvlFunc_common1_88c`** — a label-number collision (below) that is
    byte-identical in the instruction stream and lives entirely in the
    relocation table.
  * **`Func_801b5c0`** — `tryc` said OK, but its reference holds SEVEN
    functions, so the size check was SKIPPED and that "OK" covered only the
    instruction stream.

The last is the cheapest lesson: **when `tryc` prints "size check skipped", its
verdict is weaker than it looks.** Both multi-function references and
inside-function pools trigger it.

## INVERT THE TEST TO KEEP CSE OFF A MULTIPLY OPERAND

`Func_808c30c` reads unmistakably as `d = amount; if (scaled) { ... }` — the
fallback copy sits above the compare, exactly gcc's shape for a hoisted
single-instruction else arm. Written that way it is **15 differing**, and the
visible fault is three instructions: the multiply reads the COPY where the ROM
reads the parameter.

With `d = amount` dominating the multiply, CSE merges the two into one quantity
and rewrites the operand. That is not cosmetic — it drops the parameter's
in-loop reference count from three to two, which demotes it below the walking
pointer and the counter in the allocator's priority order and **rotates three
registers through the entire function**. Fifteen differences from one
substitution.

Writing the fallback as the THEN arm keeps that assignment off the multiply's
dominator path. gcc still hoists the one-instruction arm above the compare, so
the EMITTED shape is byte-identical either way — but the RTL operand stays the
parameter and the allocation falls into place.

Two things to carry forward:

  * **When a destructive op sources a COPY where the ROM sources the ORIGINAL,
    that is CSE quantity merging, and the fix is CFG-SHAPED, not
    expression-shaped.**
  * **When a hoisted single-instruction arm is visible in the ROM, screen BOTH
    polarities.** The emitted code does not tell you which one the source used.

A seven-flag sweep was run and rejected — this residue looks exactly like an
`-fno-gcse` park and is not one. No flag reaches what the inversion reaches.

## A LABEL-NUMBER COLLISION, AND THE FIX NOT TAKEN

`OvlFunc_common1_88c` references a real cross-object data symbol `.L10` — and
gcc's own first branch label in that function is also numbered `.L10`, so the
pool's `.word .L10` captures the LOCAL label. Instructions byte-identical;
defect entirely in the relocation table.

**Two fixes work and only one was shipped.** A statement that merely ALLOCATES
LABEL NUMBERS above the first branch — `do { } while (0);`, or an empty `else`
— shifts gcc's numbering off the collision and passes, with byte-identical
output. That is a real and previously unrecorded trick, and it is the wrong
thing to ship: the construct has no meaning to a reader, its effect is
invisible, and any unrelated edit that adds or removes a branch silently breaks
it. The linker alias is this project's documented remedy, an immediate sibling
already uses it for five labels, and absolute assignments emit no bytes. So
`_TBL_L10 = .L10;` went into the three overlay scripts that list the object.

Recording the trick anyway, because it is the only known way to move gcc's
label numbering, and a future collision may sit somewhere an alias cannot reach.

## THREE SMALLER LEVERS

  * **One-variable-two-ranges is real and NOT free.** On `OvlFunc_936_200b864`
    the ROM builds `-13` by subtracting from the zero it has just stored, and
    reproducing that took 46 differing to 14. Reusing the SAME variable again
    for a later store overshoots — 31 differing and a line SHORT. A second,
    separate local is exact. Each reuse has to be one the ROM actually made.
  * **Operand order can be a TYPE question.** `OvlFunc_common1_88c`'s upload
    address gives the three-operand `add` under every pointer-typed spelling,
    because the frontend canonicalises the pointer operand to the front. Writing
    the arithmetic in `int` and casting back produces the ROM's two-operand
    form. Reordering the expression cannot reach it; changing the type can.
  * **A constant may need a DIFFERENT BASIC BLOCK.** The same function's size
    constant schedules one slot early as a literal at the call; named and
    assigned in a DOMINATING block it rematerialises in the ROM's phase. In the
    SAME block as the call it is catastrophic — 49 of 59.

## A FLAG RULE, AND WHAT JUSTIFIES ONE

`OvlFunc_948_20097ac` needs `-fno-rerun-cse-after-loop`. The ROM rebuilds a flag
id at both its test and its set; at `-O2` the second CSE pass hoists it into a
callee-saved register and keeps it live across the intervening call, costing a
fifth callee-saved register and pushing a coordinate out of `r8` into `r10`.

What justifies the rule rather than a spelling: **three source forms were
measured and all three produce IDENTICAL output**, because the constant is
folded before CSE ever runs. The rematerialisation is a pass-level property, not
a source-level one. That is the bar — not "I could not find a spelling", but
"the spellings provably cannot differ".

The same function also shows BOTH constant spellings side by side: two offsets
written `K << S` because the ROM builds them with `mov`/`lsl`, and a third
written as a plain literal because no eight-bit `mov` plus shift can build it
and the ROM pools it. Measure the operand; do not guess from magnitude.

## THE PARK

`OvlFunc_879_2008454` sits at 86 lines against 83. Its blocker is an ARRAY
ADDRESS cached in a callee-saved register across a call where the ROM reloads
it — that one cached value is the third pushed register and displaces every
later one. Three spellings tie at 66 differing and all three share the
assumption that the array is referenced twice in a form gcc can common; the
park names defeating the CSE itself as the untried direction. The
rematerialisation lever is on file for CONSTANTS reused across a call, and
whether it reaches an array address is exactly what is untested.

It also measured a needed `file_table.sym` member (`_FILE_1a`): the literal
emits an inline `mov`, the symbol pools it as the ROM does. The entry was
deliberately NOT added, because an unreferenced symbol is clutter — it goes in
with the body.

## PROCESS

`git add` aborts the ENTIRE command on one non-existent pathspec, and I hit that
three times this batch by predicting a split's suffix instead of listing it.
Once a `||` fallback then staged a pile of compiler-generated `.s` files, which
had to be reset. **List the split products before constructing the paths.**

Six functions were screened concurrently by subagents while the build tree and
git stayed single-writer; every result was re-verified with `objcmp` here before
being written in. One agent proposed the label-shifting hack above and
correctly flagged the alias as the durable alternative, which is the choice this
batch took.
