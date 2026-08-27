# Batch 104 — the 1000+ band scouted, and five more jump tables

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`
(the four overlay functions out of their own `overlays/<rom>/overlay.elf`).

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `OvlFunc_928_2008e4c` | `02008e4c` | ovl_7b6668 | [ovl_314_c_c_a_c_c_c_c_b.c](../src/overlays/rom_7b6668/ovl_314_c_c_a_c_c_c_c_b.c) |
| `OvlFunc_945_200854c` | `0200854c` | ovl_7cb2c0 | [ovl_30_c_c_a_a_a_a_a_b.c](../src/overlays/rom_7cb2c0/ovl_30_c_c_a_a_a_a_a_b.c) |
| `OvlFunc_946_20093ac` | `020093ac` | ovl_7ced6c | [ovl_30_c_c_a_c_c_a_a_b.c](../src/overlays/rom_7ced6c/ovl_30_c_c_a_c_c_a_a_b.c) |
| `Anim_Summon` | `080d6578` | main ROM | [rom_d6504_a_c_b.c](../src/rom_c9000/rom_d6504_a_c_b.c) |
| `OvlFunc_953_2009a4c` | `02009a4c` | ovl_7d95dc | [ovl_30_c_c_c_a_a_c_c.c](../src/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_c_c.c) |

Eight functions were parked. Three of them are close enough to be leads rather
than dead ends: `Anim_UnleashIntro` at **2** of 80, `Task_BlitAnim` at 29 of 105
where 28 of the 29 are one register renumbering, and `OvlFunc_936_2009f14` at 12
of 103.

2509 functions remain in assembly; 91 of them still contain a jump table.

## The 1000+ instruction band: scouted, and it has a real blocker

The standing question from the last round was whether the 53 functions of a
thousand instructions or more are worth taking as a standing target.
`OvlFunc_883_200b4c8` — 1027 instructions, 264 calls, three labels — was taken
as far as it goes and parked at
[src/non_matching/overlays/200b4c8.c](../src/non_matching/overlays/200b4c8.c) at
**1025 of 1027**.

Getting there needed a new tool,
[tools/draft_script.py](../tools/draft_script.py), which simulates the register
file across argument setup and emits one line of C per `bl`. It got **264 of 264
calls** right. Transcribing that many calls by hand is not hard, it is long, and
long is where transcription errors come from — one wrong immediate in call 180
looks exactly like a codegen difference in the screen output. The tool turns the
work into review.

The residue is **constant reuse at scale**. About fifteen pooled constants are
hoisted into callee-saved registers and shared across the body; gcc as we invoke
it rematerialises them. `-fno-expensive-optimizations` reaches the *length*
(1027 of 1027 lines, differing count from 977 down to 864) but not a two-word
frame spill, and it is not in any of the five per-file flag groups the tree uses,
so adopting it would mean a sixth.

**Recommendation, and the reason this batch went back to the 60–110 band:** the
big functions are not blocked on transcription any more, they are blocked on one
optimizer behaviour. That behaviour has a *small* instance — see `Task_BlitAnim`
below — and small instances are where it should be solved. Working a 1027-line
function to learn something about a two-call constant hoist is the expensive way
round.

## A named constant that has to cross a call

`OvlFunc_946_20093ac` holds `0x7e` in `r7` across three calls:

```
ldr r7, =0x7e / ldr r3, =0x8c8 / sub r3, r7 / add r0, r3
...
ldrsh r3, [r5, r2] / sub r3, r7 / cmp r3, #8
```

A literal cannot produce that. `0x7e` fits in `mov`, so a literal gives
`sub r3, #0x7e` for the switch and lets gcc fold `- 0x7e + 0x8c8` into a single
pooled `0x84a` for the flag — measured, six differing. Only a **linker symbol**
survives into a register: gcc cannot fold `0x8c8 - (int)&_AREA_7e` at compile
time, so it loads both and subtracts, and the symbol is then already live when
the switch needs it.

