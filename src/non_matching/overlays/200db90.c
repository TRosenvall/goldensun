/* OvlFunc_969_200db90 -- 0x0200db90, asm/overlays/rom_7f8b34/ovl_2b_c.s
 * and its twin OvlFunc_925_200b460 -- 0x0200b460
 *
 * The twins differ in ONE constant (0xa4 against 0x90), so one solution
 * elevates both.  41 of 41 lines, ELEVEN differing.  Candidate: scratch/Ldb90.c.
 *
 * SOLVED, and both halves generalise -- see docs/elevation.md:
 *
 *   SOURCE ORDER OF TWO LOADS DECIDES WHICH GETS r8 AND WHICH GETS r10.
 *   Two values are loaded before the first call and both survive it.  Written
 *   in the ROM's apparent order (halfword first, pointer second) gcc assigned
 *   them to the opposite high registers from the ROM.  Swapping the two
 *   assignment statements in the source -- while the emitted load order stayed
 *   the ROM's, because that follows first USE, not source position -- fixed the
 *   allocation, and fixed the mul operand order with it.  20 differing -> 11.
 *
 *   MUL COPIES THE SECOND OPERAND.  `mul rD, rS` computes rD = rD * rS, and
 *   gcc emits the copy for the RIGHT-hand operand of the C expression.  The ROM
 *   copies the addend-side value, so the source wants `c * r`, not `r * c`.
 *
 * BLOCKER: scheduling of the tail.  The same twelve instructions in a different
 * order.  Ours hoists the final halfword update's two loads (`ldr r1,=0xfffffe00`
 * and `ldrh r3,[r6]`) up past the three word stores; the ROM leaves them at the
 * bottom in source order.
 *
 * NOT AN ALIASING PROBLEM, despite appearances.  The hoist crosses stores to
 * a+0x10/0x38/0x40 while loading from a+0x64, but every one of those is a
 * constant offset from the SAME base register, so gcc disambiguates by
 * arithmetic and never consults alias analysis.  -fno-strict-aliasing changes
 * nothing, which is the confirmation rather than a surprise.
 *
 * TRIED, all 11: ALIAS, CSE, SCHED2 (worse, 15), O1 (worse, 29, and one line
 * short), -fno-schedule-insns; naming the computed word in a local and storing
 * it twice; moving the a+0x38 store above the a+0x10 store in the source.
 */
