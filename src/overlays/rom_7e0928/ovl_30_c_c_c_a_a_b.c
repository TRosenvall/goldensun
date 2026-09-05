// fakematch
/* OvlFunc_956_2008c5c  --  0x02008c5c
 *   [asm/overlays/rom_7e0928/ovl_30_c_c_c_a_a.s, 2nd of 2 -- SPLIT NEEDED]
 *
 * A TWIN, FOUND BY tools/twins.py.  Identical opcode sequence to the solved
 * OvlFunc_954_2008840 (src/overlays/rom_7db0c8/ovl_30_c_c_a_c_a.c) across all
 * 106 instructions.  Written by copying that body and changing ONLY the eight
 * operands the two references disagree on:
 *
 *   OvlFunc_common1_1814   0x7f, 0x78  ->  0x11, 0x3
 *   __Func_809218c  #1     0xa5        ->  0xbf
 *   __Func_80921c4  #1     0xa1        ->  0xbb
 *   __Func_809218c  #2     0xa2        ->  0xbc
 *   __Func_80921c4  #2     0xa4        ->  0xbe
 *   the r1 scratch in the __Func_8091eb0 block   0 -> 4
 *   _AREA_8f       ->  _AREA_91   (new area.sym entry, = 0x91)
 *
 * THE AREA SYMBOL IS NOT A GUESS.  The ROM POOLS 0x91 where an eight-bit `mov`
 * would do -- 0x91 is under 256, so gcc would build it with a bare `mov` given
 * a literal -- and that pooling-where-a-mov-would-do is the recorded tell for a
 * link-time symbol.  The twin carries exactly the same shape one area earlier
 * with `_AREA_8f`, which is already in area.sym, so this is the same idiom at
 * the next id rather than a new hypothesis.
 *
 * Everything else is inherited unchanged: the r5/r6 named locals, the pinned
 * fills, the `do { } while (0)` barrier and the gState pointer block.  Those
 * were measured on the twin's SHAPE, and the shape is identical here -- but the
 * candidate was screened under objcmp rather than assumed, since a sibling's
 * cure can be actively wrong when register roles differ.
 */
extern unsigned char gState[];
extern int _AREA_91;
extern int OvlFunc_common1_1814(int a, int b);
extern void OvlFunc_common1_16f8(void);
extern void OvlFunc_common1_1708(void);

extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_WaitScript(int slot);
extern void __SetFlag(int id);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8091eb0(int a, int b);
extern void __Func_8091f90(int a, int b);
extern void __Func_8091fa8(int a, int b);
extern void __Func_8092848(int a, int b, int c);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_956_2008c5c(void)
{
    register int n __asm__("r6");
    register int m __asm__("r5");
    int i;

    OvlFunc_common1_16f8();
    __CutsceneStart();
    { PIN2; q1 = 0x11; q0 = 0x3; n = OvlFunc_common1_1814(q0, q1); }
    OvlFunc_common1_1708();
    i = 9;
    do {
        __MapActor_WaitScript(8);
        i--;
    } while (i >= 0);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 8; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xbf; q0 = 8; q1 <<= 3; q2 = 0xc0; __Func_809218c(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xbb; q2 = 0xc0; q0 = 0; q1 <<= 3; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(8, 1);
    { PIN3; q2 = 0; q1 = 8; q0 = 0; __Func_8092848(q0, q1, q2); }
    __CutsceneWait(0xa);
    __MapActor_SetAnim(8, 3);
    { PIN2; q1 = 3; q0 = 0; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 10; q2 <<= 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 8; q1 <<= 10; q2 <<= 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xbc; q0 = 0; q1 <<= 3; q2 = 0xc0; __Func_809218c(q0, q1, q2); }
    { PIN3; q1 = 0xbe; q2 = 0xc0; q0 = 8; q1 <<= 3; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(0, 0x10);
    { PIN2; q1 = 9; q0 = 8; __MapActor_SetAnim(q0, q1); }
    __CutsceneWait(0xa);
    {
        register int t __asm__("r1");
        t = 4;
        __asm__ volatile ("" : : "r" (t));
        t -= n;
        t += 1;
        __Func_8091eb0(0x48, t);
    }
    {
        register unsigned char *g __asm__("r3");
        register int v __asm__("r2");
        g = gState;
        v = 0x22b;
        g += v;
        v = 3;
        *g = v;
    }
    do { } while (0);
    m = (int)(&_AREA_91);
    { PIN2; q1 = 4; q0 = m; __Func_8091f90(q0, q1); }
    { PIN2; q0 = m; q1 = 5; __Func_8091fa8(q0, q1); }
    __SetFlag(0x8d << 1);
}
