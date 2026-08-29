/* Cluster OvlFunc_926_200c1c4..OvlFunc_926_200c1c4 extracted from goldensun/asm/overlays/rom_7b2078/ovl_314_c_c_c_c_c_c_c.s.
 *
 * Slotted between ovl_314_c_c_c_c_c_c_c_a.o and the rest of the overlay.
 *
 * ONE `.global` WAS ADDED to the .s to split this out. tools/split_s.py refused
 * the split because `.L51d8` is referenced from the half above and defined in
 * the half below, and a `.L` symbol does not survive into the object symbol
 * table. Exporting it emits no bytes; `make compare` was verified green after
 * the export and BEFORE the split, so the two changes stay separable. Ninth
 * such line in this tree.
 *
 * The actor pointer is never written before the calls -- it arrives in r0 and
 * stays there -- so only the second argument differs between the arms.
 */
extern unsigned int iwram_3001e40;
extern void __Func_80929d8(void *a, int n);

void OvlFunc_926_200c1c4(void *a)
{
    if ((iwram_3001e40 >> 1) & 1)
        __Func_80929d8(a, 0xa);
    else
        __Func_80929d8(a, 9);
}
