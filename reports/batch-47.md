# Batch 47 — six functions, and a convention settled by the repository

Verified from a clean build: `make clean && make compare` → `goldensun.gba: OK`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`
and sits at exactly the address its name claims.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_8021c34` | `08021c34` | main ROM | [rom_20198_c_c_c_c_a_b.c](../src/rom_15000/rom_20198_c_c_c_c_a_b.c) |
| `OvlFunc_895_20088f4` | `020088f4` | ovl_78dee8 | [ovl_30_c_c_c_a_b.c](../src/overlays/rom_78dee8/ovl_30_c_c_c_a_b.c) |
| `OvlFunc_938_200806c` | `0200806c` | ovl_7c37ac | [ovl_30_c_c_a_a.c](../src/overlays/rom_7c37ac/ovl_30_c_c_a_a.c) |
| `OvlFunc_939_20086e4` | `020086e4` | ovl_7c460c | [ovl_314_a_c_a_a_c_c.c](../src/overlays/rom_7c460c/ovl_314_a_c_a_a_c_c.c) |
| `OvlFunc_953_2009a14` | `02009a14` | ovl_7d95dc | [ovl_30_c_c_c_a_a_c_b.c](../src/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_c_b.c) |
| `OvlFunc_964_20092e0` | `020092e0` | ovl_7ed0a0 | [ovl_30_a_c_a_a_a.c](../src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_a.c) |

## The `pop {r1}` tell decided all six

Batch 46 documented it: an epilogue that pops the return address into **r1
instead of r0** means r0 is live across the epilogue, which means the function
**returns something**. gcc-2.96 reaches for r0 whenever it can.

Every function in this batch ends `pop {r1}`. Two area dispatchers `return 0`;
three script selectors return a pointer; `Func_8021c34` returns the UI box it
just created — written `void`, the box would be created and dropped.

Six functions in two rounds settled by two instructions at the end, one round
after the tell was written down. It is not recoverable from the body: a tail
call whose result is discarded and one whose result is returned compile
identically.

## Area dispatch and script selection

`OvlFunc_895_20088f4` and `OvlFunc_953_2009a14` are the **two-arm** form of the
family whose three-arm member `OvlFunc_920_200846c` went in with batch 45. Same
`gState+0x1c0` read, `off = 0` as a *variable* included — Thumb `ldrsh` has no
immediate-offset form, so the zero has to live in a register.

The other three select a **script pointer** by area and return it. They differ in
where the join sits, and the ROM says which:

| | Behaviour | Join |
|---|---|---|
| `OvlFunc_938_200806c` | only the matching arm calls `__Func_808b868`; the fallback returns its script untouched | after the call |
| `OvlFunc_964_20092e0` | both arms fall through to the call — the `if` chooses the pointer, the call happens once | before the `bl` |
| `OvlFunc_939_20086e4` | area `0x9f` calls `__GetFlag` **and discards the result**, returning its script unconditionally; area `0x68` branches on the same flag and falls through to the shared fallback | mixed |

That last asymmetry is worth stating. Writing both arms as conditionals is the
tidier reading and is wrong — **the third time in four batches that the fix was
to make two arms of one construct less symmetric than they look.**

Every compared constant is pooled where `cmp #imm` would do, so every one is an
area id. `_AREA_68` and `_AREA_9f` were added to `area.sym` **by value**, as that
file's own comment prescribes for ids with no semantic name yet; an absolute
symbol definition emits no bytes.

`Func_8021c34`'s string is a `.L` label defined in a **sibling** `.s` that already
exports it. No new `.global` was needed — the first time that has been true. The
count in `HANDOFF.md` stays at nine.

## How much of this vein is left

54 functions still in `asm/` read the area halfword. Only a handful are pure
selectors; the rest are large functions that merely **consult** the area on their
way to doing something else, running from 60 to 925 instructions. **This vein is
close to worked out** and the remainder will not fall to a template — worth
knowing before planning another round around it.

## A convention settled by the repository, not by intuition

Converting a function deletes its hand-written `.s`. The next build writes a
**generated** `.s` to the same path from the new `.c`, so git reports the file as
*modified* rather than deleted.

That reads like compiler output leaking into the corpus, and it is not.

`.gitignore` covers `.o`, `.d`, `.elf`, `.map` — and deliberately **not** `.s`.
The tree carries **2,535 tracked `.s` files bearing gcc's own banner**, every one
clean, every one arriving with the upstream base commit *"Adopt Coaltergeist's
tree as the build base"*. **Tracking the generated assembly beside the C is the
convention**, and 391 of 391 elevations in this tree follow it.

I had already acted on the wrong rule before checking: three `.s` files were
`git rm --cached`ed out of a commit as though they were stray build output, and
a tool was written to hunt for more of them. The tool found 2,535 — which is what
made the premise checkable at all. All three are back and the tally is 391 of
391.

`tools/asmfacts.py --asm-pairs` now checks the **real** failure mode: a `.c`
committed *without* its `.s`. That one is invisible to the build, because the
`.s` regenerates either way, and it leaves the pair inconsistent with every other
file in the tree. Recorded in `docs/elevation.md` under working discipline, with
the evidence, so the question stays settled by the repository.

## Parked this batch

Two, both predicted by the catalogue before any C was written, and both parked
with *why nothing is worth trying* rather than a list of formulations:

- **`Func_801d94c`** — a pool load ordered before a pointer dereference, plus an
  add-and-load where gcc emits an indexed load. Naming the intermediate pointer
  is the documented lever for the second, and gcc folds it straight back because
  the pointer has one use.
- **`OvlFunc_949_200828c`** — passes `-1` twice, and the ROM builds each with its
  own `mov`/`neg`. Two separate locals give `neg r2, r2 / mov r1, r2`: gcc
  materialises the constant once and **copies** it, the "separate variables do
  not defeat a copy" result confirmed again.

Every defect in both is straight-line, which is the unreachable side of the
basic-block lever.

**A negative result worth recording:** all 103 parks were swept for the epilogue
tell — a park differing *only* in `pop {rN}` would have unparked for free. **None
do.** The tell is real, but it was not silently costing anything.
