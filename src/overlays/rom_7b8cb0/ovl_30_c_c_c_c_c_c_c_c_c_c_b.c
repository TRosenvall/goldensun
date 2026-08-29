/* Cluster OvlFunc_931_2008c0c..OvlFunc_931_2008c0c extracted from goldensun/asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c.s.
 *
 * Total .text for this TU = 56 bytes (= 0x38).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_a.o and
 * asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_c.o in
 * goldensun/overlays/rom_7b8cb0/overlay.ld. The .data section stays in the _c
 * piece and the overlay script's .data line is repointed there.
 *
 * UNPARKED. This sat in src/non_matching/ from batch 53 at 1 of 24, and is the
 * SAME FUNCTION as OvlFunc_932_200aa10 in ovl_7b9cb4 -- instruction for
 * instruction, constants included. That twin was elevated in batch 61, and its
 * source matches here unedited except for the name.
 *
 * WHY THE OLD PARK NOTE WAS WRONG IS THE USEFUL PART. It recorded the reuse
 * spelling -- `v = 0; p[0x55] = v; v -= 0xd;` -- as tried and WORSE (2 of 24),
 * and concluded gcc's byte-width narrowing was unreachable. The spelling was
 * right; the STATEMENT ORDER was not. The subtraction has to come after the
 * intervening loads:
 *
 *      z = 0;
 *      *p = z;
 *      c = *(unsigned char **)(a + 0x50);   <- these two loads sit between
 *      t = c[9];                            <-
 *      z = z - 0xd;
 *
 * Written with the subtraction immediately after the store, gcc also moves the
 * store and the result is worse than doing nothing.
 *
 * The lesson for park notes generally: "tried and worse" about a SPELLING can
 * be true while the same spelling in a different ORDER is exact. A park that
 * rules out a lever should say where in the statement sequence it was placed.
 */
extern void __Func_80929d8(void *a, int n);
extern void __Actor_SetSpriteFlags(void *a, int n);

void OvlFunc_931_2008c0c(unsigned char *actor)
{
    unsigned char *a;
    unsigned char *p;
    unsigned char *c;
    int z;
    int t;
    int m;
    int w;

    a = actor;
    p = a + 0x55;
    z = 0;
    *p = z;
    c = *(unsigned char **)(a + 0x50);
    t = c[9];
    z = z - 0xd;
    z = z & t;
    m = 4;
    z = z | m;
    c[9] = z;
    __Func_80929d8(a, 3);
    __Actor_SetSpriteFlags(a, 0);
    w = 0x4ccc;
    *(int *)(a + 0x18) = w;
    *(int *)(a + 0x1c) = w;
}
