/* Cluster OvlFunc_931_2008448..OvlFunc_931_2008448 extracted from goldensun/asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_a_c_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * The third member of the three-way-talk family in this overlay, after
 * ovl_30_c_c_c_c_c_c_c_c_c_a_a_b.c and ..._a_a_c_b.c. Same structure: a flag
 * guard, a hand-off to __Func_80b0278, and a two-line arm.
 *
 * NOTE WHERE THE SECOND `if` ENDS -- flag 0x909 guards only the extra
 * __MessageID, and the __ActorMessage after it runs either way. That is the
 * trap that parked the first member of this family for several rounds, and it
 * is now the third time it has come up in the same overlay.
 *
 * The actor changes between arms -- 0x15 in the first, 0x10 in the third -- and
 * __Func_80b0278 takes both. That is not a transcription slip; the ROM has it.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __Func_80b0278(int a, int b);
extern int OvlFunc_931_2008338(void);

void OvlFunc_931_2008448(void)
{
    if (!__GetFlag(0x90 << 2)) {
        __CutsceneStart();
        __MessageID(0x18f1);
        __ActorMessage(0x15, 0);
        __CutsceneEnd();
    } else if (OvlFunc_931_2008338()) {
        __Func_80b0278(0x15, 0x10);
    } else {
        __CutsceneStart();
        __MessageID(0x18f2);
        if (__GetFlag(0x909))
            __MessageID(0x1945);
        __ActorMessage(0x10, 0);
        __CutsceneEnd();
    }
}
