/* OvlFunc_882_200810c  [ovl_77dd1c] and four byte-identical siblings
 *
 * Source asm: goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_a_a.s
 * Siblings:   OvlFunc_883_2008d70, OvlFunc_898_2008ea4, OvlFunc_901_2008d24,
 *             and one more -- all IDENTICAL, not merely same-shaped.
 *
 * Blocker: STACK-ARGUMENT REGISTER REUSE. Seventeen instructions against
 * seventeen, diverging at the two stack arguments:
 *
 *     rom    mov r3, #0xa / mov r2, #0x54 / str r3, [sp] / str r2, [sp, #4]
 *     ours   mov r3, #0xa / str r3, [sp] / mov r3, #0x54 / str r3, [sp, #4]
 *
 * The ROM builds both constants into separate registers before storing
 * either; gcc builds one, stores it, and reuses the register. Same divergence
 * as OvlFunc_946_2009624 (src/non_matching/overlays/constant_reuse.c), where
 * naming the two values as locals was tried and cost an instruction rather
 * than separating them.
 *
 * This is now a recognised class in tools/elevation_candidates.py
 * ("stack-arg-pair"), which it was not when this family was picked -- the
 * family looked unblocked precisely because the filter did not know the
 * shape yet.
 *
 * THE DECLARATION LEVER DOES NOT REACH THIS CLASS (tested 2026-08-03 on
 * OvlFunc_901_2008d24, same shape, ovl_314_c_c_a_a_c_c_c_c.s).
 *
 * Prototyping a callee, or withholding the prototype, moves which register a
 * call fills FIRST -- that is what retired arg-fill-order in batch 07, in both
 * directions. It has no effect here: prototyped and implicit produce the same
 * 17-vs-17 diff at the same instruction.
 *
 * Naming the two stack values as locals still costs an instruction (18 vs 17),
 * exactly as recorded when this class was first written up, and so does
 * sharing a local for a value that appears both as a register argument and as
 * a stack argument.
 *
 * So the three levers that have solved other ordering classes -- declaration
 * state, named intermediates, and statement order -- are all confirmed not to
 * apply. Whatever separates the ROM here is upstream of argument set-up.
 */
extern void __SetFlag(int flag);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

/* Sets save bit 0x210 and applies a map edit. */
void OvlFunc_882_200810c(void)
{
    __SetFlag(0x84 << 2);
    __Func_8010704(0x28, 0x54, 7, 4, 0xa, 0x54);
}
