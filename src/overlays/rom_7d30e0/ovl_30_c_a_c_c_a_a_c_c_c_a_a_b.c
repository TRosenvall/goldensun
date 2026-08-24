/* Cluster OvlFunc_948_2009120..OvlFunc_948_2009120 extracted from goldensun/asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_a_a_c.o above it and the rest of
 * the overlay in goldensun/overlays/rom_7d30e0/overlay.ld. First of a twin pair
 * differing only in the sub-state number.
 *
 * Runs sub-state 2, then -- only while the player's actor is in facing state
 * 0x4000 and the interaction halfword at [iwram_3001ebc]+0x19c has counted past
 * 0xc -- advances the dialogue and clears the counter.
 *
 * THE THIRD AND FOURTH MEMBERS of the inverted-narrow_constant shape. `*q = 0`
 * written directly compiles to a HALFWORD POOL LOAD, because gcc narrows the
 * whole expression to sixteen bits; the assignment goes through a named `int`
 * so the value stays word-wide until the store and gcc emits `mov r3, #0`.
 * See src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_b.c for the first.
 *
 * The address is built in three named steps for the same reason as every other
 * member: the ROM computes the offset at runtime (`mov r2,#0xce / lsl r2,#1`)
 * and puts the sum in a THIRD register (`add r5, r3, r2`, not destructive).
 *
 * The counter is read with `ldrsh`, so it is signed; 0x4000 is built as
 * `mov r2,#0x80 / lsl r2,#7` and written that way.
 */
extern unsigned int iwram_3001ebc;

extern void OvlFunc_948_20090b8(int n);
extern void *__MapActor_GetActor(int slot);
extern void __Func_8093c00(void);

void OvlFunc_948_2009120(void)
{
    unsigned char *base;
    unsigned int off;
    short *q;
    void *a;
    int z;

    OvlFunc_948_20090b8(2);
    a = __MapActor_GetActor(0);
    if (*(unsigned short *)((unsigned char *)a + 6) == (0x80 << 7)) {
        base = (unsigned char *)iwram_3001ebc;
        off = 0xce;
        off <<= 1;
        q = (short *)(base + off);
        if (*q > 0xc) {
            __Func_8093c00();
            z = 0;
            *q = z;
        }
    }
}
