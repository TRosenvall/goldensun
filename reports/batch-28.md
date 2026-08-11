# Batch 28 — 8 functions, and a lever for the stack-arg-pair class

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–27 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare`, unassisted. Every address
below read back from the linked ELFs and every path confirmed to exist.

## The stack-arg-pair lever

This class has been parked against since batch 03. Where a call takes two stack
arguments, the ROM materialises **both** into separate registers before storing
either; gcc reuses one register and interleaves each build with its own store:

    rom    mov r3, #0x2d / mov r2, #0x2b / str r3, [sp] / str r2, [sp, #4]
    ours   mov r3, #0x2d / str r3, [sp]  / mov r3, #0x2b / str r3, [sp, #4]

**The fix: name both values as locals, assigned immediately before the call, in
the order the ROM stores them.** Three steps, each of which mattered:

| formulation | result |
|---|---|
| literals at the call site | 3 positions differ |
| the **shared** value named (it is also an earlier argument) | 2 differ |
| both named, first-stored assigned first | **match** |

**The middle step is the one that was missing for two years of parks.** A value
that appears both as a register argument *and* as a stack argument has to be
named **once and used twice** — that is what frees the register the other stack
value needs. In this batch it is `0x2b`, `0x54` and `0x1a` in different members.

**Adjacency is the other half.** `OvlFunc_932_20084cc` was parked one round
earlier having tried named locals and got *thirteen* differing positions, worse
than literals — because the assignments sat at the top of the function with a
call between them and their use, so gcc hoisted both materialisations above it.
Moving the same two lines next to the call matches. Making values live EARLIER
is not the same as making them live SIMULTANEOUSLY.

**The class is not "has stack arguments."** Several functions elevated in
batches 25–27 pass arguments on the stack and need no lever at all, because in
those the ROM fills the slots first and then the registers — which is what gcc
does anyway. The blocker is specifically the ROM keeping two slot values live
simultaneously.

Both parks that fell to this had recorded the right experiment and drawn the
wrong conclusion from it. That is worth knowing when reading any "tried, did not
work" line in `src/non_matching/`.

## Functions

| function | address | overlay | note |
|---|---|---|---|
| `OvlFunc_911_2008230` | `0x02008230` | rom_79e5c0 | facing read before the call |
| `OvlFunc_968_2009f28` | `0x02009f28` | rom_7f2f14 | the lever came from here |
| `OvlFunc_968_200a26c` | `0x0200a26c` | rom_7f2f14 | sibling |
| `OvlFunc_932_20084cc` | `0x020084cc` | rom_7b9cb4 | **unparked** |
| `OvlFunc_882_200810c` | `0x0200810c` | rom_77dd1c | **unparked**, family head |
| `OvlFunc_883_2008d70` | `0x02008d70` | rom_780898 | **unparked**, byte-identical |
| `OvlFunc_898_2008ea4` | `0x02008ea4` | rom_793768 | **unparked**, 5 constants differ |
| `OvlFunc_901_2008d24` | `0x02008d24` | rom_797990 | **unparked**, byte-identical |

Five of the eight came out of the parked set.

## One more codegen fact

`OvlFunc_911_2008230` reads the actor's facing **before** `__CutsceneStart` and
does the range subtraction after, so the value is live across the call and lands
in a callee-saved register:

    ldrh r5, [r0, #6] / bl __CutsceneStart / ldr r3, =0xffff5fff / add r5, r3

Its near-twin in `rom_7c5974` reads it after the call and matches written that
way. Same function, different overlay, and the read has to go on the side of the
call the ROM put it on.

## Counts

271 functions elevated in total. 3,028 hand-written functions remain in `asm/`
of 5,714. 95 parked functions, of which 4 document blocker classes rather than
individual functions.
