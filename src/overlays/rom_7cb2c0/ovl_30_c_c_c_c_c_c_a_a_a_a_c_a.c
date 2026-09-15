// fakematch
/* Cluster OvlFunc_945_200c13c..OvlFunc_945_200c13c extracted from goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_c.s.
 *
 * Total .text for this TU = 92 bytes (= 0x5c).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_b.o and asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_c_b.o in
 * goldensun/overlays/rom_7cb2c0/overlay.ld.
 *
 * TWO PINS, BOTH LOAD-BEARING, NO BARRIER. `__Func_80933f8(-1, -1, -1, 0)`
 * written plainly has cse1 common the three -1s into one pseudo and copy it out
 * (`mov r2,#1 / neg r2,r2 / mov r0,r2 / mov r1,r2`, two instructions SHORT);
 * the ROM materialises each separately (`mov r0,#1 / mov r1,#1 / mov r2,#1 /
 * mov r3,#0 / neg r1,r1 / neg r2,r2 / neg r0,r0`). Pinning r0 and r1 defeats it
 * and the third -1 can stay a literal.
 *
 * The same two pins then place the `__Func_8092adc` interleave: the ROM sets r0
 * between the `mov r1,#0xd0` and its shift, which is the recorded split-pin
 * lever (the assignment's position, not the declaration's).
 *
 * TEARDOWN, measured by REMOVING from the finished file:
 *   the p0 pin on the -1 call                 31 differing
 *   the p1 pin on the -1 call                 31
 *   the __Func_8092adc statement split         2
 *   nothing (as landed)                        0
 *
 * Also measured and inert: pinning r2 and r3 as well (exact either way, so they
 * are scaffolding the teardown rejected), `-a`/`-b`/`-c` on named locals holding
 * 1 (31 -- the front end folds it), and a plain local for the 0xd0 << 8 value
 * without pins (31).
 */
extern void __CutsceneStart(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __WaitFrames(int n);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __MessageID(int id);
extern void OvlFunc_945_200c86c(int a);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_945_200c13c(void)
{
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");

    __CutsceneStart();
    p0 = -1;
    p1 = -1;
    __Func_80933f8(p0, p1, -1, 0);
    __WaitFrames(1);
    OvlFunc_945_200c8e8(0xf, 1, 1);
    __Func_80925cc(8, 1);
    __MessageID(0x1e43);
    OvlFunc_945_200c86c(8);
    p1 = 0xd0;
    p0 = 8;
    p1 <<= 8;
    __Func_8092adc(p0, p1, 0x28);
    OvlFunc_945_200c8e8(9, 0xf, 0);
}
