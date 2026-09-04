// fakematch
/* OvlFunc_949_2008894  --  0x02008894
 *
 * Cut out of goldensun/asm/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_c_c_c_c_c_c.s.
 *
 * A cutscene gated on __Func_8091c7c: on the zero arm it bumps a counter, walks
 * an actor through three positions and sets a flag; on the other it only speaks.
 *
 * MATCHED ON THE FIRST SCREEN, which is the point worth recording. This was
 * chosen by tools/templated.py -- 0.75 over twelve shared symbols, zero r8-r11
 * traffic -- and every construct it needed was already known:
 *
 *   - SIX CALLS WANT AN INTERLEAVED ARGUMENT FILL, and each wants a DIFFERENT
 *     one. The three shapes present here are
 *         mov r1 / mov r2 / mov r0 / lsl r1 / lsl r2
 *         mov r1 / mov r0 / lsl r1 / mov r2
 *         mov r1 / lsl r1 / mov r2 / mov r0
 *     Pinning r0-r2 and writing each call's assignments in that call's own ROM
 *     order reaches all three. Nothing else in the file changes between them,
 *     so this is the pin's second knob doing the whole job six times over.
 *
 *   - __Func_8092c40 HAS NO DECLARATION. The ROM fills its arguments r1 before
 *     r0, which is the implicitly-declared order; a prototype reverses them.
 *     Taken from src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_c_c_a_a.c, where the
 *     same callee needed the same absence. `extern int f();` is NOT equivalent
 *     -- it behaves as a full prototype. The absence is the lever.
 *
 *   - THE GUARD IS WRITTEN == 0 WITH THE LONG ARM AS THE `if` BODY, which puts
 *     the ROM's `bne` on the short arm. The counter bump through
 *     iwram_3001ebc + 0xec * 2 is the same idiom as the file above.
 *
 * Six interleaves, three distinct shapes, one screen. The lever is no longer
 * being discovered; it is being spent.
 */

extern int iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __ActorMessage(int actor, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_949_2008894(void)
{
    char *p;
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");

    __CutsceneStart();
    p1 = 0x98; p2 = 0x9c; p0 = 0; p1 <<= 1; p2 <<= 1;
    __Func_80921c4(p0, p1, p2);
    p1 = 0xc0; p0 = 0; p1 <<= 8; p2 = 0;
    __Func_8092adc(p0, p1, p2);
    p1 = 0x80; p1 <<= 7; p2 = 0; p0 = 0x1c;
    __Func_8092adc(p0, p1, p2);
    __CutsceneWait(0x14);
    __MessageID(0xe3d);
    __Func_8092c40(0x1c, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        p = (char *)iwram_3001ebc;
        *(unsigned short *)(p + (0xec << 1)) += 1;
        __ActorMessage(0x1c, 0);
        p1 = 0x80; p2 = 0x80; p0 = 0x1c; p1 <<= 9; p2 <<= 8;
        __MapActor_SetSpeed(p0, p1, p2);
        p1 = 0xa0; p2 = 0x98; p0 = 0x1c; p1 <<= 1; p2 <<= 1;
        __Func_80921c4(p0, p1, p2);
        p1 = 0x9e; p2 = 0xa4; p0 = 0x1c; p1 <<= 1; p2 <<= 1;
        __Func_80921c4(p0, p1, p2);
        p1 = 0xa0; p0 = 0x1c; p1 <<= 8; p2 = 0;
        __Func_8092adc(p0, p1, p2);
        __SetFlag(0x8c1);
    } else {
        __ActorMessage(0x1c, 0);
    }
    __CutsceneEnd();
}
