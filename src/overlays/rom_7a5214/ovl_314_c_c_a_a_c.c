/* WHOLE-FILE CONVERSION of asm/overlays/rom_7a5214/ovl_314_c_c_a_a_c.s --
 * OvlFunc_918_2008918 is its only function, so no split and no linker change.
 *
 * 588 instructions, 1600 BYTES, 614 encodings and 168 relocations identical.  THIS
 * IS THE LARGEST FUNCTION ELEVATED IN THIS PROJECT -- batch 280's record was 397
 * instructions and 1148 bytes.
 *
 * SIXTY-THREE PIN BLOCKS, AND THAT COUNT IS MEASURED RATHER THAN ASSERTED.  A script
 * that mechanically folds every { PIN...; q0 = ...; call(q...) } block back to a
 * plain literal call gives 586 of 614 differing and +32 bytes.  So the pins are
 * carrying the function, not decorating it -- batch 280's reading that the pin count
 * scales with a cutscene script's repetition, confirmed at a larger size.
 *
 * A PINNED FILL'S SHIFT PLACEMENT DECIDES sched2's mov ORDER, AND "ALL MOVS THEN ALL
 * SHIFTS" IS NOT THE RULE.  `q0 = 0xa8 << 16;` AS ONE EXPRESSION puts `mov r0` FIRST
 * and took this function from 6 encodings to EXACT; splitting it as
 * `q0 = 0xa8; ... q0 <<= 16;` puts it third.  Six permutations measured, and
 * `q1 = -1` against `q1 = 1; q1 = -q1;` was worth 2.  The companion instance in
 * src/overlays/rom_7a5214/ovl_314_c_c_a_c.c's park runs the OTHER way -- there the
 * shift must be separated.  The lever is "match the ROM's emission order by moving
 * the shift", and both directions occur.
 *
 * `.L2dd0`, `.L2dc0` and `.L2dcc` are already `.global` in
 * asm/overlays/rom_7a5214/ovl_314_c_c_c_c.s, so the __asm__(".L...") rename works
 * with no label.sym entry and no .s edit.
 *
 * No per-file Makefile flag override applies to this stem (tryc.makefile_flags is
 * empty for it): plain GCC296_CFLAGS at -O2 with -fcall-used-r4.
 *
 * NOTE A BASENAME COLLISION for whoever touches this directory next:
 * ovl_314_c_c_a_c.s exists BOTH here and at asm/overlays/rom_7eaf28/.  Different
 * directories, so the path-based rule is fine, but it is not clean.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gScript_918__02009e54[];
extern unsigned char gScript_918__02009ec8[];
extern int L2dc0[] __asm__(".L2dc0");
extern int L2dcc __asm__(".L2dcc");

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CheckPartyItem(int id);
extern void __PlaySound(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __StartTask(void *f, int n);
extern void __StopTask(void *f);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8091200(int a, int b);
extern void __Func_8091220(int a, int b);
extern void __Func_8091254(int a);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8092c40(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_918_2009424(int a);
extern void OvlFunc_918_200962c(void);
extern void OvlFunc_918_2009244(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_918_2008918(void)
{
    unsigned char *p;
    unsigned char *b;
    unsigned char *sc;
    int f;
    int h;
    int s5;
    void *tf;

    f = __GetFlag(3);
    p = __MapActor_GetActor(3) + 0x23;
    *p = 0xfe & *p;
    __Func_8092b08(3, 2);
    p = __MapActor_GetActor(0) + 0x23;
    *p = 0xfe & *p;
    __Func_8092b08(0, 2);
    __CheckPartyItem(0xb8);
    __PlaySound(0x11);
    __CutsceneStart();
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xa6; q2 = 0xa0; q1 <<= 16; q2 <<= 15; q0 = 0;
      __MapActor_SetPos(q0, q1, q2); }
    h = 0xc0 << 8;
    *(short *)(__MapActor_GetActor(0) + 6) = h;
    { PIN3; q1 = 0x94; q2 = 0xb4; q1 <<= 16; q2 <<= 15; q0 = 1;
      __MapActor_SetPos(q0, q1, q2); }
    *(short *)(__MapActor_GetActor(1) + 6) = h;
    { PIN3; q1 = 0xb6; q2 = 0xb4; q1 <<= 16; q0 = 2; q2 <<= 15;
      __MapActor_SetPos(q0, q1, q2); }
    *(short *)(__MapActor_GetActor(2) + 6) = h;
    if (f != 0) {
        { PIN3; q1 = 0xa6; q2 = 0xd0; q0 = 3; q1 <<= 16; q2 <<= 15;
          __MapActor_SetPos(q0, q1, q2); }
        *(short *)(__MapActor_GetActor(3) + 6) = h;
    }
    OvlFunc_918_2009424(0);
    __WaitFrames(0xa);
    b = iwram_3001ebc;
    *(int *)(b + (0xe0 << 1)) = 0x100;
    *(int *)(b + (0xe4 << 1)) = 0x30;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    __Func_80933d4(0x13333, 0x2666);
    { PIN4; q0 = 0xa8 << 16; q1 = 1; q2 = 0x98; q1 = -q1; q2 <<= 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0xa);
    __PlaySound(0x7b);
    s5 = 0xa;
    __Func_8010704(0x1a, 3, 1, 2, s5, 8);
    __Func_80105d4(0x1a, 0x26, 1, 1, s5, 0x2b);
    __WaitFrames(4);
    __Func_80105d4(0x1a, 0x25, 1, 2, s5, 0x2a);
    __WaitFrames(4);
    __Func_80105d4(0x1a, 0x24, 1, 3, s5, 0x29);
    __WaitFrames(4);
    __Func_80105d4(0x1a, 0x23, 1, 4, s5, 0x28);
    __WaitFrames(0x50);
    __MessageID(0x14d3);
    { PIN3; q2 = 0x14; q0 = 0x8009; q1 = 0; __Func_8093040(q0, q1, q2); }
    __Func_809259c(0, 2);
    __Func_809259c(1, 2);
    __Func_809259c(3, 2);
    { PIN2; q1 = 2; q0 = 2; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN4; q0 = 0xa8 << 16; q1 = 1; q2 = 0xb4; q3 = 1; q1 = -q1; q2 <<= 15;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x28);
    OvlFunc_918_2009424(1);
    __CutsceneWait(0x3c);
    __PlaySound(0x15);
    OvlFunc_918_2009424(4);
    { PIN3; q0 = 0x8009; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q2 = 0x50; q0 = 2; q1 = 0x101; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 0x8009; __ActorMessage(q0, q1); }
    __CutsceneWait(0x28);
    { PIN3; q2 = 0x14; q0 = 0x8009; q1 = 0; __Func_8093040(q0, q1, q2); }
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(3, 3);
    { PIN2; q1 = 3; q0 = 2; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q1 = 0; q2 = 0x14; q0 = 0x8009; __Func_8093040(q0, q1, q2); }
    OvlFunc_918_2009424(0);
    __CutsceneWait(0x28);
    { PIN3; q2 = 0x14; q0 = 0x8009; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 0x81; q0 = 0; q1 <<= 1; __MapActor_Surprise(q0, q1); }
    { PIN2; q1 = 0x81; q0 = 1; q1 <<= 1; __MapActor_Surprise(q0, q1); }
    { PIN2; q1 = 0x81; q0 = 3; q1 <<= 1; __MapActor_Surprise(q0, q1); }
    { PIN2; q1 = 0x81; q1 <<= 1; q0 = 2; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    __Func_80925cc(1, 2);
    { PIN3; q1 = 0xe0; q0 = 1; q1 <<= 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q0 = 0; q1 <<= 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0xa; q0 = 0x8001; q1 = 0; __Func_8093040(q0, q1, q2); }
    __MapActor_DoAnim(2, 4);
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 2; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0; q2 = 0x14; q0 = 0x8002; __Func_8093040(q0, q1, q2); }
    OvlFunc_918_2009424(0);
    __CutsceneWait(0x28);
    { PIN3; q2 = 0xa; q0 = 0x8009; q1 = 0; __Func_8093040(q0, q1, q2); }
    __Func_809259c(0, 2);
    __Func_809259c(1, 2);
    __Func_809259c(3, 2);
    __Func_80925cc(2, 2);
    { PIN3; q0 = 0; q1 = h; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = h; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0x28; q1 = h; q0 = 2; __Func_8092adc(q0, q1, q2); }
    OvlFunc_918_2009424(4);
    { PIN2; q1 = 0; q0 = 0x8009; __Func_8092c40(q0, q1); }
    { PIN3; q1 = 0xe0; q0 = 1; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 2; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    if (__Func_8091c7c(0, 0) != 0) {
        { PIN3; q2 = 0x14; q0 = 1; q1 = 0x103; __MapActor_Emote(q0, q1, q2); }
        { PIN2; q1 = 4; q0 = 1; __MapActor_SetAnim(q0, q1); }
        __MessageID(0x14dd);
        __ActorMessage(0x8001, 0);
        { PIN3; q0 = 2; q1 = 0x103; q2 = 0xa; __MapActor_Emote(q0, q1, q2); }
        __MapActor_SetAnim(2, 3);
        __ActorMessage(0x8002, 0);
    }
    __CutsceneWait(0x14);
    OvlFunc_918_2009424(4);
    __MessageID(0x14df);
    { PIN3; q0 = 0x8009; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q2 = 0xa; q1 = 0; q0 = 0x8009; __Func_8093040(q0, q1, q2); }
    OvlFunc_918_2009424(0);
    __CutsceneWait(0x14);
    { PIN2; q0 = 0x80; q0 <<= 9; q1 = 0; __Func_8091220(q0, q1); }
    { PIN2; q1 = 1; q0 = 0x406218; __Func_8091200(q0, q1); }
    __Func_8091254(0x14);
    __WaitFrames(0x28);
    __Func_809259c(0, 2);
    __Func_809259c(1, 2);
    __Func_809259c(3, 2);
    __Func_80925cc(2, 2);
    { PIN3; q0 = 1; q1 = h; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = h; q2 = 0x14; q0 = 2; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    L2dcc = 0;
    L2dc0[0] = 0xa8 << 16;
    L2dc0[1] = 0x80 << 14;
    L2dc0[2] = 0xd0 << 14;
    tf = OvlFunc_918_200962c;
    { PIN2; q1 = 0xc8; q1 <<= 4; q0 = (int)tf;
      __StartTask((void *)q0, q1); }
    __CutsceneWait(0xdc);
    __StopTask(tf);
    { PIN2; q0 = 0x80; q1 = 1; q0 <<= 9; __Func_8091200(q0, q1); }
    __Func_8091254(0x14);
    __WaitFrames(0x28);
    OvlFunc_918_2009424(4);
    __CutsceneWait(0x14);
    { PIN3; q2 = 0xa; q1 = 0; q0 = 0x8009; __Func_8093040(q0, q1, q2); }
    OvlFunc_918_2009424(0);
    __ActorMessage(0x8009, 0);
    __MapActor_RunScript(8, gScript_918__02009e54);
    __CutsceneWait(0x28);
    { PIN3; q1 = 0x81; q2 = 0x3c; q0 = 1; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q0 = 0x8001; q1 = 0; __ActorMessage(q0, q1); }
    { PIN3; q1 = 0x81; q2 = 0xa; q0 = 2; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q0 = 0x8002; q1 = 0; __ActorMessage(q0, q1); }
    { PIN3; q1 = 0xe0; q0 = 1; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 2; q1 <<= 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0xa; q0 = 0; q1 <<= 7; __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(1, 1);
    { PIN3; q2 = 0xa; q0 = 0x8001; q1 = 0; __Func_8093040(q0, q1, q2); }
    __Func_80925cc(2, 1);
    { PIN3; q0 = 0x8002; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    if (f != 0) {
        __Func_80925cc(3, 1);
        { PIN3; q0 = 0x8003; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    }
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(3, 3);
    __MapActor_DoAnim(2, 3);
    sc = gScript_918__02009ec8;
    __MapActor_SetBehavior(1, sc);
    if (f != 0)
        __MapActor_SetBehavior(3, sc);
    { PIN2; q1 = (int)sc; q0 = 2; __MapActor_RunScript(q0, (unsigned char *)q1); }
    __CutsceneWait(0x14);
    p = __MapActor_GetActor(0) + 0x23;
    *p = 1 | *p;
    __SetFlag(0x844);
    { PIN2; q1 = 0xc8; q1 <<= 4; q0 = (int)OvlFunc_918_2009244;
      __StartTask((void *)q0, q1); }
    __CutsceneEnd();
}
