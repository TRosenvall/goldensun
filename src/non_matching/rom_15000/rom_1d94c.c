/* Func_801d94c -- NOT MATCHING. 5 instructions in disagreeing regions, of 17.
 *
 * Source asm: goldensun/asm/rom_15000/rom_1ca1c_c_c_a.s
 *
 * Blocker classes: pool-loads-first AND indexed load, both in straight-line
 * code, so both are the unreachable variety.
 *
 * 1. THE CONSTANT IS LOADED BEFORE THE POINTER IS DEREFERENCED:
 *
 *        rom    ldr r3, =iwram_3001ea0 / ldr r2, =0x5a4 / ldr r5, [r3] / add r0, r5, r2
 *        ours   ldr r3, =iwram_3001ea0 / ldr r5, [r3]   / ldr r3, =0x5a4 / add r0, r5, r3
 *
 *    Two independent loads in the opposite order. This is the pool-loads-first
 *    shape, and batch 42's reading of update_equiv_regs in local-alloc.c says
 *    it is only reachable when the assignment can sit in a basic block that
 *    holds none of the uses. This function has no branches at all, so
 *    REG_BASIC_BLOCK (regno) < 0 cannot hold. Assigning `off = 0x5a4;` before
 *    the pointer load -- the obvious source-level fix -- changes nothing.
 *
 * 2. THE HALFWORD READ IS AN ADD PLUS A PLAIN LOAD, NOT AN INDEXED LOAD:
 *
 *        rom    add r3, r5, r2 / ldrh r3, [r3, #0x0]
 *        ours   ldrh r3, [r5, r2]
 *
 *    The documented lever for this is to name the intermediate pointer, which
 *    is what `p = base + off; idx = *(unsigned short *)p;` below does. gcc
 *    folds it straight back into the indexed form, because `p` has exactly one
 *    use. This is the load twin of the indexed-store family in
 *    src/non_matching/overlays/, and it fails for the same reason: the
 *    construct that produces the ROM's form needs the pointer live across
 *    something, and there is nothing here to hold it.
 *
 * The offset variable IS correctly reused -- `off = 0x574` then `off += 0x9c`
 * reproduces the ROM's `ldr r2, =0x574 ... add r2, #0x9c` -- so that part of
 * the shape is right and is not what fails.
 *
 * NEXT: nothing to try until one of the two classes is retired. Both are on
 * the straight-line side of the basic-block lever.
 */
extern unsigned char *iwram_3001ea0;
extern void _Func_80b08b8(unsigned char *p);
extern void Func_80217a4(void *p);

void Func_801d94c(void)
{
    unsigned char *base;
    unsigned char *p;
    unsigned int off;
    unsigned int idx;

    off = 0x5a4;
    base = iwram_3001ea0;
    _Func_80b08b8(base + off);
    off = 0x574;
    p = base + off;
    idx = *(unsigned short *)p;
    off += 0x9c;
    Func_80217a4(*(void **)(base + ((idx << 2) + off)));
}
