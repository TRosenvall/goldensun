// fakematch
/* OvlFunc_955_2008a1c  --  0x02008a1c
 *
 * Was the whole of goldensun/asm/overlays/rom_7ddb88/ovl_30_c_c_c_a_c_c_c_c_c_c_c_c_c.s;
 * split_s.py confirmed one function and no data tail.
 *
 * A near-exact twin of src/overlays/rom_7db0c8/ovl_30_c_c_a_c_a.c
 * (OvlFunc_954_2008840), elevated the same round: same 107 instructions, same
 * structure, five differing constants and four call fills whose register order
 * differs. Written from that file with substitutions, which is what a twin is
 * for -- but the four fills were still transcribed from THIS listing, because
 * the sibling fills r1 before r2 where this one fills r2 first at every site.
 *
 * THE BARRIER BLOCKS A CONSTANT FOLD, and this twin sharpens why. The sibling
 * computes an argument as `mov r1, #0 / sub r1, r6 / add r1, #1`, where the
 * leading zero makes gcc fold `0 - n` to a `neg`. Here the ROM's leading value
 * is 2, not 0, so that particular fold cannot apply -- and gcc folds a DIFFERENT
 * way, collapsing `t = 2; t -= n; t += 1;` into `mov r1, #3 / sub r1, r6` by
 * moving the increment into the constant.
 *
 * So the two twins fold for two different reasons and BOTH need the same cure:
 * a volatile asm after the first assignment, which makes the intermediate
 * observable and forces all three instructions. The lever is not "block the
 * neg" -- it is "make the intermediate real", and it covers any peephole that
 * would collapse a chain of arithmetic on one variable.
 */
extern unsigned char gState[];
extern int _AREA_90;
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

void OvlFunc_955_2008a1c(void)
{
    register int n __asm__("r6");
    register int m __asm__("r5");
    int i;

    OvlFunc_common1_16f8();
    __CutsceneStart();
    { PIN2; q1 = 0x59; q0 = 0x4d; n = OvlFunc_common1_1814(q0, q1); }
    OvlFunc_common1_1708();
    i = 9;
    do {
        __MapActor_WaitScript(8);
        i--;
    } while (i >= 0);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 8; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0x80; q0 = 8; q1 = 0x58; q2 <<= 1; __Func_809218c(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0x80; q2 <<= 1; q0 = 0; q1 = 0x78; __Func_80921c4(q0, q1, q2); }
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
    { PIN3; q2 = 0x80; q0 = 0; q1 = 0x70; q2 <<= 1; __Func_809218c(q0, q1, q2); }
    { PIN3; q2 = 0x80; q2 <<= 1; q0 = 8; q1 = 0x60; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(0, 0x10);
    { PIN2; q1 = 9; q0 = 8; __MapActor_SetAnim(q0, q1); }
    __CutsceneWait(0xa);
    {
        register int t __asm__("r1");
        t = 2;
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
    m = (int)(&_AREA_90);
    { PIN2; q1 = 4; q0 = m; __Func_8091f90(q0, q1); }
    { PIN2; q0 = m; q1 = 5; __Func_8091fa8(q0, q1); }
    __SetFlag(0x8d << 1);
}