The park [ovl_7ced6c/2009494.c](../src/non_matching/ovl_7ced6c/2009494.c) had
found this shape on the neighbouring function in the same overlay. This is the
first function it closes.

**Where the subtraction is written matters as much as that it is a symbol.**
Hoisting it into locals —

```c
base = (int)&_AREA_7e;  d = 0x8c8 - base;  if (__GetFlag(*e + d) == 0)
```

— puts both pool loads *ahead* of the `ldrsh`, and gives six differing again.
Written inline in the argument, the area is read first and the constants follow.

## A peeled `case 0` means an `else switch`

`Anim_Summon` tests `cmp r3, #0 / beq` *before* its twelve-slot table, then
subtracts 1 and dispatches on 1..12. Written as one switch with `case 0:`
grouped alongside `case 11:` (they share a body), gcc builds a thirteen-slot
table from 0 and the peel disappears: 90 lines against 92, 77 differing.

```c
if (c == 0) { Anim_Meteor(desc); } else switch (c) { ... }
```

reproduces exactly — and gcc then **cross-jumps the if-branch into case 11's
body on its own**, which is why one `beq` lands on a jump-table target.

`else switch` and the early-return form are not interchangeable here:

```c
if (c == 0) { Anim_Meteor(desc); return; }   /* 97 lines, five too many */
switch (c) { ... }
```

loses the shared epilogue, and this function's epilogue is three `gfree` calls.

## `case N:` alongside `default:` keeps the table

`Anim_UnleashIntro` dispatches on 0..4 where case 4 and the out-of-range case
share a body. Written with `default:` alone, gcc drops the table for a
comparison tree (59 differing). Written as

```c
case 4:
default:
    id = FILE_be;
    break;
```

the five-slot table appears and slot 4 doubles as the `bhi` target, which is
what the ROM has. Spelling the last case explicitly *next to* `default:` is the
lever; it is not redundant.

## The statement-form argument lever, and its boundary

`src/rom_c9000/rom_e0524.c` established that writing an argument as its own
statements gets the shift into the right place:

```c
arg = 0xc8;  ...  arg <<= 4;  StartTask(Func_80cc960, arg);
```

`Anim_UnleashIntro` needed it twice and it worked twice, taking six differing
instructions to two. Both were a shift racing a **pool load**:

```
rom    lsl r1, #4 / ldr r0, =Func_80cc960
ours   ldr r0, =Func_80cc960 / lsl r1, #4          (before)
```

The two that remain are a shift racing a **`mov`**:

```
rom    mov r2, #0x80 / lsl r0, #19
ours   lsl r0, #19 / mov r2, #0x80
```

Seven spellings were compiled against that one — the length as a local assigned
before the shift, before the other argument, before the preceding call, the
shift folded into the argument expression, the address through a separate
pointer local, and an unprototyped function-pointer typedef. All seven give the
same two instructions. **The statement-form lever reaches a shift against a pool
load and does not reach a shift against a `mov`.** That is a boundary worth
having written down, because the two look identical in a diff.

## gcc-2.96 has no immediate for an HImode constant

Storing a literal `0` through a `short *` does **not** give `mov r3, #0`. It
gives `ldr r3, =0x0` — a four-byte pool load of zero. Measured on every
spelling: `short`, `unsigned short`, through a `short *` local, indexing a
`short *` return. All pool.

So when the ROM has

```
mov r3, #0x0 / add r0, #0x64 / strh r3, [r0]
```

the source's right-hand side is **int-typed** and the `strh` is truncating it.
On `OvlFunc_936_2009f14`, writing `{ int zero = 0; *(short *)(...) = zero; }`
per case takes it from 106 differing to 12, and the only residue is that gcc
coalesces the four zeros into one pseudo whose live range crosses a `bl`, so it
lands in a callee-saved register where the ROM's is materialised after the call.

Two other things fell out of that function and both generalise:

