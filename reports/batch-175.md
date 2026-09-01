# Batch 175

Five functions and four parks. Three of the four parks turn out to be the same
wall, and measuring them together produced the batch's most useful result.

## Function breakdown

| # | function | address | file | screens | what it took |
|---|---|---|---|---|---|
| 1 | `Func_808edac` | `0x0808edac` | [rom_8d9a4_c_a_c_c_c_c_a_a_b.c](src/rom_8a000/rom_8d9a4_c_a_c_c_c_c_a_a_b.c) | **1** | nothing |
| 2 | `OvlFunc_899_200c704` | `0x0200c704` | [ovl_30_c_c_c_c_c_c_a_b_a_a_b.c](src/overlays/rom_794ac0/ovl_30_c_c_c_c_c_c_a_b_a_a_b.c) | 2 | loop increment order |
| 3 | `Func_809b3d8` | `0x0809b3d8` | [rom_9ad70_c_a_c_c_b.c](src/rom_8a000/rom_9ad70_c_a_c_c_b.c) | **1** | three edits to its twin |
| 4 | `OvlFunc_936_200b6f8` | `0x0200b6f8` | [ovl_30_c_c_c_c_a_b.c](src/overlays/rom_7c097c/ovl_30_c_c_c_c_a_b.c) | 3 | signed halfword decrement; read order at a `+=` |
| 5 | `LoadItemIcon` | `0x0801a370` | [rom_19ebc_a_c_c_c_a_c_c_b.c](src/rom_15000/rom_19ebc_a_c_c_c_a_c_c_b.c) | **1** | nothing |

All five verified in the linked ELF with a `.gcc2_compiled.` symbol at the
address, after a clean `make clean && make && make compare`.

## THE FAMILY TOOL IS NOW THE MAIN SOURCE

Every one of these five came off `tools/family_siblings.py`, and two of them
came off it in the strongest possible form.

`Func_809b3d8` is a near-twin of `Func_809b364`, which batch 174 elevated from
the same `rom_9ad70` family. Three edits to the solved file — `a->0x1c = -t`
for `= t`, `- d * 5` for `+ d * 5`, `0x80 << 13` for `0x90 << 12` — and it
matched at the ROM's exact 56 lines on the first screen. The two things that had
been hard the first time, the named `gState` base and `_CONST_1` for a pooled
comparison constant, came across for free.

`LoadItemIcon` is the same story one step removed: its sibling `LoadItemIconID`
had already established the `Blk` struct with `f600`/`f602`/`f604`, the
`extern int L29ee4[] __asm__(".L29ee4")` declaration, and `LoadIcon`'s
signature. Writing the new function was transcription.

This is what batch 174 predicted and it is now the dominant mode: elevate one
member of a family and its neighbours become cheap. **Re-run the ranking after
every round**, because a family's score changes the moment one of its members
lands.

## TWO SMALL LEVERS, BOTH ABOUT WHERE A THING IS WRITTEN

**The loop-order lever applies to the INCREMENT.** Batch 171 recorded
initialiser order as three-for-three and made it a first check.
`OvlFunc_899_200c704` is the same lever one clause over:

```
    rom    add r0, #0x1 / add r2, #0x10
    ours   add r2, #0x10 / add r0, #0x1
```

Written `for (i = 0; i <= 0x24; i++) { ...; t += 0x10; }` the pointer bump lives
in the body and lands first. Moved into the increment clause,
`for (i = 0; i <= 0x24; i++, t += 0x10)`, it matches. Exact length both ways;
2 differing to zero.

> A bump written in the loop body and the same bump written in the increment
> clause are not the same program to gcc.

**A compound assignment reads its destination first — if you write it that
way.** `OvlFunc_936_200b6f8` had two lines left:

```
    rom    ldr r3, [r5, #0x8]  / ldr r2, [r5, #0x24]
    ours   ldr r2, [r5, #0x24] / ldr r3, [r5, #0x8]
```

The source needs the added value later, so the obvious spelling is
`vx = a->0x24; a->8 += vx;` — and that emits the `vx` load first, because it is
the earlier statement. Writing `a->8 += a->0x24;` and *then* `vx = a->0x24;`
matches: gcc CSEs the second load away and keeps the destination read first.
Naming a value you need later does not have to happen at its first use.

