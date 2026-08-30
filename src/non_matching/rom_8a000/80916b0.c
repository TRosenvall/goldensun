/* CutsceneStart -- 0x080916b0, asm/rom_8a000/rom_91584_a_c_a_c_c_c.s
 *
 * 60 lines against the ROM's 64, 39 differing.  Candidate at scratch/Ncs_best.c.
 *
 * PARTIAL PROGRESS WORTH KEEPING: A STORE THROUGH A POINTER TEMPORARY, NOT AN
 * OFFSET EXPRESSION.  The ROM computes each destination into a register and
 * stores at immediate zero -- `add r3, r6, r2 / strh r5, [r3, #0]` -- while
 * `*(short *)(p + o) = z` with a named offset gives gcc the register-offset
 * form `strh r5, [r6, r2]`, one instruction shorter EVERY time.  Rewriting the
 * two 0xcc2/0xcc4 stores as `q = (short *)(p + o); *q = z;` recovers both
 * instructions: 58 lines and 48 differing -> 60 and 39.
 *
 * That is a general shape and it is the reason to keep this note: when the ROM
 * has `add` followed by an immediate-offset store and we have a register-offset
 * store, the source wants a POINTER local, not an offset local.
 *
 * BLOCKER, two independent residues that the same treatment does NOT reach:
 *
 *   1. GCC DERIVES THE OFFSETS FROM EACH OTHER.  The ROM builds 0x1da, 0x1dc
 *      and 0x1de fresh -- `mov r3, #0xed / lsl r3, #1`, then #0xee, then #0xef
 *      -- and gcc emits one and then `add r1, #0x10` / `add r1, #0x2` off it.
 *      Writing them as three separate `(0xed << 1)` expressions is already the
 *      independent spelling; gcc derives anyway.
 *   2. `-1` STORED TO A HALFWORD IS POOLED.  We get `ldr r2, =0xffffffff`
 *      where the ROM has `mov r3, #1 / neg r3, r3`.  Note the CONTRAST inside
 *      one function: the neighbouring store of 0xffff IS pooled in the ROM too,
 *      so the two fields differ -- one is written as an unsigned 0xffff and the
 *      other as a signed -1, and only the -1 needs the mov/neg pair.
 *
 * TRIED AND WORSE: pointer temporaries for ALL the stores plus a named `m = -1`
 * -- 61 lines but 50 differing.  It recovers a line and disturbs the register
 * assignment throughout, which is a net loss.
 *
 * Settled: the `o = 0xfa << 1` offset is shared between the gState load and the
 * iwram store and then mutated by 4, which is the register-offset pair the ROM
 * has; and the guard reads a SIGNED halfword at 0xcb6 through the
 * register-offset `ldrsh` thumb requires.
 */
