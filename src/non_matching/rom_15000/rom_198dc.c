/* Func_80198dc -- NOT MATCHING. 11 instructions in disagreeing regions, of 18,
 * same length.
 *
 * Source asm: goldensun/asm/rom_15000/rom_1908c_c_a_c.s
 *
 * A hand-written fill loop -- NOT a dma.h helper. `stmia r3!, {r0}` with a
 * single register is a word store with post-increment, which is what
 * `*w++ = 0;` produces. tools/find_solved_shape.py finds no elevated .c
 * emitting that form, so this is the tree's first.
 *
 * The skeleton is right: 18 lines against 18, the same instructions, and the
 * `do { i++; ... } while (i != 8)` shape reproduces the ROM's un-rotated loop.
 * What differs is register assignment and two orderings:
 *
 *   the offset is loaded BEFORE the pointer is dereferenced (`ldr r4, =0x12dc`
 *   ahead of `ldr r3, [r3]`) -- the pool-loads-first shape, in straight-line
 *   code ahead of the loop, so the basic-block lever cannot reach it
 *
 *   inside the loop the ROM does `stmia` THEN `strh`; gcc emits them the other
 *   way round regardless of the order the statements are written in
 *
 * TRIED:
 *   `*w++ = 0;` before `*h = 0; h++;`          11 of 18 (this body)
 *   the two stores swapped                     16 of 18, and 21 lines
 *   the offset assigned before the pointer     17 of 18, and 21 lines
 *
 * Both reorderings make it dramatically worse rather than moving the two
 * instructions, which says gcc is not simply following statement order here --
 * it is scheduling the loop body as a unit.
 *
 * NEXT: the loop-body ordering is the interesting half. If a future batch finds
 * another single-register `stmia` loop, the two together would say whether the
 * store order is reachable at all.
 */
extern unsigned char *iwram_3001e8c;

void Func_80198dc(void)
{
    unsigned char *p;
    unsigned int off;
    int *w;
    short *h;
    int i;

    p = iwram_3001e8c;
    off = 0x12dc;
    h = (short *)(p + off);
    off -= 0x20;
    w = (int *)(p + off);
    i = 0;
    do {
        i++;
        *w++ = 0;
        *h = 0;
        h++;
    } while (i != 8);
}
