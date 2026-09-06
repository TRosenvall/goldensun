# Batch 237 — two files that stopped needing to be split

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_956_20084a4` | `0x020084a4` | [ovl_30_a_c_c_a_c_c_c_c_c.c](src/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_c_c_c.c) |
| 2 | `OvlFunc_964_2009abc` | `0x02009abc` | [ovl_30_c_a_a.c](src/overlays/rom_7ed0a0/ovl_30_c_a_a.c) |
| 3 | `OvlFunc_964_2009c2c` | `0x02009c2c` | [ovl_30_c_a_a.c](src/overlays/rom_7ed0a0/ovl_30_c_a_a.c) |
| 4 | `OvlFunc_964_2009d04` | `0x02009d04` | [ovl_30_c_a_a.c](src/overlays/rom_7ed0a0/ovl_30_c_a_a.c) |
| 5 | `OvlFunc_925_200b1c0` | `0x0200b1c0` | [ovl_314_c_c_c_a_c_c_c.c](src/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_c.c) |
| 6 | `OvlFunc_925_200b208` | `0x0200b208` | [ovl_314_c_c_c_a_c_c_c.c](src/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_c.c) |
| 7 | `OvlFunc_925_200b324` | `0x0200b324` | [ovl_314_c_c_c_a_c_c_c.c](src/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_c.c) |

Entries 2–4 are all three functions of one `.s`, landed together. Entries 5–7
complete a five-function `.s` whose first two landed in batch 235, and the
split made for those two **collapses here**: the file now holds all five, and
its two `overlay.ld` lines fold back into one.

That is two split collapses in three batches. A split is scaffolding, and the
cheapest time to take it down is the batch that finishes the file.

## A RECOMBINATION IS NOT A CONCATENATION

One TU forces one return type. Merging the five meant `__MapActor_GetActor`
became `struct Actor *`, and batch 235's two functions had to give up their
`a[2]`/`a[3]`/`a[4]` subscripts for `a->f8`/`a->fc`/`a->f10`.

**Both were re-screened after the retype rather than assumed still exact.** The
verification is a whole-object comparison against the pre-split reference
recovered from `f4d733ad^` — 1312 bytes, 585 encodings, 60 relocations — because
`objcmp --func` cannot isolate a function in a multi-function candidate.

The same applies to entries 2–4: `2009abc` reads `+0x64` with `ldrsh` and
`2009d04` with `ldrh`, and one struct has to serve both. **A union is the wrong
fix** — gcc-2.96 aligns it to 4, the struct grows `0x70` to `0x74`, and the
`sub sp` and a store move with it. `short f64` plus an `unsigned short *` cast
at `2009d04`'s two read-modify-write sites is exact.

## A `char` LVALUE PINS A BYTE STORE TOO EARLY

The notebook records that strict aliasing can SINK a store and a `char *`
lvalue pins it. That entry runs in one direction only. This is the other one.

`200b208` and `200b324` each hide an actor with a byte store inside a loop
whose counter is spilled across the call. Through an `unsigned char *` the
store is alias set 0, so it conflicts with the counter's own spill slot and
sched2 may not hoist the reload above it. The ROM does. Giving the byte a
struct member hands the store that member's alias set and the reload moves.

Three controls on the finished `200b208`, everything else held fixed:

| control | result |
|---|---|
| revert only the byte store to a `char` subscript | 9 differing |
| finished file under `-fno-strict-aliasing` | 9 differing |
| finished file with sched2 off | 124 differing |

So the lvalue type is the whole lever, the mechanism is alias sets, and the
reorder is post-reload scheduling.

**This is a landing constraint, not just a finding.** The TU now depends on
strict aliasing being ON — the opposite of the usual direction — so it must
never fall under an `ALIAS_CFLAGS` rule. It does not today; no Makefile rule
names `rom_7b0400`. A directory wildcard added later would break it silently.

## INDEX, DO NOT WALK

A source `q = arr; ... *q++` is emitted BEFORE the loop guard. The ROM's copy
sits after it, in the strength-reduction preheader. Writing `arr[j]` lets LSR
build the induction variable itself, in the preheader, and yields the ROM's
`ldmia r6!, {r0}` for free: 32 differing to 9.

## A `goto` LOOP DOES NOT HAVE TO BE A TRADE-OFF

Recorded as a judgement call: `goto` stops gcc hoisting an invariant, but gives
up the strength-reduced induction variables a `for` would build — so count the
live values before reaching for it.

`2009d04` wants both. It rebuilds an invariant inside the loop AND carries two
increments in one block. Taking the `goto` and **writing the second counter by
hand** gets both:

| form | differing |
|---|---|
| `for`, derived index | 239 |
| `goto`, derived index | 223 |
| `goto`, `who++` written out | exact |

That extends the recorded corollary — anything set up before the loop had to be
written there — from the preheader into the body.

## THE `orr` OPERAND ORDER IS REACHABLE BY NAMING THE DESTINATION ADDRESS

The recorded section on `orr rd, rs` lists spellings that ALL cost a second
`GetActor` call here — 45 differing. `bp = &GetActor(t)->f23; *bp |= 2;` is
exact. It is the same lever as "name the store's DESTINATION pointer when the
ROM computes the address first", in its read-modify-write case. The two
sections should point at each other.

## THE ALLOCATION LEVER ALSO RUNS IN THE CALL-CLOBBERED HALF, BY CONFLICT

Recorded: hoisting a constant's assignment above an unrelated load flips a HIGH
register, via allocno priority. Measured in `20084a4` in the LOW half, where the
mechanism is different — conflict, not priority. With `g = 0` after the first
store, the `2` is built and reloaded in r2/r3 (20 differing); hoisting it above
makes the zero live across the store, r2 and r3 are both taken, and the copies
fall to the ROM's r1. Four placements measured: **anywhere before the first
store is exact, anywhere after is 20.**

Recogniser: "our low scratch register is one slot too high" means a
neighbouring value's live range is too SHORT, not that the allocator is out of
reach.

## Scaffolding retired

`20084a4` shipped with three levers fewer than it was solved with — a named
destination pointer, two named constants, and an r2 pin — each load-bearing
when found and each measured inert once the others were in. The sweep was
re-run rather than trusted, which is the only reason the file needs no
`fakematch.txt` entry.

`200b208` dropped a named destination pointer the same way; the five-function
leaf dropped four spellings that measure exactly zero.

`fakematch.txt` gains entries for the two functions that DO carry pins,
including `OvlFunc_925_200af18`, which batch 235's landing note asked for and
never got.
