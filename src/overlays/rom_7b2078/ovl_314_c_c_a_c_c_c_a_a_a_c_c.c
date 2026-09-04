// fakematch
/* OvlFunc_926_2009160  --  0x02009160
 *
 * Was the whole of goldensun/asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_c_c.s
 * after OvlFunc_926_200902c was split off ahead of it.
 *
 * The second half of the same cutscene as ovl_314_c_c_a_c_c_c_a_a_a_c_b.c, and
 * it SHARES THAT FUNCTION'S ENTIRE TAIL -- from the call into
 * OvlFunc_926_2008f80 to the end, the two are identical but for one shift width
 * (`lsl r1, #7` here against `#6` there). Every lever in the tail is documented
 * in that file and was copied across verbatim: the two pinned
 * __MapActor_SetPos calls, the four re-called __MapActor_GetActor field stores,
 * the split `v = 0x80; v <<= 8;` build, and the named destination pointer on the
 * halfword store. No barrier is needed here because there is no long-lived
 * named constant to hold down.
 *
 * WHAT THIS ONE ADDS is a clean instance of a pattern already noted elsewhere:
 * FIVE CONSECUTIVE CALLS TO ONE FUNCTION AND ONLY THE LAST NEEDS A PIN.
 * __Func_809280c is called with (0xd, 0x13, 0), (0xe, ...), (0xf, ...),
 * (0x10, ...) and (0x12, ...); the ROM fills r0, r1, r2 in ascending order for
 * the first four and in DESCENDING order for the fifth. Plain literals get the
 * four right unaided and the fifth wrong. The call list is therefore transcribed
 * from the listing rather than written as a loop over a table -- a loop would
 * have been shorter and would have had no way to express the exception.
 *
 * Both __MapActor_Emote calls need their arguments anchored, for the same reason
 * as in the sibling: the ROM sets `mov r0` ahead of the pooled `ldr r1`, and gcc
 * left to itself emits the pool load first. Everything else in the head of the
 * function is ascending order and needs nothing.
 *
 * __MapActor_SetSpeed here is called with the same two pool constants as in the
 * sibling and the ROM fills them in a DIFFERENT order (`ldr r2 / mov r0 /
 * ldr r1` against `ldr r1 / mov r0 / ldr r2`). Same callee, same arguments, same
 * overlay, two orders -- which is the whole argument for reading each call site
 * off the listing instead of reusing the last one that worked.
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
extern void __SetFlag(int id);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);

void OvlFunc_926_2009160(void)
{
    __Func_809280c(0xd, 0x13, 0);
    __Func_809280c(0xe, 0x13, 0);
    __Func_809280c(0xf, 0x13, 0);
    __Func_809280c(0x10, 0x13, 0);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 0; q1 = 0x13; q0 = 0x12;
        __Func_809280c(q0, q1, q2);
    }
    __CutsceneWait(0x14);
    __Func_80925cc(0xf, 2);
    __CutsceneWait(0x14);
    __MessageID(0x187a);
    __Func_8093040(0xf, 0, 0x14);
    __Func_8093040(0x10, 0, 0x14);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q0 = 0x12; q1 = 0x105; q2 = 0x3c;
        __MapActor_Emote(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q0 = 0x10; q1 = 0x101; q2 = 0x3c;
        __MapActor_Emote(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 0x14; q0 = 0x10; q1 = 0;
        __Func_8093040(q0, q1, q2);
    }
    __MapActor_DoAnim(0x12, 4);
    __CutsceneWait(0x14);
    __Func_8093040(0x12, 0, 0x14);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x81; q0 = 0x10; q1 <<= 1; q2 = 0x3c;
        __MapActor_Emote(q0, q1, q2);
    }
    __Func_8093040(0x10, 0, 0x14);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 0; q1 = 0x12; q0 = 0xf;
        __Func_8092848(q0, q1, q2);
    }
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x12, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xf, 3);
    __CutsceneWait(0x14);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 0x6666; q0 = 0xf; q1 = 0xcccc;
        __MapActor_SetSpeed(q0, q1, q2);
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
        q1 = 0x80; q0 = 0xf; q1 <<= 7; q2 = 0x1e;
        __Func_8092adc(q0, q1, q2);
    }
    __SetFlag(0x301);
}
