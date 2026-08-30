/* Cluster Func_801b148..Func_801b148 extracted from
 * goldensun/asm/rom_15000/rom_1aeec_a_a_a.s.
 *
 * Teardown: close the box, walk two linked lists silencing every node that has
 * a live handle, then free the block.  Matched on the second screen, and the
 * one thing that had to be corrected is worth stating on its own.
 *
 * THE ADDRESSING MODE NAMES THE SIGNEDNESS.  The node flag at +0xa is read
 * `ldrh r3, [r5, #0xa]` -- unsigned, IMMEDIATE offset.  Declared `short` it
 * comes out `mov r2, #0xa / ldrsh r3, [r5, r2]`, because thumb has no ldrsh
 * with an immediate and a signed field must therefore build its offset in a
 * register.  So on a halfword read:
 *
 *      ldrh  rD, [rB, #imm]   -> the field is UNSIGNED
 *      ldrsh rD, [rB, rO]     -> the field is SIGNED
 *
 * and the register-offset form is not a scheduling accident, it is the only
 * encoding available.  One character of the struct, 58 differing to zero.
 * The base's own field at +0x12 really is signed and keeps its `ldrsh`.
 *
 * The two list walks are written out TWICE rather than factored, because the
 * ROM inlines both, and each carries its own `z = 0` for the halfword store --
 * a named zero, which the ROM confirms by pushing r7 for it.  The store crosses
 * the Func_8003f3c call inside the loop, which is what earns it the register.
 *
 * The large field offsets are left as plain expressions: gcc builds 0x348,
 * 0x34c and 0x350 as `mov #0xd2 / lsl #2` and friends because they factor as
 * imm8 << 2, and computes an address for +0x40 because 64 is past the ldrh
 * immediate's reach of 62.  Both are the compiler's choice, not a spelling.
 */
struct Node {
    unsigned char pad00[4];
    struct Node *next;
    unsigned char pad08[2];
    unsigned short fa;
    unsigned short fc;
};

extern unsigned char *iwram_3001e98;

extern void Func_801a97c(void);
extern void CloseUIBox(int box, int n);
extern void WaitFrames(int n);
extern void Func_8003f3c(int id);
extern void Func_801c21c(void);
extern void gfree(int tag);

void Func_801b148(void)
{
    unsigned char *p;
    struct Node *q;
    int z;

    p = iwram_3001e98;
    Func_801a97c();
    CloseUIBox(*(int *)(p + (0xd4 << 2)), 2);
    WaitFrames(1);
    q = *(struct Node **)(p + (0xd2 << 2));
    if (q != 0) {
        z = 0;
        do {
            if (q->fa != 0) {
                Func_8003f3c(q->fc);
                q->fa = z;
            }
            q = q->next;
        } while (q != 0);
    }
    q = *(struct Node **)(p + (0xd3 << 2));
    if (q != 0) {
        z = 0;
        do {
            if (q->fa != 0) {
                Func_8003f3c(q->fc);
                q->fa = z;
            }
            q = q->next;
        } while (q != 0);
    }
    Func_801c21c();
    if (*(short *)(p + 0x12) != 0) {
        Func_8003f3c(*(unsigned short *)(p + 0xc));
        if (*(short *)(p + 0x12) != 0)
            Func_8003f3c(*(unsigned short *)(p + 0x40));
    }
    Func_8003f3c(*(unsigned short *)(p + (0xb9 << 2)));
    gfree(0x12);
}