That function's other line was `*(unsigned short *)q -= 1;` coming out as
`ldr r2, =0xffff / add r3, r2` — a `-1` that becomes `+0xffff` in an unsigned
16-bit type. `*(short *)q -= 1;` gives the ROM's `sub r3, #0x1`.

## THE THREE PARKS THAT ARE ONE WALL

Three of this batch's four parks are duplicate-constant CSE, and having three at
once made the shared fact visible:

| park | the repeat | cost |
|---|---|---|
| [`20092ac.c`](src/non_matching/ovl_7fa4ec/20092ac.c) | one `0x100` across three calls | r8, **+5 lines** |
| [`801965c.c`](src/non_matching/rom_15000/801965c.c) | a zero twice through halfword stores | +1 line |
| [`200a69c.c`](src/non_matching/ovl_7e7574/200a69c.c) | two constants twice across one call | r5/r6, +1 line |

**`-fno-gcse` is inert on all three.** So the pass responsible is `cse.c`'s
local constant CSE, not the global one, and no flag reaches it. That took four
runs across three functions to establish and is now recorded once so it does not
have to be re-established: `-fno-gcse` is the obvious reach and it is the wrong
one.

The class also has a predictable victim, which is new. `200a69c` is a cutscene
script — thirteen calls in a row, no branches, no loops — and it is unreachable
because two speed constants appear at two call sites with a call between them.
That is exactly the shape that forces a callee-saved register. **A function can
look completely trivial and still be blocked by this**, so the repeated-constant
check is worth running before the shape is even read.

`20092ac` also killed the one probe with a mechanism behind it: declaring the
two uses with different parameter modes, so they would be different RTL. gcc
folds the narrowing before CSE runs.

## THE FOURTH PARK, AND WHAT IT GOT RIGHT

[`80c0130.c`](src/non_matching/rom_b5000/80c0130.c) is 32 lines against 32 with
three differing — gcc hoists a DMA source's `+ 0x22` above a volatile `strh` and
swaps two pool loads. `-fno-schedule-insns2` doubles the count, the recorded
"destroying the evidence" signature, which per batch 173 rules out the scheduler
rather than merely that flag.

Its value is what gcc reproduced without help, three things that each look like
they would need a lever:

- **Two adjacent globals from one pool entry, at a NEGATIVE offset.** The ROM
  reaches `iwram_3001e78` as `mov r3, r2 / sub r3, #0x88` off
  `iwram_3001f00`'s pool address. `extern unsigned char iwram_3001f00[];` with
  `*(unsigned char **)(iwram_3001f00 - 0x88)` gives it exactly. Batch 174's
  one-array rule works backwards.
- **A second DMA's base derived from the first.** `add r3, #0x24` off
  `&REG_DMA0SAD` instead of a fresh `&REG_DMA3SAD` load, from plainly writing
  `DMA0_SET(...)` then `DMA3_SET(...)`.
- **A register destination derived from another register's address**, likewise:
  `add r1, #0x14` off `&REG_BG2CNT` from writing `&REG_BG2PA`.

## A TELL WITH AN EXCEPTION

`20092ac`'s pooled `ldr r3, =0x30` matched with a plain literal, as did batch
174's `ldr r2, =0x21`, and `801965c` has a third pooled zero. All are eight-bit
values a `mov` could build, all are stored through a halfword pointer, and in
every case **gcc pools the plain literal by itself**.

The recorded pooled-constant tell — an eight-bit value in the pool means the
source names a linker symbol — therefore has an exception for halfword stores.
Check that a plain literal does not already pool before adding to `const.sym`.
`Func_809b364` is the case where the symbol genuinely is needed, and there the
constant feeds a `cmp`, not a `strh`.

One more from `801965c`, worth keeping because the natural spelling is wrong:
the ROM's `strh r3, [r7] / lsl r3, #0x10 / cmp r3, #0x0` is a 16-bit zero test
on a value that came from a halfword *load* — so gcc already knows the high bits
are clear and will not narrow it. Writing the copy and the test as one
expression, `if ((out[i] = src[i]) == 0)`, makes the tested value the
**assignment's** value, whose type is the `unsigned short` lvalue's, and that is
what produces the `lsl #16`. Two separate statements do not.
