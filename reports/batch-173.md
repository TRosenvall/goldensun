# Batch 173

Five functions, all from the small-and-medium bands that batch 172 opened up
after the standing filters went homogeneous. Fourteen screens for five matches.

## Function breakdown

| # | function | address | file | screens | what it took |
|---|---|---|---|---|---|
| 1 | `Func_80be070` | `0x080be070` | [rom_bbb0c_a_c_a_c_b.c](src/rom_b5000/rom_bbb0c_a_c_a_c_b.c) | **1** | nothing |
| 2 | `OvlFunc_880_20092c8` | `0x020092c8` | [ovl_30_c_c_c_b.c](src/overlays/rom_7795e8/ovl_30_c_c_c_b.c) | **1** | nothing; a CRC-16 |
| 3 | `Func_800be20` | `0x0800be20` | [rom_b798_c_c_c_b.c](src/rom_9000/rom_b798_c_c_c_b.c) | 3 | unsigned compare; accumulator born before the call |
| 4 | `OvlFunc_969_20083a0` | `0x020083a0` | [ovl_314_a_a_a_c.c](src/overlays/rom_7f6e64/ovl_314_a_a_a_c.c) | 6 | `ALIAS_CFLAGS` |
| 5 | `Func_809b364` | `0x0809b364` | [rom_9ad70_c_a_c_b.c](src/rom_8a000/rom_9ad70_c_a_c_b.c) | 4 | named `gState` base; `_CONST_1` |

Every address was read back out of the linked ELF after a clean
`make clean && make && make compare`, each with a `.gcc2_compiled.` local symbol
at the same address -- the check batch 172 added after discovering that a stale
object makes an uncompiled elevation look green.

## A CORRECTION TO BATCH 172

