// fakematch
/* OvlFunc_936_2009ed8  --  0x02009ed8
 *
 * From goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_c_c_a.s, which held
 * this function alone, so no split was needed.
 *
 * Member of shape group 2 (flat, {bl, bx, ldr, lsl, mov, pop, push, str}).
 *
 * TWO BLOCKERS AT ONCE, both already on file, and they happen to want the same
 * fix. Each of the two __MapActor_SetSpeed calls is
 *
 *     mov r0, #0x14 / ldr r1, =0x19999 / ldr r2, =0xcccc
 *
 * which is (a) the precompute bind -- both pool loads exceed the rtx_cost
 * threshold so gcc precomputes them and the cheap `mov r0` lands last, as in
 * src/overlays/rom_7aa430/ovl_e90_c_c_a_a_c_c.c -- and (b) a REPEATED PAIR of
 * pool constants, 0x19999 and 0xcccc at both call sites, which gcc would
 * otherwise load once and copy. Pinning all three argument registers at each
 * site answers both: r0 is placed first, and r1/r2 being call-clobbered forces
 * the reload the ROM performs.
 *
 * The `.L5144` slot is zeroed through the tree's asm-renamed extern idiom, and
 * needs no help -- gcc emits `ldr r3 / mov r2, #0 / str r2, [r3]` on its own.
 */

extern int L5144 __asm__(".L5144");
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __StartTask(void *fn, int arg);
extern void OvlFunc_936_2009f14(void);

void OvlFunc_936_2009ed8(void)
{
    L5144 = 0;
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q0 = 0x14;
        q1 = 0x19999;
        q2 = 0xcccc;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q0 = 0x15;
        q1 = 0x19999;
        q2 = 0xcccc;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    {
        register int q1 __asm__("r1");
        q1 = 0xc8;
        q1 <<= 4;
        __StartTask(OvlFunc_936_2009f14, q1);
    }
}