* **A call result in the store expression must not go through a named local.**
  `p = GetActor(n); *(short *)(p + 0x64) = 0;` keeps `p` live and copies
  (`mov r2, r0 / add r2, #0x64`), ten instructions long over five arms. Inlined
  into the store, `r0` is dead after the `add` and the ROM's form appears.
* **A `bls`/`b` where the ROM has one `bhi` can be a length symptom, not a
  shape one.** A Thumb conditional branch reaches ±254 bytes. At 105
  instructions plus a five-word table the epilogue is just out of range and gcc
  inverts the test. It corrected itself once the body was the right length —
  chasing it directly would have been chasing a consequence.

## A constant hoisted across a call — the small instance of the big blocker

`Task_BlitAnim` case 1 makes two indirect calls and both take a length of
`0x4000`. That is not a Thumb immediate, so gcc materialises it as
`mov #0x80 / lsl #7` — and having built it once, keeps it in a **callee-saved
register across the first call**:

```
rom    mov r2, #0x80 / ... / lsl r2, #7 / bl   ...   mov r1, #0x80 / lsl r1, #7 / bl
ours   mov r5, #0x80 / lsl r5, #7 / ... / bl   ...   mov r2, r5 / bl
```

That third live value is why the prologue is `push {r5, r6, r7, lr}` against the
ROM's `push {r5, r6, lr}`, and the extra register renumbers r5/r6/r7 through the
whole body. Twenty-eight of the twenty-nine differing instructions are that
renumbering.

**Flags do not reach it.** Screened under `-fno-gcse`,
`-fno-rerun-cse-after-loop`, `-fno-cse-follow-jumps` and `-fno-force-mem`: all
identical. `-fno-expensive-optimizations` is worse. This is local CSE, not
global, which is consistent. Worth noting the sibling function out of the same
`.s`, [rom_cd260_b.c](../src/rom_c9000/rom_cd260_b.c), *is* built with
`-fno-rerun-cse-after-loop`, so the flag was the first thing tried.

This is the same family as the 1027-instruction park's residue, at two calls
instead of fifteen. It is parked at
[src/non_matching/rom_c9000/cd260_a.c](../src/non_matching/rom_c9000/cd260_a.c)
and is the recommended place to attack the class.

## The r0-against-a-shift rotation, fourth sighting

```
rom    mov r1, #0xe8 / mov r2, #0xda / mov r0, #8 / lsl r1, #16 / lsl r2, #18
ours   mov r1, #0xe8 / mov r2, #0xda / lsl r1, #16 / lsl r2, #18 / mov r0, #8
```

`OvlFunc_948_2009fd8` ends on exactly this, four times over, joining
`OvlFunc_911_2008304` and `OvlFunc_888_20085cc` from the previous round. Two
levers were tried and neither moves it: an unprototyped callee declaration gives
the identical output, and the return-type lever makes it worse. Given the
boundary found on `Anim_UnleashIntro` above, this is very likely the same
shift-against-a-`mov` class — which would make it five functions on one
unsolved shape, and a good candidate for the next focused round.

## Jump-table case order, four more times

Every table this round had a case order that is not numeric, and every one of
them is readable off the block layout:

| Function | Case order |
|---|---|
| `OvlFunc_945_200854c` | 1/2, 4/23, 5, 15/17/19, 21 |
| `OvlFunc_948_2009fd8` | 12, 10, 8, 6, 4, 2 (descending) |
| `Anim_Summon` | 1..12, with 0 grouped at 11 |
| `OvlFunc_953_2009a4c` | 5, 69, 7, 70, 64, 65, 66, 12, 21, 67, 68, 31 |

`OvlFunc_953_2009a4c` is the largest table converted so far: **66 slots, 12 live
targets, 18% density** — and gcc still builds a table rather than a comparison
tree, so the fifty-four gaps can be left to the default. Its `.s` held no data
at all, so the whole file converted and the linker slot only changed name.

The 4/23 pairing in `OvlFunc_945_200854c` is the kind of thing worth pointing
at: nothing about the function suggests those two areas belong together, and
nobody would guess it. The table says so.
