/* Func_80a3354 (0x080a3354) -- NON-MATCHING.
 * Blocker class: address-birth placement in the loop setup.
 *
 * 54 lines against the ROM's 56, 27 differing, with the first eleven exact and
 * the whole loop body, the CreateUIBox call and its stack argument correct.
 *
 * The divergence is where the loop's base address is computed:
 *
 *     rom    mov r0,#0xa5 / lsl r0,#1 / ldr r1,=0x1e / mov r2,#3 / add r3,r7,r0
 *     ours   mov r1,#0xa5 / lsl r1,#1 / add r3,r7,r1 / ldr r1,=0x1e / mov r2,#3
 *
 * The ROM builds the offset, sets up the loop's stored value and counter, and
 * only THEN forms the address. Ours forms it immediately, which also costs the
 * register the ROM keeps the offset in.
 *
 * MEASURED, all 54 lines and 27 differing:
 *   the pointer assigned after `i = 3;`
 *   the offset named in its own local and the pointer assigned after `i = 3;`
 *
 * So the address-local birth-statement lever does NOT reach this one either.
 * That is now two negatives against it (see src/non_matching/ovl_797990/
 * 2008c1c.c for the first); the lever's demonstrated scope remains a pointer
 * COMPUTED from a base in straight-line code, not one feeding a loop.
 *
 * WHAT IS RIGHT: the pooled 0x1e. The ROM loads it from a mid-function pool
 * (`ldr r1, .La33b0 @ 0x1e`) for a halfword store while using `mov r2, #0x1e`
 * for the same value as a call argument a few instructions later -- the
 * halfword exception running in the ROM-pools direction, so the plain literal
 * is correct at both sites and naming either would be the error. The
 * decrement-first loop, the shared `2` between the stack argument and the last
 * byte store, and the derived 0x111/0x113 offset pair are all exact.
 *
 * NEXT: the two-line deficit is not yet attributed; the mid-function pool
 * makes tryc's line count unreliable here, so this wants a build-gated check
 * rather than more screening.
 */
extern unsigned char *iwram_3001f2c;
extern void *Func_80a1814(void *g);
extern void Func_80a1870(void *q, int a, int b, int c, int d);
extern void *_CreateUIBox(int a, int b, int c, int d, int e);

void Func_80a3354(void)
{
    unsigned char *g;
    void *q;
    short *p;
    int i;
    int z;
    int two;
    int off;

    g = iwram_3001f2c;
    q = Func_80a1814(g);
    Func_80a1870(q, 2, 2, 8, 0);
    p = (short *)(g + (0xa5 << 1));
    i = 3;
    do {
        i--;
        *p = 0x1e;
        p--;
    } while (i >= 0);
    z = 0;
    *(int *)(g + 0x28) = z;
    *(int *)(g + 0x24) = z;
    two = 2;
    *(void **)(g + 0x2c) = _CreateUIBox(0, 0x11, 0x1e, 3, two);
    off = 0x111;
    *(int *)(g + 0x20) = z;
    g[0x88 << 1] = z;
    g[off] = z;
    g[0x89 << 1] = 8;
    off += 2;
    g[off] = two;
}
