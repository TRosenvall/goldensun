/* Cluster OvlFunc_935_2008aa0..OvlFunc_935_2008aa0 extracted from
 * goldensun/asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_c_c_c.s.
 *
 * A per-frame flicker: bump a counter, and every 64th tick pick a random one of
 * six actors; then sweep all six, resetting whichever disagree with their save
 * bit.
 *
 * FOUR READINGS, and one of them is a lesson about the screen itself.
 *
 *   THE MODULO IS UNSIGNED.  The ROM calls the RAM-resident helper.  Declaring
 *   __Random as returning `unsigned int` is what selects it; with a signed
 *   return gcc emits __modsi3 and the call is simply wrong.
 *
 *   THE HELPER'S NAME IS A LINKER ALIAS, NOT A DIFFERENCE.  tryc.py compares
 *   symbols, so it reports `bl __umodsi3` against the ROM's `bl _umodsi3_RAM`
 *   and will NEVER go below one differing line here.  Every overlay linker
 *   script that needs it carries `__umodsi3 = _umodsi3_RAM;`, this one at line
 *   93, and five already-elevated files link through exactly that route.  The
 *   residual line is not real -- but see the warning below before trusting that
 *   reasoning on its own.
 *
 *   THE DATA ADDRESS IS HELD ACROSS THE CALLS.  The ROM materialises &.L2230
 *   into r5 BEFORE `bl __Random` and stores through it after.  Assigning the
 *   global directly makes gcc load the address afterwards into a scratch
 *   register; a named `int *q = &L2230;` before the call keeps it where the ROM
 *   has it.  7 differing -> 3.
 *
 *   `i = 0` IS WRITTEN BEFORE `t = 0xff << 16`.  The ROM interleaves them --
 *   `mov r7, #0xff / mov r6, #0 / lsl r7, #0x10` -- so the counter's
 *   initialisation lands INSIDE the constant's two-instruction build.  Swapping
 *   the two statements is all it takes.  Hoisting the constant into a block that
 *   dominates the loop, which is the usual lever for that shape, is much worse
 *   here (17 and 24 differing).
 *
 * THE LESSON.  An earlier version of this file was installed on the strength of
 * "the only difference left is the alias, so it must match" -- and make compare
 * rejected the overlay.  The alias reasoning was right; what was wrong was
 * treating THREE differing lines as one, because two of them were the statement
 * swap above and I had stopped reading the diff.  tryc.py is a screen, not a
 * verdict: read every line it prints before deciding which ones do not count.
 */
extern int L222c __asm__(".L222c");
extern int L2230 __asm__(".L2230");

extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned int __Random(void);
extern int __GetFlag(int id);
extern void __PlaySound(int id);

void OvlFunc_935_2008aa0(void)
{
    unsigned char *e;
    int i;
    int t;
    int c;
    unsigned int n;
    int *q;

    e = __MapActor_GetActor(0xa);
    if (e[0x5b] != 0)
        return;
    c = L222c + 1;
    L222c = c;
    if ((c & 0x3f) == 0) {
        q = &L2230;
        n = __Random() % 6;
        *q = n;
        e = __MapActor_GetActor(n + 0xa);
        *(int *)(e + 0x48) = 0xa3d;
    }
    i = 0;
    t = 0xff << 16;
    do {
        e = __MapActor_GetActor(i + 0xa);
        if (__GetFlag(i + (0x80 << 2))) {
            if (*(int *)(e + 0x28) > 0 || *(int *)(e + 0xc) <= 0x20ffff) {
                *(int *)(e + 0xc) = t;
                *(int *)(e + 0x48) = 0;
                *(int *)(e + 0x28) = 0;
                __PlaySound(0x6a);
            }
        } else {
            if (*(int *)(e + 0x28) > 0 || *(int *)(e + 0xc) <= 0xffff) {
                *(int *)(e + 0x48) = 0;
                *(int *)(e + 0x28) = 0;
                *(int *)(e + 0xc) = t;
                __PlaySound(0x6a);
            }
        }
        i += 1;
    } while (i <= 5);
}
