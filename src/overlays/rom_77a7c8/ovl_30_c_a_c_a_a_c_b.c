/* Cluster OvlFunc_881_2008314..OvlFunc_881_2008314 extracted from goldensun/asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c_a.o and the rest of the overlay in
 * goldensun/overlays/rom_77a7c8/overlay.ld.
 *
 * Resets an actor's sprite state, and on one save bit clears its position.
 *
 * ONE ZERO, USED FOUR TIMES. The ROM keeps it in r6 across the branch and the
 * call, so it is a single named local rather than four literals -- the
 * shared-value form of the stack-arg-pair lever, applied to plain stores.
 * Written as literals gcc rebuilds it, which is the opposite of what this
 * function wants and the opposite of what the neighbouring
 * ovl_30_c_c.c wants three files away. Read the ROM.
 *
 * The +0x59 store is a pointer walk (`add r3, #0x59` then `strb`), and the
 * flag is `0x8a << 4` rather than a pooled 0x8a0.
 */
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __Actor_SetColorswap(void *a, int n);
extern int __GetFlag(int id);
extern void __SetFlag(int id);

int OvlFunc_881_2008314(void *a)
{
    unsigned char *q;
    int z;

    __Actor_SetSpriteFlags(a, 0);
    __Actor_SetColorswap(a, 0xa);
    q = (unsigned char *)a + 0x59;
    z = 0;
    *q = z;
    if (__GetFlag(0x8a << 4)) {
        __SetFlag(0x2f1);
        *(int *)((unsigned char *)a + 8) = z;
        *(int *)((unsigned char *)a + 0xc) = z;
    }
    return 0;
}
