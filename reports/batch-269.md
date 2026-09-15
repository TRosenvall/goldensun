# Batch 269 -- five functions, a newly opened pool, and a pin that replaces a barrier

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. All five addresses checked
against the linked ELF (four of them in their overlay images), plus a control
symbol not in the batch that correctly came back absent.

| | |
|---|---|
| elevated | **5** |
| new tools | 0 |
| `.sym` entries added | 0 |
| whole-file conversions | 4 |
| splits | 2 (one a text/data rehome) |
| build-input changes | 0 |
| fakematch debt added | **2** |
| parks retired | 1 |

Solo throughout, no agents.

## What landed

| | function | address | source |
|---|---|---|---|
| 1 | `Func_80a847c` | `0x080a847c` | [rom_a7380_c_a_b.c](src/rom_a1000/rom_a7380_c_a_b.c) |
| 2 | `OvlFunc_948_2009df8` | `0x02009df8` | [ovl_30_c_c_c_a_c.c](src/overlays/rom_7d30e0/ovl_30_c_c_c_a_c.c) |
| 3 | `OvlFunc_971_2008f30` | `0x02008f30` | [ovl_30_a_c_c_c_c.c](src/overlays/rom_7fb4a8/ovl_30_a_c_c_c_c.c) |
| 4 | `OvlFunc_945_200c13c` | `0x0200c13c` | [ovl_30_c_c_c_c_c_c_a_a_a_a_c_a.c](src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_c_a.c) *(fakematch)* |
| 5 | `OvlFunc_931_2008d58` | `0x02008d58` | [ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c.c](src/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c.c) *(fakematch, from a park)* |

## The pool was the finding, before any lever was

Batches 266--268 spent three rounds and roughly forty measured spellings on
whole-function register rotations in the main ROM, and two of those rounds
produced **zero** elevations. Batch 268 ended by deriving a targeting filter --
a ROM prologue with no `mov rN, r8|r9|r10|r11` before the push avoids the
rotation class -- which passed only **5 of 49** main-ROM cold candidates.

Applied to the OVERLAY pool for the first time it passed candidates at 34--39
instructions against the main ROM's 60+. Four of this batch's five came from
there, and **three of the four were exact within six candidate spellings.**

That is not a statement about overlays being easy. It is that the main-ROM cold
pool has been picked down to functions whose residues are allocator decisions,
and the overlay pool has not. When a class stops yielding, re-derive the
candidate list before re-deriving the lever.

**The single-function overlay pool is now exhausted of unattempted work.** A
scan for single-function overlay `.s` files at 20--45 instructions with no
high-register prologue save and no data section returns 25, and `park_for` says
24 of them are already parked. The fifth landing came from re-attacking one of
those parks with a lever derived earlier in the same round, which is where this
pool goes next.

## A call result feeding a load wants a NAME; one feeding a store does not

`OvlFunc_948_2009df8` divides a field of a call's result:

```c
v = __MapActor_GetActor(0xb)->pos.x / 0x100000;   /* ldr r3, [r0, #8] ... in r3 */

p = __MapActor_GetActor(0xb);                     /* ldr r0, [r0, #8] ... in r0 */
v = p->pos.x / 0x100000;
```

The ROM runs the whole signed divide in r0 -- the register the call returned in.
Inlined, the load lands in r3 and the `0xfffff` pool temp in r2; named, the load
lands in r0 and the temp in r3. **18 differing to exact**, and six of those
eighteen were an argument-setup order that turned out not to be an independent
residue at all -- it fell out with the register choice.

This is the exact opposite of the recorded rule *"a call result in a store
expression must not go through a named local"*, and both stand. The
discriminator is what the pointer's use does with it:

| the use | write it |
|---|---|
| address arithmetic past the store's immediate range, then a store | INLINE |
| a load whose offset folds into the `ldr` | NAMED |

In the store case the named `p` has to survive into the address computation, so
gcc copies it (`mov r2, r0 / add r2, #0x64`) and the name costs two instructions
per site. In the load case there is no address computation -- offset 8 folds
into the `ldr` -- so the name costs nothing, and what it buys is the load
destination taking r0. Same result through `int *p` with `p[2]`, so it is the
BINDING that moves the register, not the struct type.

Three recorded rules now point at this from different sides -- this one, the
store one, and "two results of the same call need two pointer variables" -- and
none of them is "always name it" or "never name it". **Read what the ROM does
with r0 first.**

## The explicit loop guard SPELLS a shape; it does not improve one

`OvlFunc_971_2008f30` searches the `gState+0x1f8` party roster. So does the
parked `AddPartyMember` (`0x0807961c`). Same idiom, same array, same two-pointer
search, and the same construct measures opposite:

