/* OvlFunc_896_200c78c -- 0x0200c78c, asm/overlays/rom_78ef88/ovl_314_c_c_c_c_c.s
 * and its twin OvlFunc_897_200b30c -- 0x0200b30c,
 * asm/overlays/rom_791794/ovl_30_c_c_c_c_c.s
 *
 * The two have IDENTICAL opcode streams and differ in exactly three constants
 * (two table labels and one task function), so one solution elevates both.
 *
 * 88 of 88 lines, EIGHT differing.  Candidate at scratch/Lc78c.c.
 *
 * SOLVED:
 *   - `DMA3_CLEAR(buf, 0xca << 1)` from include/dma.h reproduces the
 *     `stmia r3!, {r0,r1,r2} / sub r3,#0xc` pair; the size is recoverable from
 *     the control word (0x85000065 -> 0x65 words -> 0x194 bytes).
 *   - `p = buf;` must come BEFORE the DMA call, not after: gcc otherwise
 *     schedules `mov r5, r9` past the whole DMA sequence.  50 differing -> 44.
 *   - Consuming the actor pointer (`a += 0x55; *a = z;` rather than
 *     `a[0x55] = z;`) -- 44 -> 8, the single biggest step.
 *
 * BLOCKER, three small things that together are the whole residue:
 *   - the ROM interleaves `str r0, [r5]` between the two byte stores and gcc
 *     will not (three lines, and swapping the source statements only moves
 *     which three);
 *   - the two table loads use OPPOSITE reg+reg operand orders in the ROM --
 *     `ldr r3, [r7, r2]` for the first and `ldr r3, [r3, r7]` for the second --
 *     and gcc emits the same order for both.  Writing the two address
 *     expressions with the operands reversed (`j + (int)T` versus
 *     `(int)T + j`) changes nothing; gcc canonicalises before it picks.
 *   - `lsl r1, #4` and `ldr r0, =fn` are swapped at the __StartTask call.
 *
 * TRIED: a separate byte-offset variable per table (56 differing, much worse);
 * both statement orders for the stores; naming the task priority so the
 * interleave lever could act on the final call.
 */
