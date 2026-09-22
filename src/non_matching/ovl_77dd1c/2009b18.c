/* OvlFunc_882_2009b18 -- NON-MATCHING, 2 ENCODINGS OF 545.  Size equal.
 *
 * Blocker class: sched2 LUID TIE on a call-argument copy -- pre-reload scheduling,
 * which no source construct in this idiom reaches.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_77dd1c/2009b18.c \
 *     asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_a_c_c_a.s
 * ONE function in the reference -- it CONVERTS WHOLE, no split.  59 pin sites, so
 * one fakematch row if it lands.
 *
 * THE RESIDUE, one pair at the first __StartTask:
 *
 *     rom   lsl r1,#0x4 / str r6,[r5] / mov r0,r7 / mov r9,r2 / bl __StartTask
 *     ours  lsl r1,#0x4 / mov r0,r7 / str r6,[r5] / mov r9,r2 / bl __StartTask
 *
 * Both insns are class-3 against the last-scheduled insn and BOTH HAVE EXACTLY ONE
 * DEPENDENT (the call), so the tie falls to INSN_LUID -- and sched1 has already
 * placed the argument copy above the store.
 *
 * THE rank_for_schedule TIE-BREAK CHAIN, for this whole class: priority ->
 * (pre-reload only) reg-weight -> class relative to the last-scheduled insn ->
 * NUMBER OF DEPENDENT INSNS -> INSN_LUID.  Post-reload, ties between two
 * argument-setup insns fall through to LUID, which is exactly why source reordering
 * sometimes works and sometimes cannot.
 *
 * MEASURED, all inert at 2: the store inside the pin block (3 placements), a
 * comma-expression in either argument, a `volatile` store, `q1 = 0xc8 << 4` folded,
 * a plain unpinned argument, a local function-pointer with q0 pinned.  WORSE:
 * swapped store order 6, do{}while(0) 12, a "memory" clobber 14.
 *
 * UN-PINNING IS A LEVER AT A SPECIFIC SITE, AGAIN.  This function's
 * __MapActor_SetSpeed(0x16, ...) needed q2 LEFT UNPINNED
 * (`__MapActor_SetSpeed(q0, q1, 0x80 << 9)`): 4 -> 2.  Six pinned spellings of that
 * site all sat at 4 or 6.
 *
 * ONE THING THAT DID REACH A SIBLING'S ANALOGOUS SITE AND NOT THIS ONE: the
 * `__asm__ volatile ("" : : "r" (q0));` barrier after the first pinned assignment,
 * which took src/overlays/rom_7ac2d8/ovl_22c4_c_c_c_c_c.c to exact.  Here a
 * 3-position x 7-spelling sweep of it found nothing.  Worth knowing that the
 * barrier lever is site-specific rather than general.
 *
 * NO ALIAS_CFLAGS ROW IS NEEDED FOR THIS OVERLAY.  The known-open item for
 * OvlFunc_882_200c41c did not recur -- neither this function nor
 * src/non_matching/ovl_77dd1c/2008434.c shows a pointer-reload difference.
 *
 * No .sym entry is implied; no constant here has the in-function control the bar
 * requires.  No per-file Makefile flag override applies to this stem.
 */
