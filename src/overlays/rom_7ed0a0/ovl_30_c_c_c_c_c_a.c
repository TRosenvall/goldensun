/* OvlFunc_964_200a3a0  --  0x0200a3a0
 * OvlFunc_964_200a410  --  0x0200a410
 * OvlFunc_964_200a480  --  0x0200a480
 * OvlFunc_964_200a52c  --  0x0200a52c
 *
 * The first four functions of goldensun/asm/overlays/rom_7ed0a0/ovl_30_c_c_c_c_c.s.
 * The fifth -- a 993-instruction cutscene -- and the file's .data stay as
 * assembly in ovl_30_c_c_c_c_c_b.s, and the linker script lists the two objects
 * where the one used to be, in both the .text and the .data run.
 *
 * Four map-repaint helpers for the same cutscene. Each blanks a rectangle, then
 * repaints one tile per actor at wherever that actor is currently standing,
 * reading the two 12.20 coordinates back with `asr #20`.
 *
 * THE POSITION IS READ WITH TWO SEPARATE CALLS, and the C has to say so. Every
 * one of these blocks is
 *
 *      mov r0, #8 / bl __MapActor_GetActor / ldr r5, [r0, #8]
 *      mov r0, #8 / bl __MapActor_GetActor / ldr r3, [r0, #0x10]
 *
 * -- the same actor fetched twice, once per coordinate. Fetching it once into a
 * local and reading both fields would drop a call. So the two subscripts are
 * written as two separate `__MapActor_GetActor(n)->field` expressions, which is
 * what the original must have looked like.
 *
 * TWO LEVERS, both already on the books, both needed here:
 *
 *   THE STACK-ARG PAIR MUST BE NAMED. The opening call of the first two
 *   functions passes two constants in the stack slots, and the ROM builds BOTH
 *   into separate registers before storing either:
 *       rom   mov r3, #9 / mov r2, #0x26 / str r3, [sp] / str r2, [sp, #4]
 *       ours  mov r3, #9 / str r3, [sp] / mov r3, #0x26 / str r3, [sp, #4]
 *   Passing them as literals walks one register through both slots. Naming them
 *   as two locals is what separates them, and it is the whole difference -- 5
 *   differing positions of 48 becomes an exact match.
 *
 *   ONE CALLEE HAS NO PROTOTYPE. OvlFunc_964_200a52c came down to two
 *   instructions, a swap of `mov r1, r5` and `mov r0, #0` in the second call to
 *   OvlFunc_964_2008244. Deleting that function's `extern` declaration puts r0
 *   after r1 and closes it. This is batch 92's rule applied deliberately for
 *   the first time rather than stumbled on, and it is the second function it
 *   has closed.
 *
 * ONE THING THAT LOOKED LIKE A LEVER AND WAS NOT. In OvlFunc_964_200a52c the
 * ROM keeps 1 and 0xff in r8 and r6 across both calls to OvlFunc_964_2008244,
 * which reads as two function-scope locals. Written that way gcc hoists them
 * above the first call and the function comes out at 52 instructions against
 * 49. Passing plain literals at both call sites gives 49 and lets gcc discover
 * the shared registers itself. The ROM holding a value in a callee-saved
 * register is not by itself evidence that the source named it.
 */
struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x22 - 0x14];
    unsigned char f22;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_964_2008f10(int a, int b);

void OvlFunc_964_200a3a0(void)
{
    int e, f;

    e = 9;
    f = 0x26;
    __Func_8010704(0x49, 0x26, 5, 5, e, f);
    OvlFunc_964_2008f10(9, 8);
    __Func_8010704(2, 0x24, 1, 1,
                   __MapActor_GetActor(8)->f8 >> 20,
                   __MapActor_GetActor(8)->f10 >> 20);
    __Func_8010704(2, 0x24, 1, 1,
                   __MapActor_GetActor(9)->f8 >> 20,
                   __MapActor_GetActor(9)->f10 >> 20);
}

void OvlFunc_964_200a410(void)
{
    int e, f;

    e = 0x1d;
    f = 0x1e;
    __Func_8010704(0x5d, 0x1e, 6, 5, e, f);
    OvlFunc_964_2008f10(0xb, 0xa);
    __Func_8010704(2, 0x24, 1, 1,
                   __MapActor_GetActor(0xa)->f8 >> 20,
                   __MapActor_GetActor(0xa)->f10 >> 20);
    __Func_8010704(2, 0x24, 1, 1,
                   __MapActor_GetActor(0xb)->f8 >> 20,
                   __MapActor_GetActor(0xb)->f10 >> 20);
}

void OvlFunc_964_200a480(void)
{
    int e;

    e = 0x19;
    __Func_8010704(0x59, 0x31, 3, 2, e, 0x31);
    __Func_8010704(0x59, 0x33, 8, 5, e, 0x33);
    __MapActor_GetActor(0xe)->f22 = 1;
    __Func_8010704(0x16, 0x34, 1, 1,
                   __MapActor_GetActor(0xc)->f8 >> 20,
                   __MapActor_GetActor(0xc)->f10 >> 20);
    __Func_8010704(0x16, 0x34, 1, 1,
                   __MapActor_GetActor(0xd)->f8 >> 20,
                   __MapActor_GetActor(0xd)->f10 >> 20);
    __Func_8010704(0x16, 0x34, 1, 1,
                   __MapActor_GetActor(0xe)->f8 >> 20,
                   __MapActor_GetActor(0xe)->f10 >> 20);
}

void OvlFunc_964_200a52c(void)
{
    int e, f;

    e = 0x2c;
    f = 0x13;
    __Func_8010704(0x6c, 0x13, 4, 1, e, f);
    OvlFunc_964_2008244(0,
                        __MapActor_GetActor(0x11)->f8 >> 20,
                        __MapActor_GetActor(0x11)->f10 >> 20, 1, 1, 0xff);
    OvlFunc_964_2008244(0,
                        __MapActor_GetActor(0x12)->f8 >> 20,
                        __MapActor_GetActor(0x12)->f10 >> 20, 1, 1, 0xff);
}
