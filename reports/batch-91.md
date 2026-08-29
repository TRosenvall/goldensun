# Batch 91 — a fourth facing test, and what a repeated `cmp` says about a switch

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked overlay ELF with
`arm-none-eabi-nm`.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_934_2009770` | `02009770` | ovl_7bdeb0 | [ovl_169c_a_a_c_b.c](../src/overlays/rom_7bdeb0/ovl_169c_a_a_c_b.c) |
| `OvlFunc_950_200891c` | `0200891c` | ovl_7d5838 | [ovl_30_c_c_c_c_a.c](../src/overlays/rom_7d5838/ovl_30_c_c_c_c_a.c) |
| `OvlFunc_950_200866c` | `0200866c` | ovl_7d5838 | [ovl_30_c_c_a_c_a_a_c_b.c](../src/overlays/rom_7d5838/ovl_30_c_c_a_c_a_a_c_b.c) |
| `OvlFunc_962_2008100` | `02008100` | ovl_7ec19c | [ovl_30_c_a_c_a_b.c](../src/overlays/rom_7ec19c/ovl_30_c_a_c_a_b.c) |
| `OvlFunc_962_200816c` | `0200816c` | ovl_7ec19c | [ovl_30_c_a_c_a_b.c](../src/overlays/rom_7ec19c/ovl_30_c_a_c_a_b.c) |

Four of the five came from one reading, and it is the reusable result of the
batch.

## The quadrant facing test

`reports/batch-29.md` catalogued three spellings of the facing check, all of
them RANGE tests — `f - k <= n`. There is a fourth, and it is an equality on
masked bits: rotate the angle by half a quadrant, mask it to its top two bits,
and ask which quadrant it landed in.

```
	ldrh	r3, [r0, #6]
	mov	r2, #0x80 / lsl r2, #6		<- 0x2000
	add	r3, r2
	ldr	r2, =0xffffc000
	and	r3, r2
	lsl	r3, #16
	mov	r2, #0xc0 / lsl r2, #24		<- 0xc0000000
	cmp	r3, r2
	bne	...
```

```c
unsigned short d = (a->facing + 0x2000) & ~0x3fff;
if (d == 0xc000) { ... }
```

Two details are forced, and both were measured against the alternative rather
than reasoned about:

* **The mask is `~0x3fff`, not `0xc000`.** gcc pools 0xffffc000 in a single
  `ldr`; `0xc000` costs `mov` + `lsl` and diverges at that instruction. Writing
  a high mask as the complement of a small constant is what puts the 32-bit
  form in the pool — which is worth carrying beyond facing tests.
* **The result is an `unsigned short`.** The `lsl #16` against a pre-shifted
  0xc0000000 is batch 29's narrowing-cast tell, here on an equality instead of
  a range. An `int` result drops both shifts.

`& 0xc000` on an `int` screens at 40 differing of 48; the spelling above is
exact. Both this and the named-local form matched, so the local is written out
in the source because it is what makes the 16-bit compare legible, not because
it is required.

**There is more of this.** Twenty-eight files in `asm/` hold `0xffffc000`, and
ten of the functions that do are sixty instructions or fewer. Five were taken
here; two more (`OvlFunc_962_200806c`, `OvlFunc_950_2008500`) got past the
facing test and stopped on something else, described below; the remaining five
are the sanctum family already parked for a split pool.

## A repeated `cmp` against the same immediate is a switch tree

`OvlFunc_971_200906c` did not match, but working out why produced a reading
that is worth more than the function.

gcc-2.96 lowers a `switch` two ways. **Two case labels** gives a plain equality
chain. **Three or more** gives the balanced decision tree, whose signature is
the same constant compared twice in a row — once feeding `beq`, once feeding a
relational branch out to the default:

```
	cmp	r5, #0xd
	beq	.Lcase_d
	cmp	r5, #0xd	<- again
	bgt	.Ldefault
	cmp	r5, #0xc
	bne	.Ldefault
	<case 0xc, by fall-through>
```

That repeat is not redundant code and not something to spell away. It says the
source has at least one more case label than the number of distinct bodies the
ROM shows — the extra label shares a body with another case or with the
default, so it leaves no trace of its own. Adding a `case 0xe:` carrying the
default's value reproduced the tree exactly, register for register, which is
the control that makes this a reading rather than a guess.

`200906c` is parked because the third label's real value is not recoverable
from the function alone, and because the three near message ids it selects
(0x297f / 0x2982 / 0x2985) also merge into one pool load and an add — the
branchless-nearby-constant behaviour, here across the arms of a switch.

## Two parks that are close, and one that is very close

**`OvlFunc_898_2008acc` — 44 instructions against 44, and only the literal pool
in a different place.** Same order, same operands, same registers; gcc dumps the
pool one `bl` earlier than the ROM does. The address arithmetic says the ROM's
point is the aligned one: counting from `0x02008acc` to the ROM's dump gives 84
bytes, so the pool sits 4-aligned at `0x02008b20` and the continuation lands at
`0x02008b28` — which is what the ROM's label `.Lb28` is named after. Ours dumps
at 82, is not aligned, and gcc pads. So gcc is choosing a *worse* point, and not
for range: the first pending constant is 58 bytes back against a `pool_range` of
1020. Four source spellings of the tail and three flag settings all give
byte-identical output, which is what makes this a placement blocker rather than
a reading problem. It is the cleanest instance of that class in the tree —
everything else about the function is right.

**A new two-member class: the message base held in a callee-saved register.**
`OvlFunc_962_200806c` and `OvlFunc_950_2008500` each speak three consecutive
message ids, and the ROM loads the first into a callee-saved register and
reaches the other two with `add r0, r5, #1` / `add r0, r5, #2`. We get three
independent pool loads and one fewer instruction, because gcc never allocates
the second callee-saved register — the ROM pushes `{r5, r6, lr}`, we push
`{r5, lr}`.

This is filed separately from `constant_reuse.c` on purpose. That file is about
CSE; this is not. The reuse here has to survive **three intervening calls**, so
what differs is a register-allocation decision — something has to judge a
constant worth a callee-saved register plus a push/pop pair, and this build
judges it is not. `-fcall-used-r4`, which is in `GCC296_CFLAGS` and takes one
register out of the callee-saved set, is the obvious suspect and is the thing
to measure next against a function of this shape that *does* match.

Writing the three ids as separate literals instead of `base`, `base + 1`,
`base + 2` gives byte-identical output, so tree-level constant folding is not
what decides it either.

## Two clusters screened and closed

* The parked 17-member `OvlFunc_883_200834c` was retried with batch 89's
  assignment-order idea. Three spellings gave 122–125 differing of ~142 against
  the park's 28 — decisively worse. Noted in the park, along with the fact that
  `find_shape.py` makes that cluster seventeen rather than the thirteen the
  original note claimed.
* `OvlFunc_881_2009888` screens at 53 differing of 61, all cascading from
  `__Func_80933f8(-1, -1, -1, 0)` — the `-1` rematerialisation blocker. The
  existing park at `20097fc.c` now records that it stands for four functions.
* `Func_80bf250` and its two shape siblings hit the same return-constant hoist
  that parked `Func_80bf37c` in batch 89. That park now stands for **six**
  functions rather than three.

## What went into the tree

`docs/elevation.md` gained two sections: the switch-tree rule and the quadrant
facing test. Three parks were extended rather than duplicated. No tooling
changed this round.
