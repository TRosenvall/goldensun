# Batch 174

Five functions and two parks. The round's result is a tool: the sibling
observation that closed batches 172 and 173 by hand is now
[tools/family_siblings.py](tools/family_siblings.py), and four of the five
elevations came off it.

## Function breakdown

| # | function | address | file | screens | what it took |
|---|---|---|---|---|---|
| 1 | `Func_8011590` | `0x08011590` | [rom_11568_a_c_a.c](src/rom_9000/rom_11568_a_c_a.c) | 2 | `*p++` for the paired global load |
| 2 | `Func_80058ac` | `0x080058ac` | [rom_56cc_a_a_a_c_b.c](src/rom_c0/rom_56cc_a_a_a_c_b.c) | 2 | typed stack buffer |
| 3 | `Func_8005a78` | `0x08005a78` | [rom_56cc_a_a_c_a_b.c](src/rom_c0/rom_56cc_a_a_c_a_b.c) | 2 | `int` parameter at the call site |
| 4 | `Func_8091174` | `0x08091174` | [rom_8d9a4_c_c_c_a_a_a_c_b.c](src/rom_8a000/rom_8d9a4_c_c_c_a_a_a_c_b.c) | **1** | nothing |
| 5 | `OvlFunc_947_2008fcc` | `0x02008fcc` | [ovl_314_c_a_a_a_c_b.c](src/overlays/rom_7d0e88/ovl_314_c_a_a_a_c_b.c) | **1** | nothing |

All five read back out of the linked ELF after a clean
`make clean && make && make compare`, each with a `.gcc2_compiled.` symbol at
the same address.

## THE TOOL

Batches 172 and 173 each ended with the same observation, arrived at by
accident both times: the thing that closed the function was sitting in an
already-solved `.c` in the target's own `_a/_b/_c` family.
`Sprite_DeleteLayerIndex` took its entire second half from `Sprite_DeleteLayer`;
`Func_809b364` took `g = gState;` with the offset left in the expression, and
`_CONST_1` for a pooled `1`, from a sibling thirty lines away that used both
against the very same halfword.

`fuzzy_solved.py` structurally cannot find these. It scores whole-skeleton
similarity, so a sibling that shares one idiom while differing everywhere else
ranks near zero. But the tree's splits keep genuinely related routines adjacent,
and that adjacency is signal the skeleton comparison throws away.

`family_siblings.py` ranks the remaining functions by how much of their own
family is elevated, and — the part that matters — prints **which symbols the
target's assembly names that a sibling already spells**. A family with sixty
solved siblings is worth nothing on its own; a family with one solved sibling
that already spells the exact global the target loads is worth a lot. Four of
this batch's five came off its output, two of them on the first screen.

What a sibling supplies is not a lever. `docs/elevation.md` already had every
lever these functions needed. It supplies the **instantiation** — which symbol
name, which of several near-equivalent spellings, whether a particular field is
reached one way or another. A general rule tells you a lever exists; a sibling
tells you what to type.

## `ldmia rN!, {rM}` FOR ONE WORD, OUTSIDE A LOOP, IS AN EXPLICIT `*p++`

`Func_8011590` reads two adjacent pointer globals:

```
    ldr   r3, =iwram_3001e6c
    ldmia r3!, {r5}          <- one word, post-incrementing
    ldr   r7, [r3, #0x0]
```

An `ldmia` with a single register in the list is not a block move; it is a load
with writeback, and gcc-2.96 emits it when the source walks a pointer.

| spelling | result vs a 46-line ROM |
|---|---|
| `a = p[0]; base = p[1];` | 46 lines, 2 differing — and the loads come out in the *opposite* order |
| `q = p; a = *q++; base = *q;` | **matches** |

The less obvious half is the declaration. The two globals had to be reached
through **one** `extern unsigned char *iwram_3001e6c[];`, not two separate
`extern`s at their own addresses: gcc cannot know that two independent externs
are four bytes apart, so it pools both and the `ldmia` is unreachable no matter
how the accesses are spelled. **Adjacent globals that the ROM reaches from a
single pool entry are one array**, and `wram.sym` confirms the addresses.

## AN OFFSET BELONGS IN THE LOAD, NOT IN THE ADDRESS

`Func_80058ac` reads a halfword out of a 16-byte stack buffer:

```
    rom    mov r3, sp        / ldrh r3, [r3, #0x8]
    ours   add r3, sp, #0x8  / ldrh r3, [r3, #0x0]
```

