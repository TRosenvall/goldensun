/* THREE overlay functions, one shared blocker. A seventh class.
 *
 *   OvlFunc_967_2008030  asm/overlays/rom_7f21b8/ovl_30_a.s
 *   OvlFunc_973_200804c  asm/overlays/rom_7fc720/ovl_30_c_a_c_a_a.s
 *   OvlFunc_921_20085dc  asm/overlays/rom_7a7298/ovl_30_c_c_c_c_a_a.s
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
