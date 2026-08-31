# Batch 159 — five elevated, and sibling transfer became a tool

Clean `make clean && make compare` green from scratch. All five addresses
checked against the linked ELFs.

| function | address | file |
|---|---|---|
| `Func_808bc9c` | `0x0808bc9c` | `src/rom_8a000/rom_8ba38_a_a_a_b.c` |
| `Func_80a195c` | `0x080a195c` | `src/rom_a1000/rom_a1814_a_a_b.c` |
| `OvlFunc_881_20080d4` | `0x020080d4` | `src/overlays/rom_77a7c8/ovl_30_a_a_a_c_a_b.c` |
| `OvlFunc_969_2008314` | `0x02008314` | `src/overlays/rom_7f6e64/ovl_314_a_a_a_b.c` |
| `OvlFunc_939_2009240` | `0x02009240` | `src/overlays/rom_7c460c/ovl_314_c_a_c_c_c_b.c` |

## The headline: `tools/shapesib.py`

Transferring a matching sibling's idiom has been the highest-yield move for
several batches, but finding the sibling was ad hoc. Two attempts to
systematise it:

**The wrong proxy.** Ranking candidates by how many matching `.c` files sit in
the same DIRECTORY measures how well-worked a bank is, not whether anything
there shares a shape. Both candidates picked that way stalled.

**The right one.** `shapesib.py` reduces each function to its mnemonic sequence
with operands dropped — registers and constants are exactly what differs
between siblings, while the C idiom survives in the mnemonics — and ranks by
difflib ratio against the 1701 already-matching functions.

Results: `OvlFunc_969_2008314` scored **0.958** and came in at 1 of 69 on the
FIRST screen, the one difference being a linker alias rather than C.
`OvlFunc_939_2009240` scored 0.769 and came in at 1 of 40.

**And its limit, measured.** `Func_80a195c`'s best sibling scored 0.731 and was
NOT a usable template — no loop, different shape. Somewhere near 0.7 the score
stops meaning "same idiom" and starts meaning "same size and call pattern".
Read the sibling before trusting the number.

## Two detector faults, both caught by contradiction

Building the tool, the corpus came out **empty** — impossible with hundreds of
functions elevated. Elevated functions live in gcc's OUTPUT format
(`.type NAME,function`); the parser only knew the hand-written
`.thumb_func_start`.

With that fixed, the sweep returned rows scoring **1.000** — functions matching
THEMSELVES, because the candidate scan was now reading generated assembly too.
Anything whose `.s` has a `.c` beside it is already elevated and is skipped.

Both were caught the same way: a number contradicting something already known.

## `parked()` was under-excluding by 84 entries

A park header names its function as `NAME -- 0xaddr` or `NAME -- asm/<path>.s`.
Only the first was matched, and **80 of the 182 park headers** use the second —
so any park whose FILENAME is not the address was re-offered.
`OvlFunc_common1_148` returned to the top of the candidate list the round after
it was parked. Third re-offer this batch; `parked()` goes from 556 to 640.

## The copy tell, both sides

**Reachable** when the two names take different values later: `Func_80a195c`
leaves the returned count in r0 through the pointer computation, then copies it
into the register the dead base occupied. Writing count and loop counter as one
variable gives no copy; declaring both and assigning `n = c` after the pointer
matched outright.

**Unreachable** when the copy is a pure duplicate — `Func_80a8b10`,
`Func_80e38b8`, `HeightTile_B`, `Func_801cee0` all copy a value that never
changes, and gcc coalesces the two names. `pure_copies()` in `pickable.py`
detects this; only 17 of 51 low-call candidates score zero.

## The operand-mode fix has a PLACEMENT rule

Routing a stored constant through an `int` local turns a pooled HImode literal
into a `mov`. Where the assignment goes is decided by the ROM:

* `OvlFunc_939_2009240` — the sibling's `v = 0x5b;` had to be the FIRST
  statement, because the ROM's `mov` precedes the whole body.
* `Func_808fe38` — `one = 1;` immediately before the store gives 9 differing;
  hoisted to the top it gives 49, because the constant stays live across the
  allocation, the DMA and three other stores.

And it is not free: on `OvlFunc_common1_148` every route through a local is
WORSE than leaving the literal pooled (1 → 4, 4, 4, 5), because the value must
be live across the store and that function has no register to spare.

## The pooled-small-constant tell needs "single-use"

Probed with the project's flags: three forms of `*p - 0x1f` all emit
`sub r0, r0, #31`, so gcc never pools a single-use small constant and the
symbol tell holds. But a REPEATED literal does pool — a matching file masks
with `0x1f` three times and its generated `.s` carries `.word 31`. Count the
uses before treating a pooled constant as a symbol.

## Read the diff line, not tryc's hint

On `OvlFunc_969_2008314` tryc reported "`__modsi3` vs `_modsi3_RAM` ... add
`__modsi3 = _modsi3_RAM;`". The actual instructions were `bl __umodsi3` against
`bl _umodsi3_RAM` — the UNSIGNED pair, already aliased in that overlay.
Following the hint would have added a wrong alias for a symbol the function
never calls.
