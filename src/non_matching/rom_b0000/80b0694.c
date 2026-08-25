/* Func_80b0694  --  0x080b0694, asm/rom_b0000/rom_b0070_a_a_c_a_c_c_c.s
 *
 * BLOCKER CLASS: a field read TWICE that gcc reads once.
 * Status: 21 lines against the ROM's 22.
 *
 * WHAT IT DOES
 * Walks the list at +0x348 forward by the count held at +0x39e and returns the
 * node it lands on.
 *
 * THE DIFFERENCE. The ROM loads the count halfword twice -- once to test it
 * against zero, once as the loop bound:
 *
 *      add r3, r0, r4 / ldrh r3, [r3]   ... cmp r3, #0 / beq
 *      add r3, r0, r4 / ldrh r0, [r3]   ... cmp r1, r0 / bne
 *
 * gcc keeps the first value and re-uses it (`mov r0, r3`), which is one
 * instruction shorter, and destroys the base pointer doing it.
 *
 * WHY THE OBVIOUS FIX DOES NOT APPLY. Func_801c46c in this same batch loads the
 * same POOL CONSTANT twice and matched, because writing the offset as a struct
 * member lets gcc materialise the address independently at each access. That
 * works for an address; it does not work for a VALUE. Two reads of one member
 * with nothing between them that could change it are the same expression, and
 * CSE folds them wherever they are written.
 *
 * The only thing that separates them is something gcc must treat as a possible
 * write to that halfword, and there is none: the loop body only follows
 * pointers. `volatile` would do it and is a fakematch.
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
    if (b->n != 0) {
        do {
            i++;
            q = q->next;
        } while (i != b->n);
    }
    return q;
}
