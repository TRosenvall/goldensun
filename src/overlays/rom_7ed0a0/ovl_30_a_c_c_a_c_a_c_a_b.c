/* Cluster OvlFunc_964_200970c..OvlFunc_964_200970c extracted from goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_c_c_a_c_a_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7ed0a0/ovl_30_a_c_c_a_c_a_c_a_a.o and the rest of the
 * overlay in goldensun/overlays/rom_7ed0a0/overlay.ld.
 *
 * Two animations on actor 0x14 with a call between them, then clear bit 1 of
 * its +0x23 byte and set a flag.
 *
 * TWO THINGS HERE CONTRADICT RULES THIS TREE HAS WRITTEN DOWN, and both are
 * worth knowing before applying those rules mechanically.
 *
 * 1. THE SAME CALLEE GETS DIFFERENT ARGUMENT ORDERS. `__MapActor_SetAnim` is
 *    called twice. The first fills r0 then r1; the third call fills r1 then r0:
 *
 *        mov r0, #0x14 / mov r1, #1  / bl __MapActor_SetAnim
 *        mov r1, #2    / mov r0,#0x14 / bl __MapActor_SetAnim
 *
 *    A declaration is a property of the FILE, not of the call site, so no
 *    setting of it can produce both. gcc produces both anyway, from one
 *    declaration, because the order also depends on what is live around the
 *    call. Do not read a differing argument order as necessarily needing a
 *    lever -- write the obvious C and screen it first.
 *
 * 2. THE MASK IS WRITTEN INLINE. Every previous narrow-mask case in this tree
 *    needed `int m = ~2;` as a named local, because gcc narrows an inline `~m`
 *    to a byte immediate and loses the ROM's `mov/neg` pair. Here the ROM ALSO
 *    has the byte immediate (`mov r3, #0xfd`), so the narrowing is what is
 *    wanted and the named local would break it.
 *
 *    The discriminator is in the ROM, not in the source: `mov rN, #0xfd` means
 *    write it inline, `mov rN, #3 / neg rN, rN` means name it.
 */
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8092950(int slot, int n);
extern void *__MapActor_GetActor(int slot);
extern void __SetFlag(int id);

void OvlFunc_964_200970c(void)
{
    unsigned char *p;

    __MapActor_SetAnim(0x14, 1);
    __Func_8092950(0x14, 0);
    __MapActor_SetAnim(0x14, 2);
    p = (unsigned char *)__MapActor_GetActor(0x14) + 0x23;
    *p = ~2 & *p;
    __SetFlag(0x80 << 2);
}
