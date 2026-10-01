/* Func_8015e8c -- PIN-FREE BODY, 2 of 23 encodings.  Pin count 0.
 * Read the full header in p3_candidate.c; this file is the alternative body.
 *
 * FIGURE: 2 of 23 (ours 23), 52 bytes against 52, relocations identical.
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     scratch_elev/b317/B/p3_candidate_pinfree.c \
 *     asm/rom_15000/rom_15e8c_a_a.s --func Func_8015e8c
 *
 * THE RESIDUE IS TWO ADJACENT INSNS SWAPPED BY sched2 AND NOTHING ELSE:
 *     rom    mov r3, #0  /  str r2, [r1]  /  str r3, [r0]
 *     ours   str r2, [r1]  /  mov r3, #0  /  str r3, [r0]
 *
 * WHY THE DUPLICATED STORE IS HERE AND MUST NOT BE "TIDIED AWAY":
 * `*head = next` appears in both arms of the inner if on purpose. flow1 counts
 * REG_N_REFS before cross-jumping runs, so the duplicate raises `next`'s
 * reference count from 3 to 4; that lifts its allocno_compare priority above
 * `head`'s, so next takes r2 and head is pushed to r1 -- the ROM's assignment.
 * Cross-jumping (after reload) then merges the two copies back out, so the
 * emitted instruction stream is unchanged by the duplication. Collapsing the
 * two arms into one statement puts the figure back to 7.
 */
struct Node { struct Node *next; };

struct Pool {
    unsigned char pad_000[0xd98];
    struct Node *head;
    struct Node **tail;
};

extern struct Pool *iwram_3001e8c;

struct Node *Func_8015e8c(void)
{
    struct Pool *q = iwram_3001e8c;
    struct Node **head = &q->head;
    struct Node *node = *head;
    struct Node *next;

    if (node != 0) {
        next = node->next;
        if (next == 0) {
            q->tail = head;
            *head = next;
        } else {
            *head = next;
        }
        node->next = 0;
    }
    return node;
}