Same two instructions; the ROM's base register is the buffer itself and the
offset rides in the load. `*(unsigned short *)(buf + 8)` on an
`unsigned char buf[0x10]` folds the offset into the address. Declaring the
buffer with the type it is actually read as — `unsigned short buf[8]` — and
writing `buf[4]` matches.

This is the same materialisation question as the named-index rule pointed the
other way. There, naming a sum forced it into an index register; here, letting
the *access* carry the offset keeps it out of the address computation. The
discriminator is readable straight off the ROM:

> **If the base register holds a bare pointer and the constant rides in the
> load, the source indexed a typed array. If the constant is added into the
> register first, the source did pointer arithmetic on bytes.**

## A CALLER AND A CALLEE CAN DISAGREE ABOUT THE PROTOTYPE

`Func_80058ac` masks its argument to 16 bits at entry (`lsl #16 / lsr #16`),
which says its parameter is a `u16`. Its caller `Func_8005a78` passes the value
**unmasked**. Declaring the callee `unsigned short` at the call site adds a mask
the ROM does not have — two extra instructions; declaring it `int` matches.

So the two translation units did not share a prototype. That is ordinary for
this era, and it means **a callee's parameter type is a per-call-site fact, not
a global one**. Do not propagate a definition's narrow parameter type into the
`extern` at a call site unless the caller's assembly actually narrows. The tell
is cheap: a mask at the callee's entry and none at the caller's call site is
exactly this disagreement.

## THE TWO PARKS AGREE WITH EACH OTHER

Neither park is a new wall, but together they say something the individual
entries do not.

[`8091eb0.c`](src/non_matching/rom_8a000/8091eb0.c) — 43 lines against 41, 13
differing. Two residues: a branch to the immediately-following label that gcc's
jump optimiser will not delete, and a store where the ROM builds the stored
*value* first and advances the base register in place while we build the address
into a second register.

[`807961c.c`](src/non_matching/rom_77000/807961c.c) — `AddPartyMember`, 34 lines
against 37, 23 differing, with **the loop body byte-exact**. The entire residue
is that the ROM puts the loop guard *before* the loop's invariant setup and
rematerialises `ldr r0, =gState` on the not-taken path, which makes the store
block a join and costs two more branches. Our version computes the base once
before the guard and needs neither.

Between them, twelve source spellings and ten flag-group runs, and **every
single one lands on exactly the baseline numbers** — 43/13 and 34/23
respectively. That is the "if a source construct controlled it, one of these
would have moved" argument from batch 170, now with a much larger sample. Two
specific readings:

- `-fno-gcse` inert on `AddPartyMember` says the invariant hoist is the **loop
  optimiser's**, not global CSE's.
- The ROM's version of `AddPartyMember` is three instructions **longer** than
  ours. This is not gcc missing an optimisation; it is gcc declining to
  rematerialise a pool load that the original build did rematerialise — the same
  allocator difference the scratch-register and callee-saved-copy parks record,
  showing up in a third place.

Both parks carry full measurement tables, and both record what is *right* as
well as what is not, because in each case most of the function is correct and
re-deriving it would be the expensive part.

Two smaller facts worth keeping, both from `8091eb0`:

- Its derived offsets — `0x1d6` built as `0x17c + 0x5a`, `0x1f4` as
  `0x19e + 0x56` — come free from writing the plain offsets. gcc rebuilds them
  from each other on its own; they do **not** need to be spelled as derivations.
- Its pooled `ldr r2, =0x21` is **not** a `_CONST_*` symbol. gcc pools that
  literal by itself here, so the pooled-small-constant tell has an exception
  when the constant is stored through a halfword pointer at a large derived
  offset. Check that a plain literal does not already pool before reaching for
  `const.sym`.

## Method

Fourteen screens for five matches, plus twelve probes across the two parks. The
two first-screen matches (`Func_8091174`, `OvlFunc_947_2008fcc`) were both
straight-line DMA-and-call bodies where the whole job was reading the `dma.h`
macro's `cnt` word backwards to a size — `0x84000000 | (size / 4)`, so
`0x84000070` is `0x1c0` bytes — and applying the named-index rule to one
register-plus-register load. Neither needed a lever at all, which is the
batch-172 point about filters restated: the functions that need no documented
work are exactly the ones a filter tuned to predict documented work will hide.