Batch 172 reported "the epilogue names the return type" as a finding. It is not
new: `docs/elevation.md` already carried it twice, in batch 46 ("Tell:
`pop {r1}` in a function that looks void names a RETURN VALUE") and again for
`OvlFunc_971_20091bc`. The batch-172 report, its HANDOFF entry and the
`elevation.md` section have all been rewritten.

The case does add something, and the corrected entry now says only that. Both
earlier entries explain the r1 epilogue as the function returning *the value the
last call left in r0*, and both fix it by writing an explicit `return f(...)`.
`Func_80b6378` has no `return`, no trailing call and no value to return -- its
body ends in a store inside a loop -- and `void` -> `int` alone matched. So the
mechanism is one step earlier than those entries state: r0 is live at exit from
the **declared type alone**. `pop {r1}` means *declared* non-void, not "returns
something."

Worth naming the process failure, because it is cheap to repeat. Both existing
entries were found by a `grep` that took one command, and it was run only
because a *different* function's park note happened to mention the same tell in
passing. **A finding that feels new is worth one grep of `docs/elevation.md`
before it is written up as new** -- an 11,000-line notebook is past the size
where memory is a reliable index of it.

## AN ACCUMULATOR'S BIRTH POSITION DECIDES ITS REGISTER BANK, AND THE ASSEMBLY ARGUES AGAINST THE FIX

`Func_800be20` scans a command stream and sums a byte from each accepted
record. It came out at the ROM's exact 43 lines with five differing, and all
five were one allocation decision: the ROM keeps the running total in **r7**,
paying a push for it; ours used caller-saved r1.

Moving `total = 0;` above the `_GetSpriteInfo` call -- no other edit -- matched.

What makes this worth recording is that the ROM's own listing argues against the
fix:

```
    bl _GetSpriteInfo
    ldrb r3, [r0, #0x5]
    mov  r7, #0x0        <- the initialisation, AFTER the call
```

Read literally that says the accumulator is born after the call, which is
exactly where we had it and exactly what does not work. gcc decides the register
bank from the **source-level live range**, which crosses the call, and then
sinks the constant's materialisation below the call because rematerialising a
literal is free. So:

> **A constant initialiser appearing after a call in the ROM says nothing about
> where it was written. The callee-saved register it lands in says the live
> range crossed the call.**

This is the batch-171 push-list discriminator with a third reading. So far "a
push the ROM has and we lack" has meant *split a merged local*; here it means
*the value was born earlier than we wrote it*. Both are the same question,
because **the push list is a direct readout of which live ranges cross calls**,
and it is cheap to read before any lever is tried.

The other line of the five was signedness: `a > 0xee` on a value from `ldrb`
emits `bgt` with an `int` local and `bhi` with an `unsigned` one.

## A PURE-PLACEMENT RESIDUE HAS TWO CAUSES, AND ALIASING IS THE ONE NO SPELLING REACHES

`OvlFunc_969_20083a0` is a velocity integrator: three `int` fields advanced by
three more, two divisions, and a final halfword update written through a
**pointer read out of the struct at +0x50**. First screen: the ROM's exact 45
lines, five differing, of which two are `bl __divsi3` against `bl _divsi3_RAM`
-- a linker alias this overlay's own `overlay.ld` already resolves. So the real
residue was three lines, and they were the same three instructions in a
different order: gcc hoists `ldr r1, [r5, #0x50]` three instructions early and
sinks the `sub`/`str` pair for +0x4c past it.

That shape -- exact length, identical instruction multiset, a few lines
transposed -- is the recorded signature for "one probe then park." Three source
probes went into it before any flag was tried:

| probe | result (rom 45 lines) |
|---|---|
| baseline | 45 lines, 5 differing (2 of them the linker alias) |
| move the +0x4c statement after the +0x1c update | 47 lines, 45 differing |
| name the subtraction's result | inert |
| store +0x4c between the +0x18 and +0x1c updates | 49 lines, 47 differing |
| `-fno-schedule-insns2` | 45 lines, **22** differing |
| `-fno-gcse`, `-fno-strength-reduce` | inert |
| **`-fno-strict-aliasing`** | **45 lines, 2 differing -- both the linker alias** |

At -O2 strict aliasing lets gcc assume the pointer loaded from `+0x50` cannot
alias the `int` struct it was loaded from, which frees that load to move up and
hide its latency. The ROM leaves it where the source puts it.

**Two lessons, and the second is the one that cost time.**

First: `-fno-schedule-insns2` making it *worse* (22 against 5) is the recorded
"destroying the evidence" signature, and it should have been read as ruling out
**the scheduler**, not merely that flag. A residue that the scheduler is not
causing has to be coming from an assumption the scheduler was handed.

Second, and generalising:

> A pure-placement residue has two causes. If the instruction that moved is a
> **load through a pointer that could alias its surroundings**, suspect aliasing
> and try the flag FIRST -- source order cannot express an aliasing assumption,
> so no spelling will ever reach it. Reserve the one-probe-then-park rule for
> placements among values gcc can already see do not alias.

The tell is visible in the diff itself: ask what the moved instruction *is*. A
`ldr` of a pointer out of a struct that the same function writes through is the
textbook strict-aliasing case. A transposition of two arithmetic ops on locals
is not.

## THE SIBLING LESSON PAID TWICE MORE

Batch 172 ended with "before ranking anything, look at what is already solved in
the target's own family." `Func_809b364` is the strongest case yet.

It screened at 51 lines against the ROM's 55, with the first diff at line 1: our
`gState + 0x1da` folded to a single pooled `ldr =gState+474` where the ROM has
`ldr =gState` and a separate `mov`/`lsl`/`add`. Two further spellings of the
named base got to 54 lines and left one instruction missing -- the ROM's
`ldr r3, =0x1` against our `cmp r3, #0x1`. A pooled `1` where an eight-bit `mov`
would do is the pooled-constant tell, and no C spelling of the literal reaches
it.

`src/rom_8a000/rom_9ad70_c_c_a.c` -- a solved function **in the same `.s`
family** -- answers both at once:

```c
extern int _CONST_1;
...
    g = gState;
    m = *(short *)(g + (0xed << 1));
    if (m == (int)&_CONST_1) {
```

The named `gState` base with the offset left inside the expression, and
`_CONST_1` from `const.sym` for the pooled 1. Both idioms were already recorded
in `docs/elevation.md`, and both were already *applied to this exact offset*
30 lines away in the same family. Copying them took the screen from 54 lines
with a missing instruction to 55 lines with one symbolic difference,
`=_CONST_1` against `=0x1`, which is the same bytes after linking.

The point is not that the levers were undocumented -- they were not. It is that
**the family sibling supplied the specific instantiation**: which symbol name,
which of several near-equivalent spellings of the base, and the confirmation
that this particular halfword is compared against a pooled constant rather than
an immediate. A general rule tells you a lever exists; a sibling tells you what
to type.

## A SPLIT CAN REQUIRE EXPORTING LABELS FIRST

`split_s.py` refused `asm/overlays/rom_7795e8/ovl_30_c_c_c.s` outright: three
`.L` data labels would have ended up referenced across the new file boundary,
and a `.L` symbol does not survive into the object's symbol table, so the link
would have failed silently at the ROM level or loudly at the link.

Notably the labels had nothing to do with the target function -- they cross
between the *other* two parts. The fix is the one the tool prints: add
`.global` for each label in the `.s`, verify `make compare` is still green as
its **own step**, then split. A `.global` emits no bytes, so keeping the export
and the split separable means a layout mistake and a bad export cannot be
confused for each other. Both steps were verified green here before any C was
written.

## Parks

None. Every function attempted this round matched.
