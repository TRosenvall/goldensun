/* Cluster Func_801b36c..Func_801b36c extracted from goldensun/asm/rom_15000/rom_1aeec_a_a_c_a_c_a.s.
 *
 * Total .text for this TU = 44 bytes (= 0x2c). Parked in an earlier batch; elevated in
 * batch 277. No pins, no flags, and NO `volatile` -- which is the point of this file.
 *
 * IDENTICAL TWIN: Func_80b0694, at src/rom_b0000/rom_b0070_a_a_c_a_c_c_c_a_a_b.c. The two
 * streams are identical across all 22 normalised lines; one source with the name changed
 * matches both, and each was verified by objcmp against its OWN reference. Found with
 * tools/dupfuncs.py. If you edit one, edit both.
 *
 * `while (i != x)` GIVES TWO LOADS OF ONE FIELD, AND THAT RETIRES A BLOCKER CLASS.
 *
 * The park's entire diagnosis was that the ROM's two reads of `b->n` with nothing between
 * them are the same expression, that CSE folds them wherever they are written, and that
 * therefore "`volatile` would do it and is a fakematch". That is measurably false.
 *
 * `loop.c` calls `duplicate_loop_exit_test`, which COPIES the exit test to the loop front.
 * It runs at pass 08 -- AFTER cse1 and gcse -- so the copy is never available to be
 * folded, and LICM then hoists the copy into the preheader. That is exactly the ROM's
 * `add r3, r0, r4 / ldrh r0, [r3]`.
 *
 * Measured: the park's guard-plus-`do/while` is 21 lines against 22 and 16 differing; the
 * plain `while` is exact; `for (i = 0; i != b->n; i++)` is exact too.
 *
 * GENERAL FORM: when the ROM re-reads a field once for a zero test and again as a hoisted
 * loop bound, write the `while` rather than the guarded `do/while`. Two reads of one field
 * in a loop condition are NOT necessarily a CSE problem, and reaching for `volatile` there
 * costs a fakematch row for nothing.
 */
struct Node {
    int f0;
    struct Node *next;
};

struct B {
    unsigned char pad00[0x348];
    struct Node *head;
    unsigned char pad34c[0x52];
    unsigned short n;
};

struct Node *Func_801b36c(struct B *b)
{
    struct Node *q;
    int i;

    q = b->head;
    i = 0;
    while (i != b->n) {
        i++;
        q = q->next;
    }
    return q;
}
