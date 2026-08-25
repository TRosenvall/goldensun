/* OvlFunc_965_200a6fc -- NOT MATCHING. 2 of 29 lines differ, same length.
 *
 * Source asm: goldensun/asm/overlays/rom_7ef4f4/ovl_30_c_a_c_a.s
 *
 * Blocker class: branch polarity, inner test of a two-arm range check.
 *
 * An absolute-difference guard: d = other->0xc - self->0xc, and the call
 * happens only when |d| < 0x80000. The ROM tests the two signs in separate
 * blocks, each materialising 0x80 << 12 for itself.
 *
 * EVERYTHING MATCHES EXCEPT WHICH WAY THE FIRST INNER TEST BRANCHES:
 *
 *     rom    cmp r2, r3 / bge <out> / b <call>
 *     ours   cmp r2, r3 / blt <call> / b <out>
 *
 * Two instructions, same count, opposite sense. The ROM tests the RETURN
 * condition and branches away to the epilogue, then jumps unconditionally over
 * the negative arm to the call. gcc consistently emits the complement.
 *
 * WHAT WAS TRIED (all three give the identical 2-line diff):
 *
 *   1. `if (d >= 0) { if (d >= K) return; } else if (a - b >= K) return;`
 *      -- the plain if/else form. Getting the OUTER arms in the ROM's order
 *      was itself a fix: the first attempt had them swapped and stood at 9
 *      lines. The ROM's fall-through is the NON-NEGATIVE arm (`blt` jumps to
 *      the negative one), which is the branch-polarity lever working exactly
 *      as documented -- it just does not reach the inner test.
 *   2. The same with an explicit `goto call;` ending the positive arm, so the
 *      source itself contains the ROM's unconditional forward jump.
 *   3. A fully flat goto chain with a single shared `out:` label and no
 *      `return` anywhere, on the theory that gcc gives return-blocks special
 *      placement and that removing them would stop the inversion.
 *
 * All three are 2 of 29. gcc-2.96 normalises the pair in its jump optimiser
 * after the source shape is gone, so no spelling of the CONDITION reaches it.
 *
 * NOT A LOOP-ROTATION case and not the multiple-exit goto shape -- both of
 * those change the number of branches, and here the count is already right.
 *
 * NEXT: this is the first function parked purely on inner-test polarity, so
 * there is no family yet. If a second turns up, the two together would say
 * whether the trigger is the shared epilogue label (both arms exit to the same
 * place) or something about the arm being empty apart from the return.
 */
extern void *__MapActor_GetActor(int slot);
extern void *OvlFunc_965_200a660(void);
extern void OvlFunc_965_20080c4(void);

void OvlFunc_965_200a6fc(void)
{
    unsigned char *s;
    unsigned char *p;
    int a;
    int b;
    int d;

    s = (unsigned char *)__MapActor_GetActor(0);
    p = (unsigned char *)OvlFunc_965_200a660();
    if (p == 0)
        return;
    b = *(int *)(p + 0xc);
    a = *(int *)(s + 0xc);
    d = b - a;
    if (d >= 0) {
        if (d >= (0x80 << 12))
            return;
    } else if (a - b >= (0x80 << 12)) {
        return;
    }
    OvlFunc_965_20080c4();
}
