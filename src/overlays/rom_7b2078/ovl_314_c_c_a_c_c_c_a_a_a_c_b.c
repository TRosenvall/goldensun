// fakematch
/* OvlFunc_926_200902c  --  0x0200902c
 *
 * Cut out of goldensun/asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_c.s.
 *
 * A branching cutscene: two guarded dialogue arms selected by the parameter,
 * then a shared tail that repositions two actors and pokes four fields of a
 * third. OvlFunc_926_2009160, the other function in the original .s and now
 * ovl_314_c_c_a_c_c_c_a_a_a_c_c.c, shares that whole tail.
 *
 * FOUR LEVERS. The first two are new here.
 *
 * 1. THE MESSAGE BASE IS A PINNED CALLEE-SAVED LOCAL, and this is what the
 *    function is worth reading for. The ROM loads `0x183a` into r5, feeds
 *    __MessageID with `mov r0, r5`, and inside the first arm derives the second
 *    id with `sub r0, r5, #1` -- it never materialises 0x1839 at all. Written as
 *    a plain `int m = 0x183a`, constant propagation folds `m - 1` to its own
 *    pool entry long before liveness is considered, m dies at its first use, and
 *    the parameter takes r5 instead: 104 of 113 differing, and only ONE
 *    callee-saved register pushed where the ROM pushes two. Declaring
 *    `register int m __asm__("r5")` takes it to 25 in one step, and the
 *    parameter moves to r6 on its own. The pin does not fight cprop; it makes
 *    the register assignment a fact cprop has no say in, and the derivation then
 *    falls out of the cost model. THE TELL IS THE PROLOGUE WIDTH: two pushed
 *    registers against our one said a second value had to be named before any
 *    instruction was compared.
 *
 * 2. ONE BARRIER MOVES THE PROBLEM, TWO BARRIERS FIX IT. With m pinned, gcc
 *    scheduled `ldr r5, =0x183a` up into __MapActor_SetSpeed's argument group,
 *    two statements above where the ROM issues it. A `do { } while (0)` before
 *    __CutsceneWait moved it exactly one hole later -- still early, now inside
 *    the wait's own setup -- and a barrier placed AFTER the wait instead fixed
 *    the load but dropped `mov r6, r0` out of the prologue, because it put the
 *    parameter copy in a scheduling region with room to sink. BRACKETING THE
 *    CALL WITH BOTH gives the exact match. A pinned hard register has no data
 *    dependence to hold its assignment down, so the only thing bounding it is
 *    the region walls, and one wall only tells it which way to fall.
 *
 * 3. A SHIFTED CONSTANT HAS TO BE SHIFTED IN THE SOURCE. `0x80 << 8` written
 *    inline became `ldr r3, =0x8000` against the ROM's `mov r3, #0x80 / lsl
 *    r3, #8`; splitting it into `v = 0x80; v <<= 8;` gives the ROM's build.
 *    Writing that as an r3 pin instead is WRONG and cost an attempt -- the
 *    address load lands in r3 as well and overwrites the pinned value, which
 *    tryc shows as `strh r3, [r3, #0x1e]`. Note also that the two neighbouring
 *    stores of `0xc0 << 12` and `0x80 << 24` need none of this and come out as
 *    mov+lsl inline; only the halfword store went to the pool.
 *
 * 4. STORE ADDRESS BEFORE STORE VALUE. Naming the destination pointer so it is
 *    assigned ahead of the value gives the ROM's r2/r3 split; leaving it as a
 *    nested dereference reverses them. Two spellings of that were tried and are
 *    byte-identical, so only the ORDER is real, not the shape.
 *
 * The four repeated __MapActor_GetActor(0x13) calls are four separate statements
 * feeding four separate field stores, exactly as the ROM re-calls it each time.
 * The type on the halfword store must be `unsigned short`: as `short` the
 * constant reaches the pool sign-extended to 0xffff8000.
 */
extern void OvlFunc_926_2008f80(void);

extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MessageID(int id);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);

void OvlFunc_926_200902c(int a)
{
    register int m __asm__("r5");

    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xcccc; q0 = 0xf; q2 = 0x6666;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    do { } while (0);
    __CutsceneWait(0x3c);
    do { } while (0);
    m = 0x183a;
    __MessageID(m);
    if (a == 0) {
        __MessageID(m - 1);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q0 = 0xf; q1 = 0x101; q2 = 0x3c;
            __MapActor_Emote(q0, q1, q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q2 = 0x14; q0 = 0xf; q1 = 0;
            __Func_8093040(q0, q1, q2);
        }
        __Func_80925cc(0xf, 2);
        __MessageID(0x18ae);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q2 = 0x14; q0 = 0xf; q1 = 0;
            __Func_8093040(q0, q1, q2);
        }
        __MapActor_DoAnim(0xf, 4);
        __CutsceneWait(0x14);
        __Func_8093040(0xf, 0, 0x14);
        __MapActor_DoAnim(0xf, 3);
        __CutsceneWait(0x14);
    }
    if (a == 2) {
        __MessageID(0x18ac);
        __Func_80925cc(0xf, 2);
        __CutsceneWait(0x14);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 0x14; q0 = 0xf; q1 = 0;
        __Func_8093040(q0, q1, q2);
    }
    OvlFunc_926_2008f80();
    __Func_80925cc(0xf, 3);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xe8; q2 = 0xa8; q0 = 0x13; q1 <<= 16; q2 <<= 16;
        __MapActor_SetPos(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xe8; q2 = 0xa8; q1 <<= 16; q2 <<= 16; q0 = 0x14;
        __MapActor_SetPos(q0, q1, q2);
    }
    *(int *)(__MapActor_GetActor(0x13) + 0xc) = 0xc0 << 12;
    *(int *)(__MapActor_GetActor(0x13) + 0x3c) = 0x80 << 24;
    *(int *)(__MapActor_GetActor(0x13) + 0x18) = 0xcccc;
    {
        unsigned short *t = *(unsigned short **)(__MapActor_GetActor(0x13) + 0x50);
        int v = 0x80;
        v <<= 8;
        t[0xf] = v;
    }
    __PlaySound(0x7c);
    __CutsceneWait(0x28);
    __Func_80921c4(0xf, 0xd8, 0x98);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80; q0 = 0xf; q1 <<= 6; q2 = 0x1e;
        __Func_8092adc(q0, q1, q2);
    }
}
