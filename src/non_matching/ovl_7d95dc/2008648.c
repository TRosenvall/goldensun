/* OvlFunc_953_2008648 -- asm/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_a_c_c_a.s
 *
 * BLOCKER: ARGUMENT FILL ORDER. 6 of 43, LENGTH EXACT.
 *
 * SCREENED AS A PREDICTION, NOT AS A SEARCH. Before writing it, the argument-
 * temporary boundary predicted the exact residue: three `__Func_8092adc` call
 * sites each carry the split-constant interleave
 *
 *     rom   mov r1,#K / mov r0,#0x12 / lsl r1,#8
 *     ours  mov r1,#K / lsl r1,#8    / mov r0,#0x12
 *
 * at two differing lines apiece, so six. The first screen returned 6 of 43,
 * length exact, everything else -- all thirteen calls, both `__Func_8092848`
 * sites, the message, the anim and the epilogue -- correct.
 *
 * That is the point of this file. The boundary is no longer just an
 * explanation of past failures; it predicted a number in advance and the
 * number came back. Two differing lines per interleave site is now a usable
 * estimate, and tools/pickable.py prints it as a floor for each candidate.
 *
 * NOT RE-TRIED HERE. The naming spellings were exhausted on
 * ovl_780898/2008fec.c in the same round -- four variants, all byte-identical.
 * Re-running them on a third function would measure the same thing again.
 *
 * ONE INCIDENTAL OBSERVATION worth keeping. The two `__Func_8092848(0x12, 0,
 * 0x14)` calls -- identical arguments, identical callee -- are filled in
 * DIFFERENT orders by the ROM itself:
 *
 *     first    mov r1,#0 / mov r2,#0x14 / mov r0,#0x12
 *     second   mov r2,#0x14 / mov r1,#0 / mov r0,#0x12
 *
 * Both reproduce. So fill order is not a property of the callee or of the
 * argument values; it is decided per site by surrounding context. That is
 * consistent with it being scheduling over rematerialised temporaries, and it
 * is why no per-call-site spelling controls it.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_DoAnim(int slot, int anim);
extern void OvlFunc_953_2009c48(int a);
extern void OvlFunc_953_2009c5c(int a, int b);

void OvlFunc_953_2008648(void)
{
    __CutsceneStart();
    __Func_8092848(0x12, 0, 0x14);
    __MessageID(0x2122);
    OvlFunc_953_2009c48(0x12);
    __Func_8092adc(0x12, 0xd0 << 8, 0x14);
    __Func_8092adc(0x12, 0xb0 << 8, 0x14);
    __Func_8092adc(0x12, 0x80 << 8, 0x28);
    __Func_8092848(0x12, 0, 0x14);
    OvlFunc_953_2009c48(0x12);
    __MapActor_DoAnim(0x12, 3);
    OvlFunc_953_2009c48(0x12);
    OvlFunc_953_2009c5c(0x12, 0xa0 << 7);
    __CutsceneEnd();
}
