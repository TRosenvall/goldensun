# Batch 143 — a one-word declaration, and the limits of four levers

Verified from a clean build: `make clean` → host recovery for the five
`old_agbcc` objects ([batch-61](batch-61.md)) → `make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`. Every
address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_801eea0` | `0801eea0` | main ROM | [rom_1de5c_c_c_c_c_a_a_a_c_a.c](../src/rom_15000/rom_1de5c_c_c_c_c_a_a_a_c_a.c) |
| `Func_8099340` | `08099340` | main ROM | [rom_97b54_a_c_c_a_c_c_c_c_a.c](../src/rom_8a000/rom_97b54_a_c_c_a_c_c_c_c_a.c) |
| `Func_8012d70` | `08012d70` | main ROM | [rom_1219c_c_a.c](../src/rom_9000/rom_1219c_c_a.c) |
| `OvlFunc_933_2008344` | `02008344` | ovl_7bc690 | [ovl_314_a_c_a.c](../src/overlays/rom_7bc690/ovl_314_a_c_a.c) |
| `OvlFunc_974_200807c` | `0200807c` | ovl_7fcd20 | [ovl_30_a_c_a_c_c_a_a_a.c](../src/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_a_a.c) |
| `OvlFunc_943_200b3b8` | `0200b3b8` | ovl_7c7b9c | [ovl_30_c_a_a_c_a_c_c_a.c](../src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_c_a.c) |

Twelve functions were parked. Six sit at **2, 2, 2, 3, 4 and 7 instructions**,
which makes them leads rather than dead ends.

## The headline: a global polled without an intervening call must be `volatile`

`OvlFunc_974_200807c` polls `gKeyHeld` five times in a row with no call between
the reads. Declared as a plain `extern`, gcc CSEs all five into one load and the
function comes out **nine instructions short** — 73 against 82, differing from
line 1. With `extern volatile unsigned int gKeyHeld;` it goes to 84 lines
differing from line 36, and the rest was structural.

The declaration was already in the tree, in
`src/rom_c9000/rom_e3958_c_c_c_b.c`, where it was needed for a dead read. The
fact was known; it had simply never been carried to the files that poll the
value.

**So I counted rather than guessing at the scope.** Twelve symbols are declared
`volatile` somewhere in `src/`. Two are also declared plain elsewhere in
quantity: `gKeyHeld` in 8 files and `iwram_3001e40` in 52. Screening every park
that reads `iwram_3001e40` produced **one outright match** —
`OvlFunc_933_2008344`, whose park had recorded an entirely different blocker —
and no other movement. `Func_80b86ec` went from 46 lines and 37 differing to 45
and 21.

**Then the negative, which is what bounds the lever.** I swept all 47 parks that
record being *shorter* than the ROM — the exact signature of a CSE the ROM did
not perform — and re-screened every one with a resolvable reference and a
scalar global, with all globals marked volatile. **None improved.** Five parks
now carry a "volatile: tried, no change" note so the sweep is not repeated.

The rule that fits every case measured: **volatile matters only when a global
is re-read with no intervening call.** `OvlFunc_932_20082cc`'s poll loop
re-reads two actor fields every iteration and needs nothing, because
`__WaitFrames` sits between the reads and gcc must assume the call writes
through the pointers.

## One basic block: `if (x == 0) f(); else break;`

The second half of `OvlFunc_974_200807c`, and it is the same shape as batch
142's early-return finding moved from a return into a loop:

```c
if (gKeyHeld != 0) break;  __WaitFrames(1);      /* 84 lines, 47 differing */
if (gKeyHeld == 0) __WaitFrames(1); else break;  /* exact match */
```

The ROM keeps the test and the call in ONE basic block, which is what the
if/else produces; the early-break spelling splits them and costs a branch.

Batch 142 recorded the return-value version of this (`SetDjinni`, worth four
instructions). This round also **measured its limit**: rewriting the void
`return;` guards in `Func_808e0b0` and `Func_8096b88` as nested `if` blocks
changed neither by a single instruction. There is no value to hoist, so there
is nothing to move.

## Copying parameters into locals: sometimes everything, usually nothing

`Func_8093168` went from **56 differing to 4** on this alone. The ROM shows all
three copies — `mov r0, r2` (x), `mov r2, r3` (y), `mov r1, r0` (v = x) — with
the `iwram_3001ebc` load sitting between the first and second. Using the
parameters directly makes gcc compare the incoming argument register, saves a
move, and leaves the function an instruction short.

It does not generalise. Applied to three other close parks it produced:
`Actor_SetAnimAndSpeed` no change, `Func_808e0b0` no change, and
`CheckEquipmentCritBoost` **2 differing to 46** — because there the parameter
must stay an `int` for the pointer-base inversion to work at all.

## Two things measured to be out of reach

**`cmp rN, #<nonzero>` followed by `bge` is not reachable from an `if`.**
`Func_8093168` sits at 4 of 57 on exactly that: the ROM has `cmp r0, #8 / bge`,
we emit `cmp r0, #7 / bgt`. Identical semantics; gcc-2.96 canonicalises
`x >= 8` to `x > 7` when it inverts a branch. Four spellings were tried — `<= 7`,
`!(x >= 8)`, a `goto` form, and assigning in both arms — and the first three are
byte-identical while the fourth is worse.

