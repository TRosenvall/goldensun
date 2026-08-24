/* SOLVED FOR FUNCTIONS WITH A BRANCH -- see reports/arg-interleave.md.
 *
 * THE LEVER: assign the shifted constant to a named local in a DIFFERENT BASIC
 * BLOCK from the call. Crossing a block boundary stops gcc keeping the value in
 * a register; it rematerialises at the call, and its rebuild of a
 * two-instruction constant is the split pair with the other argument in the gap.
 *
 * Five functions were matched with it in batch 37, including one from the
 * sibling pool-load-first class -- which establishes that the two classes are
 * one mechanism seen from two angles.
 *
 * THE FOUR MEMBERS BELOW ARE STILL PARKED, and the reason is worth reading
 * before anyone re-attempts them: ALL FOUR ARE STRAIGHT-LINE. There is no
 * basic-block boundary to put between the assignment and the call, so the lever
 * has nothing to work with. A call does NOT create a boundary; only a branch
 * does.
 *
 * Written with a named local anyway, a straight-line member gets WORSE -- gcc
 * keeps the value in a callee-saved register and pays a push/pop for it.
 * OvlFunc_908_20081a8 goes from 2 differing instructions to 6.
 *
 * What these four need is a way to make gcc rematerialise a value inside a
 * single basic block. That is the same thing OvlFunc_882_200c5b8 needs, and
 * the same thing the `-1` triple in src/non_matching/ovl_787e04/20093e4.c
 * needs. Three parked shapes, one missing construct.
 *
 *
 *   OvlFunc_967_2008030  asm/overlays/rom_7f21b8/ovl_30_a.s
 *   OvlFunc_973_200804c  asm/overlays/rom_7fc720/ovl_30_c_a_c_a_a.s
 *   OvlFunc_921_20085dc  asm/overlays/rom_7a7298/ovl_30_c_c_c_c_a_a.s
 *   OvlFunc_908_20081a8  asm/overlays/rom_79c0c4/ovl_30_c_c_c_a_a_a.s
 *
 * The fourth arrived in batch 36 from tools/match_shapes.py --near, which is
 * worth noting because the near matcher CANNOT see this blocker: it collapses
 * registers, so `mov r1 / mov r0 / lsl r1` and `mov r1 / lsl r1 / mov r0` have
 * the same skeleton. A near lead can therefore land on a known wall, and the
 * first one that did cost two screens rather than a round only because the
 * class was already written up here. It is nineteen against nineteen with two
 * positions differing:
 *
 *     rom    mov r1, #0xc0 / mov r0, #0x15 / lsl r1, #8 / mov r2, #0xa
 *     ours   mov r1, #0xc0 / lsl r1, #8    / mov r0, #0x15 / mov r2, #0xa
 *
 * Tried on this member, on top of everything below: the mismatching callee
 * undeclared (3 differ, worse), the PRECEDING callee undeclared (4, worse), and
 * the preceding callee given an `int` return (4, worse). -fno-schedule-insns2
 * and -O1 both go to 5. Every direction is worse than doing nothing, which is
 * the same result the other three members give.
 *
 * INTERLEAVED ARGUMENT SET-UP. Every instruction is right in all three; the
 * ROM splits a shifted constant's mov/lsl pair around another argument's move,
 * and gcc emits the pair contiguously:
 *
 *     rom    mov r1, #0x81 / mov r0, #0xe / lsl r1, #1
 *     ours   mov r1, #0x81 / lsl r1, #1   / mov r0, #0xe
 *
 * WHAT IS ESTABLISHED, and why this is worth another attempt:
 *
 *   * it is NOT an -O1 translation unit. Screened at both -O2 and -O1; the
 *     diff is identical, so the per-file -O1 rules in the Makefile do not
 *     explain it.
 *   * the instruction scheduler is not responsible either. -fno-schedule-insns
 *     and -fno-schedule-insns2 both leave the output unchanged.
 *   * nor is the callee's declaration. A full prototype, an old-style empty
 *     parameter list, and no declaration at all all produce the same order.
 *   * nor is the argument count: two-argument and three-argument calls both
 *     come out contiguous.
 *   * gcc-2.96 DOES emit the ROM's exact pattern from C somewhere in this
 *     tree -- SIX generated .s files contain a shifted constant's mov/lsl
 *     split by a move that is never itself shifted, e.g.
 *
 *         asm/overlays/rom_7c6bac/ovl_30_c_c_a_c_a_b.s
 *             mov r1, #128 / mov r0, #12 / lsl r1, r1, #7
 *
 *     (An earlier version of this file claimed seventeen. That count came
 *     from a looser scan that also caught the case where BOTH constants are
 *     shifted -- eleven of the seventeen -- which gcc produces readily and
 *     which is a different pattern. Six is the real number.)
 *
 * RESOLVED AS FAR AS IT CAN BE, 2026-08-03. Their src/ was read (the
 * permission is now explicit -- see docs/attribution.md) and the answer is
 * that THE C IS NOT THE VARIABLE.
 *
 * The decisive evidence is in their own matched corpus.
 * src/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_b.c contains
 *
 *     __MapActor_Surprise(0xb, 0x81 << 1);
 *
 * which is the SAME call shape as OvlFunc_967_2008030 below -- and it compiles
 * to the CONTIGUOUS form, and byte-matches the ROM there. So the ROM itself
 * contains BOTH orderings for identical source. The interleaving is a
 * context-dependent choice gcc-2.96 makes; it is not something the call site
 * can express.
 *
 * Ruled out by direct experiment since: eight formulations of the two-argument
 * call -- inline shift, named local assigned at its declaration, assigned as a
 * separate statement, both operands as locals in either order, a volatile
 * local, and the plain literal -- all produce the contiguous form. So does the
 * three-argument case with the shift written inline, which is the shape their
 * working examples use.
 *
 * What remains is the surrounding context: register pressure, or whatever else
 * gcc's scheduler responds to. Chasing that per function is not worthwhile.
 * Someone should characterise the trigger across all six known instances at
 * once, or accept these as fakematch candidates.
 *
 * (Superseded, kept for the record) THE NEXT STEP, and it is a short one: read the .c that produced
 * asm/overlays/rom_7c6bac/ovl_30_c_c_a_c_a_b.s. Its codegen shows an
 * unremarkable three-argument call, __Func_8092adc(12, 0x4000, 0), so whatever
 * differs is in the source and one look would settle it.
 *
 * That has NOT been done here, because docs/attribution.md says this project
 * does not read another decomp's src/ while writing its own C, and quietly
 * relaxing a documented rule unsupervised is not a call to make alone. It is
 * worth asking whether the rule should cover this: the file in question is not
 * a function being decompiled, and the question is about compiler behaviour
 * rather than anyone's expression of a function.
 *
 * Everything above reads compiler OUTPUT, not anyone's source; the clean-room
 * rule is intact as it stands.
 */

/* --- OvlFunc_967_2008030 ------------------------------------------------- */
extern void __MapActor_Surprise(int slot, int effect);

int OvlFunc_967_2008030(void)
{
    __MapActor_Surprise(0xe, 0x102);
    return 0;
}

/* --- OvlFunc_973_200804c ------------------------------------------------- */
extern void __MessageID(int id);
extern void __MapActor_Emote(int slot, int effect, int arg);
extern void __ActorMessage(int slot, int arg);

void OvlFunc_973_200804c(void)
{
    __MessageID(0x23cd);
    __MapActor_Emote(0xd, 0x102, 0);
    __ActorMessage(0xd, 0);
}

/* --- OvlFunc_921_20085dc ------------------------------------------------- */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_8092adc(int slot, int angle, int speed);

void OvlFunc_921_20085dc(void)
{
    __CutsceneStart();
    __MessageID(0x156d);
    __ActorMessage(8, 0);
    __Func_8092adc(8, 0x3000, 0xa);
    __CutsceneEnd();
}
