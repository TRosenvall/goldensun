/* Cluster OvlFunc_939_2008468..OvlFunc_939_2008468 extracted from goldensun/asm/overlays/rom_7c460c/ovl_314_a_c_a_a_c_a.s.
 *
 * Slotted between ovl_314_a_c_a_a_c_a_a.o and the rest of the overlay.
 *
 * TWO ARMS JOINING AT A SHARED STORE, so the tail is a join rather than a
 * duplicated `strb` in each arm -- same shape as OvlFunc_964_2008fe8 in batch
 * 44, and the same reason: the ROM has one `strb r3, [r0]` after the label.
 *
 * THE ARMS BUILD THE POINTER IN A DIFFERENT ORDER and the C has to follow.
 * The taken arm does `add r0, #0x23` and then loads through it; the other does
 * `mov r3, #1` FIRST and adds afterwards. Writing both arms as
 * `p = ... + 0x23;` puts the add before the 1 in the second arm and two
 * positions swap, so the second arm advances the pointer after the value.
 *
 * The first __MapActor_GetActor is used INLINE inside the comparison -- the
 * named-variable lever from batch 44. Assigned to a local it lands in the
 * wrong register.
 */
extern void *__MapActor_GetActor(int slot);

void OvlFunc_939_2008468(void)
{
    unsigned char *p;
    int v;

    if (*(int *)((unsigned char *)__MapActor_GetActor(0) + 0xc) >= (0x80 << 13)) {
        p = (unsigned char *)__MapActor_GetActor(8) + 0x23;
        v = 2 | *p;
    } else {
        p = (unsigned char *)__MapActor_GetActor(8);
        v = 1;
        p += 0x23;
    }
    *p = v;
}
