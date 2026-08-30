/* Func_80b86ec  --  0x080b86ec, asm/rom_b5000/rom_b8228_c_a_c_a_a_c.s
 *
 * BLOCKER CLASS: a constant gcc rematerialises where the ROM keeps it live.
 * Status: 47 lines against the ROM's 44, with -fno-strict-aliasing.
 *
 * WHAT IT DOES
 * Nudges a scroll halfword by +/-0x200 while either shoulder button is held,
 * then, if a second object's field at +0x14 is clear, makes a five-argument
 * call.
 *
 * -fno-strict-aliasing IS PART OF THE ANSWER AND IS NOT ENOUGH. The ROM keeps
 * `&gKeyHeld` in r0 and RE-READS the value for the second test:
 *
 *      ldr r3, [r0]  ... strh r3, [r1, #0x36]  ... ldr r3, [r0]
 *
 * At -O2 gcc reads it once, because strict aliasing says an `unsigned short`
 * store cannot touch an `unsigned int` global. With the flag the two reads come
 * back and the first six instructions match exactly -- so the reading is right
 * and the TU would want the flag.
 *
 * WHAT IS LEFT. The mask 0x200 is both the test mask and the increment, and the
 * ROM keeps it in r2 across both:
 *
 *      mov r2, #0x80 / lsl r2, #2 ... and r3, r2 ... add r3, r2
 *
 * gcc builds it, uses it for the AND, then builds it AGAIN for the ADD --
 * `mov r2, #0x80 / lsl r2, #2` a second time, which is the three extra
 * instructions. A named `int k` used in both places, which is what the ROM's
 * register use says, does not stop it: rematerialising a two-instruction
 * constant is cheaper than keeping a register live, and gcc prices it that way.
 *
 * ALSO TRIED: `(k & gKeyHeld)` rather than `(gKeyHeld & k)`, to move the mask's
 * pseudo ahead of the load. Byte-identical.
 *
 * The second global is reached as `iwram_3001e80[0x20]`, not as a separate
 * symbol -- the ROM's `ldr r3, =iwram_3001e80 / add r3, #0x80` is one symbol
 * plus an offset too large for `ldr`'s immediate field, which an array index
 * reproduces.
  *
 * UPDATE: gKeyHeld must be declared VOLATILE, and that was worth half the
 * difference here -- 46 lines and 37 differing becomes 45 and 21, with the
 * first difference moving from line 2 to line 24. The ROM re-reads the global
 * on every test; without volatile gcc CSEs the reads into one and the whole
 * function shortens. The tree already had `extern volatile unsigned int
 * gKeyHeld;` in src/rom_c9000/rom_e3958_c_c_c_b.c, where it was needed for a
 * dead read, so this is a confirmation rather than a discovery -- but the
 * declaration had not been carried across to the files that poll it.
 * OvlFunc_974_200807c matched outright once it was applied there.
 *
 * The recorded blocker below is still the remaining one; it just starts 22
 * lines later than it used to.
*/

struct O {
    unsigned char pad00[0x36];
    unsigned short f36;
};

struct P {
    unsigned char pad00[0x14];
    int f14;
};

extern void *iwram_3001e80[];
extern volatile unsigned int gKeyHeld;
extern void Func_80c0a24(int a, int b, int c, int d, int e);

void Func_80b86ec(void)
{
    struct O *o;
    struct P *p;
    int k;

    o = iwram_3001e80[0];
    p = iwram_3001e80[0x20];
    k = 0x80 << 2;
    if ((gKeyHeld & k) != 0)
        o->f36 += k;
    if ((gKeyHeld & (0x80 << 1)) != 0)
        o->f36 += 0xfffffe00;
    if (p->f14 == 0)
        Func_80c0a24(0xf0 << 15, 0xf0 << 15, 0, 0, 0x80 << 9);
}
