/* OvlFunc_882_200a0fc  --  0x0200a0fc, cut from
 * goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_a.o and
 * asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c.o in
 * goldensun/overlays/rom_77dd1c/overlay.ld.
 *
 * Sets the same option on four actors, with the value chosen by two bits of the
 * frame counter shifted by a variable this overlay keeps in .bss.
 *
 * REACHING A `.lcomm` SYMBOL FROM C, and the tree already had the answer. The
 * overlay declares
 *
 *     .global .L57fc
 *     .lcomm  .L57fc, 4
 *
 * -- a four-byte .bss slot whose NAME is not a C identifier. My first move was
 * to rename it and update all five references, the way batch 80 renamed a data
 * label. That builds, but it is the wrong tool here: two already-elevated files
 * in this same overlay reference it as
 *
 *     extern unsigned int L57fc __asm__(".L57fc");
 *
 * and the rename broke both. gcc's asm-label extension gives the declaration a
 * link name that need not be a valid identifier, which reaches the symbol
 * WITHOUT touching any other file. That is the idiom to use, and batch 80's
 * rename would have been unnecessary had I looked for it -- the note there is
 * corrected.
 *
 * The body is a plain if/else. Both arms call the same helper on the same four
 * slot ids in the same order, differing only in the constant, and gcc does not
 * cross-jump them.
 */
extern unsigned int iwram_3001e40;
extern unsigned int L57fc __asm__(".L57fc");
extern void *__MapActor_GetActor(int slot);
extern void OvlFunc_882_200a09c(void *a, int n);

void OvlFunc_882_200a0fc(void)
{
    if (((iwram_3001e40 >> L57fc) & 3) != 0) {
        OvlFunc_882_200a09c(__MapActor_GetActor(0x20), 1);
        OvlFunc_882_200a09c(__MapActor_GetActor(0x21), 1);
        OvlFunc_882_200a09c(__MapActor_GetActor(0x1e), 1);
        OvlFunc_882_200a09c(__MapActor_GetActor(0x1d), 1);
    } else {
        OvlFunc_882_200a09c(__MapActor_GetActor(0x20), 8);
        OvlFunc_882_200a09c(__MapActor_GetActor(0x21), 8);
        OvlFunc_882_200a09c(__MapActor_GetActor(0x1e), 8);
        OvlFunc_882_200a09c(__MapActor_GetActor(0x1d), 8);
    }
}
