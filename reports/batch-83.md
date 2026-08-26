# Batch 83 — which operand becomes the destination, and a fifth `.sym`

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `OvlFunc_898_2008cfc` | `02008cfc` | [ovl_314_c_c_c_a_a_c_a_b.c](../src/overlays/rom_793768/ovl_314_c_c_c_a_a_c_a_b.c) |
| `OvlFunc_898_2008d78` | `02008d78` | [ovl_314_c_c_c_a_a_c_a_b.c](../src/overlays/rom_793768/ovl_314_c_c_c_a_a_c_a_b.c) |
| `OvlFunc_901_2008804` | `02008804` | [ovl_314_c_c_a_a_c_c_a_c_c_a_a_b.c](../src/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a_a_b.c) |
| `OvlFunc_901_2008864` | `02008864` | [ovl_314_c_c_a_a_c_c_a_c_c_a_a_b.c](../src/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a_a_b.c) |
| `OvlFunc_926_2009334` | `02009334` | [ovl_314_c_c_a_c_c_c_a_a_c_b.c](../src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_c_b.c) |
| `OvlFunc_920_2008214` | `02008214` | [ovl_30_c_a_c_c_a_a_b.c](../src/overlays/rom_7a6ae4/ovl_30_c_a_c_c_a_a_b.c) |

**Batch 81's closest park is solved**, and it took two related things: a width,
and a symbol.

## Which operand becomes the `orr` destination

Thumb's `orr` is two-operand, so the destination *is* one of the operands — and
which one tells you what the source expression looked like:

    rom    ldrb r2, [r0] / mov r3, #2 / orr r3, r2      the CONSTANT is rd
    ours   ldrb r3, [r0] / mov r2, #2 / orr r3, r2      the VALUE is rd

`*p |= 2` gives ours. So does every commutative rearrangement — `*p = 2 | *p`,
`*p = *p | 2`, naming the loaded value, naming the constant as an `int`. gcc
canonicalises a commutative operator to put the constant second and none of
those reach it.

**Name the constant in a local of the width it is combined with, and write it
first:**

```c
unsigned char two = 2;      /* p is unsigned char * */
*p = two | *p;
```

The width is the part that matters — an `int` local changes nothing. That alone
elevated `OvlFunc_920_2008214`.

## For a halfword field it goes one step further, onto the pool tell

The same trick on a `u16` gets the operand order and loses the pool, because the
widened local is SImode:

| source | operands | constant |
|---|---|---|
| `*p \|= 2` | wrong way round | `ldr r3, =2` ✔ |
| `unsigned short two = 2; *p = two \| *p;` | right ✔ | `mov r3, #2` |
| `two = (unsigned short)(int)&_CONST_2;` | right ✔ | `ldr r3, =_CONST_2` ✔ |

**Only the symbol form gives both**, across twelve measured spellings. That is
the pool tell — *gcc never pools what an eight-bit `mov` can build* — arriving
with a measurement behind it rather than as an assumption, which is the
difference between this and the false lead batch 79 recorded.

The internal control is unusually good and is why this is filed as the tell
rather than as a compiler difference. `OvlFunc_898_2008cfc` uses the value 2
**twice**: once as the save-flag id in `__GetFlag(2)`, where the ROM writes
`mov r0, #2`, and once in this OR, where it writes `ldr r3, =2`. Same value,
same function, one immediate and one pooled.

## `const.sym`, and the bar for adding to it

A fifth symbol fragment, alongside `area.sym`, `message.sym`, `file_table.sym`
and `wram.sym`. Those each cover one identified id space; this one is for pooled
constants belonging to none of them.

Its header states the bar, so it does not become a way to explain away any
inconvenient constant:

> 1. the ROM **pools** a value an eight-bit `mov` could build, and
> 2. **no** spelling of a literal reproduces both the pool and the surrounding
>    register allocation — measured, with the attempts recorded in the file that
>    needs the symbol.

Named by value (`_CONST_2 = 0x2`), asserting nothing about meaning. An absolute
assignment emits no bytes, so a wrong name costs nothing and a wrong value fails
`make compare` at once.

## Stack arguments want names too

The same lever, in its other application:

    literals   mov r3, #0x12 / str r3, [sp] / mov r3, #0xe / str r3, [sp, #4]
    rom        mov r3, #0x12 / mov r2, #0xe / str r3, [sp] / str r2, [sp, #4]

gcc computes and stores each argument in turn, reusing r3. Naming them as locals
keeps two pseudos alive and the four instructions come out in the ROM's order.
Three of `OvlFunc_920_2008214`'s five differences were this.

Read together with batch 82's finding — that a named local used once can *cost*
the preferred register — the rule is that a local is a lever in **both**
directions, and which way to push it is read off the ROM.

## One Makefile rule

`OvlFunc_901_2008804` reads and then sets the same save flag, `0x307`. At plain
`-O2` gcc hoists the id into r5 across the call, paying a push and a pop to save
one pool load — the constant-CSE shape `pick_candidates.py` screens for. The ROM
loads it twice. With `CSE_CFLAGS` (`-fno-rerun-cse-after-loop`) it loads it twice
too, and the function goes from 29 differing lines to none. Its file-mate is
unaffected by the flag and rides along.

## The range dispatch

`OvlFunc_926_2009334` picks one of four handlers by the player's facing
quadrant, and its four tests are not spelled the same way in the ROM:

    add r3, r2, r0 / lsl r3, #16 / cmp r3, r0        @ against 0x3fff0000
    add r3, r2, r0 / lsl r3, #16 / lsr r3, #16 / cmp r3, r1   @ against 0x3fff

The first leaves the value in the high half and compares against a pre-shifted
bound; the other three shift back down. **Both fall out of the same C** —
`(unsigned short)(a - 0x2000) <= 0x3fff` — and gcc picks the form per arc. The
two bounds are hoisted once and reused across all four, which is what writing
the same comparison four times gives. It matched on the first screen.
