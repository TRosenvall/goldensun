# Batch 244 — eight for eight, a park retired, and three doc entries bounded

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_945_200a7d8` | `0x0200a7d8` | [ovl_30_…_c_c_a_b.c](src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_a_b.c) |
| 2 | `OvlFunc_888_200a90c` | `0x0200a90c` | [ovl_30_…_c_a_b.c](src/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a_b.c) |
| 3 | `OvlFunc_928_20085f4` | `0x020085f4` | [ovl_314_c_c_a_c_c_a_a.c](src/overlays/rom_7b6668/ovl_314_c_c_a_c_c_a_a.c) |
| 4 | `OvlFunc_933_2009180` | `0x02009180` | [ovl_4e4_c_b.c](src/overlays/rom_7bc690/ovl_4e4_c_b.c) |
| 5 | `OvlFunc_918_20098b8` | `0x020098b8` | [ovl_17ec_c_c_b.c](src/overlays/rom_7a5214/ovl_17ec_c_c_b.c) |
| 6 | `OvlFunc_965_2008d4c` | `0x02008d4c` | [ovl_30_a_a_c_c_a_c.c](src/overlays/rom_7ef4f4/ovl_30_a_a_c_c_a_c.c) |
| 7 | `OvlFunc_965_2008eac` | `0x02008eac` | [ovl_30_a_a_c_c_a_c.c](src/overlays/rom_7ef4f4/ovl_30_a_a_c_c_a_c.c) |
| 8 | `GetMarsDjinni` | `0x08095dd0` | [rom_944ec_a_c_a_c_a_b.c](src/rom_8a000/rom_944ec_a_c_a_c_a_b.c) |

Eight targets, eight matches, **no new parks**. Entry 7 was a bonus file-sibling.
Entry 8 is the only NAMED symbol, so its address came from the linked ELF.

**No landing in this batch needs a flag group** — including entry 8, which had
an `ALIAS_CFLAGS` alternative and did not take it.

## A PARK IS RETIRED, AND THE BLOCKER ENTRY WAS WRONG ABOUT ITS OWN EXAMPLE

`src/non_matching/overlays/2008eac.c` said of a four-argument constant-reuse
shape that *"there is no source spelling that separates them."* A four-register
pin does. Entry 7 is that function, landed.

The recorded entry's **general** claim — that the reuse belongs to the TU —
still stands. Only the example it names by name is wrong, so the park is
deleted and the section kept.

## A UNION MEMBER IS A THIRD ESCAPE FROM THE ALIAS BOUND, FOR ANY MODE

The recorded bound: *"The only source-level escape is `volatile` on BOTH sides
of the pair."* The other known escape is a `char` lvalue via alias set 0, which
cannot help entry 8 — a `char` lvalue cannot emit a halfword store.

The residue was the post-reload scheduler seeing no memory dependence between a
pointer store and a halfword store, because at `-O2` they sit in different alias
sets. A union restores the dependence **in the source**.

It is not "unions alias everything", and five controls pin that:

| spelling | result |
|---|---|
| `union {void *; unsigned short}` on the halfword store, plain other side | exact |
| the same tag on the **pointer** store, plain halfword | exact |
| two **different** union tags, one per side | exact |
| `union {int; unsigned short}`, other side a pointer store | 2 differ |
| …the same union, other side respelled as `int` | exact |
| `struct {void *; unsigned short}` on **both** sides | 5 differ |

A `COMPONENT_REF` of a **union** carries the union's alias set, and
`record_component_aliases` makes every member type's set a **subset** of it, so
`alias_sets_conflict_p` reports a conflict against any access whose type is a
member — and only those. A `COMPONENT_REF` of a **struct** carries the *field's*
set, which is why the struct spelling is inert.

That is the same subset machinery the "distinct alias set per struct tag" entry
uses for **separation**, read in the other direction. Both landings were
measured: the plain spelling plus `-fno-strict-aliasing` is exact, and the union
plus the flag is still exact. The union ships because it needs no Makefile rule.

## A PIN SET IS MINIMAL IN WIDTH AS WELL AS IN COUNT

Every recorded minimisation strips whole **sites**. None asks whether a pinned
site needs all its arguments named.

On entry 3 none of the six does — all six drop the third register at zero cost,
and three need only r0. **Nine named registers where the template form ships
eighteen.** Mechanism: those sites need the slot `mov` scheduled ahead of the
expensive argument's build, and naming r0 alone does that. Cost is about two
extra compiles per surviving pin.

## THE ORR-DESTINATION LEVER HAS A SECOND PRESENTATION: NAME THE RESULT

On entry 2 the documented `unsigned char m = 1; lv |= m;` spelling is **611
differing**. What matches is naming the IOR's **result** —
`u = q[0x5a] | 1; q[0x5a] = u;` — so the result is its own pseudo.

Operand order is not the lever: `1 | q[0x5a]` is byte-identical.

Worth re-screening two-line `orr` parks with, since the recorded spelling is
exactly the one they will already have tried.

## THE HImode-`int` TABLE IS DIRECTIONAL, NOT A RULE

That section records ONE shared `int` local coalescing across a call as the
failure mode, with four separate locals matching. On entry 2 the ROM **holds**
the constant in a callee-saved register across the call, so one shared `int` is
exactly what matches.

**The count of locals is read off the push list, not fixed at one per site.**

## A LARGE HEX MASK IS UNSIGNED, AND THAT IS WHAT LETS gcc NARROW IT

Entry 4's ROM pools `0xfffff000` for a mask feeding a halfword store. Written
`& 0xfffff000`, gcc pools `0xf000` instead — **wrong in the pool word, not the
instruction stream**, so `tryc`'s `=value` normalisation is blind to it and only
`objcmp` catches it.

`& ~0xfff`, `& -0x1000` and `& (int)0xfffff000` all match; `& 0xfffff000u` does
not. C89 types a literal above `INT_MAX` as unsigned, and combine then narrows
to the store's mode.

**The named-`int` cure the neighbouring entry prescribes is actively wrong
here** — it costs a register. Try the signed spelling first; it is free.

## REPLAY THE ROM'S ARGUMENT WINDOW; DO NOT ALWAYS FILL ASCENDING

On entry 1 the recorded uniform-ascending default is wrong, and loudly: writing
r1 after r0 at twelve sites makes cse hoist a constant into **a held pseudo the
ROM does not have**, costing eight bytes and shifting the pool — 470 of 472
differences.

Read the shorter output as this week's tell running backwards: *something is
commoned that should not be.* Forcing the descending callees by name does not
rescue it.

The hole test did almost all of that function: forty `mov rLOW, rHIGH` copies
over five registers, every one a hole by construction, and "nominated minus
holes" lands **21 differing at the exact size and exact instruction count** from
a standing start — against 700 for no pins and 703 for all pins. One of the six
closing pins is itself a hole and needed the pin anyway. **The hole test
nominates; it does not decide.**

## Three smaller results

**A pin at an expensive site that is not a CSE site can hurt** — two such sites
cost 3 and 2 on entry 2. The nomination screen is "expensive **and** commoned",
not "expensive".

**A repeated accessor call is a source-level repeat, and it is per-FUNCTION.**
The ROM emits one `bl __MapActor_GetActor` per field store; caching the result
costs 117 differing and eight bytes short on one function, 44 on another — while
the near-identical `OvlFunc_968_200ca2c`, landed yesterday, **does** cache.
Count the `bl`s between the stores.

**Pinning r0 alone orders a pooled r1, and the mirror pin does nothing.** On a
`mov r0,#0 / ldr r1,=0x101` transposition, pinning the cheap argument alone is
exact; pinning only the pooled one leaves it unchanged. That cures from the
*cheap* side where the recorded rule cures from the pool side — and it sharpens
"a partial pin can be worse than no pin at all": on a two-argument site whose
other argument is **pooled**, there is no constant left for CSE to re-home, so
the partial pin is a complete cure.

