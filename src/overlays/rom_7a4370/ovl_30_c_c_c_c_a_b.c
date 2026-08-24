/* Cluster OvlFunc_917_200972c..OvlFunc_917_200972c extracted from goldensun/asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a.o and
 * asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_c.o in
 * goldensun/overlays/rom_7a4370/overlay.ld.
 *
 * A cross-overlay copy of src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_b.c, found by
 * tools/match_shapes.py. Read that file for the two load-bearing details:
 * the -1 is compared twice so gcc builds it as `mov/neg` unprompted, and the
 * byte offset must be a NAMED LOCAL or gcc folds it into the base pointer and
 * stores with `strh r3,[r0]` instead of the ROM's register-offset form.
 *
 * The one difference from the exemplar is that the stored value arrives as a
 * parameter rather than being computed, which is why r1 is saved into r8 in
 * the prologue. It needs nothing from the source to happen.
 */
extern int __CheckPartyItem(int item);
extern int __CheckItem(int party, int item);
extern void *__GetUnit(int party);

void OvlFunc_917_200972c(int item, int value)
{
    int party;
    int slot;
    void *unit;

    party = __CheckPartyItem(item);
    if (party == -1)
        return;
    slot = __CheckItem(party, item);
    if (slot == -1)
        return;
    unit = __GetUnit(party);
    {
        int off = (slot << 1) + 0xd8;

        *(short *)((char *)unit + off) = value;
    }
}
