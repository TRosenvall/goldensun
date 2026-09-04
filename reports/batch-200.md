# Batch 200

Five elevated, and with them the `arg_interleave_flat` class is **closed — all
fourteen members, every one matched on its first screen.**

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_899_2008428` | `0x02008428` | [ovl_30_a_c_a_a_c_c_c_a_a.c](src/overlays/rom_794ac0/ovl_30_a_c_a_a_c_c_c_a_a.c) |
| 2 | `OvlFunc_942_2008144` | `0x02008144` | [ovl_30_c_c_a_a_b.c](src/overlays/rom_7c6bac/ovl_30_c_c_a_a_b.c) |
| 3 | `OvlFunc_944_2008468` | `0x02008468` | [ovl_30_c_c_a_c_c_a_b.c](src/overlays/rom_7ca63c/ovl_30_c_c_a_c_c_a_b.c) |
| 4 | `OvlFunc_959_2008bac` | `0x02008bac` | [ovl_9dc_a_c_c_a_a_a_a_a_a.c](src/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_a_a_a_a.c) |
| 5 | `OvlFunc_881_200a81c` | `0x0200a81c` | [ovl_30_…_a_a_c_b.c](src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_c_b.c) |

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## The class, closed

Fourteen functions, found by grouping the unelevated corpus by opcode set, all
sharing one blocker: the ROM writes one argument **inside** another register's
two-instruction build, and gcc emits it before or after the whole block. The fix
is the same every time — pin the argument registers, assign them in the ROM's
own order.

    batch 194   200bdec                          (before the note was re-read)
    batch 198   2008dc0, 2008e54, 2008e84
    batch 199   2008f5c, 2008f8c, 200881c, 20088ac, 20099bc
    batch 200   2008428, 2008144, 2008468, 2008bac, 200a81c

The note that had closed them concluded they *"need a compiler-level answer, not
a source-level one."* Its reasoning — that the basic-block lever needs a block
dominating the call, and these functions are branchless — is correct about that
lever. A pin does not go through basic blocks.

## Four things the class taught beyond the fourteen functions

**Shape grouping by opcode set works even though it ignores behaviour.**
`OvlFunc_921_20099bc` opens a cutscene and runs a map transition where the rest
play a sound and blit a table. Its body has nothing in common with them, and it
was grouped correctly anyway, because the blocker is a property of the
instruction sequence and not of what the sequence is for.

**"Screened to confirm rather than assumed" is not enough.** The note did screen,
and reported real numbers. Every variant it ran changed the *source* — which is
exactly what the mechanism it had correctly identified rules out. Measuring from
inside the closed set produces false confidence. The question to ask of a
measurement is whether it can reach outside the thing being tested.

**The interleaved neighbour can be a load.** `OvlFunc_944_2008468`'s is
`ldr r2, =0x1410000`, sitting between `mov r1, #0xa4` and its shift. Same fix;
the shape does not care whether the neighbour is a `mov` or a load.

**One member needed a second lever.** `OvlFunc_942_2008144` also carried a
`precompute_register_parameters` bind: both arguments of `__MapActor_SetSpeed`
are pool loads, so both are precomputed and the cheap `mov r0, #8` lands last.
Pinning r0 alone fixed that and left the two pool loads themselves transposed —
3 of 30 down to 2 — and pinning all three in the ROM's order closed it. **Two
independent pool *loads* order the same way two independent movs do**, which
matches what was measured on a three-register fill in batch 197.

## What the class cost, and what it was worth

These fourteen accumulated for many rounds because the ranker was correctly
refusing them, for a reason that was true of the tools then available. Once the
verdict was wrong, nothing revisited it — including a member elevated in batch
194 without anyone connecting it back to the note.

The note is kept rather than deleted. Its shape description, its discovery
method and its member list are the record of how a fourteen-function class was
found, closed for the wrong reason, and reopened, and the first two of those are
still good.