## Landing notes

**Entry 4 took the full three-step gated landing.** Its `.s` holds seven
functions plus a real `.section .data`, and two local labels crossed the cut.
Exported them with `.global`, **gated that alone** (a `.global` emits no bytes,
compare stayed green), split and gated again with no `.c` in the tree, then
placed the `.c`. The data and both exported labels landed in the piece that
holds them.

**Entry 5's split was unavoidable despite a single function** — its `.data` blob
is loaded from *another file in the same overlay*, so deleting the `.s` would
have broken that reference.

**One agent's suggested linker edit was wrong** and would have been easy to
follow: it named a `src/` path for the middle line. The build rule is
`asm/%.o: src/%.c`, so the script keeps `asm/` throughout. `split_s.py` wrote it
correctly; hand-editing is what would have got it wrong.

**Left as found:** the annotations above both functions in entry 8's `.s` are
stale mis-attributions naming unrelated routines, and `split_s.py` attaches a
comment block to the function below it, so they travelled into the split files
unchanged. Guessing better names would be worse than leaving an obviously wrong
one.

**For a later batch:** entry 8's three elemental siblings all exist —
`GetJupiterDjinni` is already solved and *is* its template; `GetVenusDjinni`
(`0x08096140`) and `GetMercuryDjinni` (`0x080965a8`) are still assembly in other
`.s` files and are near-twins.
