// fakematch
/*
 * ClearCallbackTable -- asm/rom_15000/rom_1908c_c_a_c_c_b.s
 *
 * fakematch: `off` is pinned to r4.  It has to be, and the reason is exact:
 * gcc-2.96's REG_ALLOC_ORDER for ARM is { 3, 2, 1, 0, 12, 14, 4, ... }, and
 * local-alloc runs BEFORE global-alloc, so any pseudo local to one basic block
 * takes r3/r2 ahead of every loop-carried value.  The ROM's allocation is
 * b->r3, q->r2, i->r1, z->r0, off->r4, i.e. off is allocated LAST -- which only
 * happens if it is a GLOBAL allocno (flow.c marks a pseudo global when it is
 * live at a block end or referenced in two blocks).  Nothing in C makes `off`
 * global here without also emitting an instruction, so the register is named.
 *
 * NOT fake, and load-bearing: the union.  Reaching the halfword through
 * `union CbSlot` gives that store the UNION's alias set, which conflicts with
 * the `int` word store (record_component_aliases makes every member type a
 * subset), restoring the memory dependence sched2 needs to keep the two stores
 * in ROM order.  Dropping it alone costs 2 encodings (the stores swap).
 * The union owns no pointer arithmetic -- `q` stays `short *` and the cast is
 * at the use site -- because STRUCTURE_SIZE_BOUNDARY would make sizeof 4.
 */
extern unsigned char *iwram_3001e8c;

union CbSlot { int w; short h; };

void Func_80198dc(void)
{
    unsigned char *b;
    short *q;
    register int off __asm__("r4");
    int i;
    int z;

    off = 0x12dc;
    b = iwram_3001e8c;
    q = (short *)(b + off);
    off -= 0x20;
    i = 0;
    z = 0;
    b += off;
    do {
        i++;
        *(int *)b = z;
        b += 4;
        ((union CbSlot *)q)->h = z;
        q++;
    } while (i != 8);
}
