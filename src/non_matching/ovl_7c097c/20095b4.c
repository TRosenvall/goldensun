/* OvlFunc_936_20095b4  [ovl_7c097c]  --  0x020095b4
 *
 * Source asm: goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a.s
 *
 * Seven of eighteen. Blocker: CONSTANT-CSE, and it sharpens the rule.
 *
 * `0x80 << 2` is passed to __GetFlag and again to __SetFlag. The ROM rebuilds
 * it; gcc builds it once into r6, pays a wider push and pop, and copies.
 *
 * THE BASIC-BLOCK LEVER SHOULD REACH THIS AND DOES NOT, which is the useful
 * part. The lever works when each occurrence is its own pseudo, set once and
 * used once, spanning more than one basic block -- the two conditions in
 * `update_equiv_regs` (see docs/elevation.md). Two separate locals assigned
 * before the `if` satisfies that on paper:
 *
 *     f1 = 0x80 << 2;  f2 = 0x80 << 2;
 *     if (!__GetFlag(f1)) { ...; __SetFlag(f2); }
 *
 * and it is 7 of 18, exactly as the literal is.
 *
 * THE DIFFERENCE FROM THE CASES WHERE IT WORKS is where the uses are.
 * OvlFunc_892_2008054 and OvlFunc_959_2008ce0 both have ALL the repeated uses
 * inside a conditional block, with the assignments in the block above. Here one
 * use -- the __GetFlag in the `if` condition -- is in the SAME BLOCK as the
 * assignments. CSE merges f1 and f2 into one pseudo before local-alloc runs,
 * and that pseudo is then referenced three times, so REG_N_REFS == 2 fails and
 * nothing is rematerialised.
 *
 * So the rule needs a third clause: EVERY repeated use must be in a different
 * basic block from the assignment. One use in the assignment's own block
 * anchors the merged pseudo there and defeats it.
 *
 * TRIED: two separate locals (7), one local for the second use only with the
 * first left as a literal (7), the literal at both sites (7, this file).
 */
extern unsigned int iwram_3001ee0;
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void *__MapActor_GetActor(int slot);

void OvlFunc_936_20095b4(void)
{
    unsigned char *p;
    void *a;

    if (!__GetFlag(0x80 << 2)) {
        p = (unsigned char *)iwram_3001ee0;
        a = __MapActor_GetActor(0);
        *(void **)(p + 0x18) = a;
        __SetFlag(0x80 << 2);
    }
}
