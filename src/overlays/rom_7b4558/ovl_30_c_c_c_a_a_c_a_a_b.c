/* OvlFunc_927_2009c34  --  0x02009c34
 *
 * Cut out of goldensun/asm/overlays/rom_7b4558/ovl_30_c_c_c_a_a_c_a_a.s.
 *
 * Stamps a five-cell plus shape of map tiles centred on actor 0xe -- the centre
 * with parameter 0xff, the four neighbours with 0 -- then, if the actor is
 * standing on row 0x1b, clears its flag byte, parks it at -0x20000 on two axes,
 * sets flag 0x214 and stamps one more cell.
 *
 * Found by tools/templated.py at a perfect 1.00.
 *
 * THE CONSTANTS ARE PLAIN LITERALS, AND I GUESSED THE OPPOSITE FIRST. The ROM
 * keeps 1 and 0xff in r8 and r10 across all six calls, which reads exactly like
 * the recorded "named local held across a call" shape -- so the first candidate
 * named them, and measured 80 differing of 92. Written as plain literals at
 * every call site it is 8 of 90.
 *
 * The tell that distinguishes the two is WHERE THE FIRST USE GOES. The ROM's
 * first call stores the literals straight to the stack --
 * `mov r3, #1 / str r3, [sp]` -- and only afterwards copies them into the high
 * registers for later reuse. A named local would have been materialised into
 * its register FIRST and the stack store fed from there. So the registers are
 * gcc hoisting a constant it sees reused, not the source naming a value:
 *
 *     ROM     mov r3, #1 / str r3, [sp] / ... / mov r8, r3
 *     named   mov r3, #1 / mov r8, r3   / ... / mov r3, r8 / str r3, [sp]
 *
 * Worth keeping because the surface symptom -- a constant living in a
 * callee-saved register across several calls -- is identical either way. Read
 * the ORDER of the first use before concluding the source named it.
 *
 * THE LAST SIX INSTRUCTIONS WERE THE CALLEE'S RETURN TYPE. Declared `void`,
 * `mov r0, #2` is emitted one slot early at three of the six call sites.
 * Declared to return a value it lands where the ROM has it. That is the recorded
 * mechanism: an int-returning callee carries `(set (reg:SI 0 r0) (call ...))`,
 * the next real write of r0, which truncates the dependent list of the `mov r0`
 * feeding it and flips the argument-setup scheduling tie. `int`, `unsigned int`
 * and `void *` all match; only `void` fails, so the lever is whether the call
 * writes r0 at all rather than what width it writes.
 *
 * The two __MapActor_GetActor(0xe) calls at the top are genuinely two calls,
 * one per field, not one call with the pointer held -- the same reading as
 * "two loads of the same field are direct field reads", one level up.
 *
 * `z--` before the fifth call is a compound assignment: the ROM's `sub r5, #1`
 * is two-address, and the fourth call passes `z + 1` without disturbing z.
 */

struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x55 - 0x18];
    unsigned char f55;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __SetFlag(int id);
extern int OvlFunc_927_2008244(int a, int b, int c, int d, int e, int f);

void OvlFunc_927_2009c34(void)
{
    struct A *a;
    int x;
    int z;

    __CutsceneStart();
    x = __MapActor_GetActor(0xe)->f8 >> 20;
    z = __MapActor_GetActor(0xe)->f10 >> 20;
    OvlFunc_927_2008244(2, x, z, 1, 1, 0xff);
    OvlFunc_927_2008244(2, x + 1, z, 1, 1, 0);
    OvlFunc_927_2008244(2, x - 1, z, 1, 1, 0);
    OvlFunc_927_2008244(2, x, z + 1, 1, 1, 0);
    z--;
    OvlFunc_927_2008244(2, x, z, 1, 1, 0);
    if ((__MapActor_GetActor(0xe)->f10 >> 20) == 0x1b) {
        a = __MapActor_GetActor(0xe);
        a->f55 = 0;
        a->f14 = 0xfffe0000;
        a->fc = 0xfffe0000;
        __SetFlag(0x85 << 2);
        OvlFunc_927_2008244(2, 0x2b, 0x17, 1, 1, 0xff);
    }
    __CutsceneEnd();
}
