# Batch 87 — a family found by its instruction shape

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `OvlFunc_901_2008b40` | `02008b40` | [ovl_314_c_c_a_a_c_c_c_a_b.c](../src/overlays/rom_797990/ovl_314_c_c_a_a_c_c_c_a_b.c) |
| `OvlFunc_901_2008b9c` | `02008b9c` | [ovl_314_c_c_a_a_c_c_c_a_b.c](../src/overlays/rom_797990/ovl_314_c_c_a_a_c_c_c_a_b.c) |
| `OvlFunc_901_2008cc8` | `02008cc8` | [ovl_314_c_c_a_a_c_c_c_c_a_b.c](../src/overlays/rom_797990/ovl_314_c_c_a_a_c_c_c_c_a_b.c) |
| `OvlFunc_955_20089b0` | `020089b0` | [ovl_30_c_c_c_a_c_c_c_c_c_c_c_c_b.c](../src/overlays/rom_7ddb88/ovl_30_c_c_c_a_c_c_c_c_c_c_c_c_b.c) |
| `OvlFunc_959_2009be4` | `02009be4` | [ovl_9dc_c_a_c_c_a_a_a_a_b.c](../src/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_a_a_b.c) |

## Three of them came out of a regex

Batch 86 solved three door-opening scripts. Rather than hunt the rest by hand,
this round wrote the solved function's **exact instruction shape** as a pattern
with its per-function constants as capture groups —

    ldr r0, =(.L[0-9a-f]+) / mov r1, #(..) / mov r2, #(..) / bl __Func_8010560
    mov r3, #(..) / mov r2, #(..) / str r3, [sp] / str r2, [sp, #4]
    ...
    ldrb r2, [r5] / mov r3, #0xfe / and r3, r2
    ldrb r3, [r6, #9] / mov r2, #0xc / orr r3, r2

— and ran it over `asm/overlays/`. It found the three remaining members and
handed back nine constants each: the table label, both `__Func_8010560`
arguments, both stack arguments, and the three callback arguments. Filling those
into the solved template produced three files that **all screened clean on the
first attempt**, and a looser search on just the prologue confirms the family is
now exhausted.

This is worth doing whenever a solve looks like a template.
`tools/find_twins.py` only finds functions that are byte-identical up to symbol
names; these differ in nine constants each and it cannot see them. A shape
regex costs ten minutes and finds a different, larger class of sibling.

## The two ordinary ones

`OvlFunc_955_20089b0` passes `0x3a` as argument five of **both**
`__Func_8010704` calls and a different literal as argument six of each. Only the
shared one is named — the ROM keeps it in r5 across the pair (`mov r5, #0x3a`
once, `str r5, [sp]` twice) and rebuilds the other per call. Batch 83's
stack-argument lever, applied to exactly the argument that needs it.

`OvlFunc_959_2009be4` is a plain `switch` on two bits, and its **`default` arm
is real**: the ROM has a fifth block calling the same handler as `case 1`,
reached by the tree's two fallthrough branches. gcc does not cross-jump the two,
so both arms are written out with the same call in each. Writing one and jumping
to it would be a different function.

## One park, and why no symbol was invented

`OvlFunc_940_20083dc` is 42 lines against 43. Two constants sit `0x47` apart —
`0x209`, stored into iwram, and `0x1c2`, the `gState` member offset read
immediately after — and **both compilers noticed and derived opposite ends**:

    rom    ldr r2, =0x209 ... sub r2, #0x47      0x1c2 derived FROM 0x209
    ours   mov r2, #0xe0 / lsl r2, #1
           add r2, #0x49  ... sub r2, #0x47      0x209 derived FROM 0x1c0

gcc already had `0x1c0` in a register for the iwram offset and built `0x209`
from it in one instruction, which beats a pool load.

It would be easy to "fix" this by declaring `0x209` a `const.sym` symbol, since
a symbol cannot be folded. **That is not allowed here and the park says so**:
`const.sym`'s bar requires the ROM to pool a value an eight-bit `mov` could
build, and `0x209` is past that range, so its pool load carries no information
at all. The tell is not present and no symbol is invented.

## Where the project stands

| | count |
|---|---|
| elevated to C | 3,055 |
| still hand-written assembly | 2,624 in 1,034 files |
| parked with a measured diagnosis | 195 |

About 54% by function count. The remainder is not a random sample — what is left
concentrates in the blocker classes the parks document, and 36 of it is the
MP2K/m4a driver that `tools/not_c.py` marks as never having been C. Several
parks are families, so cracking one class releases more than one function.
