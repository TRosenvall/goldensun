/* Cluster OvlFunc_964_2008fe8..OvlFunc_964_2008fe8 extracted from goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_a_c_c_a_a.s.
 *
 * Slotted between ovl_30_a_a_c_c_a_a_a.o and the rest of the overlay.
 *
 * WHETHER A CALL RESULT GOES THROUGH A NAMED VARIABLE DECIDES WHICH REGISTER
 * HOLDS IT. The last __MapActor_GetActor is used INLINE inside the expression
 * that reads +0x10; assigned to the same `a` the three earlier calls use, gcc
 * puts the joined value in r0 where the ROM has r3, and five positions differ.
 * Nothing else about the function changes.
 *
 * The `>> 20` on the position word is the tile-coordinate tell. The two arms
 * join before the final store, which is why the tail is a goto rather than a
 * duplicated store in each arm.
 */
extern void *__MapActor_GetActor(int slot);

int OvlFunc_964_2008fe8(void *p)
{
    unsigned char *a;
    int v;

    a = (unsigned char *)__MapActor_GetActor(0);
    if (*(int *)(a + 0xc) > (int)0xffd00000) {
        a = (unsigned char *)__MapActor_GetActor(8);
        if ((*(int *)(a + 0x10) >> 20) == 0xa) {
            a = (unsigned char *)__MapActor_GetActor(8);
            *(int *)((unsigned char *)p + 8) = *(int *)(a + 8);
            *(int *)((unsigned char *)p + 0xc) = (int)0xffe00000;
            v = *(int *)((unsigned char *)__MapActor_GetActor(8) + 0x10);
            goto out;
        }
    }
    v = 0;
    *(int *)((unsigned char *)p + 8) = v;
    *(int *)((unsigned char *)p + 0xc) = v;
out:
    *(int *)((unsigned char *)p + 0x10) = v;
    return 0;
}