extern unsigned char L54b0[] __asm__(".L54b0");
extern int L57f8 __asm__(".L57f8");
extern int L57fc __asm__(".L57fc");
extern unsigned char gScript_882__0200ca00[];
extern unsigned char gScript_882__0200ca3c[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __LoadFieldActors(unsigned char *p);
extern void __DeleteFieldActor(int slot);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_WaitScript(int slot);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __StartTask(void (*f)(void), int n);
extern void __StopTask(void (*f)(void));
extern void __Func_8012330(int a, int b, int c);
extern void __Func_809202c(void);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_882_200c2bc(unsigned char *a);
extern void OvlFunc_882_200c550(void);
extern void OvlFunc_882_200c560(void);
extern void OvlFunc_882_200c56c(void);
extern void OvlFunc_882_200c5a8(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")
#define PINR1 register int q1 __asm__("r1")

void OvlFunc_882_2009b18(void)
{
    unsigned char *a;
    unsigned int i;
    int v;

    if (__GetFlag(0x838) == 0) {
        __CutsceneStart();
        __LoadFieldActors(L54b0);
        OvlFunc_882_200c550();
        __WaitFrames(1);
        __PlaySound(0x8d);
        { PIN3; q0 = 0x80 << 10; q1 = 0x80; q2 = 0x80; q1 <<= 10; q2 <<= 9;
          __Func_8012330(q0, q1, q2); }
        __CutsceneWait(0x1e);
        { PIN3; q0 = 0xc0 << 10; q1 = 0xc0; q2 = 0x80; q1 <<= 10; q2 <<= 9;
          __Func_8012330(q0, q1, q2); }
        __PlaySound(0x91);
        __CutsceneWait(0x1e);
        a = __MapActor_GetActor(0);
        if (a != 0)
            __MapActor_SetPos(0x16, *(int *)(a + 8), *(int *)(a + 0x10));
        { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 10; q2 <<= 9;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN2; q1 = 0x80; q0 = 0x16; q1 <<= 10;
          __MapActor_SetSpeed(q0, q1, 0x80 << 9); }
        __MapActor_SetBehavior(0, gScript_882__0200ca00);
        __MapActor_RunScript(0x16, gScript_882__0200ca3c);
        __MapActor_WaitScript(0);
        { PIN3; q1 = 0x80; q0 = 0; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
        { PIN3; q1 = 0x80; q0 = 0x16; q1 <<= 1; q2 = 0x1e; __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0x80 << 11; q1 = 0x80; q2 = 0x80; q1 <<= 11; q2 <<= 9;
          __Func_8012330(q0, q1, q2); }
        __PlaySound(0x91);
        __CutsceneWait(0x28);
        { PIN3; q0 = 0xa0 << 11; q1 = 0xa0 << 11; q2 = 0x80; q2 <<= 9;
          __Func_8012330(q0, q1, q2); }
        __PlaySound(0x91);
        __CutsceneWait(0x14);
        { PIN2; q1 = 0x81; q0 = 0; q1 <<= 1; __MapActor_Surprise(q0, q1); }
        { PIN2; q1 = 0x81; q1 <<= 1; q0 = 0x16; __MapActor_Surprise(q0, q1); }
        __CutsceneWait(0x28);
        __MapActor_SetAnim(0x20, 5);
        __MapActor_SetAnim(0x21, 5);
        __MapActor_SetAnim(0x1e, 8);
        { PIN2; q1 = 8; q0 = 0x1d; __MapActor_SetAnim(q0, q1); }
        *(int *)(__MapActor_GetActor(0x1e) + 0x18) = 0xffff0000;
        __Func_8092b08(0x20, 2);
        __Func_8092b08(0x21, 2);
        __Func_8092b08(0x1e, 3);
        { PIN2; q1 = 3; q0 = 0x1d; __Func_8092b08(q0, q1); }
        __MessageID(0xe7f);
        __Func_8093040(0x1c, 0, 0x14);
        { PIN3; q1 = 0xc0; q0 = 0; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xc0; q2 = 0x14; q0 = 0x16; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
        { PIN2; q0 = 0x80; q1 = 0x80; q0 <<= 11; q1 <<= 8; __Func_80933d4(q0, q1); }
        { PIN4; q0 = 0xe0; q1 = 1; q0 <<= 15; q1 = -q1; q2 = 0x14b0000; q3 = 1;
          __Func_80933f8(q0, q1, q2, q3); }
        __Func_8093530();
        i = 0;
        do {
            OvlFunc_882_200c2bc(__MapActor_GetActor(0x20));
            OvlFunc_882_200c2bc(__MapActor_GetActor(0x21));
            OvlFunc_882_200c2bc(__MapActor_GetActor(0x1e));
            OvlFunc_882_200c2bc(__MapActor_GetActor(0x1d));
            i++;
            __WaitFrames(1);
        } while (i <= 0x27);
        L57f8 = 0;
        L57fc = 0;
        { PINR1; q1 = 0xc8; q1 <<= 4; __StartTask(OvlFunc_882_200c56c, q1); }
        { PINR1; q1 = 0xc8; q1 <<= 4; __StartTask(OvlFunc_882_200c5a8, q1); }
        __CutsceneWait(0x28);
        L57fc = 1;
        __CutsceneWait(0x1e);
        { PIN3; q1 = 0xe4; q2 = 0x91; q1 <<= 15; q2 <<= 17; q0 = 0x13;
          __MapActor_SetPos(q0, q1, q2); }
        a = __MapActor_GetActor(0x13);
        v = *(int *)(a + 0xc) + (0x80 << 15);
        *(int *)(a + 0xc) = v;
        *(int *)(a + 0x3c) = v;
        { PIN3; q1 = 0xcccc; q2 = 0x6666; q0 = 0x13; __MapActor_SetSpeed(q0, q1, q2); }
        __PlaySound(0x91);
        { PIN3; q2 = 0x14d; q0 = 0x13; q1 = 0x72; __Func_8092158(q0, q1, q2); }
        __MapActor_SetAnim(0x13, 2);
        { PIN3; q0 = 0x80 << 10; q1 = 0x80; q2 = 0x80; q1 <<= 10; q2 <<= 9;
          __Func_8012330(q0, q1, q2); }
        __PlaySound(0x91);
        L57fc = 0;
        { PIN3; q0 = 0x13; q1 = 0x6666; q2 = 0x3333; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q2 = 0x96; q2 <<= 1; q0 = 0x13; q1 = 0x72; __Func_8092158(q0, q1, q2); }
        __MapActor_SetAnim(0x13, 2);
        { PIN3; q0 = 0xa0 << 11; q1 = 0xa0; q2 = 0x80; q1 <<= 11; q2 <<= 9;
          __Func_8012330(q0, q1, q2); }
        __PlaySound(0x91);
        L57fc = 2;
        { PIN3; q0 = 0x13; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q2 = 0x14d; q0 = 0x13; q1 = 0x72; __Func_8092158(q0, q1, q2); }
        __MapActor_SetAnim(0x13, 2);
        { PIN3; q0 = 0x80 << 10; q1 = 0x80; q2 = 0x80; q1 <<= 10; q2 <<= 9;
          __Func_8012330(q0, q1, q2); }
        __PlaySound(0x91);
        L57fc = 0;
        { PIN3; q0 = 0x13; q1 = 0x6666; q2 = 0x3333; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q2 = 0x96; q2 <<= 1; q0 = 0x13; q1 = 0x72; __Func_8092158(q0, q1, q2); }
        __MapActor_SetAnim(0x13, 2);
        { PIN3; q0 = 0x80 << 11; q1 = 0x80; q2 = 0x80; q1 <<= 11; q2 <<= 9;
          __Func_8012330(q0, q1, q2); }
        __PlaySound(0x91);
        L57fc = 2;
        { PIN3; q0 = 0x13; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q2 = 0x14d; q0 = 0x13; q1 = 0x72; __Func_8092158(q0, q1, q2); }
        __MapActor_SetAnim(0x13, 2);
        { PIN3; q0 = 0x80 << 10; q1 = 0x80 << 10; q2 = 0x80; q2 <<= 9;
          __Func_8012330(q0, q1, q2); }
        __PlaySound(0x91);
        L57fc = 1;
        __CutsceneWait(0x14);
        { PIN2; q1 = 0x81; q0 = 0x20; q1 <<= 1; __MapActor_Surprise(q0, q1); }
        __Func_80925cc(0x20, 2);
        __ActorMessage(0x1f, 0);
        { PIN3; q1 = 0x80; q2 = 0; q0 = 0x21; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
        __Func_80925cc(0x21, 2);
        { PIN3; q2 = 0x28; q0 = 0x1c; q1 = 0; __Func_8093040(q0, q1, q2); }
        { PIN2; q1 = 0x81; q0 = 0x1e; q1 <<= 1; __MapActor_Surprise(q0, q1); }
        __Func_80925cc(0x1e, 2);
        __ActorMessage(0x1e, 0);
        L57f8 = 1;
        { PIN2; q1 = 1; q0 = 0x1d; __MapActor_SetAnim(q0, q1); }
        __WaitFrames(1);
        __Func_8092950(0x1d, 0);
        { PIN3; q0 = 0x1d; q1 = 0x105; q2 = 0x14; __MapActor_Emote(q0, q1, q2); }
        { PIN3; q1 = 0x80; q0 = 0x1d; q1 <<= 8; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
        __Func_8092adc(0x1d, 0, 0x14);
        { PIN3; q1 = 0x80; q0 = 0x1d; q1 <<= 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0x80; q0 = 0x1d; q1 <<= 7; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0x80; q2 = 0; q0 = 0x1d; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
        __Func_80925cc(0x1d, 2);
        { PIN3; q2 = 0x28; q0 = 0x1d; q1 = 4; __MapActor_Jump(q0, q1, q2); }
        { PIN2; q1 = 9; q0 = 0x1d; __MapActor_SetAnim(q0, q1); }
        __CutsceneWait(0xa);
        { PIN3; q1 = 0; q2 = 0x14; q0 = 0x1d; __Func_8093040(q0, q1, q2); }
        __PlaySound(0x121);
        { PIN3; q0 = 1; q1 = 1; q2 = 0xe666; q0 = -q0; q1 = -q1;
          __Func_8012330(q0, q1, q2); }
        { PIN2; q0 = 0xc0; q1 = 0xc0; q0 <<= 11; q1 <<= 8; __Func_80933d4(q0, q1); }
        { PIN4; q0 = 0xa8; q1 = 1; q2 = 0x8d; q3 = 1; q0 <<= 15; q1 = -q1; q2 <<= 18;
          __Func_80933f8(q0, q1, q2, q3); }
        __Func_8093530();
        __Func_809202c();
        { PIN3; q2 = 0; q1 = 0; q0 = 0x16; __Func_809280c(q0, q1, q2); }
        __CutsceneWait(0x14);
        { PIN2; q1 = 0x81; q1 <<= 1; q0 = 0x16; __MapActor_Surprise(q0, q1); }
        __CutsceneWait(0x1e);
        __StopTask(OvlFunc_882_200c56c);
        __StopTask(OvlFunc_882_200c5a8);
        __ActorMessage(0x16, 0);
        { PIN3; q2 = 0; q1 = 0x16; q0 = 0; __Func_809280c(q0, q1, q2); }
        __CutsceneWait(0x14);
        OvlFunc_882_200c560();
        __MapActor_SetAnim(0, 3);
        { PIN2; q1 = 3; q0 = 0x16; __MapActor_DoAnim(q0, q1); }
        __CutsceneWait(0x14);
        __MapActor_SetAnim(0x16, 2);
        a = __MapActor_GetActor(0);
        if (a != 0)
            __MapActor_TravelTo(0x16, *(short *)(a + 0xa), *(short *)(a + 0x12));
        __MapActor_WaitMovement(0x16);
        { PIN3; q1 = 0; q2 = 0; q0 = 0x16; __MapActor_SetPos(q0, q1, q2); }
        __DeleteFieldActor(0x1f);
        __DeleteFieldActor(0x1c);
        __DeleteFieldActor(0x1e);
        __DeleteFieldActor(0x1d);
        __DeleteFieldActor(0x20);
        __DeleteFieldActor(0x21);
        __SetFlag(0x838);
        __CutsceneEnd();
    }
}