| function | `for` | hand `i = 0; if (i < n) { do {} while }` |
|---|---|---|
| `AddPartyMember` | 34 lines, 23 differing | 38, 31 -- WORSE |
| `OvlFunc_971_2008f30` | 45 lines, 29 differing | **EXACT** |

Both stand. gcc compiles a `for` to either a standalone guard followed by a
do-while, or a rotation with the test at the bottom shared with the loop entry.
It picks the second when the body OPENS with an early exit, because the exit
test and the entry test merge -- and when it does, the loop's invariant setup
(here the array base) is hoisted ABOVE the guard instead of sinking into it.

`2008f30` has that shape and `807961c` does not. So the hand guard is not a
lever that makes things better; it is a way to spell a shape, and it pays only
when the `for` picked the other one. **If the ROM has a standalone `cmp` /
`b<cond>` guard and your candidate does not, reach for it. If both already have
it, it costs four instructions and buys nothing.**

In both functions `if (n > 0)` is NOT the same as `i = 0; if (i < n)` -- gcc
compares the variable and does not fold the constant. On `2008f30` that is the
difference between 8 differing and exact.

`goto <label>` over `break` has now decided two loops: `AddPartyMember`
(27 -> 23) and this one (29 -> 12, and it is what brought the length to exact).
Both are searches whose early exit targets the shared `return 0` tail.

## A register pin beats a commoned small constant, and the barrier is not needed

This is the batch's most portable result, and it retired a park.

The recorded fakematch idiom is `register unsigned int rq __asm__("rN") = K;`
**plus** `__asm__ volatile ("" : : "r" (rq));`, with a flat note beside it that
the pin alone is inert. That was measured on the POOLED-constant form, where
cse1 demotes the later sites to `REG_EQUAL` notes and only a construct that
CONSUMES the register brings the immediate back.

For the `mov rN,#1 / neg rN,rN` form there is no pool load and nothing to
demote: cse1 simply forms one pseudo and copies it out. Declaring the
destination a hard register means there is no pseudo to form, and **the pin
alone is enough.**

| function | commoned value | what it took |
|---|---|---|
| `OvlFunc_945_200c13c` | three `-1` in one argument list | two pins, no barrier |
| `OvlFunc_931_2008d58` | two `-1` at calls 3 and 7 | **one** pin, no barrier |

`2008d58` was PARKED at 27 of 69 and one instruction long. Its park read the
blocker as being about the PAIR -- *"two separately named locals do nothing --
there is no dominating block to rematerialise them from -- and CSE_CFLAGS does
not fix it either"* -- and contrasted it with `OvlFunc_891_200a244`, where the
same naming works *"because the function has a guard, and the lever needs one"*.

That reading holds and is now bounded. **The guard is what a NAMED LOCAL needs;
a hard-register declaration needs nothing.** And the blocker is about the FIRST
site, not the pair: pinning the earlier `-1` is exact, and `__Func_8091ff0(-1)`
stays a plain literal and still gets its own `mov r0,#1 / neg r0,r0`, because
once no shared pseudo is formed there is nothing for a later site to copy from.

On `2008d58` the barrier is actively wrong: adding one MOVES the transposition
(r1/r2 instead of r0/r2) rather than closing it. Assigning the folded
`0xfc << 14` to the pin is what fixes it, and no barrier appears in the file.

**Try the bare pin before the barrier on any commoned small constant.**

## Both fakematches were torn down before landing

Per the recorded discipline -- *measure a fakematch's scaffolding by REMOVING
it, not by adding it*.

`OvlFunc_945_200c13c` first worked with four pins. Teardown:

| removed | differing |
|---|---|
| the p0 pin on the `-1` call | 31 |
| the p1 pin on the `-1` call | 31 |
| the `__Func_8092adc` statement split | 2 |
| the r2 and r3 pins | 0 -- **dropped** |
| nothing (as landed) | **0** |

`OvlFunc_931_2008d58` first worked with a pin at both `-1` sites. Teardown:

| removed | differing |
|---|---|
| the p1 pin on the `-1` at `__Func_80933f8` | 23, one long |
| the p0 pin on `0xfc << 14` at the same call | 2 |
| the `__Func_8092adc(0, 0x80 << 7, 0)` split | 2 |
| the `__Func_8092adc(0x12, 0xb0 << 8, 0x28)` split | 2 |
| the pin at the second `-1` site | 0 -- **dropped** |
| nothing (as landed) | **0** |

Building up tells you what helped; tearing down tells you what is still needed.
Both files landed two pieces smaller than the first form that worked.

One detail from `2008d58` worth carrying: the two `__Func_8092adc` splits want
DIFFERENT orders. `0xd0 << 8` gets `mov r1 / mov r2 / lsl / mov r0` with no
help; `0xb0 << 8`, three instructions later, needs `mov r1 / mov r2 / mov r0 /
lsl`. Same callee, same argument shape. **Read each site.**

