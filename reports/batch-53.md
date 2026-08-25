# Batch 53 — eight functions, and a new way to answer "what C makes this?"

Verified from a clean build: `make clean && make compare` → `goldensun.gba: OK`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`
and sits at exactly the address its name claims.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_807a2bc` | `0807a2bc` | main ROM | [rom_79460_c_c_c_c_a_c_c_c_a_c_b.c](../src/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_b.c) |
| `OvlFunc_914_2008bcc` | `02008bcc` | ovl_7a1ff0 | [ovl_30_c_c_c_c_c_c_a_b.c](../src/overlays/rom_7a1ff0/ovl_30_c_c_c_c_c_c_a_b.c) |
| `OvlFunc_915_2008d9c` | `02008d9c` | ovl_7a2bf0 | [ovl_30_c_c_c_c_c_b.c](../src/overlays/rom_7a2bf0/ovl_30_c_c_c_c_c_b.c) |
| `OvlFunc_916_2008f74` | `02008f74` | ovl_7a37f0 | [ovl_30_c_c_c_c_c_b.c](../src/overlays/rom_7a37f0/ovl_30_c_c_c_c_c_b.c) |
| `OvlFunc_917_2009878` | `02009878` | ovl_7a4370 | [ovl_30_c_c_c_c_c_c_b.c](../src/overlays/rom_7a4370/ovl_30_c_c_c_c_c_c_b.c) |
| `OvlFunc_957_2008d58` | `02008d58` | ovl_7e3e08 | [ovl_30_c_c_a_c_c_c_c_c_c_c_a_b.c](../src/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_c_a_b.c) |
| `OvlFunc_964_2008ec8` | `02008ec8` | ovl_7ed0a0 | [ovl_30_a_a_c_a_c_c_b.c](../src/overlays/rom_7ed0a0/ovl_30_a_a_c_a_c_c_b.c) |
| `OvlFunc_968_20088c8` | `020088c8` | ovl_7f2f14 | [ovl_30_a_c_a_a.c](../src/overlays/rom_7f2f14/ovl_30_a_c_a_a.c) |

## The vein changed, so the search changed

The constant-CSE worklist that carried batches 51 and 52 is **exhausted**, and
the basic-block-lever candidates start at 25 instructions and run to 58 — a real
step up from the 10–25 range those batches lived in.

So this batch searched for **twins** instead: instruction-identical bodies among
the unelevated set, where one solve pays several times. Six of the eight came
from two twin groups.

`tools/find_twins.py` ranks by dup-count × size, and its top entry is **13 copies
of a 139-instruction function**. This batch deliberately took a lower-ranked
group — 4 copies at 23 instructions — because **highest payoff and best first
attempt are not the same thing** when the body is unknown.

## `tools/find_solved_shape.py`

The four DMA functions cost a whole round's thinking on one question: *what C
produces `stmia r3!, {r0, r1, r2}`?*

The answer was **two files away**. A neighbouring elevated function in the same
overlay already used `DMA3_COPY` from `include/dma.h`. Once found, the C took one
correction and four functions fell out.

That is now a tool. The tree tracks the **generated** `.s` beside every elevated
`.c` (batch 47 settled that convention), so the corpus of solved codegen is on
disk and searchable:

```
python3 tools/find_solved_shape.py 'stmia'
python3 tools/find_solved_shape.py --seq 'mul' 'asr'
```

It searches only `.s` files carrying gcc's banner **and** having a sibling `.c` —
that pair is the proof the C is what produced the assembly. Hand-written corpus
is skipped deliberately: it shows what the ROM does, which you already have, not
what C reproduces it.

**Use it before inventing a construct.** A shape that looks novel has often been
solved in a file nobody thought to open.

### A "no hits" answer is also useful

`OvlFunc_964_2008ec8` and its twin park an address in **r12** across some
arithmetic. `find_solved_shape.py 'mov\tr12, r'` returns nothing — no elevated
`.c` in the tree produces that — so the shape is genuinely new and time spent
deriving it is not time wasted looking in the wrong place.

It then turned out **not to matter**. gcc emits the r12 spill from register
pressure with nothing in the source asking for it, and the obvious C matched on
the first attempt. Both files say so, to stop a future reader hunting for the
construct that "causes" r12.

## `!= 0` is the wrong spelling for the non-zero idiom

`Func_807a2bc` ends with gcc's branchless "is this non-zero" sequence:

```
and r3, r2 / neg r0, r3 / orr r0, r3 / lsr r0, #0x1f
```

| Source | Result |
|---|---|
| `return (x & (1 << bit)) != 0;` | 15 lines — rewritten to `(x >> bit) & 1` |
| `return (x & (1 << bit)) ? 1 : 0;` | 15 lines — same rewrite |
| `v = x & (1 << bit); if (v) return 1; return 0;` | **18 lines, exact** |

Both expression forms let gcc see the result is a single bit and swap the mask
for a shift. Only the **statement-level `if`/`return`** produces the ROM's
sequence.

Naming the mask and the value in their own statements *without* the `if` is 19
lines — worse than either. **It is the branch that stops the rewrite, not the
naming**, which is the opposite of how every naming lever in
`docs/elevation.md` works.

## Smaller findings

- **`OvlFunc_957_2008d58`**: both non-obvious readings came off register use, not
  the body. `__TestCollision` takes the actor *as well as* the triple (`r0` is
  never rewritten before the `bl`), and `iwram_3001f30` is loaded above the call
  because the ROM keeps it in a pushed callee-saved register — the batch-49 tell.
- **The DMA four**: the iwram block pointer must be a named local. Left inline at
  both call sites, gcc reloads it for the second transfer *and* builds the second
  destination with a destructive `add r1, r2` where the ROM uses the
  three-operand form. Both defects come from one omission; one local fixes both.
- **A twin recorded rather than re-screened**: `OvlFunc_932_200aa10` is
  instruction-identical to `OvlFunc_931_2008c0c`, parked in batch 50 on mask
  narrowing. That park now states it covers two functions.
- **No new inline assembly** was added. `DMA3_COPY` is inherited; this batch only
  adds users of it.
