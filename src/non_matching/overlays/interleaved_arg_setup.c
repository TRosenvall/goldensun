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
 *   * gcc-2.96 DOES emit this interleaving from C. Scanning the 2169
 *     generated .s files in this tree for the pattern -- a `mov rX, #imm`
 *     and its `lsl rX` split by another `mov rY, #imm` -- finds 17 files
 *     where it happens, e.g.
 *
 *         mov r0, #128 / mov r1, #128 / lsl r0, r0, #10
 *
 *     So the shape is reachable and this is not a fakematch. Something about
 *     those 17 call sites differs from the plain two-literal call written
 *     below, and finding it unlocks all three of these at once -- probably
 *     many more, since the overlay corpus is full of formulaic talk and
 *     cutscene sequences built from exactly this pattern.
 *
 * That scan reads compiler OUTPUT, not anyone's source; the clean-room rule in
 * docs/attribution.md is intact.
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
