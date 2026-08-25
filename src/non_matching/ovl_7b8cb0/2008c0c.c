/* OvlFunc_931_2008c0c -- NOT MATCHING. 1 of 24 lines, same length.
 *
 * Source asm: goldensun/asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c.s
 *
 * TWIN: OvlFunc_932_200aa10 in asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a.s
 * is instruction-for-instruction identical, so this park covers two functions
 * and solving it elevates both. Found in batch 53 while screening candidates;
 * not screened separately, because the bodies are the same.
 *
 * Blocker class: constant materialisation width. ONE instruction differs.
 *
 *     rom    mov r3, #0x0 ... sub r3, #0xd      (r3 = 0xfffffff3)
 *     ours   mov r3, #0xf3                      (r3 = 0x000000f3)
 *
 * The mask is ~0xc. The ROM builds the FULL 32-BIT value by subtracting from
 * the zero it already had in r3 -- the same zero it stored to +0x55 four
 * instructions earlier. gcc narrows the mask to 0xf3, which is correct because
 * the result is stored to a byte, and emits a single `mov`.
 *
 * Both are one instruction, so this is not a cost decision gcc got wrong; it is
 * gcc knowing the value is byte-width and the ROM's compiler not knowing, or
 * not caring.
 *
 * WHAT WAS TRIED:
 *
 *   `& -0xd` and `& ~0xc`                        1 (identical output)
 *   the zero named and reused: `v = 0; p[0x55] = v; v -= 0xd;`
 *     -- the genuine reuse lever, and the shape the ROM appears to show --
 *                                                2 (WORSE: it also moves the
 *                                                store at position 3)
 *   an int temporary carrying the mask and the
 *     read: `t = -0xd; t &= s[9]; s[9] = t | 4;` 18 (much worse: 25 lines)
 *
 * The middle one is the interesting failure. The ROM really does derive the
 * mask from the stored zero, so reusing a variable is the faithful reading --
 * and it perturbs an earlier store, which is the same "the diff lands before
 * the statement that caused it" behaviour recorded in batch 49.
 *
 * NEXT: this needs gcc to keep a 32-bit mask alive into a byte store. A cast
 * chain through a wider lvalue is the untried direction -- e.g. giving the
 * flags byte a union or reading it through an `int *` -- but nothing here
 * suggests the original source did anything unusual, and one instruction in 24
 * may simply be a compiler difference.
 */
extern void __Func_80929d8(void *a, int n);
extern void __Actor_SetSpriteFlags(void *a, int f);

void OvlFunc_931_2008c0c(void *a)
{
    unsigned char *p;
    unsigned char *s;

    p = (unsigned char *)a;
    p[0x55] = 0;
    s = *(unsigned char **)(p + 0x50);
    s[9] = (s[9] & -0xd) | 4;
    __Func_80929d8(a, 3);
    __Actor_SetSpriteFlags(a, 0);
    *(int *)(p + 0x18) = 0x4ccc;
    *(int *)(p + 0x1c) = 0x4ccc;
}