Rather than try a fifth, I counted. Across all **3205 generated `.s` files** the
sequence appears exactly **once**, and that instance is **switch dispatch**, not
a comparison expression. gcc will emit the shape, but apparently only from
switch lowering.

**Scheduler interleaving is not reachable either**, on the evidence of
`OvlFunc_932_20082cc` at 2 of 74. The ROM interleaves a constant's
two-instruction build with a value's shift; we group the constant's pair. Three
spellings — splitting the shift into its own statement, naming the constant,
putting the constant on the left of the addition — are all **byte-identical to
each other**.

## Levers that worked, in brief

**Counter initialisation wants to come first.** `Func_8012d70` matched outright
once `i = 0;` was written before the offset computation rather than after: 2
differing to a match from moving one assignment up two lines. `Func_80b0070`
went 17 → 16 → 14 the same way. But do not reorder two counters against *each
other* — that took `Func_80b0070` to 42, because gcc then merges the two zeros.

**gcc allocates stack locals in reverse declaration order.** In
`OvlFunc_946_2008e00` the frame wants a 0x28-byte block at `sp+0x10` and a
three-int vector at `sp+0x38`; declaring the big block first puts it at
`sp+0x1c`. Declaring the small vector first fixes every offset in the function.

**A register-offset pair off one base needs a mutated offset.** The obvious
`t[0]` / `t[1]` gives immediate offsets off a single pointer. Keeping the offset
in its own int and doing `off += 4` between the loads reproduces the ROM's
`ldr [r2, r3] / add r3, #4 / ldr [r2, r3]`. Took `OvlFunc_959_2008d54` from 18
differing to 13.

**The halfword exception runs both ways.** `Func_801eea0` hit it twice in one
function: `*(short *)(p + 4) = 0x1e - x` pools the `0x1e`, and
`*(short *)(p + 6) = 0` pools the zero; naming each in an `int` took 13
differing to 7 and then to a match. But `Func_80b0070` shows the other
direction — the ROM emits `ldr r2, .Lb00e0` where `.Lb00e0` is a `.word 0`, so
there the literal is correct and naming it would be the error.

**Deriving a nearby global** gained two more uses —
`*(unsigned char **)((char *)&iwram_3001e90 - 4)` in `Func_801eea0` and the
same shape at `-0x4c` in `Func_80974d8`. The array-with-negative-index spelling
does NOT work: gcc declines to fold the `-1` and emits a runtime
`mov r3, #4 / neg r3, r3`.

**The increment tells, both halves.** A separate `lsl #16 / asr #16` after
`ldrh / add / strh` is a post-increment; after `ldrh / sub / strh` it is a
pre-decrement whose new value is used. `Func_8099340` matched on the first
screen because of the second.

## The naming lever now has a stated limit

Batch 142 recorded that naming an expression can be the whole fix, catastrophic,
or inert. This round found the condition that separates the first two:
**naming blocks reassociation only when gcc cannot see the definition.**

`GetNumDjinn`'s index was an opaque parameter and naming it was the entire fix.
`SetDjinni`'s `i = k + 0xf8` changed nothing, because `k = entry * 4` is
computed two lines up and gcc sees straight through the name. Without that
qualifier the lever reads as unreliable rather than conditional.

`SetDjinni` did move, though — 37 → 32 → 24 differing — on the reordering and
the else-return, and it is parked with the progression recorded.

## Parked this batch

| Function | Standing | Blocker |
|---|---|---|
| `Func_80974d8` | 2 of 48 | scheduling: load/store order |
| `OvlFunc_932_20082cc` | 2 of 74 | scheduling: constant interleave |
| `Func_8093168` | 4 of 57 | compare canonicalisation (measured unreachable) |
| `Func_808e0b0` | 3 of 52 | register-role swap |
| `Func_8096b88` | 7 of 49 | register-role swap (same family) |
| `Func_80b0070` | 14 of 55 | register choice for a materialised zero |
| `OvlFunc_959_2008d54` | 13 of 54 | register-role swap |
| `SetDjinni` | 24 of 54 | constant hoisting |
| `OvlFunc_946_2008e00` | 6 short | register pressure |
| `Func_80b86ec` | 21 of 44 | const rematerialisation (volatile fixed half) |
| `CheckEquipmentCritBoost` | 2 of 51 | prologue emission order |
| `Actor_SetAnimAndSpeed` | 2 of 51 | argument emission order |

`Func_808e0b0` and `Func_8096b88` are the same shape on the same field layout
(+0x25 flag, +0x27 count, +0x28 actor array) and should be re-attacked together.
Both need the count read TWICE in the source; reading it once is one instruction
short. And the same statement swap **helps one and hurts the other** — 5
differing to 3 on the first, 7 to 8 on the second. Statement order does not
transfer even between near-identical functions.
