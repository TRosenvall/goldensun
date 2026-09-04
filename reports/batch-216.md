# Batch 216

Five elevated, one parked. The batch turns on one identification: a pooled
constant that no literal spelling reproduces turned out to be **the size of a
routine the game copies into RAM and runs there**, and chasing that produced a
new symbol file, `size.sym`, plus the correction of an entry this same batch had
already written into `const.sym`.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `DecompressIcon` | `0x08021be0` | [rom_20198_c_c_c_c_a_a.c](src/rom_15000/rom_20198_c_c_c_c_a_a.c) |
| 2 | `LoadIcon` | `0x0801a5a4` | [rom_19ebc_c_b.c](src/rom_15000/rom_19ebc_c_b.c) |
| 3 | `Func_80f4028` | `0x080f4028` | [rom_f4008_a_a_c.c](src/rom_f4000/rom_f4008_a_a_c.c) |
| 4 | `Func_8091494` | `0x08091494` | [rom_8d9a4_…_c_b.c](src/rom_8a000/rom_8d9a4_c_c_c_a_c_c_b.c) |
| 5 | `OvlFunc_948_200a290` | `0x0200a290` | [ovl_30_…_c_b.c](src/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c_c_c_c_b.c) |

Parked: `Func_80a3c08` (20 of 62, at the ROM's exact length).

Gated on a clean `make clean && make compare`, every address verified against
the linked ELF with `tools/checkaddr.py`. Two of the five carry real names, so
the address check could actually fail on them, which is the case that tool
exists for.

## `size.sym`: A POOLED CONSTANT THAT IS A ROUTINE'S LENGTH

`DecompressIcon` pools `0x278` where gcc-2.96 always builds it with
`mov #0x9e / lsl #2`. Eight literal spellings were screened against it -- the
bare literal, `0x278u`, two arithmetic forms, `sizeof(int) * 158`, a `const`
initialiser, an `unsigned short` local and a `volatile` round-trip -- and all
eight build it. With everything else in the function already exact, that is
`const.sym`'s criterion 2 met cleanly, so an entry went in.

**Then the value was identified, and it changed the entry.**
`asm/rom_15000/rom_15430.s` holds three consecutive ARM routines:

    Func_8015afc @ 0x08015afc    0x278 to the next
    Func_8015d74 @ 0x08015d74    0x9c  to the next
    Func_8015e10 @ 0x08015e10    0x7c  to its own .func_end

`0x278` is `Func_8015d74 - Func_8015afc` -- exactly the number of bytes
DecompressIcon allocates and DMA-copies. **A size expressed as a symbol
difference is a link-time value, so gcc has no constant to build.** That is the
whole explanation for the pool, and it predicts where the tell should fire: the
ROM BUILDS 632 with `mov #0x9e / lsl #2` at four unrelated sites and POOLS it at
exactly the two functions that copy this decoder.

**Writing the subtraction in C does not work, and that is worth recording.**
`(unsigned)(Func_8015d74 - Func_8015afc)` emits TWO pool words and a runtime
`sub`: gcc cannot fold a difference of two external symbols, and the assembler
never gets the chance because the routines live in a separately assembled ARM
file. So a single symbol carrying the size is not a workaround -- it is what the
original source must have had.

`LoadIcon` then needed two more of them, and **both meet `const.sym`'s criterion
1 exactly** where `0x278` only met its reasoning: `0x9c` and `0x7c` each fit an
eight-bit `mov`, and the ROM pools them anyway.

Three entries in one identified id space is precisely the case `const.sym`'s own
header says should not accumulate there, so they moved to `size.sym` and the
`_CONST_278` entry written earlier in this batch was deleted rather than left to
rot. The rule that produced it still stands; it just had a better answer.

## A PIN CAN FORCE gcc INTO A HIGH REGISTER — AND SOMETIMES MUST

`templated.py` ranks candidates by `hi`, the count of r8-r11 references, on the
measured claim that high-register traffic predicts an intractable residue.
`LoadIcon` has some, and it is the opposite of intractable: the ROM keeps the
DMA control word `0x84000000` in **r8** across all three transfers, with
`mov r7, r8 / push {r7}` in the prologue to save it.

`dma.h`'s `DMA3_COPY` rebuilds that word per call. Doing so is **six
instructions SHORTER than the ROM** and needs no r8 save at all -- so this is
not a case of gcc being clumsy that a pin tidies up. Hoisting the shared word
into a local pinned to r8 and passing the whole control word through `DMA3_SET`
is what makes gcc reach for the high register and emit the ROM's prologue.

Read that beside the `hi` heuristic rather than against it: high-register
traffic still marks a function where gcc's own allocation will not match, but
the cure can be **asking for more pressure**, not less.

## THE FRAME SIZE IS A DIAGNOSTIC

`Func_8091494` does two DMA fills. Written with `dma.h`'s `DMA3_FILL` and
`DMA3_CLEAR` it screens with `sub sp, #0x8` where the ROM has `sub sp, #0x4`,
because **each of those inlines declares its own `u32 value`**. One local plus
two `DMA3_SET` calls is the shape, and the four-byte frame is what says so.

Two further things about that local, both measured:

  * **It must be assigned late.** `v = &value;` at the top emits `mov r5, sp`
    before the first allocation; the ROM emits it immediately before the first
    store.
  * **It must be PINNED.** Unpinned, gcc keeps `v` in r5 for the DMA argument
    but still writes the store as `str r3, [sp]` -- folding an address it can
    prove equals the frame pointer. Pinning to r5 forces the store through the
    register. A barrier was tried here instead and cost five lines; the pin is
    the tool.

## SOMETIMES NOT NAMING THE INTERMEDIATE IS THE MATCH

`OvlFunc_948_200a290` calls `__MapActor_GetActor` nine times and writes through
each result. Assigning each to a local `p` gives `mov r3, r0 / add r3, #0x59 /
strb` at every site: the local takes a register of its own and the return value
is copied into it. Using the call expression directly --
`__MapActor_GetActor(8)[0x59] = v;` -- lets gcc advance the return register in
place, which is the ROM's `add r0, #0x59 / strb r5, [r0, #0x0]`. Nine sites.

This is the reverse of the tree's usual advice, and both directions are now on
file: name an intermediate to pin down *where* a value lives or to stop a fold;
do **not** name one whose only role is to be consumed immediately, because the
name is what buys it a register.

## PER-SITE REMATERIALISATION, THREE TIMES IN ONE BATCH

A constant reused across calls gets commoned into a callee-saved register and
fed to each site with a `mov`, where the ROM rebuilds it. It appeared three
times here and the cure was the same each time -- a local, pinned to the ROM's
argument register, assigned separately per site:

  * `Func_80f4028`: `0xfa << 16`, passed to a divide and then to the projection
    call. Twelve lines.
  * `OvlFunc_948_200a290`: `0xc8 << 4`, the priority of three `__StartTask`
    calls.
  * `Func_8091494`: the same priority shape once.

`Func_80f4028` also confirms batch 215's rule about the crossed argument fill in
the case that rule predicts pins alone will not settle: its callee's pool load
sits between the two halves of the crossing, so pins get the shifts right and
leave the movs transposed, and one barrier closes it.

## THE PARK, AND WHY IT IS NOT A WALL

`Func_80a3c08` sits at 20 of 62 **at the ROM's exact length** -- no missing or
extra instruction, only register choice and schedule. Its `signed char` loop
counter reproduces gcc's packed form exactly (kept shifted left 24, read with
`asr #24`, stepped by `1 << 24`), which is the idiom its already-matched sibling
established.

Six spellings were measured. Four tie at **exactly 20**, and
`docs/elevation.md`'s own warning applies to them: a tie is evidence of a wall
only when the spellings differ STRUCTURALLY, and those four all share the
assumption that the loop body is straight-line C with the arithmetic written
inline. That assumption is what the park names as the next thing to vary. The
informative outlier is that named offset locals -- the house style its matched
sibling uses -- are strictly WORSE here at 41, which says the two functions
reach their offsets differently.

## HOUSEKEEPING

`split_s.py` was run on all three multi-function targets and its data handling
carried the two files with tails: `ovl_30_c_c_c_c_c_c_c_c_c_c_c.s` keeps its
`.section .data` with the still-assembled `_c` part, and `rom_19ebc_c.s` keeps
its `.section .rodata` `.incrom` blobs the same way, so no exported global lost
its home. That is the batch-211 hazard not firing because the tool was used.

One self-inflicted error worth recording: the first write of `LoadIcon`'s `.c`
was assembled from the screened scratch file with `tail -n +2`, which silently
dropped its `#include "dma.h"`. The link failed on three undefined `DMA3_SET`
references. Screening had passed because `tryc.py` compiled the *whole* scratch
file -- so a green screen says nothing about how the text is later transplanted.
