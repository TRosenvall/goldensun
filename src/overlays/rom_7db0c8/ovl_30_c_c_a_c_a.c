// fakematch
/* OvlFunc_954_2008840  --  0x02008840
 *
 * Was the whole of goldensun/asm/overlays/rom_7db0c8/ovl_30_c_c_a_c_a.s;
 * split_s.py confirmed one function and no data tail.
 *
 * 107 instructions. Sixteen pinned call sites and three levers, two of which
 * are refinements worth keeping.
 *
 * gcc FOLDS `0 - n` INTO A NEG, AND A BARRIER STOPS IT. The ROM computes an
 * argument as `mov r1, #0 / sub r1, r6 / add r1, #1` -- three instructions for
 * what is arithmetically `1 - n`. Writing `1 - n` gives two (`mov r1, #1 /
 * sub`); writing `(0 - n) + 1` gives the same two; writing it as three named
 * statements `t = 0; t -= n; t += 1;` gives `neg r1, r6 / add r1, #1`, still
 * two, because gcc folds the zero-minus into a negation. A volatile asm after
 * `t = 0` forces the zero to be materialised and the three-instruction form
 * appears. THE BARRIER IS NOT ONLY A SCHEDULING TOOL -- here it blocks a
 * PEEPHOLE, by making the intermediate value observable.
 *
 * AN AREA ID POOLED WHERE AN IMMEDIATE FITS, again. `ldr r5, =0x8f` against a
 * `mov` is the tell recorded in
 * src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_c.c last batch; `area.sym` has
 * `_AREA_8f = 0x8f` and `(int)(&_AREA_8f)` is exact. Second function in two
 * batches, so the check is worth making routine: a pooled constant under 0x100
 * means look in the .sym files.
 *
 * Its pinned load then hoisted past the gState poke and took one
 * `do { } while (0)` behind it.
 *
 * THE COUNTED LOOP DECREMENTS AFTER THE CALL. The ROM emits
 * `mov r0, #8 / sub r5, #1 / bl`, which reads as a decrement before the call,
 * and writing it that way puts the `sub` first. Writing the decrement AFTER the
 * call is exact -- the iteration count is identical either way since the
 * counter is dead outside the loop, and only the emitted order differs. Pinning
 * r0 and barriering it also works but is scaffolding this does not need.
 */
extern unsigned char gState[];
extern int _AREA_8f;
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

void OvlFunc_954_2008840(void)
{
    register int n __asm__("r6");
    register int m __asm__("r5");
    int i;

    OvlFunc_common1_16f8();
    __CutsceneStart();
    { PIN2; q1 = 0x7f; q0 = 0x78; n = OvlFunc_common1_1814(q0, q1); }
    OvlFunc_common1_1708();
    i = 9;
    do {
        __MapActor_WaitScript(8);
        i--;
    } while (i >= 0);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 8; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xa5; q0 = 8; q1 <<= 3; q2 = 0xc0; __Func_809218c(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xa1; q2 = 0xc0; q0 = 0; q1 <<= 3; __Func_80921c4(q0, q1, q2); }
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
    { PIN3; q1 = 0xa2; q0 = 0; q1 <<= 3; q2 = 0xc0; __Func_809218c(q0, q1, q2); }
    { PIN3; q1 = 0xa4; q2 = 0xc0; q0 = 8; q1 <<= 3; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(0, 0x10);
    { PIN2; q1 = 9; q0 = 8; __MapActor_SetAnim(q0, q1); }
    __CutsceneWait(0xa);
    {
        register int t __asm__("r1");
        t = 0;
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
    m = (int)(&_AREA_8f);
    { PIN2; q1 = 4; q0 = m; __Func_8091f90(q0, q1); }
    { PIN2; q0 = m; q1 = 5; __Func_8091fa8(q0, q1); }
    __SetFlag(0x8d << 1);
}
