// fakematch
/* OvlFunc_899_20099a4  --  0x020099a4
 *
 * From goldensun/asm/overlays/rom_794ac0/ovl_30_a_c_c_c_a_c_a_a.s, which held
 * this function alone, so no split was needed.
 *
 * Member of shape group 0 (flat, {add, bl, bx, ldr, lsl, mov, pop, push, str}).
 *
 * TWO PINNED CALLS AND A POINTER THAT ADVANCES. The two calls are the ordinary
 * interleave, with `mov r0` in a different position in each:
 *
 *     mov r1, #0x80 / mov r2, #0x80 / mov r0, #0    / lsl r1, #8 / lsl r2, #7
 *     mov r1, #0xb6 / mov r2, #0xcc / lsl r1, #2    / mov r0, #0 / lsl r2, #1
 *
 * In the first, r0 is set before either shift; in the second, between them.
 * Same function, same helper family, two different placements -- read each call
 * off the listing.
 *
 * The tail advances the pointer in place rather than indexing:
 *
 *     ldr r3, =iwram_3001ebc / ldr r3, [r3] / add r3, r2 / str r2, [r3]
 *
 * `p = (char *)iwram_3001ebc; p += 0xe4 << 1; *(int *)p = 0x10;` reproduces it
 * -- the pointer-advance tell from batch 190, and the same form used in
 * src/overlays/rom_7fcd20/ovl_30_c_c_a_c_a_c_a_c.c.
 *
 * Its near-twin OvlFunc_898_2008ef4 shares this whole tail and is PARKED on a
 * register-role swap that has nothing to do with any of the above; see
 * src/non_matching/ovl_793768/2008ef4.c.
 */

extern int iwram_3001ebc;
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __PlaySound(int id);
extern void __Func_8091e9c(int n);

void OvlFunc_899_20099a4(void)
{
    char *p;

    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80;
        q2 = 0x80;
        q0 = 0;
        q1 <<= 8;
        q2 <<= 7;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xb6;
        q2 = 0xcc;
        q1 <<= 2;
        q0 = 0;
        q2 <<= 1;
        __Func_809218c(q0, q1, q2);
    }
    p = (char *)iwram_3001ebc;
    p += 0xe4 << 1;
    *(int *)p = 0x10;
    __PlaySound(0x7b);
    __Func_8091e9c(0xf);
}
