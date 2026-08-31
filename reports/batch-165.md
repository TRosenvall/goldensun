# Batch 165

Five elevated. Verified after a clean `make clean && make compare`; SHA1
`5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
linked images.

| function | address | image |
| --- | --- | --- |
| GetMoveDisplayEffect | 0x0802706c | goldensun.elf |
| OvlFunc_916_2008150 | 0x02008150 | overlays/rom_7a37f0/overlay.elf |
| Func_801999c | 0x0801999c | goldensun.elf |
| Func_80199ec | 0x080199ec | goldensun.elf |
| Func_8096d2c | 0x08096d2c | goldensun.elf |

All five came from the `multi` population, which batch 164 opened up. Eleven
functions have now come out of it in two batches, against four thin rounds
immediately before. Three of this batch's five were at exact length on the first
screen and one matched with no edits at all.

## Initialise the RESULT before the value the conditions read

`GetMoveDisplayEffect` is a chain of independent `if`s each overwriting a result.
Written the obvious way -- compute the key, zero the result, test -- gcc
IF-CONVERTS the first test into a branchless `eor / neg / orr / lsr #31`
sequence and the whole function diverges: 37 of 36 differing, two lines long.

Moving the initialiser above the key computation:

    r = 0;
    t = m[1] & 0xf;
    if (t == 1) r = 1;

is 14 differing with nothing else changed. **When the ROM has a run of
`cmp / bne / mov` and we emit branchless arithmetic, hoist the result's
initialiser above everything the conditions read.** This is not the
assignment-position lever for register choice -- it changes the control flow gcc
emits, not which register a value lands in.

## `volatile` at the use site reaches a redundant load that `-fno-gcse` does not

The same function ends with the ROM reading `m[3]` twice with no store between.
Measured and all inert: two separate `int` locals for the two reads; passing the
already-loaded local; and `-fno-gcse`, which this file records as reaching
re-reads no cse-family flag does.

`*((volatile unsigned char *)m + 3)` at either use site matches outright.

So the flag and the use-site cast are **not** interchangeable. Reach for the cast
when the redundant load is at one identifiable site -- it is also the narrower
instrument, which is the right shape for something only one expression needs.

## THE ALIASING CLASS HAS A SECOND FORM: a load that SANK

`Func_8096d2c` sat at 4 of 41, everything else exact:

    rom   ... / ldr r6,[r5,#0x68] / strh r3,[r2] / ...
    ours  ... / strh r3,[r2] / ... / ldr r6,[r5,#0x68]

gcc SANK the `int` load past the halfword store, which is legal only because
strict aliasing says they cannot alias. `ALIAS_CFLAGS` matched outright; three
source orderings were inert.

The class is therefore not only "a reload that vanished" but also **a load that
moved to the wrong side of a store of a different width** -- same cause, opposite
symptom. `tools/aliastell.py` cannot find this form and its docstring now says
so: the ROM's order is the natural one, so nothing in the listing is anomalous
and it is visible only in the diff. Try the flag whenever a small residue is a
load on the wrong side of such a store.

## A stack argument equal to a register argument is ONE local, used twice

`OvlFunc_916_2008150` calls a six-argument routine from both arms of a branch.
In the first arm the ROM builds 4 once and uses it for BOTH the fourth argument
and the stack slot. Two literals give two materialisations and 23 differing;
`v = 4; f(0, 0, 1, v, v, 9)` matches. The other arm needs them distinct and gets
two locals.

An extra step on the per-call-site stack-argument rule: check whether the ROM's
stack value IS one of the register arguments before giving it a local of its own.

## The pooled halfword zero, fourth instance

`Func_801999c` came in at 3 differing and matched on it. A bare `0` stored to a
halfword compiles to `ldr r3, =0x0` where the ROM has `mov r3, #0`; routing it
through an `int` local fixes it. Four functions now. **When a screen is small and
the diff shows a pool load of a value that would fit an immediate, this is the
first thing to check.**
