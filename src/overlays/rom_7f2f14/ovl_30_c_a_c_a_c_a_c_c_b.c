// fakematch
/* OvlFunc_968_20094f4  --  0x020094f4
 *
 * Cut out of goldensun/asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a_c_c.s.
 *
 * A one-shot cutscene: if actor 0xc is standing on tile 0x35 and flag 0x986 is
 * clear, set the flag and play the whole sequence; otherwise fall straight out.
 *
 * THIS TU NEEDS AN EXPLICIT -O2 RULE IN THE MAKEFILE, and that is the main
 * thing to carry away. It is caught by the mis-scoped
 * `rom_7f2f14/ovl_30_c_a_c_a_c_a%` wildcard, which applies O1_CFLAGS: 14
 * differing at -O1, EXACT at -O2. Two sibling functions in this same overlay
 * already carry the same override for the same reason; this is the third.
 *
 * HOW THAT WAS CAUGHT MATTERS MORE THAN THE FIX. The 14-instruction residue
 * looked like an ordinary scheduling problem and would have absorbed a long
 * sweep of spellings. tools/tryc.py printed the diff and then warned that the
 * flags had come from a WILDCARD rule that "may belong to a neighbouring TU
 * that only shares a name prefix", with the re-screen to run. At -O2 the same
 * unchanged candidate is a clean match. A residue is only evidence about the
 * source once the flags are known to be the right ones.
 *
 * Compiling by hand at plain -O2 without going through tryc gives a FALSE
 * CLEAN here, for the mirror-image reason -- that is the trap tryc's own
 * comments describe, and it is why the screen is the gate rather than a
 * hand-rolled compile.
 *
 * TWO LEVERS, both already on file:
 *
 *   1. THE FLAG ID IS REMATERIALISED. The ROM issues `ldr r0, =0x986` fresh at
 *      __GetFlag and again at __SetFlag; gcc caches it in r5 and widens the
 *      prologue to `push {r5, lr}`. Assigning it to the r0 pin before each
 *      call forces the rebuild -- r0 is call-clobbered, so the value cannot
 *      survive the `bl`. That is the branchless rematerialisation lever from
 *      batch 193, and here it also removes the pushed register.
 *
 *   2. SIX CALLS WANT AN INTERLEAVED ARGUMENT FILL, in three distinct shapes,
 *      including __MapActor_SetSpeed where the ROM sets `mov r0, #1` BEFORE
 *      its two pool loads. Pinning r0-r2 and writing each call's assignments
 *      in that call's own ROM order reaches all of them.
 *
 * The actor is read as `unsigned char *` with raw offsets rather than through
 * include/actor.h, following the neighbour this candidate was ranked against.
 * The two `ldrsh` reads take the register-offset form automatically: Thumb-1
 * has no immediate-offset encoding for a signed halfword load, so no source
 * handle is needed for it.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);

void OvlFunc_968_20094f4(void)
{
    unsigned char *p;
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");

    __CutsceneStart();
    p = __MapActor_GetActor(0xc);
    if (*(int *)(p + 8) >> 20 == 0x35) {
        p0 = 0x986;
        if (__GetFlag(p0) == 0) {
            p0 = 0x986;
            __SetFlag(p0);
            p = __MapActor_GetActor(0);
            if (p != 0)
                __MapActor_SetPos(1, *(int *)(p + 8), *(int *)(p + 0x10));
            p0 = 1;
            p1 = 0xcccc;
            p2 = 0x6666;
            __MapActor_SetSpeed(p0, p1, p2);
            p1 = 0xce; p0 = 1; p1 <<= 2; p2 = 0x58;
            __Func_80921c4(p0, p1, p2);
            p1 = 0xce; p0 = 1; p1 <<= 2; p2 = 0x68;
            __Func_80921c4(p0, p1, p2);
            p2 = 0; p1 = 0; p0 = 1;
            __Func_8092848(p0, p1, p2);
            __CutsceneWait(0x14);
            __MapActor_DoAnim(1, 4);
            __CutsceneWait(0x14);
            __MessageID(0x2691);
            __Func_8093040(1, 0, 0x14);
            __Func_8092adc(1, 0, 0xa);
            p1 = 0x80; p0 = 1; p1 <<= 1; p2 = 0x3c;
            __MapActor_Emote(p0, p1, p2);
            p2 = 0; p1 = 0; p0 = 1;
            __Func_809280c(p0, p1, p2);
            __CutsceneWait(0x14);
            __Func_80925cc(1, 2);
            __CutsceneWait(0x14);
            p2 = 0x14; p0 = 1; p1 = 0;
            __Func_8093040(p0, p1, p2);
            __MapActor_SetAnim(0, 3);
            __MapActor_DoAnim(1, 3);
            __CutsceneWait(0x1e);
            p1 = 0xce; p0 = 1; p1 <<= 2; p2 = 0x58;
            __Func_80921c4(p0, p1, p2);
            __MapActor_SetAnim(1, 2);
            p = __MapActor_GetActor(0);
            if (p != 0)
                __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
            __MapActor_WaitMovement(1);
            __MapActor_SetPos(1, 0, 0);
            __CutsceneEnd();
        }
    }
}
