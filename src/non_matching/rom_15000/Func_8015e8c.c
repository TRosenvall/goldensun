/* Func_8015e8c @ 0x08015e8c -- asm/rom_15000/rom_15e8c_a_a.s
 *
 * REGISTER ALLOCATION ONLY. Every instruction is right; r1 and r2 are swapped
 * throughout:
 *
 *     rom    ldr r2, =0xd98 / add r1, r3, r2 / ldr r0, [r1]
 *     ours   ldr r1, =0xd98 / add r2, r3, r1 / ldr r0, [r2]
 *
 * gcc-2.96 allocates in birth order against REG_ALLOC_ORDER {3, 2, 1, 0, ...},
 * so whichever pseudo is created first takes r3, then r2, then r1. In the ROM
 * the 0xd98 constant is born before the head pointer; in every formulation
 * tried here the head pointer is born first, and that one difference flips the
 * whole function.
 *
 * Tried and measured: the offsets as struct members; as pointer arithmetic on
 * a u8*; with the constant written on the left of the addition; via a named
 * u32 offset variable; and with an early return instead of a nested if (that
 * one costs two extra instructions). All produce the same swap.
 *
 * This is the same class of problem as Func_808ed4c, which DID resolve -- there,
 * spelling the scaled index as (index << 3) rather than index * 8 changed the
 * birth order. Something equivalent probably exists here.
 */
#include "gba/types.h"

extern u8 *iwram_3001e8c;

struct Node {
    struct Node *next;
};

/* Pops the head off the free list of display nodes, or returns NULL when it is
 * exhausted. Popping the last node moves the tail cache at +0xD9C back to the
 * head slot at +0xD98. The popped node's link word is cleared.
 */
struct Node *Func_8015e8c(void)
{
    struct Node **head = (struct Node **)(iwram_3001e8c + 0xd98);
    struct Node *node = *head;

    if (node != NULL) {
        struct Node *next = node->next;

        if (next == NULL)
            *(struct Node ***)(iwram_3001e8c + 0xd9c) = head;
        *head = next;
        node->next = NULL;
    }
    return node;
}
