/* Func_8019944 (0x08019944) -- NON-MATCHING.
 * Blocker class: a held zero the source cannot force into a register.
 *
 * 38 lines against the ROM's 40, 36 differing. The two missing instructions are
 * a pair the ROM spends keeping a zero alive across the search loop:
 *
 *     rom    mov r1, #0x0 / mov r6, #0x0 / mov r12, r1
 *     ours   mov r6, #0x0 / mov r4, #0x0
 *
 * The ROM materialises zero once, copies it into r12, uses r1 directly for the
 * two stores in the PEELED first iteration, and then uses `mov r3, r12` twice
 * for the two stores in the loop's exit block. Two zeros, one of them parked in
 * a high scratch register for the whole function.
 *
 * MEASURED, both short by two lines at 36 differing:
 *   a natural `for (i = 0; i <= 7; i++)` over the two parallel arrays
 *   the first iteration PEELED explicitly, with a named `z = 0;` used only in
 *     the loop's exit block and plain literals in the peeled block -- which is
 *     what the ROM's split between r1 and r12 reads as. gcc folds `z` back to
 *     an immediate at both stores.
 *
 * This is the same fold recorded for OvlFunc_882_20090a4's loop bound and for
 * Func_80bb588's offsets: naming a compile-time-known value hands gcc a
 * constant to fold, not a variable to keep. Three functions now, and it is
 * worth stating as the general shape -- **a named constant cannot create
 * register pressure**, so a ROM that spends a register on a literal is not
 * asking for a local.
 *
 * WHAT IS RIGHT: the two parallel arrays at blk+0x12dc (halfwords, stride 2)
 * and blk+0x12bc (words, stride 4) indexed OFFSET-first so the constant is the
 * addressing base and the block pointer the index, which gives the ROM's
 * `ldrh r3, [r4, r2]`; the unsigned `i > 7` bound; and the flag-gated clear of
 * both fields with the found word returned either way.
 *
 * NEXT: nothing source-level.
 */
extern int iwram_3001e8c;

int Func_8019944(int key, int flag)
{
    int blk;
    int o1;
    int o2;
    int i;
    int ret;
    int z;

    blk = iwram_3001e8c;
    o1 = 0x12dc;
    i = 0;
    ret = 0;
    z = 0;
    o2 = 0x12bc;
    if (*(unsigned short *)(o1 + blk) == key) {
        ret = *(int *)(o2 + blk);
        if (flag != 0) {
            *(int *)(o2 + blk) = 0;
            *(unsigned short *)(o1 + blk) = 0;
        }
        return ret;
    }
    do {
        i++;
        o2 += 4;
        o1 += 2;
        if (i > 7)
            return ret;
    } while (*(unsigned short *)(o1 + blk) != key);
    ret = *(int *)(o2 + blk);
    if (flag != 0) {
        *(int *)(o2 + blk) = z;
        *(unsigned short *)(o1 + blk) = z;
    }
    return ret;
}