## `datacheck.py` caught its second data section

`asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c.s` carries the
`0x02009390` overlay table and `gScript_930__02009730` alongside the function.
Neither `tryc` nor `objcmp` can see that -- both compare one function, and both
reported exact. The tool written in batch 268 out of batch 267's lost `.rodata`
flagged it before anything was deleted; the data rehomed to `..._c_a.s`, the
`.data` linker line repointed, the `.text` line untouched, and the layout-only
build gated green BEFORE the `.c` landed.

Two catches in two batches, on a corpus where 79 hand-disassembled `.s` carry
both code and data.

## A discipline miss, caught and fixed in the same round

`OvlFunc_948_2009df8` landed with `struct Actor { unsigned char pad_00[8]; int
f8; }` -- a locally invented struct, which is exactly what
*"Check docs/structs.md before inventing a struct"* exists to prevent. `Actor`
already carries 35 layouts across 75 files for one object, and offset 8 is
`include/actor.h`'s `vec3_t pos`. Converted to `#include "actor.h"` and
`p->pos.x`, re-verified byte-identical, and the build re-gated.

Recorded because the rule is easy to read as being about BIG structs. It is not
-- a two-field local struct is how a family reaches 35 layouts.

## Parks

`OvlFunc_924_200d158` at **7** of 40, size exact. Three levers took it from 29,
all about byte stores whose offsets exceed Thumb's `strb` immediate range so
each needs its own address register: two independently computed bases rather
than one chained off the other (plain field stores make gcc derive `&f22` from
`&f55` with `sub r2, #51`), a WALKING pointer for the adjacent pair
(`*p = 1; p++; *p = 2;` against `strb r3, [r2, #1]`), and a shared zero local
also used for a later store (the ROM holds 0 in r7 across the body).

The residue is which register the f55 base gets. Worth flagging for the rotation
class: this is a rotation between two SCRATCH registers, not callee-saved ones,
unlike every main-ROM specimen -- it may not be the same mechanism.

Six further parks landed earlier in this batch window and are described in their
own commits: `Func_80165d8` (size exact at 30, the smallest rotation specimen
yet), `GetPortrait` (51, the cheapest index-vs-pointer specimen),
`Func_80f6038` (40, and the read of `find_reg`'s pass-0 rule),
`UpdateScreenEdge_V` (74 of 66, a second `ip` specimen), `GetLocationName` (two
over, residue bracketed) and `Sprite_SetAnim` (66 of 77, prologue exact).

## State

```
funcindex --stats   4363 from C, 1347 still in asm     (batch 268: 4358 / 1352)
tools/census.py     TOTAL 1345  (76 hand-asm, 14 ARM, 585 parked, 670 available)
matched .c files in src/ (excl. parks): 3997
park files: 479
fakematch.txt rows: 442  (batch 268: 440)
```

**+5 from C and -5 still in asm, for exactly the five elevated** -- the FOURTH
consecutive batch where the delta reconciles. Batch 264's figure remains
unexplained and that entry stays open.

Park files went 473 -> 479: seven parked across the batch window, one retired
(`OvlFunc_931_2008d58`, elevated here). `fakematch.txt` went 440 -> 442, both
rows added this batch and both teardown-verified.

### ELF verification

All five at their claimed addresses -- `Func_80a847c` at `0x080a847c` in
`goldensun.elf`, and the four overlay functions in their own overlay images at
`0x02009df8`, `0x02008f30`, `0x0200c13c` and `0x02008d58`. The two labels moved
by the `rom_7b8cb0` text/data rehome link at their ORIGINAL addresses --
`gOvl_02009390` at `0x02009390` and `gScript_930__02009730` at `0x02009730` --
which is the byte-neutrality check for a rehome. Two control symbols not in the
batch were checked alongside and both correctly came back absent.

## Not done

* The single-function overlay cold pool is exhausted; the next round has to
  either widen the filter (multi-function `.s`, or 45--60 instructions) or keep
  re-attacking parks with new levers, which is what produced landing 5.
* The three global_alloc parks (`Func_80a8578`, `Func_80cd52c`, `Func_80919d8`)
  still want `global.c` -- `allocno_compare`, the conflict graph, `find_reg`.
* `Func_8028ef0`'s residue is reload scratch selection plus argument-setup
  order, a class with no recorded levers.
* `8021390`'s `_MSG_1b` build-input question and
  `OvlFunc_968_200c048`/`200c520` behind `200c2bc`'s floor, both still held.
* The `DMA3_SET` clobber widening measured green tree-wide in batch 268 is still
  deliberately unshipped.
