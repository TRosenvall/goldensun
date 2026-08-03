/* Cluster OvlFunc_924_200cf90..OvlFunc_924_200cf90 extracted from goldensun/asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c.s.
 *
 * Split out of that .s; the _a and _c parts stay as assembly and keep their
 * slots in goldensun/overlays/rom_7ac2d8/overlay.ld, so the ROM layout does
 * not move.
 *
 * Finds the party member holding an item and writes a halfword into that
 * unit's record at 0xd8 + slot*2. Both lookups return -1 on failure.
 *
 * TWO THINGS ARE LOAD-BEARING HERE.
 *
 * 1. The -1 is compared TWICE, so gcc keeps it in a register and builds it as
 *    `mov r7, #1 / neg r7, r7`. That is the same mov/neg pair recorded as the
 *    `narrow-mask` blocker; here it falls out with no help at all, because the
 *    value is genuinely live across two comparisons. Worth knowing before
 *    spending a round on that blocker elsewhere: the pair is not inherently
 *    hard, it is hard when the value is used ONCE.
 *
 * 2. The byte offset must be a NAMED LOCAL. Written inline --
 *
 *        *(short *)((char *)unit + ((slot << 1) + 0xd8)) = value;
 *
 *    gcc folds the offset into the base pointer and stores with `strh r3,[r0]`,
 *    one instruction longer than the ROM. Naming it keeps the offset in its own
 *    register and produces the ROM's register-offset store `strh r2,[r0,r3]`.
 *    Parenthesising the expression, spelling `slot * 2`, and indexing a
 *    `short *` rebased by 0xd8 all give the folded form.
 *
 * That is the same lever as the mov/lsl case in
 * src/overlays/rom_7e0928/ovl_30_a_c_c_a_c_b.c: naming an intermediate stops
 * gcc from folding it into its consumer.
 */
extern int __CheckPartyItem(int item);
extern int __CheckItem(int party, int item);
extern void *__GetUnit(int party);

void OvlFunc_924_200cf90(int item, int value)
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
