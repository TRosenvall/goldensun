# Batch 66 — working the symbol pool, and two levers with measured boundaries

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.
3062 sources checked, every elevated `.c` has a tracked `.s`; 0 orphans.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_921_200816c` | `0200816c` | ovl_7a7298 | [ovl_30_c_c_a_a.c](../src/overlays/rom_7a7298/ovl_30_c_c_a_a.c) |
| `OvlFunc_933_200841c` | `0200841c` | ovl_7bc690 | [ovl_314_c_c_a_a.c](../src/overlays/rom_7bc690/ovl_314_c_c_a_a.c) |
| `OvlFunc_942_20080a0` | `020080a0` | ovl_7c6bac | [ovl_30_c_c_a_a_a.c](../src/overlays/rom_7c6bac/ovl_30_c_c_a_a_a.c) |
| `OvlFunc_942_200819c` | `0200819c` | ovl_7c6bac | [ovl_30_c_c_a_a_c.c](../src/overlays/rom_7c6bac/ovl_30_c_c_a_a_c.c) |
| `OvlFunc_952_200c034` | `0200c034` | ovl_7d768c | [ovl_30_c_c_a.c](../src/overlays/rom_7d768c/ovl_30_c_c_a.c) |

All five are pool-tell functions elevated by using `_AREA_` symbols that already
existed in `area.sym`. Four matched on the first screen.

## A refinement to standing advice, with both boundaries tested

`docs/elevation.md` has long said to **name** the zero offset that Thumb `ldrsh`
requires, since the ISA has no immediate form. That is right **only when it is
the only zero in the function.**

When a zero is also *stored* later, gcc merges the two — it keeps the offset
alive, reuses it for the `strb`, drops the ROM's separate `mov r3, #0`, and
pulls a callee-saved register into the prologue to hold it. Nine instructions
differed on `OvlFunc_921_200816c` for that single merge.

Inline the cast instead:

```c
v = *(short *)(g + (unsigned int)0);
```

Still forces the register-offset form the ISA requires; gives gcc no named value
to reuse. **9 of 46 → exact.**

Two boundaries, both measured rather than assumed:

- **Writing the store as a literal does not help.** `*t = 0;` instead of
  `z = 0; *t = z;` is byte-identical — the merge is on the *offset* side.
- **It does not fix a folded address.** `OvlFunc_922_2009a34` looks identical in
  the diff (a register-offset load where the ROM computes an address first), but
  there gcc is folding `add r3, r5, r1` into the load, not reusing a zero. The
  same edit is byte-identical. **Two different defects with the same diff shape**
  — check which one you have before reaching for this.

## Candidate selection was narrowed twice, both times to avoid known floors

- **Signed range tests excluded.** The first two picks from the area list both
  range-test the second field (`cmp r3, #9 / blt`) — the signed lower-bound
  canonicalisation, a documented one-directional floor with a two-line minimum.
  Screening them would have rediscovered a known result. 23 of 48 area
  candidates survive the filter.
- **`OvlFunc_910_200809c` skipped**: it is in the area list *and* is the function
  whose source hangs gcc-2.96 (`docs/repro/`).

## Reading beats porting, again

`OvlFunc_942_20080a0` and `OvlFunc_942_200819c` are a **shape pair** — identical
opcode sequence, different constants. The second was ported by substituting
operands and matched on the first screen.

It nearly went wrong. A grep for the pool operands showed the sibling with one
fewer return, making the two look structurally different. **The grep pattern only
matched lowercase symbol names and missed `GFX_Thermometer`.** Reading the
function in full showed the structures are identical.

That is the second instance in two batches: `OvlFunc_939_2008388` last batch had
genuinely identical streams with the two returns **swapped**. Porting without
reading fails in both directions — it can invent a difference that is not there,
and it can miss one that is.

## New tool: `tools/split_asm.py`

The file-local `.L` label trap cost a third round, so the check is now
automated. Before anything is touched it reports:

1. **The basename collision** — a C file at `src/<p>/X.c` writes gcc's assembly
   to `asm/<p>/X.s`, overwriting the `.s` being split. The link error reads as a
   stale object.
2. **File-local `.L` labels** crossing the new object boundary, with the exact
   `.global` lines to add.
3. **Data sections** that must keep a linker line.

It refuses to apply a cut whose two halves reference each other's labels. It was
used for this batch's three-way split (two functions elevated around a retained
assembly function, verified still at `0x02008144`).

## State

`_AREA_` candidates remaining after this batch: **21** after the range filter,
46 unfiltered. The `_FILE_` and `_MSG_` pools are untouched and need their
consumers identified first — `sym_candidates.py` deliberately refuses to
classify them from the value, because 95 small values are defined in more than
one namespace.
