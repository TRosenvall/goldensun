# Batch 107 — the placement rule turns out to be general

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of its overlay's linked ELF with
`arm-none-eabi-nm`.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_902_20084e4` | `020084e4` | ovl_7987ac | [ovl_30_c_c_c_a.c](../src/overlays/rom_7987ac/ovl_30_c_c_c_a.c) |
| `OvlFunc_945_20087f8` | `020087f8` | ovl_7cb2c0 | [ovl_30_c_c_a_a_a_c_b.c](../src/overlays/rom_7cb2c0/ovl_30_c_c_a_a_a_c_b.c) |
| `OvlFunc_921_2008abc` | `02008abc` | ovl_7a7298 | [ovl_30_c_c_c_c_c_a_a_a_c_b.c](../src/overlays/rom_7a7298/ovl_30_c_c_c_c_c_a_a_a_c_b.c) |
| `OvlFunc_905_20090c8` | `020090c8` | ovl_799abc | [ovl_30_c_c_c_b.c](../src/overlays/rom_799abc/ovl_30_c_c_c_b.c) |
| `OvlFunc_936_2009f14` | `02009f14` | ovl_7c097c | [ovl_30_c_c_c_a_c_c_c_c_b.c](../src/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_c_c_b.c) |

Four were fresh from assembly off `tools/find_bb_lever.py`; the fifth was a
batch-104 park. **2494 functions remain in assembly, 227 parked.**

## The finding: WHERE a value is assigned is a general rule, not a lever

Three separate blocker classes turned out this round to be the same rule seen
from different angles. All three were already half-solved in the tree, and each
was recorded as a fact about the value's **type** or **name** when it is also a
fact about the value's **position**.

**The HImode-constant rule (batch 104).** Storing a literal through a `short *`
gives a pool load, so the ROM's `mov` means the source's right-hand side is
int-typed. That is right, and it is half the answer:

| `OvlFunc_936_2009f14`, four sites | differing of 103 |
|---|---|
| bare literal `0` | 106 |
| `int zero = 0;` inside each case — batch 104's park | 12 |
| ONE `int zero = 0;` at the top of the function | 83 |
| FOUR separate `int` locals at the top | **match** |

`OvlFunc_905_20090c8` says the same at one site: `int k = 0x63;` in the store's
own block is 46 of 69; the same declaration at the top of the function matches.

The int-typed value has to be **rematerialised** at the store, which is the
basic-block lever's placement rule wearing a type. Batch 104's park had the idea
and put it in the wrong block.

**The named zero (`OvlFunc_902_20084e4`).** The ROM holds one zero in r5 across
three `__MapActor_GetActor` calls. Bare literals build it twice. `int zero = 0;`
written immediately before the first store matches; the **same declaration
hoisted to the top of the function** is 14 of 61. Here adjacency is what is
wanted and distance breaks it — the opposite direction from the case above, for
the opposite reason: this value must SURVIVE the calls, not be rebuilt at them.

**The stack-arg pair (also `20084e4`).** Already documented as "name both,
adjacent to the call", and adjacency is again the whole trick.

So the through-line is not "name the value" or "use an `int`". It is:

> Decide whether the ROM **rebuilds** the value at its use or **carries** it
> there. Rebuilt means a local in a dominating block, one per site. Carried
> means a local adjacent to the first use, shared.

Both readings are visible in the assembly before writing any C, which makes this
a diagnosis rather than a search.

## Two spellings of one constant in one function

`OvlFunc_945_20087f8` passes `0xc0 << 6` twice, in the two arms of an inner
`if`:

```
OvlFunc_945_200c880   mov r1, #0xc0 / lsl r1, #6 / mov r0, #8    pair together
__Func_8092adc        mov r1, #0xc0 / mov r0, #8 / lsl r1, #6    pair SPLIT
```

The first wants a bare literal, the second wants the lever. One of each, same
value, same function — which is as direct a demonstration as the tree is likely
to get that the two shapes are not the same phenomenon.

## The flag-first rule held

Batch 106 concluded: try `-fno-rerun-cse-after-loop` first and keep the
literals; reach for the lever only for what the flag leaves. Applied to every
function this round:

* `OvlFunc_921_2008abc` — 54 differing of 63 under default flags, **4** under
  the flag with no change to the C. The four it left needed two different
  levers (return-type on one callee, basic-block on one argument).
* `OvlFunc_945_20087f8` — flag screened byte-identical, so it stays on default
  flags. Screening it was two minutes and it kept a per-file rule out of the
  Makefile.

Screening both ways before touching the C is now cheap enough to be routine.

## Smaller things worth keeping

* **`switch (++counter)`, not `counter++; switch (counter)`.** The second
  re-loads the global; `OvlFunc_905_20090c8` switches on the value already in
  the register.
* **Five sparse cases over a 420-wide range give a comparison TREE, not a
  table.** The `mov r2, #0x87 / lsl r2, #1` and `add r2, #0xd2` in that tree are
  gcc's own arithmetic on the pivots, not source.
* **`__MapActor_GetActor` results must not go through a named local** — third
  and fourth functions to need this. Named, gcc keeps the pointer live and
  copies it; inlined into the store expression, r0 is dead after the `add`.
* **A facing test with `lsl #16` against a pre-shifted bound is an
  `unsigned short`** (`OvlFunc_921_2008abc`), the batch-29 narrowing-cast tell
  on a range rather than an equality.
