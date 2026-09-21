/* Cluster Func_80b0694..Func_80b0694 extracted from goldensun/asm/rom_b0000/rom_b0070_a_a_c_a_c_c_c_a_a.s.
 *
 * Total .text for this TU = 44 bytes (= 0x2c). Parked in an earlier batch; elevated in
 * batch 277. No pins, no flags, no `volatile`.
 *
 * IDENTICAL TWIN OF Func_801b36c (src/rom_15000/rom_1aeec_a_a_c_a_c_a_b.c) -- read that
 * file for the derivation. In short: this park concluded that two reads of one field with
 * nothing between them must be CSE-folded and that `volatile` was the only cure, i.e. a
 * fakematch. Wrong. `duplicate_loop_exit_test` runs at pass 08, AFTER cse1 and gcse, so
 * writing `while (i != b->n)` produces the second load for free. Exact, no scaffolding.
 *
 * Each of the two sources was verified by objcmp against its OWN reference rather than
 * inferred from the other. If you edit one, edit both.
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

struct Node *Func_80b0694(struct B *b)
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
