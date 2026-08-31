# Batch 162

Five elevated, all from `tools/guarded_interleave.py`. Verified after a clean
`make clean && make compare`; SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address checked against the linked images.

| function | address | image |
| --- | --- | --- |
| OvlFunc_945_200b7d8 | 0x0200b7d8 | overlays/rom_7cb2c0/overlay.elf |
| OvlFunc_923_200996c | 0x0200996c | overlays/rom_7aa430/overlay.elf |
| OvlFunc_921_20098c4 | 0x020098c4 | overlays/rom_7a7298/overlay.elf |
| OvlFunc_945_200c5d0 | 0x0200c5d0 | overlays/rom_7cb2c0/overlay.elf |
| OvlFunc_953_200839c | 0x0200839c | overlays/rom_7d95dc/overlay.elf |

## The selector has turned this into a routine

`guarded_interleave.py` was written two batches ago out of a park correction. It
has now produced nine elevations, and this batch is the point where working its
output stopped being exploratory. Four of the five reached a match with no more
than two edits, and two matched on the first screen.

The routine, in the order the steps are worth trying:

1. **Name every constant whose uses are all AFTER a guard** -- the shifted
   builds, the `mov`/`neg` builds, and the pool loads. The entry block dominates
   the sites, gcc declines to keep the values live across the guard, and the
   rematerialised sequence interleaves the way the ROM's does.
2. **Do NOT name a flag id used by the guard itself**, and screen with
   `--no-rerun-cse` from the start when the ROM materialises one id both before
   and inside a conditional branch. All three new functions needed `CSE_CFLAGS`.
3. **Delete the callee's prototype** when the residue is purely argument fill
   order.
4. **Delete any local that only holds an address.**
5. Write the arms in the ROM's fall-through order.

## The refinement that made step 2 predictable

`OvlFunc_923_200996c` (last batch) reached 7 differing with everything else
exact, and all seven were the flag id commoned into a callee-saved register.
Naming it did not help, and neither did the bare literal -- the two are
byte-identical.

The reason is worth stating precisely, because it is not "flag ids are special":
the id's FIRST use is the `__GetFlag` call in the ENTRY BLOCK itself. There is no
guard between the assignment and that use, so the entry-block mechanism has
nothing to act on and the value is simply live across a call. **The discriminator
is the position of the first use relative to the guard, not the kind of
constant.** A value first used inside the guarded block rematerialises; a value
first used in the entry block commons and needs the flag.

That prediction held on all three of this batch's new functions: each shows the
same id before and inside its guard, each was screened with `--no-rerun-cse`
first, and each needed it. `OvlFunc_921_20098c4` goes 29 differing to 2,
`OvlFunc_945_200c5d0` 61 to 0, `OvlFunc_953_200839c` 74 to 4.

## The no-prototype lever, third and fourth confirmations

Recorded as narrow two batches ago after two hits. `OvlFunc_953_200839c` is the
third: `__Func_8092c40(0x10, 0)` filled r0 before r1 where the ROM does the
reverse, and deleting the declaration fixed it and closed the function.

It remains the right thing to try the moment the residue is the same
instructions transposed rather than different register names, and it costs one
screen. It does nothing for register ASSIGNMENT.

## The address-only local, fourth confirmation

`OvlFunc_921_20098c4` came to 2 differing under the flag, and both were
`ldr r3, [r3]` against our `ldr r2, [r3]` -- the ROM reusing the address register
for the loaded value. Deleting the `char *p` local and writing
`((char *)iwram_3001e70)[0x17] = 0;` at all three sites matched outright.

Four instances now, on four different shapes. When a local's only content is an
address, it is not neutral: it costs the ordering or a register.

## Reading rule confirmed: block layout names the `if` body

`OvlFunc_953_200839c` had its two arms inverted -- the ROM branches away to the
short arm and falls through to the long one, and writing the long arm as the
`else` produced the mirror image. Swapping so the fall-through block is the `if`
body took 58 differing to 4. The ROM's layout is the answer; it does not need to
be guessed from which condition reads more naturally.

## Also this cycle

Three Makefile `CSE_CFLAGS` rules added. A flag change does not rebuild existing
objects, so each install was followed by a build from clean rather than an
incremental one.

Splitting was needed for only one of the five: two of the source files held a
single function each and converted whole, and one more had already been split.
